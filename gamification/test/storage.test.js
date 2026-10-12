import test from 'node:test';
import assert from 'node:assert/strict';
import { GitHub } from '../src/publication/github.js';

function backend() {
    const api = new GitHub('example/project', 'fixture-token'), releases = [], uploads = [];
    api.request = async (url, options = {}) => {
        if (options.method === 'POST' && url.endsWith('/releases')) {
            const value = { id: 2, ...options.body }; releases.push(value); return value;
        }
        if (url.includes('/releases?')) return releases;
        if (url.includes('/assets?') && !options.method) return url.includes('/releases/2/') ? [{ id: 22, name: 'history-old.json.gz' }] : [{ id: 11, name: 'old.exe' }];
        if (url.startsWith('https://uploads.github.com/')) { uploads.push(url); return { id: 30 }; }
        throw new Error('Unexpected fixture request');
    };
    return { api, releases, uploads };
}

test('only player downloads enter the public release; state and journals enter a draft', async () => {
    const { api, releases, uploads } = backend();
    await api.upload(1, 'obfus-crackme-easy.zip', Buffer.from('player'));
    assert(uploads[0].includes('/releases/1/'));
    for (const name of ['answers-new.json', 'history-new.json.gz', 'transaction-new.json']) await api.upload(1, name, Buffer.from('state'));
    assert(uploads.slice(1).every(url => url.includes('/releases/2/')));
    assert.equal(releases.length, 1);
    assert.equal(releases[0].draft, true);
    assert.equal(releases[0].tag_name, 'daily-crackme-state');
    const assets = await api.assets(1);
    assert.deepEqual(assets.map(asset => asset.storage), ['public', 'private']);
});

test('published or immutable state storage is refused before an upload', async () => {
    for (const entry of [{ draft: false }, { draft: true, immutable: true }]) {
        const { api, releases, uploads } = backend();
        releases.push({ id: 2, tag_name: 'daily-crackme-state', ...entry });
        await assert.rejects(api.upload(1, 'answers-new.json', Buffer.from('state')), /mutable draft/);
        assert.equal(uploads.length, 0);
    }
});

test('a lost draft creation response is recovered without a second state release', async () => {
    const { api, releases } = backend(), request = api.request;
    let lost = true;
    api.request = async (url, options = {}) => {
        const value = await request(url, options);
        if (lost && options.method === 'POST' && url.endsWith('/releases')) { lost = false; throw new Error('lost response'); }
        return value;
    };
    await assert.rejects(api.storageRelease(true), /lost response/);
    assert.equal((await api.storageRelease(true)).id, 2);
    assert.equal(releases.length, 1);
});
