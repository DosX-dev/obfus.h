#include <stdint.h>
#include <stdio.h>
#include <windows.h>
static int reference_chain(int a, double b, void *p, double q) {
    if (a)
        return 1;
    else if (b)
        return 2;
    else if (p)
        return 3;
    else if (q)
        return 4;
    else
        return 5;
}
static int reference_recursive(int n) {
    if (n <= 0)
        return 1;
    else if (n & 1)
        return n + reference_recursive(n - 1);
    else
        return reference_recursive(n - 1) - n;
}
#include "../include/obfus.h"
#define CHECK(x)                                                           \
    do {                                                                   \
        if (!(x)) {                                                        \
            fprintf(stderr, "branch failure line %d: %s\n", __LINE__, #x); \
            return 1;                                                      \
        }                                                                  \
    } while (0)
#if OBFH_TEST_BRANCH_TRACE
static volatile LONG branch_calls, steps[OBFH_V_RETURN + 1], invalid_steps;
void obfh_test_vm_enter(void) { InterlockedIncrement(&branch_calls); }
void obfh_test_vm_step(unsigned int op, unsigned int pc) {
    if (op < OBFH_V_LOAD_A || op > OBFH_V_RETURN || pc >= 32)
        InterlockedIncrement(&invalid_steps);
    else
        InterlockedIncrement(&steps[op]);
}
#endif
static int virtual_chain(int a, double b, void *p, double q) {
    VM_IF(a) { return 1; }
    VM_ELSE_IF(b) { return 2; }
    VM_ELSE_IF(p) { return 3; }
    VM_ELSE_IF(q) { return 4; }
    VM_ELSE { return 5; }
    return -1;
}
static int virtual_recursive(int n) {
    VM_IF(n <= 0) { return 1; }
    VM_ELSE_IF(n & 1) { return n + virtual_recursive(n - 1); }
    VM_ELSE { return virtual_recursive(n - 1) - n; }
    return -1000;
}
static int semantics(void) {
#if VIRT && !NO_OBF
    /* Instruction families and fault checks are covered by vm_kernel.c. */
#endif
    int effects = 0, selected = 0;
    VM_IF(++effects == 1) { selected = 1; }
    VM_ELSE_IF(++effects == 2) { selected = 2; }
    VM_ELSE { selected = 3; }
    CHECK(effects == 1 && selected == 1);
    effects = 0;
    VM_IF(++effects < 0) { selected = 1; }
    VM_ELSE_IF(++effects == 2) { selected = 2; }
    VM_ELSE { selected = 3; }
    CHECK(effects == 2 && selected == 2);
    effects = 0;
    VM_IF(0 && ++effects) { selected = 1; }
    VM_ELSE_IF(1 || ++effects) { selected = 2; }
    VM_ELSE { selected = 3; }
    CHECK(effects == 0 && selected == 2);
    VM_IF((++effects, 0.25))
    selected = 7;
    CHECK(effects == 1 && selected == 7);
    uint64_t large = UINT64_C(1) << 48;
    VM_IF(large)
    selected = 8;
    CHECK(selected == 8);
    int sum = 0;
    for (int i = 0; i < 32; ++i) {
        VM_IF(i == 3) { continue; }
        VM_ELSE_IF(i == 9) { break; }
        VM_ELSE { sum += i; }
    }
    CHECK(sum == 33);
    VM_IF(1) { goto reached; }
    CHECK(0);
reached:
    for (int a = 0; a < 2; ++a)
        for (int b = 0; b < 2; ++b) {
            selected = 0;
            VM_IF(a)
            VM_IF(b)
            selected = 11;
            VM_ELSE selected = 12;
            CHECK(selected == (a ? (b ? 11 : 12) : 0));
        }
    union {
        uint64_t bits;
        double value;
    } nan_value = {UINT64_C(0x7ff8000000000001)};
    union {
        uint64_t bits;
        double value;
    } infinity = {UINT64_C(0x7ff0000000000000)};
    unsigned int random = 0x91745123u;
    for (int i = 0; i < 1000; ++i) {
        random = random * 1664525u + 1013904223u;
        int a = (random & 1) ? 0 : -7;
        double b = (random & 2) ? -0.0 : ((random & 32) ? infinity.value : 0.25);
        void *p = (random & 4) ? NULL : &selected;
        double q = (random & 8) ? nan_value.value : 0.0;
        CHECK(virtual_chain(a, b, p, q) == reference_chain(a, b, p, q));
        CHECK(virtual_chain(0, 0, NULL, 0) == 5);
    }
    for (int n = 0; n < 32; ++n) CHECK(virtual_recursive(n) == reference_recursive(n));
    return 0;
}
static DWORD WINAPI worker(void *unused) { return semantics(); }
int main(void) {
#if OBFH_TEST_BRANCH_TRACE
    int selected = 0;
    VM_IF(1)
    selected = 1;
    VM_ELSE_IF(0)
    selected = 2;
    VM_ELSE selected = 3;
    CHECK(selected == 1 && branch_calls == 1);
    LONG before = branch_calls;
    VM_IF(0)
    selected = 1;
    VM_ELSE_IF(0)
    selected = 2;
    VM_ELSE selected = 3;
    CHECK(selected == 3 && branch_calls == before + 3);
#endif
    CHECK(semantics() == 0);
    HANDLE threads[4];
    for (int i = 0; i < 4; ++i) {
        threads[i] = CreateThread(NULL, 0, worker, NULL, 0, NULL);
        CHECK(threads[i]);
    }
    CHECK(WaitForMultipleObjects(4, threads, TRUE, 60000) == WAIT_OBJECT_0);
    for (int i = 0; i < 4; ++i) {
        DWORD result;
        CHECK(GetExitCodeThread(threads[i], &result) && result == 0);
        CHECK(CloseHandle(threads[i]));
    }
#if OBFH_TEST_BRANCH_TRACE
    CHECK(branch_calls > 10000 && invalid_steps == 0);
    CHECK(steps[OBFH_V_TEST] > 0 && steps[OBFH_V_BOOLEAN] > 0 && steps[OBFH_V_RETURN] > 0);
#endif
    puts("BRANCH_PASS");
    return 0;
}
