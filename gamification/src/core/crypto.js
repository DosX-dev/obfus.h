import { createHmac, createHash, createCipheriv, createDecipheriv, randomBytes } from 'node:crypto';
import { VERSION } from './config.js';
export const hash = (data) => createHash('sha256').update(data).digest('hex');
// Answer storage is a durable protocol, independent of future generator versions.
const ANSWER_VERSION = 1;
const answerKey = key => createHmac('sha256', key).update(JSON.stringify([ANSWER_VERSION, 'answers-key', []])).digest();
export function masterKey(text) {
    if (!/^[a-fA-F0-9]{64}$/.test(text || '')) throw new Error('CRACKME_MASTER_KEY must be 64 hexadecimal characters');
    return Buffer.from(text, 'hex');
}
export function derive(key, domain, context) {
    return createHmac('sha256', key)
        .update(JSON.stringify([VERSION, domain, context]))
        .digest();
}
export function rng(key, domain, context) {
    let counter = 0,
        buffer = Buffer.alloc(0),
        at = 0;
    return (max) => {
        if (!Number.isSafeInteger(max) || max < 1 || max > 0x100000000) throw new Error('Invalid random range');
        const bound = Math.floor(0x100000000 / max) * max;
        for (; ;) {
            if (at + 4 > buffer.length) {
                buffer = derive(key, domain, [context, counter++]);
                at = 0;
            }
            const n = buffer.readUInt32LE(at);
            at += 4;
            if (n < bound) return n % max;
        }
    };
}
export function seal(key, value) {
    const iv = randomBytes(12),
        cipher = createCipheriv('aes-256-gcm', answerKey(key), iv);
    cipher.setAAD(Buffer.from('obfus-daily-answers-v1'));
    const data = Buffer.concat([cipher.update(JSON.stringify(value)), cipher.final()]);
    return {
        version: ANSWER_VERSION,
        iv: iv.toString('hex'),
        tag: cipher.getAuthTag().toString('hex'),
        data: data.toString('base64')
    };
}
export function unseal(key, envelope) {
    if (envelope.version !== ANSWER_VERSION) throw new Error('Unsupported answer envelope');
    const cipher = createDecipheriv('aes-256-gcm', answerKey(key), Buffer.from(envelope.iv, 'hex'));
    cipher.setAAD(Buffer.from('obfus-daily-answers-v1'));
    cipher.setAuthTag(Buffer.from(envelope.tag, 'hex'));
    return JSON.parse(Buffer.concat([cipher.update(Buffer.from(envelope.data, 'base64')), cipher.final()]).toString());
}
