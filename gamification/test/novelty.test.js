import test from 'node:test';
import assert from 'node:assert/strict';
import { gzipSync } from 'node:zlib';
import { hash } from '../src/core/crypto.js';
import { LEVELS } from '../src/core/config.js';
import { createHistoryIndex, hasSeen, remember, retainHistory, loadHistory, RECENT_CHECKS_PER_LEVEL } from '../src/round/history.js';
import { profileDistance, profile, shapeFingerprint } from '../src/generation/diversity.js';
import { generate, behaviorFingerprint } from '../src/generation/generator.js';

const entry = (i, level = 'easy') => ({
    level, structure: hash('structure/' + level + i), behavior: hash('behavior/' + level + i),
    shape: hash('shape/' + level + i), profile: [['stage:affine', 2], ['affine:xor,rot,add', 20]]
});

test('new hashes cannot bypass recent normalized-shape similarity admission', () => {
    const previous = entry(1), candidate = entry(2);
    candidate.profile[1][1]++;
    const index = createHistoryIndex([previous]);
    assert(hasSeen(index, candidate), 'A near-copy with different hashes must be rejected');
    candidate.profile = [['stage:affine', 2], ['affine:mul,shrXor,sub', 20]];
    assert(!hasSeen(index, candidate), 'A sufficiently changed operation profile must remain eligible');
    assert(hasSeen(index, { ...candidate, structure: previous.structure }), 'All-time structure history remains enforced');
    remember(index, candidate);
    assert(hasSeen(index, candidate));
    assert.equal(profileDistance([], []), 0);
});

test('recent profile retention is bounded per level without losing all-time fingerprints', () => {
    const history = [];
    for (let round = 0; round < 30; round++) for (const level of LEVELS) history.push(entry(round, level));
    const retained = retainHistory(history.slice(0, -3), history.slice(-3));
    assert.equal(retained.length, history.length);
    assert.deepEqual(retained.map(value => value.structure), history.map(value => value.structure));
    for (const level of LEVELS) {
        const recent = retained.filter(value => value.level === level && value.profile);
        assert.equal(recent.length, RECENT_CHECKS_PER_LEVEL);
        assert.deepEqual(recent.map(value => value.structure), history.filter(value => value.level === level).slice(-RECENT_CHECKS_PER_LEVEL).map(value => value.structure));
    }
    assert(history[0].profile, 'Compaction must not mutate the caller history');
});

test('recent profiles reload through authenticated history and malformed metadata fails closed', async () => {
    let checks = LEVELS.map(level => entry(1, level));
    const state = { round: 1, historyCount: 3, files: [{ name: 'history.json.gz', role: 'history' }] };
    let data;
    function encode() {
        data = gzipSync(JSON.stringify({ version: 1, checks }));
        state.files[0].sha256 = hash(data);
    }
    const api = { release: async () => ({ id: 1 }), assets: async () => [{ name: 'history.json.gz' }], download: async () => data };
    encode();
    assert.deepEqual(await loadHistory(api, state), checks);
    assert(hasSeen(createHistoryIndex(await loadHistory(api, state)), entry(2)));
    checks[0].profile.push(checks[0].profile[0]);
    encode();
    await assert.rejects(loadHistory(api, state), /Duplicate profile/);
    checks = LEVELS.map(level => entry(1, level));
    checks[0].profile[0][1] = -1;
    encode();
    await assert.rejects(loadHistory(api, state), /Invalid recent-profile data/);
});

test('100 daily rounds clear both all-time novelty and recent similarity gates', async () => {
    const key = Buffer.alloc(32, 41), start = Date.UTC(2026, 9, 12);
    let history = [];
    for (let day = 0; day < 100; day++) {
        const date = new Date(start + day * 86400000).toISOString().slice(0, 10);
        const index = createHistoryIndex(history), additions = [];
        for (const level of LEVELS) {
            let selected;
            for (let attempt = 0; attempt < 3; attempt++) {
                const challenge = generate(key, { date, round: day + 1, level, attempt });
                const candidate = {
                    level, structure: challenge.fingerprint, behavior: behaviorFingerprint(challenge.ir),
                    shape: shapeFingerprint(challenge.ir), profile: [...profile(challenge.ir)]
                };
                if (!hasSeen(index, candidate)) { selected = candidate; break; }
            }
            assert(selected, 'Daily structure must remain eligible within the bounded retry policy');
            remember(index, selected);
            additions.push(selected);
        }
        history = retainHistory(history, additions);
        assert.equal(history.length, (day + 1) * LEVELS.length);
        assert(history.filter(entry => entry.profile).length <= RECENT_CHECKS_PER_LEVEL * LEVELS.length);
    }
    const data = gzipSync(JSON.stringify({ version: 1, checks: history }));
    const state = { round: 100, historyCount: 300, files: [{ name: 'history.json.gz', role: 'history', sha256: hash(data) }] };
    const api = { release: async () => ({ id: 1 }), assets: async () => [{ name: 'history.json.gz' }], download: async () => data };
    assert.deepEqual(await loadHistory(api, state), history, 'Actual generated profiles must survive durable reload');
});
