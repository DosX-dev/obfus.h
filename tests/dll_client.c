#include <stdio.h>
#include <string.h>
#include <windows.h>

#include "../include/obfus.h"
int main(int argc, char **argv) {
    if (argc != 2) return 1;
    HMODULE dll = LoadLibraryA(argv[1]);
    if (!dll) return 2;
    int (*hello)(void) = (int (*)(void))GetProcAddress(dll, "hello_func");
    int (*transformer)(int) = (int (*)(int))GetProcAddress(dll, "transform");
    const char **data = (const char **)GetProcAddress(dll, "hello_data");
    if (!hello || !transformer || !data || strcmp(*data, "(not set)")) return 3;
    *data = "Hello World!";
    if (hello() != 42) return 4;
    for (int i = -1000; i <= 1000; ++i)
        if (transformer(i) != i * 7 + 3) return 5;
    if (!FreeLibrary(dll)) return 6;
    if (puts("DLL_PASS") < 0) return 7;
    return fflush(stdout) == 0 ? 0 : 7;
}
