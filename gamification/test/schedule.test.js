import test from 'node:test';
import assert from 'node:assert/strict';
import { roundDue } from '../src/core/schedule.js';

test('manual publication can start before 13:00 without replacing a completed round', () => {
    const morning = { date: '2026-10-12', hour: 10 };
    assert.equal(roundDue(null, morning), false);
    assert.equal(roundDue(null, morning, true), true);
    assert.equal(roundDue({ date: morning.date }, morning, true), false);
    assert.equal(roundDue({ date: '2026-10-11' }, morning, true), true);
    assert.equal(roundDue(null, { ...morning, hour: 13 }), true);
    assert.equal(roundDue({ date: morning.date }, { ...morning, hour: 14 }), false);
});
