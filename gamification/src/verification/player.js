import fs from 'node:fs/promises';
import path from 'node:path';
import assert from 'node:assert/strict';
import { hash } from '../core/crypto.js';
import { REPOSITORY_ROOT } from '../core/paths.js';

export async function auditPlayer(challenge, exe) {
    const binary = await fs.readFile(exe);
    assert(!binary.includes(Buffer.from(challenge.password)), 'ASCII password leak');
    assert(!binary.includes(Buffer.from(challenge.password, 'utf16le')), 'UTF-16 password leak');
    assert(!binary.includes(Buffer.from('RESULT ')), 'Test protocol leaked into player');
    const pe = binary.readUInt32LE(0x3c),
        opt = pe + 24,
        magic = binary.readUInt16LE(opt);
    assert.equal(binary.readUInt32LE(pe + 12), 0, 'COFF symbol table must be absent');
    assert.equal(binary.readUInt16LE(opt + 68), 2, 'GUI subsystem required');
    const directories = opt + (magic === 0x20b ? 112 : 96);
    assert.equal(binary.readUInt32LE(directories + 48), 0, 'Debug directory must be absent');
    const sections = opt + binary.readUInt16LE(pe + 20), sectionCount = binary.readUInt16LE(pe + 6);
    const offset = rva => {
        for (let i = 0; i < sectionCount; i++) {
            const section = sections + i * 40, start = binary.readUInt32LE(section + 12), size = Math.max(binary.readUInt32LE(section + 8), binary.readUInt32LE(section + 16));
            if (rva >= start && rva < start + size) return binary.readUInt32LE(section + 20) + rva - start;
        }
        throw new Error('Invalid PE directory');
    };
    const exports = binary.readUInt32LE(directories);
    if (exports) {
        const table = offset(exports), count = binary.readUInt32LE(table + 24), names = offset(binary.readUInt32LE(table + 32));
        for (let i = 0; i < count; i++) {
            const start = offset(binary.readUInt32LE(names + i * 4)), end = binary.indexOf(0, start);
            // This intentional library watermark is part of the protected header, not a debug symbol.
            assert.equal(binary.toString('ascii', start, end), 'WhatSoundDoesACowMake', 'Unexpected exported test/debug entry');
        }
    }
    const resourceRva = binary.readUInt32LE(opt + (magic === 0x20b ? 112 : 96) + 16);
    assert(resourceRva !== 0, 'Embedded PNG resource required');
    assert(
        binary.includes(await fs.readFile(path.join(REPOSITORY_ROOT, 'pics/obfus.h.png'))),
        'Repository PNG must be embedded intact'
    );
    return { sha256: hash(binary), size: binary.length };
}
