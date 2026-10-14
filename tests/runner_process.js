'use strict';
const fs = require('node:fs');
const path = require('node:path');
const { spawn, spawnSync } = require('node:child_process');

// Pool admission stays with the caller; deadlines start only after admission.
exports.createProcessRunner = function ({ root, suiteDeadline, testTimeout, taskContext, isParallel, getArtifactDirectory }) {
    let childCount = 0;
    const activeChildren = new Set();
    function killTree(pid) {
        if (process.platform === 'win32') {
            const killed = spawnSync('taskkill.exe', ['/PID', String(pid), '/T', '/F'], {
                windowsHide: true,
                encoding: 'utf8',
                timeout: 5000
            });
            if (killed.error || killed.status !== 0) {
                try {
                    process.kill(pid, 'SIGKILL');
                } catch { }
            }
        } else {
            try {
                process.kill(pid, 'SIGKILL');
            } catch { }
        }
    }
    process.once('SIGINT', async () => {
        for (const pid of activeChildren) killTree(pid);
        process.exit(130);
    });
    process.once('exit', async () => {
        for (const pid of activeChildren) killTree(pid);
    });
    async function runChild(command, args, options = {}) {
        const remaining = suiteDeadline - Date.now();
        if (remaining <= 0) {
            const error = new Error('suite timeout exceeded');
            error.suiteDeadline = true;
            throw error;
        }
        const timeout = Math.min(options.timeout ?? testTimeout, remaining);
        const childId = ++childCount;
        return new Promise((resolve, reject) => {
            const child = spawn(command, args, {
                cwd: taskContext.getStore()?.cwd ?? root,
                windowsHide: true,
                stdio: ['pipe', 'pipe', 'pipe']
            });
            if (child.pid) activeChildren.add(child.pid);
            const started = Date.now();
            let stdout = '',
                stderr = '',
                failure,
                phase = '';
            const collect = (channel, data) => {
                if (channel === 'stdout') stdout += data.toString();
                else stderr += data.toString();
                const found = data.toString().match(/TEST_PHASE:([^\r\n]+)/g);
                if (found) phase = found[found.length - 1].slice(11).trim();
                if (stdout.length + stderr.length > 8 * 1024 * 1024 && !failure) {
                    failure = new Error('output limit exceeded: ' + path.basename(command));
                    killTree(child.pid);
                }
            };
            child.stdout.on('data', (data) => collect('stdout', data));
            child.stderr.on('data', (data) => collect('stderr', data));
            child.stdin.on('error', () => { });
            child.stdin.end(options.input ?? '');
            const progress = isParallel()
                ? undefined
                : setInterval(
                    async () =>
                        console.log(
                            `RUN ${path.basename(command)}: ${Math.round((Date.now() - started) / 1000)}s / ${Math.round(timeout / 1000)}s${phase ? ' [' + phase + ']' : ''}`
                        ),
                    5000
                );
            const timer = setTimeout(async () => {
                failure = new Error(`timeout ${timeout}ms: ${path.basename(command)}${phase ? ' [' + phase + ']' : ''}`);
                failure.code = 'ETIMEDOUT';
                failure.suiteDeadline = remaining <= (options.timeout ?? testTimeout);
                killTree(child.pid);
            }, timeout);
            const finish = () => {
                clearTimeout(timer);
                clearInterval(progress);
                activeChildren.delete(child.pid);
            };
            child.once('error', (error) => {
                finish();
                reject(error);
            });
            child.once('close', (status, signal) => {
                finish();
                const artifactDirectory = getArtifactDirectory();
                if (artifactDirectory)
                    fs.writeFileSync(
                        path.join(artifactDirectory, childId + '-' + path.basename(command) + '-' + child.pid + '.json'),
                        JSON.stringify(
                            {
                                command,
                                childId,
                                check: taskContext.getStore()?.name,
                                args,
                                status,
                                signal,
                                elapsedMs: Date.now() - started,
                                startedAt: started,
                                stdout,
                                stderr,
                                error: failure?.message
                            },
                            null,
                            2
                        )
                    );
                if (failure) {
                    failure.stdout = stdout;
                    failure.stderr = stderr;
                    reject(failure);
                } else resolve({ status, signal, stdout, stderr, elapsedMs: Date.now() - started });
            });
        });
    }

    return { runChild, activeChildren };
};
