#include <stdio.h>
#include <windows.h>

#include "../include/obfus.h"
int main(int argc, char **argv) {
    if (argc != 2) return 1;
    HMODULE module = LoadLibraryA(argv[1]);
    if (!module) return 2;
    int (*function)(int) = (int (*)(int))GetProcAddress(module, "multi_export");
    if (!function || function(31) != 42 || function(0) != -7) return 3;
#if !NO_OBF
    if (!GetProcAddress(module, "WhatSoundDoesACowMake")) return 4;
#endif
    if (!FreeLibrary(module)) return 5;
    puts("MULTI_DLL_PASS");
    return 0;
}
