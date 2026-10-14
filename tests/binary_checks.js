'use strict';
const assert = require('node:assert/strict');

function pe(binary) {
    const nt = binary.readUInt32LE(0x3c),
        optional = nt + 24;
    const count = binary.readUInt16LE(nt + 6),
        optionalSize = binary.readUInt16LE(nt + 20);
    const sections = [];
    for (let i = 0; i < count; i++) {
        const offset = optional + optionalSize + i * 40;
        sections.push({
            executable: !!(binary.readUInt32LE(offset + 36) & 0x20000000),
            name: binary
                .subarray(offset, offset + 8)
                .toString()
                .replace(/\0.*$/, ''),
            virtualAddress: binary.readUInt32LE(offset + 12),
            rawOffset: binary.readUInt32LE(offset + 20),
            rawSize: binary.readUInt32LE(offset + 16),
            bytes: binary.subarray(
                binary.readUInt32LE(offset + 20),
                binary.readUInt32LE(offset + 20) + binary.readUInt32LE(offset + 16)
            )
        });
    }
    return { machine: binary.readUInt16LE(nt + 4), sections };
}
function importModules(binary) {
    const { sections } = pe(binary);
    const optional = binary.readUInt32LE(0x3c) + 24;
    const directory = optional + (binary.readUInt16LE(optional) === 0x20b ? 112 : 96);
    const rva = binary.readUInt32LE(directory + 8);
    if (!rva) return [];
    const offset = (address) => {
        const section = sections.find(
            (value) => address >= value.virtualAddress && address - value.virtualAddress < value.rawSize
        );
        assert(section, 'import RVA outside image');
        return section.rawOffset + address - section.virtualAddress;
    };
    const modules = [];
    for (let descriptor = offset(rva); binary.readUInt32LE(descriptor + 12); descriptor += 20) {
        const name = offset(binary.readUInt32LE(descriptor + 12));
        modules.push(binary.subarray(name, binary.indexOf(0, name)).toString().toLowerCase());
    }
    return modules;
}
function assertNoConstantSignature(binary) {
    for (const signature of [
        Buffer.from('abcdefghijklmnopqrstuvwxyz'),
        Buffer.from('a\0b\0c\0d\0e\0f\0g\0h\0'),
        Buffer.from('SLAIDP'),
        Buffer.from([0, 1, 2, 3, 4, 5, 6, 7, 8, 9])
    ])
        assert(!binary.includes(signature), 'constant data signature survives: ' + signature.toString('hex'));
}
function withoutSkippedPayload(code, branch) {
    assert(branch >= 0 && branch + 6 <= code.length, 'junk skip branch missing');
    const end = branch + 6,
        destination = end + code.readInt32LE(branch + 2);
    assert(destination >= end && destination < code.length, 'junk skip target outside site');
    return { destination, liveBytes: Buffer.concat([code.subarray(0, end), code.subarray(destination)]) };
}
function junkCode(binary, siteCount = 13) {
    const { sections } = pe(binary);
    const offset = (rva) => {
        const section = sections.find((s) => rva >= s.virtualAddress && rva - s.virtualAddress < s.rawSize);
        assert(section, 'export RVA is outside the binary');
        return section.rawOffset + rva - section.virtualAddress;
    };
    const optional = binary.readUInt32LE(0x3c) + 24;
    const exports = offset(binary.readUInt32LE(optional + (binary.readUInt16LE(optional) === 0x20b ? 112 : 96)));
    const count = binary.readUInt32LE(exports + 24),
        functions = offset(binary.readUInt32LE(exports + 28));
    const names = offset(binary.readUInt32LE(exports + 32)),
        ordinals = offset(binary.readUInt32LE(exports + 36));
    const entries = new Map();
    for (let i = 0; i < count; ++i) {
        const start = offset(binary.readUInt32LE(names + i * 4));
        const name = binary.subarray(start, binary.indexOf(0, start)).toString();
        entries.set(name, binary.readUInt32LE(functions + binary.readUInt16LE(ordinals + i * 2) * 4));
    }
    return Array.from({ length: siteCount }, (_, i) => {
        const start = entries.get('junk_site_' + i),
            end = entries.get(i === siteCount - 1 ? 'junk_anchor' : 'junk_site_' + (i + 1));
        assert(start && end > start && end - start < 16384, 'invalid junk function boundaries');
        return binary.subarray(offset(start), offset(end));
    });
}

module.exports = { pe, importModules, assertNoConstantSignature, withoutSkippedPayload, junkCode };
