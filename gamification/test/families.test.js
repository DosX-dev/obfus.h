import test from 'node:test';
import assert from 'node:assert/strict';
import { familyStage } from '../src/generation/family-model.js';
import { generate, fingerprint } from '../src/generation/generator.js';
import { transform } from '../src/generation/model.js';
import { analyzeDifficulty, assertDifficulty } from '../src/generation/difficulty.js';
import { emitFamily } from '../src/generation/family-emitter.js';

const rotate = (x, r, bits) => Number(((BigInt(x) << BigInt(r)) | (BigInt(x) >> BigInt(bits - r))) & ((1n << BigInt(bits)) - 1n));
test('family construction changes wiring, word mixes, predicate circuits and code representation', () => {
    const layouts = new Set(), conditions = new Set(), flows = new Set();
    for (let round = 1; round <= 20; round++) {
        const c = generate(Buffer.alloc(32, 65), { date: '2026-10-12', round, level: 'medium' });
        for (const stage of c.ir.stages) {
            for (const box of stage.boxes || []) layouts.add(box.layout);
            for (const op of stage.kind === 'controlled' ? stage.ops : []) conditions.add(`${op.inverted}/${op.branchless}`);
            if (stage.program) {
                flows.add(stage.program.map(step => step.box !== undefined ? 'B' : 'P').join(''));
                assert.equal(stage.program.length, stage.boxes.length + stage.ops.length);
                assert.equal(new Set(stage.program.filter(step => step.box !== undefined).map(step => step.box)).size, stage.boxes.length);
            }
        }
    }
    assert.equal(layouts.size, 3);
    assert.equal(conditions.size, 4);
    assert(flows.size >= 15);
    const c = generate(Buffer.alloc(32, 65), { date: '2026-10-12', level: 'medium' });
    const stage = c.ir.stages.find(item => item.boxes), same = structuredClone(stage);
    for (const box of same.boxes) box.layout = (box.layout + 1) % 3;
    const input = Array(c.ir.width).fill(0x5678);
    assert.deepEqual(familyStage(input, stage, 16, false), familyStage(input, same, 16, false));
    assert.notEqual(emitFamily(stage, 16, i => i), emitFamily(same, 16, i => i));
});
test('new family admission cannot silently downgrade a difficulty', () => {
    const key = Buffer.alloc(32, 31);
    for (const [level, removed] of [['easy', 'substitute'], ['medium', 'spn'], ['medium', 'controlled']]) {
        const c = generate(key, { date: '2026-10-12', level });
        c.ir.stages = c.ir.stages.filter(stage => stage.kind !== removed);
        assert.throws(() => assertDifficulty(c.ir), /difficulty structure contract/);
    }
    const c = generate(key, { date: '2026-10-12', level: 'hard' });
    for (const stage of c.ir.stages) stage.round = false;
    assert.throws(() => assertDifficulty(c.ir), /difficulty structure contract/);
});
test('all pair forms are bijective over the complete byte-pair domain and match BigInt formulas', () => {
    for (let form = 0; form < 3; form++) {
        const op = { i: 0, j: 1, form, k: 15, m: 14, rotation: 7 }, stage = { kind: 'matrix', ops: [op] }, seen = new Set();
        for (let a = 0; a < 256; a++) for (let b = 0; b < 256; b++) {
            let u, v;
            if (form === 0) { u = Number((BigInt(a) + 15n * BigInt(b)) & 255n); v = Number((BigInt(b) + 14n * BigInt(u)) & 255n); }
            if (form === 1) { u = rotate(a ^ b, 7, 8); v = (b + u) % 256; }
            if (form === 2) { u = Number((BigInt(a) * BigInt(1 + 2 * (b % 8))) & 255n); v = b ^ rotate(u, 7, 8); }
            const output = familyStage([a, b], stage, 8, false);
            assert.deepEqual(output, [u, v]);
            assert.deepEqual(familyStage(output, stage, 8, true), [a, b]);
            seen.add(output[0] * 256 + output[1]);
        }
        assert.equal(seen.size, 65536);
    }
});
test('nonlinear nibble substitutions cover every 16-bit word exactly once', () => {
    const c = generate(Buffer.alloc(32, 31), { date: '2026-10-12', level: 'medium' });
    const box = c.ir.stages.find(s => s.boxes).boxes[0], stage = { kind: 'substitute', boxes: [{ tables: box.tables, i: 0 }] }, seen = new Set();
    for (const table of box.tables) assert.equal(new Set(table).size, 16);
    for (let x = 0; x < 65536; x++) {
        const expected = box.tables.reduce((value, table, n) => value + table[Math.floor(x / 16 ** n) % 16] * 16 ** n, 0);
        const output = familyStage([x], stage, 16, false);
        assert.equal(output[0], expected);
        assert.deepEqual(familyStage(output, stage, 16, true), [x]);
        seen.add(output[0]);
    }
    assert.equal(seen.size, 65536);
});
test('controlled permutations reverse correctly even when later operations overwrite earlier controls', () => {
    const stage = { kind: 'controlled', ops: [{ i: 0, j: 1, control: 2, bit: 0 }, { i: 1, j: 2, control: 0, bit: 1 }, { i: 0, j: 2, control: 1, bit: 2 }] };
    for (let a = 0; a < 16; a++) for (let b = 0; b < 16; b++) for (let c = 0; c < 16; c++) {
        const input = [a, b, c];
        assert.deepEqual(familyStage(familyStage(input, stage, 8, false), stage, 8, true), input);
    }
});
test('generated new families retain numeric bounds, both dependency topologies and every stage trace', () => {
    const seen = new Set();
    for (const level of ['easy', 'medium', 'hard']) for (let round = 1; round <= 30; round++) {
        const c = generate(Buffer.alloc(32, 33), { date: '2026-10-12', level, round });
        const summary = analyzeDifficulty(c.ir);
        if (level === 'easy') assert.equal(summary.minimumInputDependencies, 1);
        if (level === 'hard') { assert(summary.feistelRounds >= 24); assert(summary.sequentialRounds >= 12); }
        let state = Array.from({ length: c.ir.width }, (_, i) => i % 2 ? 65535 & (2 ** c.ir.bits - 1) : 0);
        for (const stage of c.ir.stages) {
            seen.add(stage.kind);
            for (const box of stage.boxes || []) for (const table of box.tables) {
                assert.equal(new Set(table).size, 16);
                assert(table.some((value, x) => {
                    let affine = table[0];
                    for (let bit = 0; bit < 4; bit++) if (x & (1 << bit)) affine ^= table[1 << bit] ^ table[0];
                    return affine !== value;
                }));
            }
            for (const op of stage.ops || []) {
                if (op.control !== undefined) assert.equal(new Set([op.i, op.j, op.control]).size, 3);
                if (op.k !== undefined) { assert(op.k >= 1 && op.k <= 15); assert(op.m >= 1 && op.m <= 15); assert(op.rotation > 0 && op.rotation < c.ir.bits); }
            }
            const one = { ...c.ir, stages: [stage] }, output = transform(one, state);
            assert(output.every(value => Number.isInteger(value) && value >= 0 && value < 2 ** c.ir.bits));
            assert.deepEqual(transform(one, output, true), state);
            state = output;
        }
        const changed = structuredClone(c.ir);
        const swap = changed.stages.find(s => s.kind === 'controlled');
        if (swap) { swap.ops[0].control = (swap.ops[0].control + 1) % c.ir.width; assert.notEqual(fingerprint(changed), c.fingerprint); }
    }
    for (const kind of ['substitute', 'matrix', 'spn', 'controlled']) assert(seen.has(kind));
});
