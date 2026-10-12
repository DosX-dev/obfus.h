import test from 'node:test';
import assert from 'node:assert/strict';
import { createHmac, createCipheriv } from 'node:crypto';
import { gzipSync } from 'node:zlib';
import { hash, unseal } from '../src/core/crypto.js';
import { loadHistory } from '../src/round/history.js';
import { moscow } from '../src/core/schedule.js';

test('stored v1 answers survive a different generator implementation', () => {
    const master = Buffer.alloc(32, 17), iv = Buffer.alloc(12, 3);
    // A fixture written using the original storage protocol; no generator calls.
    const key = createHmac('sha256', master).update(JSON.stringify([1, 'answers-key', []])).digest();
    const cipher = createCipheriv('aes-256-gcm', key, iv);
    cipher.setAAD(Buffer.from('obfus-daily-answers-v1'));
    const value = { round: 14, date: '2026-10-12', answers: { easy: 'PREVIOUS' } };
    const data = Buffer.concat([cipher.update(JSON.stringify(value)), cipher.final()]);
    assert.deepEqual(unseal(master, { version: 1, iv: iv.toString('hex'), tag: cipher.getAuthTag().toString('hex'), data: data.toString('base64') }), value);
});

test('100 years of history remains compressed outside the Git manifest', async () => {
    const history = Array.from({ length: 36525 * 3 }, (_, i) => ({ structure: hash('historical-graph/' + i), behavior: hash('historical-behavior/' + i), shape: hash('historical-shape/' + i) }));
    const data = gzipSync(JSON.stringify({ version: 1, checks: history }), { level: 9 });
    const state = { round: 36525, historyCount: history.length, files: [{ name: 'history-2126-10-12.json.gz', role: 'history', sha256: hash(data) }] };
    const api = { release: async () => ({ id: 1 }), assets: async () => [{ name: state.files[0].name }], download: async () => data };
    assert(JSON.stringify(state).length < 1024);
    assert.deepEqual(await loadHistory(api, state), history);
    state.files[0].sha256 = hash('wrong');
    await assert.rejects(loadHistory(api, state));
});

test('Moscow publication date crosses midnight and handles leap days', () => {
    assert.deepEqual(moscow(new Date('2028-02-28T21:01:00Z')), { date: '2028-02-29', hour: 0 });
    assert.deepEqual(moscow(new Date('2026-10-12T10:00:00Z')), { date: '2026-10-12', hour: 13 });
});
