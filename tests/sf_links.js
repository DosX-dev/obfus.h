'use strict';
const { macroDefinition } = require('./source_macros');

function fixture(source) {
    const definitions = ['OBFH_SF_LINK_CONSTANT', 'OBFH_SF_LINK_GATE']
        .map(name => macroDefinition(source, name).body).join('\n');
    const defaults = Array.from({ length: 6 }, (_, channel) =>
        `.set .Lsle${channel},0;.set .Lsln${channel},0;.set .Lslz${channel},0;`).join('');
    let code = '#include <stdint.h>\n#include <stdio.h>\n' + definitions + `
typedef struct { const unsigned char *start,*end; } LinkSpan;
#if defined(__x86_64__)
#define LINK_ADDRESS "leaq 8321f(%%rip),%%rax;movq %%rax,%[start];leaq 8322f(%%rip),%%rax;movq %%rax,%[end];jmp 8329f;8321:"
#else
#define LINK_ADDRESS "leal 8321f,%%eax;movl %%eax,%[start];leal 8322f,%%eax;movl %%eax,%[end];jmp 8329f;8321:"
#endif
#define LINK_CARRIER(name,channel,enableValue,registerValue,branchValue,spanValue) static LinkSpan name(void){LinkSpan s;\
__asm__ __volatile__("${defaults}" ".set .Lsle" #channel ",%c[enable];.set .Lsln" #channel ",%c[reg];.set .Lslz" #channel ",%c[branch];"\
OBFH_SF_LINK_CONSTANT(#channel) LINK_ADDRESS OBFH_SF_LINK_GATE(#spanValue,#channel) "8322:8329:"\
:[start]"=m"(s.start),[end]"=m"(s.end):[enable]"i"(enableValue),[reg]"i"(registerValue),[branch]"i"(branchValue):"eax","cc","memory");return s;}
`;
    const entries = [], expected = [], lengths = [];
    for (let channel = 0; channel < 6; ++channel) for (const enabled of [0, 1])
        for (const register of [0, 1]) for (const branch of [0, 1]) for (const span of [18, 23]) {
            const name = 'link_entry_' + entries.length;
            code += `LINK_CARRIER(${name},${channel},${enabled},${register},${branch},${span})\n`;
            entries.push(name);
            // Independent TEST r32,r32 + JZ/JNZ rel8 bytes; no production field algebra.
            expected.push(`{0x85,${register ? 0xd0 : 0xc0},${branch ? 0x75 : 0x74},${span}}`);
            lengths.push(enabled ? 4 : 0);
        }
    return code + `int main(void){LinkSpan(*entries[])(void)={${entries.join(',')}};
static const unsigned char expected[][4]={${expected.join(',')}};
static const unsigned char lengths[]={${lengths.join(',')}};
for(unsigned i=0;i<sizeof(entries)/sizeof(entries[0]);++i){LinkSpan s=entries[i]();
if(s.end-s.start!=lengths[i]){fprintf(stderr,"link length %u\\n",i);return 1;}
for(unsigned n=0;n<lengths[i];++n)if(s.start[n]!=expected[i][n]){fprintf(stderr,"link bytes %u:%u\\n",i,n);return 2;}}
if(puts("SF_LINKS_PASS")==EOF||fflush(stdout))return 3;return 0;}\n`;
}
module.exports = { fixture };
