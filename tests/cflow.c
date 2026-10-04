#include <math.h>
#include <stdio.h>
#include <windows.h>
static volatile LONG visits;
void obfh_test_flow_visit(void) { InterlockedIncrement(&visits); }
#define BODY            \
    int result = 0;     \
    if (a)              \
        if (b)          \
            result = 3; \
        else            \
            result = 4; \
    else if (b)         \
        result = 5;     \
    else                \
        result = 6;     \
    return result;
static int native(double a, double b) { BODY }
#include "../include/obfus.h"
static int protected_branch(double a, double b) { BODY }
static int effects(int *count, int first) {
    if (first && ++*count)
        return 1;
    else if (++*count == 1)
        return 2;
    else
        return 3;
}
static int pointer_branch(void *p) {
    if (p)
        return 1;
    else
        return 0;
}
static int wide_branch(unsigned long long n) {
    if (n)
        return 1;
    else
        return 0;
}
static int recursive(int n) {
    if (n <= 0) return 0;
    return 1 + recursive(n - 1);
}
#undef if
#undef else
#define CHECK(x)                                        \
    do {                                                \
        if (!(x)) {                                     \
            fprintf(stderr, "cflow failure: %s\n", #x); \
            return 1;                                   \
        }                                               \
    } while (0)
int main(void) {
    double values[] = {0.0, -0.0, 0.25, -0.25, 1.0, -1.0, HUGE_VAL, -HUGE_VAL, NAN};
    for (int i = 0; i < 9; ++i)
        for (int j = 0; j < 9; ++j)
            CHECK(protected_branch(values[i], values[j]) == native(values[i], values[j]));
    int count = 0;
    CHECK(effects(&count, 0) == 2 && count == 1);
    count = 0;
    CHECK(effects(&count, 1) == 1 && count == 1);
    CHECK(pointer_branch(&count) && !pointer_branch(NULL));
    CHECK(wide_branch(1ull << 63) && !wide_branch(0));
    CHECK(recursive(100) == 100);
    for (unsigned int site = 1; site <= 65535; ++site) {
        unsigned int base = OBFH_FLOW_BASE(site), step = OBFH_FLOW_STEP(site);
        double expected = OBFH_FLOW_FINAL(OBFH_FLOW_FIRST(base + step, site), site);
        CHECK(obfh_flow_token((float)(base + step), site) == expected);
        CHECK(obfh_flow_token((float)base, site) != expected);
    }
#ifdef OBFH_TEST_FLOW_TRACE
    CHECK(visits > 131000);
    LONG before = visits;
    protected_branch(0.25, 1.0);
    CHECK(visits > before);
#endif
    puts("CFLOW_PASS");
    return 0;
}
