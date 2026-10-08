#include <math.h>
#include <stdio.h>
#include <windows.h>
static volatile LONG visits;
static volatile LONG proxy_visits;
void obfh_test_proxy_if_visit(void) { InterlockedIncrement(&proxy_visits); }
static volatile LONG routes[4];
static volatile LONG if_junk_routes[128];
static volatile LONG transport_layouts[8], transport_tests[8], transport_flips[2];
void obfh_test_transport_visit(unsigned int layout, unsigned int test, unsigned int flip) {
    InterlockedIncrement(&transport_layouts[layout]);
    InterlockedIncrement(&transport_tests[test]);
    InterlockedIncrement(&transport_flips[flip]);
}
void obfh_test_if_junk_visit(unsigned int route) { InterlockedIncrement(&if_junk_routes[route]); }
void obfh_test_flow_visit(void) { InterlockedIncrement(&visits); }
void obfh_test_flow_route(unsigned int route) { InterlockedIncrement(&routes[route]); }
static volatile LONG stage_routes[8][4][2], stage_errors, exit_routes[8][2];
static volatile LONG linked_visits;
void obfh_test_flow_proxy(unsigned int expected, unsigned int actual) {
    if (expected != actual) InterlockedIncrement(&stage_errors);
    InterlockedIncrement(&linked_visits);
}
void obfh_test_flow_exit(unsigned int style, unsigned int before_state, unsigned int before_tag, unsigned int after_state) {
    if (style >= 8 || after_state > 1 || after_state != (unsigned int)(before_state == before_tag)) {
        InterlockedIncrement(&stage_errors);
        return;
    }
    InterlockedIncrement(&exit_routes[style][after_state]);
}
static unsigned int stage_reference(unsigned int v, unsigned int k, unsigned int m, unsigned int a, unsigned int r, unsigned int style) {
    if (style == 0) {
        v = (v ^ k) * m + a;
        return (v << r) | (v >> (32u - r));
    }
    if (style == 1) {
        v += a;
        v = (v << r) | (v >> (32u - r));
        return (v ^ k) * m;
    }
    if (style == 2) {
        v = v * m ^ k;
        return ((v >> r) | (v << (32u - r))) - a;
    }
    v = (v << r) | (v >> (32u - r));
    return (v ^ k) * m - a;
}
void obfh_test_flow_stage(unsigned int layout, unsigned int stage, unsigned int branch,
                          unsigned int before_state, unsigned int before_tag,
                          unsigned int after_state, unsigned int after_tag,
                          unsigned int key, unsigned int mul, unsigned int add,
                          unsigned int rotate, unsigned int style) {
    if (layout >= 8 || stage >= 4 || branch >= 2 || rotate == 0 || rotate >= 32 || !(mul & 1u) || style >= 4) {
        InterlockedIncrement(&stage_errors);
        return;
    }
    if (after_state != stage_reference(before_state, key, mul, add, rotate, style) ||
        after_tag != stage_reference(before_tag, key, mul, add, rotate, style) ||
        (before_state == before_tag) != (after_state == after_tag)) InterlockedIncrement(&stage_errors);
    InterlockedIncrement(&stage_routes[layout][stage][branch]);
}

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
#define ELSE_CONTROL_BODY                          \
    int result = 0, evaluations = 0;               \
    for (int i = 0; i < limit; ++i) {              \
        if (++evaluations && i == 0)               \
            result += 10;                          \
        else if (i == 2)                           \
            continue;                              \
        else if (i == 6)                           \
            break;                                 \
        else {                                     \
            if (i & 1)                             \
                result += i;                       \
            else                                   \
                result += 2 * i;                   \
        }                                          \
    }                                              \
    int j = 0;                                     \
    do {                                           \
        if (++j < 2)                               \
            result += 1;                           \
        else                                       \
            break;                                 \
        result += 3;                               \
    } while (j < 4);                               \
    switch (mode) {                                \
        case 0:                                    \
            if (result < 0)                        \
                return -1;                         \
            else                                   \
                break;                             \
        default:                                   \
            if (result < 0)                        \
                return -2;                         \
            else                                   \
                return result + 100 * evaluations; \
    }                                              \
    return result + 100 * evaluations;
static int native_else_control(int limit, int mode) {
    ELSE_CONTROL_BODY
}
#include "../include/obfus.h"
static int protected_branch(double a, double b) { BODY }
static int protected_loops(int outer) { LOOP_BODY }
static int protected_else_control(int limit, int mode) { ELSE_CONTROL_BODY }
static int body_gate_only(void) {
    int total = 0;
    for (int i = 0; i < 5; ++i) total += i;
    return total;
}
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
static int comma_condition(int *count, int truth) {
    if (++*count, truth)
        return 1;
    else
        return 2;
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
#define TRANSPORT_CHECK(site)                                                           \
    do {                                                                                \
        int side_effect = 0;                                                            \
        CHECK(OBFH_FLOW_CONDITION((++side_effect, 0), site) == 0 && side_effect == 1);  \
        side_effect = 0;                                                                \
        CHECK(OBFH_FLOW_CONDITION((++side_effect, -7), site) == 1 && side_effect == 1); \
    } while (0)
/* Ordered NaN conditions must not acquire a compiler-sensitive logical NOT.
   The comma operator also verifies that the user expression runs once. */
static int ordered_nan(double n, double y, int *calls) {
    int mask = 0;
    if ((++*calls, n < y)) mask |= 1;
    if ((++*calls, n <= y)) mask |= 2;
    if ((++*calls, n > y)) mask |= 4;
    if ((++*calls, n >= y)) mask |= 8;
    if ((++*calls, y < n)) mask |= 16;
    if ((++*calls, y <= n)) mask |= 32;
    if ((++*calls, y > n)) mask |= 64;
    if ((++*calls, y >= n)) mask |= 128;
    return mask;
}

int main(void) {
    TRANSPORT_CHECK(1u);
    TRANSPORT_CHECK(978u);
    TRANSPORT_CHECK(1955u);
    TRANSPORT_CHECK(2932u);
    TRANSPORT_CHECK(3909u);
    TRANSPORT_CHECK(4886u);
    TRANSPORT_CHECK(5863u);
    TRANSPORT_CHECK(6840u);
    TRANSPORT_CHECK(7817u);
    TRANSPORT_CHECK(8794u);
    TRANSPORT_CHECK(9771u);
    TRANSPORT_CHECK(10748u);
    TRANSPORT_CHECK(11725u);
    TRANSPORT_CHECK(12702u);
    TRANSPORT_CHECK(13679u);
    TRANSPORT_CHECK(14656u);
    TRANSPORT_CHECK(15633u);
    TRANSPORT_CHECK(16610u);
    TRANSPORT_CHECK(17587u);
    TRANSPORT_CHECK(18564u);
    TRANSPORT_CHECK(19541u);
    TRANSPORT_CHECK(20518u);
    TRANSPORT_CHECK(21495u);
    TRANSPORT_CHECK(22472u);
    TRANSPORT_CHECK(23449u);
    TRANSPORT_CHECK(24426u);
    TRANSPORT_CHECK(25403u);
    TRANSPORT_CHECK(26380u);
    TRANSPORT_CHECK(27357u);
    TRANSPORT_CHECK(28334u);
    TRANSPORT_CHECK(29311u);
    TRANSPORT_CHECK(30288u);
    TRANSPORT_CHECK(31265u);
    TRANSPORT_CHECK(32242u);
    TRANSPORT_CHECK(33219u);
    TRANSPORT_CHECK(34196u);
    TRANSPORT_CHECK(35173u);
    TRANSPORT_CHECK(36150u);
    TRANSPORT_CHECK(37127u);
    TRANSPORT_CHECK(38104u);
    TRANSPORT_CHECK(39081u);
    TRANSPORT_CHECK(40058u);
    TRANSPORT_CHECK(41035u);
    TRANSPORT_CHECK(42012u);
    TRANSPORT_CHECK(42989u);
    TRANSPORT_CHECK(43966u);
    TRANSPORT_CHECK(44943u);
    TRANSPORT_CHECK(45920u);
    TRANSPORT_CHECK(46897u);
    TRANSPORT_CHECK(47874u);
    TRANSPORT_CHECK(48851u);
    TRANSPORT_CHECK(49828u);
    TRANSPORT_CHECK(50805u);
    TRANSPORT_CHECK(51782u);
    TRANSPORT_CHECK(52759u);
    TRANSPORT_CHECK(53736u);
    TRANSPORT_CHECK(54713u);
    TRANSPORT_CHECK(55690u);
    TRANSPORT_CHECK(56667u);
    TRANSPORT_CHECK(57644u);
    TRANSPORT_CHECK(58621u);
    TRANSPORT_CHECK(59598u);
    TRANSPORT_CHECK(60575u);
    TRANSPORT_CHECK(61552u);
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
    int nan_calls = 0;
    CHECK(ordered_nan(NAN, 1.0, &nan_calls) == 0 && nan_calls == 8);
    nan_calls = 0;
    CHECK(ordered_nan(1.0, 2.0, &nan_calls) == 195 && nan_calls == 8);
    int count = 0;
    CHECK(effects(&count, 0) == 2 && count == 1);
    count = 0;
    CHECK(effects(&count, 1) == 1 && count == 1);
    count = 0;
    CHECK(comma_condition(&count, 0) == 2 && count == 1);
    count = 0;
    CHECK(comma_condition(&count, 1) == 1 && count == 1);
    CHECK(pointer_branch(&count) && !pointer_branch(NULL));
    CHECK(wide_branch(1ull << 63) && !wide_branch(0));
    CHECK(recursive(100) == 100);
    CHECK(protected_loops(0) == native_loops(0));
    CHECK(protected_loops(1) == native_loops(1));
    for (int limit = 0; limit <= 12; ++limit) {
        CHECK(protected_else_control(limit, 0) == native_else_control(limit, 0));
        CHECK(protected_else_control(limit, 1) == native_else_control(limit, 1));
    }
    count = 0;
    CHECK(while_effect(&count) == 3 && count == 4);

#ifdef OBFH_TEST_FLOW_TRACE
    LONG before_gate = visits;
    int gate_result = body_gate_only();
    LONG after_gate = visits;
    CHECK(gate_result == 10 && after_gate - before_gate == 5);
    CHECK(proxy_visits > 0);
    CHECK(linked_visits > 0);
    for (int variant = 0; variant < (CFLOW_V2 ? 8 : 4); ++variant) {
        CHECK(transport_layouts[variant] > 0);
        CHECK(transport_tests[variant] > 0);
    }
    CHECK(transport_flips[0] > 0 && transport_flips[1] > 0);
    CHECK(visits > 0);
    CHECK(stage_errors == 0);
    for (unsigned int kind = 0; kind < 8; ++kind) {
        CHECK(exit_routes[kind][0] > 0);
        CHECK(exit_routes[kind][1] > 0);
    }
    for (unsigned int layout = 0; layout < (CFLOW_V2 ? 8u : 4u); ++layout)
        for (unsigned int stage = 0; stage < (CFLOW_V2 ? 4u : 2u); ++stage) {
            CHECK(stage_routes[layout][stage][0] > 0);
            CHECK(stage_routes[layout][stage][1] > 0);
        }
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
