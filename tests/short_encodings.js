'use strict';
const fs = require('node:fs');
const path = require('node:path');
exports.fixture = function ({ arch, source, assert, sharedHelpers = false }) {
    const macro = source.match(/^#define OBFH_SF_LEAF_SHIFT .*$/m);
    assert(macro, 'leaf shift emitter missing');
    const flags = arch === 'x64' ? 'pushfq; popq %%rdx;' : 'pushfl; popl %%edx;';
    let c = '#include <stdint.h>\n#include <stdio.h>\n' + (sharedHelpers ? '' : macro[0]) + '\ntypedef struct {unsigned value; uintptr_t flags;} Result;\n';
    const probes = [];
    for (const modrm of [0xc0, 0xc8, 0xe0, 0xe8, 0xf8]) for (let count = 1; count <= 31; count++) {
        const id = probes.length;
        probes.push({ id, count });
        for (const old of [true, false]) c += `static Result probe_${id}_${old ? 0 : 1}(unsigned value){uintptr_t f;Result r;__asm__ __volatile__("cmpl $123,%%eax;" ${old ? `".byte 0xc1,${modrm},${count};"` : 'OBFH_SF_LEAF_SHIFT'} "${flags}":"+a"(value),"=d"(f):[rotate]"i"(${count}),[leaf_shift]"i"(${modrm}):"cc");r.value=value;r.flags=f;return r;}\n`;
    }
    c += 'int main(void){unsigned v=0;for(unsigned i=0;i<1000;i++){v=i<32?1u<<i:i==32?0:i==33?~0u:v*1664525u+1013904223u;Result a,b;\n';
    for (const { id, count } of probes) c += `a=probe_${id}_0(v);b=probe_${id}_1(v);if(a.value!=b.value||((a.flags^b.flags)&${count === 1 ? '0x8c5' : '0xc5'}u)){fprintf(stderr,"shift encoding: probe ${id}, iteration %u, input %u, values %u/%u\\n",i,v,a.value,b.value);return 1;}\n`;
    c += '}if(puts("SHORT_ENCODINGS_PASS")==EOF||fflush(stdout))return 2;return 0;}\n';
    return c;
};
exports.runSuite = async function ({ arch, compiler, directory, source, check, compile, execute, assert }) {
    await check(`${arch}/short leaf encodings: all shifts/counts, results and defined flags`, async () => {
        const c = exports.fixture({ arch, source, assert });
        const file = path.join(directory, arch + '-short-encodings.c');
        fs.writeFileSync(file, c);
        await execute(await compile(compiler, directory, arch + '-short-encodings.exe', file, []), 'SHORT_ENCODINGS_PASS');
    });
};
