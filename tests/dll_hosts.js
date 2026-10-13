'use strict';
const fs = require('node:fs');
const os = require('node:os');
const path = require('node:path');

const cases = [['dll_client', 'DLL_PASS'], ['multi_client', 'MULTI_DLL_PASS']];
const include = '#include "../include/obfus.h"';
const signature = 'int main(int argc, char **argv)';

function fixture(header, fixturesRoot = __dirname) {
    if (!path.isAbsolute(header)) throw new Error('DLL hosts require an absolute snapshot header');
    const units = cases.map(([name]) => {
        const source = fs.readFileSync(path.join(fixturesRoot, name + '.c'), 'utf8');
        if (source.split(include).length !== 2 || source.split(signature).length !== 2)
            throw new Error(name + ': DLL host protection boundary or entry point changed');
        if (/^\s*#\s*(define|undef|pragma)\b/m.test(source) || /^\s*(?:static|__declspec)\b/m.test(source))
            throw new Error(name + ': DLL host acquired state or macro mutations; keep it isolated');
        const prefix = source.slice(0, source.indexOf(include));
        const body = source.slice(source.indexOf(include) + include.length);
        const file = JSON.stringify(path.join(fixturesRoot, name + '.c').replaceAll('\\', '/'));
        return {
            prefix, body: '#line ' + prefix.split('\n').length + ' ' + file + '\n' +
                body.replace(signature, 'int ' + name + '_main(int argc, char **argv)')
        };
    });
    // Each client retains its protected entry body and runs in a fresh process.
    // DLLs, mutable exported data, loader state and cold caches never carry over.
    return units.map(unit => unit.prefix).join('\n') + '#include ' +
        JSON.stringify(header.replaceAll('\\', '/')) + '\n' + units.map(unit => unit.body).join('\n') + `
#undef if
#undef else
#undef case
#undef for
#undef while
#undef break
#undef continue
#undef return
int main(int argc, char **argv) {
    if (argc != 3) return 2;
${cases.map(([name]) => '    if (!(strcmp)(argv[1], "' + name + '")) return ' + name + '_main(argc - 1, argv + 1);').join('\n')}
    return 2;
}
`;
}

function builder({ directory, header, fixturesRoot, compile }) {
    const builds = new Map();
    const specifications = new Map();
    return (compiler, arch, config, flags) => {
        const key = JSON.stringify([compiler, arch, config, flags]);
        const name = `${arch}-${config}-dll-hosts`;
        if (specifications.has(name) && specifications.get(name) !== key)
            throw new Error('DLL host configuration changed after its build was registered');
        specifications.set(name, key);
        if (!builds.has(key)) {
            const file = path.join(directory, name + '.c');
            fs.writeFileSync(file, fixture(header, fixturesRoot));
            // Retain rejected promises: concurrent callers must observe the same
            // failed build instead of silently retrying or racing output files.
            builds.set(key, compile(compiler, directory, name + '.exe', file, flags));
        }
        return builds.get(key);
    };
}

async function verify(assert, header, fixturesRoot = __dirname) {
    const source = fixture(header, fixturesRoot);
    for (const [name, marker] of cases) {
        assert(source.includes('int ' + name + '_main(int argc, char **argv)'), 'DLL host entry missing: ' + name);
        assert(source.includes('"' + marker + '"'), 'DLL host marker missing: ' + name);
        const original = fs.readFileSync(path.join(fixturesRoot, name + '.c'), 'utf8');
        const body = original.slice(original.indexOf(include) + include.length).replace(signature,
            'int ' + name + '_main(int argc, char **argv)');
        assert(source.includes(body), 'DLL host changed a client assertion or loader operation: ' + name);
    }
    assert(source.includes('_main(argc - 1, argv + 1)'), 'DLL host does not preserve client arguments');
    assert(source.includes('if (argc != 3) return 2;'), 'DLL host accepts invalid argument count');
    const directory = fs.mkdtempSync(path.join(os.tmpdir(), 'obfh-dll-host-contract-'));
    try {
        for (const [name] of cases)
            fs.copyFileSync(path.join(fixturesRoot, name + '.c'), path.join(directory, name + '.c'));
        const file = path.join(directory, 'dll_client.c');
        const original = fs.readFileSync(file, 'utf8');
        for (const mutation of [original.replace(include, ''), original.replace(signature, 'int renamed(void)'),
        original + '\n#undef if\n', original + '\nstatic int shared_state;\n']) {
            fs.writeFileSync(file, mutation);
            let rejected = false;
            try { fixture(header, directory); } catch (_) { rejected = true; }
            assert(rejected, 'DLL host silently accepted an incompatible fixture');
        }
        fs.writeFileSync(file, original);
        let builds = 0;
        const build = builder({
            directory, header, fixturesRoot: directory, compile: async () => {
                builds++;
                throw new Error('intentional compiler failure');
            }
        });
        const first = build('compiler', 'x64', 'contract', []);
        const second = build('compiler', 'x64', 'contract', []);
        assert(first === second, 'DLL host concurrent callers did not share a build');
        const results = await Promise.allSettled([first, second]);
        assert(builds === 1 && results.every(result => result.status === 'rejected'),
            'DLL host retried or hid a failed shared build');
        let changed = false;
        try { build('compiler', 'x64', 'contract', ['OBFH_DISABLE_ANY_OBFUSCATION=1']); } catch (_) { changed = true; }
        assert(changed, 'DLL host silently reused an incompatible configuration');
    } finally {
        const resolved = path.resolve(directory);
        if (path.dirname(resolved) !== path.resolve(os.tmpdir()) ||
            !path.basename(resolved).startsWith('obfh-dll-host-contract-'))
            throw new Error('refusing to remove an unexpected DLL host directory');
        fs.rmSync(resolved, { recursive: true, force: true });
    }
}

module.exports = { cases, fixture, builder, verify };
