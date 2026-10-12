import test from 'node:test';
import assert from 'node:assert/strict';
import fs from 'node:fs/promises';
import path from 'node:path';
import { GAMIFICATION_ROOT } from '../src/core/paths.js';
import { LEVELS } from '../src/core/config.js';
import { hash, unseal } from '../src/core/crypto.js';
import { generate, behaviorFingerprint } from '../src/generation/generator.js';
import { shapeFingerprint } from '../src/generation/diversity.js';
import { createBundle } from '../src/round/bundle.js';
import { loadHistory } from '../src/round/history.js';

test('bundle assembly preserves public/private separation and noncircular checksums', async () => {
    const key = Buffer.alloc(32, 29);
    await fs.mkdir(path.join(GAMIFICATION_ROOT, 'out'), { recursive: true });
    const directory = await fs.mkdtemp(path.join(GAMIFICATION_ROOT, 'out', 'bundle-test-'));
    const selected = [];
    for (const level of LEVELS) {
        const challenge = generate(key, { date: '2026-10-12', round: 1, level });
        const exe = path.join(directory, level + '.exe');
        const data = Buffer.from('fixture-player-' + level);
        await fs.writeFile(exe, data);
        selected.push({ level, challenge, exe, folder: directory, counts: [], audit: { sha256: hash(data), size: data.length }, witness: behaviorFingerprint(challenge.ir), shape: shapeFingerprint(challenge.ir) });
    }
    const result = await createBundle(key, {
        date: '2026-10-12', round: 1, repository: 'example/project', previous: null, history: [],
        header: Buffer.from('fixture-header'), revision: 'a'.repeat(40), selected,
        publicDirectory: path.join(directory, 'public'),
        toolchainMetadata: { source: 'local', versions: { x86: 'fixture-x86', x64: 'fixture-x64' } }
    });
    assert.equal(result.files.length, 8);
    assert.equal(result.state.historyCount, 3);
    for (const file of result.files) {
        assert.equal(result.state.files.find(entry => entry.name === file.name).sha256, hash(file.data));
        for (const { challenge } of selected) {
            assert(!file.data.includes(Buffer.from(challenge.password)), 'Current plaintext answer escaped into release files');
        }
    }
    const answerFile = result.files.find(file => file.role === 'answers');
    const saved = unseal(key, JSON.parse(answerFile.data));
    assert.deepEqual(saved.answers, Object.fromEntries(selected.map(item => [item.level, item.challenge.password])));
    const manifest = JSON.parse(result.files.find(file => file.role === 'manifest').data);
    assert.deepEqual(manifest.toolchain, { source: 'local', versions: { x86: 'fixture-x86', x64: 'fixture-x64' } });
    assert.equal(manifest.files.length, 6);
    assert(!manifest.files.some(file => file.role === 'manifest' || file.role === 'checksums'));
    const sums = result.files.find(file => file.role === 'checksums').data.toString().trim().split('\n');
    assert.equal(sums.length, 7);
    assert(sums.some(line => line.endsWith('manifest-2026-10-12.json')));
    assert(!sums.some(line => line.includes('SHA256SUMS-')));
    const assets = result.files.map((file, id) => ({ ...file, id }));
    const api = { release: async () => ({ id: 1 }), assets: async () => assets, download: async asset => asset.data };
    assert.equal((await loadHistory(api, result.state)).length, 3);
});
