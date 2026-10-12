import test from 'node:test';
import assert from 'node:assert/strict';
import fs from 'node:fs/promises';
import path from 'node:path';
import { GAMIFICATION_ROOT } from '../src/core/paths.js';
import { prepare } from '../src/round/prepare.js';
import { selectCandidate } from '../src/round/candidate.js';
import { createHistoryIndex } from '../src/round/history.js';
import { generate, behaviorFingerprint } from '../src/generation/generator.js';
import { shapeFingerprint } from '../src/generation/diversity.js';

test('generation failures retain diagnostics and exhaust all three attempts without building', async t => {
    await fs.mkdir(path.join(GAMIFICATION_ROOT, 'out'), { recursive: true });
    const directory = await fs.mkdtemp(path.join(GAMIFICATION_ROOT, 'out', 'retry-test-'));
    t.mock.method(console, 'log', () => { });
    await assert.rejects(prepare(Buffer.alloc(32, 7), { date: '2026-02-30', round: 1, directory, toolchain: 'must-not-be-used' }), /No verified easy candidate after three attempts/);
    for (let attempt = 0; attempt < 3; attempt++) {
        const folder = path.join(directory, 'easy-' + attempt);
        assert.match(await fs.readFile(path.join(folder, 'failure.private.txt'), 'utf8'), /Invalid round parameters/);
        assert.deepEqual(await fs.readdir(folder), ['failure.private.txt']);
    }
    assert.deepEqual(await fs.readdir(path.join(directory, 'public')), []);
});

test('novelty rejections retain diagnostics for every attempt without compiling', async t => {
    const key = Buffer.alloc(32, 63), date = '2026-10-12', level = 'easy';
    const history = Array.from({ length: 3 }, (_, attempt) => {
        const challenge = generate(key, { date, round: 1, level, attempt });
        return { structure: challenge.fingerprint, behavior: behaviorFingerprint(challenge.ir), shape: shapeFingerprint(challenge.ir) };
    });
    const directory = await fs.mkdtemp(path.join(GAMIFICATION_ROOT, 'out', 'novelty-retry-test-'));
    t.mock.method(console, 'log', () => { });
    await assert.rejects(selectCandidate(key, {
        date, round: 1, level, directory,
        toolchain: 'must-not-be-used', known: createHistoryIndex(history)
    }), /No verified easy candidate/);
    for (let attempt = 0; attempt < 3; attempt++) {
        const folder = path.join(directory, 'easy-' + attempt);
        assert.match(await fs.readFile(path.join(folder, 'failure.private.txt'), 'utf8'), /historical novelty or recent similarity/);
        assert.deepEqual(await fs.readdir(folder), ['failure.private.txt']);
    }
});
