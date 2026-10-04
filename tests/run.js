'use strict';
const fs = require('node:fs');
const os = require('node:os');
const path = require('node:path');
const { spawn, spawnSync } = require('node:child_process');
const root = path.resolve(__dirname, '..');
const source = fs.readFileSync(path.join(root, 'include', 'obfus.h'), 'utf8');
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
        sections.push({ name: binary.subarray(offset, offset + 8).toString().replace(/\0.*$/, ''), bytes: binary.subarray(binary.readUInt32LE(offset + 20), binary.readUInt32LE(offset + 20) + binary.readUInt32LE(offset + 16)) });
    }
    return { machine: binary.readUInt16LE(nt + 4), sections };
}
async function main() {
    let directory;
    try {
        assert(process.platform === 'win32', 'This suite exercises real Windows x86/x64 executables.');
        const compilers = await discover();
        directory = fs.mkdtempSync(path.join(os.tmpdir(), 'obfh-js-suite-'));
        artifactDirectory = directory;
        console.log(`Artifacts: ${directory}`);
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
        const setup = source.slice(source.indexOf('    // Registers validation'), source.indexOf('    // Dynamic antidebugger'));
        assert(setup.includes('DuplicateHandle'), 'failure-test extraction missing');
        const failureFile = path.join(directory, 'failures.c');
        fs.writeFileSync(failureFile, fs.readFileSync(path.join(__dirname, 'failures.c.in'), 'utf8').replace('/* SETUP_BLOCK */', setup));
        for (const [arch, compiler] of Object.entries(compilers)) {
            console.log(`${arch}: ${(await run(compiler, ['-v'])).stdout.trim()}`);
            if (process.argv.includes('--only-integration')) {
                await check(`${arch}/default/integration phases + deadline`, async () => await execute(await compile(compiler, directory, `${arch}-default-integration.exe`, path.join(__dirname, 'integration.c'), [], ['-luser32', '-lgdi32']), 'Full-header stress passed'));
                continue;
            }
            await check(`${arch}/working-set option`, async () => await execute(await compile(compiler, directory, `${arch}-working-set.exe`, path.join(__dirname, 'protection.c'), ['NO_CFLOW=1', 'NO_ANTIDEBUG=1', 'MEM_CLEANER__JUST_FOR_FUN=1'], ['-luser32']), 'PROTECTION_PASS'));
            if (process.argv.includes('--only-working-set')) continue;
            for (const mode of [0, 1]) await check(`${arch}/cflow-v${mode + 1} tokens, semantics and bypass control`, async () => {
                const traceRoot = path.join(directory, `${arch}-cflow-${mode}`);
                fs.mkdirSync(path.join(traceRoot, 'include'), { recursive: true });
                fs.mkdirSync(path.join(traceRoot, 'tests'), { recursive: true });
                const signature = 'double obfh_flow_token(float encoded, unsigned int site) OBFH_SECTION_ATTRIBUTE {';
                const traced = source.replace(signature, signature + '\n    obfh_test_flow_visit();');
                assert(traced !== source, 'flow trace injection missing');
                const header = path.join(traceRoot, 'include', 'obfus.h');
                fs.writeFileSync(header, traced);
                const file = path.join(traceRoot, 'tests', 'cflow.c');
                fs.copyFileSync(path.join(__dirname, 'cflow.c'), file);
                const flags = [`CFLOW_V2=${mode}`, 'NO_ANTIDEBUG=1', 'OBFH_TEST_FLOW_TRACE=1'];
                await execute(await compile(compiler, directory, `${arch}-cflow-${mode}.exe`, file, flags), 'CFLOW_PASS');
                fs.writeFileSync(header, traced.replace('#define if(cond) if (OBFH_FLOW_CONDITION(cond, RND(1, 65535)))', '#define if(cond) if (cond)'));
                const result = await run(await compile(compiler, directory, `${arch}-cflow-${mode}-bypass.exe`, file, flags), []);
                assert(result.status === 1 && result.stderr.includes('cflow failure'), 'ordinary-if bypass was not detected');
            });
            if (process.argv.includes('--only-cflow')) continue;
            await check(`${arch}/API failure branches`, async () => await execute(await compile(compiler, directory, `${arch}-failures.exe`, failureFile, []), 'failure branches passed'));
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
            for (const [config, flags] of Object.entries(configs)) {
                const label = `${arch}/${config}`;
                for (const [file, marker] of [['vm', 'VM_PASS'], ['vm_branches', 'BRANCH_PASS'], ['algorithms', 'ALGORITHMS_PASS'], ['wrappers', 'regressions passed'], ['window', 'WINDOW_PASS'], ['path_limits', 'PATHS_PASS'], ['protection', 'PROTECTION_PASS']]) {
                    await check(`${label}/${file}`, async () => await execute(await compile(compiler, directory, `${arch}-${config}-${file}.exe`, path.join(__dirname, file + '.c'), flags, ['-luser32', '-lgdi32']), marker));
                }
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
                        for (const name of ['GetProcAddress', 'LoadLibraryA', 'abs', 'memchr', 'vprintf'])
                            assert(!binary.includes(Buffer.from('\0' + name + '\0')), 'native import leaked: ' + name);
                        assert(binary.includes(Buffer.from([0x0f, 0x01, 0xf9])), 'RDTSCP missing from executable');
                        const protectedSection = data.sections.find(section => section.name === (flags.includes('FAKE_SIGNS=1') ? 'UPX0' : '.obfh'));
                        assert(protectedSection?.bytes.includes(Buffer.from([0x0f, 0xa2])), 'CPUID/junk obfuscation missing from protected section');
                        if (arch === 'x64') {
                            assert(protectedSection.bytes.includes(Buffer.from([0xf3, 0x0f, 0x2a])), 'integer-to-float SSE conversion missing');
                            assert(protectedSection.bytes.includes(Buffer.from([0xf3, 0x0f, 0x2c])), 'float-to-integer SSE conversion missing');
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
