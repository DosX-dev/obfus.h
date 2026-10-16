'use strict';

// A contiguous reference emitter keeps the original 86 layouts independent of
// the production fragment selectors, register formula and mask formula.
const predicates = [
    ['ADJACENT_PRODUCT', ['eax', 'edx'], 1, false],
    ['SQUARE', ['eax'], 2, false],
    ['SQUARE_XOR', ['eax', 'edx'], 1, false],
    ['ODD_PRODUCT', ['eax', 'edx'], 1, true],
    ['CUBE_MINUS', ['edx', 'ecx'], 1, false],
    ['THREE_PRODUCT', ['edx', 'ecx', 'eax'], 1, false],
    ['FOUR_PRODUCT', ['edx', 'ecx', 'eax'], 7, false],
    ['SQUARE_PRODUCT', ['edx', 'ecx'], 3, false],
    ['FOURTH', ['ecx'], 14, false],
    ['FIFTH_MINUS', ['ecx', 'eax'], 1, false],
    ['ADJACENT_OR', ['ecx', 'eax'], 1, true],
    ['ADJACENT_AND', ['ecx', 'eax'], 1, false],
    ['PREVIOUS_PRODUCT', ['eax', 'ecx'], 1, false],
    ['SQUARE_PLUS', ['eax', 'ecx'], 1, false],
    ['CUBE_PLUS', ['eax', 'ecx'], 1, false],
    ['ODD_SQUARE', ['eax'], 2, false]
];
const secondPredicates = [7, 8, 9, 10, 11, 12, 13, 14, 15, 0, 1, 2, 3, 4, 5, 6];

function reference(index) {
    const family = index % 16;
    const style = index < 16 ? 0 : index < 32 ? 1 : index < 48 ? 2 : index < 64 ? 3 : index < 80 ? 4 : 5;
    const [name, registers, mask, nonzero] = predicates[family];
    const low = { eax: 'al', edx: 'dl', ecx: 'cl' };
    const predicate = item => 'OBFH_CFLOW_PRED_' + item[0] + '(' + item[1].map(reg => JSON.stringify('%%' + reg)).join(',') + ')';
    const test = item => 'testb $' + item[2] + ', %%' + low[item[1][0]] + ';';
    const inverse = style === 1 || style === 3 || style === 5;
    const op = style === 1 || style === 5 ? 'xorl' : style === 2 ? 'subl' : 'addl';
    const reg = '%%' + registers[0];
    const parts = [JSON.stringify('movl %%esp, ' + reg + ';' + (style === 3 ? 'roll $7, ' + reg + ';' : '') + op + ' $.Lcfs, ' + reg + ';')];
    parts.push(predicate(predicates[family]));
    parts.push(JSON.stringify(test(predicates[family]) + ((nonzero !== inverse) ? 'jnz' : 'jz') + ' 1f;' + (inverse ? 'jmp 2f;1:' : '') + (style === 3 ? '.byte 0xc3;' : '')));
    parts.push(style >= 4 ? 'OBFH_CFLOW_PAYLOAD_CALL_DATA' : ['OBFH_CFLOW_PAYLOAD_CALL', 'OBFH_CFLOW_PAYLOAD_INDIRECT', 'OBFH_CFLOW_PAYLOAD_MOV', 'OBFH_CFLOW_PAYLOAD_JUMP'][index % 4]);
    parts.push(JSON.stringify(inverse ? '2:' : '1:'));
    if (style === 2 || style === 3) {
        parts.push(JSON.stringify(test(predicates[family]) + (nonzero ? 'jnz' : 'jz') + (style === 2 ? ' 2f;.byte 0x0f,0x0b,0xe8;.fill .Lcfpad,1,.Lcfbyte;2:' : ' 3f;.byte 0xff,0x25;.long .Lcfbasic;3:')));
    } else if (style >= 4) {
        const second = predicates[secondPredicates[family]];
        const target = style === 4 ? '2' : '3';
        parts.push(JSON.stringify('movl %%esp, %%' + second[1][0] + ';addl $.Lcfs, %%' + second[1][0] + ';'));
        parts.push(predicate(second));
        parts.push(JSON.stringify(test(second) + (second[3] ? 'jnz' : 'jz') + ' ' + target + 'f;' + (style === 5 ? '.byte 0x0f,0x0b;' : '') + '.byte 0xff,0x25;.long .Lcfbasic;.byte .Lcfbyte,0xe9;' + target + ':'));
    }
    return parts.join(' ');
}

function fixture(header) {
    const start = header.indexOf('#define OBFH_CFLOW_FILL ');
    const end = header.indexOf('#define OBFH_CFLOW_VARIANT_COUNT ', start);
    if (start < 0 || end < start || !header.includes('#define OBFH_CFLOW_BASIC_EMIT(')) throw new Error('Missing factored basic CFLOW emitter');
    let code = '#include <stdint.h>\n#include <stdio.h>\n#define __obfh_asm__ __asm__\n#define OBFH_CFLOW_CLOBBERS "eax","ecx","edx","cc","memory"\n' + header.slice(start, end);
    code += `
typedef struct {const unsigned char *start,*end;} BasicSpan;
#if defined(__x86_64__)
#define BASIC_ADDRESS "leaq 1f(%%rip),%%rax;movq %%rax,%[start];leaq 9f(%%rip),%%rax;movq %%rax,%[end];jmp 9f;1:"
#else
#define BASIC_ADDRESS "leal 1f,%%eax;movl %%eax,%[start];leal 9f,%%eax;movl %%eax,%[end];jmp 9f;1:"
#endif
#define BASIC_CARRIER(name,body,salt,pad,byte,key) static BasicSpan name(void){BasicSpan span;\
__asm__ __volatile__(".set .Lcfs,%c[s];.set .Lcfpad,%c[p];.set .Lcfbyte,%c[b];.set .Lcfbasic,%c[k];" BASIC_ADDRESS\
: [start]"=m"(span.start),[end]"=m"(span.end):[s]"i"(salt),[p]"i"(pad),[b]"i"(byte),[k]"i"(key):OBFH_CFLOW_CLOBBERS);\
body;__asm__ __volatile__("9:");return span;}
`;
    const configurations = [[1, 1, 0, 1], [127, 3, 255, 0x7ffffffe], [128, 4, 0xa5, 0x12345678], [32767, 7, 0x80, 0x60000001]];
    const actual = [], expected = [];
    for (let config = 0; config < configurations.length; ++config) {
        for (let index = 0; index < 86; ++index) {
            const id = config * 86 + index;
            const args = configurations[config].map(value => value + 'u').join(',');
            code += `BASIC_CARRIER(basic_actual_${id},OBFH_CFLOW_BASIC_EMIT(${index}u),${args})\n`;
            code += `BASIC_CARRIER(basic_reference_${id},OBFH_CFLOW_LOCAL_ASM(${reference(index)}),${args})\n`;
            actual.push('basic_actual_' + id);
            expected.push('basic_reference_' + id);
        }
    }
    code += `int main(void){
BasicSpan (*actual[])(void)={${actual.join(',')}};
BasicSpan (*expected[])(void)={${expected.join(',')}};
for(unsigned i=0;i<sizeof(actual)/sizeof(actual[0]);++i){
BasicSpan a=actual[i](),b=expected[i]();uintptr_t length=(uintptr_t)a.end-(uintptr_t)a.start;
if(length!=(uintptr_t)b.end-(uintptr_t)b.start||length>256){fprintf(stderr,"basic CFLOW length: config %u template %u\\n",i/86,i%86);return 1;}
for(uintptr_t n=0;n<length;++n)if(a.start[n]!=b.start[n]){fprintf(stderr,"basic CFLOW byte: config %u template %u offset %u\\n",i/86,i%86,(unsigned)n);return 2;}
}
if(puts("CFLOW_BASIC_PASS")==EOF||fflush(stdout))return 3;return 0;}
`;
    return code;
}
module.exports = { fixture };
