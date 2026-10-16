import test from 'node:test';
import assert from 'node:assert/strict';
import { selectArchive, resolveLatest, verifiedArchive } from '../src/build/toolchain.js';
import { TOOLCHAIN } from '../src/core/config.js';
import { hash } from '../src/core/crypto.js';

const data = Buffer.from('fixture ordinary compiler ZIP');
const asset = { id: 23, name: 'tinycc-windows-2026-10-13.zip', size: data.length, digest: 'sha256:' + hash(data), browser_download_url: 'https://github.com/DosX-dev/TinyCC-builder/releases/download/2026-10-13/tinycc-windows-2026-10-13.zip' };
const release = () => ({ id: 12, tag_name: '2026-10-13', draft: false, prerelease: false, assets: [asset, { ...asset, name: 'tinycc-windows-upx-2026-10-13.zip' }] });

test('compiler metadata uses the runner token without sending it to archive hosts', async () => {
    const previous = process.env.GITHUB_TOKEN;
    process.env.GITHUB_TOKEN = 'fixture-token';
    try {
        const settings = await resolveLatest(async (url, options) => {
            assert.equal(options.headers.Authorization, 'Bearer fixture-token');
            return Response.json(release());
        });
        await verifiedArchive(settings, async (url, options) => {
            assert.equal(options.headers.Authorization, undefined);
            assert.equal(options.headers.Accept, 'application/octet-stream');
            return new Response(data);
        });
        delete process.env.GITHUB_TOKEN;
        await resolveLatest(async (url, options) => {
            assert.equal(options.headers.Authorization, undefined);
            return Response.json(release());
        });
        await assert.rejects(resolveLatest(async () => new Response('', { status: 403, headers: { 'x-ratelimit-remaining': '0' } })), /API rate limit exhausted/);
    } finally {
        if (previous === undefined) delete process.env.GITHUB_TOKEN;
        else process.env.GITHUB_TOKEN = previous;
    }
});

test('latest release selects only the ordinary ZIP and records its actual identity', async () => {
    const settings = await resolveLatest(async url => {
        assert.equal(url, TOOLCHAIN.api);
        return Response.json(release());
    });
    assert.equal(settings.assetId, 23);
    assert.equal(settings.release, '2026-10-13');
    assert.equal(settings.sha256, hash(data));
    assert(!settings.name.includes('upx'));
    assert.deepEqual(await verifiedArchive(settings, async () => new Response(data)), data);
});

test('missing, ambiguous, prerelease and unauthenticated compiler metadata fail closed', () => {
    for (const change of [
        r => { r.assets = r.assets.filter(a => a.name.includes('upx')); },
        r => r.assets.push(asset),
        r => { r.prerelease = true; },
        r => { r.draft = true; },
        r => { r.assets[0] = { ...asset, digest: null }; },
        r => { r.assets[0] = { ...asset, browser_download_url: 'https://example.test/tcc.zip' }; },
        r => { r.assets[0] = { ...asset, size: 129 * 1024 * 1024 }; }
    ]) {
        const r = release();
        change(r);
        assert.throws(() => selectArchive(r));
    }
});

test('network, malformed JSON, oversized responses and changed archive contents abort preparation', async () => {
    await assert.rejects(resolveLatest(async () => new Response('', { status: 503 })), /download failed/);
    await assert.rejects(resolveLatest(async () => { throw new Error('network down'); }), /network down/);
    await assert.rejects(resolveLatest(async () => new Response('{')), SyntaxError);
    const settings = selectArchive(release());
    await assert.rejects(verifiedArchive(settings, async () => new Response(Buffer.alloc(data.length))), /checksum mismatch/);
    await assert.rejects(verifiedArchive(settings, async () => new Response(data.subarray(1))), /size or checksum/);
    await assert.rejects(verifiedArchive(settings, async () => new Response(Buffer.alloc(data.length + 1))), /size limit/);
});
