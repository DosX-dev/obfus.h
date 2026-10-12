#include <stdio.h>
#include <windows.h>

#include "../include/obfus.h"

int main(int argc, char **argv) {
#if !NO_OBF && NO_ANTIDEBUG != 1
    if (argc > 1) {
        puts("RESPONSE_ENTER");
        fflush(stdout);
        if (argv[1][0] == 'a') {
            if (!IsDebuggerPresent_proxy()) return 4;
            puts("ACTUAL_DETECTED");
            fflush(stdout);
            ANTI_DEBUG;
            puts("RESPONSE_RETURNED");
            return 5;
        }
        if (argv[1][0] == 's') {
            ANTI_DEBUG;
            puts("RESPONSE_RETURNED");
            return 1;
        }
        obfh_ad_react(0x12345678u, (unsigned int)(argv[1][0] - '0'));
        puts("RESPONSE_RETURNED");
        return 1;
    }
    if (IsDebuggerPresent_proxy()) return 2;
#endif
    int taken = 0;
    if (argc == 1)
        ANTI_DEBUG;
    else
        taken = 1;
    if (argc != 1)
        ANTI_DEBUG;
    else
        taken += 2;
    for (int i = 0; i < 3; ++i) ANTI_DEBUG;
    if (taken != 2) return 3;
    puts("ANTIDEBUG_PASS");
    return 0;
}
