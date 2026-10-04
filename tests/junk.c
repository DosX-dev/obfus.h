#include <stdint.h>
#include <stdio.h>
#include <windows.h>

#include "../include/obfus.h"
#define SITE(n, macro)                               \
    __declspec(dllexport) int junk_site_##n(int x) { \
        macro;                                       \
        return x + n + 1;                            \
    }
SITE(0, BREAK_STACK_1)
SITE(1, BREAK_STACK_2)
SITE(2, BREAK_STACK_3)
SITE(3, BREAK_STACK_4)
SITE(4, BREAK_STACK_5)
SITE(5, BREAK_STACK_6)
SITE(6, BREAK_STACK_7)
SITE(7, BREAK_STACK_8)
SITE(8, BREAK_STACK_9)
SITE(9, BREAK_STACK_10)
SITE(10, BREAK_STACK_11)
SITE(11, BREAK_STACK_12)
SITE(12, BREAK_STACK_13)
__declspec(dllexport) int junk_anchor(int x) { return x; }
static int (*sites[])(int) = {junk_site_0, junk_site_1, junk_site_2, junk_site_3, junk_site_4, junk_site_5, junk_site_6, junk_site_7, junk_site_8, junk_site_9, junk_site_10, junk_site_11, junk_site_12};
static DWORD WINAPI stress(void *unused) {
    unsigned int state = 0x12345678u;
    for (int i = 0; i < 1000; ++i) {
        state = state * 1664525u + 1013904223u;
        int x = (int)(state % 1000000u);
        volatile float f = x / 8.0f;
        volatile double d = x / 4.0;
        uint64_t wide = (uint64_t)state << 32 | (state ^ 0xa5a5a5a5u);
        int memory[4] = {x, ~x, i, 17};
        void *pointer = &memory[2];
        for (int j = 0; j < 13; ++j) {
            if (sites[j](x) != x + j + 1 || f != x / 8.0f || d != x / 4.0 ||
                wide != (((uint64_t)state << 32) | (state ^ 0xa5a5a5a5u)) || pointer != &memory[2] || memory[0] != x || memory[1] != ~x || memory[2] != i || memory[3] != 17) {
                fprintf(stderr, "junk failure: site=%d iteration=%d\n", j, i);
                return 1;
            }
        }
        if (VM_ADD(x, 7) != x + 7 || VM_SUB(x, 7) != x - 7 || VM_MUL(x, 3) != x * 3) return 2;
        int calls = 0;
        VM_IF((++calls, x & 1)) {
            if (!(x & 1)) return 3;
        }
        VM_ELSE {
            if (x & 1) return 4;
        }
        if (calls != 1) return 5;
    }
    return 0;
}
int main(void) {
    if (stress(NULL)) return 1;
    HANDLE threads[4];
    for (int i = 0; i < 4; ++i) {
        threads[i] = CreateThread(NULL, 0, stress, NULL, 0, NULL);
        if (!threads[i]) return 6;
    }
    if (WaitForMultipleObjects(4, threads, TRUE, 10000) != WAIT_OBJECT_0) return 7;
    for (int i = 0; i < 4; ++i) {
        DWORD result;
        if (!GetExitCodeThread(threads[i], &result) || result) return 8;
        if (!CloseHandle(threads[i])) return 9;
    }
    puts("JUNK_PASS");
    return 0;
}
