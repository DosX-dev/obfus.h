//+---------------------------------------------------------------------------
//
//  dll.c - Windows DLL example - dynamically linked part
//

#include <windows.h>
#include "../include/obfus.h"

// Exported data
__declspec(dllexport) const char *hello_data = "(not set)";

// Exported function
__declspec(dllexport) void hello_func(void)
{
    MessageBoxA(NULL, hello_data, "From DLL", MB_ICONINFORMATION);
}
