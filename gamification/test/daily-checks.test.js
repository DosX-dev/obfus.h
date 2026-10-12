import test from 'node:test';
import assert from 'node:assert/strict';
import { generate } from '../src/generation/generator.js';
import { LEVELS } from '../src/core/config.js';
import { samples, byteSamples } from '../src/verification/probes.js';

test('fast daily samples retain every input position, length errors and invalid bytes', () => {
    for (const level of LEVELS) {
        const challenge = generate(Buffer.alloc(32, 9), { date: '2026-10-12', level });
        const p = challenge.password, text = samples(challenge, { quick: true }), bytes = byteSamples(challenge, { quick: true });
        assert.equal(text.length, 100);
        assert.equal(bytes.length, 100);
        assert.equal(text.filter(value => value === p).length, 1);
        assert.equal(bytes.filter(value => value.equals(Buffer.from(p))).length, 1);
        assert(text.includes('') && text.some(value => value.length > p.length) && text.some(value => value.length === p.length - 1));
        for (let at = 0; at < p.length; at++) {
            const changedAt = value => value.length === p.length && value[at] !== p[at] && [...value].every((char, i) => i === at || char === p[i]);
            assert(text.some(changedAt), 'Every character position must reach the verification algorithm');
            const onlyAt = value => value.length === p.length && value[at] !== p.charCodeAt(at) && value.every((char, i) => i === at || char === p.charCodeAt(i));
            assert(bytes.some(value => onlyAt(value) && value[at] >= 33 && value[at] <= 126));
            assert(bytes.some(value => onlyAt(value) && (value[at] < 33 || value[at] > 126)));
        }
        assert(samples(challenge).length > 1000);
        assert.equal(byteSamples(challenge).length, 3 + 255 * p.length);
    }
});
