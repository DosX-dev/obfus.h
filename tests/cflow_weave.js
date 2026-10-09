'use strict';
const fs = require('node:fs');
const path = require('node:path');
const cache = require('./cflow_weave_cache');
// The oracle uses ordinary unsigned C, independently of raw instruction encodings.
function fixture(header) {
const fragment = header.slice(header.indexOf('#define OBFH_P_SLOT('), header.indexOf('#define OBFH_P_FINISH_ASM'));
const reference = String.raw`
static unsigned rotate(unsigned x, unsigned r) { return (x << r) | (x >> (32-r)); }
static void reference(unsigned *x, unsigned *y, unsigned style, unsigned k, unsigned m, unsigned a, unsigned r,
                      unsigned donor, unsigned donor_style, unsigned donor_mul, unsigned donor_add, unsigned donor_rot) {
    static const unsigned unary[4][2]={{0,3},{2,1},{1,0},{3,2}};
    unsigned remaining[4]={0,1,2,3}, order[4], rank=(k>>8)%12u, count=4;
    for(unsigned i=0;i<4;++i) {
        unsigned divisor = i==0 ? 6 : i==1 ? 2 : 1;
        unsigned pick=rank/divisor; rank%=divisor;
        unsigned token=remaining[pick];
        order[i]=token<2 ? unary[style][token] : token+2;
        for(unsigned j=pick;j+1<count;++j)remaining[j]=remaining[j+1];
        --count;
    }
    for(unsigned i=0;i<4;++i) {
        unsigned op=i==0 ? unary[donor_style][(donor>>8)%12u/6u] : order[i];
        if(op>=4) {
            unsigned *target=((op==4) ^ !!(k & (1u<<19))) ? y : x;
            unsigned *other=target==x ? y : x;
            if(k & (1u<<(16+op))) { *target+=2u*(*other); *other*=3u; }
            else *target=2u*(*other)-*target;
        } else {
            switch(op) {
            case 0: *x^=(i==0?donor:k); *y^=(i==0?donor:k); break;
            case 1: *x*=(i==0?donor_mul:m); *y*=(i==0?donor_mul:m); break;
            case 2: *x+=(i==0?donor_add:a); *y+=(i==0?donor_add:a); break;
            case 3: *x=rotate(*x,i==0?donor_rot:r); *y=rotate(*y,i==0?donor_rot:r); break;
            }
        }
    }
}
`;
let source = '#include <stdio.h>\n#define __obfh_asm__(...) __asm__ __volatile__(__VA_ARGS__)\n#define OBFH_P_BEFORE\n#define OBFH_P_TRACE(s,p) ((void)0)\n' + fragment + reference;
const names=[];
for(let style=0;style<4;++style) for(let enc=0;enc<256;++enc) for(const bits of new Set([0,15,(enc>>>4)&15])) {
    const n=names.length, name='probe_'+n, key=(0xca000000 | (enc<<14) | ((enc%24)<<8) | bits)>>>0, mul=(Math.imul(n+1,2654435761)|1)>>>0, add=Math.imul(n+3,1597334677)>>>0, rot=n%31+1;
    names.push(name);
    source+=`static int ${name}(unsigned x,unsigned y) {
        enum { __obfh_style00=${style},__obfh_k00=${key}u,__obfh_m00=${mul}u,__obfh_a00=${add}u,__obfh_r00=${rot},
               __obfh_k10=${(Math.imul(key,1597334677)>>>0)}u, __obfh_style10=${(n+1)%4}, __obfh_m10=${(Math.imul(n+7,2246822519)|1)>>>0}u,
               __obfh_a10=${(add^0x9e3779b9)>>>0}u, __obfh_r10=${(rot+7)%31+1} };
        unsigned __obfh_flow_state=x,__obfh_flow_tag=y;
        OBFH_P_PREPARE(0,0); OBFH_P_STEP(0,0);
        reference(&x,&y,${style},${key}u,${mul}u,${add}u,${rot},__obfh_k10,__obfh_style10,__obfh_m10,__obfh_a10,__obfh_r10);
        return x==__obfh_flow_state && y==__obfh_flow_tag;
    }\n`;
}
source+='static int (*probes[])(unsigned,unsigned)={'+names.join(',')+'};\n';
source+=String.raw`int main(void) {
    unsigned x=0;
    for(unsigned i=0;i<64;++i) {
        for(unsigned n=0;n<sizeof(probes)/sizeof(probes[0]);++n)
            if(!probes[n](x,~x) || !probes[n](x,x) || !probes[n](x,x+1)) { printf("WEAVE_FAIL %u %u\n",i,n); return 1; }
        x=x*1664525u+1013904223u;
    }
    puts("CFLOW_WEAVE_PASS"); return 0;
}`;
return source;
}

async function runSuite({ arch, compiler, directory, source, check, compile, execute, run, assert }) {
    await require('./cflow_selectors').runSuite({ arch, compiler, directory, source, check, compile, execute });
    await check(arch + '/CFLOW linked components and register roles', async () => {
        assert(cache.update(source) === source.replace(/\r\n/g, '\n'), 'CFLOW cache differs from readable macros');
        const normal = fixture(source), file = path.join(directory, arch + '-weave.c');
        fs.writeFileSync(file, normal);
        await execute(await compile(compiler, directory, arch + '-weave.exe', file, []), 'CFLOW_WEAVE_PASS');
        // Check full transported values, not only their equality. Discard either output.
        for (const coordinate of [0, 1]) {
            const start=normal.indexOf('#define OBFH_P_PERMUTE('), end=normal.indexOf('#define OBFH_P_PREPARE(',start);
            const macro=normal.slice(start,end), register=coordinate ? 'd' : 'a';
            const target='"+' + register + '"(OBFH_P_ROLE(s, p, ' + coordinate + '))';
            const mutated=macro.replace('({ __obfh_asm__', '({ unsigned discard=OBFH_P_ROLE(s,p,' + coordinate + '); __obfh_asm__')
                .replace(target, '"+' + register + '"(discard)');
            assert(mutated !== macro && mutated.includes('"+' + register + '"(discard)'), 'coordinate mutation target missing');
            const code=normal.slice(0,start)+mutated+normal.slice(end);
            const mutation=path.join(directory, arch + '-weave-missing-' + coordinate + '.c');
            fs.writeFileSync(mutation,code);
            const exe=await compile(compiler,directory,arch + '-weave-missing-' + coordinate + '.exe',mutation,[]);
            const result=await run(exe,[]);
            assert(result.status !== 0 && result.stdout.includes('WEAVE_FAIL'), 'discarded coordinate survived');
        }
    });
}
module.exports = { fixture, runSuite };
