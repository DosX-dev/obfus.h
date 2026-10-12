import { spawn } from 'node:child_process';
export function run(file, args = [], { input, timeout = 120000, cwd, env } = {}) {
    return new Promise((resolve, reject) => {
        const p = spawn(file, args, { cwd, env, windowsHide: true, stdio: ['pipe', 'pipe', 'pipe'] });
        const stdout = [],
            stderr = [];
        let size = 0,
            timedOut = false;
        const stop = () => {
            if (timedOut) return;
            timedOut = true;
            if (process.platform === 'win32' && p.pid) {
                const killer = spawn('taskkill', ['/pid', String(p.pid), '/T', '/F'], { windowsHide: true, stdio: 'ignore' });
                killer.on('error', () => p.kill('SIGKILL'));
            }
            else p.kill('SIGKILL');
        };
        const timer = setTimeout(stop, timeout);
        for (const [stream, parts] of [
            [p.stdout, stdout],
            [p.stderr, stderr]
        ])
            stream.on('data', (data) => {
                size += data.length;
                if (size > 32 * 1024 * 1024) stop();
                else parts.push(data);
            });
        p.on('error', (error) => {
            clearTimeout(timer);
            reject(error);
        });
        p.on('close', (code, signal) => {
            clearTimeout(timer);
            const result = { code, stdout: Buffer.concat(stdout).toString(), stderr: Buffer.concat(stderr).toString() };
            if (timedOut) reject(new Error(`Process exceeded time/output limit: ${file}`));
            else if (code === null) reject(new Error(`Process terminated without an exit code${signal ? ' (' + signal + ')' : ''}: ${file}`));
            else resolve(result);
        });
        p.stdin.on('error', () => { });
        p.stdin.end(input);
    });
}
