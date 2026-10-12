import test from 'node:test';
import assert from 'node:assert/strict';
import { playerZip, unzipPlayer } from '../src/core/zip.js';
import { description } from '../src/publication/description.js';

test('deterministic single-player ZIP preserves every EXE byte and rejects corruption', () => {
    const data = Buffer.alloc(65536);
    for (let i = 0; i < data.length; i++) data[i] = i & 255;
    const zip = playerZip('hardcore.exe', data);
    assert.deepEqual(zip, playerZip('hardcore.exe', data));
    assert.deepEqual(unzipPlayer(zip), { name: 'hardcore.exe', data });
    assert(zip.length < data.length);
    for (const at of [0, 14, 18, zip.length - 22, zip.length - 6]) {
        const bad = Buffer.from(zip); bad[at] ^= 1;
        assert.throws(() => unzipPlayer(bad));
    }
    assert.throws(() => playerZip('../bad.exe', data));
    assert.throws(() => unzipPlayer(Buffer.alloc(4)));
});

test('player release links each SVG button to its current round ZIP', () => {
    const state = { date: '2026-10-12', round: 1, files: ['easy', 'medium', 'hardcore', 'extreme', 'answers', 'history'].map(role => ({ role, url: 'https://example.test/' + role })) };
    const body = description(state);
    assert(body.includes('## Download task'));
    assert(!body.includes('<table>'));
    for (const level of ['easy', 'medium', 'hardcore', 'extreme']) {
        assert(body.includes(`<a href="https://example.test/${level}"><img src="https://raw.githubusercontent.com/DosX-dev/obfus.h/main/gamification/assets/download-buttons/${level}.svg"`));
        assert(body.includes(`alt="Download ${level.toUpperCase()}" width="240" height="120"`));
    }
    assert(!body.includes('https://example.test/answers'));
    assert(!body.includes('https://example.test/history'));
    assert(!body.includes('No previous answers'));
    const next = description({ ...state, previous: { round: 1, answers: { easy: 'previous-fixture' } } });
    assert(next.includes('<details>'));
    assert(next.includes('previous-fixture'));
    const later = description({ ...state, round: 9, files: state.files.map(file => ({ ...file, url: file.url + '-round-9.zip' })) });
    assert(later.includes('href="https://example.test/extreme-round-9.zip"'));
});
