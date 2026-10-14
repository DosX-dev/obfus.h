// Forward C emission is independent of the inverse model. Hardcore arithmetic uses VM helpers.
export function emitFamily(stage, bits, slot) {
    const v = i => `s[${slot(i)}]`, mask = 2 ** bits - 1;
    function mix(destination, operations = []) {
        return operations.map(op => {
            let expression;
            if (op.kind === 'shlXor' || op.kind === 'shrXor') {
                expression = `bounded(p_xor(${destination},p_${op.kind === 'shlXor' ? 'shl' : 'shr'}(${destination},${op.value}u)))`;
            } else if (op.kind === 'swapbits') {
                let low = 0;
                for (let bit = 0; bit < bits; bit++) if (!(bit & op.value)) low |= 1 << bit;
                expression = `p_or(p_shl(p_and(${destination},${low}u),${op.value}u),p_and(p_shr(${destination},${op.value}u),${low}u))`;
            } else expression = op.kind === 'rot' ? `rot(${destination},${op.value}u)` : `bounded(p_${op.kind}(${destination},${op.value}u))`;
            return `${destination}=${expression};`;
        }).join('');
    }
    function boxCode(box) {
        if (box.layout === 2) {
            // Boolean Mobius transform: emit the generated truth table as a circuit, without lookup data.
            const expressions = box.tables.map((table, n) => {
                const digit = `nibble${n}`, output = [];
                for (let bit = 0; bit < 4; bit++) {
                    const coefficients = table.map(value => (value >>> bit) & 1);
                    for (let input = 0; input < 4; input++) for (let term = 0; term < 16; term++) {
                        if (term & (1 << input)) coefficients[term] ^= coefficients[term ^ (1 << input)];
                    }
                    const terms = coefficients.flatMap((coefficient, term) => {
                        if (!coefficient) return [];
                        const factors = [];
                        for (let input = 0; input < 4; input++) if (term & (1 << input)) factors.push(`p_and(p_shr(${digit},${input}u),1u)`);
                        return [factors.length ? factors.reduce((left, right) => `p_and(${left},${right})`) : '1u'];
                    });
                    const polynomial = terms.reduce((left, right) => `p_xor(${left},${right})`, '0u');
                    output.push(`y=p_or(y,p_shl(${polynomial},${4 * (box.route?.[n] ?? n) + bit}u));`);
                }
                return `unsigned ${digit}=p_and(p_shr(x,${4 * n}u),15u);${output.join('')}`;
            });
            return `{unsigned x=${v(box.i)},y=0u;${mix('x', box.before)}${expressions.join('')}${mix('y', box.after)}${v(box.i)}=y;}`;
        }
        const declarations = box.layout ? box.tables.map((table, n) => `static const unsigned char box${n}[]={${table.join(',')}};`).join('') : `static const unsigned char box[]={${box.tables.flat().join(',')}};`;
        const terms = box.tables.map((_, n) => {
            const digit = `p_and(p_shr(x,${4 * n}u),15u)`;
            const lookup = box.layout ? `box${n}[${digit}]` : `box[p_add(${16 * n}u,${digit})]`;
            return `y=p_or(y,p_shl(${lookup},${4 * (box.route?.[n] ?? n)}u));`;
        });
        return `{${declarations}unsigned x=${v(box.i)},y=0u;${mix('x', box.before)}${terms.join('')}${mix('y', box.after)}${v(box.i)}=y;}`;
    }
    function pairCode(op) {
        const a = v(op.i), b = v(op.j), before = mix(a, op.before?.[0]) + mix(b, op.before?.[1]), after = mix(a, op.after?.[0]) + mix(b, op.after?.[1]);
        let body;
        if (op.program) body = op.program.map(step => {
            const destination = step.target ? b : a, source = step.target ? a : b;
            function expressionFor(kind) {
                const control = kind === 'mul' ? 'p_or(p_shl(p_and(control,7u),1u),1u)' : 'control';
                return kind === 'rot' ? `rot(${destination},p_add(p_and(control,${bits / 2 - 1}u),1u))` : `bounded(p_${kind}(${destination},${control}))`;
            }
            const expression = step.alternate ? `(p_equ(p_and(control,${1 << step.bit}u),0u)?${expressionFor(step.kind)}:${expressionFor(step.alternate)})` : expressionFor(step.kind);
            return `{unsigned control=${source};${mix('control', step.sourceMix)}${destination}=${expression};}`;
        }).join('');
        else throw new Error('Pair program missing');
        return before + body + after;
    }
    const lines = [];
    if (stage.kind === 'bitshuffle') {
        for (const op of stage.ops) {
            const moves = op.route.map((destination, source) => `y=p_or(y,p_shl(p_and(p_shr(x,${source}u),1u),${destination}u));`).join('');
            lines.push(`{unsigned x=${v(op.i)},y=0u;${mix('x', op.before)}${moves}${mix('y', op.after)}${v(op.i)}=y;}`);
        }
        return lines.join('\n') + '\nobserve(s);';
    }
    if (stage.kind === 'weave') {
        for (const op of stage.ops) {
            const sources = op.sources.map(source => `{unsigned term=${v(source.index)};${mix('term', source.mix)}control=bounded(p_${source.combine}(control,term));}`).join('');
            const value = op.kind === 'rot' ? `rot(${v(op.i)},p_add(p_and(control,${bits / 2 - 1}u),1u))` : `bounded(p_${op.kind}(${v(op.i)},${op.kind === 'mul' ? 'p_or(p_shl(p_and(control,7u),1u),1u)' : 'control'}))`;
            lines.push(`{unsigned control=0u;${sources}${v(op.i)}=${value};}`);
        }
        return lines.join('\n') + '\nobserve(s);';
    }
    if (stage.program) for (const step of stage.program) lines.push(step.box !== undefined ? boxCode(stage.boxes[step.box]) : pairCode(stage.ops[step.pair]));
    else {
        for (const box of stage.boxes || []) lines.push(boxCode(box));
        for (const op of stage.ops || []) {
            if (stage.kind !== 'controlled') { lines.push(pairCode(op)); continue; }
            const zero = `p_equ(p_and(gate,${1 << op.bit}u),0u)`, active = op.inverted ? zero : `!${zero}`;
            const action = op.branchless
                ? `unsigned delta=p_and(p_xor(${v(op.i)},${v(op.j)}),p_mul(${active},${mask}u));${v(op.i)}=p_xor(${v(op.i)},delta);${v(op.j)}=p_xor(${v(op.j)},delta);`
                : `if(${active}){unsigned temporary=${v(op.i)};${v(op.i)}=${v(op.j)};${v(op.j)}=temporary;}`;
            lines.push(`{unsigned gate=${v(op.control)};${mix('gate', op.gate)}${action}}`);
        }
    }
    return lines.join('\n') + '\nobserve(s);';
}
