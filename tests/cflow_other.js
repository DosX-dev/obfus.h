'use strict';

// Frozen pre-factorization forms and operand lists keep the byte oracle independent.
const reference = String.raw`#define REF_OBFH_JUNK_PAYLOAD ".byte %c2, %c3, %c4, %c5; .long %c6; .fill %c0, 1,((( %c1)&0xffffffff)^0x80000000)-0x80000000;"
#define REF_OBFH_STACK_JUNK_PAYLOAD ".byte %c3, %c4, %c5, %c6; .long %c7; .fill %c1, 1,((( %c2)&0xffffffff)^0x80000000)-0x80000000;"
#define REF_OBFH_CFLOW_LOCAL_RANDOM \
    "i"(__obfh_cf_byte0), "i"(__obfh_cf_byte1), "i"(__obfh_cf_byte2), "i"(__obfh_cf_byte3), "i"(__obfh_cf_payload)
#define REF_OBFH_CFLOW_LOCAL_JUNK_INPUTS "i"(__obfh_cf_pad), "i"(__obfh_cf_byte), REF_OBFH_CFLOW_LOCAL_RANDOM
#define REF_OBFH_CFLOW_LOCAL_JUNK_ASM(code) \
    __obfh_asm__(code \
                 : \
                 : REF_OBFH_CFLOW_LOCAL_JUNK_INPUTS \
                 : OBFH_JUNK_CLOBBERS)
#define REF_OBFH_CFLOW_LOCAL_STACK_ASM(code, ...) \
    __obfh_asm__(code \
                 : \
                 : "i"(__obfh_cf_salt), REF_OBFH_CFLOW_LOCAL_JUNK_INPUTS \
                 : __VA_ARGS__, "cc", "memory")
#define REF_OBFH_CFLOW_TEMPLATE_86 \
    REF_OBFH_CFLOW_LOCAL_JUNK_ASM("xorl %%eax, %%eax; jz 1f; .byte 0xE8; " REF_OBFH_JUNK_PAYLOAD "1: movl $%c6, %%eax; roll $%c0, %%eax; xorl %%eax, %%edx;")
#define REF_OBFH_CFLOW_TEMPLATE_87 ({ \
    enum { __obfh_break_salt = RND(1, 32767), \
           __obfh_break_gap = RND(1, 255) }; \
    __obfh_asm__("movzbl %7, %%eax; xorl %8, %%eax; cmpl %9, %%eax; jne 1f; .byte 0x00, 0xE8; " REF_OBFH_JUNK_PAYLOAD "1:" \
                 : \
                 : REF_OBFH_CFLOW_LOCAL_JUNK_INPUTS, "m"(_0), "i"(__obfh_break_salt), "i"(__obfh_break_salt + __obfh_break_gap) \
                 : OBFH_JUNK_CLOBBERS); \
})
#define REF_OBFH_CFLOW_TEMPLATE_88 ({ \
    enum { __obfh_break_salt = RND(1, 32767), \
           __obfh_break_case1 = RND(129, 191), \
           __obfh_break_case2 = RND(257, 319) }; \
    __obfh_asm__("movzbl %7, %%eax; addl %8, %%eax; cmpl %9, %%eax; jne 1f; .byte 0x00, 0x00; " REF_OBFH_JUNK_PAYLOAD \
                 "1: cmpl %10, %%eax; jne 2f; .byte 0xFF, 0x25; " REF_OBFH_JUNK_PAYLOAD "2:" \
                 : \
                 : REF_OBFH_CFLOW_LOCAL_JUNK_INPUTS, "m"(_0), "i"(__obfh_break_salt), "i"(__obfh_break_salt + __obfh_break_case1), "i"(__obfh_break_salt + __obfh_break_case2) \
                 : OBFH_JUNK_CLOBBERS); \
})
#define REF_OBFH_CFLOW_TEMPLATE_89 \
    REF_OBFH_CFLOW_LOCAL_JUNK_ASM("xorl %%ebx, %%ebx; xorl %%edx, %%edx; xorl %%ebx, %%edx; jz 1f; mov $4, %%eax; .byte 0x00; " REF_OBFH_JUNK_PAYLOAD "1: leal %c6(%%ebx), %%ecx; rorl $%c0, %%ecx;")
#define REF_OBFH_CFLOW_TEMPLATE_90 \
    REF_OBFH_CFLOW_LOCAL_JUNK_ASM("xorl %%ebx, %%ebx; xorl %%eax, %%eax; mov %%eax, %%ebx; mov %%edx, %%ebx; xorl %%ebx, %%edx; jz 1f; .byte 0x20; " REF_OBFH_JUNK_PAYLOAD "1: movl $%c6, %%edx; bswap %%edx; xorl %%edx, %%eax;")
#define REF_OBFH_CFLOW_TEMPLATE_91 \
    REF_OBFH_CFLOW_LOCAL_JUNK_ASM("xorl %%edx, %%edx; xorl %%eax, %%eax; mov %%eax, %%edx; jz 1f; .byte 0xE8; " REF_OBFH_JUNK_PAYLOAD "1: movl $%c6, %%ecx; negl %%ecx; roll $%c0, %%ecx;")
#define REF_OBFH_CFLOW_TEMPLATE_92 \
    REF_OBFH_CFLOW_LOCAL_JUNK_ASM("xorl %%edx, %%edx; jz 1f; .byte 0xE8; " REF_OBFH_JUNK_PAYLOAD "1: movl $%c6, %%eax; notl %%eax; leal (%%eax,%%eax,2), %%edx;")
#define REF_OBFH_CFLOW_TEMPLATE_93 \
    REF_OBFH_CFLOW_LOCAL_JUNK_ASM("xorl %%eax, %%eax; jz 1f; .byte 0x50; " REF_OBFH_JUNK_PAYLOAD "1: movl $%c6, %%ecx; rorl $%c0, %%ecx; xorl %%ecx, %%eax;")
#define REF_OBFH_CFLOW_TEMPLATE_94 \
    REF_OBFH_CFLOW_LOCAL_JUNK_ASM("xorl %%edx, %%edx; jz 1f; .byte 0x00, 0x00; " REF_OBFH_JUNK_PAYLOAD "1: movl $%c6, %%edx; roll $%c0, %%edx; leal (%%edx,%%edx,4), %%eax;")
#define REF_OBFH_CFLOW_TEMPLATE_95 \
    REF_OBFH_CFLOW_LOCAL_STACK_ASM("movl %%esp, %%eax; addl %0, %%eax; leal 1(%%eax), %%edx; imull %%edx, %%eax; testb $1, %%al; jz 1f; .byte 0xE8; " REF_OBFH_STACK_JUNK_PAYLOAD "1:", "eax", "edx")
#define REF_OBFH_CFLOW_TEMPLATE_96 \
    REF_OBFH_CFLOW_LOCAL_STACK_ASM("movl %%esp, %%edx; addl %0, %%edx; imull %%edx, %%edx; testb $2, %%dl; jz 1f; .byte 0xFF, 0x25; " REF_OBFH_STACK_JUNK_PAYLOAD "1:", "edx")
#define REF_OBFH_CFLOW_TEMPLATE_97 \
    REF_OBFH_CFLOW_LOCAL_STACK_ASM("movl %%esp, %%eax; addl %0, %%eax; movl %%eax, %%edx; imull %%eax, %%eax; xorl %%edx, %%eax; testb $1, %%al; jz 1f; .byte 0x0F, 0x0B, 0xE8; " REF_OBFH_STACK_JUNK_PAYLOAD "1:", "eax", "edx")
#define REF_OBFH_CFLOW_TEMPLATE_98 \
    REF_OBFH_CFLOW_LOCAL_STACK_ASM("movl %%esp, %%eax; addl %0, %%eax; leal 1(%%eax), %%edx; imull %%edx, %%eax; addl $1, %%eax; testb $1, %%al; jnz 1f; .byte 0xC3, 0xE8; " REF_OBFH_STACK_JUNK_PAYLOAD "1:", "eax", "edx")
`;

function fixture(header) {
    const first = header.indexOf('#define OBFH_JUNK_PAYLOAD '), last = header.indexOf('// Native fragments are adjacent', first);
    const forms = header.indexOf('#define OBFH_CFLOW_TEMPLATE_86'), end = header.indexOf('#define OBFH_CFLOW_TEMPLATE_99', forms);
    if (first < 0 || last < first || forms < 0 || end < forms) throw new Error('Missing miscellaneous CFLOW forms');
    let code = '#include <stdint.h>\n#include <stdio.h>\n#define __obfh_asm__ __asm__\n#define OBFH_JUNK_CLOBBERS "eax","ebx","ecx","edx","cc","memory"\n#define OBFH_CFLOW_CLOBBERS "eax","edx","ecx","cc","memory"\n' +
        header.slice(first, last) + header.slice(forms, end) + reference;
    code += `
static volatile unsigned char _0=0;
typedef struct { const unsigned char *start,*end; } OtherSpan;
#define RND(lo,hi) ((lo)+(__other_draw%((hi)-(lo)+1u)))
#if defined(__x86_64__)
#define OTHER_ADDRESS "leaq 8301f(%%rip),%%rax;movq %%rax,%[start];leaq 8302f(%%rip),%%rax;movq %%rax,%[end];jmp 8309f;8301:"
#else
#define OTHER_ADDRESS "leal 8301f,%%eax;movl %%eax,%[start];leal 8302f,%%eax;movl %%eax,%[end];jmp 8309f;8301:"
#endif
#define OTHER_CARRIER(name,body,drawValue,padValue,byteValue,keyValue) static OtherSpan name(void){OtherSpan s;\
enum{__other_draw=(drawValue),__obfh_cf_salt=1u+((drawValue)%32767u),__obfh_cf_pad=(padValue),__obfh_cf_byte=(byteValue),\
__obfh_cf_byte0=(keyValue)&255u,__obfh_cf_byte1=((keyValue)>>8)&255u,__obfh_cf_byte2=((keyValue)>>16)&255u,__obfh_cf_byte3=((keyValue)>>24)&255u,__obfh_cf_payload=(keyValue)};\
__asm__ __volatile__(".set .Lcfpad,%c[payload_pad];.set .Lcfbyte,%c[payload_byte];.set .Lcfb0,%c[b0];.set .Lcfb1,%c[b1];.set .Lcfb2,%c[b2];.set .Lcfb3,%c[b3];.set .Lcfdata,%c[payload_key];" OTHER_ADDRESS\
 :[start]"=m"(s.start),[end]"=m"(s.end):[payload_pad]"i"(padValue),[payload_byte]"i"(byteValue),[payload_key]"i"(keyValue),[b0]"i"((keyValue)&255u),[b1]"i"(((keyValue)>>8)&255u),[b2]"i"(((keyValue)>>16)&255u),[b3]"i"(((keyValue)>>24)&255u):OBFH_JUNK_CLOBBERS);\
body;__asm__ __volatile__("8302:8309:");return s;}
`;
    const pairs = [];
    for (let config = 0; config < 24; ++config) {
        const args = [[0, 126, 127, 32766][config % 4], config % 7 + 1, [0, 128, 255][config % 3],
        [0, 1, 127, 128, 0x7fffffff, 0xffffffff][Math.floor(config / 4)]].map(value => value + 'u').join(',');
        for (let form = 86; form < 99; ++form) {
            const id = pairs.length;
            const actual = form >= 95 && header.includes('#define OBFH_CFLOW_STACK_EMIT(') ? `OBFH_CFLOW_STACK_EMIT(${form}u)` : `OBFH_CFLOW_TEMPLATE_${form}`;
            code += `OTHER_CARRIER(other_actual_${id},${actual},${args})\nOTHER_CARRIER(other_reference_${id},REF_OBFH_CFLOW_TEMPLATE_${form},${args})\n`;
            pairs.push(`{other_actual_${id},other_reference_${id}}`);
        }
    }
    return code + `static intptr_t other_displacement(const unsigned char*p){return (int32_t)((unsigned)p[0]|((unsigned)p[1]<<8)|((unsigned)p[2]<<16)|((unsigned)p[3]<<24));}
int main(void){OtherSpan(*pairs[][2])(void)={${pairs.join(',')}};
for(unsigned i=0;i<sizeof(pairs)/sizeof(pairs[0]);++i){OtherSpan a=pairs[i][0](),b=pairs[i][1]();
if(a.end-a.start!=b.end-b.start){fprintf(stderr,"other length %u:%u\\n",i/13,i%13+86);return 1;}
unsigned relocated=~0u;
#if defined(__x86_64__)
/* Separate carriers have different PCs. Validate the RIP target, then compare every other byte. */
if(i%13==1||i%13==2){for(unsigned j=0;j<12&&j+7<=(unsigned)(a.end-a.start);++j)
if(a.start[j]==15&&a.start[j+1]==182&&a.start[j+2]==5&&b.start[j]==15&&b.start[j+1]==182&&b.start[j+2]==5){
if((uintptr_t)(a.start+j+7)+other_displacement(a.start+j+3)!=(uintptr_t)&_0||(uintptr_t)(b.start+j+7)+other_displacement(b.start+j+3)!=(uintptr_t)&_0)return 3;relocated=j+3;break;}
if(relocated==~0u)return 4;}
#endif
for(unsigned j=0;j<(unsigned)(a.end-a.start);++j)if((relocated==~0u||j<relocated||j>=relocated+4)&&a.start[j]!=b.start[j]){fprintf(stderr,"other bytes %u:%u:%u\\n",i/13,i%13+86,j);return 2;}}
if(puts("CFLOW_OTHER_PASS")==EOF||fflush(stdout))return 3;return 0;}\n`;
}
module.exports = { fixture };
