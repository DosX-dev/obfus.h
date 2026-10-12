import { RELEASE_TAG } from '../core/config.js';
export class GitHub {
    constructor(repository, token, { fetcher = fetch, sleep = ms => new Promise(resolve => setTimeout(resolve, ms)) } = {}) {
        if (!/^[\w.-]+\/[\w.-]+$/.test(repository || '') || !token) throw new Error('Repository/token missing');
        this.repository = repository;
        this.token = token;
        this.base = `https://api.github.com/repos/${repository}`;
        this.fetcher = fetcher;
        this.sleep = sleep;
    }
    async request(url, { method = 'GET', body, raw = false, binary = false, optional = false } = {}) {
        const parsed = new URL(url);
        if (!['api.github.com', 'uploads.github.com'].includes(parsed.hostname) || parsed.protocol !== 'https:')
            throw new Error('Unexpected API host');
        // Retry reads only. A lost mutation response must be resolved by the publication journal.
        for (let attempt = 0; ; attempt++) {
            let response;
            try {
                response = await this.fetcher(url, {
                    method,
                    signal: AbortSignal.timeout(60000),
                    headers: {
                        Authorization: `Bearer ${this.token}`,
                        Accept: raw ? 'application/octet-stream' : 'application/vnd.github+json',
                        'X-GitHub-Api-Version': '2022-11-28',
                        ...(body ? { 'Content-Type': binary ? 'application/octet-stream' : 'application/json' } : {})
                    },
                    body: body ? (binary ? body : JSON.stringify(body)) : undefined
                });
            } catch {
                if (method !== 'GET' || attempt >= 2) throw new Error(`GitHub ${method} transport failed`);
                await this.sleep(250 * 4 ** attempt);
                continue;
            }
            if (optional && response.status === 404) return null;
            if (method === 'GET' && attempt < 2 && ([429, 502, 503, 504].includes(response.status) || response.status === 403 && response.headers.has('retry-after'))) {
                const retry = response.headers.get('retry-after');
                const seconds = retry && /^\d+$/.test(retry) ? Number(retry) : retry ? Math.max(0, (Date.parse(retry) - Date.now()) / 1000) : 0;
                if (seconds > 60) throw new Error('GitHub retry delay exceeds bounded read budget');
                await response.body?.cancel();
                await this.sleep(Math.max(250 * 4 ** attempt, Number.isFinite(seconds) ? seconds * 1000 : 0));
                continue;
            }
            if (!response.ok) throw new Error(`GitHub ${method} failed (${response.status})`);
            if (response.status === 204) return null;
            try {
                return raw ? Buffer.from(await response.arrayBuffer()) : await response.json();
            } catch {
                if (method !== 'GET' || attempt >= 2) throw new Error(`GitHub ${method} response could not be read`);
                await this.sleep(250 * 4 ** attempt);
            }
        }
    }
    release() {
        return this.request(`${this.base}/releases/tags/${RELEASE_TAG}`, { optional: true });
    }
    createRelease(body) {
        return this.request(`${this.base}/releases`, { method: 'POST', body });
    }
    updateRelease(id, body) {
        return this.request(`${this.base}/releases/${id}`, { method: 'PATCH', body });
    }
    async storageRelease(create = false) {
        const tag = `${RELEASE_TAG}-state`;
        let release = null;
        for (let page = 1; ; page++) {
            const batch = await this.request(`${this.base}/releases?per_page=100&page=${page}`);
            release = batch.find(entry => entry.tag_name === tag);
            if (release || batch.length < 100) break;
        }
        if (!release && create) release = await this.createRelease({ tag_name: tag, name: 'Daily Crackmes · internal state', draft: true, prerelease: true, make_latest: 'false', body: 'Private automation state. Keep this release as a draft; do not publish or delete it.' });
        if (release && (!release.draft || release.immutable)) throw new Error('Automation state release must remain a mutable draft');
        return release;
    }
    async assetList(id) {
        const result = [];
        for (let page = 1; ; page++) {
            const batch = await this.request(`${this.base}/releases/${id}/assets?per_page=100&page=${page}`);
            result.push(...batch);
            if (batch.length < 100) return result;
        }
    }
    async assets(id) {
        const publicAssets = (await this.assetList(id)).map(asset => ({ ...asset, storage: 'public' }));
        const storage = await this.storageRelease();
        return storage ? [...publicAssets, ...(await this.assetList(storage.id)).map(asset => ({ ...asset, storage: 'private' }))] : publicAssets;
    }
    async upload(id, name, data) {
        if (/^(answers-|history-|transaction-|manifest-|SHA256SUMS-|TASK-)/.test(name)) id = (await this.storageRelease(true)).id;
        return this.request(
            `https://uploads.github.com/repos/${this.repository}/releases/${id}/assets?name=${encodeURIComponent(name)}`,
            { method: 'POST', body: data, binary: true }
        );
    }
    download(asset) {
        return this.request(`${this.base}/releases/assets/${asset.id}`, { raw: true });
    }
    deleteAsset(id) {
        return this.request(`${this.base}/releases/assets/${id}`, { method: 'DELETE' });
    }
    async contents() {
        return this.request(`${this.base}/contents/gamification/current.json`, { optional: true });
    }
    async readManifest() {
        const c = await this.contents();
        if (!c) return null;
        return JSON.parse(Buffer.from(c.content, 'base64').toString());
    }
    async writeManifest(state) {
        const previous = await this.contents();
        return this.request(`${this.base}/contents/gamification/current.json`, {
            method: 'PUT',
            body: {
                message: `Daily crackme round ${state.round} (${state.date})`,
                content: Buffer.from(JSON.stringify(state, null, 2) + '\n').toString('base64'),
                ...(previous ? { sha: previous.sha } : {})
            }
        });
    }
}
