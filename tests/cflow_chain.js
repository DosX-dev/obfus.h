'use strict';
const fs = require('node:fs');
const path = require('node:path');
const pdata = require('./pdata_decoys');

function trace(source) {
    const start = source.includes('#define OBFH_PD_LIVE_OPERANDS')
        ? source.indexOf('#define OBFH_PD_LIVE_OPERANDS')
        : source.indexOf('#define OBFH_PD_LIVE_ASM(');
    const end = source.indexOf('#define OBFH_PD_LIVE_SELECT(', start);
    if (start < 0 || end < start) throw new Error('live native carrier missing');
    let body = source.slice(start, end);
    if (body.includes('#define OBFH_PD_LIVE_RETURN')) {
        // Current post transforms preserve the input parity; count either
        // completion route after its result, including branchless completion.
        body = body
            .replaceAll('movl %%ecx, %%eax;', 'lock incl %[visit]; movl %%ecx, %%eax;')
            .replace(
                '#define OBFH_PD_LIVE_RETURN "',
                '#define OBFH_PD_LIVE_RETURN "testl $1, %%eax; jz 98f; lock incl %[odd]; jmp 99f; 98: lock incl %[even]; 99: '
            );
    } else
        body = body
            .replaceAll('"movq %%rdx, -8(%%rbp);', '"lock incl %[visit]; movq %%rdx, -8(%%rbp);')
            .replaceAll('jz 2f; xorl', 'jz 2f; lock incl %[odd]; xorl')
            .replaceAll('"2: imull', '"2: lock incl %[even]; imull')
            .replaceAll(
                '[frame] "i"(__obfh_pd_frame),',
                '[visit] "m"(obfh_test_chain_entries[OBFH_PD_LIVE_INDEX(__obfh_pd_site)]), [odd] "m"(obfh_test_chain_routes[2u * OBFH_PD_LIVE_INDEX(__obfh_pd_site) + 1u]), [even] "m"(obfh_test_chain_routes[2u * OBFH_PD_LIVE_INDEX(__obfh_pd_site)]), [frame] "i"(__obfh_pd_frame),'
            );
    if (body.includes('#define OBFH_PD_LIVE_RETURN'))
        body = body.replace(
            '[frame] "i"(__obfh_pd_frame),',
            '[visit] "m"(obfh_test_chain_entries[OBFH_PD_LIVE_INDEX(__obfh_pd_site)]), [odd] "m"(obfh_test_chain_routes[2u * OBFH_PD_LIVE_INDEX(__obfh_pd_site) + 1u]), [even] "m"(obfh_test_chain_routes[2u * OBFH_PD_LIVE_INDEX(__obfh_pd_site)]), [frame] "i"(__obfh_pd_frame),'
        );
    if (!body.includes('lock incl %[visit]') || !body.includes('[visit] "m"'))
        throw new Error('chain tracing target missing');
    const traced = source.slice(0, start) + body + source.slice(end);
    return source.includes('__obfh_pd_site = (site)')
        ? traced
        : traced.replace(
              '__obfh_pd_kind = OBFH_PD_DRAW(site, 1u)',
              '__obfh_pd_site = (site), __obfh_pd_kind = OBFH_PD_DRAW(site, 1u)'
          );
}

function unwindFixture() {
    return (
        pdata
            .fixture()
            // The fault handler must not call protection that can re-enter the patched node.
            .replaceAll('puts("PDATA_EXCEPTION_PASS")', '(puts)("PDATA_EXCEPTION_PASS")')
            .replaceAll('fflush(stdout)', '(fflush)(stdout)')
            .replaceAll('ExitProcess(', '(ExitProcess)(')
            .replaceAll('SetUnhandledExceptionFilter(', '(SetUnhandledExceptionFilter)(')
            .replaceAll('VirtualProtect(', '(VirtualProtect)(')
            .replaceAll('FlushInstructionCache(', '(FlushInstructionCache)(')
            .replaceAll('GetCurrentProcess()', '(GetCurrentProcess)()')
            .replace(
                'typedef unsigned (*NativeFn)(unsigned);',
                'typedef unsigned (*NativeFn)(unsigned, void *, void *);'
            )
            .replaceAll('((NativeFn)fn)(x)', '((NativeFn)fn)(x, NULL, NULL)')
            .replace(
                'unsigned n=(unsigned)atoi(argv[1]);DWORD old;',
                'unsigned node=(unsigned)atoi(argv[1]), n=OBFH_PD_LIVE_SITE(node);DWORD old;'
            )
            .replace('if(n>=count)return 82;', 'if(node>=16)return 82;')
            .replace(
                '((NativeFn)carriers[n])(123u,NULL,NULL);',
                'OBFH_PD_LIVE_NODE(0)(123u, (void *)OBFH_PD_LIVE_NODE(1), (void *)OBFH_PD_LIVE_NODE(node));'
            )
    );
}

async function runSuite({ arch, compiler, directory, source, check, compile, execute, run, assert }) {
    const root = path.join(directory, arch + '-live-chain');
    function prepare(name, header, fixture = 'cflow_chain.c') {
        const folder = path.join(root, name);
        fs.mkdirSync(path.join(folder, 'include'), { recursive: true });
        fs.mkdirSync(path.join(folder, 'tests'), { recursive: true });
        fs.writeFileSync(path.join(folder, 'include', 'obfus.h'), header);
        const file = path.join(folder, 'tests', fixture);
        if (fixture === 'unwind.c') fs.writeFileSync(file, unwindFixture());
        else fs.copyFileSync(path.join(__dirname, fixture), file);
        return file;
    }
    if (arch === 'x86') {
        await check('x86/live CFLOW ladder keeps existing transport', async () => {
            await execute(
                await compile(compiler, directory, 'x86-chain-disabled.exe', prepare('disabled', source), [
                    'NO_ANTIDEBUG=1'
                ]),
                'CHAIN_DISABLED'
            );
        });
        return;
    }
    for (const seed of [0, 1, 2, 0xdeadbeef, 0xffffffff]) {
        for (const mode of [0, 1]) {
            await check(`x64/live CFLOW ladder/native routes + state + unwind/v${mode + 1}/seed ${seed}`, async () => {
                const stem = `chain-${mode}-${seed}`;
                const flags = ['NO_ANTIDEBUG=1', `CFLOW_V2=${mode}`, `OBFH_BUILD_SEED=${seed}u`];
                const exe = await compile(compiler, directory, stem + '.exe', prepare(stem, trace(source)), [
                    ...flags,
                    'OBFH_TEST_CHAIN_TRACE=1'
                ]);
                await execute(exe, 'CHAIN_PASS');
                const unwind = await compile(
                    compiler,
                    directory,
                    stem + '-unwind.exe',
                    prepare(stem + '-unwind', source, 'unwind.c'),
                    flags
                );
                const output = await execute(unwind, 'PDATA_PASS');
                pdata.measure(output.stdout, fs.readFileSync(unwind), assert);
                for (const node of ['2', '15']) await execute(unwind, 'PDATA_EXCEPTION_PASS', [node]);
            });
        }
    }
    await check('x64/live CFLOW ladder/untraced deletion and wrong-transform controls', async () => {
        const mutants = [
            [
                'child',
                source
                    .replaceAll('call *-8(%%rbp);', 'movl %%ecx, %%eax;')
                    .replaceAll('call *-%c[next](%%rbp);', 'movl %%ecx, %%eax;')
                    .replaceAll('call *%%r11;', 'movl %%ecx, %%eax;')
                    .replaceAll(
                        'OBFH_PD_LIVE_EMIT(mem, 3, "(((256-%c[next])<<16)|0x55ff)")',
                        'OBFH_PD_LIVE_EMIT(mem, 2, "0xc889")'
                    )
                    .replaceAll('OBFH_PD_LIVE_EMIT(reg, 3, "0xd3ff41")', 'OBFH_PD_LIVE_EMIT(reg, 2, "0xc889")')
                    .replaceAll('OBFH_PD_LIVE_EMIT(arg, 3, "0xd3ff41")', 'OBFH_PD_LIVE_EMIT(arg, 2, "0xc889")')
            ],
            [
                'post',
                source
                    .replaceAll('addl %[even_add], %%eax;', 'subl %[even_add], %%eax;')
                    .replaceAll('addl %[even_add], %%edx;', 'subl %[even_add], %%edx;')
                    .replaceAll('addl %[even_add], %%r9d;', 'subl %[even_add], %%r9d;')
                    .replaceAll(
                        'OBFH_PD_LIVE_IMM(branch, 1, "0x05", even_add)',
                        'OBFH_PD_LIVE_IMM(branch, 1, "0x2d", even_add)'
                    )
                    .replaceAll(
                        'OBFH_PD_LIVE_IMM(arg, 2, "0xc281", even_add)',
                        'OBFH_PD_LIVE_IMM(arg, 2, "0xea81", even_add)'
                    )
                    .replaceAll(
                        'OBFH_PD_LIVE_IMM(arg, 3, "0xc18141", even_add)',
                        'OBFH_PD_LIVE_IMM(arg, 3, "0xe98141", even_add)'
                    )
            ],
            [
                'caller',
                source.replace(/__obfh_flow_state = obfh_pd_live_entries\[__obfh_lsite0\]\([\s\S]*?\);/, '(void)0;')
            ]
        ];
        for (const [name, header] of mutants) {
            assert(header !== source, 'chain mutation target missing: ' + name);
            const exe = await compile(compiler, directory, 'chain-mutant-' + name + '.exe', prepare(name, header), [
                'NO_ANTIDEBUG=1'
            ]);
            const result = await run(exe, []);
            assert(
                result.status !== 0 && !result.stdout.includes('CHAIN_PASS'),
                'live chain mutation survived: ' + name
            );
        }
    });
    await check('x64/live CFLOW ladder/disabled contracts', async () => {
        for (const flag of ['NO_OBF=1', 'NO_CFLOW=1', 'NO_PDATA_DECOYS=1'])
            await execute(
                await compile(
                    compiler,
                    directory,
                    'chain-disabled-' + flag.slice(0, -2) + '.exe',
                    prepare(flag, source),
                    ['NO_ANTIDEBUG=1', flag]
                ),
                'CHAIN_DISABLED'
            );
    });
}

module.exports = { runSuite };
