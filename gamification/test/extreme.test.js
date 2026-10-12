import test from 'node:test';
import assert from 'node:assert/strict';
import { generate, fingerprint } from '../src/generation/generator.js';
import { transform, words } from '../src/generation/model.js';
import { analyzeDifficulty, assertDifficulty } from '../src/generation/difficulty.js';
import { emit } from '../src/generation/emitter.js';
import { seal, hash } from '../src/core/crypto.js';
import { previousAnswers } from '../src/round/answers.js';
import { description } from '../src/publication/description.js';

test('Extreme has shared acyclic circuits, live dependencies and representation changes', () => {
    for (let round = 1; round <= 10; round++) {
        const c = generate(Buffer.alloc(32, 51), { date: '2026-10-12', round, level: 'extreme' });
        const summary = analyzeDifficulty(c.ir);
        assert.equal(c.password.length, 48);
        assert(summary.braidedRounds >= 32 && summary.minimumInputDependencies === 24);
        for (const stage of c.ir.stages.filter(stage => stage.kind === 'braid')) {
            assert(stage.targets.every(i => !stage.sources.includes(i)));
            function refs(node, limit) {
                if (node.kind === 'ref') assert(node.index >= 0 && node.index < limit);
                if (node.left) refs(node.left, limit);
                if (node.right) refs(node.right, limit);
            }
            stage.nodes.forEach((node, i) => refs(node, i));
            stage.ops.forEach(op => {
                refs(op.expr, stage.nodes.length);
                assert.equal(op.expr.left.index, stage.nodes.length - 1);
            });
            const previous = c.ir.stages.indexOf(stage);
            const input = transform({ ...c.ir, stages: c.ir.stages.slice(0, previous) }, words(c.password, 16));
            const single = { ...c.ir, stages: [stage] };
            assert.deepEqual(transform(single, transform(single, input), true), input);
        }
        const downgraded = structuredClone(c.ir);
        downgraded.stages = downgraded.stages.filter(stage => stage.kind !== 'braid');
        assert.throws(() => assertDifficulty(downgraded));
        const changed = structuredClone(c.ir), stage = changed.stages.find(stage => stage.kind === 'braid');
        stage.nodes[1].left.kind = stage.nodes[1].left.kind === 'xor' ? 'add' : 'xor';
        assert.notEqual(fingerprint(changed), c.fingerprint);
        assert.notDeepEqual(transform(changed, words(c.password, 16)), c.target);
    }
});

test('Extreme emission uses actual VM, captured circuit nodes, lilac badge and clean caption', () => {
    const source = emit(generate(Buffer.alloc(32, 51), { date: '2026-10-12', level: 'extreme' }));
    assert.match(source, /return VM_ADD\(a,b\)/);
    assert.match(source, /unsigned q0=/);
    assert.match(source, /RGB\(232,178,255\)/);
    assert(!source.includes('\\"protected with obfus.h\\"'));
    const state = { date: '2026-10-12', round: 3, files: ['easy', 'medium', 'hard', 'extreme'].map(role => ({ role, url: 'https://example.test/' + role })) };
    const body = description(state);
    assert.equal(body.match(/^## /gm).length, 4);
    assert(body.includes('🔴 [HARDCORE]') && body.includes('🟣 [EXTREME]'));
});

test('saved three-level rounds survive the four-level transition; missing Extreme fails closed', async () => {
    const key = Buffer.alloc(32, 15), answers = { easy: 'old-easy', medium: 'old-medium', hard: 'old-hard' };
    const data = Buffer.from(JSON.stringify(seal(key, { date: '2026-10-12', round: 2, answers })));
    const state = { version: 4, date: '2026-10-12', round: 2, files: [{ name: 'answers.json', role: 'answers', sha256: hash(data) }] };
    const api = { release: async () => ({ id: 1 }), assets: async () => [{ name: 'answers.json' }], download: async () => data };
    assert.deepEqual((await previousAnswers(api, key, state)).answers, answers);
    await assert.rejects(previousAnswers(api, key, { ...state, version: 5 }), /Saved answer levels mismatch/);
});
