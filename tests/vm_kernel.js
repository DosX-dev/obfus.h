'use strict';
const fs = require('node:fs');
const path = require('node:path');

async function runSuite({ arch, compiler, directory, source, check, compile, execute, run, assert }) {
    const root = path.join(directory, arch + '-vm-kernel');
    fs.mkdirSync(path.join(root, 'include'), { recursive: true });
    fs.mkdirSync(path.join(root, 'tests'), { recursive: true });
    const file = path.join(root, 'tests', 'vm_kernel.c');
    fs.copyFileSync(path.join(__dirname, 'vm_kernel.c'), file);
    const header = path.join(root, 'include', 'obfus.h');
    const traced = source.replace('static long double Obfh_VirtualMachine(',
        'void study_step(unsigned int op, unsigned int pc);\nstatic long double Obfh_VirtualMachine(')
        .replace('/* OBFH_VM_TRACE_STEP */', 'study_step(op, at);');
    assert(traced.includes('study_step(op, at);'), 'VM instruction trace target missing');
    fs.writeFileSync(header, traced);

    for (const mode of [0, 1]) {
        for (const seed of [0, 1, 2, 0xDEADBEEF, 0xFFFFFFFF]) {
            await check(`${arch}/VM ISA, all programs, faults and contract/cflow-v${mode + 1}/seed ${seed}`, async () => {
                const flags = ['VIRT=1', 'NO_ANTIDEBUG=1', `CFLOW_V2=${mode}`, `OBFH_BUILD_SEED=${seed}u`];
                const stem = `${arch}-vm-kernel-${mode}-${seed}`;
                const exe = await compile(compiler, directory, stem + '.exe', file, flags);
                await execute(exe, 'KERNEL_PASS');
                const errors = [3, 4, 5, 7, 2, 6, 1, 6, 6, 6, 1, 1];
                for (let fault = 1; fault <= errors.length; ++fault) {
                    const result = await run(exe, [String(fault)]);
                    assert((result.status >>> 0) === ((0xE0BF0000 | errors[fault - 1]) >>> 0),
                        `VM fault ${fault}: unexpected status ${result.status}`);
                }
                for (const [name, marker] of [['vm', 'VM_PASS'], ['numeric', 'NUMERIC_PASS'], ['vm_branches', 'BRANCH_PASS']]) {
                    await execute(await compile(compiler, directory, stem + '-' + name + '.exe',
                        path.join(__dirname, name + '.c'), flags), marker);
                }
            });
        }
    }

    await check(`${arch}/VM ISA mutations: missing instruction, flags and virtual jump`, async () => {
        const mutants = [
            ['bitwise', traced.replace('obfh_v_write(&c, d, x ^ y);', 'obfh_v_write(&c, d, x | y);')],
            ['load', traced.replace('/* OBFH_VM_TRACE_STEP */', '')
                .replace('study_step(op, at);', 'study_step(op, at); if (op == OBFH_V_LOAD_B) continue;')],
            ['flags', traced.replace('unsigned int f = obfh_v_flags(&c);', 'unsigned int f = 0;')],
            ['jump', traced.replace('if (obfh_v_flags(&c) & (1u << flag))', 'if (0)')],
        ];
        for (const [name, mutant] of mutants) {
            assert(mutant !== traced, 'VM mutation target missing: ' + name);
            fs.writeFileSync(header, mutant);
            const exe = await compile(compiler, directory, `${arch}-vm-kernel-mutant-${name}.exe`, file,
                ['VIRT=1', 'NO_CFLOW=1', 'NO_ANTIDEBUG=1']);
            const result = await run(exe, []);
            assert(result.status !== 0 && !result.stdout.includes('KERNEL_PASS'), 'VM mutation survived: ' + name);
        }
        fs.writeFileSync(header, traced);
    });

    for (const flags of [['VIRT=0', 'NO_ANTIDEBUG=1'], ['VIRT=1', 'NO_OBF=1']]) {
        await check(`${arch}/VM disabled contract/${flags[0]}/${flags[1]}`, async () => {
            await execute(await compile(compiler, directory, `${arch}-vm-disabled-${flags[1]}.exe`,
                path.join(__dirname, 'vm.c'), flags), 'VM_PASS');
        });
    }
}

module.exports = { runSuite };
