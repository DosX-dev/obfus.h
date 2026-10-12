'use strict';
const fs = require('node:fs');
const os = require('node:os');
const path = require('node:path');

// These fixtures leave keyword interception enabled. Poisoned-keyword,
// atexit/input and DLL lifetime fixtures deliberately remain separate builds.
const cases = [
    ['custom_copy', 'CUSTOM_COPY_PASS', ['flags', 'rng', 'next', 'sign']],
    ['custom_wide', 'CUSTOM_WIDE_PASS', ['bytes_equal', 'rng', 'next', 'sign', 'record', 'calls', 'compare', 'fail_compare', 'inner_key', 'inner_values', 'compare_int', 'reentrant_compare']],
    ['custom_polymorphism', 'CUSTOM_POLYMORPHISM_PASS', ['bytes_equal', 'load_xmm', 'store_xmm', 'move', 'CHECK_CUSTOM_DRAW']],
    ['custom_extra', 'CUSTOM_EXTRA_PASS', ['compare', 'worker', 'allocation_attempts', 'failed_allocation', 'allocation_failure']]
];
const include = '#include "../include/obfus.h"';

function namespace(source, fixture, symbols) {
    const names = new Set([...symbols, 'main', ...source.match(/\bref_\w+\b/g) || []]);
    // Rename C identifiers only. Diagnostics, ASM strings and comments retain
    // their original contents, including source line-sensitive draw checks.
    return source.replace(/\/\*[\s\S]*?\*\/|\/\/[^\r\n]*|"(?:\\.|[^"\\])*"|'(?:\\.|[^'\\])*'|\b[A-Za-z_]\w*\b/g,
        token => names.has(token) ? fixture + '_' + token : token);
}

function fixture(header, fixturesRoot = __dirname) {
    if (!header || !path.isAbsolute(header)) throw new Error('CRT bundle requires the runner snapshot header');
    const units = cases.map(([name, , symbols]) => {
        const source = fs.readFileSync(path.join(fixturesRoot, name + '.c'), 'utf8');
        if (source.split(include).length !== 2) throw new Error(name + ': expected exactly one protection include');
        const split = source.indexOf(include);
        const prefix = source.slice(0, split);
        const body = source.slice(split + include.length);
        if (/^\s*#\s*(define|undef)\b/m.test(prefix) ||
            /^\s*#\s*pragma\s+(push_macro|pop_macro)/m.test(source))
            throw new Error(name + ': unexpected macro mutation before protection boundary');
        if (/^#\s*undef\s+(?:if|else|case|for|while|break|continue|return)\b/m.test(body))
            throw new Error(name + ': keyword mode changed; keep this fixture isolated');
        if ((body.match(/^int main\(void\)\s*\{/gm) || []).length !== 1)
            throw new Error(name + ': expected one no-argument main');
        const mutations = [...body.matchAll(/^#\s*(define|undef)\s+(\w+)[^\r\n]*/gm)];
        const allowed = new Set(['CHECK', ...(name === 'custom_polymorphism' ? ['CHECK_CUSTOM_DRAW'] : []),
            ...(name === 'custom_extra' ? ['malloc'] : [])]);
        if (mutations.some(match => !allowed.has(match[2])))
            throw new Error(name + ': unexpected macro mutation; review bundle isolation');
        if (name === 'custom_extra' && mutations.filter(match => match[2] === 'malloc').map(match => match[0]).join('\n') !==
            '#undef malloc\n#define malloc(size) failed_allocation(size)\n#undef malloc\n#define malloc(...) malloc_proxy(__VA_ARGS__)')
            throw new Error(name + ': allocation failure override no longer restores malloc');
        const file = JSON.stringify(path.join(fixturesRoot, name + '.c').replaceAll('\\', '/'));
        return {
            prefix: '#line 1 ' + file + '\n' + namespace(prefix, name, symbols),
            body: '#undef CHECK\n#line ' + (prefix.split('\n').length) + ' ' + file + '\n' + namespace(body, name, symbols)
        };
    });
    // All native reference wrappers precede interception. Each selected main
    // runs in its own process, so locale, tokenizer TLS and cold initialization
    // cannot be warmed or contaminated by another test case.
    return units.map(unit => unit.prefix).join('\n') + '\n#include ' +
        JSON.stringify(header.replaceAll('\\', '/')) + '\n' +
        units.map(unit => unit.body).join('\n') + `
#undef if
#undef else
#undef case
#undef for
#undef while
#undef break
#undef continue
#undef return
#undef CHECK
int main(int argc, char **argv) {
    if (argc != 2) return 2;
${cases.map(([name]) => '    if (!(strcmp)(argv[1], "' + name + '")) return ' + name + '_main();').join('\n')}
    return 2;
}
`;
}

async function runSuite({ compiler, directory, name, flags, header, fixturesRoot, compile, run, assert }) {
    const source = path.join(directory, name + '.c');
    fs.writeFileSync(source, fixture(header, fixturesRoot));
    const exe = await compile(compiler, directory, name + '.exe', source, flags);
    const failures = [];
    for (const [fixture, marker] of cases) {
        try {
            const result = await run(exe, [fixture]);
            if (result.status !== 0 || !result.stdout.includes(marker))
                failures.push(fixture + ': exit=' + result.status + ' ' + result.stdout + result.stderr);
        } catch (error) {
            if (error.suiteDeadline) throw error;
            failures.push(fixture + ': ' + error.message);
        }
    }
    const invalid = await run(exe, ['unknown']);
    assert(invalid.status === 2, 'CRT bundle accepted an unknown case');
    assert(!failures.length, failures.join('\n'));
}

async function verify(assert, fixturesRoot = __dirname) {
    const directory = fs.mkdtempSync(path.join(os.tmpdir(), 'obfh-crt-bundle-contract-'));
    try {
        for (const [name] of cases) fs.copyFileSync(path.join(fixturesRoot, name + '.c'), path.join(directory, name + '.c'));
        const header = path.join(directory, 'obfus.h');
        const file = path.join(directory, 'custom_copy.c');
        const original = fs.readFileSync(file, 'utf8');
        for (const [mutation, message] of [
            [original.replace(include, ''), 'expected exactly one protection include'],
            [original.replace('int main(void)', 'int renamed_main(void)'), 'expected one no-argument main'],
            [original + '\n#undef if\n', 'keyword mode changed'],
            [original + '\n#define memcpy(x) unrelated(x)\n', 'unexpected macro mutation']
        ]) {
            fs.writeFileSync(file, mutation);
            let rejected = false;
            try { fixture(header, directory); } catch (error) { rejected = error.message.includes(message); }
            assert(rejected, 'CRT bundle silently accepted unsafe fixture changes: ' + message);
        }
        fs.writeFileSync(file, original);
        const allocationFile = path.join(directory, 'custom_extra.c');
        const allocationSource = fs.readFileSync(allocationFile, 'utf8');
        fs.writeFileSync(allocationFile, allocationSource.replace('malloc_proxy(__VA_ARGS__)', 'failed_allocation(__VA_ARGS__)'));
        let allocationRejected = false;
        try { fixture(header, directory); } catch (error) { allocationRejected = error.message.includes('no longer restores malloc'); }
        assert(allocationRejected, 'CRT bundle accepted a leaking allocation failure override');
        fs.writeFileSync(allocationFile, allocationSource);
        const executed = [];
        let failure = '';
        await runSuite({
            directory, name: 'contract', header, fixturesRoot: directory, flags: [],
            compile: async () => 'unused.exe',
            run: async (_, args) => {
                executed.push(args[0]);
                const item = cases.find(([name]) => name === args[0]);
                if (!item) return { status: 2, stdout: '', stderr: '' };
                return { status: args[0] === 'custom_copy' ? 1 : 0, stdout: item[1], stderr: '' };
            },
            assert: (condition, message) => { if (!condition) failure = message; }
        });
        assert(cases.every(([name]) => executed.includes(name)), 'CRT bundle skipped cases after a failure');
        assert(failure.includes('custom_copy: exit=1'), 'CRT bundle lost case-specific failure diagnostics');
    } finally {
        const resolved = path.resolve(directory);
        if (path.dirname(resolved) !== path.resolve(os.tmpdir()) ||
            !path.basename(resolved).startsWith('obfh-crt-bundle-contract-'))
            throw new Error('refusing to remove an unexpected fixture directory');
        fs.rmSync(resolved, { recursive: true, force: true });
    }
}

module.exports = { cases, fixture, runSuite, verify };
