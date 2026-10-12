'use strict';
const fs = require('node:fs');
const path = require('node:path');

function fixture(header) {
    const start = header.indexOf('#define OBFH_P_TERMINAL_TEXT');
    const end = header.indexOf('#ifdef OBFH_TEST_FLOW_TRACE', start);
    if (start < 0 || end < start) throw new Error('Terminal emission missing');
    let code = '#include <stdio.h>\n' + header.slice(start, end);
    const probes = [];
    for (let style = 0; style < 8; ++style)
        for (let zero = 0; zero < 3; ++zero) {
            const name = 'terminal_' + style + '_' + zero;
            probes.push(name);
            code +=
                `static unsigned ${name}(unsigned x,unsigned y) { unsigned result;\n` +
                '__asm__ __volatile__("movl %[other], %%edx;" OBFH_P_TERMINAL_TEXT\n' +
                `: "=&a"(result) : "0"(x), [other] "m"(y), [finish] "i"(${style}), [zero_form] "i"(${zero})\n` +
                ': "edx","ecx","cc","memory"); return result; }\n';
        }
    code += 'static unsigned (*probes[])(unsigned,unsigned)={' + probes.join(',') + '};\n';
    code += String.raw`int main(void) {
    unsigned state=0;
    const unsigned edges[]={0,1,0xffffffffu,0x80000000u,0x7fffffffu,0x55555555u,0xaaaaaaaau};
    for(unsigned i=0;i<4096;++i) {
        state=state*1664525u+1013904223u;
        unsigned x=i<7?edges[i]:state;
        unsigned values[]={x,~x,x+1,x-1,0,0xffffffffu,x^0x80000000u};
        for(unsigned j=0;j<7;++j) for(unsigned p=0;p<sizeof(probes)/sizeof(probes[0]);++p)
            if(probes[p](x,values[j])!=(unsigned)(x==values[j])) {
                printf("TERMINAL_FAIL %u %u %u\n",i,j,p);return 1;
            }
    }
    puts("CFLOW_TERMINAL_PASS");return 0;
}`;
    return code;
}

async function runSuite({ arch, compiler, directory, source, check, compile, execute }) {
    if (!source.includes('#define OBFH_P_TERMINAL_TEXT')) return;
    await check(arch + '/CFLOW fused terminal families and zero forms', async () => {
        const file = path.join(directory, arch + '-terminal.c');
        fs.writeFileSync(file, fixture(source));
        await execute(await compile(compiler, directory, arch + '-terminal.exe', file, []), 'CFLOW_TERMINAL_PASS');
    });
}
module.exports = { fixture, runSuite };
