import fs from 'node:fs/promises';
import path from 'node:path';
import assert from 'node:assert/strict';
import { randomBytes } from 'node:crypto';
import { LEVELS } from '../core/config.js';
import { generate, behaviorFingerprint } from '../generation/generator.js';
import { hash } from '../core/crypto.js';
import { GAMIFICATION_ROOT, REPOSITORY_ROOT } from '../core/paths.js';
import { buildChallenge } from '../build/compiler.js';
import { verifyProbe, verifyBytes, verifyStates } from './probes.js';
import { auditPlayer } from './player.js';
import { guiChecks } from './gui.js';
import { diversityReport, shapeFingerprint } from '../generation/diversity.js';
import { assertDifficulty } from '../generation/difficulty.js';

export async function acceptance(toolchain, count = 10) {
    if (!Number.isInteger(count) || count < 10 || count > 100) throw new Error('Acceptance requires 10–100 candidates per level');
    const directory = path.join(GAMIFICATION_ROOT, 'out', 'acceptance-' + Date.now());
    await fs.mkdir(directory, { recursive: true });
    const key = randomBytes(32), headerHash = hash(await fs.readFile(path.join(REPOSITORY_ROOT, 'include/obfus.h')));
    await fs.writeFile(path.join(directory, 'replay-key.private.txt'), key.toString('hex'));
    const queue = [], records = [], jobs = [], graphs = new Set(), behaviors = new Set(), shapes = new Set(), challenges = [];
    for (const level of LEVELS) for (let round = 1; round <= count; round++) {
        const challenge = generate(key, { date: '2026-10-12', round, level });
        assert(!graphs.has(challenge.fingerprint));
        graphs.add(challenge.fingerprint);
        const behavior = behaviorFingerprint(challenge.ir), shape = shapeFingerprint(challenge.ir);
        assert(!behaviors.has(behavior), 'Repeated verification behavior');
        assert(!shapes.has(shape), 'Repeated normalized expression shapes');
        behaviors.add(behavior);
        shapes.add(shape);
        challenges.push(challenge);
        for (const arch of ['x86', 'x64']) queue.push({ challenge, arch, behavior, shape });
    }
    async function worker() {
        while (queue.length) {
            const { challenge, arch, behavior, shape } = queue.shift(), { level, round } = challenge.ir;
            const folder = path.join(directory, level, String(round), arch);
            const probe = await buildChallenge(challenge, path.join(folder, 'probe'), toolchain, arch);
            const checks = await verifyProbe(challenge, probe);
            const byteCases = await verifyBytes(challenge, probe);
            const boundaryStates = await verifyStates(challenge, probe);
            if (round === 1) {
                for (const mutate of level === 'hard' ? ['drop', 'vm'] : ['drop']) {
                    const mutant = await buildChallenge(challenge, path.join(folder, mutate), toolchain, arch, { mutate });
                    await verifyProbe(challenge, mutant, { mutation: true });
                }
                const mutant = await buildChallenge(challenge, path.join(folder, 'coordinate'), toolchain, arch, { mutate: 'coordinate' });
                await verifyStates(challenge, mutant, { coordinateMutation: true });
            }
            const exe = await buildChallenge(challenge, path.join(folder, 'gui'), toolchain, arch, { player: true });
            const binary = await auditPlayer(challenge, exe);
            jobs.push({ exe, password: challenge.password });
            records.push({ level, round, arch, fingerprint: challenge.fingerprint, behavior, shape, difficulty: assertDifficulty(challenge.ir), ...checks, byteCases, boundaryStates, ...binary });
            await fs.writeFile(path.join(folder, 'verification.json'), JSON.stringify(records.at(-1), null, 4));
            console.log(`${level}/${round}/${arch}: probe, bytes, traces and player audit PASS (${records.length}/${count * 6})`);
        }
    }
    // Bounded workers avoid excessive simultaneous TCC/VM workloads. Clipboard tests are serialized.
    await Promise.all([worker(), worker()]);
    for (let i = 0; i < jobs.length; i += 6) {
        await guiChecks(jobs.slice(i, i + 6), directory);
        console.log(`GUI acceptance PASS: ${Math.min(i + 6, jobs.length)}/${jobs.length}`);
    }
    assert.equal(hash(await fs.readFile(path.join(REPOSITORY_ROOT, 'include/obfus.h'))), headerHash);
    const diversity = diversityReport(challenges);
    for (const level of LEVELS) assert(diversity[level].minimum > 0.35, 'Insufficient normalized update variation');
    const report = { version: 1, headerHash, count, builds: records.length, guiChecks: jobs.length, byteCases: records.reduce((sum, row) => sum + row.byteCases, 0), textCases: records.reduce((sum, row) => sum + row.cases, 0), witnessTracesVerified: true, diversity, records };
    await fs.writeFile(path.join(directory, 'report.json'), JSON.stringify(report, null, 4));
    console.log(`Acceptance completed locally: ${directory}`);
    return report;
}
