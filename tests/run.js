'use strict';
const fs = require('node:fs');
const os = require('node:os');
const path = require('node:path');
const { createProcessRunner } = require('./runner_process');
const breakRandom = require('./break_random');
const stackProxy = require('./stack_proxy');
const pdataDecoys = require('./pdata_decoys');
const vmKernel = require('./vm_kernel');
const crtBundle = require('./crt_bundle');
const vmSemantics = require('./vm_semantics');
const cflowChain = require('./cflow_chain');
const flowTransport = require('./flow_transport');
const { macroDefinition } = require('./source_macros');
const { pe, importModules, assertNoConstantSignature, withoutSkippedPayload, junkCode } = require('./binary_checks');
const cflowPrimitives = require('./cflow_primitives');
const dllHosts = require('./dll_hosts');
const { AsyncLocalStorage } = require('node:async_hooks');
const { CheckPool, ProcessPool, workerCount, runInPools, workerLimit } = require('./runner_pool');
const taskContext = new AsyncLocalStorage();
const finalizers = [];
const suiteStarted = Date.now();
let compilationCount = 0;
let pool,
    buildPool,
    executionPool,
    processPool,
    checkIndex = 0;
const order = new Map();
function checkGroup(name) {
    const arch = name.split('/')[0];
    // These families share a mutable header or consume earlier build results.
    for (const family of ['stack proxies', 'anti-debug', 'constant data', 'CFLOW pool', 'public break', 'pdata'])
        if (name.includes('/' + family)) return arch + '/' + family;
    if (/all junk sites|junk bytes vary/.test(name)) return arch + '/junk';
    return name;
}
function checkPriority(name) {
    if (/^(source:|runner:|binary scanner:)/.test(name)) return 1000;
    // Start unavoidable serial chains and expensive VM builds first. Independent
    // CRT/CFLOW builds are separate tasks and no longer need the highest priority.
    if (/\/(?:stack proxies|CFLOW pool|public break|constant data|anti-debug)/.test(name)) return 110;
    if (/all junk sites|junk bytes vary/.test(name)) return 105;
    if (name.includes('/VM ISA, all programs')) return 95;
    if (/\/(?:vm|vm_branches|numeric|semantic contracts)$/.test(name)) return 94;
    if (name.includes('/multiple translation units + exports')) return 93;
    if (name.includes('/GUI cache + ABI') || name.includes('/lazy letters:')) return 92;
    if (name.includes('/fast transfer bytes')) return 90;
    if (/\/cflow-v[12] tokens,/.test(name)) return 80;
    if (name.includes('/CRT proxy calls')) return 75;
    if (/false else guard|typed return transport/.test(name)) return 60;
    return 0;
}
async function check(name, action) {
    const id = checkIndex++;
    order.set(name, id);
    if (!pool) return performCheck(name, action);
    pool.add(checkGroup(name), async () => {
        await performCheck(name, async () => {
            const cwd = path.join(artifactDirectory, 'tasks', String(id));
            fs.mkdirSync(cwd, { recursive: true });
            await taskContext.run({ cwd, name }, action);
        });
    }, checkPriority(name));
    return true;
}
const root = path.resolve(__dirname, '..');
const headerFile = path.resolve(
    process.argv.find((value) => value.startsWith('--header='))?.slice(9) ?? path.join(root, 'include', 'obfus.h')
);
const source = fs.readFileSync(headerFile, 'utf8');
const selectedArch = process.argv.find((value) => value.startsWith('--arch='))?.slice(7);
const selectedConfig = process.argv.find((value) => value.startsWith('--config='))?.slice(9);
const selectedConfigs = selectedConfig?.split(',');
const results = [];
let artifactDirectory;
let snapshotRoot;
function snapshotFile(file) {
    const relative = path.relative(path.join(root, 'tests'), path.resolve(file));
    return snapshotRoot && !relative.startsWith('..') && !path.isAbsolute(relative)
        ? path.join(snapshotRoot, 'tests', relative)
        : file;
}
const configs = {
    plain: ['OBFH_DISABLE_ANY_OBFUSCATION=1'],
    default: [],
    'no-cflow': ['OBFH_DISABLE_CONTROL_FLOW=1', 'OBFH_DISABLE_ANTIDEBUG=1'],
    advanced: ['OBFH_BETTER_CONTROL_FLOW=1', 'OBFH_BETTER_ANTIDEBUG=1', 'OBFH_FAKE_SIGNATURES=1'],
    'no-antidebug': ['OBFH_DISABLE_ANTIDEBUG=1']
};
function assert(condition, message) {
    if (!condition) throw new Error(message);
}
async function performCheck(name, action) {
    const started = Date.now();
    try {
        await action();
        const durationMs = Date.now() - started;
        results.push({ name, pass: true, durationMs });
        const timing = durationMs >= 1000 ? ` (${(durationMs / 1000).toFixed(1)}s)` : '';
        console.log(`PASS ${name}${timing}`);
        return true;
    } catch (error) {
        results.push({ name, pass: false, durationMs: Date.now() - started, error: error.message });
        console.error(`FAIL ${name}: ${error.message}`);
        if (error.suiteDeadline) throw error;
        return false;
    }
}
function protectedMacro(text, name) {
    const begin = text.indexOf('// Virtualization (instruction programs)');
    assert(begin >= 0, 'protected VM block missing');
    return macroDefinition(text, name, { from: begin });
}
function replaceProtectedMacro(text, name, replacement) {
    const { start, end } = protectedMacro(text, name);
    return text.slice(0, start) + replacement + text.slice(end);
}
function timeoutOption(name, fallback) {
    const value = process.argv.find((arg) => arg.startsWith('--' + name + '='));
    const milliseconds = value ? Number(value.split('=')[1]) : fallback;
    assert(Number.isSafeInteger(milliseconds) && milliseconds >= 100, 'invalid timeout: ' + name);
    return milliseconds;
}
const testTimeout = timeoutOption('test-timeout-ms', 30000);
const buildTimeout = timeoutOption('build-timeout-ms', 45000);
const suiteDeadline = Date.now() + timeoutOption('suite-timeout-ms', 3600000);
const { runChild, activeChildren } = createProcessRunner({
    root, suiteDeadline, testTimeout, taskContext,
    isParallel: () => Boolean(pool),
    getArtifactDirectory: () => artifactDirectory
});
async function run(command, args, options = {}) {
    const queue = /^(?:i386-win32-)?tcc\.exe$/i.test(path.basename(command)) ? buildPool : executionPool;
    // Per-process timers start after admission; queue waiting still counts toward
    // the suite deadline, which runChild checks before spawning anything.
    return queue ? runInPools(queue, processPool, () => runChild(command, args, options)) : runChild(command, args, options);
}
async function discover() {
    const argument = process.argv.find((value) => value.startsWith('--tcc-dir='));
    const desktop =
        process.platform === 'win32'
            ? (
                await run('powershell.exe', [
                    '-NoProfile',
                    '-Command',
                    '[Console]::OutputEncoding = [Text.UTF8Encoding]::new(); [Environment]::GetFolderPath("Desktop")'
                ])
            ).stdout.trim()
            : '';
    const candidates = [
        argument?.slice(10),
        process.env.TCC_DIR,
        desktop && path.join(desktop, 'tcc', 'tcc'),
        'C:\\tcc'
    ].filter(Boolean);
    for (const directory of candidates) {
        const x64 = path.resolve(directory, 'tcc.exe'),
            x86 = path.resolve(directory, 'i386-win32-tcc.exe');
        if (fs.existsSync(x64) && fs.existsSync(x86)) return { x64, x86 };
    }
    throw new Error(
        'Both tcc.exe and i386-win32-tcc.exe are required. Set TCC_DIR or pass --tcc-dir=C:\\path\\to\\tcc.'
    );
}
async function compile(compiler, directory, name, file, flags, extra = []) {
    ++compilationCount;
    const output = path.join(directory, name);
    const result = await run(
        compiler,
        [
            '-w',
            ...flags.map((flag) => '-D' + flag),
            snapshotFile(file),
            '-o',
            output,
            ...extra.map((value) => (path.isAbsolute(value) ? snapshotFile(value) : value))
        ],
        { timeout: buildTimeout }
    );
    assert(
        result.status === 0,
        `compile ${path.basename(file)} exit=${result.status}: ${result.stderr || result.stdout}`
    );
    assert(fs.existsSync(output), 'compiler did not produce output');
    return output;
}
async function execute(exe, marker, args = []) {
    const result = await run(exe, args);
    assert(result.status === 0, `exit=${result.status}, stderr=${result.stderr}, stdout=${result.stdout.slice(-700)}`);
    assert(result.stdout.includes(marker), `missing completion marker ${marker}`);
    return result;
}
async function main() {
    let directory;
    try {
        assert(process.platform === 'win32', 'This suite exercises real Windows x86/x64 executables.');
        const compilers = await discover();
        assert(!selectedArch || ['x64', 'x86'].includes(selectedArch), 'unknown architecture');
        assert(
            !selectedConfigs || selectedConfigs.every((config) => Object.hasOwn(configs, config)),
            'unknown configuration'
        );
        directory = fs.mkdtempSync(path.join(os.tmpdir(), 'obfh-js-suite-'));
        artifactDirectory = directory;
        snapshotRoot = path.join(directory, 'snapshot');
        fs.mkdirSync(path.join(snapshotRoot, 'include'), { recursive: true });
        fs.writeFileSync(path.join(snapshotRoot, 'include', 'obfus.h'), source);
        fs.cpSync(path.join(root, 'tests'), path.join(snapshotRoot, 'tests'), {
            recursive: true,
            filter: (file) => fs.statSync(file).isDirectory() || /\.(?:c|h|in)$/.test(file)
        });
        const getDllHost = dllHosts.builder({
            directory, header: path.join(snapshotRoot, 'include', 'obfus.h'),
            fixturesRoot: path.join(snapshotRoot, 'tests'), compile
        });
        fs.writeFileSync(
            path.join(directory, 'header-sha256.txt'),
            require('node:crypto').createHash('sha256').update(source).digest('hex') + '\n'
        );
        console.log(`Artifacts: ${directory}`);
        const workers = workerLimit(process.argv,
            workerCount(os.availableParallelism?.() ?? os.cpus().length, os.freemem(), os.totalmem()));
        const liveWorkers = () => workerCount(os.availableParallelism?.() ?? os.cpus().length, os.freemem(), os.totalmem());
        buildPool = new ProcessPool(workers, liveWorkers);
        executionPool = new ProcessPool(workers);
        processPool = new ProcessPool(workers);
        // Checks mostly wait on child processes; compiler memory is limited by
        // buildPool, while shared mutable-header groups remain strictly serial.
        const checkWorkers = process.argv.includes('--serial') ? 1 : workers * 2;
        pool = new CheckPool(checkWorkers, () => checkWorkers);
        console.log(`Process parallelism: ${workers} child processes; builds also limited by available RAM.`);
        await check('source: ASM uses source templates without a generated cache', async () => {
            assert(!source.includes('// BEGIN COMPACT ASM CACHE'), 'generated ASM cache must not be restored');
            assert(!source.includes('// BEGIN GENERATED CFLOW SELECTORS') && !source.includes('#define OBFH_P_CACHE_00'),
                'generated CFLOW cache must not be restored');
        });
        await check('source: macro extraction boundaries', require('./source_macros.test').verify);
        await check('runner: parallel queue ordering and failure cleanup', require('./runner_pool.test').verify);
        await check('source: shared CRT fixtures preserve isolation and failure reporting', async () => {
            await crtBundle.verify(assert, path.join(snapshotRoot, 'tests'));
        });
        await check('source: shared CFLOW primitives retain independent failure reporting', async () => {
            await cflowPrimitives.verify(source, assert, path.join(snapshotRoot, 'tests'));
        });
        await check('source: shared DLL hosts preserve client bodies and build failures', async () => {
            await dllHosts.verify(assert, path.join(snapshotRoot, 'include', 'obfus.h'), path.join(snapshotRoot, 'tests'));
        });
        await check('source: shared VM semantics preserve native references and independent results', async () => {
            await require('./vm_semantics_checks').verify(path.join(snapshotRoot, 'tests'));
        });
        await check('binary scanner: skipped CPUID bytes versus live CPUID', async () => {
            const code = Buffer.from([0x0f, 0x84, 2, 0, 0, 0, 0x0f, 0xa2, 0x90]);
            assert(
                !withoutSkippedPayload(code, 0).liveBytes.includes(Buffer.from([0x0f, 0xa2])),
                'skipped payload counted as live code'
            );
            assert(
                withoutSkippedPayload(Buffer.concat([code, Buffer.from([0x0f, 0xa2])]), 0).liveBytes.includes(
                    Buffer.from([0x0f, 0xa2])
                ),
                'live CPUID was missed'
            );
        });
        await check('source: contiguous numbered pool with helpers above it', async () => {
            const pool = source.slice(
                source.indexOf('#define BREAK_STACK_CFLOW_0 '),
                source.indexOf('#define BREAK_STACK_CFLOW OBFH_CFLOW_EMIT')
            );
            const definitions = [...pool.matchAll(/^#define (\w+)/gm)].map((match) => match[1]);
            assert(
                definitions.length === 128 && definitions.every((name, index) => name === 'BREAK_STACK_CFLOW_' + index),
                'pool contains intervening helpers or missing/out-of-order variants'
            );
        });
        await check('source: 128 proxy variants with distinct guard/layout pairs', async () => {
            const variants = [
                ...source.matchAll(
                    /^#define OBFH_SF_SPEC_(\d+)\(emit\) emit\((\d+)u,\s*(\d+)u,/gm
                )
            ];
            assert(
                variants.length === stackProxy.variantCount && variants.every((v, i) => Number(v[1]) === i),
                'proxy variants missing or out of order'
            );
            assert(
                new Set(variants.map((v) => v[2] + ':' + v[3])).size === variants.length,
                'duplicate proxy guard/layout pair'
            );
            assert(
                source.includes('#define OBFH_SF_VARIANT_COUNT ' + stackProxy.variantCount + 'u'),
                'proxy count and test disagree'
            );
        });
        await check('source: protected VM macros still call the interpreter', async () => {
            const start = source.indexOf('#define VM_ADD(', source.indexOf('// Virtualization (instruction programs)'));
            const block = source.slice(start, source.indexOf('#define VM_IF', start));
            const macros = block
                .replace(/\\\r?\n/g, ' ')
                .split(/\r?\n/)
                .filter((line) => line.startsWith('#define VM_'));
            assert(macros.length === 19, `expected 19 arithmetic/identity macros, found ${macros.length}`);
            for (const macro of macros)
                assert(/OBFH_VM_EXEC\s*\(\s*Obfh_VirtualMachine\s*,/.test(macro), `VM bypass: ${macro}`);
        });
        await check('source: serializing instructions and legacy junk macros removed', async () => {
            assert(!/\b(?:cpuid|rdtscp)\b/i.test(source), 'serializing instruction remains in the header');
            assert(!/\b(?:FAKE_CPUID|NOP_FLOOD)\b/.test(source), 'legacy junk macro remains');
            assert(!source.includes('__obfh_strcmp_junk_done'), 'inline legacy NOP clone remains');
            assert(!/^#define\s+\w+_custom\b/m.test(source), 'undocumented custom compatibility alias remains');
        });
        await check('source: custom exports and unified name construction preserved', async () => {
            const begin = source.indexOf('FARPROC obfh_find_export('),
                end = source.indexOf('#define GetProcAddress', begin);
            assert(
                begin >= 0 && source.slice(begin, end).includes('AddressOfFunctions'),
                'custom export parser missing'
            );
            assert(
                !/\bGetProcAddress\(/.test(source.slice(begin, end)),
                'custom parser delegates to native GetProcAddress'
            );
            assert(!source.includes('getCharMask('), 'obsolete format-mask builder remains');
            assert(
                source.includes('#define OBFH_NAME_ORDER(') && source.includes('#define OBFH_CRT_INVOKE('),
                'shared name/call implementation missing'
            );
            const hidden = source.slice(
                source.lastIndexOf('#define HIDE_STRING'),
                source.indexOf('typedef enum', source.lastIndexOf('#define HIDE_STRING'))
            );
            assert(hidden.includes('== RND('), 'HIDE_STRING false-branch obfuscation lost');
        });
        await check('source: automatic protection paths and type conversions', async () => {
            const identity = protectedMacro(source, 'VM_OBF_DBL').body.replace(/\\\r?\n/g, ' ');
            assert(
                identity && identity.includes('(long double)(num1)') && !identity.includes('(double)(num1)'),
                'VM identity narrows its operand'
            );
            assert(
                source.includes('GetConsoleMode(console, &mode) && !obfh_format_has_count(format)'),
                'count conversion enters the console sizing pass'
            );
            assert(source.includes('float junk, float condition'), 'condition float conversion removed');
            assert(!source.includes('_0 && rdtsc_result'), 'timestamp proxy short-circuited');
            assert(source.includes('0x0f, 0x31'), 'actual timestamp read missing');
            assert(source.includes('OBFH_MEMORY_CLEANER'), 'working-set feature removed');
            assert(
                !source.includes('FARPROC fallback') && !source.includes('(FARPROC)(vprintf)'),
                'native CRT fallback leaked'
            );
            const resolver = source.slice(
                source.indexOf('static inline FARPROC obfh_crt_lookup(const char *name) {'),
                source.indexOf('// printf', source.indexOf('static inline FARPROC obfh_crt_lookup(const char *name) {'))
            );
            assert(resolver.includes('LoadLibraryA_proxy('), 'CRT loader chain bypassed');
            assert(source.includes('return value < (int)FALSE ? -value : value;'), 'custom abs replaced');
        });
        await check('source: every library break uses the unified selector', async () => {
            assert(!/\bBREAK_STACK_\d+\b/.test(source), 'old numbered public macro remains');
            const callsStart = source.indexOf('#define BAD_JMP', source.indexOf('// 08. Junk primitives'));
            assert(callsStart > 0, 'library call-site boundary missing');
            const calls = source.slice(callsStart);
            assert(!/\bBREAK_STACK_CFLOW_\d+\b/.test(calls), 'library pins a numbered template');
            assert(
                source.includes('#define BREAK_STACK_CFLOW OBFH_CFLOW_EMIT(__COUNTER__, OBFH_CFLOW_EXTRA)'),
                'public macro does not capture a fresh counter'
            );
        });
        await check('runner: timeout terminates the whole process tree', async () => {
            const script =
                "const {spawn}=require('node:child_process'); const child=spawn(process.execPath,['-e','setInterval(()=>{},1000)'],{windowsHide:true,stdio:'ignore'}); console.log('TIMEOUT_CHILD:'+child.pid); console.error('TEST_PHASE: watchdog control'); setInterval(()=>{},1000);";
            let timedOut;
            try {
                await run(process.execPath, ['-e', script], { timeout: 1000 });
            } catch (error) {
                timedOut = error;
            }
            assert(timedOut?.code === 'ETIMEDOUT', 'hanging process did not time out');
            assert(timedOut.message.includes('watchdog control'), 'timeout did not report the last phase');
            const childPid = Number(timedOut.stdout.match(/TIMEOUT_CHILD:(\d+)/)?.[1]);
            assert(childPid > 0, 'timeout child did not start');
            let alive = false;
            try {
                process.kill(childPid, 0);
                alive = true;
            } catch { }
            assert(!alive, 'timeout left a child process alive');
        });
        const registerProbe = source.slice(source.indexOf('#define OBFH_AD_READ_REGISTERS('), source.indexOf('// The worker owns the duplicated handle'));
        assert(registerProbe.includes('ResumeThread(thread)'), 'shared register probe missing');
        const setup = registerProbe + source.slice(
            source.indexOf('static inline DWORD WINAPI obfh_ad_register_worker'),
            source.indexOf('// Managed callbacks reuse Windows workers;')
        ) + '\n#endif\n';
        assert(setup.includes('DuplicateHandle') && /#endif\s*$/.test(setup), 'failure-test extraction missing');
        const poolSetup = registerProbe + source.slice(source.indexOf('// Managed callbacks reuse Windows workers;'), source.indexOf('static inline int obfh_ad_process_probe'));
        assert(poolSetup.includes('TrySubmitThreadpoolCallback') && /#endif\s*$/.test(poolSetup), 'pool-test extraction missing');
        const poolFailureFile = path.join(directory, 'antidebug-pool.c');
        fs.writeFileSync(poolFailureFile, fs.readFileSync(path.join(__dirname, 'antidebug_pool.c.in'), 'utf8')
            .replace('/* POOL_PROBES */', '#if 1\n' + poolSetup));
        const failureFile = path.join(directory, 'failures.c');
        const processNames = ['getKernel32Name_proxy', 'getDebuggerName_proxy']
            .map((name) => {
                const match = source.match(
                    new RegExp('static inline char \\*' + name + '\\(char \\*name\\) \\{[\\s\\S]*?\\n\\}')
                );
                assert(match, 'process probe name builder missing: ' + name);
                return match[0];
            })
            .join('\n');
        const nameSetup =
            '#define OBFH_HIDE_JUNK ((void)0)\n#define OBFH_NAME_ORDER(forward, reverse) ((mode & 1) ? (forward) : (reverse))\n' +
            '#define OBFH_NAME_ORDER_FOR(groups, forward, reverse) OBFH_NAME_ORDER(forward, reverse)\n' +
            '#define OBFH_LOADER_FOR(groups) ((void)0)\n' +
            '#define OBFH_NAME_INDEX(index) (index)\n#define OBFH_NAME_PAIR(bit, first, second) ((mode & 1) ? ((first),(second)) : ((second),(first)))\n' +
            [...'abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ']
                .map((char) => `static volatile char _${char} = '${char}';`)
                .join('\n') +
            '\nstatic volatile char _0 = 0;\n';
        const processProbe =
            nameSetup +
            '#define RND(min, max) (min)\n' +
            source.slice(source.indexOf('#define OBFH_LOADER_READ(state)'), source.indexOf('static volatile LONG obfh_alpha_low_state')) +
            processNames +
            '\n' +
            source.slice(
                source.indexOf('static inline int obfh_ad_process_probe'),
                source.indexOf('static inline int IsDebuggerPresent_proxy(void)')
            );
        assert(
            processProbe.includes('gs:0x60') && processProbe.includes('fs:0x30'),
            'process-probe extraction missing'
        );
        fs.writeFileSync(
            failureFile,
            fs
                .readFileSync(path.join(__dirname, 'failures.c.in'), 'utf8')
                .replace('/* REGISTER_PROBES */', '#if OBFH_BETTER_ANTIDEBUG == 1\n' + setup)
                .replace('/* PROCESS_PROBE */', processProbe)
        );
        for (const [arch, compiler] of Object.entries(compilers)) {
            if (process.argv.includes('--only-options') || !process.argv.some((arg) => arg.startsWith('--only-'))) {
                if (selectedArch && selectedArch !== arch) continue;
                await require('./public_options').runSuite({ arch, compiler, directory, source, check, run, assert });
                if (process.argv.includes('--only-options')) continue;
            }
            if (process.argv.includes('--only-helpers') || !process.argv.some((arg) => arg.startsWith('--only-'))) {
                if (selectedArch && selectedArch !== arch) continue;
                await require('./helper_conditions').runSuite({ arch, compiler, directory, source, check, compile, execute, run, assert });
                if (process.argv.includes('--only-helpers')) continue;
            }
            if (process.argv.includes('--only-case')) {
                if (selectedArch && selectedArch !== arch) continue;
                await require('./case_tokens').runSuite({ arch, compiler, directory, source, check, compile, execute, run, assert });
                continue;
            }
            if (process.argv.includes('--only-exports') || !process.argv.some((arg) => arg.startsWith('--only-'))) {
                if (selectedArch && selectedArch !== arch) continue;
                for (const seed of [1, 2])
                    await check(`${arch}/PE exports + ordinary forwarders + bounded names/seed ${seed}`, async () => {
                        await execute(await compile(compiler, directory, `${arch}-exports-${seed}.exe`,
                            path.join(__dirname, 'export_lookup.c'),
                            ['OBFH_DISABLE_ANTIDEBUG=1', 'OBFH_BETTER_CONTROL_FLOW=1', `OBFH_SEED=${seed}u`]), 'EXPORT_LOOKUP_PASS');
                    });
                if (process.argv.includes('--only-exports')) continue;
            }
            if (process.argv.includes('--only-custom') || process.argv.includes('--only-custom33')) {
                const only33 = process.argv.includes('--only-custom33');
                if (selectedArch && selectedArch !== arch) continue;
                for (const config of ['plain', 'default', 'no-cflow', 'advanced']) {
                    if (selectedConfigs && !selectedConfigs.includes(config)) continue;
                    for (const seed of ['plain', 'no-cflow'].includes(config)
                        ? [0]
                        : [0, 1, 2, 0xdeadbeef, 0xffffffff]) {
                        await check(`${arch}/${config}/custom extensions/seed ${seed}`, async () => {
                            const flags = [...configs[config], `OBFH_SEED=${seed}u`];
                            if (!only33) {
                                await crtBundle.runSuite({
                                    compiler, directory,
                                    name: `${arch}-${config}-crt-bundle-${seed}`, flags,
                                    header: path.join(snapshotRoot, 'include', 'obfus.h'),
                                    fixturesRoot: path.join(snapshotRoot, 'tests'), compile, run, assert
                                });
                                await crtBundle.runNumericSuite({
                                    compiler, directory,
                                    name: `${arch}-${config}-crt-numeric-${seed}`, flags,
                                    header: path.join(snapshotRoot, 'include', 'obfus.h'),
                                    fixturesRoot: path.join(snapshotRoot, 'tests'), compile, run, assert
                                });
                            }
                            const fixtures = only33 ? [['custom33', 'CUSTOM33_PASS']] : [
                                ['custom_inline', ''],
                                ['crt_proxies', 'CRT_ATEXIT_PASS']
                            ];
                            if (config !== 'plain') fixtures.push(['custom33_inline', '']);
                            for (const [fixture, marker] of fixtures)
                                await execute(
                                    await compile(
                                        compiler,
                                        directory,
                                        `${arch}-${config}-${fixture}-${seed}.exe`,
                                        path.join(__dirname, fixture + '.c'),
                                        flags
                                    ),
                                    marker
                                );
                        });
                    }
                }
                await check(`${arch}/custom extensions editor view`, async () => {
                    await execute(
                        await compile(compiler, directory, `${arch}-custom33-editor.exe`,
                            path.join(__dirname, 'custom33.c'), ['__INTELLISENSE__=1'], ['-U__TINYC__']),
                        'CUSTOM33_PASS'
                    );
                    if (only33) return;
                    await execute(
                        await compile(
                            compiler,
                            directory,
                            `${arch}-custom-extra-editor.exe`,
                            path.join(__dirname, 'custom_extra.c'),
                            ['__INTELLISENSE__=1'],
                            ['-U__TINYC__']
                        ),
                        'CUSTOM_EXTRA_PASS'
                    );
                    await execute(
                        await compile(compiler, directory, `${arch}-custom-critical-editor.exe`,
                            path.join(__dirname, 'custom_critical.c'), ['__INTELLISENSE__=1'], ['-U__TINYC__']),
                        'CUSTOM_CRITICAL_PASS'
                    );
                });
                continue;
            }
            for (const seed of [0, 1, 2, 0xdeadbeef, 0xffffffff]) {
                if (selectedArch && selectedArch !== arch) continue;
                await check(
                    `${arch}/local API names + buffer guards/seed ${seed}`,
                    async () =>
                        await execute(
                            await compile(
                                compiler,
                                directory,
                                `${arch}-api-names-${seed}.exe`,
                                path.join(__dirname, 'api_names.c'),
                                [`OBFH_SEED=${seed}u`],
                                ['-luser32', '-lgdi32', '-ladvapi32']
                            ),
                            'NAMES_PASS'
                        )
                );
            }
            if (selectedArch && selectedArch !== arch) continue;
            console.log(`${arch}: ${(await run(compiler, ['-v'])).stdout.trim()}`);
            await check(`${arch}/CFLOW equal-width input encodings`, async () => {
                await execute(
                    await compile(
                        compiler,
                        directory,
                        `${arch}-cflow-encoding.exe`,
                        path.join(__dirname, 'cflow_encoding.c'),
                        ['OBFH_DISABLE_CONTROL_FLOW=1', 'OBFH_DISABLE_ANTIDEBUG=1']
                    ),
                    'CFLOW_ENCODING_PASS'
                );
            });
            await cflowPrimitives.runSuite({
                arch, compiler, directory, source,
                fixturesRoot: path.join(snapshotRoot, 'tests'), check, compile, run, assert
            });
            await require('./cflow_selectors').runSuite({
                arch, compiler, directory, source,
                fixturesRoot: path.join(snapshotRoot, 'tests'), check, compile, execute, run, assert
            });
            await check(`${arch}/return interception and dangling else`, async () => {
                for (const flags of [[], ['OBFH_BETTER_CONTROL_FLOW=1'], ['OBFH_DISABLE_CONTROL_FLOW=1'], ['OBFH_DISABLE_ANY_OBFUSCATION=1']])
                    await execute(
                        await compile(
                            compiler,
                            directory,
                            `${arch}-returns-${flags.join('-') || 'default'}.exe`,
                            path.join(__dirname, 'returns.c'),
                            ['OBFH_DISABLE_ANTIDEBUG=1', ...flags]
                        ),
                        'RETURN_PASS'
                    );
                await execute(
                    await compile(
                        compiler,
                        directory,
                        `${arch}-returns-editor.exe`,
                        path.join(__dirname, 'returns.c'),
                        ['__INTELLISENSE__=1'],
                        ['-U__TINYC__']
                    ),
                    'RETURN_PASS'
                );
            });
            for (const seed of [0, 1, 2, 3, 4, 5, 6, 7, 0xdeadbeef, 0xffffffff])
                await check(`${arch}/typed return transport/seed ${seed}`, async () => {
                    await execute(
                        await compile(
                            compiler,
                            directory,
                            `${arch}-return-transport-${seed}.exe`,
                            path.join(__dirname, 'return_transport.c'),
                            ['OBFH_DISABLE_ANTIDEBUG=1', `OBFH_SEED=${seed}u`]
                        ),
                        'RETURN_TRANSPORT_PASS'
                    );
                });
            for (const seed of [0, 1, 2, 0xdeadbeef, 0xffffffff])
                for (const flags of [[], ['OBFH_BETTER_CONTROL_FLOW=1'], ['OBFH_DISABLE_PDATA_DECOYS=1']])
                    await check(`${arch}/false else guard transport/seed ${seed}/${flags.join('-') || 'default'}`, async () => {
                        await execute(
                            await compile(
                                compiler,
                                directory,
                                `${arch}-else-guard-${seed}-${flags.join('-') || 'default'}.exe`,
                                path.join(__dirname, 'else_guard.c'),
                                ['OBFH_DISABLE_ANTIDEBUG=1', `OBFH_SEED=${seed}u`, ...flags]
                            ),
                            'ELSE_GUARD_PASS'
                        );
                    });
            await check(`${arch}/IntelliSense interface and real compiler isolation`, async () => {
                const file = path.join(__dirname, 'editor_view.c');
                await execute(
                    await compile(
                        compiler,
                        directory,
                        `${arch}-editor-view.exe`,
                        file,
                        ['__INTELLISENSE__=1', 'EXPECT_EDITOR=1'],
                        ['-U__TINYC__']
                    ),
                    'EDITOR_VIEW_PASS'
                );
                await execute(
                    await compile(compiler, directory, `${arch}-editor-real.exe`, file, [
                        '__INTELLISENSE__=1',
                        'EXPECT_EDITOR=0',
                        'OBFH_DISABLE_ANTIDEBUG=1'
                    ]),
                    'EDITOR_VIEW_PASS'
                );
                const expanded = await run(compiler, [
                    '-w',
                    '-E',
                    '-D__INTELLISENSE__=1',
                    '-U__TINYC__',
                    snapshotFile(file)
                ]);
                assert(
                    !expanded.stdout.includes('__obfh_sf_variant') && !expanded.stdout.includes('Obfh_VirtualMachine'),
                    'editor still instantiates heavy protection'
                );
            });
            await check(`${arch}/RND formula, ranges and counter capture/all seeds`, async () => {
                for (const seed of [0, 1, 2, 3735928559, 4294967295])
                    await execute(
                        await compile(
                            compiler,
                            directory,
                            `${arch}-random-constants-${seed}.exe`,
                            path.join(__dirname, 'random_constants.c'),
                            ['OBFH_DISABLE_CONTROL_FLOW=1', 'OBFH_DISABLE_ANTIDEBUG=1', `OBFH_SEED=${seed}u`]
                        ),
                        'RANDOM_CONSTANTS_PASS'
                    );
            });
            if (process.argv.includes('--only-preprocessor')) continue;
            if (process.argv.includes('--only-chain') || !process.argv.some((arg) => arg.startsWith('--only-'))) {
                await cflowChain.runSuite({ arch, compiler, directory, source, check, compile, execute, run, assert });
                if (process.argv.includes('--only-chain')) continue;
            }
            if (process.argv.includes('--only-vm') || !process.argv.some((arg) => arg.startsWith('--only-'))) {
                await require('./vm_packing').runSuite({ arch, compiler, directory, source, check, compile, execute, assert });
                await vmKernel.runSuite({
                    arch, compiler, directory, source,
                    fixturesRoot: path.join(snapshotRoot, 'tests'), check, compile, execute, run, assert
                });
                if (process.argv.includes('--only-vm')) continue;
            }
            if (
                process.argv.includes('--only-pdata') ||
                process.argv.includes('--only-integration') ||
                !process.argv.some((arg) => arg.startsWith('--only-'))
            ) {
                await pdataDecoys.runSuite({
                    arch,
                    compiler,
                    directory,
                    source,
                    check,
                    compile,
                    execute,
                    run,
                    assert,
                    afterChecks: (action) => finalizers.push(action)
                });
                if (process.argv.includes('--only-pdata')) continue;
            }
            if (process.argv.includes('--only-flow-tokens') || !process.argv.some((arg) => arg.startsWith('--only-'))) {
                await require('./case_tokens').runSuite({ arch, compiler, directory, source, check, compile, execute, run, assert });
                for (const mode of [0, 1])
                    for (const seed of [0, 1, 2, 0xdeadbeef, 0xffffffff])
                        await check(`${arch}/fast continue/default semantics/cflow-v${mode + 1}/seed ${seed}`, async () =>
                            execute(await compile(compiler, directory, `${arch}-fast-tokens-${mode}-${seed}.exe`,
                                path.join(__dirname, 'fast_tokens.c'),
                                ['OBFH_DISABLE_ANTIDEBUG=1', `OBFH_BETTER_CONTROL_FLOW=${mode}`, `OBFH_SEED=${seed}u`]), 'FAST_TOKENS_PASS')
                        );
                await require('./fast_tokens').runSuite({
                    arch, compiler, directory, source,
                    fixturesRoot: path.join(snapshotRoot, 'tests'), check, compile, execute, run, assert
                });
            }
            if (process.argv.includes('--only-flow-tokens')) {
                for (const mode of [0, 1])
                    await flowTransport.runSuite({ arch, compiler, directory, mode, source, check, compile, execute, run, assert });
                continue;
            }
            if (!process.argv.includes('--only-integration') && !process.argv.includes('--only-configs')) {
                const proxyRoot = path.join(directory, arch + '-stack-proxy');
                fs.mkdirSync(path.join(proxyRoot, 'include'), { recursive: true });
                fs.mkdirSync(path.join(proxyRoot, 'tests'), { recursive: true });
                const proxyHeader = path.join(proxyRoot, 'include', 'obfus.h');
                const proxyFile = path.join(proxyRoot, 'tests', 'stack_proxy.c');
                fs.writeFileSync(proxyFile, stackProxy.fixture());
                const proxyBuilds = new Map(),
                    proxyHistograms = [];
                for (const seed of [0, 1, 2, 3735928559, 4294967295])
                    await check(
                        `${arch}/stack proxies/all ${stackProxy.variantCount} variants + arbitrary guards/seed ${seed}`,
                        async () => {
                            const traced = source
                                .replace(
                                    'OBFH_SF_SELECT(__obfh_sf_variant);',
                                    'obfh_test_sf_visit(__obfh_sf_id, __obfh_sf_variant, __LINE__); OBFH_SF_SELECT(__obfh_sf_variant);'
                                )
                                .replaceAll(
                                    '#define OBFH_SF_INPUT "movl %%esp, %%eax;"',
                                    '#define OBFH_SF_INPUT "movl %[sf_input], %%eax;"'
                                )
                                .replace(
                                    /#define OBFH_SF_INPUTS\s*\\\r?\n/,
                                    '#define OBFH_SF_INPUTS \\\n    [sf_input] "m"(guard_input), \\\n'
                                )
                                .replace(/(#define OBFH_SF_GUARD_INPUTS[ \t]+)/, '$1[sf_input] "m"(guard_input), ')
                                .replace(/(#define OBFH_SF_START_PREFIX[^\n]*\n[^\n]*\n[ \t]*: )/, '$1[sf_input] "m"(guard_input), ');
                            assert(
                                traced !== source && traced.includes('[sf_input] "m"(guard_input)'),
                                'proxy instrumentation target missing'
                            );
                            assert(!source.includes('#define OBFH_SF_GUARD_INPUTS ') || traced.includes('#define OBFH_SF_GUARD_INPUTS [sf_input] "m"(guard_input), '),
                                'guard-only arbitrary-input instrumentation missing');
                            assert(!source.includes('#define OBFH_SF_START_PREFIX ') || /#define OBFH_SF_START_PREFIX[^\n]*\n[^\n]*\n[ \t]*: \[sf_input\]/.test(traced),
                                'guard-prefix arbitrary-input instrumentation missing');
                            fs.writeFileSync(proxyHeader, traced);
                            const flags = ['OBFH_DISABLE_CONTROL_FLOW=1', 'OBFH_DISABLE_ANTIDEBUG=1', `OBFH_SEED=${seed}u`];
                            const output = await execute(
                                await compile(compiler, directory, `${arch}-stack-proxy-${seed}.exe`, proxyFile, flags),
                                'STACK_PROXY_PASS'
                            );
                            proxyHistograms.push(stackProxy.measure(output.stdout, seed, assert).histogram);
                            fs.writeFileSync(proxyHeader, source);
                            const dll = await compile(
                                compiler,
                                directory,
                                `${arch}-stack-proxy-${seed}.dll`,
                                proxyFile,
                                flags,
                                ['-shared']
                            );
                            proxyBuilds.set(seed, junkCode(fs.readFileSync(dll), stackProxy.count));
                        }
                    );
                await check(`${arch}/stack proxies/distribution + seed changes + reproducibility`, async () => {
                    assert(
                        proxyHistograms.every((h) => h.filter(Boolean).length >= 95),
                        'proxy selector collapsed to a few layouts'
                    );
                    assert(
                        Array.from({ length: stackProxy.variantCount }, (_, i) =>
                            proxyHistograms.some((h) => h[i])
                        ).every(Boolean),
                        'a proxy layout was never selected'
                    );
                    const first = proxyBuilds.get(1),
                        second = proxyBuilds.get(2);
                    assert(
                        first && second && first.every((code, i) => !code.equals(second[i])),
                        'proxy payload did not change by seed'
                    );
                    const oldSpillReload = Buffer.from('48894d10488955188b45108b5518', 'hex');
                    const oldLocalPair = Buffer.from('8945fc8955f8', 'hex');
                    assert(
                        first.every((code) => !code.includes(oldLocalPair)),
                        'fixed proxy local-slot pair returned'
                    );
                    if (arch === 'x64')
                        assert(
                            first.every((code) => !code.includes(oldSpillReload)),
                            'proxy spill/immediate-reload signature returned'
                        );
                    if (arch === 'x64') {
                        for (const signature of ['89c8678d0450c3', '89c839d00f8d0200000089d0c3'])
                            assert(
                                first.every((code) => !code.includes(Buffer.from(signature, 'hex'))),
                                'unparameterized leaf-body signature returned'
                            );
                    }
                    fs.writeFileSync(proxyHeader, source);
                    const repeat = await compile(
                        compiler,
                        directory,
                        `${arch}-stack-proxy-repeat.dll`,
                        proxyFile,
                        ['OBFH_DISABLE_CONTROL_FLOW=1', 'OBFH_DISABLE_ANTIDEBUG=1', 'OBFH_SEED=1u'],
                        ['-shared']
                    );
                    assert(
                        junkCode(fs.readFileSync(repeat), stackProxy.count).every((code, i) => code.equals(first[i])),
                        'fixed-seed proxy bytes are not reproducible'
                    );
                    fs.writeFileSync(
                        path.join(directory, `${arch}-stack-proxy-distribution.json`),
                        JSON.stringify(proxyHistograms, null, 2)
                    );
                });
                for (const seed of [1, 2])
                    await check(
                        `${arch}/stack proxies/native leaf/frame/link instructions + scratch stack/seed ${seed}`,
                        async () => {
                            await execute(
                                await compile(
                                    compiler,
                                    directory,
                                    `${arch}-stack-proxy-native-${seed}.exe`,
                                    path.join(root, 'tests', 'stack_proxy_native.c'),
                                    ['OBFH_DISABLE_CONTROL_FLOW=1', 'OBFH_DISABLE_ANTIDEBUG=1', `OBFH_SEED=${seed}u`]
                                ),
                                'STACK_PROXY_NATIVE_PASS'
                            );
                        }
                    );
                await require('./asm_matrix').runSuite({ arch, compiler, directory, source, check, compile, execute, assert });
                await require('./stack_proxy_pool').runSuite({ arch, compiler, directory, check, compile, execute, assert });
                await require('./sf_live_guards').runSuite({
                    arch, compiler, directory, source, fixturesRoot: path.join(snapshotRoot, 'tests'),
                    check, compile, execute, run, assert
                });
                if (process.argv.includes('--only-stack-proxy')) continue;
                await check(
                    `${arch}/API failure branches`,
                    async () =>
                        await execute(
                            await compile(compiler, directory, `${arch}-failures.exe`, failureFile, []),
                            'failure branches passed'
                        )
                );
                const antiRoot = path.join(directory, arch + '-antidebug');
                await check(`${arch}/anti-debug pool: fresh results, failures and late completion`, async () =>
                    await execute(await compile(compiler, directory, `${arch}-antidebug-pool.exe`, poolFailureFile, []), 'ANTIDEBUG_POOL_PASS'));
                await check(`${arch}/anti-debug pool: DLL unload after callback and timeout`, async () => {
                    const dll = await compile(compiler, directory, `${arch}-antidebug-pool.dll`, path.join(__dirname, 'antidebug_pool_dll.c'), ['OBFH_BETTER_ANTIDEBUG=1', 'OBFH_BETTER_CONTROL_FLOW=1'], ['-shared']);
                    const host = await compile(compiler, directory, `${arch}-antidebug-pool-host.exe`, path.join(__dirname, 'antidebug_pool_host.c'), []);
                    await execute(host, 'ANTIDEBUG_POOL_DLL_PASS', [dll]);
                });
                fs.mkdirSync(path.join(antiRoot, 'include'), { recursive: true });
                fs.mkdirSync(path.join(antiRoot, 'tests'), { recursive: true });
                const antiHeader = path.join(antiRoot, 'include', 'obfus.h');
                const antiFile = path.join(antiRoot, 'tests', 'antidebug.c');
                fs.writeFileSync(antiHeader, source);
                fs.copyFileSync(path.join(__dirname, 'antidebug.c'), antiFile);
                const debugHost = await compile(
                    compiler,
                    directory,
                    `${arch}-antidebug-host.exe`,
                    path.join(__dirname, 'antidebug_host.c'),
                    []
                );
                for (const [label, flags] of [
                    ['plain', ['OBFH_DISABLE_ANY_OBFUSCATION=1']],
                    ['disabled', ['OBFH_DISABLE_ANTIDEBUG=1']],
                    ['default', []],
                    ['advanced', ['OBFH_BETTER_ANTIDEBUG=1', 'OBFH_BETTER_CONTROL_FLOW=1']]
                ]) {
                    await check(`${arch}/anti-debug statement syntax and normal execution/${label}`, async () => {
                        const exe = await compile(
                            compiler,
                            directory,
                            `${arch}-antidebug-${label}.exe`,
                            antiFile,
                            flags
                        );
                        await execute(exe, 'ANTIDEBUG_PASS');
                        if (label === 'default' || label === 'advanced') {
                            const binary = fs.readFileSync(exe);
                            assert(
                                !binary.includes(Buffer.from('IsDebuggerPresent\0')),
                                'plain debugger API name/import remains'
                            );
                            await execute(debugHost, 'ANTIDEBUG_REAL_DEBUGGER_PASS', [exe]);
                            for (const route of ['0', '1']) {
                                let stopped;
                                try {
                                    await run(exe, [route], { timeout: 1500 });
                                } catch (error) {
                                    stopped = error;
                                }
                                assert(
                                    stopped?.code === 'ETIMEDOUT' &&
                                    stopped.stdout.includes('RESPONSE_ENTER') &&
                                    !stopped.stdout.includes('RESPONSE_RETURNED'),
                                    'remote response crashed or returned'
                                );
                            }
                        }
                    });
                }
                await check(`${arch}/anti-debug resolver fallback`, async () => {
                    const fallback = source.replace(
                        /if\s*\(check\)\s*return\s+check\(\)\s*!=\s*FALSE;/,
                        'if (0) return check() != FALSE;'
                    );
                    assert(fallback !== source, 'fallback target missing');
                    fs.writeFileSync(antiHeader, fallback);
                    await execute(
                        await compile(compiler, directory, `${arch}-antidebug-fallback.exe`, antiFile, []),
                        'ANTIDEBUG_PASS'
                    );
                });
                await check(`${arch}/anti-debug positive signal dispatch`, async () => {
                    const start = source.indexOf('static inline int IsDebuggerPresent_proxy(void)');
                    const end = source.indexOf('// Live paths', start);
                    assert(start >= 0 && end > start, 'detector target missing');
                    fs.writeFileSync(
                        antiHeader,
                        source.slice(0, start) +
                        'static inline int IsDebuggerPresent_proxy(void) { return 1; }\n\n' +
                        source.slice(end)
                    );
                    const exe = await compile(compiler, directory, `${arch}-antidebug-signal.exe`, antiFile, []);
                    let stopped;
                    try {
                        await run(exe, ['signal'], { timeout: 1500 });
                    } catch (error) {
                        stopped = error;
                    }
                    assert(
                        stopped?.code === 'ETIMEDOUT' &&
                        stopped.stdout.includes('RESPONSE_ENTER') &&
                        !stopped.stdout.includes('RESPONSE_RETURNED'),
                        'positive signal did not reach the remote response'
                    );
                });
                if (process.argv.includes('--only-antidebug')) continue;
                const constantBuilds = new Map();
                for (const [suffix, flags, modes] of [
                    ...[0, 1, 2, 0xdeadbeef, 0xffffffff].map(seed =>
                        [seed, [`OBFH_SEED=${seed}u`], ['groups', 'automatic', 'concurrent']]),
                    ...['OBFH_DISABLE_ANY_OBFUSCATION=1', 'OBFH_DISABLE_CONTROL_FLOW=1', 'OBFH_BETTER_CONTROL_FLOW=1'].map(flag => [flag, [flag], ['concurrent']])
                ])
                    await check(`${arch}/lazy letters: first use, groups and concurrent publication/${suffix}`, async () => {
                        const exe = await compile(
                            compiler,
                            directory,
                            `${arch}-constant-loader-${suffix}.exe`,
                            path.join(__dirname, 'constant_loader.c'),
                            ['OBFH_DISABLE_ANTIDEBUG=1', ...flags]
                        );
                        for (const mode of modes)
                            await execute(exe, 'CONSTANT_LOADER_PASS', [mode]);
                    });
                for (const seed of [0, 1, 2, 3735928559, 4294967295])
                    await check(`${arch}/constant data types and binary signatures/seed ${seed}`, async () => {
                        const exe = await compile(
                            compiler,
                            directory,
                            `${arch}-constants-${seed}.exe`,
                            path.join(__dirname, 'constant_data.c'),
                            ['OBFH_DISABLE_CONTROL_FLOW=1', 'OBFH_DISABLE_ANTIDEBUG=1', `OBFH_SEED=${seed}u`]
                        );
                        const output = await execute(exe, 'CONSTANT_DATA_PASS');
                        const rows = [...output.stdout.matchAll(/^JUNK (\w+) (\d+) ([0-9a-f]+)\r?$/gm)];
                        assert(
                            rows.length === 68 && rows.every((row) => row[3].length === Number(row[2]) * 2),
                            'random data objects missing'
                        );
                        const binary = fs.readFileSync(exe);
                        assertNoConstantSignature(binary);
                        const protectedData = pe(binary).sections.find((section) => section.name === '.obfh');
                        assert(protectedData && protectedData.bytes.length > 512, 'random data was not emitted');
                        constantBuilds.set(seed, rows.map((row) => row[0].trim()).join('\n'));
                    });
                await check(`${arch}/constant data seed changes and exact reproducibility`, async () => {
                    assert(constantBuilds.get(1) !== constantBuilds.get(2), 'seed did not change constant data');
                    const repeated = await compile(
                        compiler,
                        directory,
                        `${arch}-constants-repeat.exe`,
                        path.join(__dirname, 'constant_data.c'),
                        ['OBFH_DISABLE_CONTROL_FLOW=1', 'OBFH_DISABLE_ANTIDEBUG=1', 'OBFH_SEED=1u']
                    );
                    const output = await execute(repeated, 'CONSTANT_DATA_PASS');
                    const rows = [...output.stdout.matchAll(/^JUNK (\w+) (\d+) ([0-9a-f]+)\r?$/gm)];
                    assert(
                        constantBuilds.get(1) === rows.map((row) => row[0].trim()).join('\n'),
                        'fixed-seed constant data differs'
                    );
                });
                if (process.argv.includes('--only-data')) continue;
                if (process.argv.includes('--only-integration')) {
                    await check(
                        `${arch}/default/integration phases + deadline`,
                        async () =>
                            await execute(
                                await compile(
                                    compiler,
                                    directory,
                                    `${arch}-default-integration.exe`,
                                    path.join(__dirname, 'integration.c'),
                                    [],
                                    ['-luser32', '-lgdi32']
                                ),
                                'Full-header stress passed'
                            )
                    );
                    continue;
                }
                await check(
                    `${arch}/working-set option`,
                    async () =>
                        await execute(
                            await compile(
                                compiler,
                                directory,
                                `${arch}-working-set.exe`,
                                path.join(__dirname, 'protection.c'),
                                ['OBFH_DISABLE_CONTROL_FLOW=1', 'OBFH_DISABLE_ANTIDEBUG=1', 'OBFH_MEMORY_CLEANER=1'],
                                ['-luser32']
                            ),
                            'PROTECTION_PASS'
                        )
                );
                if (process.argv.includes('--only-working-set')) continue;
                const junkVariants = new Map();
                for (const seed of [0, 1, 2, 3735928559, 4294967295])
                    await check(`${arch}/seed ${seed}/all junk sites + VM stress`, async () => {
                        const flags = ['OBFH_DISABLE_ANTIDEBUG=1', `OBFH_SEED=${seed}u`, `OBFH_BETTER_CONTROL_FLOW=${seed & 1}`];
                        const file = path.join(__dirname, 'junk.c');
                        await execute(
                            await compile(compiler, directory, `${arch}-junk-${seed}.exe`, file, flags),
                            'JUNK_PASS'
                        );
                        // Keep modes equal for the machine-code diversity comparison.
                        const dllFlags = ['OBFH_DISABLE_ANTIDEBUG=1', `OBFH_SEED=${seed}u`];
                        const dll = await compile(compiler, directory, `${arch}-junk-${seed}.dll`, file, dllFlags, [
                            '-shared'
                        ]);
                        junkVariants.set(seed, junkCode(fs.readFileSync(dll)));
                    });
                await check(`${arch}/junk bytes vary by seed and repeat with a fixed seed`, async () => {
                    const first = junkVariants.get(1),
                        second = junkVariants.get(2);
                    assert(first && second, 'seed stress failed before binary comparison');
                    assert(
                        first.every((code, i) => !code.equals(second[i])),
                        'a junk site did not vary across seeds'
                    );
                    const payloads = [...junkVariants.values()].map((sites) => {
                        const code = sites[0],
                            instruction = code.indexOf(Buffer.from([0x31, 0xc0, 0x0f, 0x84]));
                        const anchor = instruction + 8;
                        assert(instruction >= 0 && code[anchor] === 0xe8, 'legacy skip anchor changed');
                        const destination = anchor + code.readInt32LE(instruction + 4);
                        assert(
                            destination >= anchor + 10 && code[destination] === 0xb8,
                            'junk skip does not land at arithmetic transport'
                        );
                        return code.subarray(anchor + 1, anchor + 5);
                    });
                    assert(
                        new Set(
                            payloads.map((bytes) =>
                                [...bytes]
                                    .slice(1)
                                    .map((b, i) => (b - bytes[i]) & 255)
                                    .join(',')
                            )
                        ).size > 1,
                        'random bytes repeat the same affine pattern'
                    );
                    for (const index of [0, 3, 4, 5, 6, 7, 8]) {
                        const code = first[index],
                            branch = code.indexOf(Buffer.from([0x0f, 0x84]));
                        const { liveBytes } = withoutSkippedPayload(code, branch);
                        assert(
                            !liveBytes.includes(Buffer.from([0x0f, 0xa2])),
                            'serializing instruction remains on a junk live path'
                        );
                    }
                    for (const index of [9, 10, 11, 12]) {
                        const code = first[index],
                            opcode = index === 12 ? 0x85 : 0x84;
                        const branch = code.indexOf(Buffer.from([0x0f, opcode]));
                        const { liveBytes } = withoutSkippedPayload(code, branch);
                        assert(
                            !liveBytes.includes(Buffer.from([0x0f, 0xa2])),
                            'lightweight live path serializes with CPUID'
                        );
                    }
                    const repeat = await compile(
                        compiler,
                        directory,
                        `${arch}-junk-repeat.dll`,
                        path.join(__dirname, 'junk.c'),
                        ['OBFH_DISABLE_ANTIDEBUG=1', 'OBFH_SEED=1u'],
                        ['-shared']
                    );
                    const repeated = junkCode(fs.readFileSync(repeat));
                    assert(
                        first.every((code, i) => code.equals(repeated[i])),
                        'fixed-seed junk code is not reproducible'
                    );
                });
                for (const seed of [0, 1, 2, 3735928559, 4294967295])
                    await check(`${arch}/ASM identities/arbitrary inputs/seed ${seed}`, async () => {
                        await execute(
                            await compile(
                                compiler,
                                directory,
                                `${arch}-predicates-${seed}.exe`,
                                path.join(__dirname, 'break_predicates.c'),
                                ['OBFH_DISABLE_CONTROL_FLOW=1', 'OBFH_DISABLE_ANTIDEBUG=1', `OBFH_SEED=${seed}u`]
                            ),
                            'BREAK_PREDICATES_PASS'
                        );
                    });
                for (const seed of [0, 1])
                    await check(`${arch}/choose_expr/direct and captured RND/seed ${seed}`, async () => {
                        await execute(
                            await compile(
                                compiler,
                                directory,
                                `${arch}-choose-${seed}.exe`,
                                path.join(__dirname, 'choose_random.c'),
                                ['OBFH_DISABLE_CONTROL_FLOW=1', 'OBFH_DISABLE_ANTIDEBUG=1', `OBFH_SEED=${seed}u`]
                            ),
                            'CHOOSE_RANDOM_PASS'
                        );
                    });
                const cflowVariants = new Map();
                for (const seed of [0, 1, 2, 3735928559, 4294967295])
                    await check(`${arch}/CFLOW pool seed ${seed}/128 templates`, async () => {
                        const flags = ['OBFH_DISABLE_CONTROL_FLOW=1', 'OBFH_DISABLE_ANTIDEBUG=1', `OBFH_SEED=${seed}u`];
                        const file = path.join(__dirname, 'cflow_junk.c');
                        await execute(
                            await compile(compiler, directory, `${arch}-cflow-pool-${seed}.exe`, file, flags),
                            'CFLOW_JUNK_PASS'
                        );
                        const dll = await compile(compiler, directory, `${arch}-cflow-pool-${seed}.dll`, file, flags, [
                            '-shared'
                        ]);
                        cflowVariants.set(seed, junkCode(fs.readFileSync(dll), 128));
                    });
                await check(`${arch}/CFLOW pool changes by seed and repeats exactly`, async () => {
                    const first = cflowVariants.get(1),
                        second = cflowVariants.get(2);
                    assert(
                        first && second && first.every((code, i) => !code.equals(second[i])),
                        'CFLOW template did not change with seed'
                    );
                    const dll = await compile(
                        compiler,
                        directory,
                        `${arch}-cflow-repeat.dll`,
                        path.join(__dirname, 'cflow_junk.c'),
                        ['OBFH_DISABLE_CONTROL_FLOW=1', 'OBFH_DISABLE_ANTIDEBUG=1', 'OBFH_SEED=1u'],
                        ['-shared']
                    );
                    const repeated = junkCode(fs.readFileSync(dll), 128);
                    assert(
                        first.every((code, i) => code.equals(repeated[i])),
                        'CFLOW fixed-seed build differs'
                    );
                    assert(
                        new Set(first.map((code) => code.toString('hex'))).size === 128,
                        'duplicate CFLOW machine-code templates'
                    );
                });
                const randomRoot = path.join(directory, `${arch}-break-random`);
                fs.mkdirSync(path.join(randomRoot, 'include'), { recursive: true });
                fs.mkdirSync(path.join(randomRoot, 'tests'), { recursive: true });
                const tracedRandom = source.replace(
                    'OBFH_CFLOW_SELECT(__obfh_break_index);',
                    'obfh_test_break_visit(__obfh_break_id, __obfh_break_index); OBFH_CFLOW_SELECT(__obfh_break_index);'
                );
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
                for (const seed of [0, 1, 2, 3735928559, 4294967295])
                    await check(`${arch}/public break seed ${seed}/512 call sites`, async () => {
                        const flags = ['OBFH_DISABLE_CONTROL_FLOW=1', 'OBFH_DISABLE_ANTIDEBUG=1', 'OBFH_BETTER_CONTROL_FLOW=0', `OBFH_SEED=${seed}u`];
                        const output = await run(
                            await compile(compiler, directory, `${arch}-random-${seed}.exe`, randomFile, flags),
                            []
                        );
                        assert(output.status === 0, 'public break corrupted execution: ' + output.stderr);
                        const measurement = breakRandom.measurements(output.stdout, seed, assert);
                        const dll = await compile(
                            compiler,
                            directory,
                            `${arch}-random-${seed}.dll`,
                            randomCodeFile,
                            flags,
                            ['-shared']
                        );
                        const code = junkCode(fs.readFileSync(dll), breakRandom.siteCount);
                        measurement.distinctMachineCode = new Set(code.map((bytes) => bytes.toString('hex'))).size;
                        assert(
                            measurement.distinctMachineCode >= 500,
                            'public call sites repeat almost identical machine code'
                        );
                        randomBuilds.set(seed, { ...measurement, code });
                    });
                await check(`${arch}/public break distribution + reproducibility`, async () => {
                    assert(randomBuilds.size === 5, 'public selector sampling build failed');
                    const histogram = Array(128).fill(0);
                    let adjacentRepeats = 0;
                    for (const sample of randomBuilds.values()) {
                        sample.histogram.forEach((count, index) => (histogram[index] += count));
                        adjacentRepeats += sample.adjacentRepeats;
                    }
                    assert(
                        histogram.every((count) => count > 0),
                        'not all 128 templates were selected across sampled seeds'
                    );
                    assert(adjacentRepeats < 2560 * 0.04, 'too many adjacent template repetitions');
                    const first = randomBuilds.get(1),
                        second = randomBuilds.get(2);
                    const changedTemplates = first.rows.filter((row, index) => row[2] !== second.rows[index][2]).length;
                    assert(changedTemplates > 460, 'different seed barely changes selection');
                    assert(
                        first.code.every((code, index) => !code.equals(second.code[index])),
                        'seed did not change a public call site'
                    );
                    const flags = ['OBFH_DISABLE_CONTROL_FLOW=1', 'OBFH_DISABLE_ANTIDEBUG=1', 'OBFH_BETTER_CONTROL_FLOW=0', 'OBFH_SEED=1u'];
                    const repeated = junkCode(
                        fs.readFileSync(
                            await compile(compiler, directory, `${arch}-random-repeat.dll`, randomCodeFile, flags, [
                                '-shared'
                            ])
                        ),
                        breakRandom.siteCount
                    );
                    assert(
                        first.code.every((code, index) => code.equals(repeated[index])),
                        'public fixed-seed code is not reproducible'
                    );
                    const report = {
                        samples: 2560,
                        histogram,
                        adjacentRepeats,
                        changedTemplates,
                        builds: [...randomBuilds].map(([seed, { code, ...measurement }]) => ({ seed, ...measurement }))
                    };
                    fs.writeFileSync(
                        path.join(directory, `${arch}-break-random.json`),
                        JSON.stringify(report, null, 2)
                    );
                    console.log(
                        `RANDOM ${arch}: 128/128 templates, ${adjacentRepeats} adjacent repeats, ${changedTemplates}/512 templates changed between seeds 1 and 2`
                    );
                });
                await check(`${arch}/public break OBFH_BETTER_CONTROL_FLOW + syntax`, async () => {
                    for (let index = 0; index < 128; ++index)
                        assert(breakRandom.extraIndex(index) !== index, 'advanced second template equals the first');
                    const output = await run(
                        await compile(compiler, directory, `${arch}-random-v2.exe`, randomFile, [
                            'OBFH_DISABLE_CONTROL_FLOW=1',
                            'OBFH_DISABLE_ANTIDEBUG=1',
                            'OBFH_BETTER_CONTROL_FLOW=1'
                        ]),
                        []
                    );
                    assert(output.status === 0, 'advanced public selector failed');
                    breakRandom.measurements(output.stdout, 0, assert);
                    const syntax = path.join(randomRoot, 'tests', 'syntax.c');
                    fs.writeFileSync(
                        syntax,
                        '#include "../include/obfus.h"\nint main(void) { int x=1; if(x) BREAK_STACK_CFLOW; else x=3; BREAK_STACK_CFLOW; BREAK_STACK_CFLOW; return x != 1; }'
                    );
                    for (const flags of [['OBFH_DISABLE_ANY_OBFUSCATION=1'], ['OBFH_DISABLE_ANTIDEBUG=1'], ['OBFH_DISABLE_ANTIDEBUG=1', 'OBFH_BETTER_CONTROL_FLOW=1']]) {
                        const nativeHeader = path.join(randomRoot, 'include', 'obfus.h');
                        fs.writeFileSync(nativeHeader, source);
                        const result = await run(
                            await compile(
                                compiler,
                                directory,
                                `${arch}-random-syntax-${flags.length}-${flags[0]}.exe`,
                                syntax,
                                flags
                            ),
                            []
                        );
                        assert(result.status === 0, 'public macro breaks dangling else or adjacent expansions');
                    }
                });
                if (process.argv.includes('--only-junk')) continue;
                if (process.argv.includes('--only-numeric')) {
                    for (const [config, flags] of Object.entries(configs))
                        await check(
                            `${arch}/${config}/numeric representation`,
                            async () =>
                                await execute(
                                    await compile(
                                        compiler,
                                        directory,
                                        `${arch}-${config}-numeric.exe`,
                                        path.join(__dirname, 'numeric.c'),
                                        flags
                                    ),
                                    'NUMERIC_PASS'
                                )
                        );
                    continue;
                }
                for (const unicode of [false, true])
                    for (const protectedBuild of [false, true])
                        await check(
                            `${arch}/${unicode ? 'Unicode' : 'ANSI'}/${protectedBuild ? 'protected' : 'plain'}`,
                            async () => {
                                const flags = [
                                    ...(unicode ? ['UNICODE=1', '_UNICODE=1'] : []),
                                    ...(protectedBuild ? ['OBFH_DISABLE_ANTIDEBUG=1', 'OBFH_BETTER_CONTROL_FLOW=1'] : ['OBFH_DISABLE_ANY_OBFUSCATION=1'])
                                ];
                                await execute(
                                    await compile(
                                        compiler,
                                        directory,
                                        `${arch}-charset-${unicode}-${protectedBuild}.exe`,
                                        path.join(__dirname, 'charset.c'),
                                        flags,
                                        ['-luser32']
                                    ),
                                    'CHARSET_PASS'
                                );
                            }
                        );
                for (const [config, flags] of Object.entries(configs)) {
                    const files = ['multi_a.c', 'multi_b.c', 'multi_main.c'].map((name) =>
                        path.join(__dirname, name)
                    );
                    await check(`${arch}/${config}/multiple translation units + exports/EXE`, async () => {
                        const exe = await compile(
                            compiler,
                            directory,
                            `${arch}-${config}-multi.exe`,
                            files[0],
                            flags,
                            files.slice(1)
                        );
                        await execute(exe, 'MULTI_PASS');
                    });
                    await check(`${arch}/${config}/multiple translation units + exports/DLL`, async () => {
                        const dll = await compile(compiler, directory, `${arch}-${config}-multi.dll`, files[0], flags, [
                            files[1],
                            '-shared'
                        ]);
                        const exports = fs.readFileSync(dll.replace(/\.dll$/, '.def'), 'utf8');
                        assert(
                            (exports.match(/\bmulti_export\b/g) || []).length === 1,
                            'public export missing or duplicated'
                        );
                        if (config !== 'plain')
                            assert(
                                (exports.match(/\bWhatSoundDoesACowMake\b/g) || []).length === 1,
                                'cow export missing or duplicated'
                            );
                        assert(
                            !exports.includes('obfh_flow') && !exports.includes('obfh_crt'),
                            'internal helper leaked into exports'
                        );
                        const client = await getDllHost(compiler, arch, config, flags);
                        await execute(client, 'MULTI_DLL_PASS', ['multi_client', dll]);
                    });
                }
                if (process.argv.includes('--only-platform')) continue;
                await check(`${arch}/CRT per-site cache concurrency, lifetime and variable names`, async () => {
                    const traceRoot = path.join(directory, arch + '-cache');
                    fs.mkdirSync(path.join(traceRoot, 'include'), { recursive: true });
                    fs.mkdirSync(path.join(traceRoot, 'tests'), { recursive: true });
                    const target = 'FARPROC function = GetProcAddress(module, name);';
                    const traced = source.replace(target, 'obfh_test_crt_cold();\n    ' + target);
                    assert(traced !== source, 'CRT trace injection missing');
                    fs.writeFileSync(path.join(traceRoot, 'include', 'obfus.h'), traced);
                    const file = path.join(traceRoot, 'tests', 'cache.c');
                    fs.copyFileSync(path.join(__dirname, 'cache.c'), file);
                    await execute(
                        await compile(compiler, directory, arch + '-cache.exe', file, ['OBFH_DISABLE_ANTIDEBUG=1', 'OBFH_BETTER_CONTROL_FLOW=1']),
                        'CACHE_PASS'
                    );
                    const mutant = traced.replace('if (i > length) {', 'if (0) {');
                    assert(mutant !== traced, 'CRT cache bypass target missing');
                    fs.writeFileSync(path.join(traceRoot, 'include', 'obfus.h'), mutant);
                    const result = await run(
                        await compile(compiler, directory, arch + '-cache-bypass.exe', file, [
                            'OBFH_DISABLE_ANTIDEBUG=1',
                            'OBFH_BETTER_CONTROL_FLOW=1'
                        ]),
                        []
                    );
                    assert(
                        result.status === 1 && result.stderr.includes('cache failure'),
                        'cache bypass was not detected'
                    );
                });
                if (process.argv.includes('--only-cache')) continue;
                for (const mode of [0, 1])
                    await flowTransport.runSuite({ arch, compiler, directory, mode, source, check, compile, execute, run, assert });
                if (process.argv.includes('--only-cflow')) continue;
                await check(`${arch}/negative control: disabled VM must fail`, async () => {
                    const mutantRoot = path.join(directory, arch + '-mutant');
                    fs.mkdirSync(path.join(mutantRoot, 'include'), { recursive: true });
                    fs.mkdirSync(path.join(mutantRoot, 'tests'), { recursive: true });
                    const mutant = replaceProtectedMacro(source, 'VM_ADD', '#define VM_ADD(a, b) ((a) + (b))');
                    fs.writeFileSync(path.join(mutantRoot, 'include', 'obfus.h'), mutant);
                    const file = path.join(mutantRoot, 'tests', 'vm.c');
                    fs.copyFileSync(path.join(__dirname, 'vm.c'), file);
                    const exe = await compile(compiler, directory, arch + '-mutant.exe', file, [
                        'OBFH_DISABLE_ANTIDEBUG=1'
                    ]);
                    const result = await run(exe, []);
                    assert(
                        result.status === 1 && result.stderr.includes('VM failure'),
                        'VM bypass was not detected by the runtime test'
                    );
                });
                await check(`${arch}/VM branch trace and bypass mutations`, async () => {
                    const tracedRoot = path.join(directory, arch + '-branch-trace');
                    fs.mkdirSync(path.join(tracedRoot, 'include'), { recursive: true });
                    fs.mkdirSync(path.join(tracedRoot, 'tests'), { recursive: true });
                    const testFile = path.join(tracedRoot, 'tests', 'vm_branches.c');
                    fs.copyFileSync(path.join(__dirname, 'vm_branches.c'), testFile);
                    let traced = source
                        .replace('/* OBFH_VM_TRACE_ENTER */', 'obfh_test_vm_enter();')
                        .replace('/* OBFH_VM_TRACE_STEP */', 'obfh_test_vm_step(op,at);');
                    traced = traced.replace(
                        'static inline long double Obfh_VirtualMachine(',
                        'void obfh_test_vm_enter(void);\nvoid obfh_test_vm_step(unsigned int op,unsigned int pc);\nstatic inline long double Obfh_VirtualMachine('
                    );
                    assert(traced.includes('obfh_test_vm_step(op,at);'), 'instruction tracing missing');
                    const headerFile = path.join(tracedRoot, 'include', 'obfus.h');
                    fs.writeFileSync(headerFile, traced);
                    const traceFlags = ['OBFH_DISABLE_CONTROL_FLOW=1', 'OBFH_DISABLE_ANTIDEBUG=1', 'OBFH_TEST_BRANCH_TRACE=1'];
                    await execute(
                        await compile(compiler, directory, arch + '-branch-trace.exe', testFile, traceFlags),
                        'BRANCH_PASS'
                    );
                    const mutants = [
                        ['VM_IF', '#define VM_IF(condition) if (condition)'],
                        ['VM_ELSE_IF', '#define VM_ELSE_IF(condition) else if (condition)'],
                        ['VM_ELSE', '#define VM_ELSE else']
                    ];
                    for (const [name, replacement] of mutants) {
                        assert(
                            protectedMacro(traced, name).body.includes('OBFH_VM_EXEC('),
                            'branch mutant target missing: ' + name
                        );
                        fs.writeFileSync(headerFile, replaceProtectedMacro(traced, name, replacement));
                        const exe = await compile(
                            compiler,
                            directory,
                            arch + '-' + name + '-bypass.exe',
                            testFile,
                            traceFlags
                        );
                        const result = await run(exe, []);
                        assert(
                            result.status === 1 && result.stderr.includes('branch failure'),
                            'VM bypass not detected: ' + name
                        );
                    }
                });
                if (process.argv.includes('--only-branches')) continue;
            }
            const semanticsFile = vmSemantics.writeFixture(path.join(directory, arch + '-semantic-contracts'),
                source, path.join(snapshotRoot, 'tests'));
            for (const [config, flags] of Object.entries(configs)) {
                if (selectedConfigs && !selectedConfigs.includes(config)) continue;
                const label = `${arch}/${config}`;
                await check(`${label}/semantic contracts`, async () => {
                    const exe = await compile(compiler, directory, `${arch}-${config}-semantic-contracts.exe`,
                        semanticsFile, flags, ['-luser32', '-lgdi32']);
                    await vmSemantics.executeContracts(exe, execute);
                });
                for (const [file, marker] of [
                    ['algorithms', 'ALGORITHMS_PASS'],
                    ['wrappers', 'regressions passed'],
                    ['window', 'WINDOW_PASS'],
                    ['path_limits', 'PATHS_PASS'],
                    ['protection', 'PROTECTION_PASS']
                ]) {
                    await check(
                        `${label}/${file}`,
                        async () =>
                            await execute(
                                await compile(
                                    compiler,
                                    directory,
                                    `${arch}-${config}-${file}.exe`,
                                    path.join(__dirname, file + '.c'),
                                    flags,
                                    ['-luser32', '-lgdi32']
                                ),
                                marker
                            )
                    );
                }
                if (!flags.includes('OBFH_DISABLE_ANY_OBFUSCATION=1'))
                    await check(
                        `${label}/per-site API cache isolation + concurrent first use`,
                        async () =>
                            await execute(
                                await compile(
                                    compiler,
                                    directory,
                                    `${arch}-${config}-api-site-cache.exe`,
                                    path.join(__dirname, 'api_site_cache.c'),
                                    flags,
                                    ['-luser32', '-lgdi32']
                                ),
                                'SITE_CACHE_PASS'
                            )
                    );
                await check(`${label}/GUI cache + ABI + callback reentry`, async () => {
                    const exe = await compile(
                        compiler,
                        directory,
                        `${arch}-${config}-gui-calls.exe`,
                        path.join(__dirname, 'gui_calls.c'),
                        flags,
                        ['-luser32', '-lgdi32']
                    );
                    await execute(exe, 'GUI_CALLS_PASS');
                    if (!flags.includes('OBFH_DISABLE_ANY_OBFUSCATION=1'))
                        assert((await run(exe, ['x'])).status >>> 0 === 0xe0bf4701,
                            'invalid GUI cache input did not fail fast');
                });
                await check(`${label}/GUI cache + ABI/inline nesting and imports`, async () => {
                    const inline = await compile(
                        compiler,
                        directory,
                        `${arch}-${config}-gui-inline.exe`,
                        path.join(__dirname, 'gui_inline.c'),
                        flags,
                        ['-luser32', '-lgdi32']
                    );
                    assert((await run(inline, [])).status === 0, 'GUI warm decode or macro nesting failed');
                    if (!flags.includes('OBFH_DISABLE_ANY_OBFUSCATION=1')) {
                        const modules = importModules(fs.readFileSync(inline));
                        assert(
                            !modules.includes('user32.dll') && !modules.includes('gdi32.dll'),
                            'covered GUI calls retain imports'
                        );
                    }
                });
                await check(`${label}/KERNEL32 cache + errors + memory + termination`, async () => {
                    const exe = await compile(
                        compiler,
                        directory,
                        `${arch}-${config}-kernel-calls.exe`,
                        path.join(__dirname, 'kernel_calls.c'),
                        flags,
                        ['-luser32', '-lgdi32']
                    );
                    await execute(exe, 'KERNEL_CALLS_PASS');
                    assert((await run(exe, ['x'])).status === 23, 'cached ExitProcess changed exit code');
                });
                await check(`${label}/files + memory + paths + threads + painting + registry`, async () => {
                    const exe = await compile(
                        compiler,
                        directory,
                        `${arch}-${config}-api-calls.exe`,
                        path.join(__dirname, 'api_calls.c'),
                        flags,
                        ['-luser32', '-lgdi32', '-ladvapi32']
                    );
                    await execute(exe, 'API_CALLS_PASS');
                });
                await check(`${label}/extended WinAPI exports + calls`, async () => {
                    const exe = await compile(
                        compiler,
                        directory,
                        `${arch}-${config}-winapi-extended.exe`,
                        path.join(__dirname, 'winapi_extended.c'),
                        flags,
                        ['-luser32', '-lgdi32']
                    );
                    await execute(exe, 'WINAPI_EXTENDED_PASS');
                });
                // Compatible semantics share one header build but retain fresh processes.
                // Poisoned keywords, callbacks and lifetime contracts stay isolated.
                await check(`${label}/CRT proxy calls/shared semantic bundle`, async () => {
                    await crtBundle.runSuite({
                        compiler, directory,
                        name: `${arch}-${config}-crt-bundle`, flags,
                        header: path.join(snapshotRoot, 'include', 'obfus.h'),
                        fixturesRoot: path.join(snapshotRoot, 'tests'), compile, run, assert
                    });
                });
                await check(`${label}/CRT native-keyword numeric and sorting bundle`, async () => {
                    await crtBundle.runNumericSuite({
                        compiler, directory,
                        name: `${arch}-${config}-crt-numeric`, flags,
                        header: path.join(snapshotRoot, 'include', 'obfus.h'),
                        fixturesRoot: path.join(snapshotRoot, 'tests'), compile, run, assert
                    });
                });
                for (const [fixture, marker] of [
                    ['custom_inline', ''],
                    ...(config === 'plain' ? [] : [['custom33_inline', '']])
                ])
                    await check(`${label}/CRT proxy calls/${fixture}`, async () => {
                        const exe = await compile(compiler, directory,
                            `${arch}-${config}-${fixture.replaceAll('_', '-')}.exe`,
                            path.join(__dirname, fixture + '.c'), flags);
                        await execute(exe, marker);
                    });
                await check(`${label}/CRT proxy calls + atexit + input`, async () => {
                    const exe = await compile(
                        compiler,
                        directory,
                        `${arch}-${config}-crt-proxies.exe`,
                        path.join(__dirname, 'crt_proxies.c'),
                        flags
                    );
                    await execute(exe, 'CRT_ATEXIT_PASS');
                    const chars = await run(exe, ['chars'], { input: 'Q' });
                    assert(chars.status === 0 && chars.stdout === 'Z', 'getchar/putchar/EOF contract failed');
                    const errors = await run(exe, ['perror']);
                    const messages = errors.stderr.trim().split(/\r?\n/);
                    assert(
                        errors.status === 0 &&
                        messages.length === 2 &&
                        messages[0].startsWith('OBFH_PERROR_TEST:') &&
                        messages[0] === messages[1],
                        'perror changed caller errno during resolution'
                    );
                    const failure = await run(exe, ['puts-error']);
                    assert(failure.status === 0, 'puts error return or stream error flag failed');
                    const output = await run(exe, ['puts']);
                    assert(
                        output.status === 0 &&
                        output.stdout.replace(/\r\n/g, '\n') === 'literal %s %n %%\n\nline\n\nCRT_PUTS_PASS\n',
                        'puts output or single evaluation failed'
                    );
                    const input = await run(exe, ['gets'], { input: 'proxy-input\n' });
                    assert(input.status === 0 && input.stdout.includes('CRT_GETS_PASS'), 'protected gets input failed');
                });
                await check(`${label}/default-integration keygen`, async () => {
                    const exe = await compile(
                        compiler,
                        directory,
                        `${arch}-${config}-keygen.exe`,
                        path.join(__dirname, 'keygen_demo.c'),
                        flags
                    );
                    await execute(exe, 'DCD48287-ACFB1ECA-576C2D3E-E3459984');
                    await execute(exe, 'STRESS_PASS B2CA27B1', ['--stress']);
                    const empty = await run(exe, [' -- ']);
                    assert(empty.status === 1 && empty.stdout === '', 'empty normalized name was accepted');
                });
                await check(`${label}/stdin + CRT streams`, async () => {
                    const exe = await compile(
                        compiler,
                        directory,
                        `${arch}-${config}-streams.exe`,
                        path.join(__dirname, 'streams.c'),
                        flags
                    );
                    const result = await run(exe, [path.join(directory, `${arch}-${config}-io.tmp`)], {
                        input: '42 automated\n'
                    });
                    assert(
                        result.status === 0 && result.stdout.includes('STREAMS_PASS'),
                        `exit=${result.status}: ${result.stderr}`
                    );
                });
                await check(`${label}/real-header concurrent stress`, async () => {
                    const exe = await compile(
                        compiler,
                        directory,
                        `${arch}-${config}-integration.exe`,
                        path.join(__dirname, 'integration.c'),
                        flags,
                        ['-luser32', '-lgdi32']
                    );
                    await execute(exe, 'Full-header stress passed');
                });
                await check(`${label}/DLL exports + shared data`, async () => {
                    const dll = await compile(
                        compiler,
                        directory,
                        `${arch}-${config}-fixture.dll`,
                        path.join(__dirname, 'fixture.c'),
                        flags,
                        ['-shared']
                    );
                    const client = await getDllHost(compiler, arch, config, flags);
                    await execute(client, 'DLL_PASS', ['dll_client', dll]);
                    assert(
                        pe(fs.readFileSync(dll)).machine === (arch === 'x64' ? 0x8664 : 0x14c),
                        'wrong DLL architecture'
                    );
                    const definition = dll.replace(/\.dll$/, '.def');
                    assert(fs.existsSync(definition), 'DLL import definition missing');
                    const imported = await compile(
                        compiler,
                        directory,
                        `${arch}-${config}-import.exe`,
                        path.join(__dirname, 'dll_import.c'),
                        flags,
                        [definition]
                    );
                    await execute(imported, 'DLL_IMPORT_PASS');
                });
                await check(`${label}/hidden-string binary + timestamp`, async () => {
                    const exe = await compile(
                        compiler,
                        directory,
                        `${arch}-${config}-probe.exe`,
                        path.join(__dirname, 'probe.c'),
                        flags
                    );
                    const marker = 'obfh_unique_hidden_probe_8a39b17f';
                    await execute(exe, marker);
                    const binary = fs.readFileSync(exe),
                        data = pe(binary);
                    assert(data.machine === (arch === 'x64' ? 0x8664 : 0x14c), 'wrong executable architecture');
                    const literal = binary.includes(Buffer.from(marker));
                    assert(config === 'plain' ? literal : !literal, 'hidden string binary check failed');
                    if (config !== 'plain') {
                        assertNoConstantSignature(binary);
                        for (const name of ['GetProcAddress', 'LoadLibraryA', 'abs', 'memchr', 'vprintf'])
                            assert(!binary.includes(Buffer.from('\0' + name + '\0')), 'native import leaked: ' + name);
                        assert(binary.includes(Buffer.from([0x0f, 0x31])), 'timestamp read missing from executable');
                        const protectedSection = data.sections.find(
                            (section) => section.name === (flags.includes('OBFH_FAKE_SIGNATURES=1') ? 'UPX0' : '.obfh')
                        );
                        assert(protectedSection, 'protected data section missing');
                        const executableSections = data.sections.filter((section) => section.executable);
                        if (arch === 'x64') {
                            assert(
                                executableSections.some((section) =>
                                    section.bytes.includes(Buffer.from([0xf3, 0x0f, 0x2a]))
                                ),
                                'integer-to-float SSE conversion missing'
                            );
                            assert(
                                executableSections.some((section) =>
                                    section.bytes.includes(Buffer.from([0xf3, 0x0f, 0x2c]))
                                ),
                                'float-to-integer SSE conversion missing'
                            );
                        }
                    }
                });
            }
            for (const flags of [[], ['OBFH_DISABLE_ANY_OBFUSCATION=1']]) {
                await check(
                    `${arch}/math string and pointer arguments/${flags.length ? 'native' : 'protected'}`,
                    async () =>
                        await execute(
                            await compile(
                                compiler,
                                directory,
                                `${arch}-math-${flags.length}.exe`,
                                path.join(__dirname, 'math.c'),
                                flags
                            ),
                            'Math string/pointer arguments passed'
                        )
                );
            }
        }
        const progress = setInterval(
            () =>
                console.log(
                    `RUN suite: ${results.length}/${checkIndex} checks completed; ${activeChildren.size} child processes active`
                ),
            10000
        );
        try {
            await pool.drain();
        } finally {
            clearInterval(progress);
        }
        for (const action of finalizers) action();
        await performCheck('header remained unchanged during the run', async () =>
            assert(
                fs.readFileSync(headerFile, 'utf8') === source,
                'header changed while the suite was running; rerun against an immutable header'
            )
        );
    } catch (error) {
        if (pool) {
            try {
                await pool.drain();
            } catch {
                /* Tasks already recorded their failures. */
            }
        }
        results.push({ name: 'suite setup', pass: false, error: error.message });
        console.error(error.message);
    }
    results.sort((a, b) => (order.get(a.name) ?? Infinity) - (order.get(b.name) ?? Infinity));
    const failed = results.filter((result) => !result.pass);
    if (directory)
        fs.writeFileSync(
            path.join(directory, 'results.json'),
            JSON.stringify(
                {
                    durationMs: Date.now() - suiteStarted,
                    compilationCount,
                    parallelism: pool && {
                        workers: pool.limit, peak: pool.peak,
                        builds: { workers: buildPool.limit, peak: buildPool.peak },
                        executions: { workers: executionPool.limit, peak: executionPool.peak },
                        processes: { workers: processPool.limit, peak: processPool.peak }
                    },
                    results,
                    passed: results.length - failed.length,
                    failed: failed.length
                },
                null,
                2
            )
        );
    console.log(
        `\n${failed.length ? 'FAIL' : 'PASS'}: ${results.length - failed.length}/${results.length} checks; ${failed.length} failures.`
    );
    console.log('Verdict applies to the tested compiler builds and Windows environment; invalid C remains invalid.');
    if (directory) console.log(`Logs and binaries: ${directory}`);
    process.exitCode = failed.length ? 1 : 0;
}
main().catch((error) => {
    console.error(error);
    process.exitCode = 1;
});
