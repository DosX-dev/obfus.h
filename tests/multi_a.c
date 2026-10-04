#include <windows.h>

#include "../include/obfus.h"
int multi_a(int x) {
    if (x)
        return VM_ADD(x, 11);
    else
        return -7;
}
__declspec(dllexport) int multi_export(int x) { return multi_a(x); }
