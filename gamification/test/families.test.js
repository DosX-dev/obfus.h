import test from 'node:test';
import assert from 'node:assert/strict';
import { familyStage } from '../src/generation/family-model.js';
import { generate, fingerprint } from '../src/generation/generator.js';
import { transform } from '../src/generation/model.js';
import { analyzeDifficulty, assertDifficulty } from '../src/generation/difficulty.js';
import { emitFamily } from '../src/generation/family-emitter.js';
import { shapeFingerprint } from '../src/generation/diversity.js';

const rotate = (x, r, bits) => Number(((BigInt(x) << BigInt(r)) | (BigInt(x) >> BigInt(bits - r))) & ((1n << BigInt(bits)) - 1n));

test('word routing mixes are bijective over every byte and 16-bit word', () => {
    for (const bits of [8, 16]) for (const kind of ['shlXor', 'shrXor', 'swapbits']) {
        for (let amount = 1; amount < bits; amount++) {
            if (kind === 'swapbits' && (amount & (amount - 1))) continue;
            const stage = { kind: 'bitshuffle', ops: [{ i: 0, route: Array.from({ length: bits }, (_, i) => i), before: [{ kind, value: amount }] }] };
            const seen = new Uint8Array(2 ** bits);
            for (let value = 0; value < 2 ** bits; value++) {
                const binary = value.toString(2).padStart(bits, '0').split('').reverse().map(Number);
                const expected = binary.reduce((sum, bit, index) => {
                    const partner = kind === 'swapbits' ? index ^ amount : kind === 'shlXor' ? index - amount : index + amount;
                    return sum + (kind === 'swapbits' ? binary[partner] : bit ^ (binary[partner] || 0)) * 2 ** index;
                }, 0);
                const output = familyStage([value], stage, bits, false);
                assert.equal(output[0], expected);
                assert.equal(seen[output[0]], 0);
                seen[output[0]] = 1;
                assert.equal(familyStage(output, stage, bits, true)[0], value);
            }
        }
    }
});

test('all difficulties use the extended word mixes and track their structure', () => {
    for (const level of ['easy', 'medium', 'hardcore', 'extreme']) {
        const seen = new Set();
        for (let round = 1; round <= 12; round++) {
            const c = generate(Buffer.alloc(32, 93), { date: '2026-10-15', round, level });
            const visit = value => {
                if (!value || typeof value !== 'object') return;
                if (['shlXor', 'shrXor', 'swapbits'].includes(value.kind)) seen.add(value.kind);
                for (const child of Object.values(value)) visit(child);
            };
            visit(c.ir.stages.filter(stage => stage.kind !== 'affine'));
            const changed = structuredClone(c.ir);
            const box = changed.stages.flatMap(stage => stage.boxes || []).find(box => box.before?.length);
            box.before[0] = { kind: box.before[0].kind === 'swapbits' ? 'shlXor' : 'swapbits', value: 1 };
            assert.notEqual(fingerprint(changed), fingerprint(c.ir));
            assert.notEqual(shapeFingerprint(changed), shapeFingerprint(c.ir));
        }
        assert.deepEqual([...seen].sort(), ['shlXor', 'shrXor', 'swapbits'].sort(), level);
    }
});
test('composed lifts cover the byte-pair domain and every control operation', () => {
    const kinds = ['xor', 'add', 'sub', 'mul', 'rot'];
    const stage = { kind: 'matrix', ops: [{ i: 0, j: 1, program: kinds.map((kind, i) => ({ target: i % 2, kind, alternate: kinds[(i + 1) % kinds.length], bit: i, sourceMix: [{ kind: 'xor', value: 173 }, { kind: 'rot', value: 3 }] })) }] };
    const seen = new Set();
    for (let a = 0; a < 256; a++) for (let b = 0; b < 256; b++) {
        const output = familyStage([a, b], stage, 8, false);
        const expected = [BigInt(a), BigInt(b)];
        for (const step of stage.ops[0].program) {
            const mixed = expected[1 - step.target] ^ 173n;
            const control = ((mixed << 3n) | (mixed >> 5n)) & 255n;
            const kind = control & (1n << BigInt(step.bit)) ? step.alternate : step.kind;
            let value = expected[step.target];
            if (kind === 'xor') value ^= control;
            if (kind === 'add') value += control;
            if (kind === 'sub') value -= control;
            if (kind === 'mul') value *= 1n + 2n * (control & 7n);
            if (kind === 'rot') {
                const count = 1n + (control & 3n);
                value = (value << count) | (value >> (8n - count));
            }
            expected[step.target] = value & 255n;
        }
        assert.deepEqual(output, expected.map(Number));
        assert.deepEqual(familyStage(output, stage, 8, true), [a, b]);
        seen.add(output[0] * 256 + output[1]);
    }
    assert.equal(seen.size, 65536);
    const broken = structuredClone(stage);
    broken.ops[0].program.pop();
    assert.notDeepEqual(familyStage([73, 191], stage, 8, false), familyStage([73, 191], broken, 8, false));
});
test('lift programs vary algorithms and control directions, not only constants', () => {
    const programs = new Set(), kinds = new Set(), directions = new Set(), lengths = new Set();
    for (let round = 1; round <= 30; round++) {
        const c = generate(Buffer.alloc(32, 61), { date: '2026-10-12', round, level: 'medium' });
        for (const stage of c.ir.stages) for (const op of stage.ops || []) if (op.program) {
            programs.add(op.program.map(step => `${step.target}:${step.kind}/${step.sourceMix.map(item => item.kind)}`).join(';'));
            lengths.add(op.program.length);
            for (const step of op.program) { kinds.add(step.kind); directions.add(step.target); }
            const changed = structuredClone(c.ir), replacement = changed.stages[c.ir.stages.indexOf(stage)].ops[stage.ops.indexOf(op)];
            replacement.program[0].kind = replacement.program[0].kind === 'add' ? 'xor' : 'add';
            assert.notEqual(fingerprint(changed), fingerprint(c.ir));
            assert.notEqual(shapeFingerprint(changed), shapeFingerprint(c.ir));
            const renamed = structuredClone(c.ir), renamedOp = renamed.stages[c.ir.stages.indexOf(stage)].ops[stage.ops.indexOf(op)];
            [renamedOp.i, renamedOp.j] = [renamedOp.j, renamedOp.i];
            for (const step of renamedOp.program) step.target ^= 1;
            assert.equal(shapeFingerprint(renamed), shapeFingerprint(c.ir), 'Local pair renaming must not inflate novelty');
        }
    }
    assert(programs.size > 500);
    assert.equal(kinds.size, 5);
    assert.equal(directions.size, 2);
    assert.equal(lengths.size, 5);
});
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
    const c = generate(key, { date: '2026-10-12', level: 'hardcore' });
    for (const stage of c.ir.stages) stage.round = false;
    assert.throws(() => assertDifficulty(c.ir), /difficulty structure contract/);
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
    for (const level of ['easy', 'medium', 'hardcore']) for (let round = 1; round <= 30; round++) {
        const c = generate(Buffer.alloc(32, 33), { date: '2026-10-12', level, round });
        const summary = analyzeDifficulty(c.ir);
        if (level === 'easy') assert.equal(summary.minimumInputDependencies, 1);
        if (level === 'hardcore') { assert(summary.feistelRounds >= 24); assert(summary.sequentialRounds >= 12); }
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
