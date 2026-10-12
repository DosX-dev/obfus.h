#include <stdio.h>
#include <windows.h>

#include "../include/obfus.h"

#if !NO_OBF && NO_ANTIDEBUG != 1
static HANDLE probe_gate;
static DWORD WINAPI probe_worker(void *unused) {
    (void)unused;
    if (WaitForSingleObject(probe_gate, 10000) != WAIT_OBJECT_0) return 1;
    for (int i = 0; i < 32; ++i) {
        if (obfh_ad_process_probe()) return 2;
#if ANTIDEBUG_V2 == 1
        if (obfh_ad_register_probe()) return 3;
#endif
    }
    return 0;
}
#if ANTIDEBUG_V2 == 1
static HANDLE hardware_step, hardware_done;
static int hardware_results[3];
static DWORD WINAPI hardware_worker(void *unused) {
    for (int i = 0; i < 3; ++i) {
        if (WaitForSingleObject(hardware_step, 10000) != WAIT_OBJECT_0) return 1;
        hardware_results[i] = obfh_ad_register_probe();
        SetEvent(hardware_done);
    }
    return WaitForSingleObject(hardware_step, 10000) == WAIT_OBJECT_0 ? 0 : 2;
}
static int hardware_checks(void) {
    hardware_step = CreateEventA(NULL, FALSE, FALSE, NULL);
    hardware_done = CreateEventA(NULL, FALSE, FALSE, NULL);
    if (!hardware_step || !hardware_done) return 20;
    HANDLE target = CreateThread(NULL, 0, hardware_worker, NULL, 0, NULL);
    if (!target) return 21;
    for (int i = 0; i < 3; ++i) {
        CONTEXT context = {0};
        context.ContextFlags = CONTEXT_DEBUG_REGISTERS;
        if (SuspendThread(target) == (DWORD)-1 || !GetThreadContext(target, &context)) return 22;
        context.Dr0 = (DWORD_PTR)&hardware_step;
        context.Dr7 = (context.Dr7 & ~0xffff00ffu) | (i == 1 ? 1u : 0u);
        if (!SetThreadContext(target, &context) || ResumeThread(target) == (DWORD)-1) return 23;
        SetEvent(hardware_step);
        if (WaitForSingleObject(hardware_done, 10000) != WAIT_OBJECT_0 || hardware_results[i] != (i == 1)) return 24;
        if (SuspendThread(target) == (DWORD)-1 || !GetThreadContext(target, &context)) return 25;
        // x64 Windows may clear disabled DR0 on resume; an enabled point must survive.
        if ((i == 1 && context.Dr0 != (DWORD_PTR)&hardware_step) || (context.Dr7 & 0xffu) != (i == 1 ? 1u : 0u)) return 26;
        if (ResumeThread(target) == (DWORD)-1) return 27;
    }
    SetEvent(hardware_step);
    DWORD status;
    if (WaitForSingleObject(target, 10000) != WAIT_OBJECT_0 || !GetExitCodeThread(target, &status) || status) return 28;
    CloseHandle(target);
    CloseHandle(hardware_step);
    CloseHandle(hardware_done);
    return 0;
}
#endif
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
#if ANTIDEBUG_V2 == 1
    int hardware_status = hardware_checks();
    if (hardware_status) return hardware_status;
#endif
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
