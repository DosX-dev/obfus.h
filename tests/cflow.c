#include <math.h>
#include <stdio.h>
#include <windows.h>
static volatile LONG visits;
static volatile LONG routes[4];
static volatile LONG if_junk_routes[128];
void obfh_test_if_junk_visit(unsigned int route) { InterlockedIncrement(&if_junk_routes[route]); }
void obfh_test_flow_visit(void) { InterlockedIncrement(&visits); }
void obfh_test_flow_route(unsigned int route) { InterlockedIncrement(&routes[route]); }
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
#define LOOP_BODY                                \
    int result = 0, i = 0, cases = 0;            \
    if (outer)                                   \
        for (int j = 0; j < 2; ++j) result += 1; \
    else                                         \
        result = 7;                              \
    if (outer) switch (++cases) {                \
            case 1:                              \
                result += 3;                     \
                break;                           \
            default:                             \
                result += 4;                     \
        }                                        \
    else                                         \
        result += 11;                            \
    for (; i < 8; ++i) {                         \
        if (i == 2) continue;                    \
        if (i == 5) break;                       \
        result += i;                             \
    }                                            \
    int count = 3;                               \
    do {                                         \
        result++;                                \
    } while (--count);                           \
    while (i-- > 0) {                            \
        if (i == 2) continue;                    \
        result += i;                             \
    }                                            \
    for (;;) {                                   \
        result++;                                \
        break;                                   \
    }                                            \
    switch ((unsigned long long)0x80000000u) {   \
        case 0x80000000ull:                      \
            result += 13;                        \
            break;                               \
        default:                                 \
            result = -1;                         \
    }                                            \
    switch (-2) {                                \
        case -2:                                 \
            result += 17;                        \
            break;                               \
    }                                            \
    return result + cases;
static int native_loops(int outer) { LOOP_BODY }
#include "../include/obfus.h"
static int protected_branch(double a, double b) { BODY }
static int protected_loops(int outer) { LOOP_BODY }
static int while_effect(int *count) {
    int result = 0;
    while (++*count < 4) result++;
    return result;
}
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
    OBFH_CFLOW_SELECT(0);
    OBFH_CFLOW_SELECT(1);
    OBFH_CFLOW_SELECT(2);
    OBFH_CFLOW_SELECT(3);
    OBFH_CFLOW_SELECT(4);
    OBFH_CFLOW_SELECT(5);
    OBFH_CFLOW_SELECT(6);
    OBFH_CFLOW_SELECT(7);
    OBFH_CFLOW_SELECT(8);
    OBFH_CFLOW_SELECT(9);
    OBFH_CFLOW_SELECT(10);
    OBFH_CFLOW_SELECT(11);
    OBFH_CFLOW_SELECT(12);
    OBFH_CFLOW_SELECT(13);
    OBFH_CFLOW_SELECT(14);
    OBFH_CFLOW_SELECT(15);
    OBFH_CFLOW_SELECT(16);
    OBFH_CFLOW_SELECT(17);
    OBFH_CFLOW_SELECT(18);
    OBFH_CFLOW_SELECT(19);
    OBFH_CFLOW_SELECT(20);
    OBFH_CFLOW_SELECT(21);
    OBFH_CFLOW_SELECT(22);
    OBFH_CFLOW_SELECT(23);
    OBFH_CFLOW_SELECT(24);
    OBFH_CFLOW_SELECT(25);
    OBFH_CFLOW_SELECT(26);
    OBFH_CFLOW_SELECT(27);
    OBFH_CFLOW_SELECT(28);
    OBFH_CFLOW_SELECT(29);
    OBFH_CFLOW_SELECT(30);
    OBFH_CFLOW_SELECT(31);
    OBFH_CFLOW_SELECT(32);
    OBFH_CFLOW_SELECT(33);
    OBFH_CFLOW_SELECT(34);
    OBFH_CFLOW_SELECT(35);
    OBFH_CFLOW_SELECT(36);
    OBFH_CFLOW_SELECT(37);
    OBFH_CFLOW_SELECT(38);
    OBFH_CFLOW_SELECT(39);
    OBFH_CFLOW_SELECT(40);
    OBFH_CFLOW_SELECT(41);
    OBFH_CFLOW_SELECT(42);
    OBFH_CFLOW_SELECT(43);
    OBFH_CFLOW_SELECT(44);
    OBFH_CFLOW_SELECT(45);
    OBFH_CFLOW_SELECT(46);
    OBFH_CFLOW_SELECT(47);
    OBFH_CFLOW_SELECT(48);
    OBFH_CFLOW_SELECT(49);
    OBFH_CFLOW_SELECT(50);
    OBFH_CFLOW_SELECT(51);
    OBFH_CFLOW_SELECT(52);
    OBFH_CFLOW_SELECT(53);
    OBFH_CFLOW_SELECT(54);
    OBFH_CFLOW_SELECT(55);
    OBFH_CFLOW_SELECT(56);
    OBFH_CFLOW_SELECT(57);
    OBFH_CFLOW_SELECT(58);
    OBFH_CFLOW_SELECT(59);
    OBFH_CFLOW_SELECT(60);
    OBFH_CFLOW_SELECT(61);
    OBFH_CFLOW_SELECT(62);
    OBFH_CFLOW_SELECT(63);
    OBFH_CFLOW_SELECT(64);
    OBFH_CFLOW_SELECT(65);
    OBFH_CFLOW_SELECT(66);
    OBFH_CFLOW_SELECT(67);
    OBFH_CFLOW_SELECT(68);
    OBFH_CFLOW_SELECT(69);
    OBFH_CFLOW_SELECT(70);
    OBFH_CFLOW_SELECT(71);
    OBFH_CFLOW_SELECT(72);
    OBFH_CFLOW_SELECT(73);
    OBFH_CFLOW_SELECT(74);
    OBFH_CFLOW_SELECT(75);
    OBFH_CFLOW_SELECT(76);
    OBFH_CFLOW_SELECT(77);
    OBFH_CFLOW_SELECT(78);
    OBFH_CFLOW_SELECT(79);
    OBFH_CFLOW_SELECT(80);
    OBFH_CFLOW_SELECT(81);
    OBFH_CFLOW_SELECT(82);
    OBFH_CFLOW_SELECT(83);
    OBFH_CFLOW_SELECT(84);
    OBFH_CFLOW_SELECT(85);
    OBFH_CFLOW_SELECT(86);
    OBFH_CFLOW_SELECT(87);
    OBFH_CFLOW_SELECT(88);
    OBFH_CFLOW_SELECT(89);
    OBFH_CFLOW_SELECT(90);
    OBFH_CFLOW_SELECT(91);
    OBFH_CFLOW_SELECT(92);
    OBFH_CFLOW_SELECT(93);
    OBFH_CFLOW_SELECT(94);
    OBFH_CFLOW_SELECT(95);
    OBFH_CFLOW_SELECT(96);
    OBFH_CFLOW_SELECT(97);
    OBFH_CFLOW_SELECT(98);
    OBFH_CFLOW_SELECT(99);
    OBFH_CFLOW_SELECT(100);
    OBFH_CFLOW_SELECT(101);
    OBFH_CFLOW_SELECT(102);
    OBFH_CFLOW_SELECT(103);
    OBFH_CFLOW_SELECT(104);
    OBFH_CFLOW_SELECT(105);
    OBFH_CFLOW_SELECT(106);
    OBFH_CFLOW_SELECT(107);
    OBFH_CFLOW_SELECT(108);
    OBFH_CFLOW_SELECT(109);
    OBFH_CFLOW_SELECT(110);
    OBFH_CFLOW_SELECT(111);
    OBFH_CFLOW_SELECT(112);
    OBFH_CFLOW_SELECT(113);
    OBFH_CFLOW_SELECT(114);
    OBFH_CFLOW_SELECT(115);
    OBFH_CFLOW_SELECT(116);
    OBFH_CFLOW_SELECT(117);
    OBFH_CFLOW_SELECT(118);
    OBFH_CFLOW_SELECT(119);
    OBFH_CFLOW_SELECT(120);
    OBFH_CFLOW_SELECT(121);
    OBFH_CFLOW_SELECT(122);
    OBFH_CFLOW_SELECT(123);
    OBFH_CFLOW_SELECT(124);
    OBFH_CFLOW_SELECT(125);
    OBFH_CFLOW_SELECT(126);
    OBFH_CFLOW_SELECT(127);
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
    CHECK(protected_loops(0) == native_loops(0));
    CHECK(protected_loops(1) == native_loops(1));
    count = 0;
    CHECK(while_effect(&count) == 3 && count == 4);
    for (unsigned int site = 1; site <= 65535; ++site) {
        unsigned int base = OBFH_FLOW_BASE(site), step = OBFH_FLOW_STEP(site);
        double expected = OBFH_FLOW_FINAL(OBFH_FLOW_FIRST(base + step, site), site);
        CHECK(obfh_flow_token((float)(base + step), site) == expected);
        CHECK(obfh_flow_token((float)base, site) != expected);
    }
#ifdef OBFH_TEST_FLOW_TRACE
    CHECK(visits > 131000);
    for (int route = 0; route < 4; ++route) CHECK(routes[route] > 0);
    LONG selected_if_sites = 0;
    for (int route = 0; route < 128; ++route) selected_if_sites += if_junk_routes[route];
    CHECK(selected_if_sites > 0);
    LONG before = visits;
    protected_branch(0.25, 1.0);
    CHECK(visits > before);
    before = visits;
    count = 0;
    while_effect(&count);
    CHECK(visits > before);
#endif
    puts("CFLOW_PASS");
    return 0;
}
