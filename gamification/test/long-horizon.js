import assert from 'node:assert/strict';
import { generate, behaviorFingerprint } from '../src/generation/generator.js';
import { shapeFingerprint, profile, profileDistance } from '../src/generation/diversity.js';
import { createHistoryIndex, hasSeen, remember } from '../src/round/history.js';
const days = Number(process.argv[2] || 3653);
assert(Number.isSafeInteger(days) && days > 0 && days <= 36525);
const index = createHistoryIndex([]), start = Date.UTC(2026, 9, 12), key = Buffer.alloc(32, 83), counts = {}, families = {}, minimum = {}, attempts = {};
for (let day = 0; day < days; day++) {
    const date = new Date(start + day * 86400000).toISOString().slice(0, 10);
    for (const level of ['easy', 'medium', 'hard']) {
        let selected, challenge;
        for (let attempt = 0; attempt < 3; attempt++) {
            challenge = generate(key, { date, round: day + 1, level, attempt });
            const candidate = { level, structure: challenge.fingerprint, behavior: behaviorFingerprint(challenge.ir), shape: shapeFingerprint(challenge.ir), profile: [...profile(challenge.ir)] };
            if (!hasSeen(index, candidate)) { selected = candidate; attempts[attempt] = (attempts[attempt] || 0) + 1; break; }
        }
        assert(selected, `No novel ${level} candidate on day ${day}`);
        for (const previous of index.recent.filter(entry => entry.level === level)) minimum[level] = Math.min(minimum[level] ?? 1, profileDistance(previous.profile, selected.profile));
        remember(index, selected);
        counts[level] = (counts[level] || 0) + 1;
        families[level] ??= {};
        for (const stage of challenge.ir.stages) families[level][stage.kind] = (families[level][stage.kind] || 0) + 1;
    }
    if (day % 250 === 249) console.log(`Ten-year simulation: ${day + 1}/${days} days PASS`);
}

console.log(JSON.stringify({ days, counts, minimum, attempts, families }, null, 4));
