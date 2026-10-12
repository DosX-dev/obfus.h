import path from 'node:path';
import { fileURLToPath } from 'node:url';

// Resolve against the module, not the shell's working directory.
export const REPOSITORY_ROOT = path.resolve(fileURLToPath(new URL('../../../', import.meta.url)));
export const GAMIFICATION_ROOT = path.resolve(fileURLToPath(new URL('../../', import.meta.url)));
