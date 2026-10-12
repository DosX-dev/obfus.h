#include <stdio.h>
#include <windows.h>

#include "../include/obfus.h"
#undef if
#undef else
#undef for
#undef while
#undef break
#define if(...) OBFH_GUI_FORBIDDEN_IF
#define else OBFH_GUI_FORBIDDEN_ELSE
#define for(...) OBFH_GUI_FORBIDDEN_FOR
#define while(...) OBFH_GUI_FORBIDDEN_WHILE
#define break OBFH_GUI_FORBIDDEN_BREAK
int main(void) {
    unsigned int fail = 0;
#ifdef UNICODE
    fail |= !__builtin_types_compatible_p(__typeof__(&CreateFile), __typeof__(&CreateFileW));
    fail |= !__builtin_types_compatible_p(__typeof__(&GetModuleHandle), __typeof__(&GetModuleHandleW));
    fail |= !__builtin_types_compatible_p(__typeof__(&GetStartupInfo), __typeof__(&GetStartupInfoW));
    fail |= !__builtin_types_compatible_p(__typeof__(&GetModuleFileName), __typeof__(&GetModuleFileNameW));
    fail |= !__builtin_types_compatible_p(__typeof__(&GetTempPath), __typeof__(&GetTempPathW));
#else
    fail |= !__builtin_types_compatible_p(__typeof__(&CreateFile), __typeof__(&CreateFileA));
    fail |= !__builtin_types_compatible_p(__typeof__(&GetModuleHandle), __typeof__(&GetModuleHandleA));
    fail |= !__builtin_types_compatible_p(__typeof__(&GetStartupInfo), __typeof__(&GetStartupInfoA));
    fail |= !__builtin_types_compatible_p(__typeof__(&GetModuleFileName), __typeof__(&GetModuleFileNameA));
    fail |= !__builtin_types_compatible_p(__typeof__(&GetTempPath), __typeof__(&GetTempPathA));
#endif
    fail |= !__builtin_types_compatible_p(__typeof__(SendMessageW(NULL, 0, 0, 0)), LRESULT);
    fail |= !__builtin_types_compatible_p(__typeof__(CreateWindowExA(0, "x", "x", 0, 0, 0, 0, 0, NULL, NULL, NULL, NULL)), HWND);
    fail |= !__builtin_types_compatible_p(__typeof__(SetClipboardData(0, NULL)), HANDLE);
    fail |= !__builtin_types_compatible_p(__typeof__(PostQuitMessage(0)), void);
    fail |= !__builtin_types_compatible_p(__typeof__(GetSysColor(0)), DWORD);
    fail |= !__builtin_types_compatible_p(__typeof__(GetDeviceCaps(NULL, 0)), int);
    fail |= !__builtin_types_compatible_p(__typeof__(SelectObject(NULL, NULL)), HGDIOBJ);
    fail |= !__builtin_types_compatible_p(__typeof__(RegisterClassExW(NULL)), ATOM);
    fail |= !__builtin_types_compatible_p(__typeof__(CreateFontIndirectW(NULL)), HFONT);
    // Warm same API from separate decode layouts without touching LastError.
    SetLastError(0x12345678);
    GetSysColor(COLOR_WINDOW);
    fail |= GetLastError() != 0x12345678;
    GetSysColor(COLOR_WINDOW);
    GetSystemMetrics(SM_CXSCREEN);
    fail |= GetSysColor(GetSysColor(COLOR_WINDOW) & 0) != GetSysColor(0);
    ExitProcess(fail);
}
