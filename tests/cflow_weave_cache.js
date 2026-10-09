'use strict';
const fs = require('node:fs');
const path = require('node:path');

const marker = '// Generated constant pools: regenerate from the readable macros with tests/cflow_weave_cache.js.';
function definition(source, name) {
    const functionStart = source.indexOf('#define ' + name + '(');
    const start = functionStart < 0 ? source.indexOf('#define ' + name + ' ') : functionStart;
    if (start < 0) throw new Error('Missing macro: ' + name);
    let end = start;
    do {
        end = source.indexOf('\n', end + 1);
        if (end < 0) end = source.length;
    } while (source.slice(0, end).endsWith('\\'));
    const match = source.slice(start, end).match(/^#define \w+(?:\(([^)]*)\))?\s*([\s\S]*)$/);
    return { parameters: match[1] ? match[1].split(',').map(value => value.trim()) : [], body: match[2].replace(/\\\n/g, ' ') };
}
function selectorCache(source) {
    const begin='// BEGIN GENERATED CFLOW SELECTORS', end='// END GENERATED CFLOW SELECTORS';
    const start=source.indexOf(begin);
    if(start<0)return source;
    let text=begin+'\n// clang-format off\n';
    for(const arch of ['x64','x86']) {
        text+=arch==='x64' ? '#if defined(__x86_64__)\n' : '#else\n';
        const values={OBFH_P_WORD:arch==='x64'?'q':'l',OBFH_P_CX:arch==='x64'?'%%rcx':'%%ecx',
            OBFH_P_SCALE:arch==='x64'?'8':'4',OBFH_P_CMOV_PREFIX:arch==='x64'?'0x48,':''};
        for(const name of ['MASK','MASK_ALT','TABLE','TABLE_ALT']) {
            const expression=definition(source,'OBFH_P_'+name+'_TEXT').body
                .replace(/\b(OBFH_P_WORD|OBFH_P_CX|OBFH_P_SCALE|OBFH_P_CMOV_PREFIX)\b/g, token=>JSON.stringify(values[token]));
            const literals=expression.match(/"(?:[^"\\]|\\.)*"/g) || [];
            if(expression.replace(/"(?:[^"\\]|\\.)*"/g,'').trim())throw new Error('Nonliteral selector fragment');
            text+='#define OBFH_P_'+name+'_INSTRUCTIONS '+JSON.stringify(literals.map(literal=>JSON.parse(literal)).join(''))+'\n';
            if(name.startsWith('MASK'))
                text+='#define OBFH_P_'+name+'_SWAPPED_INSTRUCTIONS '+JSON.stringify(literals.map(literal=>JSON.parse(literal)).join('').replaceAll(values.OBFH_P_CX,arch==='x64'?'%%rdx':'%%edx'))+'\n';
        }
    }
    text+='#endif\n// clang-format on\n'+end;
    const last=source.indexOf(end,start);
    if(last<0)throw new Error('Missing selector cache boundary');
    return source.slice(0,start)+text+source.slice(last+end.length);
}
function expand(macro, args) {
    if (args.length < macro.parameters.length) throw new Error('Missing generated macro arguments');
    let body = macro.body;
    for (let i = 0; i < macro.parameters.length; ++i)
        body = body.replace(new RegExp('\\b' + macro.parameters[i] + '\\b', 'g'), args[i]);
    return body.replace(/\s*##\s*/g, '');
}
function expandSlots(body, slot) {
    const name = 'OBFH_P_SLOT(';
    for (let offset = 0;;) {
        const start = body.indexOf(name, offset);
        if (start < 0) return body;
        let depth = 1, end = start + name.length, argument = end;
        const args = [];
        for (; end < body.length; ++end) {
            if (body[end] === '(') ++depth;
            else if (body[end] === ')' && --depth === 0) {
                args.push(body.slice(argument, end).trim());
                break;
            } else if (body[end] === ',' && depth === 1) {
                args.push(body.slice(argument, end).trim());
                argument = end + 1;
            }
        }
        if (depth) throw new Error('Unbalanced slot arguments');
        const replacement = expand(slot, args);
        body = body.slice(0, start) + replacement + body.slice(end + 1);
        offset = start + replacement.length;
    }
}
// Fold only a literal condition with two parenthesized arms. No C evaluation.
function foldLiteralArms(body) {
    function closing(open) {
        let depth = 0;
        for (let i = open; i < body.length; ++i) {
            if (body[i] === '(') ++depth;
            else if (body[i] === ')' && --depth === 0) return i;
        }
        throw new Error('Unbalanced generated expression');
    }
    for (;;) {
        const match = /\(([01])\s*\?\s*\(/.exec(body);
        if (!match) return body;
        const first = match.index + match[0].length - 1, firstEnd = closing(first);
        const separator = /^\s*:\s*\(/.exec(body.slice(firstEnd + 1));
        if (!separator) throw new Error('Unsupported literal conditional');
        const second = firstEnd + separator[0].length, secondEnd = closing(second);
        const outer = /^\s*\)/.exec(body.slice(secondEnd + 1));
        if (!outer) throw new Error('Unsupported literal conditional boundary');
        const chosen = match[1] === '1' ? body.slice(first, firstEnd + 1) : body.slice(second, secondEnd + 1);
        body = body.slice(0, match.index) + chosen + body.slice(secondEnd + 1 + outer[0].length);
    }
}
function foldMetadataAliases(body) {
    // Paired slots share their operation. Keep the readable declarations above;
    // the cache can refer directly to the already declared component field.
    for (;;) {
        const match = /\b(_[CO]\d+)\s*=\s*\((_[CON]\d+)\),\s*/.exec(body);
        if (!match) break;
        body = body.slice(0, match.index) + body.slice(match.index + match[0].length);
        body = body.replace(new RegExp('\\b' + match[1] + '\\b', 'g'), match[2]);
    }
    for (;;) {
        const unused = [...body.matchAll(/\b(_[CO]\d+)\s*=[^,;{}]*,\s*/g)]
            .find(match => [...body.matchAll(new RegExp('\\b' + match[1] + '\\b', 'g'))].length === 1);
        if (!unused) break;
        body = body.slice(0, unused.index) + body.slice(unused.index + unused[0].length);
    }
    return body;
}
function compactPool(body) {
    // Cache entries contain only enum expressions. Preserve token boundaries;
    // readable macros retain their formatting and remain the editing surface.
    const token=/\s+|0[xX][\da-fA-F]+[uUlL]*|\d+[uUlL]*|[A-Za-z_]\w*|>>|<<|<=|>=|==|!=|&&|\|\||[{}()[\],;?:=+*/%&|^!~<>-]/y;
    const combined=new Set(['++','--','>>','<<','<=','>=','==','!=','&&','||','+=','-=','*=','/=','%=','&=','|=','^=','->','//','/*']);
    let result='', previous='', offset=0;
    while(offset<body.length) {
        token.lastIndex=offset;
        const match=token.exec(body);
        if(!match)throw new Error('Unexpected generated enum token: '+body.slice(offset,offset+20));
        offset=token.lastIndex;
        const next=match[0];
        if(/^\s/.test(next))continue;
        if((/\w$/.test(previous) && /^\w/.test(next)) || combined.has(previous+next))result+=' ';
        result+=next; previous=next;
    }
    return result;
}
function update(source) {
    source = source.replace(/\r\n/g, '\n');
    source = selectorCache(source);
    const pool = definition(source, 'OBFH_P_POOL'), slot = definition(source, 'OBFH_P_SLOT');
    const argumentList = definition(source, 'OBFH_P_ARGUMENTS'), input = definition(source, 'OBFH_P_INPUT');
    let cache = marker + '\n// clang-format off\n';
    for (let stage = 0; stage < 4; ++stage) {
        for (let path = 0; path < 2; ++path) {
            const id = '' + stage + path;
            let body = expand(pool, [id, '__obfh_style' + id, '__obfh_k' + id,
                '__obfh_m' + id, '__obfh_a' + id, '__obfh_r' + id, String(path), '__obfh_k' + (stage ^ 1) + '0',
                '__obfh_style' + (stage ^ 1) + '0', '__obfh_m' + (stage ^ 1) + '0',
                '__obfh_a' + (stage ^ 1) + '0', '__obfh_r' + (stage ^ 1) + '0']);
            body = body.replace(/OBFH_P_HEAD_SLOTS_([01])\(([^)]*)\)/g,
                (_, head, id) => expand(definition(source, 'OBFH_P_HEAD_SLOTS_' + head), [id]));
            body = expandSlots(body, slot);
            body = body.replace(/\(\((\d+) - 1\) \/ 2\)/g, (_, n) => String(Math.floor((Number(n) - 1) / 2)))
                .replace(/\((\d+) - 1\)/g, (_, n) => String(Number(n) - 1))
                .replace(/\((\d+) & 1\)/g, (_, n) => String(Number(n) & 1));
            body = body.replace(/\((\d+) <= 2\)/g, (_, n) => Number(n) <= 2 ? '1' : '0');
            body = foldLiteralArms(body);
            body = foldMetadataAliases(body);
            cache += `#define OBFH_P_CACHE_${id} ${compactPool(body)}\n`;
            const argumentsBody = expand(argumentList, [String(stage), String(path)])
                .replace(/OBFH_P_INPUT\((\d+),\s*(\d+)\)/g, (_, id, n) => expand(input, [id, n]))
                .replace(/\s+/g, ' ').trim();
            if (argumentsBody.includes('OBFH_P_INPUT')) throw new Error('Unexpanded ASM input');
            cache += `#define OBFH_P_ARGS_${id} ${argumentsBody}\n`;
        }
    }
    cache += '// clang-format on\n';
    const first = source.indexOf(marker), last = source.indexOf('// V1 has one graph', first);
    if (first < 0 || last < first) throw new Error('Missing CFLOW cache boundary');
    return source.slice(0, first) + cache + source.slice(last);
}
module.exports = { update };
if (require.main === module) {
    const file = path.resolve(process.argv.find(arg => arg.startsWith('--header='))?.slice(9) ||
        path.join(__dirname, '../include/obfus.h'));
    const source = fs.readFileSync(file, 'utf8'), expected = update(source);
    if (process.argv.includes('--write')) fs.writeFileSync(file, expected);
    else if (expected !== source.replace(/\r\n/g, '\n')) throw new Error('CFLOW cache is stale; regenerate with --write');
}
