import test from 'node:test';
import assert from 'node:assert/strict';
import { triangularModel } from '../src/generation/triangular.js';
import { generate, fingerprint } from '../src/generation/generator.js';
import { LEVELS } from '../src/core/config.js';
import { distance } from '../src/generation/diversity.js';

test('triangular bands match an independent BigInt oracle and permute the whole byte domain', () => {
    for (const bits of [8, 16]) for (let cut = 2; cut < bits - 1; cut++)
        for (const kind of ['xor', 'add', 'sub']) for (const term of ['product', 'square', 'fold']) {
            const step = { cut, kind, term, rotation: bits - 1, salt: 2 ** cut - 1, shift: cut - 1 };
            const stage = { kind: 'triangular', ops: [{ i: 0, steps: [step] }] }, seen = new Set();
            const inputs = bits === 8 ? Array.from({ length: 256 }, (_, i) => i) : [0, 1, 32767, 32768, 65534, 65535];
            for (const value of inputs) {
                const n = BigInt(value), mask = (1n << BigInt(bits)) - 1n;
                const rotated = ((n << BigInt(step.rotation)) | (n >> BigInt(bits - step.rotation))) & mask;
                const low = rotated & ((1n << BigInt(cut)) - 1n), high = rotated >> BigInt(cut);
                const t = term === 'product' ? low * (low ^ BigInt(step.salt))
                    : term === 'square' ? low * low + BigInt(step.salt)
                        : (low & (low >> BigInt(step.shift))) ^ BigInt(step.salt);
                const upperMask = (1n << BigInt(bits - cut)) - 1n;
                const delta = t & upperMask;
                const upper = (kind === 'xor' ? high ^ delta : kind === 'add' ? high + delta : high - delta) & upperMask;
                const actual = triangularModel([value], stage, bits, false);
                assert.deepEqual(actual, [Number(low | (upper << BigInt(cut)))]);
                assert.deepEqual(triangularModel(actual, stage, bits, true), [value]);
                seen.add(actual[0]);
            }
            assert.equal(seen.size, inputs.length);
        }
});

test('every level contains nonlinear bands; constants and rotations alone do not inflate novelty', () => {
    for (const level of LEVELS) {
        const ir = generate(Buffer.alloc(32, 63), { date: '2026-10-13', level }).ir;
        assert(ir.stages.some(stage => stage.kind === 'triangular'));
        const changed = structuredClone(ir);
        for (const stage of changed.stages.filter(stage => stage.kind === 'triangular')) {
            stage.ops.reverse();
            for (const op of stage.ops) for (const step of op.steps) {
                step.salt ^= 1;
                step.rotation = step.rotation === 1 ? 2 : 1;
            }
        }
        assert.equal(fingerprint(changed), fingerprint(ir));
        assert.equal(distance(changed, ir), 0);
        changed.stages.find(stage => stage.kind === 'triangular').ops[0].steps[0].term = 'different-term';
        assert.notEqual(fingerprint(changed), fingerprint(ir));
        assert(distance(changed, ir) > 0);
    }
});
