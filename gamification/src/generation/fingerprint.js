import { hash } from '../core/crypto.js';
import { transform, words } from './model.js';

export function fingerprint(ir) {
    // Deliberately ignore constants, seed, identifier spelling and grouping.
    return hash(
        JSON.stringify({
            bits: ir.bits,
            width: ir.width,
            stages: ir.stages.map((s) => ({
                kind: s.kind,
                gates: s.gates?.map(gate => ({ target: gate.target, destination: gate.destination, controls: gate.controls, truth: gate.truth.map(value => value ^ gate.truth[0]) })),
                order: s.order,
                sources: s.sources,
                targets: s.targets,
                round: s.round,
                nodes: s.nodes?.map(expressionShape),
                program: s.program,
                boxes: s.boxes?.map(box => ({ i: box.i, tables: box.tables.length, route: box.route, before: box.before?.map(op => op.kind), after: box.after?.map(op => op.kind) })).sort((a, b) => a.i - b.i),
                ops: (['affine', 'triangular'].includes(s.kind) ? [...s.ops].sort((a, b) => a.i - b.i) : s.ops)?.map((c) => ({
                    i: c.i,
                    kind: c.kind,
                    steps: c.steps?.map(step => ({ cut: step.cut, kind: step.kind, term: step.term })),
                    route: c.route,
                    sources: c.sources?.map(source => ({ index: source.index, combine: source.combine, mix: source.mix.map(op => op.kind) })),
                    j: c.j,
                    control: c.control,
                    bit: c.bit,
                    inverted: c.inverted,
                    gate: c.gate?.map(op => op.kind),
                    before: c.before?.map(ops => Array.isArray(ops) ? ops.map(op => op.kind) : ops.kind),
                    after: c.after?.map(ops => Array.isArray(ops) ? ops.map(op => op.kind) : ops.kind),
                    form: c.form,
                    program: c.program?.map(step => ({ target: step.target, kind: step.kind, alternate: step.alternate, bit: step.alternate ? step.bit : undefined, sourceMix: step.sourceMix.map(op => op.kind) })),
                    sequence: c.sequence?.map((op) => op.kind),
                    expr: c.expr && expressionShape(c.expr)
                }))
            }))
        })
    );
}
export function behaviorFingerprint(ir) {
    // Equal functions necessarily agree on these inputs. Different witnesses prove different behavior.
    return hash(JSON.stringify(['A', 'Z', '9'].map(char => transform(ir, words(char.repeat(ir.length), ir.bits)))));
}
function expressionShape(node) {
    if (['source', 'ref'].includes(node.kind)) return { kind: node.kind, index: node.index };
    const left = expressionShape(node.left);
    if (!node.right) return { kind: node.kind, left };
    const children = [left, expressionShape(node.right)];
    // Operand order alone does not change commutative arithmetic.
    if (!['sub', 'select'].includes(node.kind)) children.sort((a, b) => {
        const left = JSON.stringify(a), right = JSON.stringify(b);
        return left < right ? -1 : left > right ? 1 : 0;
    });
    return { kind: node.kind, selector: node.selector, children };
}
