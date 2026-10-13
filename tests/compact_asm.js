'use strict';
const fs = require('node:fs');
const os = require('node:os');
const path = require('node:path');
const { spawnSync } = require('node:child_process');

const begin = '// BEGIN COMPACT ASM CACHE';
const end = '// END COMPACT ASM CACHE';

// Fold only literal register indices. Captured entropy and bit selection stay intact.
function compactRegisters(text) {
    return text
        .replace(
            /\.short (0x[0-9a-f]+)\+256\*\(0xc0\+8\*(\d+)\+(\d+)\)\+\(\((%c\[\w+\])>>(\d+)\)&1\)\*\(2\+1792\*\((\d+)-(\d+)\)\);/g,
            (whole, opcode, src, dst, key, bit, deltaDst, deltaSrc) => {
                if (dst !== deltaDst || src !== deltaSrc) throw new Error('register index mismatch');
                return `.short ${Number(opcode) + 256 * (0xc0 + 8 * Number(src) + Number(dst))}+((${key}>>${bit})&1)*${2 + 1792 * (dst - src)};`;
            }
        )
        .replace(
            /\.short 0x39\+256\*\(0xc0\+8\*(\d+)\+(\d+)\)\+\(\((%c\[\w+\])>>(\d+)\)&1\)\*1792\*\((\d+)-(\d+)\);/g,
            (whole, src, dst, key, bit, deltaDst, deltaSrc) => {
                if (dst !== deltaDst || src !== deltaSrc) throw new Error('CMP index mismatch');
                return `.short ${0x39 + 256 * (0xc0 + 8 * Number(src) + Number(dst))}+((${key}>>${bit})&1)*${1792 * (dst - src)};`;
            }
        );
}

// Compact a preprocessed template without changing labels or operand numbering.
function compactFlowTemplate(expanded) {
    // Join adjacent literals and fold fixed register arithmetic only.
    let body = expanded.replace(/"(?:[^"\\]|\\.)*"(?:\s*"(?:[^"\\]|\\.)*")*/g, group => {
        const text = [...group.matchAll(/"(?:[^"\\]|\\.)*"/g)].map(part => JSON.parse(part[0])).join('');
        return JSON.stringify(compactRegisters(text).replace(/\s*([,;:])\s*/g, '$1'));
    });
    const asm = /^__obfh_asm__\(("(?:[^"\\]|\\.)*")\s*:\s*:\s*(.*?)\s*:\s*(.*?)\)$/.exec(body);
    if (asm) {
        const text = JSON.parse(asm[1]);
        const positional = [...text.matchAll(/%c?(\d+)/g)].map(value => Number(value[1]));
        const last = Math.max(-1, ...positional);
        const named = new Set([...text.matchAll(/%c?\[(\w+)\]/g)].map(value => value[1]));
        const fields = asm[2].split(',').map(value => value.trim());
        const parsed = fields.map(value => /^(?:\[(\w+)\]\s*)?"i"\(\w+\)$/.exec(value));
        if (parsed.every(Boolean)) {
            const selected = fields.filter((field, i) => i <= last || named.has(parsed[i][1]));
            if (named.size && [...named].some(name => !parsed.some(field => field[1] === name))) throw new Error('CFLOW operand missing');
            body = `__obfh_asm__(${asm[1]} : : ${selected.join(', ')} : ${asm[3]})`;
        }
    }
    return body;
}

// The wrapper consumes i0/i1/il even when the selected body does not.
function selectProxyInputs(expanded, inputs) {
    const text = [...expanded.matchAll(/"(?:[^"\\]|\\.)*"/g)].map(value => JSON.parse(value[0])).join('');
    const used = new Set(['i0', 'i1', 'il', ...[...text.matchAll(/%c?\[(\w+)\]/g)].map(value => value[1])]);
    for (const name of used) if (!inputs.some(input => input[1] === name)) throw new Error('unknown proxy operand: ' + name);
    const selected = inputs.filter(input => used.has(input[1])).map(input => `[${input[1]}] "i"(${input[2]})`);
    return selected.join(', ');
}

// Expand the readable ASM templates ahead of time; random choices stay at use sites.
// This developer tool is not part of building an application with obfus.h.
function generate(source, compilers) {
    const helpers = source.slice(source.indexOf('#define OBFH_SF_BYTES'), source.indexOf(begin));
    if (!helpers || !source.includes(end)) throw new Error('compact ASM source boundaries missing');
    const macros = new Map();
    for (const match of helpers.matchAll(/^#define (OBFH_SF_\w+)(\([^\n]*?\))?\s/gm)) {
        // Seed-selected fragments must remain selected by the consumer's build.
        if (match[1].includes('LAYOUT') || ['OBFH_SF_GUARD_8', 'OBFH_SF_GUARD_8_ORDER'].includes(match[1])) continue;
        macros.set(
            match[1],
            match[2]
                ? match[2]
                    .slice(1, -1)
                    .split(',')
                    .map((value) => value.trim())
                : null
        );
    }
    const shared = source.slice(source.indexOf('// BEGIN SHORT ASM FORMS'), source.indexOf('// END SHORT ASM FORMS'));
    let fixture = shared + '\n' + helpers + '\n';
    let index = 0;
    for (const [name, parameters] of macros) {
        const argumentsText = parameters
            ? '(' + parameters.map((value) => JSON.stringify('obfh_placeholder_' + value)).join(',') + ')'
            : '';
        fixture += `const char *compact_${index++} = ${name}${argumentsText};\n`;
    }
    // Restrict each variant to the immediates actually mentioned in its ASM.
    // Capture still draws every parameter once, preserving counters and entropy.
    const specs = [...source.matchAll(/^#define OBFH_SF_SPEC_(\d+)\(emit\) (.+)$/gm)];
    if (specs.length !== 128) throw new Error('proxy specifications incomplete');
    const inputStart = source.indexOf('#define OBFH_SF_INPUTS ');
    const inputEnd = source.indexOf('// All choices below', inputStart);
    const inputs = [...source.slice(inputStart, inputEnd).matchAll(/\[(\w+)\] "i"\((\w+)\)/g)];
    if (inputs.length !== 28) throw new Error('proxy input contract changed');
    fixture += specs.map(match => match[0]).join('\n') + '\n#define OBFH_COLLECT_INPUTS(guard, layout, inputs) guard layout\n';
    for (const match of specs) fixture += `const char *inputs_${match[1]} = OBFH_SF_SPEC_${match[1]}(OBFH_COLLECT_INPUTS);\n`;
    const flowStart = source.indexOf('#define OBFH_JUNK_RANDOM_INPUTS');
    const flowEnd = source.indexOf('#define BREAK_STACK_CFLOW_0 ');
    if (flowStart < 0 || flowEnd < flowStart) throw new Error('CFLOW source pool missing');
    fixture += source.slice(flowStart, flowEnd) + '\n';
    for (let i = 0; i < 128; ++i) fixture += `void flow_${i}(void) { OBFH_CFLOW_TEMPLATE_${i}; }\n`;
    const directory = fs.mkdtempSync(path.join(os.tmpdir(), 'obfh-compact-'));
    try {
        const file = path.join(directory, 'fragments.c');
        fs.writeFileSync(file, fixture);
        const blocks = [begin];
        for (const [arch, compiler] of Object.entries(compilers)) {
            const output = spawnSync(compiler, ['-w', '-E', file], {
                encoding: 'utf8',
                windowsHide: true,
                timeout: 30000,
                maxBuffer: 8 * 1024 * 1024
            });
            if (output.error || output.status !== 0) throw new Error(output.error?.message || output.stderr);
            const values = new Map(
                [...output.stdout.replace(/\r\n/g, '\n').matchAll(/^const char \*compact_(\d+) = (.*);$/gm)].map(
                    (match) => [Number(match[1]), match[2]]
                )
            );
            if (values.size !== macros.size) throw new Error('fragment expansion incomplete');
            blocks.push(arch === 'x64' ? '#if defined(__x86_64__)' : '#else');
            index = 0;
            for (const [name, parameters] of macros) {
                const value = values.get(index++);
                const literals = [...value.matchAll(/"(?:[^"\\]|\\.)*"/g)].map((match) => match[0]);
                if (value.replace(/"(?:[^"\\]|\\.)*"/g, '').trim()) continue; // x64-only helper on x86
                const literal = compactRegisters(literals.map((text) => JSON.parse(text)).join('')).replace(
                    /\s*([,;:])\s*/g,
                    '$1'
                );
                const body = literal
                    .split(/(obfh_placeholder_\w+)/)
                    .filter(Boolean)
                    .map((text) =>
                        text.startsWith('obfh_placeholder_')
                            ? text.slice('obfh_placeholder_'.length)
                            : JSON.stringify(text)
                    )
                    .join(' ');
                blocks.push(
                    '#undef ' + name,
                    '#define ' + name + (parameters ? '(' + parameters.join(', ') + ')' : '') + ' ' + body
                );
            }
            for (const match of output.stdout.matchAll(/^const char \*inputs_(\d+) = (.*);$/gm)) {
                const selected = selectProxyInputs(match[2], inputs);
                blocks.push(`#define OBFH_SF_INPUTS_${match[1]} ` + selected);
            }
            const flows = [...output.stdout.matchAll(/^void flow_(\d+)\(void\) \{ (.*); \}$/gm)];
            if (flows.length !== 128) throw new Error('CFLOW expansion incomplete');
            for (const match of flows) {
                const body = compactFlowTemplate(match[2]);
                blocks.push(`#undef OBFH_CFLOW_TEMPLATE_${match[1]}`, `#define OBFH_CFLOW_TEMPLATE_${match[1]} ${body}`);
            }
            blocks.push('');
        }
        blocks.push('#endif', end);
        return blocks.join('\n');
    } finally {
        const resolved = path.resolve(directory);
        if (
            path.dirname(resolved) !== path.resolve(os.tmpdir()) ||
            !path.basename(resolved).startsWith('obfh-compact-')
        )
            throw new Error('refusing to remove an unexpected temporary directory');
        fs.rmSync(resolved, { recursive: true, force: true });
    }
}

function canonical(text) {
    // Formatting changes outside strings are harmless. ASM string bytes are not.
    return text
        .replace(/\\\r?\n/g, ' ')
        .match(/"(?:[^"\\]|\\.)*"|[A-Za-z_]\w*|\d+|[^\s]/g)
        ?.join('\n');
}

function verify(source, compilers) {
    const first = source.indexOf(begin),
        last = source.indexOf(end);
    const expected = generate(source, compilers);
    if (canonical(source.slice(first, last + end.length)) !== canonical(expected))
        throw new Error('compact ASM cache differs from readable fragments; run node tests/compact_asm.js --write');
}

module.exports = { generate, verify };

if (require.main === module) {
    const root = path.resolve(__dirname, '..');
    const file = path.join(root, 'include', 'obfus.h');
    const explicit = process.argv.find((value) => value.startsWith('--tcc-dir='))?.slice(10);
    const desktop = process.env.OneDrive
        ? path.join(process.env.OneDrive, 'Рабочий стол')
        : path.join(os.homedir(), 'Desktop');
    const directory = explicit || process.env.TCC_DIR || path.join(desktop, 'tcc', 'tcc');
    const compilers = { x64: path.join(directory, 'tcc.exe'), x86: path.join(directory, 'i386-win32-tcc.exe') };
    const source = fs.readFileSync(file, 'utf8');
    if (process.argv.includes('--write')) {
        const first = source.indexOf(begin),
            last = source.indexOf(end);
        if (first < 0 || last < first) throw new Error('cache missing');
        const result = source.slice(0, first) + generate(source, compilers) + source.slice(last + end.length);
        fs.writeFileSync(file, result.replace(/\r\n/g, '\n').replace(/\n/g, '\r\n'));
    } else verify(source, compilers);
    console.log('COMPACT_ASM_PASS: equivalent fragments for x64/x86');
}
