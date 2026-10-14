'use strict';
const fs = require('node:fs');
const path = require('node:path');
exports.runSuite = async function({ arch, compiler, directory, source, check, compile, execute, run, assert }) {
    await check(`${arch}/service conditions: all 496 forms and single evaluation`, async () => {
        const begin = source.indexOf('#define OBFH_HELPER_MIX_VALUE'), end = source.indexOf('\n#endif', begin);
        assert(begin >= 0 && end > begin, 'service condition block missing');
        const mix = source.split('\n').filter(line => /^#define OBFH_MIX_[AB]\(/.test(line)).join('\n');
        let c = '#include <stdio.h>\n#include <limits.h>\n' + mix + '\n' +
            '#define __obfh_asm__(...) __asm__ __volatile__(__VA_ARGS__)\n' +
            '#define OBFH_DATA_FIELDS(name, site) name=(site)\n' + source.slice(begin, end) + '\n';
        for (let draw = 0; draw < 496; ++draw)
            c += `static int probe_${draw}(long long value,int *calls){return OBFH_HELPER_CONDITION_AT((++*calls,value),${draw}u);}\n`;
        c += 'int main(void){long long values[]={0,1,-1,LLONG_MIN,LLONG_MAX,0x100000000LL};int calls;\n';
        for (let draw = 0; draw < 496; ++draw)
            c += `for(int i=0;i<6;++i){calls=0;if(probe_${draw}(values[i],&calls)!=(values[i]!=0)||calls!=1)return 1;}\n`;
        c += 'if(puts("HELPER_CONDITION_PASS")==EOF||fflush(stdout))return 2;return 0;}\n';
        const file = path.join(directory, arch + '-helper-conditions.c');
        fs.writeFileSync(file, c);
        await execute(await compile(compiler, directory, arch + '-helper-conditions.exe', file, []), 'HELPER_CONDITION_PASS');
        // Removing the live mix must affect the actual condition result.
        const mutant = c.replace(/#define OBFH_HELPER_MIX\(state, key, bias, form\)[\s\S]*?(?=\n#define OBFH_HELPER_CONDITION_AT)/,
            '#define OBFH_HELPER_MIX(state, key, bias, form) ((void)0)');
        assert(mutant !== c, 'service mix mutation missing');
        fs.writeFileSync(file, mutant);
        const result = await run(await compile(compiler, directory, arch + '-helper-mutant.exe', file, []), []);
        assert(result.status === 1 && !result.stdout.includes('HELPER_CONDITION_PASS'), 'service dependency deletion survived');
    });
    for (const flags of [[], ['OBFH_BETTER_CONTROL_FLOW=1'], ['OBFH_DISABLE_CONTROL_FLOW=1'], ['OBFH_DISABLE_ANY_OBFUSCATION=1']])
        await check(`${arch}/compact loader: cold concurrency, failures and pointer transport/${flags.join('/') || 'default'}`, async () =>
            execute(await compile(compiler, directory, `${arch}-helper-loader-${flags.join('-') || 'default'}.exe`,
                path.join(__dirname, 'helper_loader.c'), ['OBFH_DISABLE_ANTIDEBUG=1', ...flags]), 'HELPER_LOADER_PASS'));
};
