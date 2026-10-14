// One RCDATA JPEG resource in a relocatable COFF object. No resource compiler needed.
export function logoResource(jpeg, arch) {
    if (!jpeg.subarray(0, 3).equals(Buffer.from('ffd8ff', 'hex')) || !jpeg.subarray(-2).equals(Buffer.from('ffd9', 'hex'))) throw new Error('Invalid JPEG logo');
    const raw = Buffer.alloc(88 + jpeg.length);
    for (const offset of [0, 24, 48]) raw.writeUInt16LE(1, offset + 14);
    raw.writeUInt32LE(10, 16);
    raw.writeUInt32LE(0x80000018, 20);
    raw.writeUInt32LE(101, 40);
    raw.writeUInt32LE(0x80000030, 44);
    raw.writeUInt32LE(0x409, 64);
    raw.writeUInt32LE(72, 68);
    raw.writeUInt32LE(88, 72);
    raw.writeUInt32LE(jpeg.length, 76);
    jpeg.copy(raw, 88);
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
