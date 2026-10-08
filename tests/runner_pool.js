'use strict';
const os = require('node:os');
const GiB = 1024 ** 3;
function workerCount(cpus, free, total) {
    return Math.max(1, Math.min(Math.max(1, cpus - 2), Math.floor((free - Math.max(GiB, total * 0.1)) / (768 * 1024 ** 2))));
}
class CheckPool {
    constructor(limit) { this.limit = limit; this.tasks = []; this.peak = 0; }
    add(group, action) { this.tasks.push({ group, action }); }
    async drain() {
        const pending = this.tasks.splice(0), busy = new Set();
        let active = 0;
        await new Promise((resolve, reject) => {
            let failure;
            const pump = () => {
                const memoryLimit = workerCount(os.availableParallelism?.() ?? os.cpus().length, os.freemem(), os.totalmem());
                while (active < Math.min(this.limit, memoryLimit)) {
                    const index = pending.findIndex(task => !busy.has(task.group));
                    if (index < 0) break;
                    const task = pending.splice(index, 1)[0];
                    busy.add(task.group); active++; this.peak = Math.max(this.peak, active);
                    Promise.resolve().then(task.action).catch(error => { failure ??= error; }).finally(() => {
                        busy.delete(task.group); active--; pump();
                    });
                }
                if (!pending.length && !active) { if (failure) reject(failure); else resolve(); }
            };
            pump();
        });
    }
}
module.exports = { CheckPool, workerCount };
