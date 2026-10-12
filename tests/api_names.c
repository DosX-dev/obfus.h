#define NO_CFLOW 1
#define NO_ANTIDEBUG 1
#include "../include/obfus.h"
#undef printf
int main(void) {
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_GetCurrentThreadId(b);
        if (len != sizeof("GetCurrentThreadId") || (memcmp)(b, "GetCurrentThreadId", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_GetCurrentProcess(b);
        if (len != sizeof("GetCurrentProcess") || (memcmp)(b, "GetCurrentProcess", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_Sleep(b);
        if (len != sizeof("Sleep") || (memcmp)(b, "Sleep", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_HeapCreate(b);
        if (len != sizeof("HeapCreate") || (memcmp)(b, "HeapCreate", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_SetConsoleTextAttribute(b);
        if (len != sizeof("SetConsoleTextAttribute") || (memcmp)(b, "SetConsoleTextAttribute", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_GetCurrentProcessId(b);
        if (len != sizeof("GetCurrentProcessId") || (memcmp)(b, "GetCurrentProcessId", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_GetCurrentThread(b);
        if (len != sizeof("GetCurrentThread") || (memcmp)(b, "GetCurrentThread", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_GetExitCodeThread(b);
        if (len != sizeof("GetExitCodeThread") || (memcmp)(b, "GetExitCodeThread", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_DuplicateHandle(b);
        if (len != sizeof("DuplicateHandle") || (memcmp)(b, "DuplicateHandle", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_QueryPerformanceCounter(b);
        if (len != sizeof("QueryPerformanceCounter") || (memcmp)(b, "QueryPerformanceCounter", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_QueryPerformanceFrequency(b);
        if (len != sizeof("QueryPerformanceFrequency") || (memcmp)(b, "QueryPerformanceFrequency", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_GetTickCount(b);
        if (len != sizeof("GetTickCount") || (memcmp)(b, "GetTickCount", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_GetTickCount64(b);
        if (len != sizeof("GetTickCount64") || (memcmp)(b, "GetTickCount64", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_MultiByteToWideChar(b);
        if (len != sizeof("MultiByteToWideChar") || (memcmp)(b, "MultiByteToWideChar", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_WideCharToMultiByte(b);
        if (len != sizeof("WideCharToMultiByte") || (memcmp)(b, "WideCharToMultiByte", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_LoadResource(b);
        if (len != sizeof("LoadResource") || (memcmp)(b, "LoadResource", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_LockResource(b);
        if (len != sizeof("LockResource") || (memcmp)(b, "LockResource", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_SizeofResource(b);
        if (len != sizeof("SizeofResource") || (memcmp)(b, "SizeofResource", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_FindClose(b);
        if (len != sizeof("FindClose") || (memcmp)(b, "FindClose", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_GetDesktopWindow(b);
        if (len != sizeof("GetDesktopWindow") || (memcmp)(b, "GetDesktopWindow", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_GetParent(b);
        if (len != sizeof("GetParent") || (memcmp)(b, "GetParent", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_SetTimer(b);
        if (len != sizeof("SetTimer") || (memcmp)(b, "SetTimer", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_KillTimer(b);
        if (len != sizeof("KillTimer") || (memcmp)(b, "KillTimer", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_EnableWindow(b);
        if (len != sizeof("EnableWindow") || (memcmp)(b, "EnableWindow", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_IsWindow(b);
        if (len != sizeof("IsWindow") || (memcmp)(b, "IsWindow", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_GetFileAttributesA(b);
        if (len != sizeof("GetFileAttributesA") || (memcmp)(b, "GetFileAttributesA", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_GetFileAttributesW(b);
        if (len != sizeof("GetFileAttributesW") || (memcmp)(b, "GetFileAttributesW", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_FindFirstFileA(b);
        if (len != sizeof("FindFirstFileA") || (memcmp)(b, "FindFirstFileA", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_FindFirstFileW(b);
        if (len != sizeof("FindFirstFileW") || (memcmp)(b, "FindFirstFileW", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_FindNextFileA(b);
        if (len != sizeof("FindNextFileA") || (memcmp)(b, "FindNextFileA", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_FindNextFileW(b);
        if (len != sizeof("FindNextFileW") || (memcmp)(b, "FindNextFileW", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_DeleteFileA(b);
        if (len != sizeof("DeleteFileA") || (memcmp)(b, "DeleteFileA", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_DeleteFileW(b);
        if (len != sizeof("DeleteFileW") || (memcmp)(b, "DeleteFileW", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_CopyFileA(b);
        if (len != sizeof("CopyFileA") || (memcmp)(b, "CopyFileA", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_CopyFileW(b);
        if (len != sizeof("CopyFileW") || (memcmp)(b, "CopyFileW", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_MoveFileExA(b);
        if (len != sizeof("MoveFileExA") || (memcmp)(b, "MoveFileExA", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_MoveFileExW(b);
        if (len != sizeof("MoveFileExW") || (memcmp)(b, "MoveFileExW", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_FormatMessageA(b);
        if (len != sizeof("FormatMessageA") || (memcmp)(b, "FormatMessageA", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_FormatMessageW(b);
        if (len != sizeof("FormatMessageW") || (memcmp)(b, "FormatMessageW", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_FindResourceA(b);
        if (len != sizeof("FindResourceA") || (memcmp)(b, "FindResourceA", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_FindResourceW(b);
        if (len != sizeof("FindResourceW") || (memcmp)(b, "FindResourceW", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_PostMessageA(b);
        if (len != sizeof("PostMessageA") || (memcmp)(b, "PostMessageA", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_PostMessageW(b);
        if (len != sizeof("PostMessageW") || (memcmp)(b, "PostMessageW", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_GetWindowLongA(b);
        if (len != sizeof("GetWindowLongA") || (memcmp)(b, "GetWindowLongA", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_GetWindowLongW(b);
        if (len != sizeof("GetWindowLongW") || (memcmp)(b, "GetWindowLongW", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_SetWindowLongA(b);
        if (len != sizeof("SetWindowLongA") || (memcmp)(b, "SetWindowLongA", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_SetWindowLongW(b);
        if (len != sizeof("SetWindowLongW") || (memcmp)(b, "SetWindowLongW", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_LoadStringA(b);
        if (len != sizeof("LoadStringA") || (memcmp)(b, "LoadStringA", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_LoadStringW(b);
        if (len != sizeof("LoadStringW") || (memcmp)(b, "LoadStringW", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
#if defined(__x86_64__)
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_GetWindowLongPtrA(b);
        if (len != sizeof("GetWindowLongPtrA") || (memcmp)(b, "GetWindowLongPtrA", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
#endif
#if defined(__x86_64__)
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_GetWindowLongPtrW(b);
        if (len != sizeof("GetWindowLongPtrW") || (memcmp)(b, "GetWindowLongPtrW", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
#endif
#if defined(__x86_64__)
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_SetWindowLongPtrA(b);
        if (len != sizeof("SetWindowLongPtrA") || (memcmp)(b, "SetWindowLongPtrA", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
#endif
#if defined(__x86_64__)
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_SetWindowLongPtrW(b);
        if (len != sizeof("SetWindowLongPtrW") || (memcmp)(b, "SetWindowLongPtrW", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
#endif
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_AppendMenuA(b);
        if (len != sizeof("AppendMenuA") || (memcmp)(b, "AppendMenuA", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_AppendMenuW(b);
        if (len != sizeof("AppendMenuW") || (memcmp)(b, "AppendMenuW", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_CheckMenuItem(b);
        if (len != sizeof("CheckMenuItem") || (memcmp)(b, "CheckMenuItem", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_CloseClipboard(b);
        if (len != sizeof("CloseClipboard") || (memcmp)(b, "CloseClipboard", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_CreateMenu(b);
        if (len != sizeof("CreateMenu") || (memcmp)(b, "CreateMenu", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_CreatePopupMenu(b);
        if (len != sizeof("CreatePopupMenu") || (memcmp)(b, "CreatePopupMenu", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_CreateWindowExA(b);
        if (len != sizeof("CreateWindowExA") || (memcmp)(b, "CreateWindowExA", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_CreateWindowExW(b);
        if (len != sizeof("CreateWindowExW") || (memcmp)(b, "CreateWindowExW", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_DefWindowProcA(b);
        if (len != sizeof("DefWindowProcA") || (memcmp)(b, "DefWindowProcA", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_DefWindowProcW(b);
        if (len != sizeof("DefWindowProcW") || (memcmp)(b, "DefWindowProcW", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_DestroyWindow(b);
        if (len != sizeof("DestroyWindow") || (memcmp)(b, "DestroyWindow", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_DispatchMessageA(b);
        if (len != sizeof("DispatchMessageA") || (memcmp)(b, "DispatchMessageA", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_DispatchMessageW(b);
        if (len != sizeof("DispatchMessageW") || (memcmp)(b, "DispatchMessageW", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_EmptyClipboard(b);
        if (len != sizeof("EmptyClipboard") || (memcmp)(b, "EmptyClipboard", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_GetClientRect(b);
        if (len != sizeof("GetClientRect") || (memcmp)(b, "GetClientRect", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_GetDC(b);
        if (len != sizeof("GetDC") || (memcmp)(b, "GetDC", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_GetDlgItem(b);
        if (len != sizeof("GetDlgItem") || (memcmp)(b, "GetDlgItem", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_GetKeyState(b);
        if (len != sizeof("GetKeyState") || (memcmp)(b, "GetKeyState", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_GetMenu(b);
        if (len != sizeof("GetMenu") || (memcmp)(b, "GetMenu", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_GetMessageA(b);
        if (len != sizeof("GetMessageA") || (memcmp)(b, "GetMessageA", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_GetMessageW(b);
        if (len != sizeof("GetMessageW") || (memcmp)(b, "GetMessageW", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_GetSysColor(b);
        if (len != sizeof("GetSysColor") || (memcmp)(b, "GetSysColor", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_GetSysColorBrush(b);
        if (len != sizeof("GetSysColorBrush") || (memcmp)(b, "GetSysColorBrush", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_GetSystemMetrics(b);
        if (len != sizeof("GetSystemMetrics") || (memcmp)(b, "GetSystemMetrics", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_GetWindowRect(b);
        if (len != sizeof("GetWindowRect") || (memcmp)(b, "GetWindowRect", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_GetWindowTextA(b);
        if (len != sizeof("GetWindowTextA") || (memcmp)(b, "GetWindowTextA", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_GetWindowTextW(b);
        if (len != sizeof("GetWindowTextW") || (memcmp)(b, "GetWindowTextW", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_GetWindowTextLengthA(b);
        if (len != sizeof("GetWindowTextLengthA") || (memcmp)(b, "GetWindowTextLengthA", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_GetWindowTextLengthW(b);
        if (len != sizeof("GetWindowTextLengthW") || (memcmp)(b, "GetWindowTextLengthW", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_IsDialogMessageA(b);
        if (len != sizeof("IsDialogMessageA") || (memcmp)(b, "IsDialogMessageA", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_IsDialogMessageW(b);
        if (len != sizeof("IsDialogMessageW") || (memcmp)(b, "IsDialogMessageW", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_LoadCursorA(b);
        if (len != sizeof("LoadCursorA") || (memcmp)(b, "LoadCursorA", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_LoadCursorW(b);
        if (len != sizeof("LoadCursorW") || (memcmp)(b, "LoadCursorW", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_LoadIconA(b);
        if (len != sizeof("LoadIconA") || (memcmp)(b, "LoadIconA", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_LoadIconW(b);
        if (len != sizeof("LoadIconW") || (memcmp)(b, "LoadIconW", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_MessageBeep(b);
        if (len != sizeof("MessageBeep") || (memcmp)(b, "MessageBeep", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_MessageBoxA(b);
        if (len != sizeof("MessageBoxA") || (memcmp)(b, "MessageBoxA", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_MessageBoxW(b);
        if (len != sizeof("MessageBoxW") || (memcmp)(b, "MessageBoxW", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_MoveWindow(b);
        if (len != sizeof("MoveWindow") || (memcmp)(b, "MoveWindow", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_OpenClipboard(b);
        if (len != sizeof("OpenClipboard") || (memcmp)(b, "OpenClipboard", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_PostQuitMessage(b);
        if (len != sizeof("PostQuitMessage") || (memcmp)(b, "PostQuitMessage", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_RegisterClassExA(b);
        if (len != sizeof("RegisterClassExA") || (memcmp)(b, "RegisterClassExA", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_RegisterClassExW(b);
        if (len != sizeof("RegisterClassExW") || (memcmp)(b, "RegisterClassExW", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_ReleaseDC(b);
        if (len != sizeof("ReleaseDC") || (memcmp)(b, "ReleaseDC", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_SendMessageA(b);
        if (len != sizeof("SendMessageA") || (memcmp)(b, "SendMessageA", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_SendMessageW(b);
        if (len != sizeof("SendMessageW") || (memcmp)(b, "SendMessageW", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_SetClipboardData(b);
        if (len != sizeof("SetClipboardData") || (memcmp)(b, "SetClipboardData", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_SetFocus(b);
        if (len != sizeof("SetFocus") || (memcmp)(b, "SetFocus", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_SetWindowPos(b);
        if (len != sizeof("SetWindowPos") || (memcmp)(b, "SetWindowPos", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_SetWindowTextA(b);
        if (len != sizeof("SetWindowTextA") || (memcmp)(b, "SetWindowTextA", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_SetWindowTextW(b);
        if (len != sizeof("SetWindowTextW") || (memcmp)(b, "SetWindowTextW", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_ShowWindow(b);
        if (len != sizeof("ShowWindow") || (memcmp)(b, "ShowWindow", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_TranslateMessage(b);
        if (len != sizeof("TranslateMessage") || (memcmp)(b, "TranslateMessage", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_UnregisterClassA(b);
        if (len != sizeof("UnregisterClassA") || (memcmp)(b, "UnregisterClassA", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_UnregisterClassW(b);
        if (len != sizeof("UnregisterClassW") || (memcmp)(b, "UnregisterClassW", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_UpdateWindow(b);
        if (len != sizeof("UpdateWindow") || (memcmp)(b, "UpdateWindow", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_CreateFontA(b);
        if (len != sizeof("CreateFontA") || (memcmp)(b, "CreateFontA", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_CreateFontW(b);
        if (len != sizeof("CreateFontW") || (memcmp)(b, "CreateFontW", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_CreateFontIndirectA(b);
        if (len != sizeof("CreateFontIndirectA") || (memcmp)(b, "CreateFontIndirectA", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_CreateFontIndirectW(b);
        if (len != sizeof("CreateFontIndirectW") || (memcmp)(b, "CreateFontIndirectW", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_DeleteObject(b);
        if (len != sizeof("DeleteObject") || (memcmp)(b, "DeleteObject", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_GetDeviceCaps(b);
        if (len != sizeof("GetDeviceCaps") || (memcmp)(b, "GetDeviceCaps", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_SelectObject(b);
        if (len != sizeof("SelectObject") || (memcmp)(b, "SelectObject", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_SetBkColor(b);
        if (len != sizeof("SetBkColor") || (memcmp)(b, "SetBkColor", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_SetTextColor(b);
        if (len != sizeof("SetTextColor") || (memcmp)(b, "SetTextColor", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_ExitProcess(b);
        if (len != sizeof("ExitProcess") || (memcmp)(b, "ExitProcess", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_GetLastError(b);
        if (len != sizeof("GetLastError") || (memcmp)(b, "GetLastError", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_FreeLibrary(b);
        if (len != sizeof("FreeLibrary") || (memcmp)(b, "FreeLibrary", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_SetLastError(b);
        if (len != sizeof("SetLastError") || (memcmp)(b, "SetLastError", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_WriteConsoleA(b);
        if (len != sizeof("WriteConsoleA") || (memcmp)(b, "WriteConsoleA", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_GetStdHandle(b);
        if (len != sizeof("GetStdHandle") || (memcmp)(b, "GetStdHandle", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_GetModuleHandleA(b);
        if (len != sizeof("GetModuleHandleA") || (memcmp)(b, "GetModuleHandleA", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_GetModuleHandleExA(b);
        if (len != sizeof("GetModuleHandleExA") || (memcmp)(b, "GetModuleHandleExA", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_VirtualQuery(b);
        if (len != sizeof("VirtualQuery") || (memcmp)(b, "VirtualQuery", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_GetConsoleMode(b);
        if (len != sizeof("GetConsoleMode") || (memcmp)(b, "GetConsoleMode", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_MulDiv(b);
        if (len != sizeof("MulDiv") || (memcmp)(b, "MulDiv", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_GlobalAlloc(b);
        if (len != sizeof("GlobalAlloc") || (memcmp)(b, "GlobalAlloc", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_GlobalLock(b);
        if (len != sizeof("GlobalLock") || (memcmp)(b, "GlobalLock", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_GlobalFree(b);
        if (len != sizeof("GlobalFree") || (memcmp)(b, "GlobalFree", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_GlobalUnlock(b);
        if (len != sizeof("GlobalUnlock") || (memcmp)(b, "GlobalUnlock", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_GetStartupInfoA(b);
        if (len != sizeof("GetStartupInfoA") || (memcmp)(b, "GetStartupInfoA", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_GetCommandLineA(b);
        if (len != sizeof("GetCommandLineA") || (memcmp)(b, "GetCommandLineA", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_WriteConsoleW(b);
        if (len != sizeof("WriteConsoleW") || (memcmp)(b, "WriteConsoleW", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_GetModuleHandleW(b);
        if (len != sizeof("GetModuleHandleW") || (memcmp)(b, "GetModuleHandleW", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_GetModuleHandleExW(b);
        if (len != sizeof("GetModuleHandleExW") || (memcmp)(b, "GetModuleHandleExW", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_GetStartupInfoW(b);
        if (len != sizeof("GetStartupInfoW") || (memcmp)(b, "GetStartupInfoW", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_GetCommandLineW(b);
        if (len != sizeof("GetCommandLineW") || (memcmp)(b, "GetCommandLineW", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_CreateFileA(b);
        if (len != sizeof("CreateFileA") || (memcmp)(b, "CreateFileA", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_CreateFileW(b);
        if (len != sizeof("CreateFileW") || (memcmp)(b, "CreateFileW", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_ReadFile(b);
        if (len != sizeof("ReadFile") || (memcmp)(b, "ReadFile", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_WriteFile(b);
        if (len != sizeof("WriteFile") || (memcmp)(b, "WriteFile", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_CloseHandle(b);
        if (len != sizeof("CloseHandle") || (memcmp)(b, "CloseHandle", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_GetFileSizeEx(b);
        if (len != sizeof("GetFileSizeEx") || (memcmp)(b, "GetFileSizeEx", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_SetFilePointerEx(b);
        if (len != sizeof("SetFilePointerEx") || (memcmp)(b, "SetFilePointerEx", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_VirtualAlloc(b);
        if (len != sizeof("VirtualAlloc") || (memcmp)(b, "VirtualAlloc", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_VirtualProtect(b);
        if (len != sizeof("VirtualProtect") || (memcmp)(b, "VirtualProtect", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_VirtualFree(b);
        if (len != sizeof("VirtualFree") || (memcmp)(b, "VirtualFree", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_GetProcessHeap(b);
        if (len != sizeof("GetProcessHeap") || (memcmp)(b, "GetProcessHeap", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_HeapAlloc(b);
        if (len != sizeof("HeapAlloc") || (memcmp)(b, "HeapAlloc", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_HeapReAlloc(b);
        if (len != sizeof("HeapReAlloc") || (memcmp)(b, "HeapReAlloc", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_HeapFree(b);
        if (len != sizeof("HeapFree") || (memcmp)(b, "HeapFree", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_GetModuleFileNameA(b);
        if (len != sizeof("GetModuleFileNameA") || (memcmp)(b, "GetModuleFileNameA", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_GetModuleFileNameW(b);
        if (len != sizeof("GetModuleFileNameW") || (memcmp)(b, "GetModuleFileNameW", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_GetEnvironmentVariableA(b);
        if (len != sizeof("GetEnvironmentVariableA") || (memcmp)(b, "GetEnvironmentVariableA", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_GetEnvironmentVariableW(b);
        if (len != sizeof("GetEnvironmentVariableW") || (memcmp)(b, "GetEnvironmentVariableW", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_GetCurrentDirectoryA(b);
        if (len != sizeof("GetCurrentDirectoryA") || (memcmp)(b, "GetCurrentDirectoryA", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_GetCurrentDirectoryW(b);
        if (len != sizeof("GetCurrentDirectoryW") || (memcmp)(b, "GetCurrentDirectoryW", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_GetTempPathA(b);
        if (len != sizeof("GetTempPathA") || (memcmp)(b, "GetTempPathA", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_GetTempPathW(b);
        if (len != sizeof("GetTempPathW") || (memcmp)(b, "GetTempPathW", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_CreateThread(b);
        if (len != sizeof("CreateThread") || (memcmp)(b, "CreateThread", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_WaitForSingleObject(b);
        if (len != sizeof("WaitForSingleObject") || (memcmp)(b, "WaitForSingleObject", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_WaitForMultipleObjects(b);
        if (len != sizeof("WaitForMultipleObjects") || (memcmp)(b, "WaitForMultipleObjects", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_CreateEventA(b);
        if (len != sizeof("CreateEventA") || (memcmp)(b, "CreateEventA", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_CreateEventW(b);
        if (len != sizeof("CreateEventW") || (memcmp)(b, "CreateEventW", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_SetEvent(b);
        if (len != sizeof("SetEvent") || (memcmp)(b, "SetEvent", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_ResetEvent(b);
        if (len != sizeof("ResetEvent") || (memcmp)(b, "ResetEvent", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_BeginPaint(b);
        if (len != sizeof("BeginPaint") || (memcmp)(b, "BeginPaint", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_EndPaint(b);
        if (len != sizeof("EndPaint") || (memcmp)(b, "EndPaint", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_DrawTextA(b);
        if (len != sizeof("DrawTextA") || (memcmp)(b, "DrawTextA", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_DrawTextW(b);
        if (len != sizeof("DrawTextW") || (memcmp)(b, "DrawTextW", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_TextOutA(b);
        if (len != sizeof("TextOutA") || (memcmp)(b, "TextOutA", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_TextOutW(b);
        if (len != sizeof("TextOutW") || (memcmp)(b, "TextOutW", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_BitBlt(b);
        if (len != sizeof("BitBlt") || (memcmp)(b, "BitBlt", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_CreateCompatibleDC(b);
        if (len != sizeof("CreateCompatibleDC") || (memcmp)(b, "CreateCompatibleDC", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_CreateCompatibleBitmap(b);
        if (len != sizeof("CreateCompatibleBitmap") || (memcmp)(b, "CreateCompatibleBitmap", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_GetStockObject(b);
        if (len != sizeof("GetStockObject") || (memcmp)(b, "GetStockObject", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_RegOpenKeyExA(b);
        if (len != sizeof("RegOpenKeyExA") || (memcmp)(b, "RegOpenKeyExA", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_RegOpenKeyExW(b);
        if (len != sizeof("RegOpenKeyExW") || (memcmp)(b, "RegOpenKeyExW", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_RegQueryValueExA(b);
        if (len != sizeof("RegQueryValueExA") || (memcmp)(b, "RegQueryValueExA", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_RegQueryValueExW(b);
        if (len != sizeof("RegQueryValueExW") || (memcmp)(b, "RegQueryValueExW", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_RegSetValueExA(b);
        if (len != sizeof("RegSetValueExA") || (memcmp)(b, "RegSetValueExA", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_RegSetValueExW(b);
        if (len != sizeof("RegSetValueExW") || (memcmp)(b, "RegSetValueExW", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_RegCloseKey(b);
        if (len != sizeof("RegCloseKey") || (memcmp)(b, "RegCloseKey", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_LoadLibraryW(b);
        if (len != sizeof("LoadLibraryW") || (memcmp)(b, "LoadLibraryW", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_LoadLibraryExA(b);
        if (len != sizeof("LoadLibraryExA") || (memcmp)(b, "LoadLibraryExA", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_LoadLibraryExW(b);
        if (len != sizeof("LoadLibraryExW") || (memcmp)(b, "LoadLibraryExW", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_CreateDirectoryA(b);
        if (len != sizeof("CreateDirectoryA") || (memcmp)(b, "CreateDirectoryA", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_CreateDirectoryW(b);
        if (len != sizeof("CreateDirectoryW") || (memcmp)(b, "CreateDirectoryW", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_RemoveDirectoryA(b);
        if (len != sizeof("RemoveDirectoryA") || (memcmp)(b, "RemoveDirectoryA", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_RemoveDirectoryW(b);
        if (len != sizeof("RemoveDirectoryW") || (memcmp)(b, "RemoveDirectoryW", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_SetCurrentDirectoryA(b);
        if (len != sizeof("SetCurrentDirectoryA") || (memcmp)(b, "SetCurrentDirectoryA", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_SetCurrentDirectoryW(b);
        if (len != sizeof("SetCurrentDirectoryW") || (memcmp)(b, "SetCurrentDirectoryW", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_GetFullPathNameA(b);
        if (len != sizeof("GetFullPathNameA") || (memcmp)(b, "GetFullPathNameA", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_GetFullPathNameW(b);
        if (len != sizeof("GetFullPathNameW") || (memcmp)(b, "GetFullPathNameW", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_GetTempFileNameA(b);
        if (len != sizeof("GetTempFileNameA") || (memcmp)(b, "GetTempFileNameA", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_GetTempFileNameW(b);
        if (len != sizeof("GetTempFileNameW") || (memcmp)(b, "GetTempFileNameW", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_CreateFileMappingA(b);
        if (len != sizeof("CreateFileMappingA") || (memcmp)(b, "CreateFileMappingA", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_CreateFileMappingW(b);
        if (len != sizeof("CreateFileMappingW") || (memcmp)(b, "CreateFileMappingW", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_OpenFileMappingA(b);
        if (len != sizeof("OpenFileMappingA") || (memcmp)(b, "OpenFileMappingA", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_OpenFileMappingW(b);
        if (len != sizeof("OpenFileMappingW") || (memcmp)(b, "OpenFileMappingW", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_CreateProcessA(b);
        if (len != sizeof("CreateProcessA") || (memcmp)(b, "CreateProcessA", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_CreateProcessW(b);
        if (len != sizeof("CreateProcessW") || (memcmp)(b, "CreateProcessW", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_CreateMutexA(b);
        if (len != sizeof("CreateMutexA") || (memcmp)(b, "CreateMutexA", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_CreateMutexW(b);
        if (len != sizeof("CreateMutexW") || (memcmp)(b, "CreateMutexW", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_CreateSemaphoreA(b);
        if (len != sizeof("CreateSemaphoreA") || (memcmp)(b, "CreateSemaphoreA", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_CreateSemaphoreW(b);
        if (len != sizeof("CreateSemaphoreW") || (memcmp)(b, "CreateSemaphoreW", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_ReadConsoleA(b);
        if (len != sizeof("ReadConsoleA") || (memcmp)(b, "ReadConsoleA", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_ReadConsoleW(b);
        if (len != sizeof("ReadConsoleW") || (memcmp)(b, "ReadConsoleW", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_FlushFileBuffers(b);
        if (len != sizeof("FlushFileBuffers") || (memcmp)(b, "FlushFileBuffers", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_GetFileTime(b);
        if (len != sizeof("GetFileTime") || (memcmp)(b, "GetFileTime", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_SetFileTime(b);
        if (len != sizeof("SetFileTime") || (memcmp)(b, "SetFileTime", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_GetFileInformationByHandle(b);
        if (len != sizeof("GetFileInformationByHandle") || (memcmp)(b, "GetFileInformationByHandle", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_GetOverlappedResult(b);
        if (len != sizeof("GetOverlappedResult") || (memcmp)(b, "GetOverlappedResult", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_CancelIo(b);
        if (len != sizeof("CancelIo") || (memcmp)(b, "CancelIo", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_MapViewOfFile(b);
        if (len != sizeof("MapViewOfFile") || (memcmp)(b, "MapViewOfFile", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_UnmapViewOfFile(b);
        if (len != sizeof("UnmapViewOfFile") || (memcmp)(b, "UnmapViewOfFile", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_FlushViewOfFile(b);
        if (len != sizeof("FlushViewOfFile") || (memcmp)(b, "FlushViewOfFile", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_OpenProcess(b);
        if (len != sizeof("OpenProcess") || (memcmp)(b, "OpenProcess", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_TerminateProcess(b);
        if (len != sizeof("TerminateProcess") || (memcmp)(b, "TerminateProcess", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_GetExitCodeProcess(b);
        if (len != sizeof("GetExitCodeProcess") || (memcmp)(b, "GetExitCodeProcess", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_GetProcessTimes(b);
        if (len != sizeof("GetProcessTimes") || (memcmp)(b, "GetProcessTimes", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_SleepEx(b);
        if (len != sizeof("SleepEx") || (memcmp)(b, "SleepEx", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_ReleaseMutex(b);
        if (len != sizeof("ReleaseMutex") || (memcmp)(b, "ReleaseMutex", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_ReleaseSemaphore(b);
        if (len != sizeof("ReleaseSemaphore") || (memcmp)(b, "ReleaseSemaphore", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_InitializeCriticalSection(b);
        if (len != sizeof("InitializeCriticalSection") || (memcmp)(b, "InitializeCriticalSection", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_DeleteCriticalSection(b);
        if (len != sizeof("DeleteCriticalSection") || (memcmp)(b, "DeleteCriticalSection", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_EnterCriticalSection(b);
        if (len != sizeof("EnterCriticalSection") || (memcmp)(b, "EnterCriticalSection", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_LeaveCriticalSection(b);
        if (len != sizeof("LeaveCriticalSection") || (memcmp)(b, "LeaveCriticalSection", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_TryEnterCriticalSection(b);
        if (len != sizeof("TryEnterCriticalSection") || (memcmp)(b, "TryEnterCriticalSection", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_AllocConsole(b);
        if (len != sizeof("AllocConsole") || (memcmp)(b, "AllocConsole", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_FreeConsole(b);
        if (len != sizeof("FreeConsole") || (memcmp)(b, "FreeConsole", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_GetConsoleWindow(b);
        if (len != sizeof("GetConsoleWindow") || (memcmp)(b, "GetConsoleWindow", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_SetConsoleMode(b);
        if (len != sizeof("SetConsoleMode") || (memcmp)(b, "SetConsoleMode", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_GetConsoleCP(b);
        if (len != sizeof("GetConsoleCP") || (memcmp)(b, "GetConsoleCP", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_GetConsoleOutputCP(b);
        if (len != sizeof("GetConsoleOutputCP") || (memcmp)(b, "GetConsoleOutputCP", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_SetConsoleCP(b);
        if (len != sizeof("SetConsoleCP") || (memcmp)(b, "SetConsoleCP", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_SetConsoleOutputCP(b);
        if (len != sizeof("SetConsoleOutputCP") || (memcmp)(b, "SetConsoleOutputCP", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_GetSystemInfo(b);
        if (len != sizeof("GetSystemInfo") || (memcmp)(b, "GetSystemInfo", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_GetNativeSystemInfo(b);
        if (len != sizeof("GetNativeSystemInfo") || (memcmp)(b, "GetNativeSystemInfo", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_GetSystemTimeAsFileTime(b);
        if (len != sizeof("GetSystemTimeAsFileTime") || (memcmp)(b, "GetSystemTimeAsFileTime", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_CreateDialogParamA(b);
        if (len != sizeof("CreateDialogParamA") || (memcmp)(b, "CreateDialogParamA", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_CreateDialogParamW(b);
        if (len != sizeof("CreateDialogParamW") || (memcmp)(b, "CreateDialogParamW", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_DialogBoxParamA(b);
        if (len != sizeof("DialogBoxParamA") || (memcmp)(b, "DialogBoxParamA", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_DialogBoxParamW(b);
        if (len != sizeof("DialogBoxParamW") || (memcmp)(b, "DialogBoxParamW", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_GetDlgItemTextA(b);
        if (len != sizeof("GetDlgItemTextA") || (memcmp)(b, "GetDlgItemTextA", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_GetDlgItemTextW(b);
        if (len != sizeof("GetDlgItemTextW") || (memcmp)(b, "GetDlgItemTextW", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_SetDlgItemTextA(b);
        if (len != sizeof("SetDlgItemTextA") || (memcmp)(b, "SetDlgItemTextA", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_SetDlgItemTextW(b);
        if (len != sizeof("SetDlgItemTextW") || (memcmp)(b, "SetDlgItemTextW", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_PeekMessageA(b);
        if (len != sizeof("PeekMessageA") || (memcmp)(b, "PeekMessageA", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_PeekMessageW(b);
        if (len != sizeof("PeekMessageW") || (memcmp)(b, "PeekMessageW", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_EndDialog(b);
        if (len != sizeof("EndDialog") || (memcmp)(b, "EndDialog", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_CheckDlgButton(b);
        if (len != sizeof("CheckDlgButton") || (memcmp)(b, "CheckDlgButton", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_IsDlgButtonChecked(b);
        if (len != sizeof("IsDlgButtonChecked") || (memcmp)(b, "IsDlgButtonChecked", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_SetMenu(b);
        if (len != sizeof("SetMenu") || (memcmp)(b, "SetMenu", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_DestroyMenu(b);
        if (len != sizeof("DestroyMenu") || (memcmp)(b, "DestroyMenu", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_InvalidateRect(b);
        if (len != sizeof("InvalidateRect") || (memcmp)(b, "InvalidateRect", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_GetAsyncKeyState(b);
        if (len != sizeof("GetAsyncKeyState") || (memcmp)(b, "GetAsyncKeyState", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_ScreenToClient(b);
        if (len != sizeof("ScreenToClient") || (memcmp)(b, "ScreenToClient", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_ClientToScreen(b);
        if (len != sizeof("ClientToScreen") || (memcmp)(b, "ClientToScreen", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_GetCursorPos(b);
        if (len != sizeof("GetCursorPos") || (memcmp)(b, "GetCursorPos", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_GetWindow(b);
        if (len != sizeof("GetWindow") || (memcmp)(b, "GetWindow", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_GetAncestor(b);
        if (len != sizeof("GetAncestor") || (memcmp)(b, "GetAncestor", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_GetTextExtentPoint32A(b);
        if (len != sizeof("GetTextExtentPoint32A") || (memcmp)(b, "GetTextExtentPoint32A", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_GetTextExtentPoint32W(b);
        if (len != sizeof("GetTextExtentPoint32W") || (memcmp)(b, "GetTextExtentPoint32W", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_SetBkMode(b);
        if (len != sizeof("SetBkMode") || (memcmp)(b, "SetBkMode", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_HeapDestroy(b);
        if (len != sizeof("HeapDestroy") || (memcmp)(b, "HeapDestroy", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_GetFileSize(b);
        if (len != sizeof("GetFileSize") || (memcmp)(b, "GetFileSize", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_SetFilePointer(b);
        if (len != sizeof("SetFilePointer") || (memcmp)(b, "SetFilePointer", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_GetFileAttributesExA(b);
        if (len != sizeof("GetFileAttributesExA") || (memcmp)(b, "GetFileAttributesExA", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_GetFileAttributesExW(b);
        if (len != sizeof("GetFileAttributesExW") || (memcmp)(b, "GetFileAttributesExW", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_FindFirstFileExA(b);
        if (len != sizeof("FindFirstFileExA") || (memcmp)(b, "FindFirstFileExA", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    {
        unsigned char b[68];
        for (unsigned int i = 0; i < 68; i++) b[i] = 0xA5;
        size_t len = OBFH_GUI_NAME_FindFirstFileExW(b);
        if (len != sizeof("FindFirstFileExW") || (memcmp)(b, "FindFirstFileExW", len)) return 1;
        for (unsigned int i = (len + 3) & ~3u; i < 68; i++)
            if (b[i] != 0xA5) return 2;
    }
    printf("NAMES_PASS\n");
    return 0;
}