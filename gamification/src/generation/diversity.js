// Compare complete update shapes after removing constants and coordinate labels.
// This is a conservative structural comparison, not a difficulty score.
import { LEVELS } from '../core/config.js';
import { hash } from '../core/crypto.js';
function expression(node) {
    if (node.kind === 'source') return 'source';
    if (node.kind === 'ref') return `ref${node.index}`;
    const left = expression(node.left);
    if (!node.right) return `${node.kind}(${left})`;
    const children = [left, expression(node.right)];
    if (!['sub', 'select'].includes(node.kind)) children.sort();
    return `${node.kind}(${children.join(',')})`;
}
export function profile(ir) {
    const counts = new Map();
    const add = text => counts.set(text, (counts.get(text) || 0) + 1);
    for (const stage of ir.stages) {
        add('stage:' + stage.kind);
        for (const node of stage.nodes || []) add('circuit:' + expression(node));
        if (['feistel', 'braid'].includes(stage.kind)) add(`partition:${stage.sources.length}/${stage.targets.length}`);
        if (stage.round) add('sequentialRound');
        if (stage.program) add('spn:flow:' + stage.program.map(step => step.box !== undefined ? 'B' : 'P').join(','));
        for (const box of stage.boxes || []) {
            add(stage.kind + ':boxMix:' + [...(box.before || []), ...(box.after || [])].map(op => op.kind).join(','));
            for (const table of box.tables) {
                const visited = new Set(), cycles = [];
                for (let i = 0; i < table.length; i++) {
                    if (visited.has(i)) continue;
                    let at = i, length = 0;
                    while (!visited.has(at)) {
                        visited.add(at);
                        length++;
                        at = table[at];
                    }
                    cycles.push(length);
                }
                add(stage.kind + ':substitution:' + cycles.sort((a, b) => a - b).join(','));
            }
        }
        for (const op of stage.ops || []) {
            if (op.before) add(stage.kind + ':pairMix:' + [...op.before, ...op.after].map(ops => ops.map(item => item.kind).join(',')).join('/'));
            if (op.gate) add('controlled:gate:' + op.gate.map(item => item.kind).join(',') + (op.inverted ? ':inverted' : ':normal'));
            add(stage.kind + ':' + (op.sequence ? op.sequence.map(item => item.kind).join(',') : op.expr ? op.form + ':' + expression(op.expr) : stage.kind === 'controlled' ? 'bitSwap' : ['linearLift', 'arxLift', 'multiplicativeLift'][op.form]));
        }
    }
    return counts;
}
export function distance(left, right) {
    return profileDistance(profile(left), profile(right));
}
export function profileDistance(left, right) {
    const a = new Map(left), b = new Map(right);
    let common = 0, union = 0;
    for (const key of new Set([...a.keys(), ...b.keys()])) {
        common += Math.min(a.get(key) || 0, b.get(key) || 0);
        union += Math.max(a.get(key) || 0, b.get(key) || 0);
    }
    return union ? 1 - common / union : 0;
}
export function shapeFingerprint(ir) {
    return hash(JSON.stringify([...profile(ir)].sort(([a], [b]) => a < b ? -1 : a > b ? 1 : 0)));
}
export function diversityReport(challenges) {
    const report = {};
    for (const level of LEVELS) {
        const group = challenges.filter(challenge => challenge.ir.level === level);
        const distances = [];
        for (let i = 0; i < group.length; i++) {
            for (let j = i + 1; j < group.length; j++) {
                distances.push(distance(group[i].ir, group[j].ir));
            }
        }
        distances.sort((a, b) => a - b);
        report[level] = {
            candidates: group.length,
            pairs: distances.length,
            minimum: distances[0],
            median: distances[Math.floor(distances.length / 2)],
            maximum: distances.at(-1)
        };
    }
    return report;
}
