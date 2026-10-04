// Real-header WinAPI pointer and string integration checks.
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <windows.h>

#include "../include/obfus.h"
#define CHECK(x)                                           \
    do {                                                   \
        if (!(x)) {                                        \
            fprintf(stderr, "failed line %d\n", __LINE__); \
            return 1;                                      \
        }                                                  \
    } while (0)
DWORD WINAPI worker(void *p) { return *(DWORD *)p; }
int main(void) {
    ULONG_PTR wide = sizeof(void *) == 8 ? 0x1234567887654321ULL : 0x87654321UL;
#if !NO_OBF
    CHECK(obfh_uintptr_proxy(wide) == wide);
    CHECK((ULONG_PTR)OBFH_PTR(void *, wide) == wide);
#endif
    char *hidden = HIDE_STRING("hello");
    char *second_hidden = HIDE_STRING("second-hidden");
    char *empty_hidden = HIDE_STRING("");
    char overwrite[8192];
    memset(overwrite, 'x', sizeof overwrite);
    CHECK(strcmp(hidden, "hello") == 0);
    CHECK(strcmp(second_hidden, "second-hidden") == 0);
    CHECK(empty_hidden[0] == 0);
#if !NO_OBF
    hidden[0] = 'H';
    CHECK(strcmp(hidden, "Hello") == 0);
    CHECK(strcmp(second_hidden, "second-hidden") == 0);
#endif
    char large[8192];
    memset(large, 'a', sizeof large);
    large[0] = 0;
    large[8191] = 0;
#if !NO_OBF
    CHECK(obfh_process_hidden_string(large) == large + 1);
    CHECK(strlen(obfh_process_hidden_string(large)) == 8190);
#endif
    char name[MAX_PATH];
    DWORD name_length = GetTempPath(MAX_PATH, name);
    CHECK(name_length > 0 && name_length < MAX_PATH);
    CHECK(name_length + sizeof("obfh-wrapper-regression.tmp") <= sizeof name);
    strcat(name, "obfh-wrapper-regression.tmp");
    HANDLE file = CreateFile(name, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_TEMPORARY | FILE_FLAG_DELETE_ON_CLOSE, NULL);
    CHECK(file != INVALID_HANDLE_VALUE);
    DWORD count = 0;
    CHECK(WriteFile(file, "hello", 5, &count, NULL) && count == 5);
    CHECK(SetFilePointer(file, 0, NULL, FILE_BEGIN) == 0);
    char buffer[8] = {0};
    CHECK(ReadFile(file, buffer, 5, &count, NULL) && count == 5);
    CHECK(strcmp(buffer, "hello") == 0);
    CHECK(CloseHandle(file));
    void *memory = VirtualAlloc(NULL, 4096, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    CHECK(memory);
    memset(memory, 1, 4096);
    CHECK(VirtualFree(memory, 0, MEM_RELEASE));
    HANDLE heap = HeapCreate(0, 4096, 0);
    CHECK(heap);
    memory = HeapAlloc(heap, 0, 64);
    CHECK(memory);
    CHECK(HeapFree(heap, 0, memory));
    CHECK(HeapDestroy(heap));
    HGLOBAL global = GlobalAlloc(GMEM_FIXED, 64);
    CHECK(global);
    CHECK(GlobalFree(global) == NULL);
    DWORD result = 42, id = 0, exit_code = 0;
    HANDLE thread = CreateThread(NULL, 0, worker, &result, 0, &id);
    CHECK(thread);
    CHECK(WaitForSingleObject(thread, INFINITE) == WAIT_OBJECT_0);
    CHECK(GetExitCodeThread(thread, &exit_code) && exit_code == 42);
    CHECK(CloseHandle(thread));
    HANDLE event = CreateEventA(NULL, TRUE, FALSE, NULL);
    CHECK(event);
    CHECK(SetEvent(event));
    CHECK(WaitForMultipleObjects(1, &event, TRUE, 0) == WAIT_OBJECT_0);
    CHECK(ResetEvent(event));
    CHECK(CloseHandle(event));
    RECT rect;
    CHECK(GetWindowRect(GetDesktopWindow(), &rect));
    CHECK(GetClientRect(GetDesktopWindow(), &rect));
    CHECK(GetCurrentProcess() == (HANDLE)(LONG_PTR)-1);
    CHECK(GetModuleHandle(NULL));
    CHECK(GetModuleFileName(NULL, name, MAX_PATH));
    STARTUPINFO info;
    GetStartupInfo(&info);
    CHECK(info.cb == sizeof info);
    puts("String and WinAPI wrapper regressions passed");
    return 0;
}
