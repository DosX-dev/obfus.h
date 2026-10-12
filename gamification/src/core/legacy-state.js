// Only persisted pre-v6 rounds use this name. It is not a generator input alias.
const legacyLevel = 'hard';
export const canonicalLevel = level => level === legacyLevel ? 'hardcore' : level;
export function savedLevels(version, current) {
    if (version >= 6) return current;
    const previous = ['easy', 'medium', legacyLevel];
    return version >= 5 ? [...previous, 'extreme'] : previous;
}
