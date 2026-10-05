// Old msvcrt does not export nan/remquo; bridge to UCRT for this test only.
#include <math.h>
#include <stdio.h>
#include <windows.h>
static double (*native_nan)(const char *);
static double (*native_remquo)(double, double, int *);
double nan(const char *tag) { return native_nan(tag); }
double remquo(double x, double y, int *q) { return native_remquo(x, y, q); }
#include "../include/obfus.h"
#define CHECK(x)                                               \
    do {                                                       \
        if (!(x)) {                                            \
            fprintf(stderr, "math contract failed: %s\n", #x); \
            return 1;                                          \
        }                                                      \
    } while (0)
int main(void) {
    HMODULE crt = LoadLibraryA("ucrtbase.dll");
    CHECK(crt);
    native_nan = (double (*)(const char *))GetProcAddress(crt, "nan");
    CHECK(native_nan);
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
    CHECK(FreeLibrary(crt));
    puts("Math string/pointer arguments passed");
    return 0;
}
