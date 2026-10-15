'use strict';
const fs = require('node:fs');
const path = require('node:path');
exports.aliases = function (source, assert) {
    const start = source.indexOf('#define OBFH_SF_ASM_BITS');
    assert(start >= 0, 'proxy bit aliases missing');
    const lines = source.slice(start).split(/\r?\n/);
    const definition = [];
    for (const line of lines) {
        definition.push(line);
        if (!line.trimEnd().endsWith('\\')) break;
    }
    return definition.join('\n') + '\n';
};
exports.fixture = function ({ arch, source, assert, sharedHelpers = false }) {
    const start = source.indexOf('#define OBFH_SF_BYTES'), end = source.indexOf('#define OBFH_SF_CALL(target)', start);
    assert(start >= 0 && end > start, 'frame fragment boundaries missing');
    let text = '#include <stdint.h>\n#include <stdio.h>\n' +
        (sharedHelpers ? '' : source.slice(start, end) + exports.aliases(source, assert)) + '\n';
    text += `typedef unsigned (*Entry)(void);
#if defined(__x86_64__)
#define ADDRESS "leaq 1f(%%rip), %0; jmp 9f; 1:"
#define SP(x) __asm__ __volatile__("movq %%rsp,%0":"=r"(x)::"memory")
#else
#define ADDRESS "leal 1f,%0; jmp 9f; 1:"
#define SP(x) __asm__ __volatile__("movl %%esp,%0":"=r"(x)::"memory")
#endif
#define CARRIER(name,size,draw,frame,epilogue) static Entry name(void){Entry result; __asm__ __volatile__(OBFH_SF_ASM_BITS ADDRESS frame("test") "movl $123,%%eax;" epilogue "9:":"=r"(result):[test]"i"(size),[phantom]"i"(draw):"eax","ecx","edx","cc","memory");return result;}
`;
    const entries = [];
    for (let size = 32; size <= 192; size += 16) for (let mask = 0; mask < 16; mask++) {
        const phantom = ((mask & 1) << (1 + ((size >> 4) & 3))) | (((mask >> 1) & 1) << (4 + ((size >> 4) & 3))) | (((mask >> 2) & 1) << 6) | (((mask >> 3) & 1) << 7);
        for (const frame of ['OBFH_SF_FRAME', 'OBFH_SF_FRAME_ALT']) for (const epilogue of ['OBFH_SF_EPILOGUE', 'OBFH_SF_EPILOGUE_ALT']) {
            const name = 'entry_' + entries.length;
            entries.push(name + '()');
            text += `CARRIER(${name},${size},${phantom},${frame},${epilogue})\n`;
        }
    }
    text += `int main(void){Entry entries[]={${entries.join(',')}};volatile unsigned canary=0xabcdef01u;for(unsigned i=0;i<sizeof(entries)/sizeof(entries[0]);i++){uintptr_t before,after;SP(before);unsigned value=entries[i]();SP(after);if(value!=123||before!=after||canary!=0xabcdef01u){fprintf(stderr,"stack frames: case %u, value %u, stack delta %lld\\n",i,value,(long long)(after-before));return 1;}}if(puts("STACK_FRAMES_PASS")==EOF||fflush(stdout))return 2;return 0;}\n`;
    return text;
};
exports.runSuite = async function ({ arch, compiler, directory, source, check, compile, execute, assert }) {
    await check(`${arch}/stack frames: every size and prologue/epilogue selector`, async () => {
        const text = exports.fixture({ arch, source, assert });
        const file = path.join(directory, arch + '-stack-frames.c');
        fs.writeFileSync(file, text);
        await execute(await compile(compiler, directory, arch + '-stack-frames.exe', file, []), 'STACK_FRAMES_PASS');
    });
};
