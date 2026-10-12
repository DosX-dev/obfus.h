import fs from 'node:fs/promises';
import path from 'node:path';
import { randomBytes } from 'node:crypto';
import { GAMIFICATION_ROOT } from './src/core/paths.js';
import { provision } from './src/build/toolchain.js';
import { masterKey } from './src/core/crypto.js';
import { moscow } from './src/core/schedule.js';
import { prepare } from './src/round/prepare.js';
import { previousAnswers } from './src/round/answers.js';
import { loadHistory } from './src/round/history.js';
import { GitHub } from './src/publication/github.js';
import { recover, publish } from './src/publication/publication.js';
import { run } from './src/core/process.js';
const [command, ...args] = process.argv.slice(2);
const option = (name) => {
    const at = args.indexOf('--' + name);
    return at < 0 ? undefined : args[at + 1];
};
async function main() {
    if (Number(process.versions.node.split('.')[0]) !== 24) throw new Error('Use Node.js 24 LTS');
    if (command === 'test') {
        const directory = path.join(GAMIFICATION_ROOT, 'test'), files = (await fs.readdir(directory)).filter(name => name.endsWith('.test.js')).map(name => path.join(directory, name));
        const result = await run(process.execPath, ['--test', ...files], { timeout: 600000 });
        process.stdout.write(result.stdout);
        process.stderr.write(result.stderr);
        process.exitCode = result.code || 0;
        return;
    }
    if (process.platform !== 'win32') throw new Error('Build/GUI acceptance requires Windows');
    if (command === 'acceptance') {
        const { acceptance } = await import('./src/verification/acceptance.js');
        return acceptance((await provision(option('toolchain'))).directory, Number(option('count') || 10));
    }
    if (!['preview', 'daily'].includes(command))
        throw new Error('Usage: node cli.js preview|test|daily|acceptance [--toolchain DIR] [--date YYYY-MM-DD]');
    const time = moscow();
    let previous = null,
        state = null,
        api;
    if (command === 'daily') {
        // Both barriers are deliberate. Neither running tests nor pushing code enables publishing.
        if (!args.includes('--publish') || process.env.CRACKME_PUBLICATION_APPROVED !== 'v1')
            throw new Error('Publication is disabled pending owner approval of GUI and generator');
        if (option('date')) throw new Error('Daily publication cannot override the Moscow date');
        if (option('toolchain')) throw new Error('Daily publication requires the checksum-verified latest archive');
        const key = masterKey(process.env.CRACKME_MASTER_KEY);
        api = new GitHub(process.env.GITHUB_REPOSITORY, process.env.GITHUB_TOKEN);
        state = await recover(api);
        if (state?.date === time.date || time.hour < 13) {
            console.log('No new round due');
            return;
        }
        previous = await previousAnswers(api, key, state);
    }
    const key =
        command === 'daily'
            ? masterKey(process.env.CRACKME_MASTER_KEY)
            : process.env.CRACKME_MASTER_KEY
                ? masterKey(process.env.CRACKME_MASTER_KEY)
                : randomBytes(32);
    const date = option('date') || time.date;
    const directory = path.resolve(option('out') || path.join(GAMIFICATION_ROOT, 'out', `${command}-${date}-${Date.now()}`));
    if (command === 'preview') {
        // Retain local replay material even when preparation fails. Never used by daily mode.
        await fs.mkdir(directory, { recursive: true });
        await fs.writeFile(path.join(directory, 'replay-key.private.txt'), key.toString('hex'));
    }
    const toolchain = await provision(option('toolchain'));
    const result = await prepare(key, {
        date,
        round: state ? state.round + 1 : 1,
        directory,
        toolchain: toolchain.directory,
        toolchainMetadata: toolchain.metadata,
        previous,
        history: state ? await loadHistory(api, state) : [],
        repository: process.env.GITHUB_REPOSITORY || 'DosX-dev/obfus.h'
    });
    if (command === 'daily') await publish(api, result.state, result.files);
    else {
        console.log(`Local preview ready: ${directory}`);
    }
}
main().catch((error) => {
    console.error(error.message);
    process.exitCode = 1;
});
