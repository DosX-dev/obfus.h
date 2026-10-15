'use strict';
const fs = require('node:fs');
const path = require('node:path');
function definition(source, name) {
    const start = source.indexOf('#define ' + name + ' ');
    if (start < 0) throw new Error('Missing guard definition: ' + name);
    let end = source.indexOf('\n', start);
    while (source.slice(start, end).trimEnd().endsWith('\\')) end = source.indexOf('\n', end + 1);
    return source.slice(start, end);
}
function fixture(header, fixturesRoot, constantGuard) {
    const start = header.indexOf('// BEGIN SHORT ASM FORMS');
    const end = header.indexOf('// END SHORT ASM FORMS', start);
    if (start < 0 || end < 0) throw new Error('Short ASM helpers missing');
    const orders = [...header.matchAll(/^#define OBFH_SF_GUARD_8_ORDER (.+)$/gm)].map(match => match[1]);
    if (orders.length !== 6) throw new Error('Expected all six guard8 orders');
    let source = '#include <stdio.h>\n' + header.slice(start, end) + '\n';
    for (const guard of [8, 10, 13]) source += definition(header, 'OBFH_SF_GUARD_' + guard) + '\n';
    const cases = [];
    for (let order = 0; order < orders.length; ++order) {
        source += '#undef OBFH_SF_GUARD_8_ORDER\n#define OBFH_SF_GUARD_8_ORDER ' + orders[order] + '\n';
        for (const key of [0, 1, 0x80000000, 0xffffffff, 0x12345678])
            for (const phantom of [0, 1 << 15, 2 << 15, 3 << 15])
                for (const guard of [8, 10, 13]) {
                    const name = 'raw_guard_' + cases.length;
                    const constant = guard === 8 ? -1 : guard === 10 ? 0 : Math.imul(key, key);
                    const mutation = guard === constantGuard ? `movl $${constant},%%eax;` : '';
                    source += `static unsigned ${name}(unsigned x) { __asm__ __volatile__(OBFH_SF_GUARD_${guard} "ud2;9:;${mutation}" : "+a"(x) : [key_a] "i"(${key}u), [phantom] "i"(${phantom}u) : "edx","ecx","cc");return x;}\n`;
                    cases.push(`{${name},${guard},${key}u}`);
                }
    }
    source += 'static struct {unsigned (*run)(unsigned);unsigned kind,key;} raw_cases[]={' + cases.join(',') + '};\n';
    return source + fs.readFileSync(path.join(fixturesRoot, 'sf_live_guards.c'), 'utf8');
}
async function runSuite({ arch, compiler, directory, source, fixturesRoot, check, compile, execute, run, assert }) {
    await check(arch + '/SF guards preserve variable live results', async () => {
        const file = path.join(directory, arch + '-sf-live-guards.c');
        fs.writeFileSync(file, fixture(source, fixturesRoot));
        await execute(await compile(compiler, directory, arch + '-sf-live-guards.exe', file, []), 'SF_LIVE_GUARDS_PASS');
        // Restore the old constant output of each guard independently.
        for (const guard of [8, 10, 13]) {
            const mutant = path.join(directory, arch + '-sf-constant-' + guard + '.c');
            fs.writeFileSync(mutant, fixture(source, fixturesRoot, guard));
            const exe = await compile(compiler, directory, arch + '-sf-constant-' + guard + '.exe', mutant, []);
            const result = await run(exe, []);
            assert(result.status === 1 && result.stdout.includes('SF_LIVE_GUARDS_FAIL'), 'constant guard output survived: ' + guard);
        }
    });
}
module.exports = { fixture, runSuite };
