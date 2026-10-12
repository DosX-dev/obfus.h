import { deflateRawSync, inflateRawSync } from 'node:zlib';

function crc32(data) {
    let crc = 0xffffffff;
    for (const byte of data) {
        crc ^= byte;
        for (let bit = 0; bit < 8; bit++) crc = (crc >>> 1) ^ (crc & 1 ? 0xedb88320 : 0);
    }
    return (crc ^ 0xffffffff) >>> 0;
}
export function playerZip(name, data) {
    if (!/^[A-Za-z0-9._-]+\.exe$/.test(name) || data.length > 32 * 1024 * 1024) throw new Error('Invalid ZIP player');
    const filename = Buffer.from(name), compressed = deflateRawSync(data, { level: 9 }), crc = crc32(data);
    const local = Buffer.alloc(30);
    local.writeUInt32LE(0x04034b50); local.writeUInt16LE(20, 4); local.writeUInt16LE(8, 8); local.writeUInt16LE(33, 12);
    local.writeUInt32LE(crc, 14); local.writeUInt32LE(compressed.length, 18); local.writeUInt32LE(data.length, 22); local.writeUInt16LE(filename.length, 26);
    const central = Buffer.alloc(46);
    central.writeUInt32LE(0x02014b50); central.writeUInt16LE(20, 4); central.writeUInt16LE(20, 6); central.writeUInt16LE(8, 10); central.writeUInt16LE(33, 14);
    central.writeUInt32LE(crc, 16); central.writeUInt32LE(compressed.length, 20); central.writeUInt32LE(data.length, 24); central.writeUInt16LE(filename.length, 28);
    const end = Buffer.alloc(22), centralSize = central.length + filename.length, centralOffset = local.length + filename.length + compressed.length;
    end.writeUInt32LE(0x06054b50); end.writeUInt16LE(1, 8); end.writeUInt16LE(1, 10); end.writeUInt32LE(centralSize, 12); end.writeUInt32LE(centralOffset, 16);
    return Buffer.concat([local, filename, compressed, central, filename, end]);
}
export function unzipPlayer(zip) {
    if (zip.length < 98 || zip.readUInt32LE(0) !== 0x04034b50 || zip.readUInt16LE(6) !== 0 || zip.readUInt16LE(8) !== 8 || zip.readUInt16LE(28) !== 0) throw new Error('Invalid player ZIP header');
    const nameLength = zip.readUInt16LE(26), size = zip.readUInt32LE(18), rawSize = zip.readUInt32LE(22), start = 30 + nameLength, centralAt = start + size;
    const name = zip.toString('ascii', 30, start);
    if (!/^[A-Za-z0-9._-]+\.exe$/.test(name) || rawSize > 32 * 1024 * 1024 || centralAt + 46 + nameLength + 22 !== zip.length) throw new Error('Invalid player ZIP bounds');
    const central = zip.subarray(centralAt), end = zip.subarray(-22);
    if (central.readUInt32LE(0) !== 0x02014b50 || central.readUInt16LE(8) !== 0 || central.readUInt16LE(10) !== 8 || central.readUInt32LE(16) !== zip.readUInt32LE(14) || central.readUInt32LE(20) !== size || central.readUInt32LE(24) !== rawSize || central.readUInt16LE(28) !== nameLength || central.readUInt16LE(30) !== 0 || central.readUInt16LE(32) !== 0 || central.readUInt32LE(42) !== 0 || central.toString('ascii', 46, 46 + nameLength) !== name || end.readUInt32LE(0) !== 0x06054b50 || end.readUInt16LE(4) !== 0 || end.readUInt16LE(6) !== 0 || end.readUInt16LE(8) !== 1 || end.readUInt16LE(10) !== 1 || end.readUInt32LE(12) !== 46 + nameLength || end.readUInt32LE(16) !== centralAt || end.readUInt16LE(20) !== 0) throw new Error('Invalid player ZIP directory');
    const data = inflateRawSync(zip.subarray(start, centralAt), { maxOutputLength: 32 * 1024 * 1024 });
    if (data.length !== rawSize || crc32(data) !== zip.readUInt32LE(14)) throw new Error('Player ZIP integrity failure');
    return { name, data };
}
