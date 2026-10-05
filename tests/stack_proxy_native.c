#include <stdint.h>
#include <stdio.h>

#include "../include/obfus.h"
#undef if
#undef else
#undef printf
#undef puts

/* Execute each skipped leaf in isolation to validate instruction encodings and
   scratch-stack restoration. Production guards never enter these bodies. */
typedef unsigned (*NativeLeaf)(unsigned, unsigned);
#if defined(__x86_64__)
#define LEAF_ADDRESS "leaq 1f(%%rip), %0; jmp 9f; 1:"
#define SP(value) __asm__ __volatile__("movq %%rsp, %0" \
                                       : "=r"(value)    \
                                       :                \
                                       : "memory")
#else
#define LEAF_ADDRESS "leal 1f, %0; jmp 9f; 1:"
#define SP(value) __asm__ __volatile__("movl %%esp, %0" \
                                       : "=r"(value)    \
                                       :                \
                                       : "memory")
#endif
#define CARRIER(name, body)                                          \
    static NativeLeaf name(void) {                                   \
        enum { __obfh_sf_mask = OBFH_JUNK_WORD,                      \
               __obfh_sf_alu_a = RND(0, 4),                          \
               __obfh_sf_alu_b = RND(0, 6),                          \
               __obfh_sf_shift = RND(0, 4) };                        \
        NativeLeaf entry;                                            \
        __asm__ __volatile__(LEAF_ADDRESS body "9:"                  \
                             : "=r"(entry)                           \
                             : OBFH_SF_INPUTS                        \
                             : "eax", "ecx", "edx", "cc", "memory"); \
        return entry;                                                \
    }
CARRIER(leaf_a, OBFH_SF_LEAF_A)
CARRIER(leaf_b, OBFH_SF_LEAF_B)
CARRIER(leaf_c, OBFH_SF_LEAF_C)
CARRIER(leaf_d, OBFH_SF_LEAF_D)
CARRIER(leaf_e, OBFH_SF_LEAF_E)
CARRIER(leaf_f, OBFH_SF_LEAF_F)
CARRIER(leaf_g, OBFH_SF_LEAF_G)
CARRIER(leaf_h, OBFH_SF_LEAF_H)
CARRIER(leaf_i, OBFH_SF_LEAF_I)
CARRIER(leaf_j, OBFH_SF_LEAF_J)
CARRIER(leaf_k, OBFH_SF_LEAF_K)
CARRIER(leaf_l, OBFH_SF_LEAF_L)

int main(void) {
    NativeLeaf functions[] = {leaf_a(), leaf_b(), leaf_c(), leaf_d(), leaf_e(), leaf_f(),
                              leaf_g(), leaf_h(), leaf_i(), leaf_j(), leaf_k(), leaf_l()};
    volatile unsigned canaries[4] = {0xabcdef01u, 0x12345678u, 0xfedcba98u, 0x98765432u};
    volatile unsigned digest = 0;
    unsigned state = 1;
    for (unsigned i = 0; i < 12; ++i) {
        for (unsigned j = 0; j < 2048; ++j) {
            uintptr_t before, after;
            state = state * 1664525u + 1013904223u;
            unsigned a = j == 0 ? 0 : j == 1 ? ~0u
                                             : state;
            SP(before);
            digest ^= functions[i](a, ~state);
            SP(after);
            if (before != after || canaries[0] != 0xabcdef01u || canaries[1] != 0x12345678u ||
                canaries[2] != 0xfedcba98u || canaries[3] != 0x98765432u) return 1;
        }
    }
    puts("STACK_PROXY_NATIVE_PASS");
    return 0;
}
