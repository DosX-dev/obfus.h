// TCC supplies inline nan; msvcrt lacks remquo, so bridge only that call to UCRT.
#include <math.h>
#include <stdio.h>
#include <windows.h>
static double (*native_remquo)(double, double, int *);
// Native references are captured before public function interception.
static double (*native_unary[])(double) = {
    acos, asin, atan, ceil, cos, cosh, exp, fabs,
    floor, log, log10, sin, sinh, sqrt, tan, tanh};
double remquo(double x, double y, int *q) { return native_remquo(x, y, q); }
#include "../include/obfus.h"
#define CHECK(x) \
    do { \
        if (!(x)) { \
            fprintf(stderr, "math contract failed: %s\n", #x); \
            return 1; \
        } \
    } while (0)
int main(void) {
    HMODULE crt = LoadLibraryA("ucrtbase.dll");
    CHECK(crt);
    native_remquo = (double (*)(double, double, int *))GetProcAddress(crt, "remquo");
    CHECK(native_remquo);
    const char *tags[] = {"", "123", "0x123"};
    for (int i = 0; i < 3; i++) {
        double value = nan(tags[i]);
        CHECK(value != value);
    }
    int exponent = 0, quotient = 0;
    double whole = 0;
    CHECK(frexp(8.0, &exponent) == 0.5 && exponent == 4);
    CHECK(modf(3.5, &whole) == 0.5 && whole == 3.0);
    CHECK(remquo(7.0, 2.0, &quotient) == -1.0 && quotient != 0);
    CHECK(sin(0.0) == 0.0 && pow(2.0 + 1.0, 2.0) == 9.0);
    CHECK(log10(100.0) == 2.0);
    CHECK(atan2(0.0, 1.0) == 0.0);
    // Every shared math adapter must retain its type and evaluate arguments once.
    int reference = 0, evaluations = 0;
#define CHECK_UNARY(name) \
    do { \
        evaluations = 0; \
        volatile double actual = name((++evaluations, 0.5)); \
        volatile double expected = native_unary[reference++](0.5); \
        CHECK(actual == expected); \
        CHECK(evaluations == 1); \
    } while (0)
    CHECK_UNARY(acos);
    CHECK_UNARY(asin);
    CHECK_UNARY(atan);
    CHECK_UNARY(ceil);
    CHECK_UNARY(cos);
    CHECK_UNARY(cosh);
    CHECK_UNARY(exp);
    CHECK_UNARY(fabs);
    CHECK_UNARY(floor);
    CHECK_UNARY(log);
    CHECK_UNARY(log10);
    CHECK_UNARY(sin);
    CHECK_UNARY(sinh);
    CHECK_UNARY(sqrt);
    CHECK_UNARY(tan);
    CHECK_UNARY(tanh);
#undef CHECK_UNARY
    int left = 0, right = 0;
    CHECK(atan2((++left, 0.0), (++right, 1.0)) == 0.0);
    CHECK(fmod((++left, 7.0), (++right, 2.0)) == 1.0);
    CHECK(frexp((++left, 8.0), (++right, &exponent)) == 0.5 && exponent == 4);
    CHECK(ldexp((++left, 0.5), (++right, 4)) == 8.0);
    CHECK(modf((++left, 3.5), (++right, &whole)) == 0.5 && whole == 3.0);
    CHECK(pow((++left, 3.0), (++right, 2.0)) == 9.0);
    CHECK(left == 6 && right == 6);
    CHECK(FreeLibrary(crt));
    puts("Math string/pointer arguments passed");
    return 0;
}
