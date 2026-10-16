'use strict';
const { macroDefinition } = require('./source_macros');

// Inspect skipped MOV-immediate bytes independently of the production selector.
function fixture(source) {
    const definitions = ['OBFH_SF_CALL_CONSTANTS', 'OBFH_SF_CALL_ARGS']
        .map(name => macroDefinition(source, name).body).join('\n');
    let code = '#include <stdint.h>\n#include <stdio.h>\n' + definitions + `
#if defined(__x86_64__)
typedef struct { const unsigned char *start,*end; } Span;
#define CARRIER(name,orderValue,aValue,bValue) static Span name(void){Span s;\
__asm__ __volatile__(".set .Lsp10,%c[order];.set .Lss_arg_a,%c[a];.set .Lss_arg_b,%c[b];" OBFH_SF_CALL_CONSTANTS\
"leaq 8311f(%%rip),%%rax;movq %%rax,%[start];leaq 8312f(%%rip),%%rax;movq %%rax,%[end];jmp 8319f;8311:"\
OBFH_SF_CALL_ARGS "8312:8319:"\
:[start]"=m"(s.start),[end]"=m"(s.end):[order]"i"(orderValue),[a]"i"(aValue),[b]"i"(bValue):"eax","cc","memory");return s;}
`;
    const entries = [], expected = [];
    for (const order of [0, 1]) for (const a of [0, 1, 0x12345678, 0x7fffffff])
        for (const b of [0, 127, 128, 0x76543210]) {
            const name = 'call_entry_' + entries.length;
            code += `CARRIER(${name},${order},${a},${b})\n`;
            entries.push(name);
            const first = order ? a : b, second = order ? b : a;
            const bytes = [order ? 0xb9 : 0xba];
            for (let n = 0; n < 4; ++n) bytes.push((first >>> (8 * n)) & 255);
            bytes.push(order ? 0xba : 0xb9);
            for (let n = 0; n < 4; ++n) bytes.push((second >>> (8 * n)) & 255);
            expected.push('{' + bytes.join(',') + '}');
        }
    return code + `static int check(void){Span(*entries[])(void)={${entries.join(',')}};
static const unsigned char expected[][10]={${expected.join(',')}};
for(unsigned i=0;i<sizeof(entries)/sizeof(entries[0]);++i){Span s=entries[i]();
if(s.end-s.start!=10)return 1;
for(unsigned n=0;n<10;++n)if(s.start[n]!=expected[i][n]){fprintf(stderr,"call bytes %u:%u\\n",i,n);return 2;}}return 0;}
#else
static int check(void){return 0;}
#endif
int main(void){int r=check();if(r)return r;if(puts("SF_CALLS_PASS")==EOF||fflush(stdout))return 3;return 0;}\n`;
}
module.exports = { fixture };
