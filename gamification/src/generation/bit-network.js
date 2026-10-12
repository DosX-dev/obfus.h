// Reversible circuits over individual wires, rather than independently invertible words.
// A gate never changes its controls, so reversing gate order is an exact inverse.
export function bitNetwork(width, bits, span, layers, random, shuffle) {
    const coordinates = shuffle(width, random), gates = [];
    for (let bank = 0; bank < width; bank += span) {
        const wires = coordinates.slice(bank, bank + span).flatMap(word => Array.from({ length: bits }, (_, bit) => word * bits + bit));
        for (let layer = 0; layer < layers; layer++) {
            for (const target of shuffle(wires.length, random).map(index => wires[index])) {
                const swap = random(4) === 0;
                const others = [], count = 2 + random(3) + (swap ? 1 : 0);
                // Sample a handful of distinct wires; do not shuffle a full block for every gate.
                while (others.length < count) {
                    const wire = wires[random(wires.length)];
                    const firstControl = others.length === (swap ? 1 : 0);
                    if (wire !== target && !others.includes(wire)
                        && !(span > 1 && firstControl && Math.floor(wire / bits) === Math.floor(target / bits))) others.push(wire);
                }
                const destination = swap ? others.shift() : undefined;
                const controls = others;
                let truth;
                for (let attempt = 0; attempt < 32; attempt++) {
                    truth = Array.from({ length: 2 ** controls.length }, () => random(2));
                    // Reject affine/constant functions and functions ignoring a control.
                    const nonlinear = truth.some((value, input) => {
                        let predicted = truth[0];
                        controls.forEach((_, bit) => { if (input & (1 << bit)) predicted ^= truth[1 << bit] ^ truth[0]; });
                        return predicted !== value;
                    });
                    if (nonlinear && controls.every((_, bit) => truth.some((value, input) => value !== truth[input ^ (1 << bit)]))) break;
                    truth = undefined;
                }
                if (!truth) throw new Error('Could not generate a nonlinear bit gate');
                gates.push({ target, destination, controls, truth });
            }
        }
    }
    return { kind: 'bitnetwork', span, layers, gates };
}

export function networkModel(input, stage, bits, inverse) {
    const result = [...input];
    const read = wire => (result[Math.floor(wire / bits)] >>> (wire % bits)) & 1;
    const flip = wire => { result[Math.floor(wire / bits)] ^= 1 << (wire % bits); };
    for (const gate of inverse ? [...stage.gates].reverse() : stage.gates) {
        let index = 0;
        gate.controls.forEach((wire, bit) => { index += read(wire) * 2 ** bit; });
        if (!gate.truth[index]) continue;
        if (gate.destination === undefined) flip(gate.target);
        else if (read(gate.target) !== read(gate.destination)) {
            flip(gate.target);
            flip(gate.destination);
        }
    }
    return result;
}

export function emitNetwork(stage, bits, slot) {
    const word = wire => `s[${slot(Math.floor(wire / bits))}]`;
    const read = wire => `p_and(p_shr(${word(wire)},${wire % bits}u),1u)`;
    return stage.gates.map(gate => {
        // Independently derive an algebraic normal form from the truth table.
        const coefficients = [...gate.truth];
        for (let bit = 0; bit < gate.controls.length; bit++) for (let index = 0; index < coefficients.length; index++) {
            if (index & (1 << bit)) coefficients[index] ^= coefficients[index ^ (1 << bit)];
        }
        const terms = coefficients.flatMap((coefficient, index) => {
            if (!coefficient) return [];
            const factors = gate.controls.flatMap((_, bit) => index & (1 << bit) ? [`c${bit}`] : []);
            return [factors.length ? factors.reduce((left, right) => `p_and(${left},${right})`) : '1u'];
        });
        const expression = terms.reduce((left, right) => `p_xor(${left},${right})`, '0u');
        const declarations = gate.controls.map((wire, bit) => `unsigned c${bit}=${read(wire)};`).join('');
        const delta = gate.destination === undefined ? expression : `p_and(${expression},p_xor(${read(gate.target)},${read(gate.destination)}))`;
        const targets = [gate.target, ...(gate.destination === undefined ? [] : [gate.destination])];
        return `{${declarations}unsigned delta=${delta};${targets.map(wire => `${word(wire)}=p_xor(${word(wire)},p_shl(delta,${wire % bits}u));`).join('')}}`;
    }).join('\n') + '\nobserve(s);';
}
