import test from 'node:test';
import assert from 'node:assert/strict';
import { generate, fingerprint } from '../src/generation/generator.js';
import { familyStage } from '../src/generation/family-model.js';
import { transform, words } from '../src/generation/model.js';
import { shapeFingerprint } from '../src/generation/diversity.js';
import { LEVELS } from '../src/core/config.js';

test('arbitrary bit routing matches a BigInt oracle across the full word domain', () => {
    for (const bits of [8, 16]) {
        const route = Array.from({ length: bits }, (_, i) => (i * 5 + 3) % bits);
        const stage = { kind: 'bitshuffle', ops: [{ i: 0, route, before: [], after: [] }] };
        const seen = new Set();
        for (let value = 0; value < 2 ** bits; value++) {
            let expected = 0n;
            route.forEach((destination, source) => { expected |= ((BigInt(value) >> BigInt(source)) & 1n) << BigInt(destination); });
            const result = familyStage([value], stage, bits, false);
            assert.equal(result[0], Number(expected));
            assert.deepEqual(familyStage(result, stage, bits, true), [value]);
            seen.add(result[0]);
        }
        assert.equal(seen.size, 2 ** bits);
    }
});

test('multi-source lifts match independent integer arithmetic and undo overwritten controls', () => {
    const kinds = ['xor', 'add', 'sub', 'mul', 'rot'];
    for (const bits of [8, 16]) for (const kind of kinds) {
        const mask = (1n << BigInt(bits)) - 1n;
        const stage = {
            kind: 'weave', ops: [{
                i: 0, kind, sources: [
                    { index: 1, mix: [{ kind: 'xor', value: 73 }], combine: 'xor' },
                    { index: 2, mix: [{ kind: 'mul', value: 3 }], combine: 'sub' },
                    { index: 3, mix: [{ kind: 'add', value: 19 }], combine: 'add' }
                ]
            }]
        };
        for (let a = 0; a < 256; a++) {
            const input = [a, (a * 257) & Number(mask), Number(mask) - a, a ^ 91];
            const control = ((BigInt(input[1]) ^ 73n) - (BigInt(input[2]) * 3n & mask) + (BigInt(input[3]) + 19n & mask)) & mask;
            let value = BigInt(a);
            if (kind === 'xor') value ^= control;
            if (kind === 'add') value += control;
            if (kind === 'sub') value -= control;
            if (kind === 'mul') value *= 1n + 2n * (control & 7n);
            if (kind === 'rot') {
                const count = 1n + (control & BigInt(bits / 2 - 1));
                value = (value << count) | (value >> (BigInt(bits) - count));
            }
            const result = familyStage(input, stage, bits, false);
            assert.equal(result[0], Number(value & mask));
            assert.deepEqual(familyStage(result, stage, bits, true), input);
            const chained = { kind: 'weave', ops: [...stage.ops, { ...stage.ops[0], i: 1, sources: [{ index: 0, mix: [], combine: 'xor' }, { index: 2, mix: [], combine: 'add' }] }] };
            assert.deepEqual(familyStage(familyStage(input, chained, bits, false), chained, bits, true), input);
        }
    }
});

test('all tiers vary new logic and preserve whole-block inverse and structural fingerprints', () => {
    const count = process.env.CRACKME_QUICK_TESTS === '1' ? 10 : 100;
    for (const level of LEVELS) {
        const seen = new Set(), shapes = new Set();
        for (let round = 1; round <= count; round++) {
            const challenge = generate(Buffer.alloc(32, 117), { date: '2026-10-12', round, level });
            shapes.add(shapeFingerprint(challenge.ir));
            const input = words(challenge.password, challenge.ir.bits);
            assert.deepEqual(transform(challenge.ir, challenge.target, true), input);
            for (const stage of challenge.ir.stages) {
                seen.add(stage.kind);
                if (stage.kind === 'weave') for (const op of stage.ops) {
                    assert(!op.sources.some(source => source.index === op.i));
                    assert.equal(new Set(op.sources.map(source => source.index)).size, op.sources.length);
                }
            }
            const changed = structuredClone(challenge.ir), stage = changed.stages.find(stage => stage.kind === 'bitshuffle');
            if (stage) {
                [stage.ops[0].route[0], stage.ops[0].route[1]] = [stage.ops[0].route[1], stage.ops[0].route[0]];
                assert.notEqual(fingerprint(changed), challenge.fingerprint);
                assert.notEqual(shapeFingerprint(changed), shapeFingerprint(challenge.ir));
            }
        }
        assert.equal(shapes.size, count);
        assert(seen.has('bitshuffle'));
        if (level !== 'easy') assert(seen.has('weave'));
    }
});
