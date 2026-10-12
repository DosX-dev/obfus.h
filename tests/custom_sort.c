#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
static void reference(void *p, size_t n, size_t w, int (*cmp)(const void *, const void *)) { qsort(p, n, w, cmp); }
#include "../include/obfus.h"
#undef if
#undef else
#undef case
#undef for
#undef while
#undef break
#undef return
static unsigned calls, state = 12345, nested;
static size_t width;
static int compare(const void *a, const void *b) {
    ++calls;
    return (memcmp)(a, b, width);
}
static int extreme(const void *a, const void *b) {
    ++calls;
    return *(const int *)a < *(const int *)b ? INT_MIN : *(const int *)a > *(const int *)b ? INT_MAX
                                                                                           : 0;
}
static int reentrant(const void *a, const void *b) {
    int other[] = {9, 1, 5};
    qsort(other, 3, sizeof(int), extreme);
    nested |= other[0] != 1 || other[1] != 5 || other[2] != 9;
    return extreme(a, b);
}
#define CHECK(x) \
    do { \
        if (!(x)) { \
            (printf)("SORT_FAIL:%d\n", __LINE__); \
            return 1; \
        } \
    } while (0)
int main(void) {
    unsigned char a[8194], b[8194];
    for (width = 1; width <= 63; ++width) {
        for (size_t n = 0; n <= 128; ++n) {
            (memset)(a, 0xa5, sizeof a);
            for (size_t i = 0; i < n * width; ++i) {
                state = state * 1664525u + 1013904223u;
                a[i + 1] = (unsigned char)(state >> 24);
            }
            (memcpy)(b, a, sizeof a);
            calls = 0;
            qsort(a + 1, n, width, compare);
            CHECK(n >= 2 || !calls);
            CHECK(calls <= 4 * (n + 1) * 8);
            reference(b + 1, n, width, compare);
            CHECK(!(memcmp)(a, b, sizeof a));
        }
    }
    int values[] = {INT_MAX, 0, INT_MIN, -1, 0, 42};
    unsigned pa = 0, na = 0, wa = 0, ca = 0;
    qsort((++pa, values), (++na, 6), (++wa, sizeof(int)), (++ca, reentrant));
    CHECK(pa == 1 && na == 1 && wa == 1 && ca == 1 && !nested);
    CHECK(values[0] == INT_MIN && values[1] == -1 && values[2] == 0 && values[3] == 0 && values[4] == 42 && values[5] == INT_MAX);
    CHECK(__builtin_types_compatible_p(__typeof__(qsort(values, 6, sizeof(int), extreme)), void));
    SYSTEM_INFO info;
    (GetSystemInfo)(&info);
    char *page = (VirtualAlloc)(NULL, info.dwPageSize * 2, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    DWORD old;
    CHECK(page && (VirtualProtect)(page + info.dwPageSize, info.dwPageSize, PAGE_NOACCESS, &old));
    width = 3;
    unsigned char *edge = (unsigned char *)page + info.dwPageSize - 99;
    for (unsigned i = 0; i < 99; ++i) edge[i] = (unsigned char)(99 - i);
    qsort(edge, 33, 3, compare);
    for (unsigned i = 1; i < 33; ++i) CHECK(compare(edge + (i - 1) * 3, edge + i * 3) <= 0);
    (VirtualFree)(page, 0, MEM_RELEASE);
    (puts)("CUSTOM_SORT_PASS");
    return 0;
}
