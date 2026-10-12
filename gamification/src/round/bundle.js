import fs from 'node:fs/promises';
import path from 'node:path';
import { gzipSync } from 'node:zlib';
import { seal, hash } from '../core/crypto.js';
import { VERSION, RELEASE_TAG } from '../core/config.js';
import { HISTORY_VERSION, retainHistory } from './history.js';
import { profile } from '../generation/diversity.js';
import { playerZip } from '../core/zip.js';

// Select player ZIPs and private automation state separately.
// Generated source, plaintext current answers, traces and replay keys never enter files.
export async function createBundle(key, { date, round, repository, previous, history, header, revision, selected, publicDirectory, toolchainMetadata }) {
    if (!toolchainMetadata?.versions?.x86 || !toolchainMetadata?.versions?.x64) throw new Error('Actual compiler metadata is required');
    const files = [];
    const answers = {};
    const graphs = [];
    const witnesses = [];
    const shapes = [];
    const jobs = [];
    const report = [];

    for (const { level, challenge, exe, audit, counts, folder, witness, shape } of selected) {
        answers[level] = challenge.password;
        graphs.push(challenge.fingerprint);
        witnesses.push(witness);
        shapes.push(shape);

        const playerName = `obfus-crackme-${date}-${level}-x64.exe`;
        const name = playerName.replace(/\.exe$/, '.zip');
        files.push({ name, data: playerZip(playerName, await fs.readFile(exe)), role: level });
        jobs.push({ level, exe, screenshot: path.join(folder, 'preview.png') });
        report.push({
            level,
            attempt: challenge.ir.attempt,
            fingerprint: challenge.fingerprint,
            behavior: witness,
            stages: challenge.ir.stages.length,
            ...audit,
            counts
        });
    }

    files.push({
        name: `answers-${date}.json`,
        data: Buffer.from(JSON.stringify(seal(key, { version: VERSION, date, round, answers }))),
        role: 'answers'
    });
    const newHistory = graphs.map((structure, i) => ({
        structure, behavior: witnesses[i], shape: shapes[i],
        level: selected[i].level, profile: [...profile(selected[i].challenge.ir)]
    }));
    files.push({
        name: `history-${date}.json.gz`,
        data: gzipSync(JSON.stringify({ version: HISTORY_VERSION, checks: retainHistory(history, newHistory) }), { level: 9 }),
        role: 'history'
    });

    const assetUrl = name => `https://github.com/${repository}/releases/download/${RELEASE_TAG}/${name}`;
    const entries = () => files.map(file => ({ name: file.name, role: file.role, sha256: hash(file.data), size: file.data.length, storage: ['answers', 'history'].includes(file.role) ? 'private' : 'public', ...(!['answers', 'history'].includes(file.role) ? { url: assetUrl(file.name) } : {}) }));
    const metadata = {
        version: VERSION,
        date,
        round,
        commit: revision,
        headerSha256: hash(header),
        toolchain: toolchainMetadata,
        graphs,
        witnesses,
        shapes,
        historyCount: history.length + graphs.length,
        previous
    };

    return { state: { ...metadata, files: entries() }, files, report, jobs };
}
