#include <stdio.h>
#include <windows.h>
int main(int argc, char **argv) {
    if (argc != 2) return 1;
    for (int i = 0; i < 20; ++i) {
        HMODULE module = LoadLibraryA(argv[1]);
        int (*probe)(void) = module ? (void *)GetProcAddress(module, "pool_probe") : NULL;
        if (!probe || probe()) return 2;
        FreeLibrary(module);
    }
    HMODULE module = LoadLibraryA(argv[1]);
    int (*probe)(void) = module ? (void *)GetProcAddress(module, "pool_probe") : NULL;
    void (*delay)(HANDLE, HANDLE) = module ? (void *)GetProcAddress(module, "pool_delay") : NULL;
    HANDLE entered = CreateEventA(NULL, TRUE, FALSE, NULL);
    HANDLE finish = CreateEventA(NULL, TRUE, FALSE, NULL);
    if (!probe || !delay || !entered || !finish) return 3;
    delay(entered, finish);
    if (probe() || WaitForSingleObject(entered, 1000) != WAIT_OBJECT_0) return 4;
    FreeLibrary(module);
    // A timed-out callback must keep its own DLL mapped until after it returns.
    if (!GetModuleHandleA(argv[1])) return 5;
    SetEvent(finish);
    DWORD start = GetTickCount();
    while (GetModuleHandleA(argv[1]) && GetTickCount() - start < 5000) Sleep(1);
    if (GetModuleHandleA(argv[1])) return 6;
    CloseHandle(entered);
    CloseHandle(finish);
    puts("ANTIDEBUG_POOL_DLL_PASS");
    return 0;
}
