'use strict';
const fs = require('node:fs');
const path = require('node:path');

const contracts = [
    ['numeric', 'NUMERIC_PASS'],
    ['vm_branches', 'BRANCH_PASS'],
    ['vm', 'VM_PASS']
];

// Native references must precede the protected header, even when their contracts
// share one translation unit. Each contract still runs in a fresh process.
function renameWorker(text, name) {
    // Preserve comments, diagnostics and string/character literals verbatim.
    return text.replace(/\/\*[\s\S]*?\*\/|\/\/[^\n]*|"(?:\\.|[^"\\])*"|'(?:\\.|[^'\\])*'|\bworker\b/g,
        token => token === 'worker' ? name + '_worker' : token);
}

function fixture(fixturesRoot = __dirname) {
    const sections = contracts.map(([name, marker]) => {
        const file = path.join(fixturesRoot, name + '.c');
        const text = fs.readFileSync(file, 'utf8').replace(/\r\n/g, '\n');
        const include = '#include "../include/obfus.h"';
        const at = text.indexOf(include);
        if (at < 0 || text.indexOf(include, at + include.length) !== -1)
            throw new Error('semantic fixture header boundary missing: ' + name);
        const end = text.indexOf('\n', at);
        if (end < 0) throw new Error('semantic fixture body missing: ' + name);
        const line = text.slice(0, end + 1).split('\n').length;
        const suffix = text.slice(end + 1);
        if (/^\s*#\s*(define|undef)\b/m.test(text.slice(0, at)))
            throw new Error('semantic fixture native prefix requires isolation: ' + name);
        for (const match of text.matchAll(/^\s*#\s*(define|undef)\s+(\w+)/gm)) {
            if (match[1] !== 'define' || !['CHECK', 'ROUTE', 'Obfh_VirtualMachine'].includes(match[2]))
                throw new Error('semantic fixture macro requires isolation: ' + name + '/' + match[2]);
        }
        if (/^\s*#\s*pragma\s+(push_macro|pop_macro)/m.test(text))
            throw new Error('semantic fixture macro stack requires isolation: ' + name);
        if ((suffix.match(/int main\(void\)/g) || []).length !== 1)
            throw new Error('semantic fixture main boundary missing: ' + name);
        return {
            name, marker, file, line,
            prefix: text.slice(0, at),
            suffix: renameWorker(suffix, name)
                .replace('int main(void)', 'static int ' + name + '_contract_main(void)')
        };
    });
    const location = (file, line) => `#line ${line} ${JSON.stringify(file.replace(/\\/g, '/'))}\n`;
    let result = sections.map(({ file, prefix }) => location(file, 1) + prefix).join('\n');
    result += '\n#include "../include/obfus.h"\n';
    for (const { file, line, suffix } of sections) {
        result += location(file, line) + suffix + '\n#undef CHECK\n#undef ROUTE\n#undef Obfh_VirtualMachine\n';
    }
    // Dispatch must not initialize protected state before the selected contract's
    // original first instruction. Keep these harness branches native.
    result += ['if', 'else', 'return', 'case', 'default', 'continue', 'do', 'break', 'switch', 'while', 'for']
        .map(name => '#undef ' + name + '\n').join('');
    result += '#line 1 "vm_semantics_dispatch.c"\nint main(int argc, char **argv) {\n';
    result += 'if (argc != 2 || argv[1][0] == 0 || argv[1][1] != 0) return 99;\n';
    for (let index = 0; index < sections.length; ++index)
        result += `if (argv[1][0] == '${index}') return ${sections[index].name}_contract_main();\n`;
    return result + 'return 98;\n}\n';
}

function writeFixture(root, source, fixturesRoot) {
    fs.mkdirSync(path.join(root, 'include'), { recursive: true });
    fs.mkdirSync(path.join(root, 'tests'), { recursive: true });
    fs.writeFileSync(path.join(root, 'include', 'obfus.h'), source);
    const file = path.join(root, 'tests', 'vm_semantics.c');
    fs.writeFileSync(file, fixture(fixturesRoot));
    return file;
}

async function executeContracts(exe, execute) {
    const failures = [];
    for (let index = 0; index < contracts.length; ++index) {
        try {
            await execute(exe, contracts[index][1], [String(index)]);
        } catch (error) {
            if (error.suiteDeadline) throw error;
            failures.push(new Error(contracts[index][0] + ': ' + error.message, { cause: error }));
        }
    }
    if (failures.length)
        throw new AggregateError(failures, failures.map(error => error.message).join('\n'));
}

module.exports = { contracts, fixture, writeFixture, executeContracts };
