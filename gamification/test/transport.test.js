import test from 'node:test';
import assert from 'node:assert/strict';
import { GitHub } from '../src/publication/github.js';
import { run } from '../src/core/process.js';
import { fileURLToPath } from 'node:url';

test('GitHub reads retry bounded transient status, transport and response-body failures', async () => {
    for (const failure of [429, 502, 503, 504, 'network', 'body']) {
        let calls = 0;
        const waits = [];
        const api = new GitHub('example/project', 'fixture-token', {
            sleep: async ms => waits.push(ms),
            fetcher: async () => {
                calls++;
                if (calls === 1) {
                    if (failure === 'network') throw new Error('private transport details');
                    if (failure === 'body') return { ok: true, status: 200, json: async () => { throw new Error('partial body'); } };
                    return new Response('', { status: failure, headers: { 'retry-after': '1' } });
                }
                return Response.json({ id: 1 });
            }
        });
        assert.deepEqual(await api.release(), { id: 1 });
        assert.equal(calls, 2);
        assert.equal(waits.length, 1);
    }
});

test('GitHub mutations never automatically retry and read retries are capped', async () => {
    let calls = 0;
    const api = new GitHub('example/project', 'fixture-token', { sleep: async () => { }, fetcher: async () => { calls++; throw new Error('fixture-token'); } });
    await assert.rejects(api.createRelease({}), error => error.message === 'GitHub POST transport failed');
    assert.equal(calls, 1);
    await assert.rejects(api.release(), /transport failed/);
    assert.equal(calls, 4);
    calls = 0;
    const denied = new GitHub('example/project', 'fixture-token', { sleep: async () => { throw Error('unexpected wait'); }, fetcher: async () => { calls++; return new Response('', { status: 401 }); } });
    await assert.rejects(denied.release(), /401/);
    assert.equal(calls, 1);
});

test('GitHub optional absence and retry budget are explicit', async () => {
    const absent = new GitHub('example/project', 'fixture-token', { fetcher: async () => new Response('', { status: 404 }) });
    assert.equal(await absent.release(), null);
    const limited = new GitHub('example/project', 'fixture-token', { fetcher: async () => new Response('', { status: 429, headers: { 'retry-after': '120' } }) });
    await assert.rejects(limited.release(), /bounded read budget/);
    await assert.rejects(absent.request('https://example.test/token'), /Unexpected API host/);
});

test('processes cannot succeed after termination or exceeding timeout/output bounds', async () => {
    assert.equal((await run(process.execPath, ['-e', 'process.exit(7)'])).code, 7);
    const killed = await run(process.execPath, ['-e', "process.kill(process.pid, 'SIGKILL')"]).catch(error => ({ error }));
    // Windows may report a nonzero numeric status rather than a termination signal.
    if (killed.error) assert.match(killed.error.message, /without an exit code/);
    else assert.notEqual(killed.code, 0);
    await assert.rejects(run(process.execPath, ['-e', 'setInterval(()=>{},1000)'], { timeout: 150 }), /time\/output limit/);
    await assert.rejects(run(process.execPath, ['-e', 'process.stdout.write(Buffer.alloc(33*1024*1024))']), /time\/output limit/);
});

test('daily activation rejects unsafe setup before any GitHub connection', async () => {
    const cli = fileURLToPath(new URL('../cli.js', import.meta.url));
    const env = { ...process.env, CRACKME_PUBLICATION_APPROVED: 'v1', CRACKME_MASTER_KEY: '11'.repeat(32), CRACKME_TOOLCHAIN_URL: '', GITHUB_TOKEN: '', GITHUB_REPOSITORY: '' };
    for (const [args, message] of [
        [[], /Publication is disabled/],
        [['--publish'], /Repository\/token missing/],
        [['--publish', '--toolchain', 'unverified'], /checksum-verified latest archive/],
        [['--publish', '--date', '2026-10-12'], /cannot override/]
    ]) {
        const result = await run(process.execPath, [cli, 'daily', ...args], { env });
        assert.equal(result.code, 1);
        assert.match(result.stderr, message);
        assert(!result.stderr.includes(env.CRACKME_MASTER_KEY));
    }
});
