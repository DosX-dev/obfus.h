#include <float.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <windows.h>

#include "../include/obfus.h"
#define CHECK(x)                                               \
    do {                                                       \
        if (!(x)) {                                            \
            fprintf(stderr, "VM failure line %d\n", __LINE__); \
            return 1;                                          \
        }                                                      \
    } while (0)
#if VIRT && !NO_OBF
static volatile LONG calls;
static long double counted_vm(long double key, long long cmd, OBFH_VM_VALUE a, long double ja, OBFH_VM_VALUE b, long double jb) {
    InterlockedIncrement(&calls);
    return (Obfh_VirtualMachine)(key, cmd, a, ja, b, jb);
}
#define Obfh_VirtualMachine(...) counted_vm(__VA_ARGS__)
#define ROUTE(expr)            \
    do {                       \
        LONG before = calls;   \
        (void)(expr);          \
        CHECK(calls > before); \
    } while (0)
#else
#define ROUTE(expr) ((void)(expr))
#endif
static int stress(void) {
#if VIRT && !NO_OBF
    OBFH_VM_VALUE previous = obfh_vm_encode(-1337.25L, SALT_NUM1, 1u | (1u << 1));
    CHECK(previous.floating == 1);
    for (unsigned int nonce = 2; nonce < 34; ++nonce) {
        OBFH_VM_VALUE current = obfh_vm_encode(-1337.25L, SALT_NUM1, 1u | (nonce << 1));
        CHECK(current.floating == 1 && current.nonce != previous.nonce);
        CHECK(obfh_vm_decode(current, SALT_NUM1) == -1337.25L);
        CHECK(memcmp(current.bytes, previous.bytes, sizeof(current.bytes)) != 0);
        previous = current;
    }
    OBFH_VM_VALUE integer_packet = obfh_vm_encode(1337.0L, SALT_NUM2, 0u | (17u << 1));
    CHECK(integer_packet.floating == 0 && obfh_vm_decode(integer_packet, SALT_NUM2) == 1337.0L);
    enum { command_site = _VM_DEMUTATOR_KEY };
    long long encoded_command = _ENC_OP__NOP;
    CHECK(encoded_command / ~(int)SALT_CMD + command_site != OP__NOP);
    CHECK((Obfh_VirtualMachine)(command_site, encoded_command, previous, 1,
                                integer_packet, 2) == -1337.25L);
#endif
    volatile long double precise = 1000000000.0L;
    precise += 1.0L / 1073741824.0L;
    if (sizeof(long double) > sizeof(double)) CHECK(precise != (long double)(double)precise);
    CHECK(VM_OBF_DBL(precise) == precise);
    CHECK(VM_OBF_DBL(INT_MIN) == (long double)INT_MIN);
    unsigned int seed = 0x12345678u;
    for (int i = 0; i < 3000; ++i) {
        seed = seed * 1664525u + 1013904223u;
        int a = (int)(seed % 2001) - 1000;
        seed = seed * 1664525u + 1013904223u;
        int b = (int)(seed % 2001) - 1000;
        CHECK(VM_ADD(a, b) == a + b);
        CHECK(VM_SUB(a, b) == a - b);
        CHECK(VM_MUL(a, b) == a * b);
        // Integer division has the same truncation contract as C.
        if (b) {
            CHECK(VM_DIV(a, b) == a / b);
            CHECK(VM_MOD(a, b) == a % b);
        }
        CHECK(VM_EQU(a, b) == (a == b));
        CHECK(VM_NEQ(a, b) == (a != b));
        CHECK(VM_LSS(a, b) == (a < b));
        CHECK(VM_GTR(a, b) == (a > b));
        CHECK(VM_LEQ(a, b) == (a <= b));
        CHECK(VM_GEQ(a, b) == (a >= b));
        double x = a / 4.0, y = b / 4.0;
        CHECK(VM_ADD_DBL(x, y) == x + y);
        CHECK(VM_SUB_DBL(x, y) == x - y);
        CHECK(VM_MUL_DBL(x, y) == x * y);
        if (y) {
            long double difference = VM_DIV_DBL(x, y) - x / y;
            CHECK(difference > -1e-10 && difference < 1e-10);
        }
        CHECK(VM_LSS_DBL(x, y) == (x < y));
        CHECK(VM_GTR_DBL(x, y) == (x > y));
        CHECK(VM_OBF_INT(a) == a);
        CHECK(VM_OBF_DBL(x) == x);
    }
    ROUTE(VM_ADD(3, 4));
    ROUTE(VM_SUB(3, 4));
    ROUTE(VM_MUL(3, 4));
    ROUTE(VM_DIV(12, 4));
    ROUTE(VM_MOD(13, 4));
    ROUTE(VM_EQU(3, 4));
    ROUTE(VM_NEQ(3, 4));
    ROUTE(VM_LSS(3, 4));
    ROUTE(VM_GTR(3, 4));
    ROUTE(VM_LEQ(3, 4));
    ROUTE(VM_GEQ(3, 4));
    ROUTE(VM_OBF_INT(3));
    ROUTE(VM_ADD_DBL(3.5, 4));
    ROUTE(VM_SUB_DBL(3.5, 4));
    ROUTE(VM_MUL_DBL(3.5, 4));
    ROUTE(VM_DIV_DBL(3.5, 4));
    ROUTE(VM_LSS_DBL(3.5, 4));
    ROUTE(VM_GTR_DBL(3.5, 4));
    ROUTE(VM_OBF_DBL(3.5));
    CHECK(2 * VM_ADD(3, 4) == 14);
    CHECK(VM_ADD(INT_MIN, 0) == INT_MIN && VM_SUB(INT_MAX, 0) == INT_MAX);
    CHECK(VM_MOD(INT_MIN, 3) == INT_MIN % 3);
    int visits = 0;
    VM_IF(VM_EQU(3, 3)) { visits++; }
    VM_ELSE { visits += 10; }
    VM_IF(VM_EQU(3, 4)) { visits += 10; }
    VM_ELSE_IF(VM_EQU(4, 4)) { visits++; }
    VM_ELSE { visits += 10; }
    CHECK(visits == 2);
    return 0;
}
static DWORD WINAPI worker(void *p) { return stress(); }
int main(void) {
    CHECK(stress() == 0);
    HANDLE threads[4];
    for (int i = 0; i < 4; ++i) {
        threads[i] = CreateThread(NULL, 0, worker, NULL, 0, NULL);
        CHECK(threads[i]);
    }
    CHECK(WaitForMultipleObjects(4, threads, TRUE, 60000) == WAIT_OBJECT_0);
    for (int i = 0; i < 4; ++i) {
        DWORD status;
        CHECK(GetExitCodeThread(threads[i], &status) && status == 0);
        CHECK(CloseHandle(threads[i]));
    }
    puts("VM_PASS");
    return 0;
}
