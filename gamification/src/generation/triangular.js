// Nonlinear updates of an upper bit band, controlled only by unchanged lower
// bits. Word rotations between updates change the dependency direction.
export function triangular(width, bits, random, shuffle) {
    return {
        kind: 'triangular',
        ops: shuffle(width, random).map(i => ({
            i, steps: Array.from({ length: 2 + random(bits === 8 ? 3 : 5) }, () => {
                const cut = 2 + random(bits - 3);
                return {
                    cut, rotation: 1 + random(bits - 1), kind: ['xor', 'add', 'sub'][random(3)],
                    term: ['product', 'square', 'fold'][random(3)], salt: 1 + random(2 ** cut - 1), shift: 1 + random(cut - 1)
                };
            })
        }))
    };
}

export function triangularModel(input, stage, bits, inverse) {
    const result = [...input], mask = 2 ** bits - 1;
    const rotate = (value, count) => ((value << count) | (value >>> (bits - count))) & mask;
    for (const op of stage.ops) {
        for (const step of inverse ? [...op.steps].reverse() : op.steps) {
            const x = inverse ? result[op.i] : rotate(result[op.i], step.rotation);
            const low = x & (2 ** step.cut - 1), highMask = 2 ** (bits - step.cut) - 1;
            const term = (step.term === 'product' ? low * (low ^ step.salt)
                : step.term === 'square' ? low * low + step.salt
                    : (low & (low >>> step.shift)) ^ step.salt) & highMask;
            const high = x >>> step.cut;
            const changed = step.kind === 'xor' ? high ^ term
                : (high + ((step.kind === 'add') !== inverse ? term : -term)) & highMask;
            const value = low | (changed << step.cut);
            result[op.i] = inverse ? rotate(value, bits - step.rotation) : value;
        }
    }
    return result;
}

// Emit the forward operation directly; the inverse interpreter is not used.
export function emitTriangular(stage, bits, slot) {
    return stage.ops.map(op => op.steps.map(step => {
        const destination = `s[${slot(op.i)}]`, lowMask = 2 ** step.cut - 1, highMask = 2 ** (bits - step.cut) - 1;
        const term = step.term === 'product' ? `p_mul(low,p_xor(low,${step.salt}u))`
            : step.term === 'square' ? `p_add(p_mul(low,low),${step.salt}u)`
                : `p_xor(p_and(low,p_shr(low,${step.shift}u)),${step.salt}u)`;
        return `{unsigned x=rot(${destination},${step.rotation}u),low=p_and(x,${lowMask}u);`
            + `unsigned term=p_and(${term},${highMask}u),high=p_and(p_${step.kind}(p_shr(x,${step.cut}u),term),${highMask}u);`
            + `${destination}=p_or(low,p_shl(high,${step.cut}u));}`;
    }).join('\n')).join('\n') + '\nobserve(s);';
}
