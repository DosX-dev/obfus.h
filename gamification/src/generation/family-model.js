// Inversion uses independently derived pair formulas, not generated C helpers.
function rotate(value, count, bits) {
    const mask = (1n << BigInt(bits)) - 1n, x = BigInt(value);
    return Number(((x << BigInt(count)) | (x >> BigInt(bits - count))) & mask);
}
function oddInverse(value, bits) {
    let a = BigInt(value), b = 1n << BigInt(bits), x = 1n, y = 0n;
    const modulus = b;
    while (b) {
        const q = a / b;
        [a, b] = [b, a % b];
        [x, y] = [y, x - q * y];
    }
    return Number((x % modulus + modulus) % modulus);
}
export function familyStage(input, stage, bits, inverse) {
    const out = [...input], mask = 2 ** bits - 1;
    function mix(value, operations = [], undo = inverse) {
        for (const op of undo ? [...operations].reverse() : operations) {
            if (op.kind === 'xor') value ^= op.value;
            else if (op.kind === 'rot') value = rotate(value, undo ? bits - op.value : op.value, bits);
            else if (op.kind === 'mul') value = Number(BigInt(value) * BigInt(undo ? oddInverse(op.value, bits) : op.value) & BigInt(mask));
            else if (op.kind === 'add') value = (value + (undo ? -op.value : op.value)) & mask;
            else if (op.kind === 'sub') value = (value + (undo ? op.value : -op.value)) & mask;
            else throw new Error('Unknown family word operation');
        }
        return value;
    }
    function substitutions(boxes = stage.boxes) {
        for (const box of boxes) {
            const current = mix(out[box.i], inverse ? box.after : box.before);
            let value = 0;
            box.tables.forEach((table, n) => {
                const destination = box.route?.[n] ?? n;
                const digit = (current >>> (4 * (inverse ? destination : n))) & 15;
                value |= (inverse ? table.indexOf(digit) : table[digit]) << (4 * (inverse ? n : destination));
            });
            out[box.i] = mix(value, inverse ? box.before : box.after);
        }
    }
    function pairs(operations = stage.ops) {
        for (const op of inverse ? [...operations].reverse() : operations) {
            const enter = inverse ? op.after : op.before;
            out[op.i] = mix(out[op.i], enter?.[0]);
            out[op.j] = mix(out[op.j], enter?.[1]);
            if (op.program) {
                const coordinates = [op.i, op.j];
                for (const step of inverse ? [...op.program].reverse() : op.program) {
                    const destination = coordinates[step.target];
                    const control = mix(out[coordinates[1 - step.target]], step.sourceMix, false);
                    const value = out[destination];
                    const kind = step.alternate && (control & (1 << step.bit)) ? step.alternate : step.kind;
                    if (kind === 'xor') out[destination] = value ^ control;
                    else if (kind === 'add') out[destination] = (value + (inverse ? -control : control)) & mask;
                    else if (kind === 'sub') out[destination] = (value + (inverse ? control : -control)) & mask;
                    else if (kind === 'mul') {
                        const factor = 1 + 2 * (control & 7);
                        out[destination] = Number(BigInt(value) * BigInt(inverse ? oddInverse(factor, bits) : factor) & BigInt(mask));
                    } else if (kind === 'rot') {
                        const count = 1 + (control & (bits / 2 - 1));
                        out[destination] = rotate(value, inverse ? bits - count : count, bits);
                    } else throw new Error('Unknown pair program operation');
                }
            } else throw new Error('Pair program missing');
            const leave = inverse ? op.before : op.after;
            out[op.i] = mix(out[op.i], leave?.[0]);
            out[op.j] = mix(out[op.j], leave?.[1]);
        }
    }
    if (stage.kind === 'bitshuffle') {
        for (const op of stage.ops) {
            const current = mix(out[op.i], inverse ? op.after : op.before);
            let value = 0;
            op.route.forEach((destination, source) => {
                value |= ((current >>> (inverse ? destination : source)) & 1) << (inverse ? source : destination);
            });
            out[op.i] = mix(value, inverse ? op.before : op.after);
        }
    } else if (stage.kind === 'weave') {
        for (const op of inverse ? [...stage.ops].reverse() : stage.ops) {
            let control = 0;
            for (const source of op.sources) {
                const term = mix(out[source.index], source.mix, false);
                control = source.combine === 'xor' ? control ^ term : (control + (source.combine === 'add' ? term : -term)) & mask;
            }
            const value = out[op.i];
            if (op.kind === 'xor') out[op.i] = value ^ control;
            else if (op.kind === 'add' || op.kind === 'sub') out[op.i] = (value + ((op.kind === 'add') !== inverse ? control : -control)) & mask;
            else if (op.kind === 'mul') {
                const factor = 1 + 2 * (control & 7);
                out[op.i] = Number(BigInt(value) * BigInt(inverse ? oddInverse(factor, bits) : factor) & BigInt(mask));
            } else if (op.kind === 'rot') {
                const count = 1 + (control & (bits / 2 - 1));
                out[op.i] = rotate(value, inverse ? bits - count : count, bits);
            } else throw new Error('Unknown weave operation');
        }
    } else if (stage.kind === 'substitute') substitutions();
    else if (stage.kind === 'matrix') pairs();
    else if (stage.kind === 'spn') {
        if (stage.program) {
            for (const step of inverse ? [...stage.program].reverse() : stage.program) {
                if (step.box !== undefined) substitutions([stage.boxes[step.box]]);
                else pairs([stage.ops[step.pair]]);
            }
        } else throw new Error('SPN program missing');
    }
    else if (stage.kind === 'controlled') {
        for (const op of inverse ? [...stage.ops].reverse() : stage.ops) {
            const active = !!(mix(out[op.control], op.gate, false) & (1 << op.bit));
            if (active !== !!op.inverted) [out[op.i], out[op.j]] = [out[op.j], out[op.i]];
        }
    } else throw new Error('Unknown reversible family');
    return out;
}
