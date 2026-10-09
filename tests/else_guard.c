#include <stdio.h>
#include <windows.h>
#include "../include/obfus.h"
#undef if
#undef else
#undef return
#undef printf

#if !NO_OBF && !OBFH_EDITOR_VIEW && NO_CFLOW != 1
static unsigned check(void) {
    unsigned result = 0;
    // Separate sites exercise different compile-time tags and completions.
    result += !!OBFH_FLOW_ELSE_GUARD;
    result += !!OBFH_FLOW_ELSE_GUARD;
    result += !!OBFH_FLOW_ELSE_GUARD;
    result += !!OBFH_FLOW_ELSE_GUARD;
    result += !!OBFH_FLOW_ELSE_GUARD;
    result += !!OBFH_FLOW_ELSE_GUARD;
    result += !!OBFH_FLOW_ELSE_GUARD;
    result += !!OBFH_FLOW_ELSE_GUARD;
    return result;
}
#endif

int main(void) {
#if !NO_OBF && !OBFH_EDITOR_VIEW && NO_CFLOW != 1
    for (unsigned i = 0; i < 256; ++i)
        if (check()) return 1;
#endif
    printf("ELSE_GUARD_PASS\n");
    return 0;
}
