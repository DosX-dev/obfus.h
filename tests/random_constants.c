#include <stdio.h>

static unsigned int reference_mix(unsigned int value) {
    value ^= value >> 16;
    value *= 2246822507u;
    value ^= value >> 13;
    return value * 3266489909u;
}

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
                                (((captured + 1 + __LINE__ + (unsigned int)OBFH_SEED) * \
                                  2654435761u) % \
                                 ((maximum) - (minimum) + 1)); \
        if ((unsigned int)value != expected) return 1; \
    }

#define RANDOM_BATCH_16 RND(0, 255), RND(0, 255), RND(0, 255), RND(0, 255), RND(0, 255), RND(0, 255), RND(0, 255), RND(0, 255), RND(0, 255), RND(0, 255), RND(0, 255), RND(0, 255), RND(0, 255), RND(0, 255), RND(0, 255), RND(0, 255)
static const unsigned int random_cycle[] = {RANDOM_BATCH_16, RANDOM_BATCH_16, RANDOM_BATCH_16, RANDOM_BATCH_16, RANDOM_BATCH_16, RANDOM_BATCH_16, RANDOM_BATCH_16, RANDOM_BATCH_16, RANDOM_BATCH_16, RANDOM_BATCH_16, RANDOM_BATCH_16, RANDOM_BATCH_16, RANDOM_BATCH_16, RANDOM_BATCH_16, RANDOM_BATCH_16, RANDOM_BATCH_16};
#undef RANDOM_BATCH_16

#define CHECK_CAPTURE(input, salt) \
    { \
        enum { OBFH_MIX_FIELDS(mixed, input), \
               OBFH_DATA_FIELDS(data, salt), \
               OBFH_GUI_FIELDS(gui, input, salt), \
               line = __LINE__, \
               before = __COUNTER__, \
               OBFH_DATA_FIELDS(counter_draw, __COUNTER__), \
               after = __COUNTER__ }; \
        unsigned int seed = (unsigned int)OBFH_SEED; \
        if ((unsigned int)mixed != reference_mix(input)) return 4; \
        if ((unsigned int)data != reference_mix(line ^ seed ^ (unsigned int)(salt)*2654435761u)) return 5; \
        if ((unsigned int)gui != reference_mix((unsigned int)(input) ^ seed ^ (salt))) return 6; \
        if (after != before + 2 || (unsigned int)counter_draw != reference_mix(line ^ seed ^ (before + 1u) * 2654435761u)) return 7; \
        enum { OBFH_GUI_KEY_FIELDS(input) }; \
        if (OBFH_GUI_SITE_KEY != OBFH_GUI_KEY(input)) return 8; \
    }

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
    CHECK_CAPTURE(0u, 0u);
    CHECK_CAPTURE(1u, 1u);
    CHECK_CAPTURE(0x80000000u, 0x47554931u);
    CHECK_CAPTURE(0xffffffffu, 0xffffffffu);
    puts("RANDOM_CONSTANTS_PASS");
    return 0;
}
