import assert from 'node:assert/strict';
import { run } from '../core/process.js';
import { hash } from '../core/crypto.js';
import { ALPHABET } from '../core/config.js';
import { words, transform } from '../generation/model.js';

export function samples(challenge) {
    const p = challenge.password,
        values = [p, '', p.slice(1), p + '!', p.repeat(2), ' '.repeat(p.length)];
    for (let i = 0; i < p.length; i++)
        for (const c of ALPHABET) if (p[i] !== c) values.push(p.slice(0, i) + c + p.slice(i + 1));
    for (let i = 0; i + 1 < p.length; i++)
        if (p[i] !== p[i + 1]) values.push(p.slice(0, i) + p[i + 1] + p[i] + p.slice(i + 2));
    for (let n = 0; n < 1024; n++) {
        const bytes = Buffer.from(hash(`negative/${n}`), 'hex');
        const s = Array.from({ length: p.length }, (_, i) => ALPHABET[bytes[i % 32] % ALPHABET.length]).join('');
        if (s !== p) values.push(s);
    }
    return values;
}
export async function verifyProbe(challenge, exe, { mutation = false } = {}) {
    const inputs = mutation ? [challenge.password] : samples(challenge);
    const result = await run(exe, [], { input: inputs.join('\n') + '\n', timeout: 180000 });
    assert.equal(result.code, 0, 'Probe exit status');
    const answers = result.stdout
        .trim()
        .split(/\r?\n/)
        .map((line) => /^RESULT ([01]) (\d+)$/.exec(line));
    assert.equal(answers.length, inputs.length, 'All inputs must be processed');
    for (let i = 0; i < inputs.length; i++) {
        assert(answers[i], 'Malformed probe output');
        assert.equal(Number(answers[i][1]), !mutation && inputs[i] === challenge.password ? 1 : 0, `Input case ${i}`);
    }
    if (!mutation && challenge.ir.level === 'hard')
        assert(Number(answers[0][2]) > 100, 'Real VM instructions must execute');
    if (!mutation) {
        const trace = await run(exe, [], { input: `T ${challenge.password}\n`, timeout: 30000 });
        assert.equal(trace.code, 0);
        const lines = trace.stdout.trim().split(/\r?\n/);
        assert.match(lines.pop(), /^RESULT 1 /);
        assert.deepEqual(
            lines.map((line) => line.split(' ').map(Number)),
            transform(challenge.ir, words(challenge.password, challenge.ir.bits), false, true)
        );
    }
    return { cases: inputs.length, vmSteps: Number(answers[0][2]) };
}
export async function verifyBytes(challenge, exe) {
    const original = Buffer.from(challenge.password, 'ascii'), cases = [original, Buffer.alloc(0), Buffer.concat([original, Buffer.from([0])])];
    for (let i = 0; i < original.length; i++) {
        for (let value = 0; value < 256; value++) {
            if (value === original[i]) continue;
            const input = Buffer.from(original);
            input[i] = value;
            cases.push(input);
        }
    }
    const result = await run(exe, [], { input: cases.map(input => 'H ' + input.toString('hex')).join('\n') + '\n', timeout: 180000 });
    assert.equal(result.code, 0);
    const lines = result.stdout.trim().split(/\r?\n/);
    assert.equal(lines.length, cases.length);
    for (let i = 0; i < cases.length; i++) assert.match(lines[i], i === 0 ? /^RESULT 1 \d+$/ : /^RESULT 0 \d+$/);
    return cases.length;
}
export async function verifyStates(challenge, exe, { coordinateMutation = false } = {}) {
    const mask = 2 ** challenge.ir.bits - 1;
    const inputs = [Array(challenge.ir.width).fill(0), Array(challenge.ir.width).fill(mask), Array.from({ length: challenge.ir.width }, (_, i) => i % 2 ? mask : 0), ...['A', 'Z', '9'].map(char => words(char.repeat(challenge.ir.length), challenge.ir.bits))];
    if (coordinateMutation) {
        const changed = [...challenge.target];
        changed[0] ^= 1;
        inputs.splice(0, inputs.length, transform(challenge.ir, changed, true));
    }
    for (const input of inputs) {
        const bytes = Buffer.alloc(challenge.ir.length);
        input.forEach((value, i) => challenge.ir.bits === 8 ? bytes.writeUInt8(value, i) : bytes.writeUInt16LE(value, i * 2));
        const result = await run(exe, [], { input: 'B ' + bytes.toString('hex') + '\n', timeout: 30000 });
        assert.equal(result.code, 0);
        const lines = result.stdout.trim().split(/\r?\n/), verdict = lines.pop();
        assert.deepEqual(lines.map(line => line.split(' ').map(Number)), transform(challenge.ir, input, false, true));
        if (coordinateMutation) assert.match(verdict, /^RESULT 1 /, 'Missing coordinate must be detected by a false acceptance');
        else assert.match(verdict, JSON.stringify(input) === JSON.stringify(words(challenge.password, challenge.ir.bits)) ? /^RESULT 1 / : /^RESULT 0 /);
    }
    return inputs.length;
}
