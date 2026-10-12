'use strict';
const fs = require('node:fs');
const path = require('node:path');
const semantics = require('./vm_semantics');

async function runSuite({ arch, compiler, directory, source, check, compile, execute, run, assert, fixturesRoot }) {
    await require('./vm_fields').runSuite({ arch, compiler, directory, source, check, compile, execute, assert });
    await require('./vm_variants').runSuite({ arch, compiler, directory, source, check, compile, execute, assert });
    const root = path.join(directory, arch + '-vm-kernel');
    fs.mkdirSync(path.join(root, 'include'), { recursive: true });
    fs.mkdirSync(path.join(root, 'tests'), { recursive: true });
    const file = path.join(root, 'tests', 'vm_kernel.c');
    fs.copyFileSync(path.join(__dirname, 'vm_kernel.c'), file);
    const header = path.join(root, 'include', 'obfus.h');
    const traced = source
        .replace(
            'static long double Obfh_VirtualMachine(',
            'void study_step(unsigned int op, unsigned int pc);\nstatic long double Obfh_VirtualMachine('
        )
        .replace('/* OBFH_VM_TRACE_STEP */', 'study_step(op, at);');
    assert(traced.includes('study_step(op, at);'), 'VM instruction trace target missing');
    fs.writeFileSync(header, traced);
    const semanticsFile = semantics.writeFixture(path.join(root, 'semantics'), source, fixturesRoot);

    for (const mode of [0, 1]) {
        // Cover every opcode/word dependency order and context layout, plus boundary seeds.
        for (const seed of [0, 84, 168, 252, 0xdeadbeef, 0xffffffff]) {
            const flags = ['VIRT=1', 'NO_ANTIDEBUG=1', `CFLOW_V2=${mode}`, `OBFH_BUILD_SEED=${seed}u`];
            const stem = `${arch}-vm-kernel-${mode}-${seed}`;
            const label = `${arch}/VM ISA, all programs, faults and contract/cflow-v${mode + 1}/seed ${seed}`;
            await check(
                label,
                async () => {
                    const seedRoot = path.join(root, stem);
                    fs.mkdirSync(path.join(seedRoot, 'include'), { recursive: true });
                    fs.mkdirSync(path.join(seedRoot, 'tests'), { recursive: true });
                    fs.writeFileSync(path.join(seedRoot, 'include', 'obfus.h'), traced);
                    const seedFile = path.join(seedRoot, 'tests', 'vm_kernel.c');
                    fs.copyFileSync(file, seedFile);
                    const exe = await compile(compiler, directory, stem + '.exe', seedFile, flags);
                    await execute(exe, 'KERNEL_PASS');
                    const errors = [3, 4, 5, 7, 2, 6, 1, 6, 6, 6, 1, 1, 8, 4];
                    for (let fault = 1; fault <= errors.length; ++fault) {
                        const result = await run(exe, [String(fault)]);
                        assert(
                            result.status >>> 0 === (0xe0bf0000 | errors[fault - 1]) >>> 0,
                            `VM fault ${fault}: unexpected status ${result.status}`
                        );
                    }
                }
            );
            await check(label + '/public numeric, branches and VM contracts', async () => {
                const exe = await compile(compiler, directory, stem + '-semantics.exe', semanticsFile, flags);
                await semantics.executeContracts(exe, execute);
            });
        }
    }

    const mutants = [
        ['bitwise', traced.replace('obfh_v_write(&c, d, x ^ y);', 'obfh_v_write(&c, d, x | y);')],
        [
            'load',
            traced
                .replace('/* OBFH_VM_TRACE_STEP */', '')
                .replace('study_step(op, at);', 'study_step(op, at); if (op == OBFH_V_LOAD_B) continue;')
        ],
        ['flags', traced.replace('unsigned int f = obfh_v_flags(&c);', 'unsigned int f = 0;')],
        ['jump', traced.replace('if (obfh_v_flags(&c) & (1u << flag))', 'if (0)')],
        ['pair', traced.replace('obfh_v_write(&c, a, operands[1]);', '(void)operands[1];')],
        ['copy-return', traced.replace('a = d;', 'a = (d + 1u) & 3u;')],
        ['layout', traced.replace('field_decoders[(key >> 29) & 3u]', 'field_decoders[0]')]
    ];
    for (const [name, mutant] of mutants)
        await check(`${arch}/VM ISA mutations: missing instruction, flags and virtual jump/${name}`, async () => {
            assert(mutant !== traced, 'VM mutation target missing: ' + name);
            // Normal seed checks read the traced header concurrently. Keep each
            // negative control in its own immutable tree instead of overwriting it.
            const mutantRoot = path.join(root, 'mutants', name);
            fs.mkdirSync(path.join(mutantRoot, 'include'), { recursive: true });
            fs.mkdirSync(path.join(mutantRoot, 'tests'), { recursive: true });
            fs.writeFileSync(path.join(mutantRoot, 'include', 'obfus.h'), mutant);
            const mutantFile = path.join(mutantRoot, 'tests', 'vm_kernel.c');
            fs.copyFileSync(file, mutantFile);
            const exe = await compile(compiler, directory, `${arch}-vm-kernel-mutant-${name}.exe`, mutantFile, [
                'VIRT=1',
                'NO_CFLOW=1',
                'NO_ANTIDEBUG=1'
            ]);
            const result = await run(exe, []);
            assert(result.status !== 0 && !result.stdout.includes('KERNEL_PASS'), 'VM mutation survived: ' + name);
        });

    for (const flags of [
        ['VIRT=0', 'NO_ANTIDEBUG=1'],
        ['VIRT=1', 'NO_OBF=1']
    ]) {
        await check(`${arch}/VM disabled contract/${flags[0]}/${flags[1]}`, async () => {
            await execute(
                await compile(
                    compiler,
                    directory,
                    `${arch}-vm-disabled-${flags[1]}.exe`,
                    path.join(__dirname, 'vm.c'),
                    flags
                ),
                'VM_PASS'
            );
        });
    }
}

module.exports = { runSuite };
