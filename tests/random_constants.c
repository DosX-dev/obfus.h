#include <stdio.h>

#include "../include/obfus.h"
#undef printf
#undef puts
#undef if

/* Compare the simplified macro with the original formula, including the
   captured counter's next draw and the source line at each expansion. */
#define DRAW(minimum, maximum)                                                                             \
    {                                                                                                      \
        enum { captured = __COUNTER__,                                                                     \
               value = RND(minimum, maximum) };                                                            \
        unsigned int expected = (minimum) +                                                                \
                                (((captured + 1 + (__LINE__ * __LINE__) + (unsigned int)OBFH_BUILD_SEED) * \
                                  2654435761u) %                                                           \
                                 ((maximum) - (minimum) + 1));                                             \
        if ((unsigned int)value != expected) return 1;                                                     \
    }

int main(void) {
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
    puts("RANDOM_CONSTANTS_PASS");
    return 0;
}
