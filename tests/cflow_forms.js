'use strict';
const fs = require('node:fs');
const path = require('node:path');

function fixture(header) {
    const start = header.indexOf('// BEGIN SHORT ASM FORMS');
    const end = header.indexOf('// END SHORT ASM FORMS');
    if (start < 0 || end < start) throw new Error('Short ASM forms missing');
    let code = '#include <stdio.h>\n' + header.slice(start, end) + '\n';
    const tests = [];
    // A mask below 128 gives identical defined flags at byte and dword width.
    // In particular neither result can set SF; the operand itself is unchanged.
    const shortTests = [...new Set([...header.matchAll(/testb \$(\d+), %%(al|bl|cl|dl);/g)].map(match => match[1] + ':' + match[2]))];
    if (!shortTests.length) throw new Error('short TEST sites missing');
    for (const item of shortTests) {
        const [mask, low] = item.split(':');
        if (Number(mask) >= 128) throw new Error('short TEST changes the sign flag');
        const reg = { al: 'eax', bl: 'ebx', cl: 'ecx', dl: 'edx' }[low];
        const id = tests.length;
        code += `static int probe_${id}(unsigned x){size_t a,b;unsigned y;\n`;
        code += `__asm__ __volatile__("movl %[x],%%${reg};testl $${mask},%%${reg};pushf;pop %0":"=r"(a):[x]"m"(x):"${reg}","cc","memory");\n`;
        code += `__asm__ __volatile__("movl %[x],%%${reg};testb $${mask},%%${low};pushf;pop %0;movl %%${reg},%1":"=&r"(b),"=m"(y):[x]"m"(x):"${reg}","cc","memory");\n`;
        code += 'return y==x&&((a^b)&0x8c5u)==0;}\n';
        tests.push(`probe_${id}(values[i])`);
    }
    const registers = ['eax', 'ecx', 'edx'];
    for (let register = 0; register < 3; ++register) {
        for (let form = 0; form < 3; ++form) {
            const id = tests.length;
            code +=
                `static int probe_${id}(unsigned x) { unsigned y;\n` +
                `__asm__ __volatile__("movl %[input], %%${registers[register]};"\n` +
                `OBFH_ASM_ZERO32("${register}", "${form}")\n` +
                `"movl %%${registers[register]}, %[output];" : [output] "=m"(y) : [input] "m"(x) : "eax", "ecx", "edx", "cc"); return y == 0; }\n`;
            tests.push(`probe_${id}(values[i])`);
        }
    }
    for (let register = 0; register < 3; ++register) {
        for (let rotate = 1; rotate < 32; ++rotate) {
            for (let variant = 0; variant < 16; ++variant) {
                const family = variant >> 2;
                const id = tests.length,
                    key = (variant & 1) * 4096 + ((variant >> 1) & 1) * 8192 + family * 131072;
                code +=
                    `static int probe_${id}(unsigned x) { unsigned y, packed;\n` +
                    `OBFH_INPUT_CORE(in,0x8000ffffu,${rotate},${key}); OBFH_INPUT_EAX(in,0x8000ffffu);\n` +
                    'OBFH_INPUT_OTHER(in,0x8000ffffu,1); OBFH_INPUT_OTHER(in,0x8000ffffu,2);\n' +
                    `__asm__ __volatile__("movl %[input], %%${registers[register]}; .long %c[low]; .fill 1,4-${register ? 0 : 1}*(8-%c[length]),%c[high]; .fill ${register ? 1 : 0}*(%c[length]-7),1,%c[tail]; movl %%${registers[register]}, %[output];"\n` +
                    `: [output] "=m"(packed) : [input] "m"(x), [low] "i"(in_${register}a), [high] "i"(in_${register}b), [length] "i"(in_n), [tail] "i"(in_o ? (unsigned)in_x >> 24 : (unsigned)in_r >> 16) : "eax", "ecx", "edx", "cc");\n` +
                    `__asm__ __volatile__("movl %[input], %%${registers[register]};"\n` +
                    `OBFH_ASM_XOR_ROTATE("${register}", "0x8000ffff", "${rotate}", "${key}")\n` +
                    `"movl %%${registers[register]}, %[output];" : [output] "=m"(y) : [input] "m"(x) : "eax", "ecx", "edx", "cc");\n` +
                    `unsigned z=x ${family === 1 ? '+' : family === 2 ? '-' : '^'} 0x8000ffffu;\n` +
                    (family === 3
                        ? 'z=(z<<24)|((z&0xff00u)<<8)|((z>>8)&0xff00u)|(z>>24);\n'
                        : `z=(z<<${rotate})|(z>>${32 - rotate});\n`) +
                    `x ^= 0x8000ffffu; return packed == z && y == ((x << ${rotate}) | (x >> ${32 - rotate})); }\n`;
                tests.push(`probe_${id}(values[i])`);
            }
        }
    }
    for (let rotate = 1; rotate < 32; ++rotate) {
        for (let variant = 0; variant < 8; ++variant) {
            const id = tests.length,
                key = variant * 16384;
            code +=
                `static int probe_${id}(unsigned x) { unsigned y, same;\n` +
                '__asm__ __volatile__("movl %[input], %%eax;"\n' +
                `OBFH_ASM_MOV32("2", "0", "${key}", "15")\n` +
                `OBFH_ASM_ROTATE_PAIR("0", "${rotate}", "${key}", "0")\n` +
                `OBFH_ASM_ROTATE_PAIR("0", "${rotate}", "${key}", "1")\n` +
                `OBFH_ASM_EQ32("0", "2", "${key}", "16")\n` +
                '"sete %%cl; movzbl %%cl, %%ecx; movl %%eax, %[output]; movl %%ecx, %[same];"\n' +
                ': [output] "=m"(y), [same] "=m"(same) : [input] "m"(x) : "eax", "ecx", "edx", "cc"); return y == x && same == 1; }\n';
            tests.push(`probe_${id}(values[i])`);
        }
    }
    // Unequal operands catch comparisons that accidentally compare a register to itself.
    for (let reverse = 0; reverse < 2; ++reverse) {
        const id = tests.length;
        code +=
            `static int probe_${id}(unsigned x) { unsigned same;\n` +
            '__asm__ __volatile__("movl %[input], %%eax; movl %%eax, %%edx; notl %%edx;"\n' +
            `OBFH_ASM_EQ32("0", "2", "${reverse * 65536}", "16")\n` +
            '"sete %%cl; movzbl %%cl, %%ecx; movl %%ecx, %[same];" : [same] "=m"(same) : [input] "m"(x) : "eax", "ecx", "edx", "cc"); return same == 0; }\n';
        tests.push(`probe_${id}(values[i])`);
    }
    // Check both encodings against native operands and all defined status flags.
    // In particular, unequal CMP/SUB operands detect accidental operand reversal.
    const operations = [['addl', '0x01', '+'], ['orl', '0x09', '|'], ['andl', '0x21', '&'],
    ['subl', '0x29', '-'], ['xorl', '0x31', '^'], ['cmpl', '0x39', null]];
    code += '#ifdef __x86_64__\ntypedef unsigned long long flag_word;\n#define SAVE_FLAGS "pushfq; popq %[flags];"\n#else\ntypedef unsigned flag_word;\n#define SAVE_FLAGS "pushfl; popl %[flags];"\n#endif\n';
    for (const [operation, opcode, symbol] of operations) {
        for (let dst = 0; dst < 3; ++dst) {
            for (let src = 0; src < 3; ++src) {
                for (let direction = 0; direction < 2; ++direction) {
                    const id = tests.length;
                    const setup = `movl %[x], %%${registers[dst]}; movl %[x], %%${registers[src]};`;
                    code += `static int probe_${id}(unsigned x) { unsigned a,b; flag_word fa,fb;\n`;
                    const operands = ': [output] "=m"(a), [flags] "=m"(fa) : [x] "m"(x) : "eax", "ecx", "edx", "cc", "memory");\n';
                    // For distinct registers choose unequal values, including signed boundary pairs.
                    const load = dst === src ? setup : `movl %[x], %%${registers[dst]}; movl %[x], %%${registers[src]}; notl %%${registers[src]};`;
                    code += `__asm__ __volatile__("${load}${operation} %%${registers[src]}, %%${registers[dst]};" SAVE_FLAGS "movl %%${registers[dst]}, %[output];" ${operands}`;
                    code += `__asm__ __volatile__("${load}" OBFH_ASM_BINARY32("${opcode}", "${dst}", "${src}", "${direction}", "0") SAVE_FLAGS "movl %%${registers[dst]}, %[output];" ${operands.replace('(a)', '(b)').replace('(fa)', '(fb)')}`;
                    const rhs = dst === src ? 'x' : '~x';
                    const flagMask = ['addl', 'subl', 'cmpl'].includes(operation) ? '0x8d5u' : '0x8c5u';
                    code += `return a == b && b == (${symbol ? `x ${symbol} (${rhs})` : 'x'}) && ((fa ^ fb) & ${flagMask}) == 0; }\n`;
                    tests.push(`probe_${id}(values[i])`);
                }
            }
        }
    }
    code += 'int main(void) { unsigned values[263]={0,1,2,255,0x7fffffffu,0x80000000u,0xffffffffu};\n';
    code += 'for(unsigned i=7;i<263;++i) { unsigned x=i*0x9e3779b9u; values[i]=(x^(x>>16))*0x85ebca6bu; }\n';
    code += 'for(unsigned i=0;i<sizeof(values)/sizeof(values[0]);++i) {\n';
    tests.forEach((test, index) => {
        code += `if(!${test}) { printf("FORMS_FAIL ${index} %u\\n",i);return 1; }\n`;
    });
    return code + '} puts("CFLOW_FORMS_PASS");return 0;}\n';
}

async function runSuite({ arch, compiler, directory, source, check, compile, execute }) {
    await check(arch + '/CFLOW short ASM equivalent forms', async () => {
        const file = path.join(directory, arch + '-forms.c');
        fs.writeFileSync(file, fixture(source));
        await execute(await compile(compiler, directory, arch + '-forms.exe', file, []), 'CFLOW_FORMS_PASS');
    });
}
module.exports = { fixture, runSuite };
