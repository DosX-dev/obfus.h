// Real Windows debugger fixture. The protected child must detect this host and
// stay in its remote response without executing invalid instructions.
#include <windows.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char **argv) {
    if (argc != 2) return 1;
    SECURITY_ATTRIBUTES security = {sizeof security, NULL, TRUE};
    HANDLE reader, writer;
    if (!CreatePipe(&reader, &writer, &security, 0)) return 2;
    SetHandleInformation(reader, HANDLE_FLAG_INHERIT, 0);
    STARTUPINFOA startup = {sizeof startup};
    startup.dwFlags = STARTF_USESTDHANDLES;
    startup.hStdOutput = startup.hStdError = writer;
    startup.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
    PROCESS_INFORMATION process = {0};
    char command[4096];
    if (strlen(argv[1]) + 16 >= sizeof command) return 3;
    sprintf(command, "\"%s\" actual", argv[1]);
    if (!CreateProcessA(argv[1], command, NULL, NULL, TRUE,
                        DEBUG_ONLY_THIS_PROCESS | CREATE_NO_WINDOW, NULL, NULL, &startup, &process)) return 4;
    CloseHandle(writer);
    DWORD start = GetTickCount(), detected_at = 0;
    int failed = 0, exited = 0;
    char output[4096] = {0};
    size_t used = 0;
    while (GetTickCount() - start < 5000) {
        DEBUG_EVENT event;
        if (WaitForDebugEvent(&event, 20)) {
            if (event.dwDebugEventCode == CREATE_PROCESS_DEBUG_EVENT && event.u.CreateProcessInfo.hFile)
                CloseHandle(event.u.CreateProcessInfo.hFile);
            if (event.dwDebugEventCode == LOAD_DLL_DEBUG_EVENT && event.u.LoadDll.hFile)
                CloseHandle(event.u.LoadDll.hFile);
            if (event.dwDebugEventCode == EXCEPTION_DEBUG_EVENT &&
                event.u.Exception.ExceptionRecord.ExceptionCode != EXCEPTION_BREAKPOINT) failed = 1;
            if (event.dwDebugEventCode == EXIT_PROCESS_DEBUG_EVENT) exited = 1;
            ContinueDebugEvent(event.dwProcessId, event.dwThreadId, DBG_CONTINUE);
        }
        DWORD available = 0, got = 0;
        if (used + 1 < sizeof output && PeekNamedPipe(reader, NULL, 0, NULL, &available, NULL) && available) {
            DWORD capacity = (DWORD)(sizeof output - used - 1);
            if (ReadFile(reader, output + used, available < capacity ? available : capacity, &got, NULL)) {
                used += got; output[used] = 0;
            }
        }
        if (!detected_at && strstr(output, "ACTUAL_DETECTED")) detected_at = GetTickCount();
        if (failed || exited || strstr(output, "RESPONSE_RETURNED")) { failed = 1; break; }
        if (detected_at && GetTickCount() - detected_at >= 500) break;
    }
    if (!detected_at) failed = 1;
    TerminateProcess(process.hProcess, 0);
    start = GetTickCount();
    while (!exited && GetTickCount() - start < 2000) {
        DEBUG_EVENT event;
        if (WaitForDebugEvent(&event, 20)) {
            exited = event.dwDebugEventCode == EXIT_PROCESS_DEBUG_EVENT;
            ContinueDebugEvent(event.dwProcessId, event.dwThreadId, DBG_CONTINUE);
        }
    }
    CloseHandle(reader); CloseHandle(process.hThread); CloseHandle(process.hProcess);
    if (failed || !exited) { fprintf(stderr, "debugger child failed: %s\n", output); return 6; }
    puts("ANTIDEBUG_REAL_DEBUGGER_PASS");
    return 0;
}
