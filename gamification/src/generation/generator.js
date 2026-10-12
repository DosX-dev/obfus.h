import { ALPHABET, LEVELS, VERSION } from '../core/config.js';
import { derive, rng, hash } from '../core/crypto.js';
import { transform, words } from './model.js';
import { assertDifficulty } from './difficulty.js';
import { families } from './families.js';
function shuffle(n, random) {
    const a = Array.from({ length: n }, (_, i) => i);
    for (let i = n - 1; i > 0; --i) {
        const j = random(i + 1);
        [a[i], a[j]] = [a[j], a[i]];
    }
    return a;
}
export function fingerprint(ir) {
    // Deliberately ignore constants, seed, identifier spelling and grouping.
    return hash(
        JSON.stringify({
            bits: ir.bits,
            width: ir.width,
            stages: ir.stages.map((s) => ({
                kind: s.kind,
                order: s.order,
                sources: s.sources,
                targets: s.targets,
                round: s.round,
                program: s.program,
                boxes: s.boxes?.map(box => ({ i: box.i, tables: box.tables.length, route: box.route, before: box.before?.map(op => op.kind), after: box.after?.map(op => op.kind) })).sort((a, b) => a.i - b.i),
                ops: (s.kind === 'affine' ? [...s.ops].sort((a, b) => a.i - b.i) : s.ops)?.map((c) => ({
                    i: c.i,
                    j: c.j,
                    control: c.control,
                    bit: c.bit,
                    inverted: c.inverted,
                    gate: c.gate?.map(op => op.kind),
                    before: c.before?.map(ops => ops.map(op => op.kind)),
                    after: c.after?.map(ops => ops.map(op => op.kind)),
                    form: c.form,
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
    if (node.kind === 'source') return { kind: node.kind, index: node.index };
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
function roundExpression(sources, random, bits, minimum = 4, maximum = sources.length, branching = false) {
    const indices = shuffle(sources.length, random)
        .slice(0, minimum + random(maximum - minimum + 1))
        .map((i) => sources[i]);
    const pool = indices.map((index) => ({ kind: 'source', index }));
    while (pool.length > 1) {
        const first = random(pool.length),
            left = pool.splice(first, 1)[0];
        const second = random(pool.length),
            right = pool.splice(second, 1)[0];
        const kinds = branching ? ['add', 'xor', 'sub', 'and', 'or', 'select'] : ['add', 'xor', 'sub'];
        let node = { kind: kinds[random(kinds.length)], left, right };
        if (node.kind === 'select') {
            node.selector = indices[random(indices.length)];
            node.value = 1 << random(bits);
        }
        const wrappers = 1 + random(3);
        for (let i = 0; i < wrappers; i++) {
            const eligible = ['rot', 'mul', 'constant', 'xorConstant'].filter(kind => kind !== node.kind);
            const kind = eligible[random(eligible.length)];
            node = {
                kind,
                left: node,
                value:
                    kind === 'rot'
                        ? 1 + random(bits - 1)
                        : kind === 'mul'
                            ? 3 + 2 * random(7)
                            : 1 + random(2 ** bits - 1)
            };
        }
        pool.push(node);
    }
    return pool[0];
}
export function generate(key, { date, round = 1, level, attempt = 0 }) {
    if (
        !LEVELS.includes(level) ||
        !/^\d{4}-\d{2}-\d{2}$/.test(date) ||
        new Date(date + 'T00:00:00Z').toISOString().slice(0, 10) !== date ||
        !Number.isSafeInteger(round) ||
        round < 1 ||
        !Number.isSafeInteger(attempt) ||
        attempt < 0 ||
        attempt > 2
    )
        throw new Error('Invalid round parameters');
    const context = { date, round, level, attempt },
        random = rng(key, 'structure', context);
    const length = { easy: 12, medium: 24, hard: 32 }[level],
        bits = level === 'easy' ? 8 : 16;
    const width = (length * 8) / bits,
        mask = 2 ** bits - 1;
    const secretRandom = rng(key, 'password', context);
    const password = Array.from({ length }, () => ALPHABET[secretRandom(ALPHABET.length)]).join('');
    const stages = [],
        affine = () => ({
            kind: 'affine',
            ops: shuffle(width, random).map((i) => ({
                i,
                sequence: sequence()
            }))
        });
    function sequence() {
        const kinds = ['xor', 'add', 'sub', 'mul', 'rot', 'shlXor', 'shrXor'];
        const operations = [kinds[3 + random(4)]];
        const count = 3 + random(5);
        for (let i = 1; i < count; i++) {
            const eligible = kinds.filter((kind) => kind !== operations.at(-1));
            operations.push(eligible[random(eligible.length)]);
        }
        return operations.map((kind) => ({
            kind,
            value: ['rot', 'shlXor', 'shrXor'].includes(kind) ? 1 + random(bits - 1) : kind === 'mul' ? 3 + 2 * random(7) : 1 + random(mask)
        }));
    }
    const permute = () => {
        const order = shuffle(width, random);
        if (order.every((source, i) => source === i)) [order[0], order[1]] = [order[1], order[0]];
        return { kind: 'permute', order };
    };
    const extra = families(width, bits, random, shuffle);
    const couple = (round = false) => ({
        kind: 'couple',
        round,
        ops: shuffle(width, random).map((i) => {
            const sources = Array.from({ length: width }, (_, index) => index).filter((index) => index !== i);
            return { i, expr: roundExpression(sources, random, bits, round ? 4 : 2, round ? 6 : level === 'medium' ? 4 : 5, level === 'hard'), form: random(5) };
        })
    });
    if (level === 'easy') {
        const count = 2 + random(3);
        stages.push(affine());
        const factories = [extra.substitute];
        while (factories.length < count - 1) factories.push([affine, permute, extra.substitute][random(3)]);
        stages.push(...shuffle(factories.length, random).map(i => factories[i]()));
    } else if (level === 'medium') {
        const coupled = 4 + random(3);
        const count = Math.max(8 + random(5), coupled + 3);
        const factories = [...Array.from({ length: coupled }, () => () => couple()), extra.spn, extra.controlled];
        const eligible = [affine, permute, extra.matrix, extra.substitute, extra.spn, extra.controlled];
        while (factories.length < count - 1) factories.push(eligible[random(eligible.length)]);
        stages.push(affine(), ...shuffle(factories.length, random).map(i => factories[i]()));
    } else {
        const count = 48 + random(33);
        stages.push(affine(), extra.spn(), extra.controlled(), permute());
        const sequential = 12 + random(9);
        const roundKinds = shuffle(count, random).map(index => index < sequential);
        let remaining = 6 + random(7);
        for (let i = 0; i < count; ++i) {
            if (roundKinds[i]) stages.push(couple(true));
            else {
                const coordinates = shuffle(width, random);
                const split = 6 + random(5);
                const sources = coordinates.slice(0, split), targets = coordinates.slice(split);
                const localSources = Array.from({ length: split }, (_, index) => index);
                const ops = targets.map(() => ({ expr: roundExpression(localSources, random, bits, 4, split, true), form: random(5) }));
                stages.push({ kind: 'feistel', sources, targets, order: shuffle(width, random), ops });
            }
            if (--remaining === 0) {
                const eligible = [() => couple(), permute, affine, extra.spn, extra.matrix, extra.controlled, extra.substitute];
                stages.push(eligible[random(eligible.length)]());
                remaining = 6 + random(7);
            }
        }
        stages.push(couple());
    }
    // A physical storage permutation changes addressing without affecting the model.
    const ir = {
        version: VERSION,
        ...context,
        length,
        bits,
        width,
        stages,
        storage: shuffle(width, random),
        seed: derive(key, 'obfus-seed', context).readUInt32LE(0),
        groups: []
    };
    let at = 0;
    while (at < stages.length) {
        const end = Math.min(stages.length, at + 1 + random(level === 'easy' ? 2 : level === 'medium' ? 3 : 7));
        ir.groups.push([at, end]);
        at = end;
    }
    assertDifficulty(ir);
    const target = transform(ir, words(password, bits));
    return { ir, target, password, fingerprint: fingerprint(ir) };
}
