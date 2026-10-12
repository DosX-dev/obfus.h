'use strict';
const fs = require('node:fs');
const path = require('node:path');

function fixture(source) {
    const first = source.indexOf('// Commit outside the layout choice:');
    const last = source.indexOf('#define OBFH_SF_FLOW_EMIT', first);
    if (first < 0 || last < first) throw new Error('Missing CFLOW commit definitions');
    let code = '#include <windows.h>\n#include <stdio.h>\n#define __obfh_asm__ __asm__\n' + source.slice(first, last);
    for (const form of [0, 1]) {
        code += `static int probe_${form}(unsigned state,unsigned tag,ULONG_PTR cookie,unsigned actual,unsigned expected) {
            enum { __obfh_flow_hash=${form << 18}u };
            unsigned __obfh_flow_state=state, __obfh_flow_tag=tag;
            ULONG_PTR __obfh_cookie=cookie;
            unsigned __obfh_live_input=state^tag;
            unsigned __obfh_live_actual=actual, __obfh_live_expected=expected^__obfh_live_input;
            OBFH_SF_FLOW_COMMIT;
            return __obfh_flow_state==(state^actual) && __obfh_flow_tag==(tag^expected)
                && __obfh_cookie==(cookie^(ULONG_PTR)actual);
        }\n`;
    }
    code += `int main(void) {
        unsigned values[]={0,1,0x7fffffffu,0x80000000u,0xffffffffu};
        for(unsigned a=0;a<5;++a) for(unsigned b=0;b<5;++b)
        for(unsigned c=0;c<5;++c) for(unsigned d=0;d<5;++d) {
            ULONG_PTR cookie=~(ULONG_PTR)values[a];
            if(!probe_0(values[a],values[b],cookie,values[c],values[d]) ||
               !probe_1(values[a],values[b],cookie,values[c],values[d])) return 1;
        }
        unsigned rng=0xace12345u;
        for(unsigned i=0;i<100000;++i) {
            rng^=rng<<13; rng^=rng>>17; rng^=rng<<5;
            ULONG_PTR cookie=~(ULONG_PTR)(rng*17u);
            if(!probe_0(rng,rng*3u,cookie,rng*5u,~rng) ||
               !probe_1(rng,rng*3u,cookie,rng*5u,~rng)) return 2;
        }
        puts("CFLOW_COMMIT_PASS"); return 0;
    }\n`;
    return code;
}

async function runSuite({ arch, compiler, directory, source, check, compile, execute }) {
    await check(arch + '/CFLOW live commit coordinate and full-width cookie semantics', async () => {
        const stem = arch + '-cflow-commit';
        const file = path.join(directory, stem + '.c');
        fs.writeFileSync(file, fixture(source));
        await execute(await compile(compiler, directory, stem + '.exe', file, []), 'CFLOW_COMMIT_PASS');
    });
}
module.exports = { fixture, runSuite };
