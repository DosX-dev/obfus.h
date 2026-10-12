import fs from 'node:fs/promises';
import path from 'node:path';
import assert from 'node:assert/strict';
import { LEVELS } from '../core/config.js';
import { hash } from '../core/crypto.js';
import { REPOSITORY_ROOT } from '../core/paths.js';
import { run } from '../core/process.js';
import { createHistoryIndex, remember } from './history.js';
import { selectCandidate } from './candidate.js';
import { createBundle } from './bundle.js';
import { profile } from '../generation/diversity.js';

// Coordinate one complete round locally. No GitHub mutation is reachable from here.
export async function prepare(key, { date, round, directory, toolchain, toolchainMetadata, repository = 'DosX-dev/obfus.h', previous = null, history = [] }) {
    const publicDirectory = path.join(directory, 'public');
    await fs.mkdir(publicDirectory, { recursive: true });
    const header = await fs.readFile(path.join(REPOSITORY_ROOT, 'include/obfus.h'));
    const git = await run('git', ['rev-parse', 'HEAD'], { cwd: REPOSITORY_ROOT });
    const revision = git.stdout.trim();
    assert(git.code === 0 && /^[a-f0-9]{40,64}$/.test(revision), 'A real checkout revision is required');

    const known = createHistoryIndex(history);
    const selected = [];
    for (const level of LEVELS) {
        const candidate = await selectCandidate(key, { date, round, level, directory, toolchain, known });
        remember(known, {
            structure: candidate.challenge.fingerprint, behavior: candidate.witness, shape: candidate.shape,
            level, profile: [...profile(candidate.challenge.ir)]
        });
        selected.push({ level, ...candidate });
        console.log(`${level}: verified x86/x64 and GUI`);
    }

    const result = await createBundle(key, { date, round, repository, previous, history, header, revision, selected, publicDirectory, toolchainMetadata });
    for (const file of result.files) await fs.writeFile(path.join(publicDirectory, file.name), file.data);
    assert.equal(hash(await fs.readFile(path.join(REPOSITORY_ROOT, 'include/obfus.h'))), hash(header), 'Production header changed during preparation');
    await fs.writeFile(path.join(directory, 'report.json'), JSON.stringify({ date, round, challenges: result.report, previews: result.jobs }, null, 2));
    return result;
}
