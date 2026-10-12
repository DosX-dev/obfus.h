// Shared acyclic control circuits feed reversible updates of a disjoint bank.
// Every node reaches the final node, so no generated circuit is decorative noise.
export function braid(width, random, shuffle, expression) {
    const coordinates = shuffle(width, random), split = 8 + random(9);
    const sources = coordinates.slice(0, split), targets = coordinates.slice(split);
    const local = Array.from({ length: split }, (_, i) => i);
    const nodes = [expression(local, random, 16, 4, 6, true)];
    for (let i = 1, count = 6 + random(5); i < count; i++) {
        const kinds = ['add', 'sub', 'xor'];
        nodes.push({
            kind: 'rot', value: 1 + random(15), left: {
                kind: kinds[random(kinds.length)], left: { kind: 'ref', index: i - 1 },
                right: random(2) && i > 1 ? { kind: 'ref', index: random(i - 1) } : { kind: 'source', index: random(split) }
            }
        });
    }
    const ops = targets.map(() => ({
        form: random(5), expr: {
            kind: random(2) ? 'xor' : 'add', left: { kind: 'ref', index: nodes.length - 1 },
            right: { kind: 'rot', value: 1 + random(15), left: { kind: 'ref', index: random(nodes.length - 1) } }
        }
    }));
    return { kind: 'braid', sources, targets, nodes, ops, order: shuffle(width, random) };
}
