#include <stdio.h>
#include <windows.h>

#include "../include/obfus.h"
#define CHECK(x)                                                            \
    do {                                                                    \
        if (!(x)) {                                                         \
            fprintf(stderr, "charset failure line %d: %s\n", __LINE__, #x); \
            return 1;                                                       \
        }                                                                   \
    } while (0)
int main(void) {
    int calls = 0;
    HMODULE kernel = GetModuleHandle((++calls, TEXT("kernel32.dll")));
    CHECK(kernel && calls == 1);
    CHECK(GetModuleHandleA("kernel32.dll") == kernel);
    CHECK(GetModuleHandleW(L"kernel32.dll") == kernel);
    STARTUPINFO startup;
    GetStartupInfo(&startup);
    CHECK(startup.cb == sizeof(startup));
    TCHAR filename[MAX_PATH], path[MAX_PATH];
    CHECK(GetModuleFileName(NULL, filename, MAX_PATH));
    DWORD count = GetTempPath(MAX_PATH, path);
    CHECK(count && count < MAX_PATH - 48);
#ifdef UNICODE
    wsprintfW(path + count, L"obfh-\x042e\x043d\x0438\x043a\x043e\x0434-\x6f22\x5b57-%lu.tmp", GetCurrentProcessId());
#else
    wsprintfA(path + count, "obfh-ansi-%lu.tmp", GetCurrentProcessId());
#endif
    calls = 0;
    HANDLE file = CreateFile((++calls, path), GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_TEMPORARY, NULL);
    CHECK(file != INVALID_HANDLE_VALUE && calls == 1);
    DWORD written;
    CHECK(WriteFile(file, "charset", 7, &written, NULL) && written == 7);
    CHECK(CloseHandle(file));
    CHECK(DeleteFile(path));
    HANDLE nul = CreateFile(TEXT("NUL"), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
    CHECK(nul != INVALID_HANDLE_VALUE);
    CHECK(CloseHandle(nul));
    puts("CHARSET_PASS");
    return 0;
}
