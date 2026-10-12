'use strict';
// Opt-in microbenchmarks; never used as timing assertions in correctness tests.
const fs = require('node:fs');
const os = require('node:os');
const path = require('node:path');
const crypto = require('node:crypto');
const { spawnSync } = require('node:child_process');
const option = name => process.argv.find(value => value.startsWith('--' + name + '='))?.split('=').slice(1).join('=');
const toolchain = path.resolve(option('tcc-dir') || process.env.TCC_DIR || 'gamification/toolchain/unpacked');
const header = path.resolve(option('header') || path.join(__dirname, '../include/obfus.h'));
const seeds = (option('seeds') || '1,2,3').split(',').map(Number);
if (seeds.some(seed => !Number.isInteger(seed) || seed < 0 || seed > 0xffffffff)) throw new Error('Invalid build seeds');
const directory = fs.mkdtempSync(path.join(os.tmpdir(), 'obfh-cost-'));
const include = path.join(directory, 'include');
fs.mkdirSync(include);
const bytes = fs.readFileSync(header);
fs.writeFileSync(path.join(include, 'obfus.h'), bytes);
const report = { headerHash: crypto.createHash('sha256').update(bytes).digest('hex'), platform: os.platform(), cpu: os.cpus()[0]?.model, measurements: [] };
function run(file, args) {
    const result = spawnSync(file, args, { encoding: 'utf8', timeout: 120000, windowsHide: true });
    if (result.error || result.status !== 0) throw result.error || new Error(result.stderr || result.stdout);
    return result.stdout;
}
for (const arch of ['x86', 'x64']) for (const advanced of [false, true]) for (const seed of seeds) {
    const compiler = path.join(toolchain, arch === 'x64' ? 'tcc.exe' : 'i386-win32-tcc.exe');
    const exe = path.join(directory, `${arch}-${advanced ? 'advanced' : 'default'}-${seed}.exe`);
    const args = ['-w', '-I', include, '-D', `OBFH_BUILD_SEED=${seed}u`];
    if (advanced) args.push('-D', 'CFLOW_V2=1', '-D', 'ANTIDEBUG_V2=1');
    const started = performance.now();
    run(compiler, [...args, path.join(__dirname, 'protection_cost.c'), '-o', exe]);
    const compileMs = performance.now() - started;
    const output = run(exe, []), measurements = output.trim().split(/\r?\n/).map(line => {
        const match = /^COST (\S+) ([\d.]+) (\d+)$/.exec(line);
        if (!match || !Number.isFinite(Number(match[2]))) throw new Error('Invalid timing output');
        return { operation: match[1], medianNs: Number(match[2]), iterations: Number(match[3]) };
    });
    report.measurements.push({ arch, advanced, seed, compileMs, exeBytes: fs.statSync(exe).size, measurements });
    console.log(`${arch}/${advanced ? 'advanced' : 'default'}/${seed}: ${measurements.map(row => `${row.operation}=${row.medianNs}ns`).join(', ')}`);
}
fs.writeFileSync(path.join(directory, 'report.json'), JSON.stringify(report, null, 4));
console.log(`Report and fixed header snapshot: ${directory}`);
