// Durable round/answer schema stays stable when generation rules change.
export const VERSION = 6;
export const GENERATOR_VERSION = 7;
export const LEVELS = ['easy', 'medium', 'hardcore', 'extreme'];
export const TOOLCHAIN = Object.freeze({
    repository: 'DosX-dev/TinyCC-builder',
    api: 'https://api.github.com/repos/DosX-dev/TinyCC-builder/releases/latest'
});
export const ALPHABET = 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789!#@$%&*?+-_=';
export const RELEASE_TAG = 'daily-crackme';

export const LEVEL_LABELS = Object.freeze({ easy: 'EASY', medium: 'MEDIUM', hardcore: 'HARDCORE', extreme: 'EXTREME' });
