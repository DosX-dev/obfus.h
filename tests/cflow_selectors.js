'use strict';
const fs = require('node:fs');
const path = require('node:path');

function fixture(header) {
    const first = header.lastIndexOf('#if defined(__x86_64__)', header.indexOf('#define OBFH_P_WORD'));
    const last = header.indexOf('// Pair transport:', first);
    if (first < 0 || last < first) throw new Error('Missing selector definitions');
    let code = '#include <windows.h>\n#include <stdio.h>\n#define __obfh_asm__ __asm__\n' + header.slice(first, last);
    const cases = [];
    for (let shift = 0; shift < 4; ++shift) {
        const seen = new Set();
        for (let n = 0; seen.size < 144 && n < 100000; ++n) {
            let hash = Math.imul(n + 1, 0x9e3779b9) >>> 0;
            hash = (hash ^ (hash >>> 16)) >>> 0;
            const polarity = (hash >>> (shift + 20)) & 1;
            const form = [(hash >>> (shift + 22)) % 3, (hash >>> (shift + 26)) % 3,
            (hash >>> (shift + 25)) & 1, polarity,
            (hash >>> (shift + 6)) & 1, (hash >>> 15) & 1].join('/');
            if (seen.has(form)) continue;
            seen.add(form);
            for (const kind of ['MASK', 'TABLE']) {
                const id = cases.length;
                code += `static int probe_${id}(unsigned x,unsigned y,unsigned cookie) {\n` +
                    `enum { __obfh_flow_hash=${hash}u, __obfh_flow_key=0x6a39de17u };\n` +
                    'unsigned __obfh_flow_state=x, __obfh_flow_tag=y, __obfh_cookie=cookie;\n' +
                    `OBFH_P_SELECTOR_META(${shift}); OBFH_P_${kind}(${shift},no,yes); no:return 0; yes:return 1; }\n`;
                cases.push({ id, shift, polarity });
            }
        }
        if (seen.size !== 144) throw new Error('Missing selector combination');
    }
    code += 'int main(void) { unsigned values[]={0,1,2,0x7fffffffu,0x80000000u,0xffffffffu};\n';
    for (const { id, shift, polarity } of cases) {
        code += `for(unsigned a=0;a<6;++a)for(unsigned b=0;b<6;++b)for(unsigned c=0;c<2;++c) {\n` +
            `unsigned expected=c ^ (values[a]==values[b]) ^ ${polarity};\n` +
            `if(probe_${id}(values[a],values[b],c<<${shift})!=expected) { printf("FAIL ${id} %u %u %u\\n",a,b,c);return 1;} }\n`;
    }
    return code + 'puts("SELECTORS_PASS");return 0;}\n';
}

async function runSuite({ arch, compiler, directory, source, check, compile, execute }) {
    await check(arch + '/CFLOW selector address, bit and index forms', async () => {
        const file = path.join(directory, arch + '-selectors.c');
        fs.writeFileSync(file, fixture(source));
        await execute(await compile(compiler, directory, arch + '-selectors.exe', file, []), 'SELECTORS_PASS');
    });
}
module.exports = { fixture, runSuite };
