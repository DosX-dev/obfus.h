'use strict';
const fs = require('node:fs');
const path = require('node:path');
const options = [
    ['NO_OBF', 'OBFH_DISABLE_ANY_OBFUSCATION'], ['NO_CFLOW', 'OBFH_DISABLE_CONTROL_FLOW'],
    ['NO_ANTIDEBUG', 'OBFH_DISABLE_ANTIDEBUG'], ['NO_PDATA_DECOYS', 'OBFH_DISABLE_PDATA_DECOYS'],
    ['VIRT', 'OBFH_VIRTUALIZATION'],
    ['CFLOW_V2', 'OBFH_BETTER_CONTROL_FLOW'], ['ANTIDEBUG_V2', 'OBFH_BETTER_ANTIDEBUG'],
    ['FAKE_SIGNS', 'OBFH_FAKE_SIGNATURES'], ['MEM_CLEANER__JUST_FOR_FUN', 'OBFH_MEMORY_CLEANER'],
    ['OBFH_BUILD_SEED', 'OBFH_SEED']
];
exports.runSuite = async function({ arch, compiler, directory, source, check, run, assert }) {
    await check(`${arch}/public options: defaults, legacy warnings and new-name precedence`, async () => {
        const boundary = source.indexOf('// Platform declarations');
        assert(boundary > 0, 'configuration boundary missing');
        const file = path.join(directory, arch + '-public-options.c');
        const observed = options.map(([, name]) => `#ifdef ${name}\n#define OBSERVED_${name} ${name}\n#else\n#define OBSERVED_${name} 0\n#endif\n`).join('');
        fs.writeFileSync(file, source.slice(0, boundary) + '\n#endif\n' + observed + 'OPTIONS ' + options.map(([, name]) => 'OBSERVED_' + name).join(' ') + '\n');
        const preprocess = async flags => {
            const result = await run(compiler, ['-E', ...flags.map(flag => '-D' + flag), file]);
            assert(result.status === 0, result.stderr);
            const values = result.stdout.match(/^OPTIONS (.*)$/m)?.[1].trim().split(/\s+/).map(value => Number(value.replace(/u$/i, '')));
            assert(values?.length === options.length, 'option values missing');
            return { values, stderr: result.stderr };
        };
        const defaults = await preprocess([]);
        assert(defaults.values.every(value => value === 0), 'default protection options changed');
        assert(!defaults.stderr.includes('deprecated'), 'default build emitted a legacy warning');
        for (const [index, [old, name]] of options.entries()) {
            const value = name === 'OBFH_SEED' ? 4294967295 : 1;
            const current = await preprocess([`${name}=${value}u`]);
            assert(current.values[index] === value && !current.stderr.includes('deprecated'), 'new option failed: ' + name);
            const legacy = await preprocess([`${old}=${value}u`]);
            assert(legacy.values[index] === value && legacy.stderr.includes(old + ' is deprecated'), 'legacy alias/warning failed: ' + old);
            const conflict = await preprocess([`${old}=${value}u`, `${name}=0u`]);
            assert(conflict.values[index] === 0 && conflict.stderr.includes(old + ' is deprecated'), 'new zero must override legacy value: ' + name);
        }
        const obsolete = await preprocess(['virt_std=1']);
        assert(obsolete.values.every(value => value === 0) && obsolete.stderr.includes('virt_std is obsolete'), 'obsolete math option must warn without enabling virtualization');
    });
};
