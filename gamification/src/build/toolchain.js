import fs from 'node:fs/promises';
import path from 'node:path';
import { TOOLCHAIN } from '../core/config.js';
import { hash } from '../core/crypto.js';
import { run } from '../core/process.js';
import { GAMIFICATION_ROOT } from '../core/paths.js';

export function selectArchive(release) {
    if (!release || release.draft !== false || release.prerelease !== false || !Number.isSafeInteger(release.id) || typeof release.tag_name !== 'string' || !Array.isArray(release.assets)) throw new Error('Invalid latest compiler release');
    const archives = release.assets.filter(asset => /^tinycc-windows-\d{4}-\d{2}-\d{2}\.zip$/.test(asset.name));
    if (archives.length !== 1) throw new Error('Latest release must contain exactly one ordinary Windows ZIP without UPX');
    const asset = archives[0];
    const url = `https://github.com/${TOOLCHAIN.repository}/releases/download/${encodeURIComponent(release.tag_name)}/${asset.name}`;
    if (asset.browser_download_url !== url || !Number.isSafeInteger(asset.id) || !Number.isSafeInteger(asset.size) || asset.size <= 0 || asset.size > 128 * 1024 * 1024 || !/^sha256:[a-f0-9]{64}$/.test(asset.digest)) throw new Error('Compiler asset URL, size or SHA-256 digest is invalid');
    return { repository: TOOLCHAIN.repository, release: release.tag_name, releaseId: release.id, assetId: asset.id, name: asset.name, size: asset.size, url, sha256: asset.digest.slice(7) };
}
async function download(url, limit, fetcher) {
    const response = await fetcher(url, { headers: { Accept: 'application/vnd.github+json', 'User-Agent': 'obfus-daily-crackme' }, signal: AbortSignal.timeout(180000) });
    if (!response.ok) throw new Error(`Compiler download failed (${response.status})`);
    const chunks = [];
    let size = 0;
    for await (const chunk of response.body) {
        size += chunk.length;
        if (size > limit) throw new Error('Compiler response exceeds size limit');
        chunks.push(chunk);
    }
    return Buffer.concat(chunks);
}
export async function resolveLatest(fetcher = fetch) {
    return selectArchive(JSON.parse(await download(TOOLCHAIN.api, 4 * 1024 * 1024, fetcher)));
}
export async function verifiedArchive(settings, fetcher = fetch) {
    const data = await download(settings.url, settings.size, fetcher);
    if (data.length !== settings.size || hash(data) !== settings.sha256) throw new Error('Latest compiler size or checksum mismatch');
    return data;
}
async function find(directory) {
    const list = await fs.readdir(directory, { withFileTypes: true });
    if (list.some(f => f.name === 'tcc.exe') && list.some(f => f.name === 'i386-win32-tcc.exe')) return directory;
    for (const entry of list) if (entry.isDirectory()) {
        const result = await find(path.join(directory, entry.name));
        if (result) return result;
    }
    return null;
}
async function inspect(directory) {
    const versions = {};
    for (const [arch, name] of [['x64', 'tcc.exe'], ['x86', 'i386-win32-tcc.exe']]) {
        const result = await run(path.join(directory, name), ['-v']);
        const version = (result.stdout + result.stderr).trim();
        if (result.code !== 0 || !/\btcc version \S+/.test(version)) throw new Error(`Invalid ${arch} compiler version response`);
        versions[arch] = version;
    }
    return versions;
}
export async function provision(explicit, { fetcher = fetch } = {}) {
    if (explicit) {
        const directory = path.resolve(explicit);
        return { directory, metadata: { source: 'local', versions: await inspect(directory) } };
    }
    // One resolution per run: both architectures use the same verified archive.
    const settings = await resolveLatest(fetcher);
    const root = path.resolve(GAMIFICATION_ROOT, 'toolchain');
    const zip = path.join(root, settings.sha256 + '.zip'), expanded = path.join(root, 'unpacked');
    await fs.mkdir(root, { recursive: true });
    let data = await fs.readFile(zip).catch(() => null);
    if (!data || data.length !== settings.size || hash(data) !== settings.sha256) {
        data = await verifiedArchive(settings, fetcher);
        await fs.writeFile(zip, data);
    }
    const relative = path.relative(root, expanded);
    if (!relative || path.isAbsolute(relative) || relative.startsWith('..') || (await fs.lstat(expanded).catch(() => null))?.isSymbolicLink()) throw new Error('Unsafe toolchain extraction directory');
    await fs.rm(expanded, { recursive: true, force: true });
    const result = await run('powershell.exe', ['-NoProfile', '-ExecutionPolicy', 'Bypass', '-File', path.join(GAMIFICATION_ROOT, 'src/build/unpack.ps1'), '-Archive', zip, '-Destination', expanded]);
    if (result.code !== 0) throw new Error('Toolchain extraction failed');
    const directory = await find(expanded);
    if (!directory) throw new Error('Toolchain executables absent');
    return { directory, metadata: { ...settings, versions: await inspect(directory) } };
}
