'use strict';
const fs = require('node:fs');
const path = require('node:path');

function exportsAt(file) {
    const bytes = fs.readFileSync(file), pe = bytes.readUInt32LE(0x3c), opt = pe + 24;
    const sections = opt + bytes.readUInt16LE(pe + 20), count = bytes.readUInt16LE(pe + 6);
    function offset(rva) {
        for (let i = 0; i < count; ++i) {
            const at = sections + 40 * i, start = bytes.readUInt32LE(at + 12);
            if (rva >= start && rva < start + bytes.readUInt32LE(at + 16)) return bytes.readUInt32LE(at + 20) + rva - start;
        }
        throw new Error('unmapped case probe RVA');
    }
    const exp = offset(bytes.readUInt32LE(opt + (bytes.readUInt16LE(opt) === 0x20b ? 112 : 96)));
    const names = offset(bytes.readUInt32LE(exp + 32)), ords = offset(bytes.readUInt32LE(exp + 36)), funcs = offset(bytes.readUInt32LE(exp + 28));
    const result = {};
    for (let i = 0; i < bytes.readUInt32LE(exp + 24); ++i) {
        const name = offset(bytes.readUInt32LE(names + i * 4));
        result[bytes.toString('ascii', name, bytes.indexOf(0, name)).replace(/^_/, '')] = offset(bytes.readUInt32LE(funcs + bytes.readUInt16LE(ords + i * 2) * 4));
    }
    return { bytes, result };
}

exports.runSuite = async function({ arch, compiler, directory, source, check, compile, execute, run, assert }) {
    for (const mode of [0, 1])
        for (const seed of [0, 1, 0xdeadbeef, 0xffffffff])
            await check(`${arch}/case labels, fallthrough and nested grammar/cflow-v${mode + 1}/seed ${seed}`, async () =>
                execute(await compile(compiler, directory, `${arch}-case-tokens-${mode}-${seed}.exe`,
                    path.join(__dirname, 'case_tokens.c'),
                    ['OBFH_DISABLE_ANTIDEBUG=1', `OBFH_BETTER_CONTROL_FLOW=${mode}`, `OBFH_SEED=${seed}u`]), 'CASE_TOKENS_PASS'));
    for (const flag of ['OBFH_DISABLE_CONTROL_FLOW=1', 'OBFH_DISABLE_ANY_OBFUSCATION=1'])
        await check(`${arch}/case labels disabled/${flag}`, async () =>
            execute(await compile(compiler, directory, `${arch}-case-disabled-${flag}.exe`,
                path.join(__dirname, 'case_tokens.c'), [flag, 'OBFH_DISABLE_ANTIDEBUG=1']), 'CASE_TOKENS_PASS'));
    await check(`${arch}/case rescan, isolated size budget and seed/site diversity`, async () => {
        function definition(name) {
            const match = new RegExp('^#define ' + name + '(?=[ (])', 'm').exec(source);
            assert(match, 'missing case dependency: ' + name);
            const lines = source.slice(match.index).split('\n');
            let count = 1;
            while (lines[count - 1].trimEnd().endsWith('\\')) ++count;
            return lines.slice(0, count).join('\n');
        }
        const begin = source.indexOf('#define OBFH_CASE_CONTEXT_I'), end = source.indexOf('#define OBFUS_CONDITION_BLOCK', begin);
        assert(begin >= 0 && end > begin, 'case interception block missing');
        const macros = ['OBFH_MIX_A', 'OBFH_MIX_B', 'OBFH_MIX_FIELDS', 'OBFH_DATA_FIELDS', 'OBFH_FLOW_FAST_TRANSFER'].map(definition).join('\n');
        const body = '(int x){int sum=0;switch(x){case 0:sum+=1;case 1:sum+=2;break;case 2:sum+=3;}return sum;}\n';
        const c = '#include <stdio.h>\nstatic int heavy, nested_if;\n' +
            '__declspec(dllexport) int native_case' + body + '__declspec(dllexport) void native_end(void){}\n' +
            '#define __obfh_asm__(...) __asm__ __volatile__(__VA_ARGS__)\n#define OBFH_FLOW_ELSE_GUARD ((++heavy),0)\n#define if(...) if((++nested_if,(__VA_ARGS__)))\n' + macros + '\n' + source.slice(begin, end) +
            '__declspec(dllexport) int protected_case' + body + '__declspec(dllexport) void protected_end(void){}\n' +
            '__declspec(dllexport) int second_case' + body + '__declspec(dllexport) void second_end(void){}\n' +
            '#undef if\nint main(void){for(int i=-2;i<5;++i)if(native_case(i)!=protected_case(i)||native_case(i)!=second_case(i))return 1;\n' +
            'if(heavy||nested_if)return 2;if(0){}else{if(heavy!=1)return 3;}if(puts("CASE_MATRIX_PASS")==EOF||fflush(stdout))return 4;return 0;}\n';
        const file = path.join(directory, arch + '-case-matrix.c');
        fs.writeFileSync(file, c);
        const preprocessed = await run(compiler, ['-E', '-DOBFH_BUILD_SEED=0u', file]);
        assert(preprocessed.status === 0, 'case preprocessing failed');
        const chunks = [...preprocessed.stdout.matchAll(/__obfh_case_entry_\d+\s*:\s*if[\s\S]*?else case/g)].map(match => match[0]);
        assert(chunks.length === 6 && chunks.every(chunk => chunk.length <= 2400), 'case preprocessing expansion budget exceeded');
        assert((preprocessed.stdout.match(/__obfh_fast_site\s*=/g) || []).length === 6, 'case must capture one fast draw per site');
        const variants = [], lengths = new Set();
        for (const seed of [0, 1, 2, 3, 4, 5, 6, 7, 0xdeadbeef, 0xffffffff, 0]) {
            const exe = await compile(compiler, directory, `${arch}-case-matrix-${variants.length}.exe`, file, [`OBFH_SEED=${seed}u`]);
            await execute(exe, 'CASE_MATRIX_PASS');
            const { bytes, result } = exportsAt(exe);
            const native = bytes.subarray(result.native_case, result.native_end), protectedBytes = bytes.subarray(result.protected_case, result.protected_end);
            const second = bytes.subarray(result.second_case, result.second_end);
            const extra = protectedBytes.length - native.length;
            assert(extra >= 30 && extra <= 105, `case size budget: ${extra} extra bytes for three labels`);
            assert(!protectedBytes.equals(second), 'case sites have identical bytes');
            variants.push(protectedBytes.toString('hex'));
            lengths.add(extra);
        }
        assert(variants[0] === variants.at(-1), 'case fixed-seed replay differs');
        assert(new Set(variants.slice(0, -1)).size === variants.length - 1 && lengths.size > 1, 'case variants lack seed/size diversity');
        console.log(`CASE ${arch}: three labels add ${Math.min(...lengths)}..${Math.max(...lengths)} bytes; fixed seed replays exactly`);
    });
};
