'use strict';
const fs = require('node:fs');
const path = require('node:path');

function fixture(header) {
    const start = header.indexOf('// BEGIN SHORT ASM FORMS');
    const end = header.indexOf('// END SHORT ASM FORMS');
    if (start < 0 || end < start) throw new Error('Short ASM forms missing');
    let code = '#include <stdio.h>\n' + header.slice(start, end) + '\n';
    const tests = [];
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
