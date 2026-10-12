import test from 'node:test';
import assert from 'node:assert/strict';
import { generate } from '../src/generation/generator.js';
import { distance, diversityReport, shapeFingerprint } from '../src/generation/diversity.js';

test('variation survives removal of constants, coordinate labels and function boundaries', () => {
    const challenges = [];
    for (const level of ['easy', 'medium', 'hard']) for (let round = 1; round <= 20; round++) challenges.push(generate(Buffer.alloc(32, 21), { date: '2026-10-12', round, level }));
    const report = diversityReport(challenges);
    for (const level of Object.keys(report)) assert(report[level].minimum > 0.35, 'Too much common normalized update structure');
    const changed = structuredClone(challenges[0].ir);
    changed.storage.reverse();
    changed.stages[0].ops.reverse();
    for (const op of changed.stages[0].ops) for (const item of op.sequence) item.value ^= 1;
    assert.equal(distance(challenges[0].ir, changed), 0, 'Constants/scheduling must not inflate diversity');
    assert.equal(shapeFingerprint(challenges[0].ir), shapeFingerprint(changed));
    for (const level of ['easy', 'medium', 'hard']) {
        const group = challenges.filter(item => item.ir.level === level);
        assert.equal(new Set(group.map(item => shapeFingerprint(item.ir))).size, group.length);
    }
});
