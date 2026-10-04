#include <float.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <windows.h>
static uint64_t bits(double value) {
    union {
        double f;
        uint64_t u;
    } x = {value};
    return x.u;
}
static double number(uint64_t value) {
    union {
        uint64_t u;
        double f;
    } x = {value};
    return x.f;
}
static double native_sin(double x) { return sin(x); }
static double native_copysign(double x, double y) { return copysign(x, y); }
static double native_add(double a, double b) { return a + b; }
static double native_sub(double a, double b) { return a - b; }
static double native_mul(double a, double b) { return a * b; }
static double native_div(double a, double b) { return a / b; }
#include "../include/obfus.h"
#define CHECK(x)                                                            \
    do {                                                                    \
        if (!(x)) {                                                         \
            fprintf(stderr, "numeric failure line %d: %s\n", __LINE__, #x); \
            return 1;                                                       \
        }                                                                   \
    } while (0)
static int same(double a, double b) {
    uint64_t ua = bits(a), ub = bits(b);
    if ((ua & 0x7ff0000000000000ull) == 0x7ff0000000000000ull && (ua & 0xfffffffffffffull))
        return (ub & 0x7ff0000000000000ull) == 0x7ff0000000000000ull && (ub & 0xfffffffffffffull);
    return ua == ub;
}
int main(void) {
    double values[] = {0.0, -0.0, 1e-20, -1e-20, DBL_MIN, DBL_MAX, 1.0, -1.0,
                       number(1), number(0x8000000000000001ull), HUGE_VAL, -HUGE_VAL, number(0x7ff8000012345678ull)};
    for (int i = 0; i < sizeof(values) / sizeof(values[0]); ++i) {
        double x = values[i];
#if !NO_OBF
        CHECK(bits(_MUTATE_MATH(x)) == bits(x));
        int calls = 0;
        CHECK(bits(_MUTATE_MATH((++calls, x))) == bits(x) && calls == 1);
#endif
        CHECK(same(sin(x), native_sin(x)));
        CHECK(bits(copysign(1.0, x)) == bits(native_copysign(1.0, x)));
        CHECK(bits((double)VM_OBF_DBL(x)) == bits(x));
        for (int j = 0; j < sizeof(values) / sizeof(values[0]); ++j) {
            double y = values[j];
            CHECK(same(VM_ADD_DBL(x, y), native_add(x, y)));
            CHECK(same(VM_SUB_DBL(x, y), native_sub(x, y)));
            CHECK(same(VM_MUL_DBL(x, y), native_mul(x, y)));
            CHECK(same(VM_DIV_DBL(x, y), native_div(x, y)));
            // Ordered comparisons with a quiet NaN must always be false.
            if (i == 12 || j == 12) {
                CHECK(!VM_LSS_DBL(x, y));
                CHECK(!VM_GTR_DBL(x, y));
                CHECK(!VM_LEQ(x, y));
                CHECK(!VM_GEQ(x, y));
            }
        }
    }
#if !NO_OBF
    uint64_t wide = 0xfffffffffffffff1ull;
    CHECK(_MUTATE_MATH(wide) == wide);
    int n = 0;
    CHECK(_MUTATE_MATH(++n) == 1 && n == 1);
#endif
    // Deterministic random normal/subnormal payloads exercise transport exactly.
    uint64_t seed = 0x0123456789abcdefull;
    for (int i = 0; i < 1500; ++i) {
        seed = seed * 6364136223846793005ull + 1;
        double x = number(seed & 0xffefffffffffffffull);
        CHECK(bits((double)VM_OBF_DBL(x)) == bits(x));
#if !NO_OBF
        CHECK(bits(_MUTATE_MATH(x)) == bits(x));
#endif
    }
    puts("NUMERIC_PASS");
    return 0;
}
