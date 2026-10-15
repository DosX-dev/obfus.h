'use strict';
const fs = require('node:fs');
const path = require('node:path');
const { macroDefinition } = require('./source_macros');

async function runSuite({ arch, compiler, directory, source, check, compile, execute, assert }) {
    await check(`${arch}/VM encodings, cache layouts, guard orders and vector transport`, async () => {
        const normalized = source.replace(/\r\n/g, '\n');
        const registerEnd = normalized.indexOf('} OBFH_V_REGISTER;');
        const begin = normalized.lastIndexOf('typedef struct', registerEnd);
        const end = normalized.indexOf('static inline unsigned int obfh_v_mix(', registerEnd);
        assert(begin >= 0 && end > begin, 'VM representation block missing');
        const block = normalized.slice(begin, end);
        const names = [...new Set([...block.matchAll(/^#define (\w+)/gm)].map((match) => match[1]))];
        const definition = name => macroDefinition(normalized, name, { last: true }).body;
        const maskBegin = normalized.indexOf('#if (OBFH_SEED >> 8) % 3u');
        const maskEnd = normalized.indexOf('static inline unsigned int obfh_v_flags(', maskBegin);
        const transportBegin = normalized.indexOf('#if (OBFH_SEED >> 10) & 1u');
        const transportEnd = normalized.indexOf('static inline OBFH_VM_VALUE obfh_vm_encode(', transportBegin);
        const mathBegin = normalized.indexOf('#define OBFH_MATH_ADDRESS_ENCODE(');
        const mathEnd = normalized.indexOf('#define _MUTATE_MATH(', mathBegin);
        assert(maskBegin >= 0 && transportBegin >= 0 && mathBegin >= 0, 'VM transport variants missing');
        let c = '#include <stdio.h>\n#include <string.h>\n#include <stddef.h>\n#include <stdint.h>\ntypedef int LONG;typedef uintptr_t ULONG_PTR;typedef void *PVOID;\n';
        for (const name of ['OBFH_MIX_A', 'OBFH_MIX_B', 'OBFH_MIX_FIELDS']) c += definition(name) + '\n';
        function structBlock(name) {
            const end = normalized.indexOf('} ' + name + ';') + name.length + 3;
            const begin = normalized.lastIndexOf('typedef struct {', end);
            assert(begin >= 0 && end > begin, 'missing cache layout: ' + name);
            return normalized.slice(begin, end);
        }
        const guardStart = normalized.indexOf('#if ((OBFH_SEED >> 16) & 7u) % 6u');
        const guardEnd = normalized.indexOf('#define OBFH_SF_GUARD_8 ', guardStart);
        assert(guardStart >= 0 && guardEnd > guardStart, 'missing guard permutations');
        for (let i = 0; i < 6; ++i) {
            const seed = (i << 16) | (i << 19) | ((i & 1) << 22);
            c += `#define OBFH_SEED ${seed}u\nstatic int cache_guard_${i}(void){\n`;
            c += structBlock('OBFH_CRT_ENTRY') + '\n' + structBlock('OBFH_GUI_SLOT') + '\n';
            c += 'if(sizeof(OBFH_CRT_ENTRY)!=(sizeof(void*)==8?48:40)||sizeof(OBFH_GUI_SLOT)!=(sizeof(void*)==8?16:8))return 1;\n';
            c += 'if(offsetof(OBFH_CRT_ENTRY,state)%4||offsetof(OBFH_CRT_ENTRY,address)%sizeof(void*)||offsetof(OBFH_GUI_SLOT,ready)%4||offsetof(OBFH_GUI_SLOT,encoded)%sizeof(void*))return 2;\n';
            c += normalized.slice(guardStart, guardEnd);
            c += 'unsigned value=0;for(unsigned n=0;n<4096;++n){value=value*1664525u+1013904223u;unsigned result;\n';
            c += '__asm__ __volatile__("movl %1,%%eax;movl %%eax,%%edx;" OBFH_SF_GUARD_8_ORDER "xorl %%edx,%%eax;movl %%eax,%0" : "=m"(result):"m"(value),[key_a]"i"(0x9e3779b9u):"eax","edx","cc");if(result!=0xffffffffu)return 3;}return 0;}\n#undef OBFH_SEED\n#undef OBFH_SF_GUARD_8_ORDER\n';
        }
        for (let selection = 0; selection < 216; ++selection) {
            const opcode = selection % 6, layout = Math.floor(selection / 6) % 4, word = Math.floor(selection / 24);
            const seed = (opcode << 24) | (layout << 4) | (word << 6);
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
            const encoders = ['op*m+offset', '(op*m)^offset', '(op^offset)*m', '(op+offset)*m', 'op*m-offset', '(op-offset)*m'];
            c += `expected=(${encoders[opcode]})&255u;\n`;
            c += 'if(OBFH_V_OPCODE_ENCODE(op,key)!=expected||OBFH_V_OPCODE_DECODE(expected,key,inverse)!=op)return 3;}}}\n';
            c += 'unsigned int value=0xffffffffu,key=0xfedcba98u;for(unsigned int pc=0;pc<128;++pc){\n';
            c += 'value=value*1664525u+1013904223u;unsigned int expected;\n';
            const masks = Array.from({ length: 9 }, (_, i) => `value${['^', '+', '-'][Math.floor(i / 3)]}(${['key+pc*0x9e3779b9u', 'key^(pc*0x9e3779b9u)', 'key-pc*0x9e3779b9u'][i % 3]})`);
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
        const crtBegin = normalized.indexOf('#if (OBFH_SEED % 3u) == 0u');
        const crtEnd = normalized.indexOf('static inline FARPROC obfh_crt_site(', crtBegin);
        assert(crtBegin >= 0 && crtEnd > crtBegin, 'CRT byte encodings missing');
        const transportSeeds = [0, 256, 512, 1024, 1280, 1536, 4096, 8192, 2097152, 4194304, 6291456];
        for (const seed of transportSeeds) {
            c += `#define OBFH_SEED ${seed}u\n`;
            c += normalized.slice(maskBegin, maskEnd) + normalized.slice(transportBegin, transportEnd) + normalized.slice(mathBegin, mathEnd) + normalized.slice(crtBegin, crtEnd);
            c += `static int transport_${seed}(void){\n`;
            c += 'for(unsigned int value=0;value<256;++value)for(unsigned int mask=0;mask<256;++mask){\n';
            c += `unsigned char crt=(unsigned char)(value${['^', '+', '-'][seed % 3]}mask);if(OBFH_CRT_BYTE_ENCODE(value,mask)!=crt||OBFH_CRT_BYTE_DECODE(crt,mask)!=value)return 5;\n`;
            c += 'unsigned char encoded=OBFH_V_VALUE_ENCODE(value,mask);if((unsigned char)OBFH_V_VALUE_DECODE(encoded,mask)!=value)return 1;\n';
            c += 'unsigned char wire=OBFH_VM_BYTE_ENCODE(value,mask,(mask*37u)&255u);if((unsigned char)OBFH_VM_BYTE_DECODE(wire,mask,(mask*37u)&255u)!=value)return 2;}\n';
            c += 'unsigned int values[5]={0,1,0x7fffffffu,0x80000000u,0xffffffffu};\n';
            c += 'for(unsigned int i=0;i<5;++i)for(unsigned int j=0;j<5;++j){unsigned int wire=OBFH_V_VALUE_ENCODE(values[i],values[j]);if(OBFH_V_VALUE_DECODE(wire,values[j])!=values[i])return 3;}\n';
            c += 'size_t pointers[5]={0,1,(size_t)-1,(size_t)-2,((size_t)-1)>>1};\n';
            for (let form = 0; form < 3; ++form)
                c += `{enum {__obfh_math_form=${form}};for(unsigned int i=0;i<5;++i)for(unsigned int j=0;j<5;++j){size_t wire=OBFH_MATH_ADDRESS_ENCODE(pointers[i],pointers[j]);if(wire!=(pointers[i]${['^', '+', '-'][form]}pointers[j])||OBFH_MATH_ADDRESS_DECODE(wire,pointers[j])!=pointers[i])return 4;}}\n`;
            const first = seed & 4194304 ? 5 : 13, last = seed & 4194304 ? 13 : 5, left = seed & 2097152 ? '>>' : '<<', right = seed & 2097152 ? '<<' : '>>';
            c += `for(unsigned int i=0;i<1000;++i){unsigned int actual=i*2654435761u,expected=actual;expected^=expected${left}${first};expected^=expected${right}17;expected^=expected${left}${last};OBFH_VM_STATE_STEP(actual);if(actual!=expected)return 6;}return 0;}\n`;
            c += ['OBFH_VM_STATE_FIRST', 'OBFH_VM_STATE_LAST', 'OBFH_VM_STATE_STEP', 'OBFH_CRT_BYTE_ENCODE', 'OBFH_CRT_BYTE_DECODE', 'OBFH_SEED', 'OBFH_V_VALUE_ENCODE', 'OBFH_V_VALUE_DECODE', 'OBFH_VM_BYTE_ENCODE', 'OBFH_VM_BYTE_DECODE', 'OBFH_MATH_ADDRESS_ENCODE', 'OBFH_MATH_ADDRESS_DECODE'].map((name) => '#undef ' + name).join('\n') + '\n';
        }
        const vectorBegin = normalized.indexOf('// All forms copy the same 128 bits');
        const vectorEnd = normalized.indexOf('#define memmove', vectorBegin);
        assert(vectorBegin >= 0 && vectorEnd > vectorBegin, 'missing vector forms');
        const xmmFixture = fs.readFileSync(path.join(__dirname, 'custom_polymorphism.c'), 'utf8');
        c += xmmFixture.slice(xmmFixture.indexOf('static void load_xmm'), xmmFixture.indexOf('#include "../include/obfus.h"'));
        const width = arch === 'x64' ? 'q' : 'l';
        c += `#define OBFH_MOVE_WIDTH "${width}"\n`;
        for (let mode = 0; mode < 3; ++mode) {
            c += `#define OBFH_SEED ${mode << 23}u\n` + normalized.slice(vectorBegin, vectorEnd);
            for (let reg = 0; reg < 8; ++reg) {
                c += `static void vector_${mode}_${reg}(unsigned char *d,unsigned char *s,size_t n){unsigned char scratch[16];d+=n-16;s+=n-16;n/=16;\n`;
                c += `__asm__ __volatile__(OBFH_MOVE_VECTOR("sub${width} $16,%0;sub${width} $16,%1;"):"+D"(d),"+S"(s),"+c"(n):"d"(scratch),"i"(${reg * 8}):"memory","cc");}\n`;
            }
            c += '#undef OBFH_SEED\n#undef OBFH_MOVE_VECTOR\n#undef OBFH_MOVE_VECTOR_LOAD\n#undef OBFH_MOVE_VECTOR_STORE\n';
        }
        c += 'static int vectors(void){unsigned char before[128],after[128],data[400],expected[400];\n';
        c += 'void(*copies[])(unsigned char*,unsigned char*,size_t)={' + Array.from({ length: 24 }, (_, i) => `vector_${Math.floor(i / 8)}_${i % 8}`).join(',') + '};\n';
        c += 'for(unsigned i=0;i<128;++i)before[i]=(unsigned char)(i*19+7);\n';
        c += 'for(unsigned f=0;f<24;++f)for(unsigned n=16;n<=256;n+=16)for(unsigned offset=1;offset<32;++offset){\n';
        c += 'for(unsigned i=0;i<400;++i)data[i]=expected[i]=(unsigned char)i;memmove(expected+offset,expected,n);load_xmm(before);copies[f](data+offset,data,n);store_xmm(after);if(memcmp(data,expected,400)||memcmp(before,after,128))return 1;}return 0;}\n';
        // Exercise the real helper bodies against an independent uint32 oracle.
        const helperBegin = normalized.indexOf('enum { OBFH_MIX_FIELDS(obfh_v_helper_draw');
        const helperEnd = normalized.indexOf('// Reverse traversal', helperBegin);
        assert(helperBegin >= 0 && helperEnd > helperBegin, 'VM helper bodies missing');
        const helperBlock = normalized.slice(helperBegin, helperEnd);
        const helperNames = ['OBFH_V_CONTEXT', 'obfh_v_helper_draw', 'obfh_v_helper_draw_mix',
            'obfh_v_mix', 'obfh_v_control_read', 'obfh_v_control_write', 'obfh_v_flags', 'obfh_v_set_flags'];
        const helperMacros = [...new Set([...helperBlock.matchAll(/^#define (\w+)/gm)].map(match => match[1]))];
        c += '#define PHANTOM_NOP ((void)0)\n';
        for (let seed = 0; seed < 32; ++seed) {
            let draw = (seed ^ 0x6d2b79f5) >>> 0;
            draw = Math.imul(draw ^ (draw >>> 16), 2246822507) >>> 0;
            draw = Math.imul(draw ^ (draw >>> 13), 3266489909) >>> 0;
            c += `#define OBFH_SEED ${seed}u\n`;
            for (const name of helperNames) c += `#define ${name} ${name}_${seed}\n`;
            c += 'typedef struct {unsigned controls[4],key,flags,flag_key;} OBFH_V_CONTEXT;\n' + helperBlock;
            c += `static int helpers_${seed}(void){OBFH_V_CONTEXT c={0};unsigned x=0;\n`;
            c += `if(obfh_v_helper_draw!=${draw}u)return 1;\n`;
            c += 'for(unsigned i=0;i<4096;++i){unsigned y;x=x*1664525u+1013904223u;y=x;\n';
            c += `y^=y${draw & 64 ? '<<' : '>>'}${11 + (draw & 7)};y*=${[2246822507, 3266489909, 2221713035][draw % 3]}u;y^=y${(draw & 192) === 128 ? '<<' : '>>'}${12 + ((draw >>> 3) & 7)};if(obfh_v_mix(x)!=y)return 2;\n`;
            c += `c.key=x;c.flag_key=~x;obfh_v_set_flags(&c,i);if(c.flags!=(i${['^', '+', '-'][draw % 3]}c.flag_key)||obfh_v_flags(&c)!=i)return 3;\n`;
            c += `unsigned r=i%4,mask=c.key${draw & 512 ? '^' : '+'}(r*3266489917u);\n`;
            c += `mask^=mask${draw & 64 ? '<<' : '>>'}${11 + (draw & 7)};mask*=${[2246822507, 3266489909, 2221713035][draw % 3]}u;mask^=mask${(draw & 192) === 128 ? '<<' : '>>'}${12 + ((draw >>> 3) & 7)};\n`;
            c += 'obfh_v_control_write(&c,r,x);if(c.controls[r]!=(x^mask)||obfh_v_control_read(&c,r)!=x)return 4;c.controls[r]^=0x87654321u;if(obfh_v_control_read(&c,r)!=(x^0x87654321u))return 5;}return 0;}\n';
            for (const name of ['OBFH_SEED', ...helperNames, ...helperMacros]) c += '#undef ' + name + '\n';
        }
        c += '#undef PHANTOM_NOP\n';
        c += 'int main(void){\n';
        for (let seed = 0; seed < 32; ++seed) c += `if(helpers_${seed}())return 106;\n`;

        c += 'if(vectors())return 105;\n';
        for (let i = 0; i < 6; ++i) c += `if(cache_guard_${i}())return 104;\n`;
        for (let selection = 0; selection < 216; ++selection) c += `if(probe_${selection}())return ${selection + 1};\n`;
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
