#include <stdio.h>

#include "../include/obfus.h"
int multi_a(int x);
int multi_b(int x);
int main(void) {
    if (multi_a(31) != 42 || multi_a(0) != -7 || multi_b(101) != 101) return 1;
    puts("MULTI_PASS");
    return 0;
}
