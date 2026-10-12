#include <stdio.h>
#include <windows.h>
unsigned int obfh_test_chain_entries[16], obfh_test_chain_routes[32];
#include "../include/obfus.h"
#undef if
#undef else
#undef for
#undef while
#undef switch
#undef break
#undef puts
#undef printf
#if defined(OBFH_PD_LIVE_ADDRESS)
static unsigned int pre(unsigned int site, unsigned int x) {
    site = OBFH_PD_LIVE_SITE(site);
    unsigned int k = OBFH_PD_DRAW(site, 4u), a = OBFH_PD_DRAW(site, 5u), m = OBFH_PD_DRAW(site, 6u) | 1u;
    unsigned int r = 1u + (OBFH_PD_DRAW(site, 7u) % 31u), kind = OBFH_PD_DRAW(site, 1u) & 7u;
    switch (kind) {
        case 0:
            x = (x ^ k) * m + a;
            break;
        case 1:
            x = ((x << r) | (x >> (32 - r))) ^ k;
            x += a;
            break;
        case 2:
            x += k;
            x ^= x >> r;
            break;
        case 3:
            x = (((x & 255u) << 24) | ((x & 65280u) << 8) | ((x >> 8) & 65280u) | (x >> 24));
            x ^= k;
            x += a;
            break;
        case 4:
            x = (~x + k) ^ a;
            break;
        case 5:
            x = (x ^ (x >> 13)) * m;
            break;
        case 6:
            x = ((x >> r) | (x << (32 - r))) + k;
            x = (((x & 255u) << 24) | ((x & 65280u) << 8) | ((x >> 8) & 65280u) | (x >> 24));
            break;
        case 7:
            x -= k;
            x = ((x << r) | (x >> (32 - r))) ^ a;
            break;
    }
    return x;
}
static unsigned int post(unsigned int site, unsigned int x) {
    site = OBFH_PD_LIVE_SITE(site);
    unsigned int k = OBFH_PD_DRAW(site, 4u) & ~1u, a = OBFH_PD_DRAW(site, 5u) & ~1u, m = OBFH_PD_DRAW(site, 6u) | 1u;
    return (x & 1u) ? ((x ^ a) * m + k) : ((x * m + k) ^ a);
}
static unsigned int ref(unsigned int a, unsigned int b, unsigned int c, unsigned int x) {
    x = pre(a, x);
    x = pre(b, x);
    x = pre(c, x);
    x = post(c, x);
    x = post(b, x);
    return post(a, x);
}

#if defined(__x86_64__)
#define SP(out) __asm__ __volatile__("movq %%rsp, %0" \
                                     : "=r"(out) \
                                     : \
                                     : "memory")
#endif
static DWORD WINAPI worker(void *argument) {
    unsigned int x = (unsigned int)(ULONG_PTR)argument;
    for (unsigned int n = 0; n < 4096; n++) {
        x = x * 1664525u + 1013904223u;
        unsigned int a = (x >> 5) & 15u, b = (a + ((x >> 12) & 7u) * 2u + 1u) & 15u, c = (a + 8u) & 15u;
        ULONG_PTR before, after;
        volatile unsigned long long live = ((unsigned long long)x << 32) | ~x;
        SP(before);
        unsigned int result = OBFH_PD_LIVE_NODE(a)(x, (void *)OBFH_PD_LIVE_NODE(b), (void *)OBFH_PD_LIVE_NODE(c));
        SP(after);
        if (result != ref(a, b, c, x) || before != after || live != (((unsigned long long)x << 32) | ~x)) return 1;
    }
    return 0;
}
static int coverage(void) {
    unsigned int x = 0;
    for (unsigned int n = 0; n < 128; n++) {
        x = x * 22695477u + 1u;
        for (unsigned int a = 0; a < 16; a++) {
            unsigned int y = OBFH_PD_LIVE_NODE(a)(x, NULL, NULL);
            if (y != post(a, pre(a, x))) return 1;
            for (unsigned int b = 0; b < 16; b++) {
                unsigned int c = (a + 8u) & 15u;
                y = OBFH_PD_LIVE_NODE(a)(x, (void *)OBFH_PD_LIVE_NODE(b), (void *)OBFH_PD_LIVE_NODE(c));
                if (y != ref(a, b, c, x)) return 2;
            }
        }
    }
#ifdef OBFH_TEST_CHAIN_TRACE
    for (unsigned int i = 0; i < 16; i++)
        if (!obfh_test_chain_entries[i] || !obfh_test_chain_routes[2 * i] || !obfh_test_chain_routes[2 * i + 1]) return 3;
#endif
    return 0;
}
#define TRANSFER(hash) \
    static int transfer_##hash(unsigned int value) { \
        enum { __obfh_flow_hash = hash }; \
        unsigned int __obfh_flow_state = value, __obfh_flow_tag = 0x88776655u; \
        ULONG_PTR __obfh_cookie = 0; \
        OBFH_P_CHAIN(0x88776655u); \
        return (__obfh_flow_state == __obfh_flow_tag) != (value == 0x88776655u); \
    }
TRANSFER(0)
TRANSFER(1)
TRANSFER(2)
TRANSFER(0xdeadbeef)
TRANSFER(0xffffffff)
static int transport(void) {
    unsigned int x = 0x88776655u;
    for (unsigned int i = 0; i < 1024; i++) {
        if (transfer_0(x) || transfer_1(x) || transfer_2(x) || transfer_0xdeadbeef(x) || transfer_0xffffffff(x)) return 1;
        x = x * 1664525u + 1013904223u;
    }
    return 0;
}
static int recursion(unsigned int n, unsigned int value) {
    unsigned int result = OBFH_PD_LIVE_NODE(n & 15u)(value, (void *)OBFH_PD_LIVE_NODE((n + 1) & 15u), (void *)OBFH_PD_LIVE_NODE((n + 2) & 15u));
    if (n && recursion(n - 1, value + 1)) return 1;
    return result != ref(n & 15u, (n + 1) & 15u, (n + 2) & 15u, value);
}
int main(void) {
    if (coverage() || transport() || worker((void *)17) || recursion(64, 0xffffffffu)) return 1;
    HANDLE threads[4];
    for (unsigned int i = 0; i < 4; i++) {
        threads[i] = CreateThread(NULL, 0, worker, (void *)(ULONG_PTR)(i + 1), 0, NULL);
        if (!threads[i]) return 2;
    }
    if (WaitForMultipleObjects(4, threads, TRUE, 30000) != WAIT_OBJECT_0) return 3;
    for (unsigned int i = 0; i < 4; i++) {
        DWORD code;
        if (!GetExitCodeThread(threads[i], &code) || code) return 4;
        CloseHandle(threads[i]);
    }
    puts("CHAIN_PASS");
    return 0;
}
#else
int main(void) {
    puts("CHAIN_DISABLED");
    return 0;
}
#endif
