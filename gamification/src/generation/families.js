// Constructors describe transformations only; model and C emission are independent.
export function families(width, bits, random, shuffle) {
    function mix(maximum = 3) {
        const kinds = ['xor', 'add', 'sub', 'rot', 'mul'];
        return Array.from({ length: 1 + random(maximum) }, () => {
            const kind = kinds[random(kinds.length)];
            return { kind, value: kind === 'rot' ? 1 + random(bits - 1) : kind === 'mul' ? 3 + 2 * random(7) : 1 + random(2 ** bits - 1) };
        });
    }
    function boxes() {
        return shuffle(width, random).map(i => ({
            i, before: mix(2), after: mix(2), route: shuffle(bits / 4, random), layout: random(3), tables: Array.from({ length: bits / 4 }, () => {
                for (let attempt = 0; attempt < 8; attempt++) {
                    const table = shuffle(16, random);
                    const affine = table.every((value, x) => {
                        let predicted = table[0];
                        for (let bit = 0; bit < 4; bit++) if (x & (1 << bit)) predicted ^= table[1 << bit] ^ table[0];
                        return predicted === value;
                    });
                    if (!affine) return table;
                }
                throw new Error('Could not generate a nonlinear bijective substitution');
            })
        }));
    }
    function pairs() {
        const ops = [];
        for (let layer = 0; layer < 2 + random(2); layer++) {
            const order = shuffle(width, random);
            for (let at = 0; at < width; at += 2) {
                const program = [], kinds = ['xor', 'add', 'sub', 'mul', 'rot'];
                // Each lift reads the other coordinate, which it leaves unchanged.
                // Alternating targets guarantees that both directions participate.
                const first = random(2), count = 3 + random(5);
                for (let step = 0; step < count; step++) {
                    const eligible = kinds.filter(kind => kind !== program.at(-1)?.kind);
                    const kind = eligible[random(eligible.length)];
                    const alternatives = kinds.filter(other => other !== kind);
                    program.push({ target: (first + step) % 2, kind, alternate: random(3) === 0 ? alternatives[random(alternatives.length)] : undefined, bit: random(bits), sourceMix: mix(2) });
                }
                ops.push({ i: order[at], j: order[at + 1], program, before: [mix(2), mix(2)], after: [mix(2), mix(2)] });
            }
        }
        return ops;
    }
    return {
        substitute: () => ({ kind: 'substitute', boxes: boxes() }),
        matrix: () => ({ kind: 'matrix', ops: pairs() }),
        spn: () => {
            const substitutions = boxes(), ops = pairs();
            // Interleave whole reversible steps; pair steps retain their own dependencies.
            const pendingBoxes = shuffle(substitutions.length, random), program = [];
            let pair = 0;
            while (pendingBoxes.length || pair < ops.length) {
                if (pendingBoxes.length && (pair === ops.length || random(2))) program.push({ box: pendingBoxes.pop() });
                else program.push({ pair: pair++ });
            }
            return { kind: 'spn', boxes: substitutions, ops, program };
        },
        controlled: () => ({
            kind: 'controlled', ops: Array.from({ length: width }, () => {
                const indices = shuffle(width, random);
                return { i: indices[0], j: indices[1], control: indices[2], bit: random(bits), gate: mix(), inverted: !!random(2), branchless: !!random(2) };
            })
        })
    };
}
