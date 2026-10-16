'use strict';
const fs = require('node:fs');
const path = require('node:path');

exports.runSuite = async function ({ arch, compiler, directory, mode, source, check, compile, execute, run, assert }) {
    for (const part of [0, 1, 2, 0xdeadbeef, 0xffffffff, 'if', 'while', 'for', 'token', 'proxy', 'oracle', 'stage'])
        await check(`${arch}/cflow-v${mode + 1} tokens, semantics and bypass control/${part}`, async () =>
            checkFlowTransport({ arch, compiler, directory, mode, part, source, compile, execute, run, assert })
        );
};
async function checkFlowTransport({ arch, compiler, directory, mode, part, source, compile, execute, run, assert }) {
    assert([0, 1, 2, 0xdeadbeef, 0xffffffff, 'if', 'while', 'for', 'token', 'proxy', 'oracle', 'stage'].includes(part),
        'unknown flow transport scenario');
    const traceRoot = path.join(directory, `${arch}-cflow-${mode}-${part}`);
    fs.mkdirSync(path.join(traceRoot, 'include'), { recursive: true });
    fs.mkdirSync(path.join(traceRoot, 'tests'), { recursive: true });
    const traced = source
        .replace(
            '#define OBFH_FLOW_CONDITION(',
            require('./cflow_single_lane').trace + '\n#define OBFH_FLOW_CONDITION('
        )
        .replace(
            'OBFH_CFLOW_SELECT(__obfh_break_index);',
            'obfh_test_if_junk_visit(__obfh_break_index); OBFH_CFLOW_SELECT(__obfh_break_index);'
        )
        .replace('OBFH_P_PROXY; BREAK_STACK_CFLOW', 'obfh_test_proxy_if_visit(); OBFH_P_PROXY; BREAK_STACK_CFLOW')
        .replace(
            '__obfh_flow_state ^= __obfh_live_actual;',
            'obfh_test_flow_proxy(__obfh_live_expected, __obfh_live_actual); __obfh_flow_state ^= __obfh_live_actual;'
        )
        .replace(
            '    OBFH_SF_FLOW_COMMIT;',
            '    obfh_test_flow_proxy(__obfh_live_expected ^ __obfh_live_input, __obfh_live_actual); OBFH_SF_FLOW_COMMIT;'
        )
        .replace(
            /#define OBFH_P_FINISH\(style\)[\s\S]*?(?=\r?\n#define)/,
            '#define OBFH_P_FINISH(style) ({ unsigned int __obfh_exit_before_state=__obfh_flow_state, __obfh_exit_before_tag=__obfh_flow_tag; OBFH_P_EXIT_SELECT_0(style); obfh_test_flow_exit((style)&7u,__obfh_exit_before_state,__obfh_exit_before_tag,__obfh_flow_result); })\n'
        );
    assert(source.includes('#define OBFH_C_FIELDS('), 'compositional flow core missing');
    assert(traced !== source, 'flow trace injection missing');
    const header = path.join(traceRoot, 'include', 'obfus.h');
    fs.writeFileSync(header, traced);
    const file = path.join(traceRoot, 'tests', 'cflow.c');
    fs.copyFileSync(path.join(__dirname, 'cflow.c'), file);
    const flags = [`OBFH_BETTER_CONTROL_FLOW=${mode}`, 'OBFH_DISABLE_ANTIDEBUG=1', 'OBFH_TEST_FLOW_TRACE=1'];
    if (source.includes('#define OBFH_P_COUPLED_FIRST(') || source.includes('#define OBFH_P_COUPLED_CODE('))
        flags.push('OBFH_TEST_COUPLED_PRIMITIVES=1');
    for (const seed of typeof part === 'number' ? [part] : []) {
        await execute(
            await compile(compiler, directory, `${arch}-cflow-${mode}-seed-${seed}.exe`, file, [
                ...flags,
                `OBFH_SEED=${seed}u`
            ]),
            'CFLOW_PASS'
        );
        await execute(
            await compile(
                compiler,
                directory,
                `${arch}-cflow-${mode}-proxy-${seed}.exe`,
                path.join(__dirname, 'cflow_proxy.c'),
                [...flags.filter((flag) => flag !== 'OBFH_TEST_FLOW_TRACE=1'), `OBFH_SEED=${seed}u`]
            ),
            'FLOW_PROXY_PASS'
        );
    }
    if (typeof part === 'number') return;
    const ifMutant = traced.replace(
        /^#define if\((?:cond|\.\.\.)\)[^\r\n]*(?:\\\r?\n[^\r\n]*)*/gm,
        '#define if(...) if (__VA_ARGS__)'
    );
    assert(ifMutant !== traced, 'if bypass target missing');
    if (part === 'if') {
        fs.writeFileSync(header, ifMutant);
        const result = await run(await compile(compiler, directory, `${arch}-cflow-${mode}-bypass.exe`, file, flags), []);
        assert(result.status === 1 && result.stderr.includes('cflow failure'), 'ordinary-if bypass was not detected');
    }
    const whileMutant = traced.replace(
        '#define while(...) while (OBFUS_CONDITION_BLOCK((__VA_ARGS__)))',
        '#define while(...) while (__VA_ARGS__)'
    );
    assert(whileMutant !== traced, 'while bypass target missing');
    if (part === 'while') {
        fs.writeFileSync(header, whileMutant);
        const whileResult = await run(
            await compile(compiler, directory, `${arch}-cflow-${mode}-while-bypass.exe`, file, flags),
            []
        );
        assert(
            whileResult.status === 1 && whileResult.stderr.includes('cflow failure'),
            'ordinary-while bypass was not detected'
        );
    }
    const forMutant = traced.replace(
        /#define for\(\.\.\.\)[\s\S]*?(?=\r?\n\r?\n)/,
        '#define for(...) for (__VA_ARGS__)'
    );
    assert(forMutant !== traced, 'for bypass target missing');
    if (part === 'for') {
        fs.writeFileSync(header, forMutant);
        const forResult = await run(
            await compile(compiler, directory, `${arch}-cflow-${mode}-for-bypass.exe`, file, flags),
            []
        );
        assert(
            forResult.status === 1 && forResult.stderr.includes('cflow failure'),
            'per-iteration for bypass was not detected'
        );

    }
    // Mutate user-site transport after the header's helpers have been compiled.
    // Corrupting loader/CRT startup instead only tests an unrelated early crash.
    const transportStart = traced.indexOf('#define OBFH_FLOW_CONDITION(');
    let transportEnd = transportStart;
    do {
        const next = traced.indexOf('\n', transportEnd);
        const continued = traced.slice(transportEnd, next).trimEnd().endsWith(String.fromCharCode(92));
        transportEnd = next + 1;
        if (!continued) break;
    } while (transportEnd > 0);
    const transport = traced.slice(transportStart, transportEnd);
    const tokenMutant = transport.replace(
        /unsigned int __obfh_flow_state[\\\s]*=[\s\S]*?(?=unsigned int __obfh_flow_tag)/,
        'unsigned int __obfh_flow_state = __obfh_false_tag; (void)(condition); \\\n        '
    );
    assert(tokenMutant !== transport, 'encoded condition mutation target missing');
    if (part === 'token') {
        fs.writeFileSync(header, traced + '\n#undef OBFH_FLOW_CONDITION\n' + tokenMutant);
        const tokenResult = await run(
            await compile(compiler, directory, `${arch}-cflow-${mode}-token-bypass.exe`, file, flags),
            []
        );
        assert(
            tokenResult.status === 1 && tokenResult.stderr.includes('cflow failure'),
            'discarded condition transport was not detected'
        );
    }
    const proxyEmit = traced.match(/#define OBFH_SF_FLOW_START\(guard\)[\s\S]*?(?=\r?\n#define)/)?.[0];
    assert(proxyEmit, 'linked proxy guard mutation target missing');
    // Preserve operands and the finishing/commit operations; bypass only the
    // guard's computation. The untraced oracle must detect its lost dependency.
    const strippedProxy = proxyEmit.replace(' guard ', ' "jmp 9f;" ')
        .replace('__obfh_asm__(guard', '__obfh_asm__("jmp 9f;"');
    assert(strippedProxy !== proxyEmit, 'native guard removal target missing');
    if (part === 'proxy') {
        fs.writeFileSync(header, traced + '\n#undef OBFH_SF_FLOW_START\n' + strippedProxy + '\n');
        const proxyResult = await run(
            await compile(compiler, directory, `${arch}-cflow-${mode}-proxy-bypass.exe`, file, flags),
            []
        );
        assert(
            proxyResult.status === 1 && proxyResult.stderr.includes('cflow failure'),
            'removed native proxy guard was not detected'
        );
    }
    // The oracle uses ordinary C keywords and no instrumentation callbacks.
    const oracleProxy = strippedProxy.replace('obfh_test_flow_proxy(__obfh_live_expected, __obfh_live_actual); ', '');
    if (part === 'oracle') {
        fs.writeFileSync(header, source + '\n#undef OBFH_SF_FLOW_START\n' + oracleProxy + '\n');
        const proxyOracle = path.join(traceRoot, 'tests', 'cflow_proxy.c');
        fs.copyFileSync(path.join(__dirname, 'cflow_proxy.c'), proxyOracle);
        const proxyOracleResult = await run(
            await compile(
                compiler,
                directory,
                `${arch}-cflow-${mode}-proxy-oracle.exe`,
                proxyOracle,
                flags.filter((flag) => flag !== 'OBFH_TEST_FLOW_TRACE=1')
            ),
            []
        );
        assert(
            proxyOracleResult.status === 1 && !proxyOracleResult.stdout.includes('FLOW_PROXY_PASS'),
            'guard removal preserved the untraced condition-state contract'
        );
    }
    const permutation = traced.match(/#define OBFH_P_STEP\(s,\s*p\)[\s\S]*?(?=\r?\n#define)/)?.[0];
    assert(permutation, 'stage mutation target missing');
    if (part === 'stage') {
        fs.writeFileSync(header, traced + '\n#undef OBFH_P_STEP\n#define OBFH_P_STEP(s,p) ((void)0)\n');
        const stageResult = await run(
            await compile(compiler, directory, arch + '-cflow-' + mode + '-stage-bypass.exe', file, flags),
            []
        );
        assert(
            stageResult.status === 1 && stageResult.stderr.includes('cflow failure'),
            'skipped permutation was not detected'
        );
    }

}

