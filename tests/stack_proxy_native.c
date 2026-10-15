#include <stdint.h>
#include <stdio.h>

#include "../include/obfus.h"
#undef if
#undef else
#undef case
#undef printf
#undef puts

/* Execute skipped leaf and framed bodies in isolation to validate instruction
   encodings and stack restoration. Production guards never enter these bodies. */
typedef unsigned (*NativeLeaf)(unsigned, unsigned);
#if defined(__x86_64__)
#define LEAF_ADDRESS "leaq 1f(%%rip), %0; jmp 9f; 1:"
#define SP(value) __asm__ __volatile__("movq %%rsp, %0" \
                                       : "=r"(value) \
                                       : \
                                       : "memory")
#else
#define LEAF_ADDRESS "leal 1f, %0; jmp 9f; 1:"
#define SP(value) __asm__ __volatile__("movl %%esp, %0" \
                                       : "=r"(value) \
                                       : \
                                       : "memory")
#endif
#define CARRIER(name, body) \
    static NativeLeaf name(void) { \
        OBFH_SF_CAPTURE; \
        NativeLeaf entry; \
        __asm__ __volatile__(OBFH_SF_ASM_CONSTANTS LEAF_ADDRESS body "9:" \
                             : "=r"(entry) \
                             : OBFH_SF_INPUTS \
                             : "eax", "ecx", "edx", "cc", "memory"); \
        return entry; \
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
CARRIER(frame_a, OBFH_SF_FRAME("frame_a") OBFH_SF_BODY_A OBFH_SF_EPILOGUE)
CARRIER(frame_b, OBFH_SF_FRAME("frame_b") OBFH_SF_BODY_B OBFH_SF_EPILOGUE_ALT)
CARRIER(frame_c, OBFH_SF_FRAME_ALT("frame_c") OBFH_SF_BODY_C OBFH_SF_EPILOGUE)
CARRIER(frame_d, OBFH_SF_FRAME_ALT("frame_d") OBFH_SF_BODY_D OBFH_SF_EPILOGUE_ALT)
CARRIER(link_chain,
        OBFH_SF_FRAME("frame_a") OBFH_SF_BODY_A OBFH_SF_CALL_AT("2f", "0") OBFH_SF_EPILOGUE
        "2:" OBFH_SF_FRAME("frame_b") OBFH_SF_BODY_B OBFH_SF_CALL_AT("3f", "1") OBFH_SF_EPILOGUE_ALT
        "3:" OBFH_SF_FRAME_ALT("frame_c") OBFH_SF_BODY_C OBFH_SF_EPILOGUE)
CARRIER(link_shared,
        OBFH_SF_FRAME_ALT("frame_a") OBFH_SF_BODY_A OBFH_SF_CALL_AT("2f", "0") OBFH_SF_CALL_AT("3f", "1") OBFH_SF_EPILOGUE
        "2:" OBFH_SF_FRAME("frame_b") OBFH_SF_BODY_B OBFH_SF_CALL_AT("3f", "2") OBFH_SF_EPILOGUE
        "3:" OBFH_SF_FRAME("frame_c") OBFH_SF_BODY_C OBFH_SF_EPILOGUE_ALT)
CARRIER(link_tail,
        OBFH_SF_FRAME("frame_a") OBFH_SF_BODY_A OBFH_SF_CALL_AT("2f", "3") OBFH_SF_EPILOGUE
        "2:" OBFH_SF_FRAME_ALT("frame_b") OBFH_SF_BODY_B
        "leave; jmp 3f;"
        "3:" OBFH_SF_FRAME("frame_c") OBFH_SF_BODY_C OBFH_SF_EPILOGUE)
CARRIER(link_gate,
        OBFH_SF_FRAME("frame_a") OBFH_SF_ARGS OBFH_SF_CALL_AT("2f", "4") OBFH_SF_EPILOGUE
        "2:" OBFH_SF_FRAME("frame_b") OBFH_SF_BODY_B OBFH_SF_EPILOGUE)

CARRIER(link_eight,
        OBFH_SF_FRAME("frame_c")
            OBFH_SF_LAYOUT_28(OBFH_SF_BODY_A, OBFH_SF_BODY_B, OBFH_SF_LEAF_A, OBFH_SF_LEAF_B, OBFH_SF_BODY_A))

/* Reversed ordered comparisons need a reversed branch too: equality-only
   tests cannot detect signed/unsigned direction or overflow mistakes. */
#define COMPARE_PROBE(name, reg, cc, reverse, bit, flip) \
    static unsigned name(unsigned x, unsigned y) { \
        __asm__ __volatile__(OBFH_SF_ASM_BITS "movl %1, %%edx; movl %%edx, %%ecx;" OBFH_SF_LEAF_COMPARE(#reg, #cc, #reverse, #bit, "10") "movl $0, %%eax; jmp 9f; 6: movl $1, %%eax; 9:" \
                             : "+a"(x) \
                             : "r"(y), [phantom] "i"((flip) << (bit)) \
                             : "ecx", "edx", "cc"); \
        return x; \
    }
COMPARE_PROBE(unsigned_forward, 2, 0x76, 0x73, 10, 0)
COMPARE_PROBE(unsigned_reverse, 2, 0x76, 0x73, 10, 1)
COMPARE_PROBE(signed_forward, 1, 0x7d, 0x7e, 11, 0)
COMPARE_PROBE(signed_reverse, 1, 0x7d, 0x7e, 11, 1)
#define SUB_PROBE(name, flip) \
    static unsigned name(unsigned x, unsigned y) { \
        __asm__ __volatile__(OBFH_SF_ASM_BITS OBFH_SF_LEAF_REVERSE_SUB \
                             : "+a"(x) \
                             : "d"(y), [phantom] "i"((flip) << 13) \
                             : "cc"); \
        return x; \
    }
SUB_PROBE(sub_forward, 0)
SUB_PROBE(sub_reverse, 1)
static int check_leaf_variants(void) {
    const unsigned edges[] = {0, 1, 0x7fffffff, 0x80000000, 0x80000001, 0xfffffffe, 0xffffffff};
    unsigned state = 1;
    for (unsigned i = 0; i < 10000; ++i) {
        state = state * 1664525u + 1013904223u;
        unsigned x = i < 49 ? edges[i / 7] : state;
        state = state * 1664525u + 1013904223u;
        unsigned y = i < 49 ? edges[i % 7] : state;
        if (unsigned_forward(x, y) != (x <= y) || unsigned_reverse(x, y) != (x <= y) ||
            signed_forward(x, y) != ((int32_t)x >= (int32_t)y) || signed_reverse(x, y) != ((int32_t)x >= (int32_t)y) ||
            sub_forward(x, y) != y - x || sub_reverse(x, y) != y - x) return 0;
    }
    return 1;
}

int main(void) {
    if (!check_leaf_variants()) return 2;
    NativeLeaf functions[] = {leaf_a(), leaf_b(), leaf_c(), leaf_d(), leaf_e(), leaf_f(),
                              leaf_g(), leaf_h(), leaf_i(), leaf_j(), leaf_k(), leaf_l(),
                              frame_a(), frame_b(), frame_c(), frame_d(),
                              link_chain(), link_shared(), link_tail(), link_gate(), link_eight()};
    volatile unsigned canaries[4] = {0xabcdef01u, 0x12345678u, 0xfedcba98u, 0x98765432u};
    volatile unsigned digest = 0;
    unsigned state = 1;
    for (unsigned i = 0; i < sizeof(functions) / sizeof(functions[0]); ++i) {
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
