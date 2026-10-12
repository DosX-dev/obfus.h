import { LEVELS } from '../src/core/config.js';
import test from 'node:test';
import assert from 'node:assert/strict';
import { generate } from '../src/generation/generator.js';
import { distance, diversityReport, shapeFingerprint } from '../src/generation/diversity.js';

test('variation survives removal of constants, coordinate labels and function boundaries', () => {
    const challenges = [];
    for (const level of LEVELS) for (let round = 1; round <= 20; round++) challenges.push(generate(Buffer.alloc(32, 21), { date: '2026-10-12', round, level }));
    const report = diversityReport(challenges);
    for (const level of Object.keys(report)) assert(report[level].minimum > 0.35, 'Too much common normalized update structure');
    const original = challenges.find(challenge => challenge.ir.stages.some(stage => stage.kind === 'affine')).ir;
    const changed = structuredClone(original);
    changed.storage.reverse();
    const affine = changed.stages.find(stage => stage.kind === 'affine');
    affine.ops.reverse();
    for (const op of affine.ops) for (const item of op.sequence) item.value ^= 1;
    assert.equal(distance(original, changed), 0, 'Constants/scheduling must not inflate diversity');
    assert.equal(shapeFingerprint(original), shapeFingerprint(changed));
    for (const level of LEVELS) {
        const group = challenges.filter(item => item.ir.level === level);
        assert.equal(new Set(group.map(item => shapeFingerprint(item.ir))).size, group.length);
    }
});
