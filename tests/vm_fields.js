'use strict';
const fs = require('node:fs');
const path = require('node:path');

async function runSuite({ arch, compiler, directory, source, check, compile, execute, assert }) {
    await check(`${arch}/VM field decoders: all 24 orders and exact field values`, async () => {
        function definition(name) {
            const start = source.lastIndexOf('#define ' + name);
            assert(start >= 0, 'missing macro: ' + name);
            const lines = source.slice(start).split('\n');
            let count = 1;
            while (lines[count - 1].trimEnd().endsWith('\\')) ++count;
            return lines.slice(0, count).join('\n');
        }
        const begin = source.indexOf('#define OBFH_VM_FIELDS_0(');
        const end = source.indexOf('static long double Obfh_VirtualMachine(', begin);
        assert(begin >= 0 && end > begin, 'missing field layout block');
        const block = source.slice(begin, end);
        const names = [...new Set([...block.matchAll(/^#define (\w+)/gm)].map((m) => m[1]))];
        const permutations = [[0, 1, 2], [0, 2, 1], [1, 0, 2], [1, 2, 0], [2, 0, 1], [2, 1, 0]];
        const seeds = new Map();
        for (let seed = 0; seed < 4096 && seeds.size < 24; ++seed) {
            const draw = Number((BigInt(seed ^ 0x564d4644) * 2654435761n) & 0xffffffffn);
            seeds.set(draw % 24, seed);
        }
        assert(seeds.size === 24, 'not all decoder permutations reachable');
        let c = '#include <stdio.h>\n#include <stdint.h>\n#define __obfh_asm__(...) __asm__ __volatile__(__VA_ARGS__)\n';
        for (const name of ['OBFH_MIX_A', 'OBFH_MIX_B', 'OBFH_PHANTOM_CAPTURE', 'OBFH_PHANTOM_NOP_AT']) c += definition(name) + '\n';
        for (const [selection, seed] of seeds) {
            const first = selection % 4;
            const rest = [1, 2, 3].map((v) => (first + v) % 4);
            const order = [first, ...permutations[Math.floor(selection / 4)].map((v) => rest[v])];
            c += `#undef OBFH_BUILD_SEED\n#define OBFH_BUILD_SEED ${seed}u\n` + block;
            c += `static int probe_${selection}(unsigned int word, unsigned int layout, unsigned int *result) {\n`;
            c += 'unsigned int op,d,a,b,imm; void *labels[4]={&&fields_0,&&fields_1,&&fields_2,&&fields_3};\n';
            for (let i = 0; i < 3; ++i) c += `if ((uintptr_t)labels[${order[i]}] >= (uintptr_t)labels[${order[i + 1]}]) return 1;\n`;
            c += 'goto *labels[layout]; OBFH_VM_FIELDS_LAYOUT\nfields_ready: result[0]=op;result[1]=d;result[2]=a;result[3]=b;result[4]=imm;return 0;}\n';
            c += names.map((name) => '#undef ' + name).join('\n') + '\n';
        }
        c += 'int main(void) { unsigned int word=1u,r[5],e[5];\n';
        // Independent bit-position oracle; does not reuse production decoder expressions.
        c += 'const unsigned int shifts[4][5]={{0,8,10,12,14},{24,18,20,22,0},{6,0,2,4,14},{0,12,8,10,14}};\n';
        c += 'for(unsigned int n=0;n<4096;++n){word=word*1664525u+1013904223u;for(unsigned int l=0;l<4;++l){\n';
        c += 'for(unsigned int j=0;j<5;++j){e[j]=word>>shifts[l][j];if(j>0&&j<4)e[j]&=3u;}if(l==1)e[4]&=0x3ffffu;\n';
        for (const selection of seeds.keys()) c += `if(probe_${selection}(word,l,r))return 2;for(unsigned int j=0;j<5;++j)if(r[j]!=e[j])return 3;\n`;
        c += '}}puts("VM_FIELDS_PASS");return 0;}\n';
        const file = path.join(directory, arch + '-vm-fields.c');
        fs.writeFileSync(file, c);
        await execute(await compile(compiler, directory, arch + '-vm-fields.exe', file, []), 'VM_FIELDS_PASS');
    });
}

module.exports = { runSuite };
