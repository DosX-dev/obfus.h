import test from 'node:test';
import assert from 'node:assert/strict';
import { bitNetwork, networkModel } from '../src/generation/bit-network.js';
import { rng } from '../src/core/crypto.js';
import { generate } from '../src/generation/generator.js';
import { transform } from '../src/generation/model.js';
import { LEVELS } from '../src/core/config.js';

function shuffle(count, random) {
    const values = Array.from({ length: count }, (_, index) => index);
    for (let index = count - 1; index > 0; index--) {
        const other = random(index + 1);
        [values[index], values[other]] = [values[other], values[index]];
    }
    return values;
}
function oracle(input, stage, bits) {
    let state = input.reduce((value, word, index) => value | (BigInt(word) << BigInt(index * bits)), 0n);
    for (const gate of stage.gates) {
        const address = gate.controls.reduce((value, wire, index) => value | (Number((state >> BigInt(wire)) & 1n) << index), 0);
        if (gate.truth[address]) {
            const mask = 1n << BigInt(gate.target);
            if (gate.destination === undefined) state ^= mask;
            else if (((state >> BigInt(gate.target)) & 1n) !== ((state >> BigInt(gate.destination)) & 1n)) state ^= mask | (1n << BigInt(gate.destination));
        }
    }
    return input.map((_, index) => Number((state >> BigInt(index * bits)) & ((1n << BigInt(bits)) - 1n)));
}
test('bit circuits are bijective across the complete byte domain and match a packed BigInt oracle', () => {
    const stage = bitNetwork(1, 8, 1, 2, rng(Buffer.alloc(32, 51)), shuffle), seen = new Set();
    for (let value = 0; value < 256; value++) {
        const output = networkModel([value], stage, 8, false);
        assert.deepEqual(output, oracle([value], stage, 8));
        assert.deepEqual(networkModel(output, stage, 8, true), [value]);
        seen.add(output[0]);
    }
    assert.equal(seen.size, 256);
});
test('cross-word gates preserve all controls and invert chained updates on boundary states', () => {
    const stage = bitNetwork(24, 16, 24, 2, rng(Buffer.alloc(32, 81)), shuffle);
    for (const gate of stage.gates) {
        assert(!gate.controls.includes(gate.target));
        assert(!gate.controls.includes(gate.destination));
        assert.equal(new Set(gate.controls).size, gate.controls.length);
    }
    for (const input of [Array(24).fill(0), Array(24).fill(65535), Array.from({ length: 24 }, (_, index) => index * 2701)]) {
        const output = networkModel(input, stage, 16, false);
        assert.deepEqual(output, oracle(input, stage, 16));
        assert.deepEqual(networkModel(output, stage, 16, true), input);
    }
});
test('each difficulty uses its own circuit scope and survives removal controls', () => {
    for (const level of LEVELS) {
        const challenge = generate(Buffer.alloc(32, 43), { date: '2026-10-12', level });
        const networks = challenge.ir.stages.filter(stage => stage.kind === 'bitnetwork');
        assert(networks.length >= (level === 'extreme' ? 2 : 1));
        if (level === 'easy') assert(networks.every(stage => stage.span === 1));
        if (level === 'extreme') assert(networks.every(stage => stage.span === challenge.ir.width));
        for (const stage of networks) for (const gate of stage.gates) {
            if (level !== 'easy') assert(gate.controls.some(wire => Math.floor(wire / challenge.ir.bits) !== Math.floor(gate.target / challenge.ir.bits)));
        }
        const changed = structuredClone(challenge.ir);
        changed.stages = changed.stages.filter(stage => stage.kind !== 'bitnetwork');
        assert.notDeepEqual(transform(changed, transform(challenge.ir, challenge.target, true)), challenge.target);
    }
});
