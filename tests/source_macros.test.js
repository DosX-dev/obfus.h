'use strict';
const assert = require('node:assert/strict');
const { macroDefinition } = require('./source_macros');

exports.verify = function () {
    for (const eol of ['\n', '\r\n']) {
        const source = ['#define SAMPLE_EXTRA 0', '#define SAMPLE(x) \\', '    ((x) + 1)',
            '#define SAMPLE 2', '#define EMPTY', ''].join(eol);
        const first = macroDefinition(source, 'SAMPLE');
        assert.equal(first.body, ['#define SAMPLE(x) \\', '    ((x) + 1)'].join(eol) + (eol === '\r\n' ? '\r' : ''));
        assert.equal(source.slice(first.start, first.end), first.body);
        assert.equal(macroDefinition(source, 'SAMPLE', { last: true }).body.trimEnd(), '#define SAMPLE 2');
        assert.equal(macroDefinition(source, 'SAMPLE', { from: first.end }).body.trimEnd(), '#define SAMPLE 2');
        assert.equal(macroDefinition(source, 'EMPTY').body.trimEnd(), '#define EMPTY');
    }
    assert.equal(macroDefinition('#define X 1', 'X').body, '#define X 1');
    assert.throws(() => macroDefinition('#define XY 1', 'X'), /Missing macro/);
    assert.throws(() => macroDefinition('#define X \\', 'X'), /Unterminated macro/);
    assert.throws(() => macroDefinition('', 'X.*'), /Invalid macro/);
};
