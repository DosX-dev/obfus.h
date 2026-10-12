import { isDeepStrictEqual } from 'node:util';

export function validateState(state) {
    if (!state) throw new Error('Invalid publication state');
    const validRound = Number.isSafeInteger(state.round) && state.round >= 1;
    const validDate = /^\d{4}-\d{2}-\d{2}$/.test(state.date)
        && new Date(state.date + 'T00:00:00Z').toISOString().slice(0, 10) === state.date;
    const validFiles = Array.isArray(state.files) && state.files.length > 0;
    if (!validRound || !validDate || !validFiles) throw new Error('Invalid publication state');
    const names = new Set();
    for (const file of state.files) {
        if (
            !file
            || !/^[A-Za-z0-9][A-Za-z0-9._-]{0,199}$/.test(file.name)
            || !/^[a-f0-9]{64}$/.test(file.sha256)
            || names.has(file.name)
        ) throw new Error('Invalid publication asset metadata');
        names.add(file.name);
    }
}
export const stateEqual = (a, b) =>
    !!(a && b && isDeepStrictEqual(a, b));
