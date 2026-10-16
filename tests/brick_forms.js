'use strict';
// Frozen pre-change cores independently check values, carry-sensitive N,
// boundary immediate encodings and instruction-span growth.
const reference = String.raw`#define BRICK_REFERENCE_B "addl $%c[key_b], %%eax; roll $%c[rotate], %%eax; xorl %%edx, %%eax;"
#define BRICK_REFERENCE_C "leal (%%eax, %%eax, 2), %%eax; xorl %%eax, %%edx; addl $%c[key_a], %%eax;"
#define BRICK_REFERENCE_D "notl %%eax; addl %%edx, %%eax; imull $%c[factor], %%eax; roll $%c[rotate], %%eax;"
#define BRICK_REFERENCE_E "bswap %%eax; xorl %%edx, %%eax; roll $%c[rotate], %%eax;"
#define BRICK_REFERENCE_K "testl %%edx, %%edx; js 6f; addl $%c[key_a], %%eax; .byte 0xeb,4; 6: negl %%eax; xorl %%edx, %%eax; 7: roll $%c[rotate], %%eax;"
#define BRICK_REFERENCE_N "addl $%c[key_a], %%eax; adcl $%c[key_b], %%edx; xorl %%edx, %%eax; rorl $%c[rotate], %%eax;"
#define BRICK_REFERENCE_O "xchgl %%eax, %%edx; subl $%c[key_a], %%eax; bswap %%edx; addl %%edx, %%eax;"
#define BRICK_REFERENCE_R "movl $%c[loops], %%ecx; 6: xorl %%edx, %%eax; roll $%c[rotate], %%eax; addl $%c[key_a], %%edx; decl %%ecx; jnz 6b;"
#define BRICK_REFERENCE_S "shldl $%c[rotate], %%edx, %%eax; subl $%c[key_b], %%eax; xorl $%c[mask], %%eax;"`;
function definition(source, name) {
    const lines = source.slice(source.indexOf('#define ' + name)).split(/\r?\n/), result = [];
    if (!lines[0].startsWith('#define ' + name)) throw new Error('Missing ' + name);
    for (const line of lines) { result.push(line); if (!line.trimEnd().endsWith('\\')) break; }
    return result.join('\n') + '\n';
}
function fixture(header) {
    const start = header.indexOf('#define OBFH_SF_ROTATE_B '), end = header.indexOf('// All fifteen identities', start);
    if (start < 0 || end < start) throw new Error('Missing compact proxy spellings');
    let code = '#include <stdint.h>\n#include <stdio.h>\n' + header.slice(start, end) + definition(header, 'OBFH_SF_ASM_BITS') + reference;
    code += `
#if defined(__x86_64__)
#define BRICK_ADDRESS "leaq 1f(%%rip),%%rax;movq %%rax,%[start];leaq 2f(%%rip),%%rax;movq %%rax,%[end];"
#else
#define BRICK_ADDRESS "leal 1f,%%eax;movl %%eax,%[start];leal 2f,%%eax;movl %%eax,%[end];"
#endif
#define BRICK_PROBE(name,body,keyA,keyB,count,draw) static uint64_t name(unsigned x,unsigned y,unsigned *length) {\
uintptr_t start,end; unsigned a,d;\
__asm__ __volatile__(OBFH_SF_ASM_BITS OBFH_SF_SPELLING_CONSTANTS BRICK_ADDRESS "movl %[x],%%eax;movl %[y],%%edx;1:" body "2:movl %%eax,%[a];movl %%edx,%[d];"\
 :[start]"=m"(start),[end]"=m"(end),[a]"=m"(a),[d]"=m"(d)\
 :[x]"m"(x),[y]"m"(y),[key_a]"i"(keyA),[key_b]"i"(keyB),[rotate]"i"(count),[phantom]"i"(draw),[factor]"i"(7),[mask]"i"(0xa5a55a5au),[loops]"i"(3) :"eax","edx","ecx","cc","memory");\
*length=(unsigned)(end-start);return ((uint64_t)a<<32)|d; }
`;
    const actual = [], expected = [], labels = [];
    const configurations = [];
    for (const key of [0, 1, 127, 128, 129, 0x7fffffff, 0x80000000, 0xffffff7f, 0xffffff80, 0xffffff81, 0xffffffff])
        for (const rotate of [1, 2, 31]) for (const draw of [0, 0xffffffff]) configurations.push([key, rotate, draw]);
    for (let rotate = 3; rotate < 31; ++rotate) for (const draw of [0, 0xffffffff]) configurations.push([0x12345678, rotate, draw]);
    for (let c = 0; c < configurations.length; ++c) {
        const [key, rotate, draw] = configurations[c];
        const args = [key, (~key) >>> 0, rotate, draw].map(x => x + 'u').join(',');
        for (const family of ['B', 'C', 'D', 'E', 'K', 'N', 'O', 'R', 'S']) {
            const id = actual.length;
            code += `BRICK_PROBE(brick_actual_${id},OBFH_SF_CORE_${family},${args})\nBRICK_PROBE(brick_reference_${id},BRICK_REFERENCE_${family},${args})\n`;
            actual.push('brick_actual_' + id); expected.push('brick_reference_' + id); labels.push('"' + family + ':' + c + '"');
        }
    }
    code += `int main(void) {
uint64_t (*actual[])(unsigned,unsigned,unsigned*)={${actual.join(',')}};
uint64_t (*expected[])(unsigned,unsigned,unsigned*)={${expected.join(',')}};
const char *labels[]={${labels.join(',')}};
const unsigned edges[]={0,1,127,128,0x7fffffffu,0x80000000u,0xfffffffeu,0xffffffffu};
unsigned state=1;
for(unsigned i=0;i<sizeof(actual)/sizeof(actual[0]);++i)for(unsigned j=0;j<256;++j){
 state=state*1664525u+1013904223u;unsigned x=j<64?edges[j/8]:state;state=state*1664525u+1013904223u;unsigned y=j<64?edges[j%8]:state;
 unsigned a_length,b_length;uint64_t a=actual[i](x,y,&a_length),b=expected[i](x,y,&b_length);
 if(a!=b){fprintf(stderr,"brick value %s input %u\\n",labels[i],j);return 1;}
 if(a_length>b_length){fprintf(stderr,"brick grew %s: %u > %u\\n",labels[i],a_length,b_length);return 2;}
}
if(puts("BRICK_FORMS_PASS")==EOF||fflush(stdout))return 3;return 0;
}\n`;
    return code;
}
module.exports = { fixture };
