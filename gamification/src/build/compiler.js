import fs from 'node:fs/promises';
import path from 'node:path';
import { emit } from '../generation/emitter.js';
import { logoResource } from './resource.js';
import { run } from '../core/process.js';
import { REPOSITORY_ROOT } from '../core/paths.js';

export function compiler(toolchain, arch) {
    return path.join(toolchain, arch === 'x64' ? 'tcc.exe' : 'i386-win32-tcc.exe');
}
export async function buildChallenge(challenge, directory, toolchain, arch, { player = false, mutate } = {}) {
    await fs.mkdir(directory, { recursive: true });
    const include = path.join(directory, 'include');
    await fs.mkdir(include, { recursive: true });
    let header = await fs.readFile(path.join(REPOSITORY_ROOT, 'include/obfus.h'), 'utf8');
    if (!player) {
        if (!header.includes('/* OBFH_VM_TRACE_STEP */')) throw new Error('VM trace contract changed');
        header = header.replace('/* OBFH_VM_TRACE_STEP */', 'daily_vm_step(op, at);');
    }
    await fs.writeFile(path.join(include, 'obfus.h'), header);
    const source = path.join(directory, 'challenge.c'),
        exe = path.join(directory, player ? 'player.exe' : 'probe.exe');
    await fs.writeFile(source, emit(challenge, { mutate }));
    const args = ['-w', '-I', include, source];
    if (player) {
        const resource = path.join(directory, 'logo.o');
        await fs.writeFile(resource, logoResource(await fs.readFile(path.join(REPOSITORY_ROOT, 'gamification/assets/logo.jpg')), arch));
        args.push(resource, '-luser32', '-lgdi32', '-lole32', '-lshell32', '-Wl,-subsystem=gui');
    } else args.push('-DSELFTEST');
    args.push('-o', exe);
    const result = await run(compiler(toolchain, arch), args, { timeout: 180000 });
    if (result.code !== 0) {
        // Compiler diagnostics can echo source containing private target data. Keep them local.
        await fs.writeFile(path.join(directory, 'build.private.log'), result.stderr);
        throw new Error(`Build failed (${arch}/${challenge.ir.level}); private diagnostic retained locally`);
    }
    await fs.rm(exe.replace(/\.exe$/, '.def'), { force: true });
    return exe;
}
