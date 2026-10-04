#include <string.h>
#include <windows.h>

#include "../include/obfus.h"
__declspec(dllexport) const char *hello_data = "(not set)";
__declspec(dllexport) int hello_func(void) { return strcmp(hello_data, "Hello World!") == 0 ? VM_ADD(20, 22) : -1; }
__declspec(dllexport) int transform(int value) { return VM_ADD(VM_MUL(value, 7), 3); }
BOOL WINAPI DllMain(HINSTANCE instance, DWORD reason, LPVOID reserved) { return TRUE; }
