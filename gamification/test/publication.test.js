import test from 'node:test';
import assert from 'node:assert/strict';
import { hash } from '../src/core/crypto.js';
import { publish, recover } from '../src/publication/publication.js';
import { description } from '../src/publication/description.js';
import { stateEqual } from '../src/publication/state.js';
import { playerZip } from '../src/core/zip.js';
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
        this.splitStorage = false;
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
    const name = `TASK-${date}.txt`,
        data = Buffer.from('task-' + round);
    const state = {
        date,
        round,
        files: [{ name, sha256: hash(data), role: 'task', url: 'https://example.test/' + name }]
    };
    return [state, [{ name, data }]];
}
test('first round, replay, skipped date and immutable release', async () => {
    const api = new FakeGitHub(),
        a = bundle('2026-10-12', 1);
    await publish(api, ...a);
    assert.equal(api.list.length, 1);
    assert.equal(api.r.make_latest, 'false');
    const count = api.calls.filter(name => name.startsWith('upload:') || ['body', 'commit', 'delete'].includes(name)).length;
    await publish(api, ...a);
    assert.equal(api.calls.filter(name => name.startsWith('upload:') || ['body', 'commit', 'delete'].includes(name)).length, count);
    const b = bundle('2026-10-15', 2);
    await publish(api, ...b);
    assert.equal(api.manifest.round, 2);
    assert.equal(api.list.length, 1);
    api.r.immutable = true;
    await assert.rejects(publish(api, ...bundle('2026-10-16', 3)), /mutable/);
});
for (const failure of [
    'upload:TASK-2026-10-13.txt',
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
        assert(api.list.some((a) => a.name === 'TASK-2026-10-12.txt') || api.manifest.date === '2026-10-13');
        const pending = api.list.some((a) => a.name.startsWith('transaction-'));
        if (pending) await recover(api);
        else await publish(api, ...bundle('2026-10-13', 2));
        assert.equal(api.manifest.date, '2026-10-13');
        assert.equal(api.list.length, 1);
    });
}
test('old answers appear only with the new complete round', async () => {
    const api = new FakeGitHub();
    await publish(api, ...bundle('2026-10-12', 1));
    const [state, files] = bundle('2026-10-13', 2);
    state.previous = { date: '2026-10-12', round: 1, answers: { easy: 'OLD' } };
    api.fail = 'upload:TASK-2026-10-13.txt';
    await assert.rejects(publish(api, state, files));
    assert(!api.r.body.includes('OLD'));
    await publish(api, state, files);
    assert(api.r.body.includes('OLD'));
    assert(!description(state).includes('NEW-SECRET'));
});
export { FakeGitHub };

test('state equality tolerates JSON object-key reordering without accepting changed data', () => {
    const [state] = bundle('2026-10-12', 1);
    const reordered = { files: state.files.map(file => Object.fromEntries(Object.entries(file).reverse())), round: state.round, date: state.date };
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
            assert(api.list.some(a => a.name === 'TASK-2026-10-13.txt'), 'Committed current assets must survive a lost response');
        } else assert(api.list.some(a => a.name === 'TASK-2026-10-12.txt'));
        await recover(api);
        await publish(api, ...bundle('2026-10-13', 2));
        assert.equal(api.manifest.round, 2);
        assert.equal(api.list.length, 1);
        assert.equal(hash(api.list[0].data), api.manifest.files[0].sha256);
    }
});

test('partial multi-file upload preserves the old set and retries the complete new set', async () => {
    const api = new FakeGitHub();
    await publish(api, ...bundle('2026-10-12', 1));
    const [state, files] = bundle('2026-10-13', 2);
    for (const level of ['easy', 'medium', 'hard']) {
        const name = `obfus-crackme-2026-10-13-${level}-x64.exe`, data = Buffer.from(level);
        files.push({ name, data });
        state.files.push({ name, sha256: hash(data), role: level, url: 'https://example.test/' + name });
    }
    api.fail = 'upload:obfus-crackme-2026-10-13-hard-x64.exe';
    await assert.rejects(publish(api, state, files));
    assert.equal(api.manifest.date, '2026-10-12');
    assert.deepEqual(api.list.map(a => a.name), ['TASK-2026-10-12.txt']);
    await publish(api, state, files);
    assert.equal(api.list.length, 4);
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
    assert(api.list.some(asset => asset.name === 'TASK-2026-10-12.txt'));
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
    assert(api.list.some(asset => asset.name === 'TASK-2026-10-12.txt'));
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
    assert.equal(api.list.length, 1);
});

test('all first-publication interruptions recover, including orphan private assets', async () => {
    const initial = () => {
        const [state, files] = bundle('2026-10-12', 1);
        for (const role of ['answers', 'manifest', 'history']) {
            const name = `${role}-2026-10-12.json`, data = Buffer.from(role);
            files.push({ name, data });
            state.files.push({ name, sha256: hash(data), role, url: 'https://example.test/' + name });
        }
        return [state, files];
    };
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
        assert.equal(api.list.length, 4);
    }
});

function legacyPlayers() {
    const state = { date: '2026-10-12', round: 1, files: [] }, files = [];
    for (const role of ['easy', 'medium', 'hard', 'answers', 'history']) {
        const name = ['easy', 'medium', 'hard'].includes(role) ? `obfus-crackme-2026-10-12-${role}-x64.exe` : `${role}-2026-10-12.json`;
        const data = Buffer.from('original-' + role);
        files.push({ name, data }); state.files.push({ name, role, sha256: hash(data), size: data.length, url: 'https://example.test/' + name });
    }
    const newFiles = files.map((file, index) => index < 3 ? { name: file.name.replace(/\.exe$/, '.zip'), data: playerZip(file.name, file.data) } : file);
    const next = { ...state, files: newFiles.map((file, index) => ({ name: file.name, role: state.files[index].role, sha256: hash(file.data), size: file.data.length, storage: index < 3 ? 'public' : 'private', ...(index < 3 ? { url: 'https://example.test/' + file.name } : {}) })) };
    return { state, files, next, newFiles };
}

test('repackaging preserves the round and survives interruption at every request boundary', async () => {
    const fixture = legacyPlayers();
    const baseline = new FakeGitHub();
    await publish(baseline, fixture.state, fixture.files);
    baseline.splitStorage = true; baseline.calls = [];
    await publish(baseline, fixture.next, fixture.newFiles, { repackage: true });
    const boundaries = baseline.calls.length;
    for (const after of [false, true]) for (let at = 1; at <= boundaries; at++) {
        const api = new FakeGitHub();
        await publish(api, fixture.state, fixture.files);
        api.splitStorage = true; api.calls = []; api.failAt = at; api.after = after;
        try { await publish(api, fixture.next, fixture.newFiles, { repackage: true }); } catch { }
        api.failAt = null;
        await recover(api);
        await publish(api, fixture.next, fixture.newFiles, { repackage: true });
        assert.equal(api.manifest.round, 1);
        assert.equal(api.list.filter(asset => asset.storage === 'public').length, 3);
        assert.equal(api.list.filter(asset => asset.storage === 'private').length, 2);
        assert(api.list.filter(asset => asset.storage === 'public').every(asset => asset.name.endsWith('.zip')));
        assert.deepEqual(api.manifest, fixture.next);
    }
});

test('repackaging rejects changed executable bytes and preserves old published players', async () => {
    const fixture = legacyPlayers(), api = new FakeGitHub();
    await publish(api, fixture.state, fixture.files);
    api.splitStorage = true;
    fixture.newFiles[0].data = playerZip(fixture.files[0].name, Buffer.from('changed'));
    fixture.next.files[0].sha256 = hash(fixture.newFiles[0].data);
    await assert.rejects(publish(api, fixture.next, fixture.newFiles, { repackage: true }), /changed player bytes/);
    assert.deepEqual(api.manifest, fixture.state);
    assert.equal(api.list.length, 5);
});

test('daily updates after repackaging keep only three public ZIPs and two private state files', async () => {
    const fixture = legacyPlayers(), api = new FakeGitHub();
    await publish(api, fixture.state, fixture.files);
    api.splitStorage = true;
    await publish(api, fixture.next, fixture.newFiles, { repackage: true });
    const newFiles = fixture.newFiles.map(file => ({ ...file, name: file.name.replace('2026-10-12', '2026-10-13') }));
    const next = { ...fixture.next, date: '2026-10-13', round: 2, files: fixture.next.files.map(file => ({ ...file, name: file.name.replace('2026-10-12', '2026-10-13') })) };
    await publish(api, next, newFiles);
    assert.deepEqual(await recover(api), next);
    assert.equal(api.list.filter(asset => asset.storage === 'public').length, 3);
    assert.equal(api.list.filter(asset => asset.storage === 'private').length, 2);
    assert(api.list.every(asset => asset.name.includes('2026-10-13')));
});

test('extra same-date rounds are explicitly fenced and recover at every request boundary', async () => {
    const fixture = legacyPlayers();
    const newFiles = fixture.newFiles.map(file => ({ ...file, name: file.name.replace('2026-10-12', '2026-10-12-round-2') }));
    const next = { ...fixture.next, round: 2, previous: { date: fixture.state.date, round: 1, answers: { easy: 'old-answer' } }, files: fixture.next.files.map(file => ({ ...file, name: file.name.replace('2026-10-12', '2026-10-12-round-2') })) };
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
        assert.equal(api.list.filter(asset => asset.storage === 'public').length, 3);
        assert.equal(api.list.filter(asset => asset.storage === 'private').length, 2);
        assert(api.list.every(asset => !asset.name.startsWith('transaction-')));
        const mutations = api.calls.filter(name => name.startsWith('upload:') || ['body', 'commit', 'delete'].includes(name)).length;
        await publish(api, next, newFiles, { advanceFromRound: 1 });
        assert.equal(api.calls.filter(name => name.startsWith('upload:') || ['body', 'commit', 'delete'].includes(name)).length, mutations);
    }
    const collision = { ...next, files: fixture.next.files };
    await assert.rejects(publish(new FakeGitHub(), collision, fixture.newFiles, { advanceFromRound: 0 }), /Invalid manual/);
});
