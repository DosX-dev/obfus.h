'use strict';

// Locate a complete definition without confusing a name with its longer prefix.
// Selection is explicit because editor fallbacks and protected definitions coexist.
function macroDefinition(source, name, { from = 0, last = false } = {}) {
    if (!/^[A-Za-z_]\w*$/.test(name)) throw new Error('Invalid macro name: ' + name);
    const pattern = new RegExp('^#define[ \\t]+' + name + '(?=[ \\t(\\r\\n]|$)', 'gm');
    pattern.lastIndex = from;
    let match, selected;
    while ((match = pattern.exec(source))) {
        selected = match;
        if (!last) break;
    }
    if (!selected) throw new Error('Missing macro: ' + name);
    const start = selected.index;
    let cursor = start;
    while (cursor < source.length) {
        const newline = source.indexOf('\n', cursor);
        const end = newline < 0 ? source.length : newline;
        if (!source.slice(cursor, end).trimEnd().endsWith('\\'))
            return { start, end, body: source.slice(start, end) };
        if (newline < 0) break;
        cursor = newline + 1;
    }
    throw new Error('Unterminated macro: ' + name);
}

module.exports = { macroDefinition };
