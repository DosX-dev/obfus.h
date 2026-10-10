#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "../include/obfus.h"

#undef if
#undef else
#undef for
#undef while
#undef return
#undef printf
#undef fflush
#undef memcpy
#undef memcmp

#if NO_OBF
#define RET_BY_VAR(value) \
    { return (value); }
#endif

struct ReturnPair {
    uint64_t integer;
    double real;
};

static uint64_t wide(uint64_t value) { RET_BY_VAR(value); }
static void *pointer(void *value) { RET_BY_VAR(value); }
static double real(double value) { RET_BY_VAR(value); }
static float small_real(float value) { RET_BY_VAR(value); }
static struct ReturnPair aggregate(struct ReturnPair value) { RET_BY_VAR(value); }
static int qualified(volatile int value) { RET_BY_VAR(value); }

#define RETURN_COPY(n) \
    __declspec(dllexport) int return_copy_##n(int value) { RET_BY_VAR(value); }
RETURN_COPY(0)
RETURN_COPY(1)
RETURN_COPY(2)
RETURN_COPY(3)
RETURN_COPY(4)
RETURN_COPY(5)
RETURN_COPY(6)
RETURN_COPY(7)
RETURN_COPY(8)
RETURN_COPY(9)

int main(void) {
    int (*copies[])(int) = {return_copy_0, return_copy_1, return_copy_2,
                            return_copy_3, return_copy_4, return_copy_5, return_copy_6,
                            return_copy_7, return_copy_8, return_copy_9};
    uint32_t state = 0x12345678u;
    for (unsigned i = 0; i < 10000; ++i) {
        state = state * 1664525u + 1013904223u;
        int input = (int)state;
        for (unsigned n = 0; n < 10; ++n)
            if (copies[n](input) != input) return 1;
        uint64_t bits = ((uint64_t)state << 32) | ~state;
        if (wide(bits) != bits || qualified(input) != input) return 2;
        if (pointer(&state) != &state || pointer(NULL) != NULL) return 3;
    }
    const uint64_t patterns[] = {0, UINT64_C(0x8000000000000000),
                                 UINT64_C(0x7ff0000000000000), UINT64_C(0xfff0000000000000),
                                 UINT64_C(0x7ff8000012345678), UINT64_C(0x0000000000000001)};
    for (unsigned i = 0; i < sizeof(patterns) / sizeof(patterns[0]); ++i) {
        double input, output;
        memcpy(&input, &patterns[i], sizeof(input));
        output = real(input);
        if (memcmp(&input, &output, sizeof(input))) return 4;
        struct ReturnPair pair = {UINT64_C(0xfedcba9876543210), input};
        struct ReturnPair result = aggregate(pair);
        if (result.integer != pair.integer || memcmp(&result.real, &pair.real, sizeof(input))) return 5;
    }
    const uint32_t small_patterns[] = {0, 0x80000000u, 0x7f800000u,
                                       0xff800000u, 0x7fc01234u, 1};
    for (unsigned i = 0; i < sizeof(small_patterns) / sizeof(small_patterns[0]); ++i) {
        float input, output;
        memcpy(&input, &small_patterns[i], sizeof(input));
        output = small_real(input);
        if (memcmp(&input, &output, sizeof(input))) return 6;
    }
    printf("RETURN_TRANSPORT_PASS\n");
    return fflush(stdout) == EOF ? 7 : 0;
}
