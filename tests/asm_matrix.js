'use strict';
const fs = require('node:fs');
const path = require('node:path');
const sections = [
    ['frames', 'STACK_FRAMES_PASS'],
    ['encodings', 'SHORT_ENCODINGS_PASS'],
    ['jumps', 'SHORT_JUMPS_PASS']
];

// These fixtures use fragments only; they have no protected-header initialization
// or process lifetime contract. Share helpers, but isolate carrier names/types.
exports.fixture = function ({ arch, source, assert }) {
    const start = source.indexOf('#define OBFH_SF_BYTES');
    const end = source.indexOf('// BEGIN COMPACT ASM CACHE', start);
    assert(start >= 0 && end > start, 'proxy fragment boundaries missing');
    let text = '#include <stdint.h>\n#include <stdio.h>\n' + source.slice(start, end) + '\n';
    for (const [prefix, module] of [['frames', './stack_frames'], ['encodings', './short_encodings'], ['jumps', './short_jumps']]) {
        const fixture = require(module).fixture({ arch, source, assert, sharedHelpers: true });
        text += fixture.replace(/\b(Entry|Result|CARRIER|ADDRESS|SP|main|entry_\d+(?:_\d+)?|probe_\d+_\d+)\b/g,
            name => prefix + '_' + (name === 'main' ? 'run' : name)) + '\n';
    }
    return text + 'int main(int argc,char **argv){if(argc!=2||argv[1][0]==0||argv[1][1]!=0)return 99;' +
        sections.map(([name], index) => `if(argv[1][0]=='${index}')return ${name}_run();`).join('') +
        'return 99;}\n';
};

exports.runSuite = async function ({ arch, compiler, directory, source, check, compile, execute, assert }) {
    await check(`${arch}/ASM matrix: frames, shift results/flags and proxy jumps`, async () => {
        const file = path.join(directory, arch + '-asm-matrix.c');
        fs.writeFileSync(file, exports.fixture({ arch, source, assert }));
        const exe = await compile(compiler, directory, arch + '-asm-matrix.exe', file, []);
        const failures = [];
        // A damaged stack or illegal instruction must not suppress the other
        // matrices. Share compilation, but retain the original fresh processes.
        for (let index = 0; index < sections.length; ++index) {
            const [name, marker] = sections[index];
            try {
                await execute(exe, marker, [String(index)]);
            } catch (error) {
                if (error.suiteDeadline) throw error;
                failures.push(new Error(name + ': ' + error.message, { cause: error }));
            }
        }
        if (failures.length)
            throw new AggregateError(failures, failures.map(error => error.message).join('\n'));
    });
};
