import fs from 'node:fs/promises';
import path from 'node:path';
import assert from 'node:assert/strict';
import { generate, behaviorFingerprint } from '../generation/generator.js';
import { shapeFingerprint, profile } from '../generation/diversity.js';
import { transform, words } from '../generation/model.js';
import { buildChallenge } from '../build/compiler.js';
import { verifyProbe, verifyBytes, verifyStates } from '../verification/probes.js';
import { auditPlayer } from '../verification/player.js';
import { guiChecks } from '../verification/gui.js';
import { hasSeen } from './history.js';

// A candidate is usable only after independent-model, native and GUI checks.
async function verifyCandidate(challenge, folder, toolchain) {
    assert.deepEqual(
        transform(challenge.ir, challenge.target, true),
        words(challenge.password, challenge.ir.bits)
    );

    const quick = process.env.CRACKME_FAST_VERIFICATION === '1';
    const counts = [];
    for (const arch of ['x86', 'x64']) {
        const probe = await buildChallenge(challenge, path.join(folder, arch, 'probe'), toolchain, arch);
        const text = await verifyProbe(challenge, probe, { quick });
        const byteCases = await verifyBytes(challenge, probe, { quick });
        const boundaryStates = await verifyStates(challenge, probe);
        counts.push({ arch, ...text, byteCases, boundaryStates });
    }

    const exe = await buildChallenge(challenge, path.join(folder, 'x64', 'gui'), toolchain, 'x64', { player: true });
    const audit = await auditPlayer(challenge, exe);
    await guiChecks([{ exe, password: challenge.password, screenshot: path.join(folder, 'preview.png') }], folder);
    return { exe, audit, counts };
}

export async function selectCandidate(key, { date, round, level, directory, toolchain, known }) {
    for (let attempt = 0; attempt < 3; attempt++) {
        const folder = path.join(directory, `${level}-${attempt}`);
        await fs.mkdir(folder, { recursive: true });
        try {
            const challenge = generate(key, { date, round, level, attempt });
            const witness = behaviorFingerprint(challenge.ir);
            const shape = shapeFingerprint(challenge.ir);
            if (hasSeen(known, { structure: challenge.fingerprint, behavior: witness, shape, level, profile: [...profile(challenge.ir)] })) {
                throw new Error('Candidate rejected by historical novelty or recent similarity checks');
            }

            const verified = await verifyCandidate(challenge, folder, toolchain);
            return { challenge, folder, witness, shape, ...verified };
        } catch (error) {
            await fs.writeFile(path.join(folder, 'failure.private.txt'), error.message);
            console.log(`${level}: attempt ${attempt + 1} failed verification`);
        }
    }
    throw new Error(`No verified ${level} candidate after three attempts`);
}
