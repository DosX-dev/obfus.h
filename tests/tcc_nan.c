/* TCC 0.9.27 x64 diagnostic: negated ordered comparisons with quiet NaN.
   No obfus.h is included. Correct C behavior returns zero on both architectures. */
#include <stdio.h>
static int consume(int value) { return value; }
int main(void) {
    union {
        unsigned long long bits;
        double value;
    } x = {0x7ff8000012345678ull};
    volatile double n = x.value, y = 1.0;
    int calls = 0;
    int comma_not = (++calls, !(n < y));
    printf("comma_not=%d calls=%d\n", comma_not, calls);
    int stored = n < y;
    int passed = consume(n < y);
    int branch = 0;
    if (n < y) branch = 1;
    printf("stored=%d passed=%d branch=%d\n", stored, passed, branch);
    unsigned int normal = (n < y) ? 0x123u : 0x456u;
    unsigned int inverted = !(n < y) ? 0x456u : 0x123u;
    unsigned int check_normal = !(n < y) ? 0x123u : 0x456u;
    unsigned int check_inverted = !(!(n < y)) ? 0x456u : 0x123u;
    printf("normal=%x inverted=%x check_normal=%x check_inverted=%x\n", normal, inverted, check_normal, check_inverted);
    return comma_not != 1 || calls != 1 || stored || passed || branch || normal != 0x456u || inverted != 0x456u || check_normal != 0x123u || check_inverted != 0x123u;
}
