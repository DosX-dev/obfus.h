#include <stdio.h>
#include <windows.h>

#include "../include/obfus.h"

#if !NO_OBF && NO_ANTIDEBUG != 1
static HANDLE probe_gate;
static DWORD WINAPI probe_worker(void *unused) {
    (void)unused;
    if (WaitForSingleObject(probe_gate, 10000) != WAIT_OBJECT_0) return 1;
    for (int i = 0; i < 32; ++i)
        if (obfh_ad_process_probe()) return 2;
    return 0;
}
#endif

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
    // Race cold address publication, then exercise immutable warm reads.
    HANDLE workers[4];
    probe_gate = CreateEventA(NULL, TRUE, FALSE, NULL);
    if (!probe_gate) return 10;
    for (int i = 0; i < 4; ++i) {
        workers[i] = CreateThread(NULL, 0, probe_worker, NULL, 0, NULL);
        if (!workers[i]) return 11;
    }
    SetEvent(probe_gate);
    for (int i = 0; i < 4; ++i) {
        DWORD status;
        if (WaitForSingleObject(workers[i], 10000) != WAIT_OBJECT_0 ||
            !GetExitCodeThread(workers[i], &status) || status) return 12;
        CloseHandle(workers[i]);
    }
    CloseHandle(probe_gate);
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
