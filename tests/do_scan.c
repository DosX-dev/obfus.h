#include <stdio.h>

#include "../include/obfus.h"
#undef if
#undef else
#undef case
#undef for
#undef while
#undef do
#undef return
#undef switch
#undef break
#undef continue
#undef default

static unsigned forms, positions, rotations;
static int failed;
#define PROBE(site) \
    static void probe_##site(unsigned value) { \
        OBFH_FLOW_DO_FIELDS(site); \
        unsigned actual, expected = (__obfh_do_aux & 2u) ? value ^ __obfh_do_key : value + __obfh_do_key; \
        unsigned rotation = 1u + ((__obfh_do_aux >> 16) % 31u); \
        if ((__obfh_do_aux >> 2) & 1u) expected = (expected << rotation) | (expected >> (32u - rotation)); \
        expected = (expected & __obfh_do_mask) | __obfh_do_force; \
        if (!expected || !(__obfh_do_mask & ~__obfh_do_force)) failed = 1; \
        unsigned scanned = 0, copy = expected; \
        if (__obfh_do_reverse) { \
            while (copy >>= 1) ++scanned; \
        } else { \
            while (!(copy & 1u)) { \
                copy >>= 1; \
                ++scanned; \
            } \
        } \
        OBFH_FLOW_DO_SCAN(actual, "movl %8, %%eax;", "m"(value)); \
        if (actual != scanned || actual != __obfh_do_index) failed = 1; \
        forms |= 1u << (__obfh_do_aux & 7u); \
        positions |= 1u << __obfh_do_index; \
        rotations |= 1u << rotation; \
    }
#define ROW(n) PROBE(n##0) PROBE(n##1) PROBE(n##2) PROBE(n##3) PROBE(n##4) PROBE(n##5) PROBE(n##6) PROBE(n##7) PROBE(n##8) PROBE(n##9)
ROW(1)
ROW(2)
ROW(3) ROW(4) ROW(5) ROW(6) ROW(7) ROW(8) ROW(9) ROW(10) ROW(11) ROW(12) ROW(13) ROW(14) ROW(15) ROW(16)
#define CALL(n) probe_##n(value);
#define CALL_ROW(n) CALL(n##0) CALL(n##1) CALL(n##2) CALL(n##3) CALL(n##4) CALL(n##5) CALL(n##6) CALL(n##7) CALL(n##8) CALL(n##9)
    int main(void) {
    unsigned state = 0x6129ac31u;
    for (unsigned i = 0; i < 1024; ++i) {
        state ^= state << 13;
        state ^= state >> 17;
        state ^= state << 5;
        unsigned value = i < 32 ? 1u << i : i < 64 ? ~(1u << (i - 32))
                                        : i == 64  ? 0
                                        : i == 65  ? ~0u
                                                   : state;
        CALL_ROW(1)
        CALL_ROW(2)
        CALL_ROW(3) CALL_ROW(4) CALL_ROW(5) CALL_ROW(6) CALL_ROW(7) CALL_ROW(8)
            CALL_ROW(9) CALL_ROW(10) CALL_ROW(11) CALL_ROW(12) CALL_ROW(13) CALL_ROW(14) CALL_ROW(15) CALL_ROW(16) if (failed) return 1;
    }
    unsigned position_count = 0, rotation_count = 0;
    for (unsigned i = 1; i < 32; ++i) {
        position_count += (positions >> i) & 1u;
        rotation_count += (rotations >> i) & 1u;
    }
    if (forms != 255u || position_count < 24 || rotation_count < 24) return 2;
    puts("DO_SCAN_PASS");
    return 0;
}
