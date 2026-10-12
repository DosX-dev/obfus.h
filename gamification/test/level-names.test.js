import test from 'node:test';
import assert from 'node:assert/strict';
import { gzipSync } from 'node:zlib';
import { LEVELS, VERSION, levelsForVersion } from '../src/core/config.js';
import { canonicalLevel } from '../src/core/legacy-state.js';
import { seal, hash } from '../src/core/crypto.js';
import { generate } from '../src/generation/generator.js';
import { previousAnswers } from '../src/round/answers.js';
import { loadHistory, createHistoryIndex, hasSeen } from '../src/round/history.js';
import { validateState } from '../src/publication/state.js';
import { description } from '../src/publication/description.js';

test('Hardcore is the only current generator key and publication role', () => {
    assert.equal(VERSION, 6);
    assert.deepEqual(LEVELS, ['easy', 'medium', 'hardcore', 'extreme']);
    const key = Buffer.alloc(32, 17);
    assert.equal(generate(key, { date: '2026-10-12', level: 'hardcore' }).ir.level, 'hardcore');
    const old = levelsForVersion(5)[2];
    assert.throws(() => generate(key, { date: '2026-10-12', level: old }), /Invalid round/);
    const files = ['answers', 'history', ...LEVELS].map(role => ({ role, name: role + '.zip', sha256: hash(role), storage: ['answers', 'history'].includes(role) ? 'private' : 'public', url: 'https://example.test/' + role }));
    const state = { version: VERSION, round: 4, date: '2026-10-13', files };
    validateState(state);
    assert(description(state).includes('[HARDCORE](https://example.test/hardcore)'));
    assert.throws(() => validateState({ ...state, files: files.map(file => file.role === 'hardcore' ? { ...file, role: old } : file) }), /Incomplete/);
});

test('pre-v6 encrypted answers preserve their exact passwords under canonical names', async () => {
    const key = Buffer.alloc(32, 18);
    for (const version of [4, 5, 6]) {
        const answers = Object.fromEntries(levelsForVersion(version).map(level => [level, 'stored-' + level]));
        const data = Buffer.from(JSON.stringify(seal(key, { date: '2026-10-12', round: 3, answers })));
        const state = { version, date: '2026-10-12', round: 3, files: [{ role: 'answers', name: 'answers.json', sha256: hash(data) }] };
        const api = { release: async () => ({ id: 1 }), assets: async () => [{ name: 'answers.json' }], download: async () => data };
        assert.deepEqual((await previousAnswers(api, key, state)).answers,
            Object.fromEntries(Object.entries(answers).map(([level, answer]) => [canonicalLevel(level), answer])));
        const bad = Buffer.from(data); bad[bad.length - 1] ^= 1;
        await assert.rejects(previousAnswers({ ...api, download: async () => bad }, key, state));
    }
});

test('authenticated legacy history keeps Hardcore novelty checks and old release links', async () => {
    const old = levelsForVersion(5)[2];
    const checks = Array.from({ length: 4 }, (_, i) => ({ level: levelsForVersion(5)[i], structure: hash('graph/' + i), behavior: hash('behavior/' + i), shape: hash('shape/' + i), profile: [['stage:affine', 1]] }));
    const data = gzipSync(JSON.stringify({ version: 1, checks }));
    const state = { version: 5, round: 1, date: '2026-10-12', historyCount: 4, files: [{ role: 'history', name: 'history.json.gz', sha256: hash(data) }] };
    const api = { release: async () => ({ id: 1 }), assets: async () => [{ name: 'history.json.gz' }], download: async () => data };
    const history = await loadHistory(api, state);
    assert.equal(history[2].level, 'hardcore');
    assert.equal(history[2].structure, checks[2].structure);
    assert(hasSeen(createHistoryIndex(history), { level: 'hardcore', structure: hash('new-graph'), behavior: hash('new-behavior'), shape: hash('new-shape'), profile: checks[2].profile }));
    assert.equal(checks[2].level, old);
    const body = description({ ...state, files: [{ role: old, url: 'https://example.test/original.zip' }], previous: { round: 1, answers: { [old]: 'stored-answer' } } });
    assert(body.includes('[HARDCORE](https://example.test/original.zip)'));
    assert(body.includes('**HARDCORE**: `stored-answer`'));
});
