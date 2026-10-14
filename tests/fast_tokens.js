'use strict';
const fs = require('node:fs');
const path = require('node:path');
const { macroDefinition } = require('./source_macros');

// Extract a single exported probe, rather than comparing PE timestamps or
// unrelated polymorphic header bodies. Both PE32 and PE32+ use RVA export tables.
function exportedProbe(file, wanted = 'fast_probe', whole = false) {
    const b = Buffer.isBuffer(file) ? file : fs.readFileSync(file), pe = b.readUInt32LE(0x3c), opt = pe + 24;
    const directories = opt + (b.readUInt16LE(opt) === 0x20b ? 112 : 96);
    const sections = opt + b.readUInt16LE(pe + 20), count = b.readUInt16LE(pe + 6);
    function offset(rva) {
        for (let i = 0; i < count; ++i) {
            const at = sections + 40 * i, start = b.readUInt32LE(at + 12);
            if (rva >= start && rva < start + b.readUInt32LE(at + 16))
                return b.readUInt32LE(at + 20) + rva - start;
        }
        throw new Error(`unmapped RVA ${rva}`);
    }
    const exp = offset(b.readUInt32LE(directories));
    const names = offset(b.readUInt32LE(exp + 32)), ords = offset(b.readUInt32LE(exp + 36));
    const funcs = offset(b.readUInt32LE(exp + 28));
    for (let i = 0; i < b.readUInt32LE(exp + 24); ++i) {
        const name = offset(b.readUInt32LE(names + i * 4));
        if (b.toString('ascii', name, b.indexOf(0, name)).replace(/^_/, '') !== wanted) continue;
        const start = offset(b.readUInt32LE(funcs + b.readUInt16LE(ords + i * 2) * 4));
        if (whole) {
            const end = b.indexOf(Buffer.from([0xc9, 0xc3]), start);
            if (end < start || end > start + 256) throw new Error('isolated do probe epilogue missing');
            return b.subarray(start, end + 2);
        }
        // Skip the entire data region before looking for the epilogue: random
        // data is allowed to contain leave/ret or apparent LEA/jump encodings.
        let insertion = start;
        if (b[insertion] === 0x55) ++insertion;
        if (b[insertion] === 0x48) ++insertion;
        if (b[insertion] === 0x89 && b[insertion + 1] === 0xe5) insertion += 2;
        if (b[insertion] === 0x48) ++insertion;
        if (b[insertion] === 0x81 && b[insertion + 1] === 0xec) insertion += 6;
        if (b[insertion] === 0x90) ++insertion;
        for (let at = insertion; at < insertion + 10; ++at) {
            if (b[at] !== 0xeb || b[at + 1] < 8 || b[at + 1] > 15) continue;
            let end = at + 2 + b[at + 1];
            if (b[end] === 0x48 && b[end + 1] === 0x8d) end += 8;
            else if (b[end] === 0x8d) end += 6;
            if (!([0xc3, 0xc9, 0x5d].includes(b[end]))) continue;
            return b.subarray(insertion, end);
        }
        throw new Error('fast probe has no single short jump over its payload: ' + b.subarray(start, start + 64).toString('hex'));
    }
    throw new Error('fast_probe export missing');
}

exports.runSuite = async function ({ arch, compiler, directory, source, fixturesRoot = __dirname, check, compile, execute, run, assert }) {
    await check(`${arch}/fast transfer bytes, fixed-seed replay and semantic negative controls`, async () => {
        const root = path.join(directory, `${arch}-fast-transfer`);
        fs.mkdirSync(path.join(root, 'include'), { recursive: true });
        fs.mkdirSync(path.join(root, 'tests'), { recursive: true });
        const header = path.join(root, 'include', 'obfus.h');
        fs.writeFileSync(header, source);
        const probe = path.join(root, 'tests', 'probe.c');
        fs.writeFileSync(probe, `__declspec(dllexport) unsigned native_do(unsigned x) { unsigned sum = 0; do { sum += x; } while (--x); return sum; }
#include "../include/obfus.h"
#undef return
#undef if
#undef else
#undef case
#undef while
#undef for
__declspec(dllexport) void fast_probe(void) { OBFH_FLOW_FAST_TRANSFER; }
__declspec(dllexport) void fast_probe_second(void) { OBFH_FLOW_FAST_TRANSFER; }
__declspec(dllexport) unsigned protected_do(unsigned x) { unsigned sum = 0; do { sum += x; } while (--x); return sum; }
int main(void) { fast_probe(); fast_probe_second(); for (unsigned i = 1; i < 100; ++i) if (native_do(i) != protected_do(i)) return 1; puts("FAST_PROBE_PASS"); return 0; }
`);
        // Check the real include boundary, then compile only the production
        // primitives for the byte matrix. Unrelated VM/CRT bodies are not part
        // of the exported insertion under examination.
        const probeFlags = ['OBFH_DISABLE_ANTIDEBUG=1', 'OBFH_DISABLE_PDATA_DECOYS=1'];
        const fullBuilds = await Promise.allSettled(['first', 'repeat'].map(async name => {
            const file = await compile(compiler, directory, `${arch}-fast-full-${name}.exe`, probe,
                [...probeFlags, 'OBFH_SEED=0u']);
            await execute(file, 'FAST_PROBE_PASS');
            return file;
        }));
        for (const result of fullBuilds)
            if (result.status === 'rejected' && result.reason.suiteDeadline) throw result.reason;
        assert(fullBuilds.every(result => result.status === 'fulfilled'), fullBuilds.map((result, index) =>
            result.status === 'rejected' ? `full probe ${index}: ${result.reason.message}` : '').filter(Boolean).join('\n'));
        const [fullFirst, fullRepeat] = fullBuilds.map(result => result.value);
        const fullBytes = exportedProbe(fullFirst);
        const fullDoBytes = exportedProbe(fullFirst, 'protected_do', true);
        assert(fullBytes.equals(exportedProbe(fullRepeat)), 'full-header fast insertion replay changed');
        assert(fullDoBytes.equals(exportedProbe(fullRepeat, 'protected_do', true)), 'full-header do replay changed');
        const primitiveHeader = path.join(root, 'include', 'fast_primitives.h');
        const definition = name => macroDefinition(source, name).body;
        const start = source.indexOf('#define OBFH_FLOW_FAST_TRANSFER');
        const end = source.indexOf('#define if(', start);
        assert(start >= 0 && end > start, 'fast primitive boundaries missing');
        fs.writeFileSync(primitiveHeader, '#include <stdio.h>\n#define __obfh_asm__(...) __asm__ __volatile__(__VA_ARGS__)\n' +
            ['OBFH_MIX_A', 'OBFH_MIX_B', 'OBFH_MIX_FIELDS', 'OBFH_DATA_FIELDS'].map(definition).join('\n') +
            '\n' + source.slice(start, end));
        const primitiveProbe = path.join(root, 'tests', 'primitive_probe.c');
        fs.writeFileSync(primitiveProbe, fs.readFileSync(probe, 'utf8').replace('../include/obfus.h', '../include/fast_primitives.h'));
        const variants = [], lengths = new Set(), fastForms = new Set(), doForms = new Set(), doVariants = [], blobs = [], prefixLengths = [];
        const seeds = [0, 1, 2, 3, 4, 5, 6, 7, 0x10000, 0x10001, 0xdead0000, 0xdeadbeef, 0xffffffff, 0];
        const probes = await Promise.allSettled(seeds.map(async (seed, index) => {
            const file = await compile(compiler, directory, `${arch}-fast-probe-${index}.exe`, primitiveProbe,
                ['OBFH_DISABLE_ANTIDEBUG=1', 'OBFH_DISABLE_PDATA_DECOYS=1', `OBFH_SEED=${seed}u`]);
            await execute(file, 'FAST_PROBE_PASS');
            return file;
        }));
        for (const result of probes)
            if (result.status === 'rejected' && result.reason.suiteDeadline) throw result.reason;
        assert(probes.every(result => result.status === 'fulfilled'), probes.map((result, index) =>
            result.status === 'rejected' ? `probe ${index}/seed ${seeds[index]}: ${result.reason.message}` : '').filter(Boolean).join('\n'));
        // Preserve original seed order for replay comparisons and state blobs,
        // regardless of the order in which independent processes finish.
        for (const result of probes) {
            const file = result.value;
            const bytes = exportedProbe(file);
            assert(!bytes.equals(exportedProbe(file, 'fast_probe_second')), 'different sites share fast transfer bytes');
            variants.push(bytes.toString('hex'));
            lengths.add(bytes.length);
            assert(bytes.length >= 10 && bytes.length <= 25, 'fast transfer size budget exceeded');
            const jump = bytes[0] === 0xeb ? 0 : arch === 'x64' ? 8 : 6;
            assert(bytes[jump] === 0xeb, 'fast transfer has no single short jump');
            const payloadEnd = jump + 2 + bytes[jump + 1];
            const pair = jump ? bytes.subarray(0, jump) : bytes.subarray(payloadEnd);
            if (pair.length) {
                const width = arch === 'x64' ? 4 : 3, op = width === 4 ? 1 : 0;
                assert(pair.length === 2 * width, 'unbalanced LEA pair');
                assert((width !== 4 || pair[0] === 0x48 && pair[width] === 0x48) && pair[op] === 0x8d && pair[width + op] === 0x8d, 'LEA width/opcode incorrect');
                assert([0x40, 0x49, 0x52].includes(pair[op + 1]) && pair[op + 1] === pair[width + op + 1], 'LEA register not preserved');
                assert(pair.readInt8(op + 2) !== 0 && pair.readInt8(op + 2) === -pair.readInt8(width + op + 2), 'LEA displacements are not inverse');
                fastForms.add(`${jump ? 'before' : 'after'}/${pair[op + 1]}/${pair.readInt8(op + 2) > 0 ? 'positive' : 'negative'}`);
            } else fastForms.add('jump-only');
            // Execute the extracted insertion in a native wrapper, retaining
            // full-width values in all three possible registers and EFLAGS.
            // The x86 wrapper saves/restores its callee-saved EBX and EDI.
            const prefix = arch === 'x64'
                ? '4989c848b83f00000098badcfe48b9010000000000008048baf0ffffffffffffff39d09c4159'
                : '53578b7c240cb83f000000b901000000baf0ffffff39d09c5b';
            const suffix = arch === 'x64'
                ? '9c415a49890049894808498950104d8948184d895020c3'
                : '8907894f04895708895f0c9c588947105f5bc3';
            blobs.push(Buffer.concat([Buffer.from(prefix, 'hex'), bytes, Buffer.from(suffix, 'hex')]));
            prefixLengths.push(prefix.length / 2);
            const native = exportedProbe(file, 'native_do', true);
            const guarded = exportedProbe(file, 'protected_do', true);
            // ESP read, accumulator ADD/XOR, optional ROL, AND, OR and BSF/BSR.
            // Parse complete instructions, not arbitrary payload bytes or PE data.
            const live = guarded.toString('hex').match(/89e0(05|35)[0-9a-f]{8}(c1c0[0-9a-f]{2})?25[0-9a-f]{8}0d[0-9a-f]{8}0f(bc|bd)c0/);
            assert(live, 'do live scan instructions missing');
            assert(guarded.length - native.length <= 48, 'do guard exceeded isolated code-size budget');
            doForms.add(`${live[1]}/${!!live[2]}/${live[3]}`);
            doVariants.push(live[0]);
        }
        assert(variants[0] === variants.at(-1), 'fixed seed changed fast transfer bytes');
        assert(new Set(variants.slice(0, -1)).size === variants.length - 1, 'fast payload did not vary by seed');
        assert(lengths.size > 1, 'fast transfer spans did not vary');
        assert(fastForms.has('jump-only') && [...fastForms].some(x => x.startsWith('before/')) && [...fastForms].some(x => x.startsWith('after/')), 'fast transfer composition lacked placement diversity');
        assert(doForms.size >= 4, 'do composition did not cover independent mix/rotation/scan choices');
        assert(doVariants[0] === doVariants.at(-1), 'fixed seed changed do composition');
        const matrix = path.join(root, 'tests', 'matrix.c');
        fs.writeFileSync(matrix, '#include "../include/obfus.h"\n#undef return\n' +
            Array.from({ length: 512 }, (_, i) => `__declspec(dllexport) void matrix_${i}(void) { OBFH_FLOW_FAST_TRANSFER; }`).join('\n') + '\nint main(void) { return 0; }\n');
        const matrixBytes = fs.readFileSync(await compile(compiler, directory, `${arch}-fast-site-matrix.exe`, matrix,
            ['OBFH_DISABLE_ANTIDEBUG=1', 'OBFH_DISABLE_PDATA_DECOYS=1', 'OBFH_SEED=0u']));
        const matrixVariants = new Set(), matrixRegisters = new Set(), matrixOrders = new Set();
        for (let i = 0; i < 512; ++i) {
            const b = exportedProbe(matrixBytes, `matrix_${i}`), width = arch === 'x64' ? 4 : 3;
            assert(b.length >= 10 && b.length <= 25, 'matrix insertion size exceeded');
            matrixVariants.add(b.toString('hex'));
            const jump = b[0] === 0xeb ? 0 : 2 * width;
            assert(b[jump] === 0xeb, 'matrix short jump missing');
            const end = jump + 2 + b[jump + 1], pair = jump ? b.subarray(0, jump) : b.subarray(end);
            if (pair.length) {
                const op = width === 4 ? 1 : 0;
                assert(pair.length === 2 * width && pair[op] === 0x8d && pair[width + op] === 0x8d &&
                    (width !== 4 || pair[0] === 0x48 && pair[width] === 0x48) &&
                    [0x40, 0x49, 0x52].includes(pair[op + 1]) && pair[op + 1] === pair[width + op + 1] &&
                    pair.readInt8(op + 2) !== 0 && pair.readInt8(op + 2) === -pair.readInt8(width + op + 2), 'matrix LEA identity invalid');
                matrixRegisters.add(pair[op + 1]);
                matrixOrders.add(pair.readInt8(op + 2) > 0 ? 'positive' : 'negative');
            }
            const prefix = arch === 'x64' ? '4989c848b83f00000098badcfe48b9010000000000008048baf0ffffffffffffff39d09c4159'
                : '53578b7c240cb83f000000b901000000baf0ffffff39d09c5b';
            const suffix = arch === 'x64' ? '9c415a49890049894808498950104d8948184d895020c3' : '8907894f04895708895f0c9c588947105f5bc3';
            blobs.push(Buffer.concat([Buffer.from(prefix, 'hex'), b, Buffer.from(suffix, 'hex')]));
        }
        assert(matrixVariants.size === 512 && matrixRegisters.size === 3 && matrixOrders.size === 2, 'matrix lacks site/register/LEA order diversity');
        const stateFixture = path.join(root, 'tests', 'fast_state.c');
        const stateData = `static const unsigned char blobs[][128] = {\n${blobs.map(b => '{' + [...b].join(',') + '}').join(',\n')}\n};\nstatic const unsigned lengths[] = {${blobs.map(b => b.length).join(',')}};`;
        const stateSource = fs.readFileSync(path.join(fixturesRoot, 'fast_state.c.in'), 'utf8');
        fs.writeFileSync(stateFixture, stateSource.replace('/*FAST_BLOBS*/', stateData));
        await execute(await compile(compiler, directory, `${arch}-fast-register-state.exe`, stateFixture, []), 'FAST_STATE_PASS');
        const mutantBlobs = blobs.map(b => Buffer.from(b));
        const pairedIndex = variants.findIndex(v => {
            const b = Buffer.from(v, 'hex');
            return b[0] !== 0xeb || b.length > 2 + b[1];
        });
        assert(pairedIndex >= 0, 'no LEA state mutation target');
        const guardedBytes = Buffer.from(variants[pairedIndex], 'hex');
        const beforePair = guardedBytes[0] !== 0xeb;
        const prefixLength = prefixLengths[pairedIndex];
        const deltaOffset = prefixLength + (beforePair ? 0 : 2 + guardedBytes[1]) + (arch === 'x64' ? 3 : 2);
        mutantBlobs[pairedIndex][deltaOffset] ^= 1;
        const mutantData = `static const unsigned char blobs[][128] = {\n${mutantBlobs.map(b => '{' + [...b].join(',') + '}').join(',\n')}\n};\nstatic const unsigned lengths[] = {${mutantBlobs.map(b => b.length).join(',')}};`;
        fs.writeFileSync(stateFixture, stateSource.replace('/*FAST_BLOBS*/', mutantData));
        const changedState = await run(await compile(compiler, directory, `${arch}-fast-register-mutant.exe`, stateFixture, []), []);
        assert(changedState.status === 1 && !changedState.stdout.includes('FAST_STATE_PASS'), 'unbalanced LEA escaped state oracle');
    });

    // Independent contracts own immutable inputs. Mutants never overwrite a
    // header consumed by another queued compiler; the runner retains its RAM
    // and process limits, deadlines and one fresh build per original case.
    function prepare(name, text, files) {
        const root = path.join(directory, `${arch}-fast-independent-${name}`);
        fs.mkdirSync(path.join(root, 'include'), { recursive: true });
        fs.mkdirSync(path.join(root, 'tests'), { recursive: true });
        fs.writeFileSync(path.join(root, 'include', 'obfus.h'), text);
        for (const file of files)
            fs.copyFileSync(path.join(fixturesRoot, file), path.join(root, 'tests', file));
        return path.join(root, 'tests', files[0]);
    }
    for (const mode of [0, 1])
        await check(`${arch}/fast transfer bytes/do entry/cflow-v${mode + 1}`, async () => {
            const file = prepare('entry-' + mode, source, ['do_entry.c']);
            await execute(await compile(compiler, directory, `${arch}-do-entry-${mode}.exe`, file,
                ['OBFH_DISABLE_ANTIDEBUG=1', `OBFH_BETTER_CONTROL_FLOW=${mode}`]), 'DO_ENTRY_PASS');
        });
    for (const seed of [0, 1, 2, 0xdeadbeef, 0xffffffff])
        await check(`${arch}/fast transfer bytes/do scan/seed ${seed}`, async () => {
            const file = prepare('scan-' + seed, source, ['do_scan.c']);
            await execute(await compile(compiler, directory, `${arch}-do-scan-${seed}.exe`, file,
                ['OBFH_DISABLE_ANTIDEBUG=1', `OBFH_SEED=${seed}u`]), 'DO_SCAN_PASS');
        });
    for (const disabled of ['OBFH_DISABLE_CONTROL_FLOW=1', 'OBFH_DISABLE_ANY_OBFUSCATION=1'])
        await check(`${arch}/fast transfer bytes/disabled/${disabled}`, async () => {
            const file = prepare('disabled-' + disabled, source, ['fast_tokens.c', 'fast_tokens_body.h']);
            await execute(await compile(compiler, directory, `${arch}-fast-disabled-${disabled}.exe`, file,
                ['OBFH_DISABLE_ANTIDEBUG=1', disabled]), 'FAST_TOKENS_PASS');
        });
    for (const [kind, mutate, fixture, marker, diagnostic] of [
        ['do-scan-mutant', text => text.replace('0xbcu + __obfh_do_reverse', '0xbdu - __obfh_do_reverse'),
            'do_scan.c', 'DO_SCAN_PASS', ''],
        ['do-entry-bypass', text => text.replace(/#define do \\\r?\n\s*do \\\r?\n\s*if OBFH_FLOW_DO_GUARD/, '#define do do'),
            'do_entry.c', 'DO_ENTRY_PASS', ''],
        ['continue-target', text => text.replace('#define continue ({ OBFH_FLOW_FAST_TRANSFER; continue; })',
            '#define continue ({ OBFH_FLOW_FAST_TRANSFER; break; })'), 'fast_tokens.c', '', 'fast token failure'],
        ['default-dispatch', text => text.replace(/(#define default\s*(?:\\\r?\n\s*)?)default:/, '$1case 12345:'),
            'fast_tokens.c', '', 'fast token failure']
    ])
        await check(`${arch}/fast transfer bytes/negative control/${kind}`, async () => {
            const mutant = mutate(source);
            assert(mutant !== source, `${kind} mutation target missing`);
            const file = prepare(kind, mutant, fixture === 'fast_tokens.c'
                ? [fixture, 'fast_tokens_body.h'] : [fixture]);
            const result = await run(await compile(compiler, directory, `${arch}-fast-mutant-${kind}.exe`, file,
                ['OBFH_DISABLE_ANTIDEBUG=1']), []);
            assert(result.status === 1 && (!marker || !result.stdout.includes(marker)) && result.stderr.includes(diagnostic),
                `${kind} semantic regression escaped`);
        });
};
