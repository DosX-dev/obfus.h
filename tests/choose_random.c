#include <stdio.h>
#include "../include/obfus.h"
#undef if
#undef for
#undef while
#undef printf
#undef puts
#undef fflush

// Exactly one RND expansion: the counter immediately follows 'before'.
#define DIRECT(n) do { \
    enum { before = __COUNTER__, line = __LINE__ }; \
    int chosen = __builtin_choose_expr(RND(0, 7) < 4, 17, 31); \
    unsigned int value = ((before + 1u + line * line + (unsigned int)OBFH_BUILD_SEED) * 2654435761u) % 8u; \
    if (chosen != (value < 4 ? 17 : 31)) return n + 1; \
    seen |= chosen == 17 ? 1 : 2; \
    if (printf("DIRECT %d %u %d\n", n, value, chosen) < 0) return 90; \
} while (0)
#define CAPTURED(n) do { \
    enum { value = RND(0, 7) }; \
    int chosen = __builtin_choose_expr(value < 4, 17, 31); \
    if (chosen != (value < 4 ? 17 : 31)) return n + 40; \
    if (printf("CAPTURED %d %u %d\n", n, value, chosen) < 0) return 91; \
} while (0)
int main(void) {
    int seen = 0;
    for (unsigned int index = 0; index < 128; ++index) {
        unsigned int extra = OBFH_CFLOW_EXTRA_INDEX(index);
        if (extra >= 128 || extra == index || extra == 86 || (extra >= 89 && extra <= 94)) return 95;
    }
    DIRECT(0); DIRECT(1); DIRECT(2); DIRECT(3);
    DIRECT(4); DIRECT(5); DIRECT(6); DIRECT(7);
    DIRECT(8); DIRECT(9); DIRECT(10); DIRECT(11);
    DIRECT(12); DIRECT(13); DIRECT(14); DIRECT(15);
    CAPTURED(0); CAPTURED(1); CAPTURED(2); CAPTURED(3);
    CAPTURED(4); CAPTURED(5); CAPTURED(6); CAPTURED(7);
    CAPTURED(8); CAPTURED(9); CAPTURED(10); CAPTURED(11);
    CAPTURED(12); CAPTURED(13); CAPTURED(14); CAPTURED(15);
    if (seen != 3) return 92;
    if (puts("CHOOSE_RANDOM_PASS") < 0) return 93;
    return fflush(stdout) == 0 ? 0 : 94;
}
