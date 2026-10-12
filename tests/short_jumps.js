'use strict';
const fs = require('node:fs');
const path = require('node:path');
exports.fixture = function ({ arch, source, assert, sharedHelpers = false }) {
    const first = source.indexOf('#define OBFH_SF_BYTES'), last = source.indexOf('// BEGIN COMPACT ASM CACHE', first);
    assert(first >= 0 && last > first, 'proxy fragments missing');
    let helpers = source.slice(first, last);
    const original = helpers.replace('.byte 0xeb,4; 6: negl', 'jmp 7f; 6: negl')
        .replace('.byte 0xeb,(4 + ((%c[rotate] != 1) & 1)); 6:', 'jmp 7f; 6:')
        .replace('.byte 0xeb,2; 6: negl', 'jmp 7f; 6: negl').replace('.byte 0xeb,8; 6:', 'jmp 7f; 6:');
    assert(original !== helpers && !original.includes('.byte 0xeb,'), 'short jump reference boundaries changed');
    const bodies = [];
    for (const name of ['BODY_K', 'LEAF_C', 'LEAF_J', 'LEAF_L']) {
        const regex = new RegExp('^#define OBFH_SF_' + name + ' [^\\n]*(?:\\n(?=[ \t])[^\\n]*)*', 'm');
        const old = original.match(regex);
        assert(old, 'missing reference body ' + name);
        bodies.push(old[0].replace('OBFH_SF_' + name, 'OLD_' + name));
    }
    let c = '#include <stdio.h>\n#include <stdint.h>\n' + (sharedHelpers ? '' : helpers) + '\n' + bodies.join('\n') + '\n';
    c += `typedef unsigned (*Entry)(unsigned,unsigned);
#if defined(__x86_64__)
#define ADDRESS "leaq 1f(%%rip),%0;jmp 9f;1:"
#define SP(x) __asm__ __volatile__("movq %%rsp,%0":"=r"(x)::"memory")
#else
#define ADDRESS "leal 1f,%0;jmp 9f;1:"
#define SP(x) __asm__ __volatile__("movl %%esp,%0":"=r"(x)::"memory")
#endif
#define CARRIER(name,body,count,shift,draw) static Entry name(void){Entry result;__asm__ __volatile__(ADDRESS body "9:":"=r"(result):[phantom]"i"(draw),[rotate]"i"(count),[leaf_shift]"i"(shift),[key_a]"i"(0xfeed1234u),[key_b]"i"(0x9abcdef1u),[leaf_alu_a]"i"(0x35),[leaf_alu_b]"i"(0xf2),[leaf_frame]"i"(40),[leaf_slot]"i"(16),[local_a]"i"(8),[local_b]"i"(24),[frame_a]"i"(64):"eax","ecx","edx","cc","memory");return result;}
`;
    const pairs = [];
    for (let count = 1; count <= 31; count++) for (const shift of [0xc0, 0xc8, 0xe0, 0xe8, 0xf8]) for (const draw of [0, 0xffff]) for (const name of ['BODY_K', 'LEAF_C', 'LEAF_J', 'LEAF_L']) {
        const id = pairs.length;
        for (const old of [false, true]) {
            const body = (old ? 'OLD_' : 'OBFH_SF_') + name;
            c += `CARRIER(entry_${id}_${old ? 0 : 1},${name === 'BODY_K' ? 'OBFH_SF_FRAME("frame_a") ' + body + ' OBFH_SF_EPILOGUE' : body},${count},${shift},${draw})\n`;
        }
        pairs.push(`{entry_${id}_0(),entry_${id}_1()}`);
    }
    c += `int main(void){Entry pairs[][2]={${pairs.join(',')}};unsigned values[]={0,1,0x7fffffffu,0x80000000u,~0u,0x12345678u};volatile unsigned canary=0xabcdef01u;for(unsigned i=0;i<sizeof(pairs)/sizeof(pairs[0]);i++)for(unsigned a=0;a<6;a++)for(unsigned b=0;b<6;b++){uintptr_t before,after;SP(before);unsigned x=pairs[i][0](values[a],values[b]);unsigned y=pairs[i][1](values[a],values[b]);SP(after);if(x!=y||before!=after||canary!=0xabcdef01u){fprintf(stderr,"short jumps: pair %u, inputs %u/%u, values %u/%u, stack delta %lld\\n",i,a,b,x,y,(long long)(after-before));return 1;}}if(puts("SHORT_JUMPS_PASS")==EOF||fflush(stdout))return 2;return 0;}\n`;
    return c;
};
exports.runSuite = async function ({ arch, compiler, directory, source, check, compile, execute, assert }) {
    await check(`${arch}/short proxy jumps: targets, all shift counts and native stack`, async () => {
        const c = exports.fixture({ arch, source, assert });
        const file = path.join(directory, arch + '-short-jumps.c');
        fs.writeFileSync(file, c);
        await execute(await compile(compiler, directory, arch + '-short-jumps.exe', file, []), 'SHORT_JUMPS_PASS');
    });
};
