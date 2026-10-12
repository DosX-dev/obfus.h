'use strict';
const fs = require('node:fs');
const path = require('node:path');
const assert = require('node:assert/strict');
const semantics = require('./vm_semantics');

async function verify(fixturesRoot = __dirname) {
    const generated = semantics.fixture(fixturesRoot);
    const header = generated.indexOf('#include "../include/obfus.h"');
    assert.equal(generated.match(/#include "\.\.\/include\/obfus.h"/g).length, 1);
    for (const [name] of semantics.contracts) {
        const original = fs.readFileSync(path.join(fixturesRoot, name + '.c'), 'utf8').replace(/\r\n/g, '\n');
        const boundary = original.indexOf('#include "../include/obfus.h"');
        const prefix = original.slice(0, boundary);
        assert(generated.indexOf(prefix) < header, name + ': native references crossed protection boundary');
        const line = original.slice(0, original.indexOf('\n', boundary) + 1).split('\n').length;
        const location = '#line ' + line + ' ' + JSON.stringify(path.join(fixturesRoot, name + '.c').replace(/\\/g, '/'));
        assert(generated.includes(location), name + ': original source lines changed');
        assert(generated.includes('static int ' + name + '_contract_main(void)'), name + ': missing entrypoint');
        // All original success diagnostics must survive the source composition.
        for (const literal of original.match(/"(?:\\.|[^"\\])*"/g) || []) {
            if (!literal.includes('../include/obfus.h')) assert(generated.includes(literal), name + ': diagnostic altered');
        }
    }
    assert.equal(generated.match(/^#undef CHECK$/gm).length, 3);
    assert.equal(generated.match(/^#undef Obfh_VirtualMachine$/gm).length, 3);
    assert.equal(generated.match(/^#undef ROUTE$/gm).length, 3);
    const dispatcher = generated.indexOf('#line 1 "vm_semantics_dispatch.c"');
    for (const name of ['if', 'else', 'return', 'case', 'default', 'continue', 'do', 'break', 'switch', 'while', 'for'])
        assert(generated.indexOf('#undef ' + name + '\n') < dispatcher, name + ': dispatcher is still protected');
    const calls = [];
    await assert.rejects(semantics.executeContracts('fixture', async (exe, marker, args) => {
        calls.push(args[0]);
        if (args[0] !== '1') throw new Error('oracle failure ' + marker);
    }), error => error instanceof AggregateError && error.errors.length === 2
        && error.message.includes('numeric:') && error.message.includes('vm:'));
    assert.deepEqual(calls, ['0', '1', '2'], 'one failed contract suppressed another');
    const deadline = Object.assign(new Error('deadline'), { suiteDeadline: true });
    await assert.rejects(semantics.executeContracts('fixture', async () => { throw deadline; }), error => error === deadline);
}

module.exports = { verify };
