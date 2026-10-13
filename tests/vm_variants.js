'use strict';
const fs = require('node:fs');
const path = require('node:path');

async function runSuite({ arch, compiler, directory, source, check, compile, execute, assert }) {
    await check(`${arch}/VM encodings, 24 register orders and byte transport`, async () => {
        const normalized = source.replace(/\r\n/g, '\n');
        const registerEnd = normalized.indexOf('} OBFH_V_REGISTER;');
        const begin = normalized.lastIndexOf('typedef struct', registerEnd);
        const end = normalized.indexOf('static unsigned int obfh_v_mix(', registerEnd);
        assert(begin >= 0 && end > begin, 'VM representation block missing');
        const block = normalized.slice(begin, end);
        const names = [...new Set([...block.matchAll(/^#define (\w+)/gm)].map((match) => match[1]))];
        function definition(name) {
            const start = normalized.lastIndexOf('#define ' + name);
            assert(start >= 0, 'missing macro: ' + name);
            const lines = normalized.slice(start).split('\n');
            let count = 1;
            while (lines[count - 1].trimEnd().endsWith('\\')) ++count;
            return lines.slice(0, count).join('\n');
        }
        const maskBegin = normalized.indexOf('#if (OBFH_SEED >> 8) % 3u');
        const maskEnd = normalized.indexOf('static unsigned int obfh_v_flags(', maskBegin);
        const transportBegin = normalized.indexOf('#if (OBFH_SEED >> 10) & 1u');
        const transportEnd = normalized.indexOf('static OBFH_VM_VALUE obfh_vm_encode(', transportBegin);
        const mathBegin = normalized.indexOf('#if (OBFH_SEED >> 12) % 3u');
        const mathEnd = normalized.indexOf('#define _MUTATE_MATH(', mathBegin);
        assert(maskBegin >= 0 && transportBegin >= 0 && mathBegin >= 0, 'VM transport variants missing');
        let c = '#include <stdio.h>\n#include <stddef.h>\n';
        for (let selection = 0; selection < 64; ++selection) {
            const opcode = selection & 3, layout = (selection >> 2) & 3, word = selection >> 4;
            const seed = (opcode << 2) | (layout << 4) | (word << 6);
            c += `#define OBFH_SEED ${seed}u\nstatic int probe_${selection}(void) {\n` + block;
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
            c += ['OBFH_SEED', ...names].map((name) => '#undef ' + name).join('\n') + '\n';
        }
        // Exercise the production enum draws, without compiling 256 whole headers.
        for (const name of ['RND', 'OBFH_JUNK_WORD', 'OBFH_MIX_A']) c += definition(name) + '\n';
        const execBegin = normalized.indexOf('#define OBFH_VM_EXEC(');
        const enumBegin = normalized.indexOf('enum {', execBegin);
        const enumEnd = normalized.indexOf('};', enumBegin) + 2;
        const drawEnum = normalized.slice(enumBegin, enumEnd).replace(/\\\n/g, '\n').replace('(operation)', '0');
        c += 'static unsigned char roles[256],programs[8];\n';
        for (let seed = 0; seed < 256; ++seed) {
            c += `#define OBFH_SEED ${seed}u\nstatic int roles_${seed}(void){\n${drawEnum}\n`;
            c += 'if((1u<<__obfh_va | 1u<<__obfh_vb | 1u<<__obfh_vc | 1u<<__obfh_vd)!=15u)return 1;\n';
            c += 'roles[(__obfh_va<<6)|(__obfh_vb<<4)|(__obfh_vc<<2)|__obfh_vd]=1;programs[__obfh_vvariant]=1;return 0;}\n#undef OBFH_SEED\n';
        }
        // Full byte domain and mask domain, plus uint32/pointer wrap boundaries.
        const transportSeeds = [0, 256, 512, 1024, 1280, 1536, 4096, 8192];
        for (const seed of transportSeeds) {
            c += `#define OBFH_SEED ${seed}u\n`;
            c += normalized.slice(maskBegin, maskEnd) + normalized.slice(transportBegin, transportEnd) + normalized.slice(mathBegin, mathEnd);
            c += `static int transport_${seed}(void){\n`;
            c += 'for(unsigned int value=0;value<256;++value)for(unsigned int mask=0;mask<256;++mask){\n';
            c += 'unsigned char encoded=OBFH_V_VALUE_ENCODE(value,mask);if((unsigned char)OBFH_V_VALUE_DECODE(encoded,mask)!=value)return 1;\n';
            c += 'unsigned char wire=OBFH_VM_BYTE_ENCODE(value,mask,(mask*37u)&255u);if((unsigned char)OBFH_VM_BYTE_DECODE(wire,mask,(mask*37u)&255u)!=value)return 2;}\n';
            c += 'unsigned int values[5]={0,1,0x7fffffffu,0x80000000u,0xffffffffu};\n';
            c += 'for(unsigned int i=0;i<5;++i)for(unsigned int j=0;j<5;++j){unsigned int wire=OBFH_V_VALUE_ENCODE(values[i],values[j]);if(OBFH_V_VALUE_DECODE(wire,values[j])!=values[i])return 3;}\n';
            c += 'size_t pointers[5]={0,1,(size_t)-1,(size_t)-2,((size_t)-1)>>1};\n';
            c += 'for(unsigned int i=0;i<5;++i)for(unsigned int j=0;j<5;++j){size_t wire=OBFH_MATH_ADDRESS_ENCODE(pointers[i],pointers[j]);if(OBFH_MATH_ADDRESS_DECODE(wire,pointers[j])!=pointers[i])return 4;}return 0;}\n';
            c += ['OBFH_SEED', 'OBFH_V_VALUE_ENCODE', 'OBFH_V_VALUE_DECODE', 'OBFH_VM_BYTE_ENCODE', 'OBFH_VM_BYTE_DECODE', 'OBFH_MATH_ADDRESS_ENCODE', 'OBFH_MATH_ADDRESS_DECODE'].map((name) => '#undef ' + name).join('\n') + '\n';
        }
        c += 'int main(void){\n';
        for (let selection = 0; selection < 64; ++selection) c += `if(probe_${selection}())return ${selection + 1};\n`;
        for (let seed = 0; seed < 256; ++seed) c += `if(roles_${seed}())return 100;\n`;
        c += 'unsigned int count=0;for(unsigned int i=0;i<256;++i)count+=roles[i];if(count!=24)return 101;\n';
        c += 'for(unsigned int i=0;i<8;++i)if(!programs[i])return 102;\n';
        for (const seed of transportSeeds) c += `if(transport_${seed}())return 103;\n`;
        c += 'puts("VM_VARIANTS_PASS");return 0;}\n';
        const file = path.join(directory, arch + '-vm-variants.c');
        fs.writeFileSync(file, c);
        await execute(await compile(compiler, directory, arch + '-vm-variants.exe', file, []), 'VM_VARIANTS_PASS');
    });
}

module.exports = { runSuite };
