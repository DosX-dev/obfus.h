import test from 'node:test';
import assert from 'node:assert/strict';
import { LEVELS, VERSION } from '../src/core/config.js';
import { seal, hash } from '../src/core/crypto.js';
import { previousAnswers } from '../src/round/answers.js';
import { validateState } from '../src/publication/state.js';

test('only the current complete four-level publication schema is accepted', () => {
    const files = ['answers', 'history', ...LEVELS].map(role => ({ role, name: role + '.zip', sha256: hash(role), storage: LEVELS.includes(role) ? 'public' : 'private' }));
    const state = { version: VERSION, round: 4, date: '2026-10-12', files };
    validateState(state);
    assert.throws(() => validateState({ ...state, version: VERSION - 1 }), /Unsupported/);
    assert.throws(() => validateState({ ...state, files: files.slice(1) }), /Incomplete/);
    assert.throws(() => validateState({ ...state, files: files.map(file => ({ ...file, storage: undefined })) }), /visibility/);
});

test('current saved answers authenticate and retain exact passwords', async () => {
    const key = Buffer.alloc(32, 18), answers = Object.fromEntries(LEVELS.map(level => [level, 'stored-' + level]));
    const value = { version: VERSION, date: '2026-10-12', round: 4, answers };
    const data = Buffer.from(JSON.stringify(seal(key, value)));
    const state = { ...value, files: [{ role: 'answers', name: 'answers.json', sha256: hash(data) }] };
    const api = { release: async () => ({ id: 1 }), assets: async () => [{ name: 'answers.json' }], download: async () => data };
    assert.deepEqual((await previousAnswers(api, key, state)).answers, answers);
    await assert.rejects(previousAnswers(api, key, { ...state, version: VERSION - 1 }), /Unsupported/);
    const bad = Buffer.from(data); bad[bad.length - 1] ^= 1;
    await assert.rejects(previousAnswers({ ...api, download: async () => bad }, key, state));
    const missing = Buffer.from(JSON.stringify(seal(key, { ...value, answers: { easy: 'missing-levels' } })));
    await assert.rejects(previousAnswers({ ...api, download: async () => missing }, key, { ...state, files: [{ ...state.files[0], sha256: hash(missing) }] }), /levels mismatch/);
});
