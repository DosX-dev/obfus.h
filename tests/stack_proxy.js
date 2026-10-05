'use strict';
const count = 256;
const variantCount = 128;
function fixture() {
    const site = (name, action) => `__declspec(dllexport) unsigned ${name}(unsigned x) {
    ULONG_PTR before, after;
    unsigned long long wide = ((unsigned long long)x << 32) | (x ^ 0xa5a5a5a5u);
    double real = (double)x + 0.5;
    unsigned memory[2] = {x, ~x};
    void *pointer = memory;
    SP(before); ${action}; SP(after);
    if (before != after || memory[0] != x || memory[1] != ~x || pointer != memory || real != (double)x + 0.5 || wide != (((unsigned long long)x << 32) | (x ^ 0xa5a5a5a5u))) return x;
    return x + 1u;
}`;
    return `#include <stdio.h>
#include <windows.h>
static volatile unsigned guard_input;
static unsigned ids[${count}], variants[${count}], lines[${count}], current;
static volatile LONG threaded;
void obfh_test_sf_visit(unsigned id, unsigned variant, unsigned line) {
    if (!threaded) { ids[current] = id; variants[current] = variant; lines[current] = line; }
}
#include "../include/obfus.h"
#if defined(__x86_64__)
#define SP(out) __asm__ __volatile__("movq %%rsp, %0" : "=r"(out) : : "memory")
#else
#define SP(out) __asm__ __volatile__("movl %%esp, %0" : "=r"(out) : : "memory")
#endif
${Array.from({ length: variantCount }, (_, i) => site('forced_' + i, 'OBFH_SF_VARIANT_' + i)).join('\n')}
${Array.from({ length: count }, (_, i) => site('junk_site_' + i, 'STACK_PROXY_FUNCTIONS')).join('\n')}
__declspec(dllexport) unsigned junk_anchor(unsigned x) { return x; }
static unsigned (*forced[])(unsigned) = {${Array.from({ length: variantCount }, (_, i) => 'forced_' + i).join(',')}};
static unsigned (*sites[])(unsigned) = {${Array.from({ length: count }, (_, i) => 'junk_site_' + i).join(',')}};
static DWORD WINAPI worker(void *parameter) {
    unsigned state = (unsigned)(ULONG_PTR)parameter;
    for (unsigned i = 0; i < 1000; ++i) {
        state = state * 1664525u + 1013904223u;
        if (sites[i % ${count}](state) != state + 1u) return 1;
    }
    return 0;
}
int main(void) {
    unsigned state = 0x12345678u;
    static const unsigned edges[] = {0,1,2,3,255,256,0x7fffffffu,0x80000000u,0xffffffffu};
    for (unsigned i = 0; i < 500; ++i) {
        state = state * 1664525u + 1013904223u;
        guard_input = i < sizeof(edges)/sizeof(edges[0]) ? edges[i] : state;
        for (unsigned j = 0; j < ${variantCount}; ++j) if (forced[j](state) != state + 1u) return 1;
        for (current = 0; current < ${count}; ++current) if (sites[current](state) != state + 1u) return 2;
    }
    for (current = 0; current < ${count}; ++current)
        printf("PROXY %u %u %u %u\\n", current, ids[current], variants[current], lines[current]);
    InterlockedExchange(&threaded, 1);
    HANDLE threads[4];
    for (unsigned i = 0; i < 4; ++i) {
        threads[i] = CreateThread(NULL, 0, worker, (void *)(ULONG_PTR)(i + 1), 0, NULL);
        if (!threads[i]) return 3;
    }
    if (WaitForMultipleObjects(4, threads, TRUE, 10000) != WAIT_OBJECT_0) return 4;
    for (unsigned i = 0; i < 4; ++i) {
        DWORD result;
        if (!GetExitCodeThread(threads[i], &result) || result) return 5;
        CloseHandle(threads[i]);
    }
    puts("STACK_PROXY_PASS");
    return 0;
}
`;
}
function measure(stdout, seed, assert) {
    const rows = [...stdout.matchAll(/^PROXY (\d+) (\d+) (\d+) (\d+)\r?$/gm)].map(m => m.slice(1).map(Number));
    assert(rows.length === count, 'proxy trace incomplete');
    const histogram = Array(variantCount).fill(0);
    rows.forEach(([site, id, variant, line], index) => {
        let value = (id ^ seed ^ line ^ 0x53504631) >>> 0;
        value = Math.imul(value ^ (value >>> 16), 2246822507) >>> 0;
        value = Math.imul(value ^ (value >>> 13), 3266489909) >>> 0;
        assert(site === index && variant === (value % variantCount), 'proxy selector disagrees with uint32 reference');
        histogram[variant]++;
    });
    return { histogram, rows };
}
module.exports = { fixture, measure, count, variantCount };
