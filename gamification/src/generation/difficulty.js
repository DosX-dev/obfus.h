// Structural admission checks, not a claim about the time needed by a human solver.
// Dependencies are conservative: both sides of a conditional contribute here.
export function analyzeDifficulty(ir) {
    let dependencies = Array.from({ length: ir.width }, (_, i) => new Set([i]));
    let branches = 0;
    const forms = new Set(), partitions = new Set();
    const kinds = new Map();
    function sources(node, coordinates, refs = []) {
        if (node.kind === 'ref') return new Set(refs[node.index]);
        if (node.kind === 'source') return new Set(dependencies[coordinates[node.index]]);
        const result = sources(node.left, coordinates, refs);
        if (node.right) for (const source of sources(node.right, coordinates, refs)) result.add(source);
        if (node.kind === 'select') {
            branches++;
            for (const source of dependencies[coordinates[node.selector]]) result.add(source);
        }
        return result;
    }
    for (const stage of ir.stages) {
        kinds.set(stage.kind, (kinds.get(stage.kind) || 0) + 1);
        if (stage.kind === 'bitnetwork') {
            for (const gate of stage.gates) {
                const targets = [gate.target, ...(gate.destination === undefined ? [] : [gate.destination])].map(wire => Math.floor(wire / ir.bits));
                const combined = new Set([...targets, ...gate.controls.map(wire => Math.floor(wire / ir.bits))].flatMap(word => [...dependencies[word]]));
                for (const target of targets) dependencies[target] = new Set(combined);
            }
        }
        if (stage.kind === 'permute') dependencies = stage.order.map(source => dependencies[source]);
        if (stage.kind === 'weave') {
            for (const op of stage.ops) {
                const combined = new Set(dependencies[op.i]);
                for (const source of op.sources) for (const input of dependencies[source.index]) combined.add(input);
                dependencies[op.i] = combined;
            }
        }
        if (['matrix', 'spn', 'controlled'].includes(stage.kind)) {
            for (const op of stage.ops) {
                const combined = new Set([...dependencies[op.i], ...dependencies[op.j], ...(op.control === undefined ? [] : dependencies[op.control])]);
                dependencies[op.i] = new Set(combined);
                dependencies[op.j] = new Set(combined);
            }
        }
        if (stage.kind === 'couple') {
            const coordinates = Array.from({ length: ir.width }, (_, i) => i);
            for (const op of stage.ops) {
                forms.add(op.form);
                dependencies[op.i] = new Set([...dependencies[op.i], ...sources(op.expr, coordinates)]);
            }
        }
        if (['feistel', 'braid'].includes(stage.kind)) {
            partitions.add(stage.sources.length);
            const refs = [];
            for (const node of stage.nodes || []) refs.push(sources(node, stage.sources, refs));
            const changed = dependencies.map(values => new Set(values));
            stage.ops.forEach((op, i) => {
                forms.add(op.form);
                const destination = stage.targets[i];
                changed[destination] = new Set([...dependencies[destination], ...sources(op.expr, stage.sources, refs)]);
            });
            dependencies = stage.order.map(source => changed[source]);
        }
    }
    return {
        stages: ir.stages.length,
        coupled: kinds.get('couple') || 0,
        braidedRounds: kinds.get('braid') || 0,
        rounds: (kinds.get('braid') || 0) + (kinds.get('feistel') || 0) + ir.stages.filter(stage => stage.kind === 'couple' && stage.round).length,
        feistelRounds: kinds.get('feistel') || 0,
        sequentialRounds: ir.stages.filter(stage => stage.kind === 'couple' && stage.round).length,
        branches,
        updateForms: [...forms].sort(),
        partitionWidths: [...partitions].sort((a, b) => a - b),
        minimumInputDependencies: Math.min(...dependencies.map(values => values.size)),
        functions: ir.groups.length
    };
}

export function assertDifficulty(ir) {
    const summary = analyzeDifficulty(ir);
    let valid = false;
    if (ir.level === 'easy') valid = ir.bits === 8 && ir.length === 12
        && summary.stages >= 2 && summary.stages <= 4
        && ir.stages.some(stage => stage.kind === 'substitute')
        && ir.stages.every(stage => ['affine', 'triangular', 'permute', 'substitute', 'bitshuffle', 'bitnetwork'].includes(stage.kind))
        && ir.stages.filter(stage => stage.kind === 'bitnetwork').every(stage => stage.span === 1);
    if (ir.level === 'medium') valid = ir.bits === 16 && ir.length === 24
        && summary.stages >= 8 && summary.stages <= 12
        && summary.coupled >= 4 && summary.functions >= 3
        && ir.stages.some(stage => stage.kind === 'spn') && ir.stages.some(stage => stage.kind === 'controlled')
        && summary.updateForms.length >= 3 && summary.minimumInputDependencies >= ir.width / 2;
    if (ir.level === 'hardcore') valid = ir.bits === 16 && ir.length === 32
        && summary.rounds >= 48 && summary.rounds <= 80
        && summary.feistelRounds >= 24 && summary.sequentialRounds >= 12
        && summary.branches > 0 && summary.partitionWidths.length >= 3
        && summary.updateForms.length === 5 && summary.minimumInputDependencies === ir.width;
    if (ir.level === 'extreme') valid = ir.bits === 16 && ir.length === 48
        && summary.rounds >= 96 && summary.rounds <= 144
        && summary.braidedRounds >= 32 && summary.feistelRounds >= 24 && summary.sequentialRounds >= 24
        && summary.branches > 0 && summary.partitionWidths.length >= 4
        && summary.updateForms.length === 5 && summary.minimumInputDependencies === ir.width
        && ir.stages.filter(stage => stage.kind === 'affine').length >= 8;
    valid = valid && ir.stages.some(stage => stage.kind === 'bitnetwork');
    if (ir.level === 'extreme') valid = valid && ir.stages.filter(stage => stage.kind === 'bitnetwork').length >= 2
        && ir.stages.filter(stage => stage.kind === 'bitnetwork').every(stage => stage.span === ir.width && stage.layers >= 2);
    if (!valid) throw new Error('Generated algorithm does not meet its difficulty structure contract');
    return summary;
}
