'use strict';
const fs = require('node:fs');
const os = require('node:os');
const path = require('node:path');
const { spawnSync } = require('node:child_process');

const begin = '// BEGIN COMPACT ASM CACHE';
const end = '// END COMPACT ASM CACHE';

// Use the preprocessor to expand only the string fragments, never the runtime.
// This developer tool is not part of building an application with obfus.h.
function generate(source, compilers) {
    const helpers = source.slice(source.indexOf('#define OBFH_SF_BYTES'), source.indexOf(begin));
    if (!helpers || !source.includes(end)) throw new Error('compact ASM source boundaries missing');
    const macros = new Map();
    for (const match of helpers.matchAll(/^#define (OBFH_SF_\w+)(\([^\n]*?\))?\s/gm)) {
        if (match[1].includes('LAYOUT')) continue;
        macros.set(match[1], match[2] ? match[2].slice(1, -1).split(',').map(value => value.trim()) : null);
    }
    let fixture = helpers + '\n';
    let index = 0;
    for (const [name, parameters] of macros) {
        const argumentsText = parameters ? '(' + parameters.map(value => JSON.stringify('obfh_placeholder_' + value)).join(',') + ')' : '';
        fixture += `const char *compact_${index++} = ${name}${argumentsText};\n`;
    }
    const directory = fs.mkdtempSync(path.join(os.tmpdir(), 'obfh-compact-'));
    try {
        const file = path.join(directory, 'fragments.c');
        fs.writeFileSync(file, fixture);
        const blocks = [begin];
        for (const [arch, compiler] of Object.entries(compilers)) {
            const output = spawnSync(compiler, ['-w', '-E', file], { encoding: 'utf8', windowsHide: true, timeout: 30000, maxBuffer: 8 * 1024 * 1024 });
            if (output.error || output.status !== 0) throw new Error(output.error?.message || output.stderr);
            const values = new Map([...output.stdout.replace(/\r\n/g, '\n').matchAll(/^const char \*compact_(\d+) = (.*);$/gm)].map(match => [Number(match[1]), match[2]]));
            if (values.size !== macros.size) throw new Error('fragment expansion incomplete');
            blocks.push(arch === 'x64' ? '#if defined(__x86_64__)' : '#else');
            index = 0;
            for (const [name, parameters] of macros) {
                const value = values.get(index++);
                const literals = [...value.matchAll(/"(?:[^"\\]|\\.)*"/g)].map(match => match[0]);
                if (value.replace(/"(?:[^"\\]|\\.)*"/g, '').trim()) continue; // x64-only helper on x86
                const literal = literals.map(text => JSON.parse(text)).join('');
                const body = literal.split(/(obfh_placeholder_\w+)/).filter(Boolean).map(text => text.startsWith('obfh_placeholder_') ? text.slice('obfh_placeholder_'.length) : JSON.stringify(text)).join(' ');
                blocks.push('#undef ' + name, '#define ' + name + (parameters ? '(' + parameters.join(', ') + ')' : '') + ' ' + body);
            }
            blocks.push('');
        }
        blocks.push('#endif', end);
        return blocks.join('\n');
    } finally {
        const resolved = path.resolve(directory);
        if (path.dirname(resolved) !== path.resolve(os.tmpdir()) || !path.basename(resolved).startsWith('obfh-compact-'))
            throw new Error('refusing to remove an unexpected temporary directory');
        fs.rmSync(resolved, { recursive: true, force: true });
    }
}

function canonical(text) {
    // Formatting changes outside strings are harmless. ASM string bytes are not.
    return text.replace(/\\\r?\n/g, ' ').match(/"(?:[^"\\]|\\.)*"|[A-Za-z_]\w*|\d+|[^\s]/g)?.join('\n');
}

function verify(source, compilers) {
    const first = source.indexOf(begin), last = source.indexOf(end);
    const expected = generate(source, compilers);
    if (canonical(source.slice(first, last + end.length)) !== canonical(expected)) throw new Error('compact ASM cache differs from readable fragments; run node tests/compact_asm.js --write');
}

module.exports = { generate, verify };

if (require.main === module) {
    const root = path.resolve(__dirname, '..');
    const file = path.join(root, 'include', 'obfus.h');
    const explicit = process.argv.find(value => value.startsWith('--tcc-dir='))?.slice(10);
    const desktop = process.env.OneDrive ? path.join(process.env.OneDrive, 'Рабочий стол') : path.join(os.homedir(), 'Desktop');
    const directory = explicit || process.env.TCC_DIR || path.join(desktop, 'tcc', 'tcc');
    const compilers = { x64: path.join(directory, 'tcc.exe'), x86: path.join(directory, 'i386-win32-tcc.exe') };
    const source = fs.readFileSync(file, 'utf8');
    if (process.argv.includes('--write')) {
        const first = source.indexOf(begin), last = source.indexOf(end);
        if (first < 0 || last < first) throw new Error('cache missing');
        const result = source.slice(0, first) + generate(source, compilers) + source.slice(last + end.length);
        fs.writeFileSync(file, result.replace(/\r\n/g, '\n').replace(/\n/g, '\r\n'));
    } else verify(source, compilers);
    console.log('COMPACT_ASM_PASS: equivalent fragments for x64/x86');
}
