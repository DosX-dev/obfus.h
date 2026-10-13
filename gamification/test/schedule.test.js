import test from 'node:test';
import assert from 'node:assert/strict';
import { moscow, roundDue } from '../src/core/schedule.js';

test('manual publication can start before 13:00 without replacing a completed round', () => {
    const morning = { date: '2026-10-12', hour: 10 };
    assert.equal(roundDue(null, morning), false);
    assert.equal(roundDue(null, morning, true), true);
    assert.equal(roundDue({ date: morning.date }, morning, true), false);
    assert.equal(roundDue({ date: '2026-10-11' }, morning, true), true);
    assert.equal(roundDue(null, { ...morning, hour: 13 }), true);
    assert.equal(roundDue({ date: morning.date }, { ...morning, hour: 14 }), false);
});

test('extra round requires the exact expected round and a manual invocation', () => {
    const time = { date: '2026-10-12', hour: 14 }, current = { date: time.date, round: 1 };
    assert.equal(roundDue(current, time, true, 1), true);
    assert.equal(roundDue(current, time, false, 1), false);
    assert.equal(roundDue({ ...current, round: 2 }, time, true, 1), false);
    assert.equal(roundDue(null, time, true, 1), false);
    assert.equal(roundDue({ ...current, date: '2026-10-11' }, time, true, 1), false);
});

test('quarter-hour backups retry unfinished dates and never repeat a completed date', () => {
    const unfinished = { date: '2026-10-12', round: 4 };
    const completed = { date: '2026-10-13', round: 5 };
    for (let hour = 10; hour <= 20; hour++)
        for (const minute of [0, 15, 30, 45]) {
            const time = moscow(new Date(`2026-10-13T${hour}:${String(minute).padStart(2, '0')}:00Z`));
            assert.equal(time.date, completed.date);
            assert.equal(roundDue(unfinished, time), true);
            assert.equal(roundDue(completed, time), false);
        }
    assert.equal(roundDue(unfinished, moscow(new Date('2026-10-13T09:45:00Z'))), false);
    assert.equal(roundDue(completed, moscow(new Date('2026-10-14T10:00:00Z'))), true);
});
