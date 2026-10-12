'use strict';
const os = require('node:os');
const { AsyncResource } = require('node:async_hooks');
const GiB = 1024 ** 3;
// Current TCC fixtures stay well below this per-build budget. Keep at least
// 1 GiB / 10% of total RAM free and reevaluate admission as memory changes.
const buildMemory = 256 * 1024 ** 2;
function workerCount(cpus, free, total) {
    return Math.max(
        1,
        Math.min(Math.max(1, cpus - 2), Math.floor((free - Math.max(GiB, total * 0.1)) / buildMemory))
    );
}
class CheckPool {
    constructor(limit, limitProvider = () => workerCount(
        os.availableParallelism?.() ?? os.cpus().length, os.freemem(), os.totalmem())) {
        this.limit = limit;
        this.limitProvider = limitProvider;
        this.tasks = [];
        this.peak = 0;
    }
    add(group, action, priority = 0) {
        if (!Number.isFinite(priority)) throw new Error('invalid check priority');
        this.tasks.push({ group, action, priority });
    }
    async drain() {
        const pending = this.tasks.splice(0);
        // Prioritize whole groups, preserving FIFO for mutable-header and
        // build-result dependencies within each group. Equal priorities stay FIFO.
        const groupPriority = new Map();
        for (const task of pending)
            groupPriority.set(task.group, Math.max(groupPriority.get(task.group) ?? -Infinity, task.priority));
        pending.sort((a, b) => groupPriority.get(b.group) - groupPriority.get(a.group));
        const busy = new Set();
        let active = 0;
        await new Promise((resolve, reject) => {
            let failure;
            const pump = () => {
                const memoryLimit = this.limitProvider();
                while (active < Math.min(this.limit, memoryLimit)) {
                    const index = pending.findIndex((task) => !busy.has(task.group));
                    if (index < 0) break;
                    const task = pending.splice(index, 1)[0];
                    busy.add(task.group);
                    active++;
                    this.peak = Math.max(this.peak, active);
                    Promise.resolve()
                        .then(task.action)
                        .catch((error) => {
                            failure ??= error;
                        })
                        .finally(() => {
                            busy.delete(task.group);
                            active--;
                            pump();
                        });
                }
                if (!pending.length && !active) {
                    if (failure) reject(failure);
                    else resolve();
                }
            };
            pump();
        });
    }
}
// A check can wait for compilation or execute several fresh processes. Reserve
// resources for actual child processes rather than idle checks waiting in queues.
class ProcessPool {
    constructor(limit, limitProvider = () => limit) {
        this.limit = limit;
        this.limitProvider = limitProvider;
        this.pending = [];
        this.active = 0;
        this.peak = 0;
    }
    run(action) {
        return new Promise((resolve, reject) => {
            this.pending.push({ action: AsyncResource.bind(action), resolve, reject });
            this.pump();
        });
    }
    pump() {
        const limit = Math.max(1, Math.min(this.limit, this.limitProvider()));
        while (this.pending.length && this.active < limit) {
            const task = this.pending.shift();
            ++this.active;
            this.peak = Math.max(this.peak, this.active);
            Promise.resolve().then(task.action).then(task.resolve, task.reject).finally(() => {
                --this.active;
                this.pump();
            });
        }
    }
}
module.exports = { CheckPool, ProcessPool, workerCount };
