export const VERSION = 5;
export const LEVELS = ['easy', 'medium', 'hard', 'extreme'];
export const TOOLCHAIN = Object.freeze({
    repository: 'DosX-dev/TinyCC-builder',
    api: 'https://api.github.com/repos/DosX-dev/TinyCC-builder/releases/latest'
});
export const ALPHABET = 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789!#@$%&*?+-_=';
export const RELEASE_TAG = 'daily-crackme';

export const LEVEL_LABELS = Object.freeze({ easy: 'EASY', medium: 'MEDIUM', hard: 'HARDCORE', extreme: 'EXTREME' });
export const levelsForVersion = version => version >= 5 ? LEVELS : LEVELS.slice(0, 3);
