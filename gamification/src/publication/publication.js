import { hash } from '../core/crypto.js';
import { RELEASE_TAG } from '../core/config.js';
import { validateState, stateEqual } from './state.js';
import { description } from './description.js';

async function verifyAssets(api, release, state) {
    const assets = await api.assets(release.id);
    for (const file of state.files) {
        const asset = assets.find((a) => a.name === file.name);
        if (!asset || hash(await api.download(asset)) !== file.sha256)
            throw new Error('Published asset integrity failure');
    }
}
async function finish(api, release, transaction) {
    validateState(transaction.next);
    if (transaction.journal !== `transaction-${transaction.next.date}.json`) throw new Error('Journal date mismatch');
    const current = await api.readManifest();
    if (current) validateState(current);
    if (!stateEqual(current, transaction.next) && (current ? transaction.next.round !== current.round + 1 || transaction.next.date <= current.date : transaction.next.round !== 1)) throw new Error('Invalid recovery sequence');
    await verifyAssets(api, release, transaction.next);
    // The journal is durable before either visible pointer changes. Retrying is idempotent.
    await api.updateRelease(release.id, {
        name: 'Daily Crackmes',
        body: description(transaction.next),
        prerelease: true,
        make_latest: 'false'
    });
    if (!stateEqual(await api.readManifest(), transaction.next)) await api.writeManifest(transaction.next);
    const keep = new Set(transaction.next.files.map((f) => f.name));
    for (const asset of await api.assets(release.id)) {
        if (
            !keep.has(asset.name) &&
            /^(obfus-crackme-|TASK-|SHA256SUMS-|manifest-|answers-|history-|transaction-)/.test(asset.name)
        ) {
            // Keep the recovery journal until all obsolete assets are gone.
            if (asset.name !== transaction.journal) await api.deleteAsset(asset.id);
        }
    }
    const journal = (await api.assets(release.id)).find((a) => a.name === transaction.journal);
    if (journal) await api.deleteAsset(journal.id);
    return transaction.next;
}
export async function recover(api) {
    const release = await api.release();
    if (!release) {
        if (await api.readManifest()) throw new Error('Published manifest exists but the daily release is missing; refusing to reset history');
        return null;
    }
    if (release.immutable) throw new Error('Daily release must be mutable');
    const journals = (await api.assets(release.id)).filter((a) => /^transaction-\d{4}-\d{2}-\d{2}\.json$/.test(a.name));
    if (journals.length > 1) throw new Error('Multiple publication journals; manual inspection required');
    if (journals.length) {
        const transaction = JSON.parse((await api.download(journals[0])).toString());
        if (transaction.version !== 1 || transaction.journal !== journals[0].name)
            throw new Error('Invalid publication journal');
        return finish(api, release, transaction);
    }
    const state = await api.readManifest();
    if (state) {
        validateState(state);
        await verifyAssets(api, release, state);
    } else if (release.body !== 'Preparing the first verified round.') {
        throw new Error('Daily release state is incomplete; refusing to reset history');
    }
    return state;
}
export async function publish(api, state, files) {
    validateState(state);
    if (!Array.isArray(files) || files.length !== state.files.length || new Set(files.map(file => file.name)).size !== files.length || files.some(file => state.files.find(entry => entry.name === file.name)?.sha256 !== hash(file.data))) throw new Error('Publication bundle does not match its manifest');
    let release = await api.release();
    if (release?.immutable) throw new Error('Daily release must be mutable');
    if (!release) {
        if (await api.readManifest()) throw new Error('Published manifest exists but the daily release is missing; refusing to reset history');
        release = await api.createRelease({
            tag_name: RELEASE_TAG,
            name: 'Daily Crackmes',
            prerelease: true,
            make_latest: 'false',
            body: 'Preparing the first verified round.'
        });
    }
    const current = await recover(api);
    if (current?.date === state.date) return current;
    if (current && (state.date < current.date || state.round !== current.round + 1))
        throw new Error('Invalid publication sequence');
    if (!current && state.round !== 1) throw new Error('First round must be 1');
    const journal = `transaction-${state.date}.json`;
    let durable = false;
    try {
        // Clean orphan uploads from an interruption before a journal was written.
        const active = new Set(current?.files.map((f) => f.name) || []);
        for (const asset of await api.assets(release.id)) {
            if (!active.has(asset.name) && files.some((f) => f.name === asset.name)) await api.deleteAsset(asset.id);
        }
        for (const file of files) {
            const a = await api.upload(release.id, file.name, file.data);
            if (hash(await api.download(a)) !== hash(file.data)) throw new Error('Uploaded asset checksum mismatch');
        }
        await verifyAssets(api, release, state);
        const transaction = { version: 1, journal, next: state };
        await api.upload(release.id, journal, Buffer.from(JSON.stringify(transaction)));
        durable = true;
        return await finish(api, release, transaction);
    } catch (error) {
        // An upload can succeed even if its response was lost. Look for the journal.
        durable ||= (await api.assets(release.id)).some((a) => a.name === journal);
        // Deleting the final journal can succeed while its HTTP response is lost.
        // In that case the committed manifest is the durable completion record.
        durable ||= stateEqual(await api.readManifest(), state);
        if (!durable)
            for (const asset of await api.assets(release.id)) {
                if (files.some((f) => f.name === asset.name) && !current?.files.some((f) => f.name === asset.name))
                    await api.deleteAsset(asset.id);
            }
        // With a durable complete journal, leave the old set intact and resume next run.
        throw error;
    }
}
