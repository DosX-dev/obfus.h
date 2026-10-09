#include <stdio.h>
#include <windows.h>
#include "../include/obfus.h"

static int calls;
static int valued(int x) {
    if (x > 0)
        return ++calls;
    else if (x < 0)
        return --calls;
    else
        return calls;
}
static void empty(int x) {
    if (x)
        return;
    else
        ++calls;
    return;
}
static int nested(int a, int b) {
    if (a)
        if (b)
            return 11;
        else
            return 12;
    else
        return 13;
}
static int loop(int n) {
    for (int i = 0; i < n; ++i)
        if (i == 3)
            return i;
    return -1;
}
struct Pair { int a; double b; };
static struct Pair pair(void) {
    struct Pair p = {17, 2.5};
    return p;
}
static const char *pointer(const char *p) { return p; }
static double number(double x) { return x * 0.5; }
static int comma(void) { return (++calls, calls + 10); }

#undef if
#undef else
#undef printf
int main(void) {
    const char *p = "return";
    struct Pair v = pair();
    if (valued(1) != 1 || calls != 1) return 1;
    if (valued(-1) != 0 || calls != 0) return 2;
    if (valued(0) != 0 || calls != 0) return 3;
    empty(1);
    if (calls) return 4;
    empty(0);
    if (calls != 1) return 5;
    if (nested(1, 1) != 11 || nested(1, 0) != 12 || nested(0, 0) != 13) return 6;
    if (loop(6) != 3 || loop(2) != -1) return 7;
    if (v.a != 17 || v.b != 2.5 || pointer(p) != p || number(7) != 3.5) return 8;
    if (comma() != 12 || calls != 2) return 9;
    printf("RETURN_PASS\n");
    return 0;
}
