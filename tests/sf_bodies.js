'use strict';
const { macroDefinition } = require('./source_macros');
// Frozen complete cores predate shared leaf entry/exit emission. The oracle
// retains its own body composition and never calls SLOT_FRAMED/SLOT_LEAF.
const reference = String.raw`#define REF_OBFH_SF_CORE_A "xorl $%c[key_a], %%eax; imull $%c[factor], %%eax; addl %%edx, %%eax;"
#define REF_OBFH_SF_CORE_B OBFH_SF_ADD_B OBFH_SF_ROTATE_B " xorl %%edx, %%eax;"
#define REF_OBFH_SF_CORE_C "leal (%%eax, %%eax, 2), %%eax; xorl %%eax, %%edx; " OBFH_SF_ADD_A
#define REF_OBFH_SF_CORE_D "notl %%eax; addl %%edx, %%eax; imull $%c[factor], %%eax; " OBFH_SF_ROTATE_D
#define REF_OBFH_SF_CORE_E "bswap %%eax; xorl %%edx, %%eax; " OBFH_SF_ROTATE_E
#define REF_OBFH_SF_CORE_F "movl $%c[loops], %%ecx; 6: addl %%edx, %%eax; imull $%c[factor], %%eax; xorl $%c[key_b], %%eax; decl %%ecx; jnz 6b;"
#define REF_OBFH_SF_CORE_G "movzbl %%al, %%ecx; shrl $8, %%eax; xorl %%edx, %%ecx; leal (%%eax, %%ecx, 4), %%eax;"
#define REF_OBFH_SF_CORE_H "imull %%eax, %%edx; xorl $%c[key_b], %%edx; subl %%edx, %%eax; negl %%eax;"
#define REF_OBFH_SF_CORE_I "movl %%eax, %%ecx; orl $%c[mask], %%eax; andl $%c[mask], %%ecx; subl %%ecx, %%eax; xorl %%edx, %%eax;"
#define REF_OBFH_SF_CORE_J "xorl $%c[key_a], %%eax; movl %%edx, %%ecx; shll $%c[rotate], %%ecx; shrl $%c[rotate], %%eax; orl %%ecx, %%eax;"
#define REF_OBFH_SF_CORE_K "testl %%edx, %%edx; js 6f; " OBFH_SF_ADD_A " .byte 0xeb,4; 6: negl %%eax; xorl %%edx, %%eax; 7: " OBFH_SF_ROTATE_K
#define REF_OBFH_SF_CORE_L "movl %%eax, %%ecx; movl %%edx, %%eax; addl %%ecx, %%eax; imull $%c[factor], %%eax; notl %%edx; xorl %%edx, %%eax;"
#define REF_OBFH_SF_CORE_M "movl %%eax, %%ecx; shrl $16, %%ecx; xorl %%ecx, %%eax; imull $%c[factor], %%eax; xorl %%edx, %%eax;"
#define REF_OBFH_SF_CORE_N "addl $%c[key_a], %%eax; adcl $%c[key_b], %%edx; xorl %%edx, %%eax; " OBFH_SF_ROTATE_N
#define REF_OBFH_SF_CORE_O "xchgl %%eax, %%edx; " OBFH_SF_SUB_A " bswap %%edx; addl %%edx, %%eax;"
#define REF_OBFH_SF_CORE_P "movl $%c[factor], %%ecx; xorl %%edx, %%edx; divl %%ecx; xorl %%edx, %%eax;"
#define REF_OBFH_SF_CORE_Q "movzbl %%al, %%ecx; movzbl %%dl, %%edx; imull %%edx, %%ecx; shrl $8, %%eax; addl %%ecx, %%eax;"
#define REF_OBFH_SF_CORE_R "movl $%c[loops], %%ecx; 6: xorl %%edx, %%eax; " OBFH_SF_ROTATE_R " addl $%c[key_a], %%edx; decl %%ecx; jnz 6b;"
#define REF_OBFH_SF_CORE_S "shldl $%c[rotate], %%edx, %%eax; " OBFH_SF_SUB_B " xorl $%c[mask], %%eax;"
#define REF_OBFH_SF_CORE_T "movl %%eax, %%ecx; shrl $16, %%eax; shll $16, %%ecx; orl %%ecx, %%eax; xorl %%edx, %%eax; negl %%eax;"
#define REF_OBFH_SF_LEAF_CORE_A OBFH_SF_LEAF_ALU_A "cmpl %%edx, %%eax; jbe 6f;" OBFH_SF_LEAF_SHIFT "subl %%edx, %%eax; ret; 6:" OBFH_SF_LEAF_ALU_B "leal (%%eax, %%edx, 2), %%eax; ret;"
#define REF_OBFH_SF_LEAF_CORE_B OBFH_SF_LEAF_ENTER OBFH_SF_LEAF_STORE OBFH_SF_LEAF_ALU_A OBFH_SF_LEAF_SHIFT OBFH_SF_LEAF_LOAD "xorl %%ecx, %%eax; addl %%edx, %%eax;" OBFH_SF_LEAF_EXIT
#define REF_OBFH_SF_LEAF_CORE_C OBFH_SF_LEAF_ENTER OBFH_SF_LEAF_STORE "movl %%edx, %%ecx; roll $%c[rotate], %%ecx;" OBFH_SF_LEAF_COMPARE("1", "0x7d", "0x7e", "11", "9") OBFH_SF_LEAF_ALU_A "xorl %%ecx, %%eax; .byte 0xeb,(4 + ((.Lsr != 1) & 1)); 6:" OBFH_SF_LEAF_SHIFT "addl %%edx, %%eax; 7:" OBFH_SF_LEAF_LOAD "subl %%ecx, %%eax;" OBFH_SF_LEAF_EXIT
#define REF_OBFH_SF_LEAF_CORE_D "movl $%c[loops], %%ecx; 6:" OBFH_SF_LEAF_ALU_A OBFH_SF_LEAF_SHIFT OBFH_SF_LEAF_ALU_B "addl %%edx, %%eax; decl %%ecx; jnz 6b; ret;"
#define REF_OBFH_SF_LEAF_CORE_E "cmpl $%c[key_a], %%eax; jb 6f; bswap %%eax;" OBFH_SF_LEAF_ALU_A "ret; 6: movl $%c[factor], %%ecx; xorl %%edx, %%edx; divl %%ecx; xorl %%edx, %%eax;" OBFH_SF_LEAF_SHIFT "ret;"
#define REF_OBFH_SF_LEAF_CORE_F OBFH_SF_LEAF_ENTER OBFH_SF_LEAF_STORE OBFH_SF_LEAF_REVERSE_SUB OBFH_SF_LEAF_ALU_A OBFH_SF_LEAF_LOAD "xorl %%ecx, %%eax;" OBFH_SF_LEAF_ALU_B "adcl %%edx, %%eax;" OBFH_SF_LEAF_EXIT
#define REF_OBFH_SF_LEAF_CORE_G "movl %%eax, %%ecx;" OBFH_SF_LEAF_SHIFT "testl $%c[mask], %%ecx; jnz 6f;" OBFH_SF_LEAF_ALU_A "ret; 6: imull $%c[factor], %%eax;" OBFH_SF_LEAF_ALU_B "xorl %%edx, %%eax; ret;"
#define REF_OBFH_SF_LEAF_CORE_H OBFH_SF_LEAF_ENTER OBFH_SF_LEAF_STORE "movl $%c[loops], %%ecx; 6:" OBFH_SF_LEAF_SHIFT "addl %%edx, %%eax; decl %%ecx; jnz 6b;" OBFH_SF_LEAF_LOAD "xorl %%ecx, %%eax;" OBFH_SF_LEAF_EXIT
#define REF_OBFH_SF_LEAF_CORE_I "bswap %%eax;" OBFH_SF_LEAF_ALU_A "cmpl %%edx, %%eax; jne 6f;" OBFH_SF_LEAF_SHIFT "ret; 6: imull $%c[factor], %%eax; subl %%edx, %%eax; ret;"
#define REF_OBFH_SF_LEAF_CORE_J OBFH_SF_LEAF_ENTER OBFH_SF_LEAF_STORE OBFH_SF_LEAF_ALU_B "testl %%edx, %%edx; js 6f;" OBFH_SF_LEAF_SHIFT ".byte 0xeb,2; 6: negl %%eax; 7:" OBFH_SF_LEAF_LOAD "addl %%ecx, %%eax;" OBFH_SF_LEAF_EXIT
#define REF_OBFH_SF_LEAF_CORE_K "movzbl %%al, %%ecx; shrl $8, %%eax; imull $%c[factor], %%ecx; xorl %%ecx, %%eax;" OBFH_SF_LEAF_ALU_A OBFH_SF_LEAF_SHIFT "addl %%edx, %%eax; ret;"
#define REF_OBFH_SF_LEAF_CORE_L OBFH_SF_LEAF_ENTER OBFH_SF_LEAF_STORE "cmpl %%edx, %%eax; jbe 6f;" OBFH_SF_LEAF_ALU_A ".byte 0xeb,8; 6:" OBFH_SF_LEAF_ALU_B "xorl %%edx, %%eax; 7:" OBFH_SF_LEAF_LOAD "subl %%ecx, %%eax;" OBFH_SF_LEAF_EXIT
#if defined(__x86_64__)
#define REF_OBFH_SF_ARGS \
    OBFH_SF_BIT_BYTES("9", "0", "2", "0xc889") \
    OBFH_SF_BIT_BYTES("9", "1", "3", "0x00418d")
#define REF_OBFH_SF_LEAF_ARGS REF_OBFH_SF_ARGS ".fill (%c[phantom] & 1),1,0x90;"
#else
#define REF_OBFH_SF_ARGS \
    OBFH_SF_BIT_BYTES("9", "0", "3", "0x0c558b") \
    OBFH_SF_BIT_BYTES("9", "0", "3", "0x08458b") \
    OBFH_SF_BIT_BYTES("9", "1", "3", "0x08458b") \
    OBFH_SF_BIT_BYTES("9", "1", "3", "0x0c558b")
#define REF_OBFH_SF_LEAF_ARGS "movl 4(%%esp),%%eax;movl 8(%%esp),%%edx;.fill (%c[phantom]&1),1,0x90;"
#endif
#define REF_OBFH_SF_LOCAL \
    OBFH_SF_BIT_BYTES("8", "0", "2", "0x4589") \
    OBFH_SF_BIT_BYTES("8", "0", "1", "-%c[local_a]") \
    OBFH_SF_BIT_BYTES("8", "0", "2", "0x5589") \
    OBFH_SF_BIT_BYTES("8", "0", "1", "-%c[local_b]") \
    OBFH_SF_BIT_BYTES("8", "1", "2", "0x5589") \
    OBFH_SF_BIT_BYTES("8", "1", "1", "-%c[local_b]") \
    OBFH_SF_BIT_BYTES("8", "1", "2", "0x4589") \
    OBFH_SF_BIT_BYTES("8", "1", "1", "-%c[local_a]")
`;
function fixture(source) {
    const first = source.indexOf('#define OBFH_SF_BYTES');
    const last = source.indexOf('#define OBFH_SF_ASM_BITS', first);
    if (first < 0 || last < first) throw new Error('Missing native body helpers');
    const names = ['OBFH_SF_ASM_BITS', 'OBFH_SF_OPERAND', 'OBFH_SF_FRAME_ASM', 'OBFH_SF_LEAF_ASM', 'OBFH_SF_SLOT_FRAMED', 'OBFH_SF_SLOT_LEAF',
        ...Array.from('ABCDEFGHIJKL', x => 'OBFH_SF_ID_LEAF_' + x)];
    const definitions = names.map(name => macroDefinition(source, name).body).join('\n');
    let code = '#include <stdint.h>\n#include <stdio.h>\n#define __obfh_asm__ __asm__ __volatile__\n' + source.slice(first, last) + definitions + '\n' + reference + `
typedef struct {const unsigned char *start,*end;} BodySpan;
#if defined(__x86_64__)
#define BODY_ADDRESS "leaq 8351f(%%rip),%%rax;movq %%rax,%[start];leaq 8352f(%%rip),%%rax;movq %%rax,%[end];jmp 8359f;8351:"
#else
#define BODY_ADDRESS "leal 8351f,%%eax;movl %%eax,%[start];leal 8352f,%%eax;movl %%eax,%[end];jmp 8359f;8351:"
#endif
#define BODY_INPUTS [phantom]"i"(__obfh_sf_phantom),[key_a]"i"(__obfh_sf_key_a),[key_b]"i"(__obfh_sf_key_b),[rotate]"i"(__obfh_sf_rotate),[factor]"i"(__obfh_sf_factor),[loops]"i"(__obfh_sf_loops),[mask]"i"(__obfh_sf_mask),[local_a]"i"(__obfh_sf_local_a),[local_b]"i"(__obfh_sf_local_b),[leaf_frame]"i"(__obfh_sf_leaf_frame),[leaf_slot]"i"(__obfh_sf_leaf_slot),[leaf_alu_a]"i"(__obfh_sf_leaf_alu_a),[leaf_alu_b]"i"(__obfh_sf_leaf_alu_b),[leaf_shift]"i"(__obfh_sf_leaf_shift)
#define BODY_CARRIER(name,body,drawValue,keyValue,countValue,shiftValue) static BodySpan name(void){BodySpan s;\
enum{__obfh_sf_phantom=drawValue,__obfh_sf_key_a=keyValue,__obfh_sf_key_b=~(unsigned)(keyValue),__obfh_sf_rotate=countValue,__obfh_sf_factor=5,__obfh_sf_loops=3,__obfh_sf_mask=0x55aa55aa,__obfh_sf_local_a=4+4*((drawValue>>7)%3),__obfh_sf_local_b=16+4*((drawValue>>8)&3),__obfh_sf_leaf_frame=40,__obfh_sf_leaf_slot=16,__obfh_sf_leaf_alu_a=0x35,__obfh_sf_leaf_alu_b=0xf2,__obfh_sf_leaf_shift=shiftValue};\
__obfh_asm__(OBFH_SF_ASM_BITS OBFH_SF_SPELLING_CONSTANTS OBFH_SF_LEAF_CONSTANTS BODY_ADDRESS :[start]"=m"(s.start),[end]"=m"(s.end):BODY_INPUTS:"eax","ecx","edx","cc","memory");\
body;__obfh_asm__("8352:8359:");return s;}
`;
    const pairs = [];
    for (let config = 0; config < 8; ++config) for (let body = 0; body < 32; ++body) {
        const id = pairs.length, leaf = body >= 20, letter = String.fromCharCode(65 + (leaf ? body - 20 : body));
        const actual = leaf ? `OBFH_SF_SLOT_LEAF(${body}u)` : `OBFH_SF_SLOT_FRAMED(${body}u)`;
        const referenceBody = leaf ? `REF_OBFH_SF_LEAF_ARGS REF_OBFH_SF_LEAF_CORE_${letter}` : `REF_OBFH_SF_ARGS REF_OBFH_SF_CORE_${letter} REF_OBFH_SF_LOCAL`;
        const args = [[0, 0x1ffff, 0x15555, 0xaaaa][config % 4] + 'u', [0, 127, 128, 0xffffffff][config % 4] + 'u', config < 4 ? 1 : 31, [0xc0, 0xc8, 0xe0, 0xe8, 0xf8][config % 5]].join(',');
        code += `BODY_CARRIER(body_actual_${id},${actual},${args})\nBODY_CARRIER(body_reference_${id},__obfh_asm__(${referenceBody}::BODY_INPUTS),${args})\n`;
        pairs.push(`{body_actual_${id},body_reference_${id}}`);
    }
    return code + `int main(void){BodySpan(*pairs[][2])(void)={${pairs.join(',')}};
for(unsigned i=0;i<sizeof(pairs)/sizeof(pairs[0]);++i){BodySpan a=pairs[i][0](),b=pairs[i][1]();
if(a.end-a.start!=b.end-b.start){fprintf(stderr,"body length %u:%u\\n",i/32,i%32);return 1;}
for(unsigned n=0;n<(unsigned)(a.end-a.start);++n)if(a.start[n]!=b.start[n]){fprintf(stderr,"body bytes %u:%u:%u\\n",i/32,i%32,n);return 2;}}
if(puts("SF_BODIES_PASS")==EOF||fflush(stdout))return 3;return 0;}\n`;
}
module.exports = { fixture };
