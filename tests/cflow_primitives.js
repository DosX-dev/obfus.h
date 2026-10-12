'use strict';
const fs = require('node:fs');
const os = require('node:os');
const path = require('node:path');

function cases(source) {
    return [
        ...(source.includes('#define OBFH_P_TERMINAL_TEXT') ? [['terminal', 'CFLOW_TERMINAL_PASS', require('./cflow_terminal')]] : []),
        ['forms', 'CFLOW_FORMS_PASS', require('./cflow_forms')],
        ['single_lane', 'MACHINE_PRIMITIVES_PASS', require('./cflow_single_lane')],
        ...(source.includes('#define OBFH_SF_FLOW_COMMIT ') ? [['commit', 'CFLOW_COMMIT_PASS', require('./cflow_commit')]] : [])
    ];
}

function fixture(source, fixturesRoot) {
    const selected = cases(source);
    const units = selected.map(([name, , generator]) => {
        const code = generator.fixture(source, fixturesRoot);
        if ((code.match(/\bint main\(void\)\s*\{/g) || []).length !== 1)
            throw new Error(name + ': expected exactly one no-argument primitive main');
        // Rewrite identifiers only; oracle diagnostics and raw ASM stay intact.
        // Local names can repeat, but helper functions and probe tables cannot.
        const renamed = code.replace(/\/\*[\s\S]*?\*\/|\/\/[^\r\n]*|"(?:\\.|[^"\\])*"|'(?:\\.|[^'\\])*'|\b[A-Za-z_]\w*\b/g,
            token => /^(?:main|probes|probe_\d+|terminal_\d+_\d+)$/.test(token) ? name + '_' + token : token);
        const macros = new Set([...code.matchAll(/^\s*#\s*define\s+(\w+)/gm)].map(match => match[1]));
        // Each generator owns its macro environment, including conditional
        // x86/x64 definitions. Clean every introduced name before the next unit.
        return renamed + '\n' + [...macros].map(macro => '#undef ' + macro).join('\n') + '\n';
    });
    return '#include <string.h>\n' + units.join('\n') + `
int main(int argc, char **argv) {
    if (argc != 2) return 2;
${selected.map(([name]) => '    if (!(strcmp)(argv[1], "' + name + '")) return ' + name + '_main();').join('\n')}
    return 2;
}
`;
}

async function runSuite({ arch, compiler, directory, source, fixturesRoot, check, compile, run, assert }) {
    await check(arch + '/CFLOW isolated primitive contracts/shared build', async () => {
        const file = path.join(directory, arch + '-cflow-primitives.c');
        fs.writeFileSync(file, fixture(source, fixturesRoot));
        const exe = await compile(compiler, directory, arch + '-cflow-primitives.exe', file, []);
        const failures = [];
        // Fresh processes preserve startup state and ensure a failed/crashed
        // oracle does not prevent any other primitive contract from executing.
        for (const [name, marker] of cases(source)) {
            try {
                const result = await run(exe, [name]);
                if (result.status !== 0 || !result.stdout.includes(marker))
                    failures.push(name + ': exit=' + result.status + ' ' + result.stdout + result.stderr);
            } catch (error) {
                if (error.suiteDeadline) throw error;
                failures.push(name + ': ' + error.message);
            }
        }
        const invalid = await run(exe, ['unknown']);
        assert(invalid.status === 2, 'CFLOW primitives accepted an unknown case');
        assert(!failures.length, failures.join('\n'));
    });
}

async function verify(source, assert, fixturesRoot) {
    const directory = fs.mkdtempSync(path.join(os.tmpdir(), 'obfh-cflow-bundle-contract-'));
    try {
        const selected = cases(source);
        for (const throwing of [false, true]) {
            const executed = [];
            let failure = '';
            await runSuite({
                arch: 'contract', directory, source, fixturesRoot,
                check: async (_, body) => body(),
                compile: async () => 'unused.exe',
                run: async (_, [name]) => {
                    executed.push(name);
                    if (name === selected[0][0] && throwing) throw new Error('injected process failure');
                    const item = selected.find(([candidate]) => candidate === name);
                    return {
                        status: !item ? 2 : name === selected[0][0] ? 91 : 0,
                        stdout: item ? item[1] : '', stderr: ''
                    };
                },
                assert: (condition, message) => { if (!condition) failure = message; }
            });
            assert(selected.every(([name]) => executed.includes(name)), 'CFLOW bundle skipped cases after a failure');
            assert(executed.includes('unknown'), 'CFLOW bundle did not reject an unknown case');
            assert(failure.includes(selected[0][0] + ': '), 'CFLOW bundle lost case-specific failure diagnostics');
        }
    } finally {
        const resolved = path.resolve(directory);
        if (path.dirname(resolved) !== path.resolve(os.tmpdir()) ||
            !path.basename(resolved).startsWith('obfh-cflow-bundle-contract-'))
            throw new Error('refusing to remove an unexpected fixture directory');
        fs.rmSync(resolved, { recursive: true, force: true });
    }
}

module.exports = { cases, fixture, runSuite, verify };
