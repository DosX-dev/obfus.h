'use strict';
const fs = require('node:fs');
const path = require('node:path');

async function runSuite({ arch, compiler, directory, source, check, compile, execute, assert }) {
    await check(`${arch}/VM encodings and state: all 64 combinations`, async () => {
        const normalized = source.replace(/\r\n/g, '\n');
        const registerEnd = normalized.indexOf('} OBFH_V_REGISTER;');
        const begin = normalized.lastIndexOf('typedef struct', registerEnd);
        const end = normalized.indexOf('static unsigned int obfh_v_mix(', registerEnd);
        assert(begin >= 0 && end > begin, 'VM representation block missing');
        const block = normalized.slice(begin, end);
        const names = [...new Set([...block.matchAll(/^#define (\w+)/gm)].map((match) => match[1]))];
        let c = '#include <stdio.h>\n#include <stddef.h>\n';
        for (let selection = 0; selection < 64; ++selection) {
            const opcode = selection & 3, layout = (selection >> 2) & 3, word = selection >> 4;
            const seed = (opcode << 2) | (layout << 4) | (word << 6);
            c += `#define OBFH_BUILD_SEED ${seed}u\nstatic int probe_${selection}(void) {\n` + block;
            // Offsets must genuinely change, without increasing the context's storage.
            const orders = [
                ['registers', 'controls', 'flags', 'result'],
                ['result', 'key', 'controls', 'registers'],
                ['controls', 'pc', 'result', 'registers'],
                ['status', 'registers', 'result', 'controls']
            ];
            for (let i = 1; i < orders[layout].length; ++i) {
                c += `if(offsetof(OBFH_V_CONTEXT,${orders[layout][i - 1]})>=offsetof(OBFH_V_CONTEXT,${orders[layout][i]}))return 1;\n`;
            }
            c += 'if(sizeof(OBFH_V_CONTEXT)!=4*sizeof(OBFH_V_REGISTER)+12*sizeof(unsigned int)+sizeof(long double))return 2;\n';
            c += 'for(unsigned int m=1;m<256;m+=2){unsigned int inverse=1;while((m*inverse&255u)!=1u)inverse+=2;\n';
            c += 'for(unsigned int offset=0;offset<256;offset+=51){unsigned int key=(m<<8)|offset;\n';
            c += 'for(unsigned int op=0;op<256;++op){unsigned int expected;\n';
            const encoders = ['op*m+offset', '(op*m)^offset', '(op^offset)*m', '(op+offset)*m'];
            c += `expected=(${encoders[opcode]})&255u;\n`;
            c += 'if(OBFH_V_OPCODE_ENCODE(op,key)!=expected||OBFH_V_OPCODE_DECODE(expected,key,inverse)!=op)return 3;}}}\n';
            c += 'unsigned int value=0xffffffffu,key=0xfedcba98u;for(unsigned int pc=0;pc<128;++pc){\n';
            c += 'value=value*1664525u+1013904223u;unsigned int expected;\n';
            const masks = [
                'value^(key+pc*0x9e3779b9u)',
                'value+(key^(pc*0x9e3779b9u))',
                'value^(key-pc*0x9e3779b9u)',
                'value-(key+pc*0x9e3779b9u)'
            ];
            c += `expected=${masks[word]};\n`;
            c += 'if(OBFH_V_WORD_ENCODE(value,key,pc)!=expected||OBFH_V_WORD_DECODE(expected,key,pc)!=value)return 4;}return 0;}\n';
            c += ['OBFH_BUILD_SEED', ...names].map((name) => '#undef ' + name).join('\n') + '\n';
        }
        c += 'int main(void){\n';
        for (let selection = 0; selection < 64; ++selection) c += `if(probe_${selection}())return ${selection + 1};\n`;
        c += 'puts("VM_VARIANTS_PASS");return 0;}\n';
        const file = path.join(directory, arch + '-vm-variants.c');
        fs.writeFileSync(file, c);
        await execute(await compile(compiler, directory, arch + '-vm-variants.exe', file, []), 'VM_VARIANTS_PASS');
    });
}

module.exports = { runSuite };
