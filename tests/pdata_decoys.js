'use strict';
function fixture() {
    return `#include <windows.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "../include/obfus.h"
#undef if
#undef else
#undef printf
#undef memset
#undef GetProcAddress
#undef GetModuleHandleA
#undef atoi
#if defined(OBFH_PDATA_DECOY_COUNT) && !NO_PDATA_DECOYS && !NO_OBF && defined(__x86_64__)
typedef unsigned (*NativeFn)(unsigned);
typedef PRUNTIME_FUNCTION (WINAPI *LookupFn)(DWORD64,PDWORD64,void *);
typedef PVOID (WINAPI *UnwindFn)(DWORD,DWORD64,DWORD64,PRUNTIME_FUNCTION,PCONTEXT,PVOID *,PDWORD64,PVOID);
static void (*carriers[])(void) = {
${Array.from({ length: 128 }, (_, i) => `#if OBFH_PDATA_DECOY_COUNT > ${i}\n__obfh_pdata_decoy_${i},\n#endif`).join('\n')}
};
static unsigned char *patched;
static LONG WINAPI exception_filter(EXCEPTION_POINTERS *e){
    if(e->ExceptionRecord->ExceptionCode!=EXCEPTION_BREAKPOINT)ExitProcess(81);
    puts("PDATA_EXCEPTION_PASS");fflush(stdout);ExitProcess(0);return EXCEPTION_EXECUTE_HANDLER;
}
int main(int argc,char **argv){
    LookupFn lookup=(LookupFn)GetProcAddress(GetModuleHandleA("ntdll.dll"),"RtlLookupFunctionEntry");
    UnwindFn unwind=(UnwindFn)GetProcAddress(GetModuleHandleA("ntdll.dll"),"RtlVirtualUnwind");
    unsigned char *stack=(unsigned char *)HeapAlloc(GetProcessHeap(),0,4096);
    DWORD64 caller_sp=((DWORD64)(uintptr_t)stack+3072)&~15ull;
    DWORD64 caller_bp=caller_sp+256,return_ip=(DWORD64)(uintptr_t)main;
    unsigned count=sizeof(carriers)/sizeof(carriers[0]),i,k,states=0,calls=0;
    if(!lookup||!unwind||!stack||count!=OBFH_PDATA_DECOY_COUNT)return 80;
    SetErrorMode(SEM_FAILCRITICALERRORS|SEM_NOGPFAULTERRORBOX);
    if(argc==2){
        unsigned n=(unsigned)atoi(argv[1]);DWORD old;
        if(n>=count)return 82;
        patched=(unsigned char *)carriers[n]+22;
        if(!VirtualProtect(patched,1,PAGE_EXECUTE_READWRITE,&old))return 83;
        *patched=0xCC;FlushInstructionCache(GetCurrentProcess(),patched,1);
        SetUnhandledExceptionFilter(exception_filter);
        ((NativeFn)((unsigned char *)carriers[n]+11))(123u);return 84;
    }
    for(i=0;i<count;i++){
        unsigned char *fn=(unsigned char *)carriers[i]+11;DWORD64 image=0,est=0;PVOID data=NULL;
        PRUNTIME_FUNCTION entry=lookup((DWORD64)(uintptr_t)fn,&image,NULL);
        unsigned frame,span;int offsets[7];
        if(!entry||image+entry->BeginAddress!=(DWORD64)(uintptr_t)fn)return 85;
        if(fn[0]!=0x55||fn[1]!=0x48||fn[2]!=0x89||fn[3]!=0xE5||fn[4]!=0x48||fn[5]!=0x81||fn[6]!=0xEC)return 86;
        frame=*(unsigned *)(fn+7);span=(unsigned)(image+entry->EndAddress-(DWORD64)(uintptr_t)fn);
        if(span<24||fn[span-11]!=0x48||fn[span-10]!=0x81||fn[span-9]!=0xC4||*(unsigned *)(fn+span-8)!=frame||fn[span-4]!=0x5D||fn[span-3]!=0xC3)return 87;
        offsets[0]=0;offsets[1]=1;offsets[2]=4;offsets[3]=11;offsets[4]=span-11;offsets[5]=span-4;offsets[6]=span-3;
        for(k=0;k<7;k++){
            CONTEXT c;unsigned char *blob=(unsigned char *)&c;unsigned z;
            for(z=0;z<sizeof(c);z++)blob[z]=0;
            *(DWORD64 *)(uintptr_t)(caller_sp-8)=return_ip;*(DWORD64 *)(uintptr_t)(caller_sp-16)=caller_bp;
            c.ContextFlags=CONTEXT_FULL;c.Rip=(DWORD64)(uintptr_t)fn+offsets[k];
            c.Rsp=(k==0||k==6)?caller_sp-8:(k==1||k==2||k==5)?caller_sp-16:caller_sp-16-frame;
            c.Rbp=(k<2||k==6)?caller_bp:caller_sp-16;
            c.Rbx=17;c.Rsi=19;c.Rdi=23;c.R12=29;c.R13=31;c.R14=37;c.R15=41;
            unwind(0,image,c.Rip,entry,&c,&data,&est,NULL);
            if(c.Rip!=return_ip||c.Rsp!=caller_sp||c.Rbp!=caller_bp||c.Rbx!=17||c.Rsi!=19||c.Rdi!=23||c.R12!=29||c.R13!=31||c.R14!=37||c.R15!=41){printf("STATE_FAIL %u %u\\n",i,k);return 88;}
            states++;
        }
        printf("PDATA_ENTRY %u %lx %lx %u %u\\n",i,entry->BeginAddress,entry->EndAddress,frame,(unsigned)OBFH_PD_DRAW(i,1u)&15u);
        for(k=0;k<32;k++){
            DWORD64 before,after;unsigned x=k*2654435761u,first,second;
            volatile unsigned sentinel[2]={x,~x};
            __asm__ __volatile__("movq %%rsp,%0":"=r"(before));
            first=((NativeFn)fn)(x);second=((NativeFn)fn)(x);
            __asm__ __volatile__("movq %%rsp,%0":"=r"(after));
            if(before!=after||sentinel[0]!=x||sentinel[1]!=~x||first!=second)return 89;
            calls+=2;
        }
    }
    HeapFree(GetProcessHeap(),0,stack);
    printf("PDATA_PASS count=%u states=%u calls=%u\\n",count,states,calls);return 0;
}
#else
int main(void){puts("PDATA_DISABLED");return 0;}
#endif
`;
}
function measure(output, binary, assert) {
    const m = output.match(/PDATA_PASS count=(\d+) states=(\d+) calls=(\d+)/); assert(m, 'no pdata result'); const count = +m[1]; assert(count >= 86 && count <= 128 && +m[2] === count * 7 && +m[3] === count * 64, 'invalid coverage');
    const nt = binary.readUInt32LE(60), opt = nt + 24, sections = []; for (let i = 0; i < binary.readUInt16LE(nt + 6); i++) { let o = opt + binary.readUInt16LE(nt + 20) + 40 * i; sections.push({ rva: binary.readUInt32LE(o + 12), size: binary.readUInt32LE(o + 16), raw: binary.readUInt32LE(o + 20) }); } const off = r => { let s = sections.find(s => r >= s.rva && r < s.rva + s.size); assert(s, 'unmapped RVA'); return s.raw + r - s.rva; };
    const dir = opt + 112 + 24, rva = binary.readUInt32LE(dir), size = binary.readUInt32LE(dir + 4); assert(size % 12 === 0, 'malformed exception table'); const records = []; for (let i = 0; i < size; i += 12) { const o = off(rva) + i; records.push({ begin: binary.readUInt32LE(o), end: binary.readUInt32LE(o + 4), unwind: binary.readUInt32LE(o + 8) }); }
    for (let i = 0; i < records.length; i++) { assert(records[i].begin < records[i].end, "invalid runtime range"); if (i) assert(records[i - 1].end <= records[i].begin, "unsorted or overlapping runtime table"); }
    const entries = [...output.matchAll(/PDATA_ENTRY (\d+) ([\da-f]+) ([\da-f]+) (\d+) (\d+)/g)].map(x => ({ id: +x[1], begin: parseInt(x[2], 16), end: parseInt(x[3], 16), frame: +x[4], family: +x[5] })); assert(entries.length === count, 'entries missing');
    const code = entries.map(e => { const r = records.find(r => r.begin === e.begin && r.end === e.end); assert(r, 'missing static PE entry'); assert(binary.subarray(off(r.unwind), off(r.unwind) + 8).equals(Buffer.from([1, 4, 2, 5, 4, 3, 1, 0x50])), 'unwind does not match native prologue'); return binary.subarray(off(e.begin), off(e.end)); }); assert(new Set(code.map(b => b.toString('hex'))).size === count, 'identical decoy bodies'); return { count, entries, code };
}
module.exports = { fixture, measure };

module.exports.runSuite = async function ({ arch, compiler, directory, source, check, compile, execute, run, assert, afterChecks = action => action() }) {
    const fs = require('fs'), path = require('path');
    const root = path.join(directory, arch + '-pdata');
    fs.mkdirSync(path.join(root, 'include'), { recursive: true });
    fs.mkdirSync(path.join(root, 'tests'), { recursive: true });
    fs.writeFileSync(path.join(root, 'include', 'obfus.h'), source);
    const file = path.join(root, 'tests', 'pdata.c');
    fs.writeFileSync(file, fixture());
    const flags = ['NO_CFLOW=1', 'NO_ANTIDEBUG=1'];
    if (arch === 'x86') {
        await check('x86/pdata decoys are absent', async () => await execute(await compile(compiler, directory, 'x86-pdata-disabled.exe', file, flags), 'PDATA_DISABLED'));
        return;
    }
    const builds = new Map();
    for (const [label, extra] of [['seed-0', ['OBFH_BUILD_SEED=0u']], ['seed-1', ['OBFH_BUILD_SEED=1u']], ['seed-max', ['OBFH_BUILD_SEED=4294967295u']], ['minimum', ['OBFH_PDATA_DECOY_COUNT=86']], ['maximum', ['OBFH_PDATA_DECOY_COUNT=128']]]) {
        await check('x64/pdata/native calls + all unwind states/' + label, async () => {
            const exe = await compile(compiler, directory, 'x64-pdata-' + label + '.exe', file, flags.concat(extra));
            const result = await execute(exe, 'PDATA_PASS');
            const data = measure(result.stdout, fs.readFileSync(exe), assert);
            if (label === 'minimum') assert(data.count === 86, 'minimum count not honoured');
            if (label === 'maximum') assert(data.count === 128, 'maximum count not honoured');
            builds.set(label, { ...data, exe });
        });
    }
    await check('x64/pdata/seed variation and exact repeatability', async () => {
        const first = builds.get('seed-1'), second = builds.get('seed-0');
        assert(first && second, 'seed builds missing');
        assert(first.code.slice(0, Math.min(first.count, second.count)).every((b, i) => !b.equals(second.code[i])), 'seed does not change decoy bodies');
        const exe = await compile(compiler, directory, 'x64-pdata-repeat.exe', file, flags.concat('OBFH_BUILD_SEED=1u'));
        const result = await execute(exe, 'PDATA_PASS');
        const data = measure(result.stdout, fs.readFileSync(exe), assert);
        assert(data.count === first.count && data.code.every((b, i) => b.equals(first.code[i])), 'fixed seed is not reproducible');
    });
    await check('x64/pdata/real exceptions through every maximum-pool decoy', async () => {
        const data = builds.get('maximum'); assert(data, 'maximum pool missing');
        for (let i = 0; i < data.count; i++) await execute(data.exe, 'PDATA_EXCEPTION_PASS', [String(i)]);
    });
    for (const flag of ['NO_PDATA_DECOYS=1', 'NO_OBF=1']) await check('x64/pdata/disabled/' + flag, async () => await execute(await compile(compiler, directory, 'x64-pdata-' + flag.split('=')[0] + '.exe', file, flags.concat(flag)), 'PDATA_DISABLED'));
    await check('x64/pdata/DLL unwind records use the DLL image base', async () => {
        const dllSource = path.join(root, 'tests', 'pdata_dll.c');
        fs.writeFileSync(dllSource, fixture().replace('int main(int argc,char **argv)', '__declspec(dllexport) int main(int argc,char **argv)'));
        const dll = await compile(compiler, directory, 'x64-pdata.dll', dllSource, flags, ['-shared']);
        const hostSource = path.join(root, 'tests', 'host.c');
        fs.writeFileSync(hostSource, '#include <windows.h>\n#include <stdio.h>\nint main(int n,char **v){HMODULE h=LoadLibraryA(v[1]);if(!h)return 90;int(*f)(int,char**)= (void*)GetProcAddress(h,"main");if(!f)return 91;int r=f(1,v);FreeLibrary(h);return r;}');
        const host = await compile(compiler, directory, 'x64-pdata-host.exe', hostSource, []);
        const result = await execute(host, 'PDATA_PASS', [dll]);
        measure(result.stdout, fs.readFileSync(dll), assert);
    });
    await check('x64/pdata/multiple translation units', async () => {
        const extraSource = path.join(root, 'tests', 'second.c');
        fs.writeFileSync(extraSource, '#include "../include/obfus.h"\nint second_unit(int x){return x+1;}');
        const exe = await compile(compiler, directory, 'x64-pdata-multi.exe', file, flags, [extraSource]);
        const result = await execute(exe, 'PDATA_PASS'); measure(result.stdout, fs.readFileSync(exe), assert);
    });
    afterChecks(() => fs.writeFileSync(path.join(root, 'coverage.json'), JSON.stringify([...builds].map(([name, data]) => ({ name, count: data.count, families: [...new Set(data.entries.map(e => e.family))], frames: [...new Set(data.entries.map(e => e.frame))] })), null, 2)));
};
