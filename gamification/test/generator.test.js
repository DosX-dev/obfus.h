import test from 'node:test';
import assert from 'node:assert/strict';
import { generate, fingerprint, behaviorFingerprint } from '../src/generation/generator.js';
import { shapeFingerprint } from '../src/generation/diversity.js';
import { transform, words } from '../src/generation/model.js';
import { seal, unseal, derive } from '../src/core/crypto.js';
import { LEVELS } from '../src/core/config.js';
const key = Buffer.alloc(32, 7);
const quick = process.env.CRACKME_QUICK_TESTS === '1';
const samples = quick ? 100 : 1000;
for (const level of LEVELS)
    test(`${samples} distinct reversible ${level} graphs`, () => {
        const fingerprints = new Set(), behaviors = new Set();
        for (let n = 0; n < samples; n++) {
            const c = generate(key, { date: '2026-10-12', round: n + 1, level });
            const input = words(c.password, c.ir.bits);
            assert.deepEqual(transform(c.ir, c.target, true), input);
            const random = Array.from(
                { length: c.ir.width },
                (_, i) => derive(key, 'test', [n, i]).readUInt16LE(0) & (2 ** c.ir.bits - 1)
            );
            assert.deepEqual(transform(c.ir, transform(c.ir, random), true), random);
            assert(!fingerprints.has(c.fingerprint), 'Repeated normalized algorithm');
            fingerprints.add(c.fingerprint);
            const behavior = JSON.stringify(['A', 'Z', '9'].map(char => transform(c.ir, words(char.repeat(c.ir.length), c.ir.bits))));
            assert(!behaviors.has(behavior), 'Checks must differ in actual behavior, not only graph spelling');
            behaviors.add(behavior);
            const zero = Array(c.ir.width).fill(0),
                maximum = Array(c.ir.width).fill(2 ** c.ir.bits - 1);
            for (const v of [zero, maximum]) assert.deepEqual(transform(c.ir, transform(c.ir, v), true), v);
            function numeric(node, excluded, width) {
                if (node.kind === 'ref') {
                    assert(Number.isInteger(node.index) && node.index >= 0);
                } else if (node.kind === 'source') {
                    assert(Number.isInteger(node.index) && node.index >= 0 && node.index < width && node.index !== excluded);
                } else if (node.right) {
                    assert(['add', 'sub', 'xor', 'and', 'or', 'select'].includes(node.kind));
                    if (node.kind === 'select') {
                        assert(Number.isInteger(node.selector) && node.selector >= 0 && node.selector < width && node.selector !== excluded);
                        assert(node.value > 0 && node.value <= 2 ** c.ir.bits - 1 && (node.value & (node.value - 1)) === 0);
                    }
                    numeric(node.left, excluded, width);
                    numeric(node.right, excluded, width);
                } else {
                    assert(Number.isInteger(node.value) && node.value > 0);
                    if (node.kind === 'mul') assert(node.value % 2 === 1 && node.value <= 15);
                    else if (['rot', 'shlXor', 'shrXor'].includes(node.kind)) assert(node.value < c.ir.bits);
                    else assert(node.value <= 2 ** c.ir.bits - 1);
                    numeric(node.left, excluded, width);
                }
            }
            for (const stage of c.ir.stages) for (const op of stage.ops || []) {
                if (op.expr) numeric(op.expr, stage.kind === 'couple' ? op.i : -1, ['feistel', 'braid'].includes(stage.kind) ? stage.sources.length : c.ir.width);
                for (const item of op.sequence || []) {
                    if (item.kind === 'mul') assert(item.value % 2 === 1 && item.value <= 15);
                    else if (['rot', 'shlXor', 'shrXor'].includes(item.kind)) assert(item.value > 0 && item.value < c.ir.bits);
                    else assert(item.value > 0 && item.value <= 2 ** c.ir.bits - 1);
                }
            }
        }
    });
test('deterministic replay and separated derivation domains', () => {
    const p = { date: '2026-10-12', level: 'hardcore' };
    assert.deepEqual(generate(key, p), generate(key, p));
    assert.notDeepEqual(derive(key, 'password', p), derive(key, 'structure', p));
    const c = generate(key, p),
        changed = structuredClone(c.ir);
    changed.seed++;
    changed.groups.reverse();
    changed.stages[0].ops[0].sequence[0].value ^= 1;
    assert.equal(fingerprint(changed), c.fingerprint, 'Fingerprint must ignore constants/grouping');
});

test('a century of daily Easy structures does not exhaust or repeat the generator', { timeout: 300000, skip: quick }, () => {
    const seen = new Set(), shapes = new Set(), behaviors = new Set();
    const first = Date.UTC(2026, 9, 12);
    for (let day = 0; day < 36525; day++) {
        const date = new Date(first + day * 86400000).toISOString().slice(0, 10);
        const c = generate(key, { date, round: day + 1, level: 'easy' });
        assert(!seen.has(c.fingerprint), 'Century schedule repeated a normalized structure');
        seen.add(c.fingerprint);
        const shape = shapeFingerprint(c.ir), behavior = behaviorFingerprint(c.ir);
        assert(!shapes.has(shape), 'Century schedule repeated normalized expression shapes');
        assert(!behaviors.has(behavior), 'Century schedule repeated actual transform behavior');
        shapes.add(shape);
        behaviors.add(behavior);
    }
});
test('answer authentication, secrecy and key rotation fails closed', () => {
    const value = { answers: { hardcore: 'PRIVATE-ANSWER' }, round: 3 };
    const envelope = seal(key, value);
    assert.deepEqual(unseal(key, envelope), value);
    assert(!JSON.stringify(envelope).includes(value.answers.hardcore));
    assert.throws(() => unseal(Buffer.alloc(32, 8), envelope));
    envelope.data = Buffer.from('tampered').toString('base64');
    assert.throws(() => unseal(key, envelope));
});
test('structural fingerprint ignores independent scheduling but detects changed operations', () => {
    const c = generate(key, { date: '2026-10-12', level: 'hardcore' }),
        changed = structuredClone(c.ir);
    changed.stages[0].ops.reverse();
    changed.storage.reverse();
    changed.seed ^= 123;
    const tree = changed.stages.find((s) => s.kind === 'feistel').ops[0].expr;
    function constants(node) {
        if (node.value !== undefined) node.value ^= 1;
        if (node.left) constants(node.left);
        if (node.right) constants(node.right);
    }
    constants(tree);
    assert.equal(fingerprint(changed), c.fingerprint);
    function alter(node) {
        if (node.right) {
            node.kind = node.kind === 'add' ? 'xor' : 'add';
            delete node.selector;
            delete node.value;
            return true;
        }
        return (node.left && alter(node.left)) || (node.right && alter(node.right));
    }
    assert(alter(tree));
    assert.notEqual(fingerprint(changed), c.fingerprint);
});
test('parameters reject invalid calendars and attempts', () => {
    for (const date of ['2026-02-30', '2026-13-01', '2026-00-01'])
        assert.throws(() => generate(key, { date, level: 'easy' }));
    for (const attempt of [-1, 3, 0.5])
        assert.throws(() => generate(key, { date: '2026-10-12', level: 'easy', attempt }));
});
