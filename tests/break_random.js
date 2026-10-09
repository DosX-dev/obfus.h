'use strict';

const siteCount = 512;
const heavy = new Set([86, 89, 90, 91, 92, 93, 94]);

// Independent uint32 reference for the header's compile-time selector.
function expectedIndex(site, seed) {
    let value = (site ^ seed ^ 2654435769) >>> 0;
    value = Math.imul(value ^ (value >>> 16), 2246822507) >>> 0;
    value = Math.imul(value ^ (value >>> 13), 3266489909) >>> 0;
    const slot = (value ^ (value >>> 16)) & 255;
    if (slot < 7) return slot === 0 ? 86 : slot + 88;
    const light = (slot - 7) % 121;
    return light < 86 ? light : light < 88 ? light + 1 : light + 7;
}

function fixture() {
    return `#include <stdio.h>
#include <windows.h>
static unsigned int ids[${siteCount}], indices[${siteCount}], current_site;
void obfh_test_break_visit(unsigned int id, unsigned int index) {
    ids[current_site] = id;
    indices[current_site] = index;
}
#include "../include/obfus.h"
#define SITE(n) __declspec(dllexport) int junk_site_##n(int x) { BREAK_STACK_CFLOW; return x + 1; }
${Array.from({ length: siteCount }, (_, i) => `SITE(${i})`).join('\n')}
__declspec(dllexport) int junk_anchor(int x) { return x; }
static int (*sites[])(int) = {${Array.from({ length: siteCount }, (_, i) => 'junk_site_' + i).join(', ')}};
#undef if
#undef for
#undef while
#undef printf
#undef puts
#undef fflush
int main(void) {
    for (unsigned int n = 0; n < ${siteCount}; ++n) {
        current_site = n;
        if (sites[n](123) != 124 || sites[n](-457) != -456) return 1;
        if (printf("SITE %u %u %u\\n", n, ids[n], indices[n]) < 0) return 2;
    }
    return fflush(stdout) == 0 ? 0 : 2;
}
`;
}

function measurements(stdout, seed, assert) {
    const rows = [...stdout.matchAll(/^SITE (\d+) (\d+) (\d+)\r?$/gm)].map((m) => m.slice(1).map(Number));
    assert(rows.length === siteCount, 'public selector output is incomplete');
    const histogram = Array(128).fill(0);
    let repeats = 0,
        heavyCount = 0;
    rows.forEach(([site, counter, index], i) => {
        assert(
            site === i && index === expectedIndex(counter, seed),
            'public selector differs from uint32 reference at site ' + i
        );
        assert(i === 0 || counter > rows[i - 1][1], 'counter was not captured independently for each call');
        histogram[index]++;
        if (heavy.has(index)) heavyCount++;
        if (i && index === rows[i - 1][2]) repeats++;
    });
    return {
        rows,
        histogram,
        distinctTemplates: histogram.filter(Boolean).length,
        adjacentRepeats: repeats,
        heavyCount
    };
}

function extraIndex(index) {
    const ordinal = index < 86 ? index : index < 89 ? index - 1 : index - 7;
    const light = (ordinal + 37) % 121;
    return light < 86 ? light : light < 88 ? light + 1 : light + 7;
}

module.exports = { extraIndex, siteCount, heavy, expectedIndex, fixture, measurements };
