// One RCDATA PNG resource in a relocatable COFF object. No resource compiler needed.
export function logoResource(png, arch) {
    if (!png.subarray(0, 8).equals(Buffer.from('89504e470d0a1a0a', 'hex'))) throw new Error('Invalid PNG logo');
    const raw = Buffer.alloc(88 + png.length);
    for (const offset of [0, 24, 48]) raw.writeUInt16LE(1, offset + 14);
    raw.writeUInt32LE(10, 16);
    raw.writeUInt32LE(0x80000018, 20);
    raw.writeUInt32LE(101, 40);
    raw.writeUInt32LE(0x80000030, 44);
    raw.writeUInt32LE(0x409, 64);
    raw.writeUInt32LE(72, 68);
    raw.writeUInt32LE(88, 72);
    raw.writeUInt32LE(png.length, 76);
    png.copy(raw, 88);
    const head = Buffer.alloc(60);
    head.writeUInt16LE(arch === 'x64' ? 0x8664 : 0x14c);
    head.writeUInt16LE(1, 2);
    head.writeUInt32LE(60 + raw.length + 10, 8);
    head.writeUInt32LE(1, 12);
    head.write('.rsrc', 20);
    head.writeUInt32LE(raw.length, 36);
    head.writeUInt32LE(60, 40);
    head.writeUInt32LE(60 + raw.length, 44);
    head.writeUInt16LE(1, 52);
    head.writeUInt32LE(0x40000040, 56);
    const reloc = Buffer.alloc(10);
    reloc.writeUInt32LE(72);
    reloc.writeUInt16LE(arch === 'x64' ? 3 : 7, 8);
    const symbol = Buffer.alloc(18);
    symbol.write('.rsrc');
    symbol.writeInt16LE(1, 12);
    symbol[16] = 3;
    return Buffer.concat([head, raw, reloc, symbol, Buffer.from([4, 0, 0, 0])]);
}
