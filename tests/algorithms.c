#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#include "../include/obfus.h"
#define CHECK(x)                                                      \
    do {                                                              \
        if (!(x)) {                                                   \
            fprintf(stderr, "algorithm failure line %d\n", __LINE__); \
            return 1;                                                 \
        }                                                             \
    } while (0)
static int fib(int n) {
    if (n <= 2)
        return 1;
    else
        return fib(n - 1) + fib(n - 2);
}
static int compare(const void *a, const void *b) {
    int x = *(const int *)a, y = *(const int *)b;
    return (x > y) - (x < y);
}
int main(void) {
    int expected[] = {1, 1, 2, 3, 5, 8, 13, 21, 34, 55, 89, 144, 233, 377, 610, 987, 1597, 2584, 4181, 6765};
    for (int n = 1; n <= 20; ++n) CHECK(fib(n) == expected[n - 1]);
    unsigned int seed = 123;
    for (int round = 0; round < 30; ++round) {
        int a[256], oracle[256];
        for (int i = 0; i < 256; ++i) {
            seed = seed * 1664525u + 1013904223u;
            a[i] = (int)(seed % 2001) - 1000;
            oracle[i] = a[i];
        }
        qsort(oracle, 256, sizeof(int), compare);
        for (int i = 1; i < 256; ++i) {
            int key = a[i], j = i - 1;
            while (j >= 0 && a[j] > key) {
                a[j + 1] = a[j];
                --j;
            }
            a[j + 1] = key;
        }
        CHECK(memcmp(a, oracle, sizeof a) == 0);
    }
    // Nested flow, continue, break, switch and side effects.
    int total = 0, effects = 0;
    for (int i = 0; i < 100; ++i) {
        if ((effects++, i % 2)) continue;
        switch (i % 3) {
            case 0:
                total += 3;
                break;
            case 1:
                total += 2;
                break;
            default:
                total++;
        }
    }
    CHECK(effects == 100 && total == 100);
    // Headless snake-style movement used by the old disassembly example.
    int x[64] = {12}, y[64] = {12}, length = 1, score = 0;
    for (int step = 0; step < 1000; ++step) {
        if (step % 32 == 0 && length < 64) {
            ++length;
            ++score;
        }
        memmove(x + 1, x, (length - 1) * sizeof(int));
        memmove(y + 1, y, (length - 1) * sizeof(int));
        x[0] = (x[0] + 1) % 24;
        y[0] = (y[0] + (step % 24 == 0)) % 24;
        CHECK(x[0] >= 0 && x[0] < 24 && y[0] >= 0 && y[0] < 24);
    }
    CHECK(length == 33 && score == 32);
    puts("ALGORITHMS_PASS");
    return 0;
}
