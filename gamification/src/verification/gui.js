import fs from 'node:fs/promises';
import path from 'node:path';
import { run } from '../core/process.js';
import { GAMIFICATION_ROOT } from '../core/paths.js';

export async function guiChecks(jobs, directory) {
    const fixture = path.join(directory, 'gui.private.json');
    await fs.writeFile(fixture, JSON.stringify(jobs));
    try {
        const result = await run(
            'powershell.exe',
            [
                '-NoProfile',
                '-STA',
                '-ExecutionPolicy',
                'Bypass',
                '-File',
                path.join(GAMIFICATION_ROOT, 'src/ui/gui-check.ps1'),
                '-Fixture',
                fixture
            ],
            { timeout: 180000 }
        );
        if (result.code !== 0 || result.stdout.match(/GUI_PASS/g)?.length !== jobs.length) {
            await fs.writeFile(path.join(directory, 'gui.private.log'), result.stderr);
            throw new Error('GUI verification failed; private diagnostic retained');
        }
        await fs.rm(path.join(directory, 'gui.private.log'), { force: true });
    } finally {
        await fs.rm(fixture, { force: true });
    }
}
