'use strict';
const fs = require('node:fs');
const path = require('node:path');

// Independent contiguous instructions retain the original terminal encodings.
function reference(style, zero) {
    const subtract = 'subl %%edx,%%eax;', xor = 'xorl %%edx,%%eax;';
    const difference = zero & 1 ? xor : subtract;
    const otherDifference = zero & 1 ? subtract : xor;
    return [
        difference + 'negl %%eax;sbbl %%eax,%%eax;.byte 0x83,0xc0,1;',
        otherDifference + '.byte 0x83,0xe8,1;sbbl %%eax,%%eax;negl %%eax;',
        ['xorl %%ecx,%%ecx;', 'subl %%ecx,%%ecx;', 'movl $0,%%ecx;'][zero] + 'cmpl %%edx,%%eax;movl $1,%%eax;cmovnel %%ecx,%%eax;',
        'cmpl %%edx,%%eax;setne %%cl;movzbl %%cl,%%eax;',
        difference + 'negl %%eax;sbbl %%eax,%%eax;notl %%eax;.byte 0x83,0xe0,1;',
        'cmpl %%edx,%%eax;sete %%cl;movzbl %%cl,%%eax;.byte 0x83,0xf0,1;',
        otherDifference + 'movl %%eax,%%ecx;negl %%ecx;orl %%ecx,%%eax;shrl $31,%%eax;.byte 0x83,0xf0,1;',
        otherDifference + 'movl $1,%%eax;.byte 0x75,5;movl $0,%%eax;'
    ][style];
}

function byteFixture() {
    let code = `
typedef struct { const unsigned char *start,*end; } TerminalSpan;
#if defined(__x86_64__)
#define TERMINAL_ADDRESS "leaq 8311f(%%rip),%%rax;movq %%rax,%[start];leaq 8312f(%%rip),%%rax;movq %%rax,%[end];jmp 8319f;8311:"
#else
#define TERMINAL_ADDRESS "leal 8311f,%%eax;movl %%eax,%[start];leal 8312f,%%eax;movl %%eax,%[end];jmp 8319f;8311:"
#endif
#define TERMINAL_BYTES(name,body,style,zero) static TerminalSpan name(void){TerminalSpan s;\
__asm__ __volatile__(TERMINAL_ADDRESS body "8312:8319:" :[start]"=m"(s.start),[end]"=m"(s.end):[finish]"i"(style),[zero_form]"i"(zero):"eax","edx","ecx","cc","memory");return s;}
`;
    const pairs = [];
    for (let style = 0; style < 8; ++style) for (let zero = 0; zero < 3; ++zero) {
        const id = style * 3 + zero;
        code += `TERMINAL_BYTES(terminal_bytes_actual_${id},OBFH_P_TERMINAL_TEXT,${style},${zero})\n`;
        code += `TERMINAL_BYTES(terminal_bytes_reference_${id},${JSON.stringify(reference(style, zero))},${style},${zero})\n`;
        pairs.push(`{terminal_bytes_actual_${id},terminal_bytes_reference_${id}}`);
    }
    return code + `static int terminal_bytes_check(void){TerminalSpan(*pairs[][2])(void)={${pairs.join(',')}};
for(unsigned i=0;i<24;++i){TerminalSpan a=pairs[i][0](),b=pairs[i][1]();
if(a.end-a.start!=b.end-b.start){fprintf(stderr,"terminal length %u\\n",i);return 1;}
for(unsigned j=0;j<(unsigned)(a.end-a.start);++j)if(a.start[j]!=b.start[j]){fprintf(stderr,"terminal bytes %u:%u\\n",i,j);return 1;}}return 0;}\n`;
}

function fixture(header) {
    const start = header.indexOf('#define OBFH_P_TERMINAL_TEXT');
    const end = header.indexOf('// The last-stage flag', start);
    if (start < 0 || end < start) throw new Error('Terminal emission missing');
    let code = '#include <stdio.h>\n' + header.slice(start, end);
    const probes = [];
    for (let style = 0; style < 8; ++style)
        for (let zero = 0; zero < 3; ++zero) {
            const name = 'terminal_' + style + '_' + zero;
            probes.push(name);
            code +=
                `static unsigned ${name}(unsigned x,unsigned y) { unsigned result;\n` +
                '__asm__ __volatile__("movl %[other], %%edx;" OBFH_P_TERMINAL_TEXT\n' +
                `: "=&a"(result) : "0"(x), [other] "m"(y), [finish] "i"(${style}), [zero_form] "i"(${zero})\n` +
                ': "edx","ecx","cc","memory"); return result; }\n';
        }
    code += byteFixture();
    code += 'static unsigned (*probes[])(unsigned,unsigned)={' + probes.join(',') + '};\n';
    code += String.raw`int main(void) {
    if(terminal_bytes_check())return 2;
    unsigned state=0;
    const unsigned edges[]={0,1,0xffffffffu,0x80000000u,0x7fffffffu,0x55555555u,0xaaaaaaaau};
    for(unsigned i=0;i<4096;++i) {
        state=state*1664525u+1013904223u;
        unsigned x=i<7?edges[i]:state;
        unsigned values[]={x,~x,x+1,x-1,0,0xffffffffu,x^0x80000000u};
        for(unsigned j=0;j<7;++j) for(unsigned p=0;p<sizeof(probes)/sizeof(probes[0]);++p)
            if(probes[p](x,values[j])!=((unsigned)(x==values[j]) ^ (p/3==3 || p/3==5 || p/3==7))) {
                printf("TERMINAL_FAIL %u %u %u\n",i,j,p);return 1;
            }
    }
    puts("CFLOW_TERMINAL_PASS");return 0;
}`;
    return code;
}

async function runSuite({ arch, compiler, directory, source, check, compile, execute }) {
    if (!source.includes('#define OBFH_P_TERMINAL_TEXT')) return;
    await check(arch + '/CFLOW fused terminal families and zero forms', async () => {
        const file = path.join(directory, arch + '-terminal.c');
        fs.writeFileSync(file, fixture(source));
        await execute(await compile(compiler, directory, arch + '-terminal.exe', file, []), 'CFLOW_TERMINAL_PASS');
    });
}
module.exports = { fixture, runSuite };
