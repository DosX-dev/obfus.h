import assert from 'node:assert/strict';
import { gunzipSync } from 'node:zlib';
import { hash } from '../core/crypto.js';
import { LEVELS } from '../core/config.js';
import { profileDistance, compactProfile } from '../generation/diversity.js';

export const HISTORY_VERSION = 1;
export const RECENT_CHECKS_PER_LEVEL = 64;
export const MINIMUM_RECENT_DISTANCE = 0.35;

export function createHistoryIndex(history) {
    return {
        structures: new Set(history.map(entry => entry.structure)),
        behaviors: new Set(history.map(entry => entry.behavior)),
        shapes: new Set(history.map(entry => entry.shape)),
        recent: history.filter(entry => entry.profile).map(({ level, profile }) => ({ level, profile: compactProfile(profile) }))
    };
}

export function hasSeen(index, { structure, behavior, shape, level, profile }) {
    if (index.structures.has(structure) || index.behaviors.has(behavior) || index.shapes.has(shape)) return true;
    const normalized = profile && compactProfile(profile);
    return !!normalized && (index.recent || []).some(entry => entry.level === level
        && profileDistance(normalized, entry.profile) < MINIMUM_RECENT_DISTANCE);
}

// Keep all historical fingerprints, but only a bounded recent comparison window.
// Profiles contain operation shapes/counts without constants or coordinate labels.
export function retainHistory(history, additions) {
    const entries = [...history, ...additions], counts = new Map();
    return entries.map(entry => ({ ...entry })).reverse().map(entry => {
        if (!entry.profile) return entry;
        const count = (counts.get(entry.level) || 0) + 1;
        counts.set(entry.level, count);
        if (count > RECENT_CHECKS_PER_LEVEL) delete entry.profile;
        else entry.profile = compactProfile(entry.profile);
        return entry;
    }).reverse();
}

export function remember(index, { structure, behavior, shape, level, profile }) {
    index.structures.add(structure);
    index.behaviors.add(behavior);
    index.shapes.add(shape);
    if (profile) {
        const recent = index.recent || [];
        index.recent = [
            ...recent.filter(entry => entry.level !== level),
            ...recent.filter(entry => entry.level === level).slice(1 - RECENT_CHECKS_PER_LEVEL),
            { level, profile: compactProfile(profile) }
        ];
    }
}

export async function loadHistory(api, state) {
    if (!state) return [];
    assert(Number.isSafeInteger(state.historyCount) && state.historyCount >= LEVELS.length && state.historyCount <= state.round * LEVELS.length, 'History does not cover every published round');
    const release = await api.release();
    const assets = await api.assets(release.id);
    const file = state.files.find(entry => entry.role === 'history');
    const asset = assets.find(entry => entry.name === file?.name);
    if (!asset) throw new Error('Structural history missing');
    const data = await api.download(asset);
    assert.equal(hash(data), file.sha256);
    const envelope = JSON.parse(gunzipSync(data, { maxOutputLength: 64 * 1024 * 1024 }).toString());
    assert.equal(envelope.version, HISTORY_VERSION, 'Unsupported history protocol');
    const history = envelope.checks;
    assert(Array.isArray(history) && history.length === state.historyCount);
    assert(history.every(entry => entry
        && /^[a-f0-9]{64}$/.test(entry.structure)
        && /^[a-f0-9]{64}$/.test(entry.behavior)
        && /^[a-f0-9]{64}$/.test(entry.shape)));
    const recentCounts = new Map();
    for (const entry of history) if (entry.profile !== undefined) {
        assert(LEVELS.includes(entry.level), 'Invalid recent-profile difficulty');
        assert(Array.isArray(entry.profile) && entry.profile.length > 0 && entry.profile.length <= 8192, 'Invalid recent-profile size');
        assert(entry.profile.every(item => Array.isArray(item) && item.length === 2
            && typeof item[0] === 'string' && item[0].length <= 4096 && /^[a-zA-Z0-9:(),/]+$/.test(item[0])
            && Number.isSafeInteger(item[1]) && item[1] > 0 && item[1] <= 10000), 'Invalid recent-profile data');
        assert.equal(new Set(entry.profile.map(item => item[0])).size, entry.profile.length, 'Duplicate profile operation');
        recentCounts.set(entry.level, (recentCounts.get(entry.level) || 0) + 1);
        assert(recentCounts.get(entry.level) <= RECENT_CHECKS_PER_LEVEL, 'Unbounded recent-profile history');
    }
    assert.equal(new Set(history.map(value => value.structure)).size, history.length, 'Repeated historical structure');
    assert.equal(new Set(history.map(value => value.behavior)).size, history.length, 'Repeated historical behavior');
    assert.equal(new Set(history.map(value => value.shape)).size, history.length, 'Repeated historical expression shapes');
    return history;
}
