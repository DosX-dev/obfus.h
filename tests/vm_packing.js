'use strict';
const fs = require('node:fs');
const path = require('node:path');
const { macroDefinition } = require('./source_macros');

async function runSuite({ arch, compiler, directory, source, check, compile, execute, assert }) {
    await check(`${arch}/VM direct packing preserves all eight instruction programs`, async () => {
        const normalized = source.replace(/\r\n/g, '\n');
        const definition = name => macroDefinition(normalized, name, { last: true }).body;
        function block(start, end) {
            const first = normalized.indexOf(start), last = normalized.indexOf(end, first);
            assert(first >= 0 && last > first, 'VM packing fixture block missing: ' + start);
            return normalized.slice(first, last);
        }
        let c = '#include <stdio.h>\n';
        c += block('enum {\n    OBFH_V_LOAD_A', 'typedef struct\n{\n    unsigned char bytes[sizeof(long double)]');
        c += fs.readFileSync(path.join(__dirname, 'vm_packing.c'), 'utf8') + '\n';
        const opcode = block('#define OBFH_V_OPCODE_MODE', '// Word masking changes');
        const word = block('#define OBFH_V_WORD_MODE', '// Capture seed mixing once.');
        const fields = block('#if ((OBFH_SEED >> 14) % 6u) == 0u', '#define OBFH_VM_FIELDS_0(');
        const macros = ['OBFH_V_OPERATION_OPCODE_0', 'OBFH_V_OPERATION_OPCODE_1', 'OBFH_V_OPERATION_IMMEDIATE',
            'OBFH_V_PACKED_FIELD', 'OBFH_V_PACKED_INSTRUCTION', 'OBFH_V_PACKED_OPERATION_0',
            'OBFH_V_PACKED_OPERATION_1', 'OBFH_V_ENCODE_RAW'];
        for (const name of macros) c += definition(name) + '\n';
        // Runtime parameters exercise many keys without duplicating megabytes of constant expressions.
        // Production enum expressions are retained verbatim; only the declaration kind changes here.
        c += definition('OBFH_V_PROGRAM_SETUP').replace(/enum\s*\{/, 'unsigned int ').replace(/\}\s*$/, ';') + '\n';
        for (let form = 0; form < 8; ++form) c += definition('OBFH_V_PROGRAM_' + form) + '\n';
        const selectedMacros = [...new Set([...(opcode + word + fields).matchAll(/^#define (\w+)/gm)].map(match => match[1]))];
        const seeds = [];
        for (let mode = 0; mode < 6; ++mode) for (let field = 0; field < 6; ++field) {
            const seed = (mode << 24) | (((field - (mode * 1024) % 6 + 6) % 6) << 14) | (((mode + field) % 9) << 6);
            seeds.push(seed);
            c += `#define OBFH_SEED ${seed}u\n` + opcode + word + fields;
            c += `static int probe_${seed}(unsigned __obfh_vkey,unsigned __obfh_voperation){\n`;
            c += 'unsigned __obfh_va=__obfh_vkey&3u,offset=1u+((__obfh_vkey>>3)%3u),pick=1u+((__obfh_vkey>>5)&1u),other=pick+(pick>=offset);\n';
            c += 'unsigned __obfh_vb=(__obfh_va+offset)&3u,__obfh_vc=(__obfh_va+other)&3u,__obfh_vd=(__obfh_va+6u-offset-other)&3u;\n';
            c += 'OBFH_V_PROGRAM_SETUP;const unsigned *actual[8]={' + Array.from({ length: 8 }, (_, i) => 'OBFH_V_PROGRAM_' + i).join(',') + '};\n';
            c += `return reference_programs(actual,__obfh_vkey,__obfh_voperation,__obfh_va,__obfh_vb,__obfh_vc,__obfh_vd,${seed}u);}\n`;
            for (const name of ['OBFH_SEED', ...selectedMacros]) c += '#undef ' + name + '\n';
        }
        c += 'int main(void){unsigned key=1;int(*probes[])(unsigned,unsigned)={' + seeds.map(seed => 'probe_' + seed).join(',') + '};\n';
        c += 'for(unsigned f=0;f<36;++f)for(unsigned n=0;n<64;++n){key=key*1664525u+1013904223u;key=(key&0x9fffffffu)|((n&3u)<<29);\n';
        c += 'for(unsigned op=0;op<19;++op)if(probes[f](key,op))return 1;}puts("VM_PACKING_PASS");return 0;}\n';
        const file = path.join(directory, arch + '-vm-packing.c');
        fs.writeFileSync(file, c);
        await execute(await compile(compiler, directory, arch + '-vm-packing.exe', file, []), 'VM_PACKING_PASS');
    });
}

module.exports = { runSuite };
