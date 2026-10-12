import test from 'node:test';
import assert from 'node:assert/strict';
import { stage } from '../src/generation/model.js';
import { generate, fingerprint } from '../src/generation/generator.js';
import { analyzeDifficulty, assertDifficulty } from '../src/generation/difficulty.js';
import { emit } from '../src/generation/emitter.js';

test('shift-XOR bijections match an independent BigInt oracle and invert every byte', () => {
    for (const bits of [8, 16]) for (const kind of ['shlXor', 'shrXor']) for (let count = 1; count < bits; count++) {
        const values = bits === 8 ? Array.from({ length: 256 }, (_, i) => i) : [0, 1, 32767, 32768, 65534, 65535];
        const operation = { kind: 'affine', ops: [{ i: 0, sequence: [{ kind, value: count }] }] };
        for (const value of values) {
            const shifted = kind === 'shlXor' ? BigInt(value) << BigInt(count) : BigInt(value) >> BigInt(count);
            const expected = Number((BigInt(value) ^ shifted) & ((1n << BigInt(bits)) - 1n));
            const changed = stage([value], operation, bits);
            assert.deepEqual(changed, [expected]);
            assert.deepEqual(stage(changed, operation, bits, true), [value]);
        }
    }
});

test('state-dependent multiplication and rotation preserve all control words and boundary values', () => {
    for (const form of [3, 4]) for (const control of [0, 1, 7, 8, 32768, 65535]) for (const value of [0, 1, 32768, 65535]) {
        const input = [value, control];
        const operation = { kind: 'couple', ops: [{ i: 0, form, expr: { kind: 'source', index: 1 } }] };
        const changed = stage(input, operation, 16);
        const expected = form === 3 ? Number(BigInt(value) * BigInt(1 + 2 * (control & 7)) & 65535n)
            : Number(((BigInt(value) << BigInt(1 + (control & 7))) | (BigInt(value) >> BigInt(15 - (control & 7)))) & 65535n);
        assert.deepEqual(changed, [expected, control]);
        assert.deepEqual(stage(changed, operation, 16, true), input);
    }
});

test('conditional round expressions select both paths without losing reversibility', () => {
    const operation = {
        kind: 'couple', ops: [{
            i: 0, form: 1, expr: {
                kind: 'select', selector: 1, value: 1,
                left: { kind: 'source', index: 2 }, right: { kind: 'source', index: 3 }
            }
        }]
    };
    for (const control of [0, 1]) {
        const input = [65530, control, 9, 21];
        const changed = stage(input, operation, 16);
        assert.equal(changed[0], control ? 3 : 15);
        assert.deepEqual(stage(changed, operation, 16, true), input);
    }
});

test('unbalanced generalized Feistel routing is reversed before its updates', () => {
    const operation = {
        kind: 'feistel', sources: [3, 1, 4], targets: [0, 2], order: [2, 4, 0, 3, 1], ops: [
            { form: 0, expr: { kind: 'source', index: 0 } },
            { form: 1, expr: { kind: 'source', index: 2 } }
        ]
    };
    const input = [10, 20, 30, 40, 50];
    const changed = stage(input, operation, 16);
    assert.deepEqual(changed, [80, 50, 34, 40, 20]);
    assert.deepEqual(stage(changed, operation, 16, true), input);
});

test('difficulty admission rejects independent Medium and fixed-partition Hard checks', () => {
    const key = Buffer.alloc(32, 37);
    const medium = generate(key, { date: '2026-10-12', level: 'medium' }).ir;
    assert(analyzeDifficulty(medium).coupled >= 4);
    medium.stages = medium.stages.filter(stage => stage.kind !== 'couple');
    assert.throws(() => assertDifficulty(medium));
    const hard = generate(key, { date: '2026-10-12', level: 'hard' });
    assert(analyzeDifficulty(hard.ir).partitionWidths.length >= 3);
    assert.match(emit(hard), /return VM_EQU\(a,b\)/);
    hard.ir.stages = hard.ir.stages.filter(stage => stage.kind !== 'feistel');
    assert.throws(() => assertDifficulty(hard.ir));
});

test('fingerprints retain conditional branch order and generalized round connections', () => {
    const challenge = generate(Buffer.alloc(32, 38), { date: '2026-10-12', level: 'hard' });
    const changed = structuredClone(challenge.ir);
    const round = changed.stages.find(stage => stage.kind === 'feistel');
    [round.sources[0], round.sources[1]] = [round.sources[1], round.sources[0]];
    assert.notEqual(fingerprint(changed), challenge.fingerprint);
    const make = (left, right) => ({
        bits: 16, width: 3, stages: [{
            kind: 'couple', ops: [{
                i: 0, form: 1,
                expr: { kind: 'select', selector: 1, value: 1, left: { kind: 'source', index: left }, right: { kind: 'source', index: right } }
            }]
        }]
    });
    assert.notEqual(fingerprint(make(1, 2)), fingerprint(make(2, 1)));
});
