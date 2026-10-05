'use strict';
const fs = require('node:fs');
const os = require('node:os');
const path = require('node:path');
const { spawn, spawnSync } = require('node:child_process');
const breakRandom = require('./break_random');
const stackProxy = require('./stack_proxy');
const pdataDecoys = require('./pdata_decoys');
const root = path.resolve(__dirname, '..');
const source = fs.readFileSync(path.join(root, 'include', 'obfus.h'), 'utf8');
const selectedArch = process.argv.find(value => value.startsWith('--arch='))?.slice(7);
const selectedConfig = process.argv.find(value => value.startsWith('--config='))?.slice(9);
const results = [];
let artifactDirectory;
const configs = {
    plain: ['NO_OBF=1'],
    default: [],
    'vm-no-cflow': ['VIRT=1', 'NO_CFLOW=1', 'NO_ANTIDEBUG=1'],
    vm: ['VIRT=1'],
    advanced: ['VIRT=1', 'CFLOW_V2=1', 'ANTIDEBUG_V2=1', 'FAKE_SIGNS=1'],
    'math-vm': ['VIRT=1', 'virt_std=1', 'NO_ANTIDEBUG=1'],
};
function assert(condition, message) { if (!condition) throw new Error(message); }
async function check(name, action) {
    const started = Date.now();
    try { await action(); const durationMs = Date.now() - started; results.push({ name, pass: true, durationMs }); console.log(`PASS ${name}${durationMs >= 1000 ? ` (${(durationMs / 1000).toFixed(1)}s)` : ""}`); return true; }
    catch (error) { results.push({ name, pass: false, durationMs: Date.now() - started, error: error.message }); console.error(`FAIL ${name}: ${error.message}`); if (error.suiteDeadline) throw error; return false; }
}
function timeoutOption(name, fallback) {
    const value = process.argv.find(arg => arg.startsWith('--' + name + '='));
    const milliseconds = value ? Number(value.split('=')[1]) : fallback;
    assert(Number.isSafeInteger(milliseconds) && milliseconds >= 100, 'invalid timeout: ' + name);
    return milliseconds;
}
const testTimeout = timeoutOption('test-timeout-ms', 30000);
const buildTimeout = timeoutOption('build-timeout-ms', 15000);
const suiteDeadline = Date.now() + timeoutOption('suite-timeout-ms', 600000);
const activeChildren = new Set();
function killTree(pid) {
    if (process.platform === 'win32') {
        const killed = spawnSync('taskkill.exe', ['/PID', String(pid), '/T', '/F'], { windowsHide: true, encoding: 'utf8', timeout: 5000 });
        if (killed.error || killed.status !== 0) { try { process.kill(pid, 'SIGKILL'); } catch { } }
    } else { try { process.kill(pid, 'SIGKILL'); } catch { } }
}
process.once('SIGINT', async () => { for (const pid of activeChildren) killTree(pid); process.exit(130); });
process.once('exit', async () => { for (const pid of activeChildren) killTree(pid); });
async function run(command, args, options = {}) {
    const remaining = suiteDeadline - Date.now();
    if (remaining <= 0) { const error = new Error('suite timeout exceeded'); error.suiteDeadline = true; throw error; }
    const timeout = Math.min(options.timeout ?? testTimeout, remaining);
    return new Promise((resolve, reject) => {
        const child = spawn(command, args, { cwd: root, windowsHide: true, stdio: ['pipe', 'pipe', 'pipe'] });
        if (child.pid) activeChildren.add(child.pid);
        const started = Date.now();
        let stdout = '', stderr = '', failure, phase = '';
        const collect = (channel, data) => {
            if (channel === 'stdout') stdout += data.toString(); else stderr += data.toString();
            const found = data.toString().match(/TEST_PHASE:([^\r\n]+)/g);
            if (found) phase = found[found.length - 1].slice(11).trim();
            if (stdout.length + stderr.length > 8 * 1024 * 1024 && !failure) {
                failure = new Error('output limit exceeded: ' + path.basename(command)); killTree(child.pid);
            }
        };
        child.stdout.on('data', data => collect('stdout', data));
        child.stderr.on('data', data => collect('stderr', data));
        child.stdin.on('error', () => { });
        child.stdin.end(options.input ?? '');
        const progress = setInterval(async () => console.log(`RUN ${path.basename(command)}: ${Math.round((Date.now() - started) / 1000)}s / ${Math.round(timeout / 1000)}s${phase ? ' [' + phase + ']' : ''}`), 5000);
        const timer = setTimeout(async () => {
            failure = new Error(`timeout ${timeout}ms: ${path.basename(command)}${phase ? ' [' + phase + ']' : ''}`);
            failure.code = 'ETIMEDOUT'; failure.suiteDeadline = remaining <= (options.timeout ?? testTimeout);
            killTree(child.pid);
        }, timeout);
        const finish = () => { clearTimeout(timer); clearInterval(progress); activeChildren.delete(child.pid); };
        child.once('error', error => { finish(); reject(error); });
        child.once('close', (status, signal) => {
            finish();
            if (artifactDirectory) fs.writeFileSync(path.join(artifactDirectory, path.basename(command) + '-' + child.pid + '.json'), JSON.stringify({ command, args, status, signal, elapsedMs: Date.now() - started, stdout, stderr, error: failure?.message }, null, 2));
            if (failure) { failure.stdout = stdout; failure.stderr = stderr; reject(failure); }
            else resolve({ status, signal, stdout, stderr, elapsedMs: Date.now() - started });
        });
    });
}
async function discover() {
    const argument = process.argv.find(value => value.startsWith('--tcc-dir='));
    const desktop = process.platform === 'win32' ? (await run('powershell.exe', ['-NoProfile', '-Command', '[Console]::OutputEncoding = [Text.UTF8Encoding]::new(); [Environment]::GetFolderPath("Desktop")'])).stdout.trim() : '';
    const candidates = [argument?.slice(10), process.env.TCC_DIR, desktop && path.join(desktop, 'tcc', 'tcc'), 'C:\\tcc'].filter(Boolean);
    for (const directory of candidates) {
        const x64 = path.join(directory, 'tcc.exe'), x86 = path.join(directory, 'i386-win32-tcc.exe');
        if (fs.existsSync(x64) && fs.existsSync(x86)) return { x64, x86 };
    }
    throw new Error('Both tcc.exe and i386-win32-tcc.exe are required. Set TCC_DIR or pass --tcc-dir=C:\\path\\to\\tcc.');
}
async function compile(compiler, directory, name, file, flags, extra = []) {
    const output = path.join(directory, name);
    const result = await run(compiler, ['-w', ...flags.map(flag => '-D' + flag), file, '-o', output, ...extra], { timeout: buildTimeout });
    assert(result.status === 0, `compile ${path.basename(file)} exit=${result.status}: ${result.stderr || result.stdout}`);
    assert(fs.existsSync(output), 'compiler did not produce output');
    return output;
}
async function execute(exe, marker, args = []) {
    const result = await run(exe, args);
    assert(result.status === 0, `exit=${result.status}, stderr=${result.stderr}, stdout=${result.stdout.slice(-700)}`);
    assert(result.stdout.includes(marker), `missing completion marker ${marker}`);
    return result;
}
function pe(binary) {
    const nt = binary.readUInt32LE(0x3c), optional = nt + 24;
    const count = binary.readUInt16LE(nt + 6), optionalSize = binary.readUInt16LE(nt + 20);
    const sections = [];
    for (let i = 0; i < count; i++) {
        const offset = optional + optionalSize + i * 40;
        sections.push({ executable: !!(binary.readUInt32LE(offset + 36) & 0x20000000), name: binary.subarray(offset, offset + 8).toString().replace(/\0.*$/, ''), virtualAddress: binary.readUInt32LE(offset + 12), rawOffset: binary.readUInt32LE(offset + 20), rawSize: binary.readUInt32LE(offset + 16), bytes: binary.subarray(binary.readUInt32LE(offset + 20), binary.readUInt32LE(offset + 20) + binary.readUInt32LE(offset + 16)) });
    }
    return { machine: binary.readUInt16LE(nt + 4), sections };
}
function assertNoConstantSignature(binary) {
    for (const signature of [Buffer.from('abcdefghijklmnopqrstuvwxyz'), Buffer.from('a\0b\0c\0d\0e\0f\0g\0h\0'), Buffer.from('SLAIDP'), Buffer.from([0, 1, 2, 3, 4, 5, 6, 7, 8, 9])])
        assert(!binary.includes(signature), 'constant data signature survives: ' + signature.toString('hex'));
}
function withoutSkippedPayload(code, branch) {
    assert(branch >= 0 && branch + 6 <= code.length, 'junk skip branch missing');
    const end = branch + 6, destination = end + code.readInt32LE(branch + 2);
    assert(destination >= end && destination < code.length, 'junk skip target outside site');
    return { destination, liveBytes: Buffer.concat([code.subarray(0, end), code.subarray(destination)]) };
}
function junkCode(binary, siteCount = 13) {
    const { sections } = pe(binary);
    const offset = rva => {
        const section = sections.find(s => rva >= s.virtualAddress && rva - s.virtualAddress < s.rawSize);
        assert(section, 'export RVA is outside the binary');
        return section.rawOffset + rva - section.virtualAddress;
    };
    const optional = binary.readUInt32LE(0x3c) + 24;
    const exports = offset(binary.readUInt32LE(optional + (binary.readUInt16LE(optional) === 0x20b ? 112 : 96)));
    const count = binary.readUInt32LE(exports + 24), functions = offset(binary.readUInt32LE(exports + 28));
    const names = offset(binary.readUInt32LE(exports + 32)), ordinals = offset(binary.readUInt32LE(exports + 36));
    const entries = new Map();
    for (let i = 0; i < count; ++i) {
        const start = offset(binary.readUInt32LE(names + i * 4));
        const name = binary.subarray(start, binary.indexOf(0, start)).toString();
        entries.set(name, binary.readUInt32LE(functions + binary.readUInt16LE(ordinals + i * 2) * 4));
    }
    return Array.from({ length: siteCount }, (_, i) => {
        const start = entries.get('junk_site_' + i), end = entries.get(i === siteCount - 1 ? 'junk_anchor' : 'junk_site_' + (i + 1));
        assert(start && end > start && end - start < 16384, 'invalid junk function boundaries');
        return binary.subarray(offset(start), offset(end));
    });
}
async function main() {
    let directory;
    try {
        assert(process.platform === 'win32', 'This suite exercises real Windows x86/x64 executables.');
        const compilers = await discover();
        assert(!selectedArch || ['x64', 'x86'].includes(selectedArch), 'unknown architecture');
        assert(!selectedConfig || Object.hasOwn(configs, selectedConfig), 'unknown configuration');
        directory = fs.mkdtempSync(path.join(os.tmpdir(), 'obfh-js-suite-'));
        artifactDirectory = directory;
        console.log(`Artifacts: ${directory}`);
        await check('binary scanner: skipped CPUID bytes versus live CPUID', async () => {
            const code = Buffer.from([0x0f, 0x84, 2, 0, 0, 0, 0x0f, 0xa2, 0x90]);
            assert(!withoutSkippedPayload(code, 0).liveBytes.includes(Buffer.from([0x0f, 0xa2])), 'skipped payload counted as live code');
            assert(withoutSkippedPayload(Buffer.concat([code, Buffer.from([0x0f, 0xa2])]), 0).liveBytes.includes(Buffer.from([0x0f, 0xa2])), 'live CPUID was missed');
        });
        await check('source: contiguous numbered pool with helpers above it', async () => {
            const pool = source.slice(source.indexOf('#define BREAK_STACK_CFLOW_0 '), source.indexOf('#define BREAK_STACK_CFLOW OBFH_CFLOW_EMIT'));
            const definitions = [...pool.matchAll(/^#define (\w+)/gm)].map(match => match[1]);
            assert(definitions.length === 128 && definitions.every((name, index) => name === 'BREAK_STACK_CFLOW_' + index), 'pool contains intervening helpers or missing/out-of-order variants');
        });
        await check('source: 128 proxy variants with distinct guard/layout pairs', async () => {
            const variants = [...source.matchAll(/^#define OBFH_SF_VARIANT_(\d+) OBFH_SF_ASM\(OBFH_SF_GUARD_(\d+), OBFH_SF_LAYOUT_(\d+)\(/gm)];
            assert(variants.length === stackProxy.variantCount && variants.every((v, i) => Number(v[1]) === i), 'proxy variants missing or out of order');
            assert(new Set(variants.map(v => v[2] + ':' + v[3])).size === variants.length, 'duplicate proxy guard/layout pair');
            assert(source.includes('#define OBFH_SF_VARIANT_COUNT ' + stackProxy.variantCount + 'u'), 'proxy count and test disagree');
        });
        await check('source: protected VM macros still call the interpreter', async () => {
            const start = source.indexOf('#define VM_ADD(', source.indexOf('#define _ENC_OP__NOP'));
            const block = source.slice(start, source.indexOf('#define VM_IF', start));
            const macros = block.split(/\r?\n/).filter(line => line.startsWith('#define VM_'));
            assert(macros.length === 19, `expected 19 arithmetic/identity macros, found ${macros.length}`);
            for (const macro of macros) assert(macro.includes('Obfh_VirtualMachine('), `VM bypass: ${macro}`);
        });
        await check('source: custom exports and generated masks preserved', async () => {
            const begin = source.indexOf('FARPROC obfh_find_export('), end = source.indexOf('#define GetProcAddress', begin);
            assert(begin >= 0 && source.slice(begin, end).includes('AddressOfFunctions'), 'custom export parser missing');
            assert(!/\bGetProcAddress\(/.test(source.slice(begin, end)), 'custom parser delegates to native GetProcAddress');
            const maskBegin = source.indexOf('char *getCharMask('), maskEnd = source.indexOf('// WriteConsoleA', maskBegin);
            assert(maskBegin >= 0 && !source.slice(maskBegin, maskEnd).includes('"%c'), 'literal mask table present');
            const hidden = source.slice(source.lastIndexOf('#define HIDE_STRING'), source.indexOf('typedef enum', source.lastIndexOf('#define HIDE_STRING')));
            assert(hidden.includes('== RND('), 'HIDE_STRING false-branch obfuscation lost');
        });
        await check('source: automatic protection paths and type conversions', async () => {
            const identity = source.split(/\r?\n/).find(line => line.startsWith('#define VM_OBF_DBL(num1)'));
            assert(identity && identity.includes('(long double)(num1)') && !identity.includes('(double)(num1)'), 'VM identity narrows its operand');
            assert(source.includes('GetConsoleMode(console, &mode) && !obfh_format_has_count(format)'), 'count conversion enters the console sizing pass');
            assert(source.includes('float junk, float condition'), 'condition float conversion removed');
            assert(!source.includes('_0 && rdtsc_result'), 'timestamp proxy short-circuited');
            assert(source.includes('0x0f, 0x01, 0xf9'), 'actual RDTSCP missing');
            assert(source.includes('MEM_CLEANER__JUST_FOR_FUN'), 'working-set feature removed');
            assert(!source.includes('FARPROC fallback') && !source.includes('(FARPROC)(vprintf)'), 'native CRT fallback leaked');
            const resolver = source.slice(source.indexOf('FARPROC obfh_crt_resolve'), source.indexOf('// printf', source.indexOf('FARPROC obfh_crt_resolve')));
            assert(resolver.includes('LoadLibraryA_proxy('), 'CRT loader chain bypassed');
            assert(source.includes('return value < (int)FALSE ? -value : value;'), 'custom abs replaced');
        });
        await check('source: every library break uses the unified selector', async () => {
            assert(!/\bBREAK_STACK_\d+\b/.test(source), 'old numbered public macro remains');
            const callsStart = source.indexOf('#define BAD_JMP');
            assert(callsStart > 0, 'library call-site boundary missing');
            const calls = source.slice(callsStart);
            assert(!/\bBREAK_STACK_CFLOW_\d+\b/.test(calls), 'library pins a numbered template');
            assert(source.includes('#define BREAK_STACK_CFLOW OBFH_CFLOW_EMIT(__COUNTER__, OBFH_CFLOW_EXTRA)'), 'public macro does not capture a fresh counter');
        });
        await check('runner: timeout terminates the whole process tree', async () => {
            const script = "const {spawn}=require('node:child_process'); const child=spawn(process.execPath,['-e','setInterval(()=>{},1000)'],{windowsHide:true,stdio:'ignore'}); console.log('TIMEOUT_CHILD:'+child.pid); console.error('TEST_PHASE: watchdog control'); setInterval(()=>{},1000);";
            let timedOut;
            try { await run(process.execPath, ['-e', script], { timeout: 1000 }); }
            catch (error) { timedOut = error; }
            assert(timedOut?.code === 'ETIMEDOUT', 'hanging process did not time out');
            assert(timedOut.message.includes('watchdog control'), 'timeout did not report the last phase');
            const childPid = Number(timedOut.stdout.match(/TIMEOUT_CHILD:(\d+)/)?.[1]);
            assert(childPid > 0, 'timeout child did not start');
            let alive = false; try { process.kill(childPid, 0); alive = true; } catch { }
            assert(!alive, 'timeout left a child process alive');
        });
        const setup = source.slice(source.indexOf('static DWORD WINAPI obfh_ad_register_worker'), source.indexOf('static int obfh_ad_process_probe'));
        assert(setup.includes('DuplicateHandle') && /#endif\s*$/.test(setup), 'failure-test extraction missing');
        const failureFile = path.join(directory, 'failures.c');
        const processProbe = source.slice(source.indexOf('static int obfh_ad_process_probe'), source.indexOf('static int IsDebuggerPresent_proxy(void)'));
        assert(processProbe.includes('gs:0x60') && processProbe.includes('fs:0x30'), 'process-probe extraction missing');
        fs.writeFileSync(failureFile, fs.readFileSync(path.join(__dirname, 'failures.c.in'), 'utf8')
            .replace('/* REGISTER_PROBES */', '#if ANTIDEBUG_V2 == 1\n' + setup).replace('/* PROCESS_PROBE */', processProbe));
        for (const [arch, compiler] of Object.entries(compilers)) {
            if (selectedArch && selectedArch !== arch) continue;
            console.log(`${arch}: ${(await run(compiler, ['-v'])).stdout.trim()}`);
            if (process.argv.includes('--only-pdata') || process.argv.includes('--only-integration') || !process.argv.some(arg => arg.startsWith('--only-'))) {
                await pdataDecoys.runSuite({ arch, compiler, directory, source, check, compile, execute, run, assert });
                if (process.argv.includes('--only-pdata')) continue;
            }
            if (!process.argv.includes('--only-integration')) {
                const proxyRoot = path.join(directory, arch + '-stack-proxy');
                fs.mkdirSync(path.join(proxyRoot, 'include'), { recursive: true });
                fs.mkdirSync(path.join(proxyRoot, 'tests'), { recursive: true });
                const proxyHeader = path.join(proxyRoot, 'include', 'obfus.h');
                const proxyFile = path.join(proxyRoot, 'tests', 'stack_proxy.c');
                fs.writeFileSync(proxyFile, stackProxy.fixture());
                const proxyBuilds = new Map(), proxyHistograms = [];
                for (const seed of [0, 1, 2, 3735928559, 4294967295]) await check(`${arch}/stack proxies/all ${stackProxy.variantCount} variants + arbitrary guards/seed ${seed}`, async () => {
                    const traced = source.replace('OBFH_SF_SELECT(__obfh_sf_variant);', 'obfh_test_sf_visit(__obfh_sf_id, __obfh_sf_variant, __LINE__); OBFH_SF_SELECT(__obfh_sf_variant);')
                        .replaceAll('#define OBFH_SF_INPUT "movl %%esp, %%eax;"', '#define OBFH_SF_INPUT "movl %[sf_input], %%eax;"')
                        .replace(/#define OBFH_SF_INPUTS\s*\\\r?\n/, '#define OBFH_SF_INPUTS \\\n    [sf_input] "m"(guard_input), \\\n');
                    assert(traced !== source && traced.includes('[sf_input] "m"(guard_input)'), 'proxy instrumentation target missing');
                    fs.writeFileSync(proxyHeader, traced);
                    const flags = ['NO_CFLOW=1', 'NO_ANTIDEBUG=1', `OBFH_BUILD_SEED=${seed}u`];
                    const output = await execute(await compile(compiler, directory, `${arch}-stack-proxy-${seed}.exe`, proxyFile, flags), 'STACK_PROXY_PASS');
                    proxyHistograms.push(stackProxy.measure(output.stdout, seed, assert).histogram);
                    fs.writeFileSync(proxyHeader, source);
                    const dll = await compile(compiler, directory, `${arch}-stack-proxy-${seed}.dll`, proxyFile, flags, ['-shared']);
                    proxyBuilds.set(seed, junkCode(fs.readFileSync(dll), stackProxy.count));
                });
                await check(`${arch}/stack proxies/distribution + seed changes + reproducibility`, async () => {
                    assert(proxyHistograms.every(h => h.filter(Boolean).length >= 95), 'proxy selector collapsed to a few layouts');
                    assert(Array.from({ length: stackProxy.variantCount }, (_, i) => proxyHistograms.some(h => h[i])).every(Boolean), 'a proxy layout was never selected');
                    const first = proxyBuilds.get(1), second = proxyBuilds.get(2);
                    assert(first && second && first.every((code, i) => !code.equals(second[i])), 'proxy payload did not change by seed');
                    fs.writeFileSync(proxyHeader, source);
                    const repeat = await compile(compiler, directory, `${arch}-stack-proxy-repeat.dll`, proxyFile, ['NO_CFLOW=1', 'NO_ANTIDEBUG=1', 'OBFH_BUILD_SEED=1u'], ['-shared']);
                    assert(junkCode(fs.readFileSync(repeat), stackProxy.count).every((code, i) => code.equals(first[i])), 'fixed-seed proxy bytes are not reproducible');
                    fs.writeFileSync(path.join(directory, `${arch}-stack-proxy-distribution.json`), JSON.stringify(proxyHistograms, null, 2));
                });
                if (process.argv.includes('--only-stack-proxy')) continue;
                await check(`${arch}/API failure branches`, async () => await execute(await compile(compiler, directory, `${arch}-failures.exe`, failureFile, []), 'failure branches passed'));
                const antiRoot = path.join(directory, arch + '-antidebug');
                fs.mkdirSync(path.join(antiRoot, 'include'), { recursive: true });
                fs.mkdirSync(path.join(antiRoot, 'tests'), { recursive: true });
                const antiHeader = path.join(antiRoot, 'include', 'obfus.h');
                const antiFile = path.join(antiRoot, 'tests', 'antidebug.c');
                fs.writeFileSync(antiHeader, source);
                fs.copyFileSync(path.join(__dirname, 'antidebug.c'), antiFile);
                const debugHost = await compile(compiler, directory, `${arch}-antidebug-host.exe`, path.join(__dirname, 'antidebug_host.c'), []);
                for (const [label, flags] of [['plain', ['NO_OBF=1']], ['disabled', ['NO_ANTIDEBUG=1']], ['default', []], ['advanced', ['ANTIDEBUG_V2=1', 'CFLOW_V2=1', 'VIRT=1']]]) {
                    await check(`${arch}/anti-debug statement syntax and normal execution/${label}`, async () => {
                        const exe = await compile(compiler, directory, `${arch}-antidebug-${label}.exe`, antiFile, flags);
                        await execute(exe, 'ANTIDEBUG_PASS');
                        if (label === 'default' || label === 'advanced') {
                            const binary = fs.readFileSync(exe);
                            assert(!binary.includes(Buffer.from('IsDebuggerPresent\0')), 'plain debugger API name/import remains');
                            await execute(debugHost, 'ANTIDEBUG_REAL_DEBUGGER_PASS', [exe]);
                            for (const route of ['0', '1']) {
                                let stopped;
                                try { await run(exe, [route], { timeout: 1500 }); } catch (error) { stopped = error; }
                                assert(stopped?.code === 'ETIMEDOUT' && stopped.stdout.includes('RESPONSE_ENTER') && !stopped.stdout.includes('RESPONSE_RETURNED'), 'remote response crashed or returned');
                            }
                        }
                    });
                }
                await check(`${arch}/anti-debug resolver fallback`, async () => {
                    const fallback = source.replace('if (check) return check() != FALSE;', 'if (0) return check() != FALSE;');
                    assert(fallback !== source, 'fallback target missing');
                    fs.writeFileSync(antiHeader, fallback);
                    await execute(await compile(compiler, directory, `${arch}-antidebug-fallback.exe`, antiFile, []), 'ANTIDEBUG_PASS');
                });
                await check(`${arch}/anti-debug positive signal dispatch`, async () => {
                    const start = source.indexOf('static int IsDebuggerPresent_proxy(void)');
                    const end = source.indexOf('// Live paths', start);
                    assert(start >= 0 && end > start, 'detector target missing');
                    fs.writeFileSync(antiHeader, source.slice(0, start) + 'static int IsDebuggerPresent_proxy(void) { return 1; }\n\n' + source.slice(end));
                    const exe = await compile(compiler, directory, `${arch}-antidebug-signal.exe`, antiFile, []);
                    let stopped;
                    try { await run(exe, ['signal'], { timeout: 1500 }); } catch (error) { stopped = error; }
                    assert(stopped?.code === 'ETIMEDOUT' && stopped.stdout.includes('RESPONSE_ENTER') && !stopped.stdout.includes('RESPONSE_RETURNED'), 'positive signal did not reach the remote response');
                });
                if (process.argv.includes('--only-antidebug')) continue;
                const constantBuilds = new Map();
                for (const seed of [0, 1, 2, 3735928559, 4294967295]) await check(`${arch}/constant data types and binary signatures/seed ${seed}`, async () => {
                    const exe = await compile(compiler, directory, `${arch}-constants-${seed}.exe`, path.join(__dirname, 'constant_data.c'), ['NO_CFLOW=1', 'NO_ANTIDEBUG=1', `OBFH_BUILD_SEED=${seed}u`]);
                    const output = await execute(exe, 'CONSTANT_DATA_PASS');
                    const rows = [...output.stdout.matchAll(/^JUNK (\w+) (\d+) ([0-9a-f]+)\r?$/gm)];
                    assert(rows.length === 68 && rows.every(row => row[3].length === Number(row[2]) * 2), 'random data objects missing');
                    const binary = fs.readFileSync(exe);
                    assertNoConstantSignature(binary);
                    const protectedData = pe(binary).sections.find(section => section.name === '.obfh');
                    assert(protectedData && protectedData.bytes.length > 512, 'random data was not emitted');
                    constantBuilds.set(seed, rows.map(row => row[0].trim()).join('\n'));
                });
                await check(`${arch}/constant data seed changes and exact reproducibility`, async () => {
                    assert(constantBuilds.get(1) !== constantBuilds.get(2), 'seed did not change constant data');
                    const repeated = await compile(compiler, directory, `${arch}-constants-repeat.exe`, path.join(__dirname, 'constant_data.c'), ['NO_CFLOW=1', 'NO_ANTIDEBUG=1', 'OBFH_BUILD_SEED=1u']);
                    const output = await execute(repeated, 'CONSTANT_DATA_PASS');
                    const rows = [...output.stdout.matchAll(/^JUNK (\w+) (\d+) ([0-9a-f]+)\r?$/gm)];
                    assert(constantBuilds.get(1) === rows.map(row => row[0].trim()).join('\n'), 'fixed-seed constant data differs');
                });
                if (process.argv.includes('--only-data')) continue;
                if (process.argv.includes('--only-integration')) {
                    await check(`${arch}/default/integration phases + deadline`, async () => await execute(await compile(compiler, directory, `${arch}-default-integration.exe`, path.join(__dirname, 'integration.c'), [], ['-luser32', '-lgdi32']), 'Full-header stress passed'));
                    continue;
                }
                await check(`${arch}/working-set option`, async () => await execute(await compile(compiler, directory, `${arch}-working-set.exe`, path.join(__dirname, 'protection.c'), ['NO_CFLOW=1', 'NO_ANTIDEBUG=1', 'MEM_CLEANER__JUST_FOR_FUN=1'], ['-luser32']), 'PROTECTION_PASS'));
                if (process.argv.includes('--only-working-set')) continue;
                const junkVariants = new Map();
                for (const seed of [0, 1, 2, 3735928559, 4294967295]) await check(`${arch}/seed ${seed}/all junk sites + VM stress`, async () => {
                    const flags = ['VIRT=1', 'NO_ANTIDEBUG=1', `OBFH_BUILD_SEED=${seed}u`, `CFLOW_V2=${seed & 1}`];
                    const file = path.join(__dirname, 'junk.c');
                    await execute(await compile(compiler, directory, `${arch}-junk-${seed}.exe`, file, flags), 'JUNK_PASS');
                    // Keep modes equal for the machine-code diversity comparison.
                    const dllFlags = ['VIRT=1', 'NO_ANTIDEBUG=1', `OBFH_BUILD_SEED=${seed}u`];
                    const dll = await compile(compiler, directory, `${arch}-junk-${seed}.dll`, file, dllFlags, ['-shared']);
                    junkVariants.set(seed, junkCode(fs.readFileSync(dll)));
                });
                await check(`${arch}/junk bytes vary by seed and repeat with a fixed seed`, async () => {
                    const first = junkVariants.get(1), second = junkVariants.get(2);
                    assert(first && second, 'seed stress failed before binary comparison');
                    assert(first.every((code, i) => !code.equals(second[i])), 'a junk site did not vary across seeds');
                    const payloads = [...junkVariants.values()].map(sites => {
                        const code = sites[0], instruction = code.indexOf(Buffer.from([0x31, 0xc0, 0x0f, 0x84]));
                        const anchor = instruction + 8;
                        assert(instruction >= 0 && code[anchor] === 0xe8, 'legacy skip anchor changed');
                        const destination = anchor + code.readInt32LE(instruction + 4);
                        assert(destination >= anchor + 10 && code[destination] === 0x0f && code[destination + 1] === 0xa2, 'junk skip does not land at CPUID');
                        return code.subarray(anchor + 1, anchor + 5);
                    });
                    assert(new Set(payloads.map(bytes => [...bytes].slice(1).map((b, i) => (b - bytes[i]) & 255).join(','))).size > 1, 'random bytes repeat the same affine pattern');
                    for (const index of [0, 3, 4, 5, 6, 7, 8]) {
                        const code = first[index], branch = code.indexOf(Buffer.from([0x0f, 0x84]));
                        const { destination } = withoutSkippedPayload(code, branch);
                        assert(code[destination] === 0x0f && code[destination + 1] === 0xa2, 'legacy skip no longer reaches CPUID');
                    }
                    for (const index of [9, 10, 11, 12]) {
                        const code = first[index], opcode = index === 12 ? 0x85 : 0x84;
                        const branch = code.indexOf(Buffer.from([0x0f, opcode]));
                        const { liveBytes } = withoutSkippedPayload(code, branch);
                        assert(!liveBytes.includes(Buffer.from([0x0f, 0xa2])), 'lightweight live path serializes with CPUID');
                    }
                    const repeat = await compile(compiler, directory, `${arch}-junk-repeat.dll`, path.join(__dirname, 'junk.c'), ['VIRT=1', 'NO_ANTIDEBUG=1', 'OBFH_BUILD_SEED=1u'], ['-shared']);
                    const repeated = junkCode(fs.readFileSync(repeat));
                    assert(first.every((code, i) => code.equals(repeated[i])), 'fixed-seed junk code is not reproducible');
                });
                for (const seed of [0, 1, 2, 3735928559, 4294967295]) await check(`${arch}/ASM identities/arbitrary inputs/seed ${seed}`, async () => {
                    await execute(await compile(compiler, directory, `${arch}-predicates-${seed}.exe`, path.join(__dirname, 'break_predicates.c'), ['NO_CFLOW=1', 'NO_ANTIDEBUG=1', `OBFH_BUILD_SEED=${seed}u`]), 'BREAK_PREDICATES_PASS');
                });
                for (const seed of [0, 1]) await check(`${arch}/choose_expr/direct and captured RND/seed ${seed}`, async () => {
                    await execute(await compile(compiler, directory, `${arch}-choose-${seed}.exe`, path.join(__dirname, 'choose_random.c'), ['NO_CFLOW=1', 'NO_ANTIDEBUG=1', `OBFH_BUILD_SEED=${seed}u`]), 'CHOOSE_RANDOM_PASS');
                });
                const cflowVariants = new Map();
                for (const seed of [0, 1, 2, 3735928559, 4294967295]) await check(`${arch}/CFLOW pool seed ${seed}/128 templates`, async () => {
                    const flags = ['VIRT=1', 'NO_CFLOW=1', 'NO_ANTIDEBUG=1', `OBFH_BUILD_SEED=${seed}u`];
                    const file = path.join(__dirname, 'cflow_junk.c');
                    await execute(await compile(compiler, directory, `${arch}-cflow-pool-${seed}.exe`, file, flags), 'CFLOW_JUNK_PASS');
                    const dll = await compile(compiler, directory, `${arch}-cflow-pool-${seed}.dll`, file, flags, ['-shared']);
                    cflowVariants.set(seed, junkCode(fs.readFileSync(dll), 128));
                });
                await check(`${arch}/CFLOW pool changes by seed and repeats exactly`, async () => {
                    const first = cflowVariants.get(1), second = cflowVariants.get(2);
                    assert(first && second && first.every((code, i) => !code.equals(second[i])), 'CFLOW template did not change with seed');
                    const dll = await compile(compiler, directory, `${arch}-cflow-repeat.dll`, path.join(__dirname, 'cflow_junk.c'), ['VIRT=1', 'NO_CFLOW=1', 'NO_ANTIDEBUG=1', 'OBFH_BUILD_SEED=1u'], ['-shared']);
                    const repeated = junkCode(fs.readFileSync(dll), 128);
                    assert(first.every((code, i) => code.equals(repeated[i])), 'CFLOW fixed-seed build differs');
                    assert(new Set(first.map(code => code.toString('hex'))).size === 128, 'duplicate CFLOW machine-code templates');
                });
                const randomRoot = path.join(directory, `${arch}-break-random`);
                fs.mkdirSync(path.join(randomRoot, 'include'), { recursive: true });
                fs.mkdirSync(path.join(randomRoot, 'tests'), { recursive: true });
                const tracedRandom = source.replace('OBFH_CFLOW_SELECT(__obfh_break_index);', 'obfh_test_break_visit(__obfh_break_id, __obfh_break_index); OBFH_CFLOW_SELECT(__obfh_break_index);');
                assert(tracedRandom !== source, 'public selector trace target missing');
                fs.writeFileSync(path.join(randomRoot, 'include', 'obfus.h'), tracedRandom);
                const randomFile = path.join(randomRoot, 'tests', 'random.c');
                fs.writeFileSync(randomFile, breakRandom.fixture());
                // Measure machine code without the trace call or its unique counter argument.
                const randomCodeRoot = path.join(directory, `${arch}-break-random-code`);
                fs.mkdirSync(path.join(randomCodeRoot, 'include'), { recursive: true });
                fs.mkdirSync(path.join(randomCodeRoot, 'tests'), { recursive: true });
                fs.writeFileSync(path.join(randomCodeRoot, 'include', 'obfus.h'), source);
                const randomCodeFile = path.join(randomCodeRoot, 'tests', 'random.c');
                fs.writeFileSync(randomCodeFile, breakRandom.fixture());
                const randomBuilds = new Map();
                for (const seed of [0, 1, 2, 3735928559, 4294967295]) await check(`${arch}/public break seed ${seed}/512 call sites`, async () => {
                    const flags = ['NO_CFLOW=1', 'NO_ANTIDEBUG=1', 'CFLOW_V2=0', `OBFH_BUILD_SEED=${seed}u`];
                    const output = await run(await compile(compiler, directory, `${arch}-random-${seed}.exe`, randomFile, flags), []);
                    assert(output.status === 0, 'public break corrupted execution: ' + output.stderr);
                    const measurement = breakRandom.measurements(output.stdout, seed, assert);
                    const dll = await compile(compiler, directory, `${arch}-random-${seed}.dll`, randomCodeFile, flags, ['-shared']);
                    const code = junkCode(fs.readFileSync(dll), breakRandom.siteCount);
                    measurement.distinctMachineCode = new Set(code.map(bytes => bytes.toString('hex'))).size;
                    assert(measurement.distinctMachineCode >= 500, 'public call sites repeat almost identical machine code');
                    randomBuilds.set(seed, { ...measurement, code });
                });
                await check(`${arch}/public break distribution + reproducibility`, async () => {
                    assert(randomBuilds.size === 5, 'public selector sampling build failed');
                    const histogram = Array(128).fill(0);
                    let heavyCount = 0, adjacentRepeats = 0;
                    for (const sample of randomBuilds.values()) {
                        sample.histogram.forEach((count, index) => histogram[index] += count);
                        heavyCount += sample.heavyCount;
                        adjacentRepeats += sample.adjacentRepeats;
                    }
                    assert(histogram.every(count => count > 0), 'not all 128 templates were selected across sampled seeds');
                    assert(heavyCount > 0 && heavyCount < 2560 * 0.06, 'CPUID weighting failed');
                    assert(adjacentRepeats < 2560 * 0.04, 'too many adjacent template repetitions');
                    const first = randomBuilds.get(1), second = randomBuilds.get(2);
                    const changedTemplates = first.rows.filter((row, index) => row[2] !== second.rows[index][2]).length;
                    assert(changedTemplates > 460, 'different seed barely changes selection');
                    assert(first.code.every((code, index) => !code.equals(second.code[index])), 'seed did not change a public call site');
                    const flags = ['NO_CFLOW=1', 'NO_ANTIDEBUG=1', 'CFLOW_V2=0', 'OBFH_BUILD_SEED=1u'];
                    const repeated = junkCode(fs.readFileSync(await compile(compiler, directory, `${arch}-random-repeat.dll`, randomCodeFile, flags, ['-shared'])), breakRandom.siteCount);
                    assert(first.code.every((code, index) => code.equals(repeated[index])), 'public fixed-seed code is not reproducible');
                    const report = { samples: 2560, histogram, heavyCount, adjacentRepeats, changedTemplates, builds: [...randomBuilds].map(([seed, { code, ...measurement }]) => ({ seed, ...measurement })) };
                    fs.writeFileSync(path.join(directory, `${arch}-break-random.json`), JSON.stringify(report, null, 2));
                    console.log(`RANDOM ${arch}: 128/128 templates, ${heavyCount}/2560 CPUID selections, ${adjacentRepeats} adjacent repeats, ${changedTemplates}/512 templates changed between seeds 1 and 2`);
                });
                await check(`${arch}/public break CFLOW_V2 + syntax`, async () => {
                    for (let index = 0; index < 128; ++index) assert(breakRandom.extraIndex(index) !== index, 'advanced second template equals the first');
                    const output = await run(await compile(compiler, directory, `${arch}-random-v2.exe`, randomFile, ['NO_CFLOW=1', 'NO_ANTIDEBUG=1', 'CFLOW_V2=1']), []);
                    assert(output.status === 0, 'advanced public selector failed');
                    breakRandom.measurements(output.stdout, 0, assert);
                    const syntax = path.join(randomRoot, 'tests', 'syntax.c');
                    fs.writeFileSync(syntax, '#include "../include/obfus.h"\nint main(void) { int x=1; if(x) BREAK_STACK_CFLOW; else x=3; BREAK_STACK_CFLOW; BREAK_STACK_CFLOW; return x != 1; }');
                    for (const flags of [['NO_OBF=1'], ['NO_ANTIDEBUG=1'], ['NO_ANTIDEBUG=1', 'CFLOW_V2=1']]) {
                        const nativeHeader = path.join(randomRoot, 'include', 'obfus.h');
                        fs.writeFileSync(nativeHeader, source);
                        const result = await run(await compile(compiler, directory, `${arch}-random-syntax-${flags.length}-${flags[0]}.exe`, syntax, flags), []);
                        assert(result.status === 0, 'public macro breaks dangling else or adjacent expansions');
                    }
                });
                if (process.argv.includes('--only-junk')) continue;
                if (process.argv.includes('--only-numeric')) {
                    for (const [config, flags] of Object.entries(configs))
                        await check(`${arch}/${config}/numeric representation`, async () => await execute(await compile(compiler, directory, `${arch}-${config}-numeric.exe`, path.join(__dirname, 'numeric.c'), flags), 'NUMERIC_PASS'));
                    continue;
                }
                for (const unicode of [false, true]) for (const protectedBuild of [false, true])
                    await check(`${arch}/${unicode ? 'Unicode' : 'ANSI'}/${protectedBuild ? 'protected' : 'plain'}`, async () => {
                        const flags = [...(unicode ? ['UNICODE=1', '_UNICODE=1'] : []), ...(protectedBuild ? ['NO_ANTIDEBUG=1', 'CFLOW_V2=1'] : ['NO_OBF=1'])];
                        await execute(await compile(compiler, directory, `${arch}-charset-${unicode}-${protectedBuild}.exe`, path.join(__dirname, 'charset.c'), flags, ['-luser32']), 'CHARSET_PASS');
                    });
                for (const [config, flags] of Object.entries(configs)) await check(`${arch}/${config}/multiple translation units + exports`, async () => {
                    const files = ['multi_a.c', 'multi_b.c', 'multi_main.c'].map(name => path.join(__dirname, name));
                    const exe = await compile(compiler, directory, `${arch}-${config}-multi.exe`, files[0], flags, files.slice(1));
                    await execute(exe, 'MULTI_PASS');
                    const dll = await compile(compiler, directory, `${arch}-${config}-multi.dll`, files[0], flags, [files[1], '-shared']);
                    const exports = fs.readFileSync(dll.replace(/\.dll$/, '.def'), 'utf8');
                    assert((exports.match(/\bmulti_export\b/g) || []).length === 1, 'public export missing or duplicated');
                    if (config !== 'plain') assert((exports.match(/\bWhatSoundDoesACowMake\b/g) || []).length === 1, 'cow export missing or duplicated');
                    assert(!exports.includes('obfh_flow') && !exports.includes('obfh_crt'), 'internal helper leaked into exports');
                    const client = await compile(compiler, directory, `${arch}-${config}-multi-client.exe`, path.join(__dirname, 'multi_client.c'), flags);
                    await execute(client, 'MULTI_DLL_PASS', [dll]);
                });
                if (process.argv.includes('--only-platform')) continue;
                await check(`${arch}/CRT cache concurrency, lifetime and capacity`, async () => {
                    const traceRoot = path.join(directory, arch + '-cache');
                    fs.mkdirSync(path.join(traceRoot, 'include'), { recursive: true });
                    fs.mkdirSync(path.join(traceRoot, 'tests'), { recursive: true });
                    const target = 'FARPROC function = GetProcAddress(module, name);';
                    const traced = source.replace(target, 'obfh_test_crt_cold();\n    ' + target);
                    assert(traced !== source, 'CRT trace injection missing');
                    fs.writeFileSync(path.join(traceRoot, 'include', 'obfus.h'), traced);
                    const file = path.join(traceRoot, 'tests', 'cache.c');
                    fs.copyFileSync(path.join(__dirname, 'cache.c'), file);
                    await execute(await compile(compiler, directory, arch + '-cache.exe', file, ['NO_ANTIDEBUG=1', 'CFLOW_V2=1']), 'CACHE_PASS');
                    const mutant = traced.replace('FARPROC cached = obfh_crt_cached(name);', 'FARPROC cached = NULL;');
                    assert(mutant !== traced, 'CRT cache bypass target missing');
                    fs.writeFileSync(path.join(traceRoot, 'include', 'obfus.h'), mutant);
                    const result = await run(await compile(compiler, directory, arch + '-cache-bypass.exe', file, ['NO_ANTIDEBUG=1', 'CFLOW_V2=1']), []);
                    assert(result.status === 1 && result.stderr.includes('cache failure'), 'cache bypass was not detected');
                });
                if (process.argv.includes('--only-cache')) continue;
                for (const mode of [0, 1]) await check(`${arch}/cflow-v${mode + 1} tokens, semantics and bypass control`, async () => {
                    const traceRoot = path.join(directory, `${arch}-cflow-${mode}`);
                    fs.mkdirSync(path.join(traceRoot, 'include'), { recursive: true });
                    fs.mkdirSync(path.join(traceRoot, 'tests'), { recursive: true });
                    const signature = 'double obfh_flow_token(float encoded, unsigned int site) OBFH_CODE_SECTION_ATTRIBUTE {';
                    const traced = source.replace(signature, signature + '\n    obfh_test_flow_visit(); obfh_test_flow_route((site >> 1) & 3u);')
                        .replace('OBFH_CFLOW_SELECT(__obfh_break_index);', 'obfh_test_if_junk_visit(__obfh_break_index); OBFH_CFLOW_SELECT(__obfh_break_index);')
                        .replace(/STACK_PROXY_FUNCTIONS;(\s*\\\r?\n\s*BREAK_STACK_CFLOW;)/, 'obfh_test_proxy_if_visit(); STACK_PROXY_FUNCTIONS;$1');
                    assert(traced !== source, 'flow trace injection missing');
                    const header = path.join(traceRoot, 'include', 'obfus.h');
                    fs.writeFileSync(header, traced);
                    const file = path.join(traceRoot, 'tests', 'cflow.c');
                    fs.copyFileSync(path.join(__dirname, 'cflow.c'), file);
                    const flags = [`CFLOW_V2=${mode}`, 'NO_ANTIDEBUG=1', 'OBFH_TEST_FLOW_TRACE=1'];
                    await execute(await compile(compiler, directory, `${arch}-cflow-${mode}.exe`, file, flags), 'CFLOW_PASS');
                    const ifMutant = traced.replace(/#define if\((?:cond|\.\.\.)\)[\s\S]*?(?=\r?\n\r?\n)/, '#define if(...) if (__VA_ARGS__)');
                    assert(ifMutant !== traced, 'if bypass target missing');
                    fs.writeFileSync(header, ifMutant);
                    const result = await run(await compile(compiler, directory, `${arch}-cflow-${mode}-bypass.exe`, file, flags), []);
                    assert(result.status === 1 && result.stderr.includes('cflow failure'), 'ordinary-if bypass was not detected');
                    const whileMutant = traced.replace('#define while(...) while (OBFUS_CONDITION_BLOCK((__VA_ARGS__)))', '#define while(...) while (__VA_ARGS__)');
                    assert(whileMutant !== traced, 'while bypass target missing');
                    fs.writeFileSync(header, whileMutant);
                    const whileResult = await run(await compile(compiler, directory, `${arch}-cflow-${mode}-while-bypass.exe`, file, flags), []);
                    assert(whileResult.status === 1 && whileResult.stderr.includes('cflow failure'), 'ordinary-while bypass was not detected');
                });
                if (process.argv.includes('--only-cflow')) continue;
                await check(`${arch}/negative control: disabled VM must fail`, async () => {
                    const mutantRoot = path.join(directory, arch + '-mutant');
                    fs.mkdirSync(path.join(mutantRoot, 'include'), { recursive: true });
                    fs.mkdirSync(path.join(mutantRoot, 'tests'), { recursive: true });
                    const start = source.indexOf('#define VM_ADD(', source.indexOf('#define _ENC_OP__NOP'));
                    const end = source.indexOf('\n', start);
                    const mutant = source.slice(0, start) + '#define VM_ADD(a, b) ((a) + (b))' + source.slice(end);
                    fs.writeFileSync(path.join(mutantRoot, 'include', 'obfus.h'), mutant);
                    const file = path.join(mutantRoot, 'tests', 'vm.c');
                    fs.copyFileSync(path.join(__dirname, 'vm.c'), file);
                    const exe = await compile(compiler, directory, arch + '-mutant.exe', file, ['VIRT=1', 'NO_ANTIDEBUG=1']);
                    const result = await run(exe, []);
                    assert(result.status === 1 && result.stderr.includes('VM failure'), 'VM bypass was not detected by the runtime test');
                });
                await check(`${arch}/VM branch trace and bypass mutations`, async () => {
                    const tracedRoot = path.join(directory, arch + '-branch-trace');
                    fs.mkdirSync(path.join(tracedRoot, 'include'), { recursive: true });
                    fs.mkdirSync(path.join(tracedRoot, 'tests'), { recursive: true });
                    const testFile = path.join(tracedRoot, 'tests', 'vm_branches.c');
                    fs.copyFileSync(path.join(__dirname, 'vm_branches.c'), testFile);
                    const functionStart = 'long double Obfh_VirtualMachine(long double uni_key';
                    let traced = source.replace(functionStart, 'void obfh_test_vm_visit(int command, long double nonce, long double site, long double kind);\nvoid obfh_test_vm_step(unsigned int state);\n' + functionStart);
                    traced = traced.replace(/letsExecute:\s*/, 'letsExecute:\n    obfh_test_vm_visit(command, num2, junk_2, junk_3);\n');
                    traced = traced.replace('switch (pc ^ mask)', 'obfh_test_vm_step(pc ^ mask);\n        switch (pc ^ mask)');
                    assert(traced.includes('obfh_test_vm_step(pc ^ mask);'), 'microinstruction trace injection missing');
                    const headerFile = path.join(tracedRoot, 'include', 'obfus.h');
                    fs.writeFileSync(headerFile, traced);
                    const traceFlags = ['VIRT=1', 'NO_CFLOW=1', 'NO_ANTIDEBUG=1', 'OBFH_TEST_BRANCH_TRACE=1'];
                    await execute(await compile(compiler, directory, arch + '-branch-trace.exe', testFile, traceFlags), 'BRANCH_PASS');
                    const mutants = [
                        ['VM_IF', '#define VM_IF(condition) if (condition)'],
                        ['VM_ELSE_IF', '#define VM_ELSE_IF(condition) else if (condition)'],
                        ['VM_ELSE', '#define VM_ELSE else'],
                    ];
                    for (const [name, replacement] of mutants) {
                        const lines = traced.split(/\r?\n/);
                        const index = lines.findIndex(line => line.startsWith('#define ' + name + (name === 'VM_ELSE' ? ' ' : '(')) && line.includes('obfh_vm_branch('));
                        assert(index >= 0, 'branch mutant target missing: ' + name);
                        lines[index] = replacement;
                        fs.writeFileSync(headerFile, lines.join('\n'));
                        const exe = await compile(compiler, directory, arch + '-' + name + '-bypass.exe', testFile, traceFlags);
                        const result = await run(exe, []);
                        assert(result.status === 1 && result.stderr.includes('branch failure'), 'VM bypass not detected: ' + name);
                    }
                });
                if (process.argv.includes('--only-branches')) continue;
            }
            for (const [config, flags] of Object.entries(configs)) {
                if (selectedConfig && selectedConfig !== config) continue;
                const label = `${arch}/${config}`;
                for (const [file, marker] of [['vm', 'VM_PASS'], ['vm_branches', 'BRANCH_PASS'], ['numeric', 'NUMERIC_PASS'], ['algorithms', 'ALGORITHMS_PASS'], ['wrappers', 'regressions passed'], ['window', 'WINDOW_PASS'], ['path_limits', 'PATHS_PASS'], ['protection', 'PROTECTION_PASS']]) {
                    await check(`${label}/${file}`, async () => await execute(await compile(compiler, directory, `${arch}-${config}-${file}.exe`, path.join(__dirname, file + '.c'), flags, ['-luser32', '-lgdi32']), marker));
                }
                await check(`${label}/default-integration keygen`, async () => {
                    const exe = await compile(compiler, directory, `${arch}-${config}-keygen.exe`, path.join(__dirname, 'keygen_demo.c'), flags);
                    await execute(exe, 'DCD48287-ACFB1ECA-576C2D3E-E3459984');
                    await execute(exe, 'STRESS_PASS B2CA27B1', ['--stress']);
                    const empty = await run(exe, [' -- ']);
                    assert(empty.status === 1 && empty.stdout === '', 'empty normalized name was accepted');
                });
                await check(`${label}/stdin + CRT streams`, async () => {
                    const exe = await compile(compiler, directory, `${arch}-${config}-streams.exe`, path.join(__dirname, 'streams.c'), flags);
                    const result = await run(exe, [path.join(directory, `${arch}-${config}-io.tmp`)], { input: '42 automated\n' });
                    assert(result.status === 0 && result.stdout.includes('STREAMS_PASS'), `exit=${result.status}: ${result.stderr}`);
                });
                await check(`${label}/real-header stress + intentional crashes`, async () => {
                    const exe = await compile(compiler, directory, `${arch}-${config}-integration.exe`, path.join(__dirname, 'integration.c'), flags, ['-luser32', '-lgdi32']);
                    await execute(exe, 'Full-header stress passed');
                    for (const mode of ['unsafe-overflow', 'unsafe-format', 'unsafe-uaf']) {
                        const result = await run(exe, [mode], { timeout: 10000 });
                        assert((result.status >>> 0) === 0xc0000005, `${mode}: expected access violation, got ${result.status}`);
                    }
                });
                await check(`${label}/DLL exports + shared data`, async () => {
                    const dll = await compile(compiler, directory, `${arch}-${config}-fixture.dll`, path.join(__dirname, 'fixture.c'), flags, ['-shared']);
                    const client = await compile(compiler, directory, `${arch}-${config}-client.exe`, path.join(__dirname, 'dll_client.c'), flags);
                    await execute(client, 'DLL_PASS', [dll]);
                    assert(pe(fs.readFileSync(dll)).machine === (arch === 'x64' ? 0x8664 : 0x14c), 'wrong DLL architecture');
                    const definition = dll.replace(/\.dll$/, '.def');
                    assert(fs.existsSync(definition), 'DLL import definition missing');
                    const imported = await compile(compiler, directory, `${arch}-${config}-import.exe`, path.join(__dirname, 'dll_import.c'), flags, [definition]);
                    await execute(imported, 'DLL_IMPORT_PASS');
                });
                await check(`${label}/hidden-string binary + CPUID`, async () => {
                    const exe = await compile(compiler, directory, `${arch}-${config}-probe.exe`, path.join(__dirname, 'probe.c'), flags);
                    const marker = 'obfh_unique_hidden_probe_8a39b17f';
                    await execute(exe, marker);
                    const binary = fs.readFileSync(exe), data = pe(binary);
                    assert(data.machine === (arch === 'x64' ? 0x8664 : 0x14c), 'wrong executable architecture');
                    const literal = binary.includes(Buffer.from(marker));
                    assert(config === 'plain' ? literal : !literal, 'hidden string binary check failed');
                    if (config !== 'plain') {
                        assertNoConstantSignature(binary);
                        for (const name of ['GetProcAddress', 'LoadLibraryA', 'abs', 'memchr', 'vprintf'])
                            assert(!binary.includes(Buffer.from('\0' + name + '\0')), 'native import leaked: ' + name);
                        assert(binary.includes(Buffer.from([0x0f, 0x01, 0xf9])), 'RDTSCP missing from executable');
                        const protectedSection = data.sections.find(section => section.name === (flags.includes('FAKE_SIGNS=1') ? 'UPX0' : '.obfh'));
                        assert(protectedSection, 'protected data section missing');
                        const executableSections = data.sections.filter(section => section.executable);
                        assert(executableSections.some(section => section.bytes.includes(Buffer.from([0x0f, 0xa2]))), 'CPUID/junk obfuscation missing from executable sections');
                        if (arch === 'x64') {
                            assert(executableSections.some(section => section.bytes.includes(Buffer.from([0xf3, 0x0f, 0x2a]))), 'integer-to-float SSE conversion missing');
                            assert(executableSections.some(section => section.bytes.includes(Buffer.from([0xf3, 0x0f, 0x2c]))), 'float-to-integer SSE conversion missing');
                        }
                    }
                });
            }
            for (const flags of [[], ['VIRT=1', 'virt_std=1']]) {
                await check(`${arch}/math string and pointer arguments/${flags.length ? 'VM' : 'normal'}`, async () => await execute(await compile(compiler, directory, `${arch}-math-${flags.length}.exe`, path.join(__dirname, 'math.c'), flags), 'Math string/pointer arguments passed'));
            }
        }
        await check('header remained unchanged during the run', async () => assert(fs.readFileSync(path.join(root, 'include', 'obfus.h'), 'utf8') === source, 'header changed while the suite was running; rerun against an immutable header'));
    } catch (error) {
        results.push({ name: 'suite setup', pass: false, error: error.message }); console.error(error.message);
    }
    const failed = results.filter(result => !result.pass);
    if (directory) fs.writeFileSync(path.join(directory, 'results.json'), JSON.stringify({ results, passed: results.length - failed.length, failed: failed.length }, null, 2));
    console.log(`\n${failed.length ? 'FAIL' : 'PASS'}: ${results.length - failed.length}/${results.length} checks; ${failed.length} failures.`);
    console.log('Verdict applies to the tested compiler builds and Windows environment; invalid C remains invalid.');
    if (directory) console.log(`Logs and binaries: ${directory}`);
    process.exitCode = failed.length ? 1 : 0;

}
main().catch(error => { console.error(error); process.exitCode = 1; });
