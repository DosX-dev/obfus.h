'use strict';
// Frozen literal encodings independently guard the capture-to-ASM boundary.
// Bodies are skipped: this oracle checks bytes; native pool tests execute them.
exports.fixture = function (source) {
    const start = source.indexOf('#define OBFH_SF_BYTES'), end = source.indexOf('#define OBFH_SF_ASM_BITS', start);
    if (start < 0 || end < start) throw new Error('Missing native proxy fragments');
    let code = '#include <stdint.h>\n#include <stdio.h>\n' + source.slice(start, end) +
        require('./stack_frames').aliases(source, (ok, message) => { if (!ok) throw new Error(message); });
    code += `
typedef struct { const unsigned char *start,*end; } ImmediateSpan;
#if defined(__x86_64__)
#define IMMEDIATE_ADDRESS "leaq 1f(%%rip),%%rax;movq %%rax,%[start];leaq 2f(%%rip),%%rax;movq %%rax,%[end];jmp 9f;1:"
#define IMMEDIATE_REFERENCE_FRAME "subq $%c[leaf_frame],%%rsp;movl %%eax,%c[leaf_slot](%%rsp);movl %c[leaf_slot](%%rsp),%%ecx;addq $%c[leaf_frame],%%rsp;ret;"
#else
#define IMMEDIATE_ADDRESS "leal 1f,%%eax;movl %%eax,%[start];leal 2f,%%eax;movl %%eax,%[end];jmp 9f;1:"
#define IMMEDIATE_REFERENCE_FRAME "subl $%c[leaf_frame],%%esp;movl %%eax,%c[leaf_slot](%%esp);movl %c[leaf_slot](%%esp),%%ecx;addl $%c[leaf_frame],%%esp;ret;"
#endif
#define IMMEDIATE_REFERENCE IMMEDIATE_REFERENCE_FRAME ".byte %c[leaf_alu_a];.long %c[key_a];.byte 0x81,%c[leaf_alu_b];.long %c[key_b];.byte (0xc1+0x10*((%c[rotate]==1)&1)),%c[leaf_shift];.fill ((%c[rotate]!=1)&1),1,%c[rotate];"
#define IMMEDIATE_ACTUAL OBFH_SF_LEAF_ENTER OBFH_SF_LEAF_STORE OBFH_SF_LEAF_LOAD OBFH_SF_LEAF_EXIT OBFH_SF_LEAF_ALU_A OBFH_SF_LEAF_ALU_B OBFH_SF_LEAF_SHIFT
#define IMMEDIATE_PROBE(name,body,frame,slot,key,aluA,aluB,shift,count) static ImmediateSpan name(void){ImmediateSpan s;\
__asm__ __volatile__(OBFH_SF_ASM_BITS OBFH_SF_SPELLING_CONSTANTS OBFH_SF_LEAF_CONSTANTS IMMEDIATE_ADDRESS body "2:9:"\
 :[start]"=m"(s.start),[end]"=m"(s.end)\
 :[phantom]"i"(0),[leaf_frame]"i"(frame),[leaf_slot]"i"(slot),[key_a]"i"(key),[key_b]"i"(~(key)),[leaf_alu_a]"i"(aluA),[leaf_alu_b]"i"(aluB),[leaf_shift]"i"(shift),[rotate]"i"(count):"eax","ecx","edx","cc","memory");return s;}
`;
    const configurations = [];
    for (let frame = 40; frame <= 104; frame += 16)
        for (let slot = 0; slot <= 24; slot += 4) configurations.push([frame, slot, 0, 0x05, 0xc2, 0xc0, 1]);
    for (let rotate = 1; rotate <= 31; ++rotate)
        for (const shift of [0xc0, 0xc8, 0xe0, 0xe8, 0xf8]) configurations.push([40, 0, 1, 0x35, 0xf2, shift, rotate]);
    for (const key of [0, 1, 127, 128, 129, 0x7fffffff, 0x80000000, 0xffffff7f, 0xffffff80, 0xffffff81, 0xffffffff])
        for (const a of [0x05, 0x0d, 0x25, 0x2d, 0x35])
            for (const b of [0xc2, 0xca, 0xd2, 0xda, 0xe2, 0xea, 0xf2]) configurations.push([104, 24, key, a, b, 0xe8, 31]);
    const pairs = [];
    configurations.forEach((args, index) => {
        const text = args.map(value => value + 'u').join(',');
        code += `IMMEDIATE_PROBE(immediate_actual_${index},IMMEDIATE_ACTUAL,${text})\nIMMEDIATE_PROBE(immediate_reference_${index},IMMEDIATE_REFERENCE,${text})\n`;
        pairs.push(`{immediate_actual_${index},immediate_reference_${index}}`);
    });
    code += `int main(void){ImmediateSpan(*pairs[][2])(void)={${pairs.join(',')}};
for(unsigned i=0;i<sizeof(pairs)/sizeof(pairs[0]);++i){ImmediateSpan a=pairs[i][0](),b=pairs[i][1]();
if(a.end-a.start!=b.end-b.start){fprintf(stderr,"immediate length %u\\n",i);return 1;}
for(unsigned j=0;j<(unsigned)(a.end-a.start);++j)if(a.start[j]!=b.start[j]){fprintf(stderr,"immediate bytes %u:%u\\n",i,j);return 2;}}
if(puts("SF_IMMEDIATES_PASS")==EOF||fflush(stdout))return 3;return 0;}\n`;
    return code;
};
