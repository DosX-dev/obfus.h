import test from 'node:test';
import assert from 'node:assert/strict';
import fs from 'node:fs/promises';
import path from 'node:path';
import { GAMIFICATION_ROOT } from '../src/core/paths.js';

test('module dependencies preserve generation, verification and publication boundaries', async () => {
    const root = path.join(GAMIFICATION_ROOT, 'src');
    const allowed = {
        core: ['core'],
        generation: ['core', 'generation'],
        build: ['core', 'generation', 'build'],
        verification: ['core', 'generation', 'build', 'verification'],
        round: ['core', 'generation', 'build', 'verification', 'round'],
        publication: ['core', 'publication']
    };
    for (const [layer, dependencies] of Object.entries(allowed)) {
        const directory = path.join(root, layer);
        for (const name of (await fs.readdir(directory)).filter(name => name.endsWith('.js'))) {
            const file = path.join(directory, name);
            const source = await fs.readFile(file, 'utf8');
            for (const [, specifier] of source.matchAll(/from\s+['"]([^'"]+)['"]/g)) {
                if (specifier.startsWith('node:')) continue;
                assert(specifier.startsWith('.'), 'Runtime dependencies must use Node built-ins or local modules');
                const target = path.resolve(directory, specifier);
                const relative = path.relative(root, target);
                assert(!relative.startsWith('..') && !path.isAbsolute(relative), 'Module imports escaped src');
                assert(dependencies.includes(relative.split(path.sep)[0]), `${layer}/${name} crossed a layer boundary`);
                assert((await fs.stat(target)).isFile(), 'Missing imported module');
            }
        }
    }
});
