#include <stdio.h>
#include <string.h>
#include <windows.h>

#include "../include/obfus.h"
__declspec(dllimport) extern const char *hello_data;
__declspec(dllimport) int hello_func(void);
__declspec(dllimport) int transform(int value);
int main(void) {
    if (strcmp(hello_data, "(not set)")) return 1;
    hello_data = "Hello World!";
    if (hello_func() != 42) return 2;
    for (int i = -1000; i <= 1000; ++i)
        if (transform(i) != i * 7 + 3) return 3;
    puts("DLL_IMPORT_PASS");
    return 0;
}
