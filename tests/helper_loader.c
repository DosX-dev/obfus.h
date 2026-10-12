#include <stdio.h>
#include <windows.h>

#include "../include/obfus.h"
#undef if
#undef else
#undef case
#undef for
#undef while
#undef return
#undef puts
#undef fflush
static DWORD WINAPI worker(void *unused) {
    for (int i = 0; i < 32; ++i) {
        HMODULE module = LoadLibraryA("kernel32.dll");
        if (!module || module != (GetModuleHandleA)("kernel32.dll")) return 1;
        if (!(FreeLibrary)(module)) return 2;
    }
    return 0;
}
static int arguments;
static const char *missing(void) {
    arguments++;
    return "obfh_missing_loader_test_73d84729.dll";
}
int main(void) {
    HANDLE threads[8];
    for (int i = 0; i < 8; ++i) {
        threads[i] = (CreateThread)(NULL, 0, worker, NULL, 0, NULL);
        if (!threads[i]) return 3;
    }
    if ((WaitForMultipleObjects)(8, threads, TRUE, 15000) != WAIT_OBJECT_0) return 4;
    for (int i = 0; i < 8; ++i) {
        DWORD result;
        if (!(GetExitCodeThread)(threads[i], &result) || result) return 5;
        (CloseHandle)(threads[i]);
    }
    (SetLastError)(0);
    HMODULE module = LoadLibraryA(missing());
    if (module || arguments != 1 || (GetLastError)() != ERROR_MOD_NOT_FOUND) return 6;
    if (puts("HELPER_LOADER_PASS") == EOF || fflush(stdout)) return 7;
    return 0;
}
