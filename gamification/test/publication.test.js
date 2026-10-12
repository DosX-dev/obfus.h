import test from 'node:test';
import assert from 'node:assert/strict';
import { hash } from '../src/core/crypto.js';
import { publish, recover } from '../src/publication/publication.js';
import { description } from '../src/publication/description.js';
import { stateEqual } from '../src/publication/state.js';
import { playerZip } from '../src/core/zip.js';
import { VERSION, LEVELS } from '../src/core/config.js';
class FakeGitHub {
    constructor() {
        this.r = null;
        this.list = [];
        this.manifest = null;
        this.nextId = 1;
        this.fail = null;
        this.calls = [];
        this.failAt = null;
        this.after = false;
        this.splitStorage = true;
    }
    step(name, work) {
        this.calls.push(name);
        const failAt = this.failAt === this.calls.length;
        if (failAt) this.failAt = null;
        if (failAt && !this.after) throw Error('Injected transport failure');
        if (this.fail === name) {
            this.fail = null;
            throw Error('Injected ' + name);
        }
        const result = work();
        if (failAt) throw Error('Injected lost response');
        return result;
    }
    async release() {
        return this.step('release', () => this.r);
    }
    async createRelease(body) {
        return this.step('create', () => (this.r = { id: 1, ...body }));
    }
    async assets() {
        return this.step('assets', () => this.list);
    }
    async upload(id, name, data) {
        return this.step('upload:' + name, () => {
            const asset = { id: this.nextId++, name, data, browser_download_url: 'https://example.test/' + name, ...(this.splitStorage ? { storage: /^(answers-|history-|transaction-)/.test(name) ? 'private' : 'public' } : {}) };
            this.list.push(asset);
            return asset;
        });
    }
    async download(asset) {
        return this.step('download', () => asset.data);
    }
    async deleteAsset(id) {
        return this.step('delete', () => {
            this.list = this.list.filter((a) => a.id !== id);
        });
    }
    async updateRelease(id, body) {
        return this.step('body', () => {
            Object.assign(this.r, body);
        });
    }
    async readManifest() {
        return this.step('manifest', () => this.manifest);
    }
    async writeManifest(state) {
        return this.step('commit', () => {
            this.manifest = state;
        });
    }
}
function bundle(date, round) {
    const files = [...LEVELS, 'answers', 'history'].map(role => {
        const name = LEVELS.includes(role) ? `obfus-crackme-${date}-round-${round}-${role}-x64.zip` : `${role}-${date}-round-${round}.json`;
        const data = LEVELS.includes(role) ? playerZip(role + '.exe', Buffer.from('player-' + round + role)) : Buffer.from(role + round);
        return { name, data, role };
    });
    const state = { version: VERSION, date, round, files: files.map(file => ({ name: file.name, role: file.role, sha256: hash(file.data), storage: LEVELS.includes(file.role) ? 'public' : 'private', url: 'https://example.test/' + file.name })) };
    return [state, files];
}
test('first round, replay, skipped date and immutable release', async () => {
    const api = new FakeGitHub(),
        a = bundle('2026-10-12', 1);
    await publish(api, ...a);
    assert.equal(api.list.length, 6);
    assert.equal(api.r.make_latest, 'false');
    const count = api.calls.filter(name => name.startsWith('upload:') || ['body', 'commit', 'delete'].includes(name)).length;
    await publish(api, ...a);
    assert.equal(api.calls.filter(name => name.startsWith('upload:') || ['body', 'commit', 'delete'].includes(name)).length, count);
    const b = bundle('2026-10-15', 2);
    await publish(api, ...b);
    assert.equal(api.manifest.round, 2);
    assert.equal(api.list.length, 6);
    api.r.immutable = true;
    await assert.rejects(publish(api, ...bundle('2026-10-16', 3)), /mutable/);
});
for (const failure of [
    'upload:obfus-crackme-2026-10-13-round-2-easy-x64.zip',
    'upload:transaction-2026-10-13.json',
    'body',
    'commit',
    'delete'
]) {
    test(`recover from ${failure}`, async () => {
        const api = new FakeGitHub();
        await publish(api, ...bundle('2026-10-12', 1));
        api.fail = failure;
        await assert.rejects(publish(api, ...bundle('2026-10-13', 2)));
        assert(api.list.some((a) => a.name === 'obfus-crackme-2026-10-12-round-1-easy-x64.zip') || api.manifest.date === '2026-10-13');
        const pending = api.list.some((a) => a.name.startsWith('transaction-'));
        if (pending) await recover(api);
        else await publish(api, ...bundle('2026-10-13', 2));
        assert.equal(api.manifest.date, '2026-10-13');
        assert.equal(api.list.length, 6);
    });
}
test('old answers appear only with the new complete round', async () => {
    const api = new FakeGitHub();
    await publish(api, ...bundle('2026-10-12', 1));
    const [state, files] = bundle('2026-10-13', 2);
    state.previous = { date: '2026-10-12', round: 1, answers: { easy: 'OLD' } };
    api.fail = 'upload:obfus-crackme-2026-10-13-round-2-easy-x64.zip';
    await assert.rejects(publish(api, state, files));
    assert(!api.r.body.includes('OLD'));
    await publish(api, state, files);
    assert(api.r.body.includes('OLD'));
    assert(!description(state).includes('NEW-SECRET'));
});
export { FakeGitHub };

test('state equality tolerates JSON object-key reordering without accepting changed data', () => {
    const [state] = bundle('2026-10-12', 1);
    const reordered = { version: VERSION, files: state.files.map(file => Object.fromEntries(Object.entries(file).reverse())), round: state.round, date: state.date };
    assert(stateEqual(state, reordered));
    reordered.files[0].sha256 = hash('different');
    assert(!stateEqual(state, reordered));
    assert(!stateEqual(state, null));
});

test('every publication request tolerates interruption before and after server effects', async () => {
    const baseline = new FakeGitHub();
    await publish(baseline, ...bundle('2026-10-12', 1));
    baseline.calls = [];
    await publish(baseline, ...bundle('2026-10-13', 2));
    const requests = baseline.calls.length;
    for (const after of [false, true]) for (let at = 1; at <= requests; at++) {
        const api = new FakeGitHub();
        await publish(api, ...bundle('2026-10-12', 1));
        api.calls = [];
        api.failAt = at;
        api.after = after;
        try { await publish(api, ...bundle('2026-10-13', 2)); } catch { }
        api.failAt = null;
        if (api.manifest.date === '2026-10-13') {
            assert(api.list.some(a => a.name === 'obfus-crackme-2026-10-13-round-2-easy-x64.zip'), 'Committed current assets must survive a lost response');
        } else assert(api.list.some(a => a.name === 'obfus-crackme-2026-10-12-round-1-easy-x64.zip'));
        await recover(api);
        await publish(api, ...bundle('2026-10-13', 2));
        assert.equal(api.manifest.round, 2);
        assert.equal(api.list.length, 6);
        assert.equal(hash(api.list[0].data), api.manifest.files[0].sha256);
    }
});

test('partial multi-file upload preserves the old set and retries the complete new set', async () => {
    const api = new FakeGitHub();
    await publish(api, ...bundle('2026-10-12', 1));
    const [state, files] = bundle('2026-10-13', 2);
    api.fail = 'upload:obfus-crackme-2026-10-13-round-2-hardcore-x64.zip';
    await assert.rejects(publish(api, state, files));
    assert.equal(api.manifest.date, '2026-10-12');
    assert.deepEqual(api.list.map(a => a.name), bundle('2026-10-12', 1)[0].files.map(file => file.name));
    await publish(api, state, files);
    assert.equal(api.list.length, 6);
    assert.equal(api.manifest.date, state.date);
});

test('an initial release creation with a lost response is recoverable', async () => {
    const api = new FakeGitHub();
    api.failAt = 3;
    api.after = true;
    await assert.rejects(publish(api, ...bundle('2026-10-12', 1)));
    assert(api.r);
    await publish(api, ...bundle('2026-10-12', 1));
    assert.equal(api.manifest.round, 1);
});

test('missing release or missing committed manifest never silently resets history', async () => {
    const api = new FakeGitHub();
    await publish(api, ...bundle('2026-10-12', 1));
    const existing = api.r;
    api.r = null;
    await assert.rejects(recover(api), /refusing to reset/);
    await assert.rejects(publish(api, ...bundle('2026-10-13', 1)), /refusing to reset/);
    assert.equal(api.manifest.round, 1);
    api.r = existing;
    api.manifest = null;
    api.list.push({ id: 99, name: 'manifest-2026-10-12.json', data: Buffer.from('{}') });
    await assert.rejects(recover(api), /refusing to reset/);
    assert(api.list.some(asset => asset.name === 'obfus-crackme-2026-10-12-round-1-easy-x64.zip'));
});

test('invalid bundles and rollback journals fail before modifying published assets', async () => {
    const api = new FakeGitHub();
    await publish(api, ...bundle('2026-10-12', 1));
    const [state, files] = bundle('2026-10-13', 2);
    files[0].data = Buffer.from('wrong');
    const before = api.calls.length;
    await assert.rejects(publish(api, state, files), /does not match/);
    assert.equal(api.calls.length, before);
    const previous = bundle('2026-10-11', 1)[0];
    api.list.push({ id: 100, name: 'transaction-2026-10-11.json', data: Buffer.from(JSON.stringify({ version: 1, journal: 'transaction-2026-10-11.json', next: previous })) });
    await assert.rejects(recover(api), /recovery sequence/);
    assert.equal(api.manifest.date, '2026-10-12');
    assert(api.list.some(asset => asset.name === 'obfus-crackme-2026-10-12-round-1-easy-x64.zip'));
});

test('missing manifest and every asset never turn a published release into a fresh installation', async () => {
    const api = new FakeGitHub();
    await publish(api, ...bundle('2026-10-12', 1));
    api.manifest = null;
    api.list = [];
    api.calls = [];
    await assert.rejects(recover(api), /refusing to reset history/);
    await assert.rejects(publish(api, ...bundle('2026-10-13', 1)), /refusing to reset history/);
    assert(!api.calls.some(name => ['create', 'body', 'commit', 'delete'].includes(name) || name.startsWith('upload:')));
    assert.equal(api.manifest, null);
    assert.deepEqual(api.list, []);
});

test('an interrupted first upload remains recoverable without a committed manifest', async () => {
    const api = new FakeGitHub();
    api.r = { id: 1, body: 'Preparing the first verified round.' };
    api.list.push({ id: 77, name: 'answers-2026-10-12.json', data: Buffer.from('partial') });
    assert.equal(await recover(api), null);
    await publish(api, ...bundle('2026-10-12', 1));
    assert.equal(api.manifest.round, 1);
    assert.equal(api.list.length, 6);
});

test('all first-publication interruptions recover, including orphan private assets', async () => {
    const initial = () => bundle('2026-10-12', 1);
    const baseline = new FakeGitHub();
    await publish(baseline, ...initial());
    for (const after of [false, true]) for (let at = 1; at <= baseline.calls.length; at++) {
        const api = new FakeGitHub();
        api.failAt = at;
        api.after = after;
        try { await publish(api, ...initial()); } catch { }
        api.failAt = null;
        await recover(api);
        await publish(api, ...initial());
        assert.equal(api.manifest.round, 1);
        assert.equal(api.list.length, 6);
    }
});

test('extra same-date rounds are explicitly fenced and recover at every request boundary', async () => {
    const [state, files] = bundle('2026-10-12', 1);
    const fixture = { state, files, next: state, newFiles: files };
    const newFiles = fixture.newFiles.map(file => ({ ...file, name: file.name.replace('round-1', 'round-2') }));
    const next = { ...fixture.next, round: 2, previous: { date: fixture.state.date, round: 1, answers: { easy: 'old-answer' } }, files: fixture.next.files.map(file => ({ ...file, name: file.name.replace('round-1', 'round-2') })) };
    const baseline = new FakeGitHub();
    await publish(baseline, fixture.state, fixture.files);
    assert.equal((await publish(baseline, next, newFiles)).round, 1, 'Ordinary reruns cannot advance today');
    await assert.rejects(publish(baseline, next, newFiles, { advanceFromRound: 2 }), /Expected current round/);
    baseline.calls = []; baseline.splitStorage = true;
    await publish(baseline, next, newFiles, { advanceFromRound: 1 });
    const boundaries = baseline.calls.length;
    assert.match(baseline.r.body, /Previous round answers/);
    for (const after of [false, true]) for (let at = 1; at <= boundaries; at++) {
        const api = new FakeGitHub();
        await publish(api, fixture.state, fixture.files);
        api.calls = []; api.splitStorage = true; api.failAt = at; api.after = after;
        try { await publish(api, next, newFiles, { advanceFromRound: 1 }); } catch { }
        api.failAt = null;
        await recover(api);
        await publish(api, next, newFiles, { advanceFromRound: 1 });
        assert.deepEqual(api.manifest, next);
        assert.equal(api.list.filter(asset => asset.storage === 'public').length, 4);
        assert.equal(api.list.filter(asset => asset.storage === 'private').length, 2);
        assert(api.list.every(asset => !asset.name.startsWith('transaction-')));
        const mutations = api.calls.filter(name => name.startsWith('upload:') || ['body', 'commit', 'delete'].includes(name)).length;
        await publish(api, next, newFiles, { advanceFromRound: 1 });
        assert.equal(api.calls.filter(name => name.startsWith('upload:') || ['body', 'commit', 'delete'].includes(name)).length, mutations);
    }
    const collision = { ...next, files: fixture.next.files };
    await assert.rejects(publish(new FakeGitHub(), collision, fixture.newFiles, { advanceFromRound: 0 }), /Invalid manual/);
});
