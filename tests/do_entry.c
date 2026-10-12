#include <stdio.h>
static unsigned entries;
static void note_entry(void) { ++entries; }
#include "../include/obfus.h"
#undef OBFH_FLOW_DO_GUARD
#define OBFH_FLOW_DO_GUARD (({ note_entry(); OBFH_FLOW_DO_ENTRY; 1; }))

int main(void) {
    unsigned i = 0, conditions = 0;
    entries = 0;
    do
        ;
    while (0);
    if (entries != 1) return 1;
    entries = 0;
    do {
        ++i;
        if (i == 2) continue;
        if (i == 4) break;
    } while (++conditions < 10);
    if (entries != 4 || i != 4 || conditions != 3) return 2;
    entries = 0;
    i = 0;
    do do
            ++i;
        while (i < 2);
    while (i < 2);
    if (entries != 3 || i != 2) return 3;
    entries = 0;
    do break;
    while (++conditions);
    if (entries != 1 || conditions != 3) return 4;
    puts("DO_ENTRY_PASS");
    return 0;
}
