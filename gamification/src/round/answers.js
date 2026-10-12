import assert from 'node:assert/strict';
import { unseal, hash } from '../core/crypto.js';
import { LEVELS, VERSION } from '../core/config.js';

export async function previousAnswers(api, key, state) {
    if (!state) return null;
    assert.equal(state.version, VERSION, 'Unsupported saved round');
    const release = await api.release(),
        assets = await api.assets(release.id);
    const file = state.files.find((f) => f.role === 'answers');
    const asset = assets.find((a) => a.name === file?.name);
    if (!asset) throw new Error('Previous encrypted answers missing');
    const data = await api.download(asset);
    assert.equal(hash(data), file.sha256);
    const value = unseal(key, JSON.parse(data.toString()));
    assert.equal(value.date, state.date);
    assert.equal(value.round, state.round);
    assert.equal(value.version, VERSION, 'Unsupported saved answers');
    assert.deepEqual(Object.keys(value.answers).sort(), [...LEVELS].sort(), 'Saved answer levels mismatch');
    assert(Object.values(value.answers).every(answer => typeof answer === 'string' && /^[\x21-\x7e]+$/.test(answer)), 'Saved answers must be printable ASCII');
    return { date: value.date, round: value.round, answers: value.answers };
}
