'use strict';
const assert = require('node:assert/strict');
const { CheckPool, ProcessPool, workerCount } = require('./runner_pool');
const { AsyncLocalStorage } = require('node:async_hooks');
async function verify() {
    assert.equal(workerCount(32, 35 * 1024 ** 3, 64 * 1024 ** 3), 30);
    assert.equal(workerCount(8, 2 * 1024 ** 3, 8 * 1024 ** 3), 4);
    assert.equal(workerCount(8, 1024 ** 3, 8 * 1024 ** 3), 1);
    const pool = new CheckPool(3),
        events = [],
        activeGroups = new Set();
    let active = 0,
        peak = 0;
    for (const [group, id] of [
        ['a', 0],
        ['a', 1],
        ['b', 2],
        ['c', 3]
    ])
        pool.add(group, async () => {
            assert(!activeGroups.has(group));
            activeGroups.add(group);
            peak = Math.max(peak, ++active);
            events.push(id);
            await new Promise((resolve) => setTimeout(resolve, 20));
            active--;
            activeGroups.delete(group);
            if (id === 2) throw new Error('expected failure');
        });
    await assert.rejects(pool.drain(), /expected failure/);
    // The live RAM limiter may serialize a three-worker pool under memory pressure.
    assert(peak >= 1 && peak <= 3);
    assert(events.indexOf(0) < events.indexOf(1));
    assert.equal(events.length, 4);
    assert.equal(active, 0);
    pool.add('a', async () => events.push(4));
    await pool.drain();
    assert.equal(events.at(-1), 4);
    const prioritized = new CheckPool(1), starts = [];
    prioritized.add('shared', async () => starts.push('shared-first'), 0);
    prioritized.add('other', async () => starts.push('other'), 5);
    prioritized.add('shared', async () => starts.push('shared-second'), 10);
    prioritized.add('equal-first', async () => starts.push('equal-first'), 5);
    prioritized.add('equal-second', async () => starts.push('equal-second'), 5);
    await prioritized.drain();
    assert.deepEqual(starts, ['shared-first', 'shared-second', 'other', 'equal-first', 'equal-second']);
    assert.throws(() => prioritized.add('invalid', async () => { }, NaN), /invalid check priority/);
    const context = new AsyncLocalStorage();
    let processLimit = 2, running = 0;
    const processes = new ProcessPool(3, () => processLimit), processStarts = [];
    const tasks = Array.from({ length: 8 }, (_, id) => context.run(id, () => processes.run(async () => {
        assert.equal(context.getStore(), id, 'queued child lost its task context');
        assert(++running <= 2, 'child process concurrency exceeded its limit');
        processStarts.push(id);
        if (id === 0) processLimit = 1;
        try {
            await new Promise((resolve) => setTimeout(resolve, 5));
            assert.equal(context.getStore(), id);
            if (id === 2) throw new Error('child failed');
            if (id === 4) processLimit = 2;
            return id;
        } finally {
            --running;
        }
    })));
    const outcomes = await Promise.allSettled(tasks);
    await new Promise((resolve) => setImmediate(resolve));
    assert.deepEqual(processStarts, [0, 1, 2, 3, 4, 5, 6, 7]);
    assert.equal(outcomes.filter((outcome) => outcome.status === 'rejected').length, 1);
    assert.match(outcomes[2].reason.message, /child failed/);
    assert.equal(processes.active, 0);
    assert.equal(processes.pending.length, 0);
    assert.equal(processes.peak, 2);
    assert.equal(await processes.run(() => 42), 42, 'failed pool was not reusable');
    const serial = new ProcessPool(1);
    await Promise.all(Array.from({ length: 3 }, () => serial.run(async () => {
        await new Promise((resolve) => setTimeout(resolve, 5));
    })));
    assert.equal(serial.peak, 1);
}
if (require.main === module)
    verify()
        .then(() => console.log('POOL_PASS'))
        .catch((error) => {
            console.error(error);
            process.exitCode = 1;
        });
module.exports = { verify };
