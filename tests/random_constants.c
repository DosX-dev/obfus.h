#include <stdio.h>

#include "../include/obfus.h"
#undef printf
#undef puts
#undef if

/* Check the linear line-salted formula, including the
   captured counter's next draw and the source line at each expansion. */
#define DRAW(minimum, maximum) \
    { \
        enum { captured = __COUNTER__, \
               value = RND(minimum, maximum) }; \
        unsigned int expected = (minimum) + \
                                (((captured + 1 + __LINE__ + (unsigned int)OBFH_BUILD_SEED) * \
                                  2654435761u) % \
                                 ((maximum) - (minimum) + 1)); \
        if ((unsigned int)value != expected) return 1; \
    }

#define RANDOM_BATCH_16 RND(0, 255), RND(0, 255), RND(0, 255), RND(0, 255), RND(0, 255), RND(0, 255), RND(0, 255), RND(0, 255), RND(0, 255), RND(0, 255), RND(0, 255), RND(0, 255), RND(0, 255), RND(0, 255), RND(0, 255), RND(0, 255)
static const unsigned int random_cycle[] = {RANDOM_BATCH_16, RANDOM_BATCH_16, RANDOM_BATCH_16, RANDOM_BATCH_16, RANDOM_BATCH_16, RANDOM_BATCH_16, RANDOM_BATCH_16, RANDOM_BATCH_16, RANDOM_BATCH_16, RANDOM_BATCH_16, RANDOM_BATCH_16, RANDOM_BATCH_16, RANDOM_BATCH_16, RANDOM_BATCH_16, RANDOM_BATCH_16, RANDOM_BATCH_16};
#undef RANDOM_BATCH_16

int main(void) {
    unsigned int counts[256] = {0};
    for (unsigned int i = 0; i < 256; ++i) {
        if (random_cycle[i] > 255) return 2;
        ++counts[random_cycle[i]];
    }
    for (unsigned int i = 0; i < 256; ++i)
        if (counts[i] != 1) return 3;
#line 100
    DRAW(0, 0);
    DRAW(0, 255);
    DRAW(1, 15);
    DRAW(1, 31);
    DRAW(0, 65535);
    DRAW(1, 32767);
    DRAW(1, 2147483646u);
    DRAW(-100, 100);
    DRAW(-255, -1);
#line 20000
    DRAW(0, 0);
    DRAW(0, 255);
    DRAW(1, 15);
    DRAW(1, 31);
    DRAW(0, 65535);
    DRAW(1, 32767);
    DRAW(1, 2147483646u);
    DRAW(-100, 100);
    DRAW(-255, -1);
#line 50000
    DRAW(0, 255);
    DRAW(1, 32767);
    puts("RANDOM_CONSTANTS_PASS");
    return 0;
}
