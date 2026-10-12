// Independent interpreter/inverse of the IR. The C emitter does not call these operations.
import { familyStage } from './family-model.js';
export function rotate(x, n, bits) {
    const mask = 2 ** bits - 1;
    return ((x << n) | (x >>> (bits - n))) & mask;
}
function inverseOdd(a, modulus) {
    let t = 0,
        next = 1,
        r = modulus,
        nr = a;
    while (nr) {
        const q = Math.floor(r / nr);
        [t, next] = [next, t - q * next];
        [r, nr] = [nr, r - q * nr];
    }
    if (r !== 1) throw new Error('Non-invertible multiplier');
    return ((t % modulus) + modulus) % modulus;
}
function evaluate(node, right, bits = 16, refs = []) {
    const mask = 2 ** bits - 1;
    if (node.kind === 'source') return right[node.index];
    if (node.kind === 'ref') return refs[node.index];
    if (node.kind === 'select') return evaluate(right[node.selector] & node.value ? node.left : node.right, right, bits, refs);
    const x = evaluate(node.left, right, bits, refs);
    if (node.kind === 'add') return (x + evaluate(node.right, right, bits, refs)) & mask;
    if (node.kind === 'xor') return x ^ evaluate(node.right, right, bits, refs);
    if (node.kind === 'and') return x & evaluate(node.right, right, bits, refs);
    if (node.kind === 'or') return x | evaluate(node.right, right, bits, refs);
    if (node.kind === 'sub') return (x - evaluate(node.right, right, bits, refs)) & mask;
    if (node.kind === 'rot') return rotate(x, node.value, bits);
    if (node.kind === 'mul') return (x * node.value) & mask;
    if (node.kind === 'constant') return (x + node.value) & mask;
    if (node.kind === 'xorConstant') return x ^ node.value;
    throw new Error('Unknown round expression');
}
// The controlling term never reads the destination. Undoing an update therefore
// sees the exact same term; sequential updates are undone in reverse order.
function update(value, term, form, bits, inverse) {
    const mask = 2 ** bits - 1;
    if (form === 0) return value ^ term;
    if (form === 1 || form === 2) return (value + ((form === 1) !== inverse ? term : -term)) & mask;
    if (form === 3) {
        const factor = 1 + 2 * (term & 7);
        return (value * (inverse ? inverseOdd(factor, mask + 1) : factor)) & mask;
    }
    if (form === 4) {
        const count = 1 + (term & 7);
        return rotate(value, inverse ? bits - count : count, bits);
    }
    throw new Error('Unknown update form');
}
function shiftXor(value, count, bits, left, inverse) {
    const mask = 2 ** bits - 1;
    if (!inverse) return (value ^ (left ? value << count : value >>> count)) & mask;
    let result = value;
    for (let shift = count; shift < bits; shift += count) result ^= left ? value << shift : value >>> shift;
    return result & mask;
}
export function stage(input, s, bits, inverse = false) {
    if (['substitute', 'matrix', 'spn', 'controlled'].includes(s.kind)) return familyStage(input, s, bits, inverse);
    const out = [...input],
        mask = 2 ** bits - 1,
        mod = mask + 1;
    if (s.kind === 'permute') {
        s.order.forEach((source, i) => {
            if (inverse) out[source] = input[i];
            else out[i] = input[source];
        });
    } else if (s.kind === 'affine') {
        for (const c of s.ops) {
            const operations = c.sequence.map(op => [op.kind, op.value]);
            for (const [kind, value] of inverse ? [...operations].reverse() : operations) {
                if (kind === 'xor') out[c.i] ^= value;
                if (kind === 'add') out[c.i] = (out[c.i] + (inverse ? mod - value : value)) & mask;
                if (kind === 'sub') out[c.i] = (out[c.i] + (inverse ? value : mod - value)) & mask;
                if (kind === 'mul') out[c.i] = (out[c.i] * (inverse ? inverseOdd(value, mod) : value)) & mask;
                if (kind === 'rot') out[c.i] = rotate(out[c.i], inverse ? bits - value : value, bits);
                if (kind === 'shlXor' || kind === 'shrXor') out[c.i] = shiftXor(out[c.i], value, bits, kind === 'shlXor', inverse);
            }
        }
    } else if (s.kind === 'couple') {
        for (const c of inverse ? [...s.ops].reverse() : s.ops) {
            const term = evaluate(c.expr, out, bits);
            out[c.i] = update(out[c.i], term, c.form, bits, inverse);
        }
    } else if (['feistel', 'braid'].includes(s.kind)) {
        const state = [...input];
        if (inverse) s.order.forEach((source, i) => { state[source] = input[i]; });
        const sources = s.sources.map(i => state[i]);
        const refs = [];
        for (const node of s.nodes || []) refs.push(evaluate(node, sources, bits, refs));
        s.targets.forEach((destination, i) => {
            state[destination] = update(state[destination], evaluate(s.ops[i].expr, sources, bits, refs), s.ops[i].form, bits, inverse);
        });
        return inverse ? state : s.order.map(source => state[source]);
    } else throw new Error('Unknown IR stage');
    return out;
}
export function words(text, bits) {
    const b = Buffer.from(text, 'ascii');
    return bits === 8 ? [...b] : Array.from({ length: b.length / 2 }, (_, i) => b.readUInt16LE(2 * i));
}
export function transform(ir, input, inverse = false, trace = false) {
    let result = [...input];
    const states = [];
    for (const s of inverse ? [...ir.stages].reverse() : ir.stages) {
        result = stage(result, s, ir.bits, inverse);
        states.push([...result]);
    }
    return trace ? states : result;
}
