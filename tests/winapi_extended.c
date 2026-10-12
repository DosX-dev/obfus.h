#include <stdio.h>
#include <string.h>
#include <windows.h>
// Trusted OS images only: independent raw EAT reference, without AppHelp's
// GetProcAddress redirects. Behavioral references below call native APIs.
static FARPROC native_export(HMODULE module, const char *symbol) {
    unsigned char *base = (unsigned char *)module;
    IMAGE_NT_HEADERS *nt = (IMAGE_NT_HEADERS *)(base + ((IMAGE_DOS_HEADER *)base)->e_lfanew);
    IMAGE_DATA_DIRECTORY directory = nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];
    IMAGE_EXPORT_DIRECTORY *table = (IMAGE_EXPORT_DIRECTORY *)(base + directory.VirtualAddress);
    DWORD *names = (DWORD *)(base + table->AddressOfNames), *functions = (DWORD *)(base + table->AddressOfFunctions);
    WORD *ordinals = (WORD *)(base + table->AddressOfNameOrdinals);
    for (DWORD i = 0; i < table->NumberOfNames; ++i) {
        if (strcmp(symbol, (const char *)(base + names[i]))) continue;
        DWORD rva = functions[ordinals[i]];
        if (rva >= directory.VirtualAddress && rva - directory.VirtualAddress < directory.Size) {
            const char *forward = (const char *)(base + rva), *dot = strrchr(forward, '.');
            char library[MAX_PATH];
            size_t n = dot - forward;
            memcpy(library, forward, n);
            library[n] = 0;
            HMODULE target = GetModuleHandleA(library);
            if (!target) target = LoadLibraryA(library);
            return native_export(target, dot + 1);
        }
        return (FARPROC)((ULONG_PTR)module + (ULONG_PTR)rva);
    }
    return NULL;
}
static DWORD native_pid(void) { return GetCurrentProcessId(); }
static HANDLE native_process(void) { return GetCurrentProcess(); }
static HANDLE native_thread(void) { return GetCurrentThread(); }
#include "../include/obfus.h"
#if NO_OBF != 1
static OBFH_GUI_SLOT test_gui_slots[OBFH_GUI_COUNT];
#endif
#undef if
#undef else
#undef for
#undef while
#undef break
// TCC's bundled headers/import libraries omit these newer/NLS APIs.
#if NO_OBF == 1
#if defined(__x86_64__)
#define GetWindowLongPtrA(...) ((LONG_PTR(WINAPI *)(HWND, int))GetProcAddress(GetModuleHandleA("user32.dll"), "GetWindowLongPtrA"))(__VA_ARGS__)
#define GetWindowLongPtrW(...) ((LONG_PTR(WINAPI *)(HWND, int))GetProcAddress(GetModuleHandleA("user32.dll"), "GetWindowLongPtrW"))(__VA_ARGS__)
#define SetWindowLongPtrA(...) ((LONG_PTR(WINAPI *)(HWND, int, LONG_PTR))GetProcAddress(GetModuleHandleA("user32.dll"), "SetWindowLongPtrA"))(__VA_ARGS__)
#define SetWindowLongPtrW(...) ((LONG_PTR(WINAPI *)(HWND, int, LONG_PTR))GetProcAddress(GetModuleHandleA("user32.dll"), "SetWindowLongPtrW"))(__VA_ARGS__)
#endif
#define GetTickCount64(...) native_GetTickCount64(__VA_ARGS__)
#define MultiByteToWideChar(...) native_MultiByteToWideChar(__VA_ARGS__)
#define WideCharToMultiByte(...) native_WideCharToMultiByte(__VA_ARGS__)
#endif
#define CHECK(x) \
    do { \
        if (!(x)) { \
            fprintf(stderr, "EXTENDED_FAIL:%d: %s\n", __LINE__, #x); \
            return 1; \
        } \
    } while (0)
#define ADDRESS(module_id, api) \
    do { \
        unsigned char bytes[64]; \
        size_t length = OBFH_GUI_NAME_##api(bytes); \
        SetLastError(0x12345678u); \
        ULONG_PTR value = obfh_gui_cold(module_id, &test_gui_slots[OBFH_GUI_ID_##api], OBFH_GUI_ID_##api, bytes, length); \
        CHECK(GetLastError() == 0x12345678u); \
        unsigned r = OBFH_GUI_ROTATE(OBFH_GUI_ID_##api); \
        value -= OBFH_GUI_BIAS(OBFH_GUI_ID_##api); \
        value = ((value >> r) | (value << (sizeof(ULONG_PTR) * 8 - r))) ^ OBFH_GUI_KEY(OBFH_GUI_ID_##api); \
        CHECK(value == (ULONG_PTR)native_export(modules[module_id], #api)); \
    } while (0)
int main(void) {
#if NO_OBF == 1
    ULONGLONG(WINAPI * native_GetTickCount64)
    (void) = (void *)GetProcAddress(GetModuleHandleA("kernel32.dll"), "GetTickCount64");
    int(WINAPI * native_MultiByteToWideChar)(UINT, DWORD, LPCSTR, int, LPWSTR, int) = (void *)GetProcAddress(GetModuleHandleA("kernel32.dll"), "MultiByteToWideChar");
    int(WINAPI * native_WideCharToMultiByte)(UINT, DWORD, LPCWSTR, int, LPSTR, int, LPCSTR, LPBOOL) = (void *)GetProcAddress(GetModuleHandleA("kernel32.dll"), "WideCharToMultiByte");
#endif
#if NO_OBF != 1
    HMODULE modules[3] = {LoadLibraryA("user32.dll"), LoadLibraryA("gdi32.dll"), LoadLibraryA("kernel32.dll")};
    CHECK(modules[0] && modules[1] && modules[2]);
    ADDRESS(2, HeapDestroy);
    ADDRESS(2, GetFileSize);
    ADDRESS(2, SetFilePointer);
    ADDRESS(2, GetFileAttributesExA);
    ADDRESS(2, GetFileAttributesExW);
    ADDRESS(2, FindFirstFileExA);
    ADDRESS(2, FindFirstFileExW);
    ADDRESS(2, CreateDirectoryA);
    ADDRESS(2, CreateDirectoryW);
    ADDRESS(2, RemoveDirectoryA);
    ADDRESS(2, RemoveDirectoryW);
    ADDRESS(2, SetCurrentDirectoryA);
    ADDRESS(2, SetCurrentDirectoryW);
    ADDRESS(2, GetFullPathNameA);
    ADDRESS(2, GetFullPathNameW);
    ADDRESS(2, GetTempFileNameA);
    ADDRESS(2, GetTempFileNameW);
    ADDRESS(2, CreateFileMappingA);
    ADDRESS(2, CreateFileMappingW);
    ADDRESS(2, OpenFileMappingA);
    ADDRESS(2, OpenFileMappingW);
    ADDRESS(2, CreateProcessA);
    ADDRESS(2, CreateProcessW);
    ADDRESS(2, CreateMutexA);
    ADDRESS(2, CreateMutexW);
    ADDRESS(2, CreateSemaphoreA);
    ADDRESS(2, CreateSemaphoreW);
    ADDRESS(2, ReadConsoleA);
    ADDRESS(2, ReadConsoleW);
    ADDRESS(2, FlushFileBuffers);
    ADDRESS(2, GetFileTime);
    ADDRESS(2, SetFileTime);
    ADDRESS(2, GetFileInformationByHandle);
    ADDRESS(2, GetOverlappedResult);
    ADDRESS(2, CancelIo);
    ADDRESS(2, MapViewOfFile);
    ADDRESS(2, UnmapViewOfFile);
    ADDRESS(2, FlushViewOfFile);
    ADDRESS(2, OpenProcess);
    ADDRESS(2, TerminateProcess);
    ADDRESS(2, GetExitCodeProcess);
    ADDRESS(2, GetProcessTimes);
    ADDRESS(2, SleepEx);
    ADDRESS(2, ReleaseMutex);
    ADDRESS(2, ReleaseSemaphore);
    ADDRESS(2, InitializeCriticalSection);
    ADDRESS(2, DeleteCriticalSection);
    ADDRESS(2, EnterCriticalSection);
    ADDRESS(2, LeaveCriticalSection);
    ADDRESS(2, TryEnterCriticalSection);
    ADDRESS(2, AllocConsole);
    ADDRESS(2, FreeConsole);
    ADDRESS(2, GetConsoleWindow);
    ADDRESS(2, SetConsoleMode);
    ADDRESS(2, GetConsoleCP);
    ADDRESS(2, GetConsoleOutputCP);
    ADDRESS(2, SetConsoleCP);
    ADDRESS(2, SetConsoleOutputCP);
    ADDRESS(2, GetSystemInfo);
    ADDRESS(2, GetNativeSystemInfo);
    ADDRESS(2, GetSystemTimeAsFileTime);
    ADDRESS(0, CreateDialogParamA);
    ADDRESS(0, CreateDialogParamW);
    ADDRESS(0, DialogBoxParamA);
    ADDRESS(0, DialogBoxParamW);
    ADDRESS(0, GetDlgItemTextA);
    ADDRESS(0, GetDlgItemTextW);
    ADDRESS(0, SetDlgItemTextA);
    ADDRESS(0, SetDlgItemTextW);
    ADDRESS(0, PeekMessageA);
    ADDRESS(0, PeekMessageW);
    ADDRESS(0, EndDialog);
    ADDRESS(0, CheckDlgButton);
    ADDRESS(0, IsDlgButtonChecked);
    ADDRESS(0, SetMenu);
    ADDRESS(0, DestroyMenu);
    ADDRESS(0, InvalidateRect);
    ADDRESS(0, GetAsyncKeyState);
    ADDRESS(0, ScreenToClient);
    ADDRESS(0, ClientToScreen);
    ADDRESS(0, GetCursorPos);
    ADDRESS(0, GetWindow);
    ADDRESS(0, GetAncestor);
    ADDRESS(1, GetTextExtentPoint32A);
    ADDRESS(1, GetTextExtentPoint32W);
    ADDRESS(1, SetBkMode);
    ADDRESS(2, GetCurrentProcess);
    ADDRESS(2, Sleep);
    ADDRESS(2, HeapCreate);
    ADDRESS(2, SetConsoleTextAttribute);
    ADDRESS(2, GetCurrentProcessId);
    ADDRESS(2, GetCurrentThread);
    ADDRESS(2, GetExitCodeThread);
    ADDRESS(2, DuplicateHandle);
    ADDRESS(2, QueryPerformanceCounter);
    ADDRESS(2, QueryPerformanceFrequency);
    ADDRESS(2, GetTickCount);
    ADDRESS(2, GetTickCount64);
    ADDRESS(2, MultiByteToWideChar);
    ADDRESS(2, WideCharToMultiByte);
    ADDRESS(2, LoadResource);
    ADDRESS(2, LockResource);
    ADDRESS(2, SizeofResource);
    ADDRESS(2, FindClose);
    ADDRESS(0, GetDesktopWindow);
    ADDRESS(0, GetParent);
    ADDRESS(0, SetTimer);
    ADDRESS(0, KillTimer);
    ADDRESS(0, EnableWindow);
    ADDRESS(0, IsWindow);
    ADDRESS(2, GetFileAttributesA);
    ADDRESS(2, GetFileAttributesW);
    ADDRESS(2, FindFirstFileA);
    ADDRESS(2, FindFirstFileW);
    ADDRESS(2, FindNextFileA);
    ADDRESS(2, FindNextFileW);
    ADDRESS(2, DeleteFileA);
    ADDRESS(2, DeleteFileW);
    ADDRESS(2, CopyFileA);
    ADDRESS(2, CopyFileW);
    ADDRESS(2, MoveFileExA);
    ADDRESS(2, MoveFileExW);
    ADDRESS(2, FormatMessageA);
    ADDRESS(2, FormatMessageW);
    ADDRESS(2, FindResourceA);
    ADDRESS(2, FindResourceW);
    ADDRESS(0, PostMessageA);
    ADDRESS(0, PostMessageW);
    ADDRESS(0, GetWindowLongA);
    ADDRESS(0, GetWindowLongW);
    ADDRESS(0, SetWindowLongA);
    ADDRESS(0, SetWindowLongW);
    ADDRESS(0, LoadStringA);
    ADDRESS(0, LoadStringW);
#if defined(__x86_64__)
    ADDRESS(0, GetWindowLongPtrA);
#endif
#if defined(__x86_64__)
    ADDRESS(0, GetWindowLongPtrW);
#endif
#if defined(__x86_64__)
    ADDRESS(0, SetWindowLongPtrA);
#endif
#if defined(__x86_64__)
    ADDRESS(0, SetWindowLongPtrW);
#endif
#endif
    CHECK(GetCurrentProcessId() == native_pid());
    CHECK(GetCurrentProcess() == native_process());
    CHECK(GetCurrentThread() == native_thread());
    DWORD status = 0;
    CHECK(GetExitCodeThread(GetCurrentThread(), &status) && status == STILL_ACTIVE);
    HANDLE copy = NULL;
    CHECK(DuplicateHandle(GetCurrentProcess(), GetCurrentThread(), GetCurrentProcess(), &copy, 0, FALSE, DUPLICATE_SAME_ACCESS));
    CHECK(CloseHandle(copy));
    HANDLE heap = HeapCreate(0, 0, 0);
    CHECK(heap);
    void *p = HeapAlloc(heap, 0, 32);
    CHECK(p);
    CHECK(HeapFree(heap, 0, p));
    CHECK(HeapDestroy(heap));
    int effects = 0;
    Sleep(effects++);
    CHECK(effects == 1);
    LARGE_INTEGER frequency, before, after;
    CHECK(QueryPerformanceFrequency(&frequency) && frequency.QuadPart > 0);
    CHECK(QueryPerformanceCounter(&before));
    CHECK(QueryPerformanceCounter(&after) && after.QuadPart >= before.QuadPart);
    (void)GetTickCount();
    (void)GetTickCount64();
    wchar_t wide[32];
    char narrow[64];
    CHECK(MultiByteToWideChar(65001u, 0, "hello", -1, wide, 32) == 6);
    CHECK(WideCharToMultiByte(65001u, 0, wide, -1, narrow, 64, NULL, NULL) == 6);
    CHECK(!strcmp(narrow, "hello"));
    char text[256];
    wchar_t wtext[256];
    CHECK(FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, ERROR_FILE_NOT_FOUND, 0, text, 256, NULL) > 0);
    CHECK(FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, ERROR_FILE_NOT_FOUND, 0, wtext, 256, NULL) > 0);
    HWND window = CreateWindowExW(0, L"STATIC", L"extended", WS_POPUP, 0, 0, 20, 20, NULL, NULL, GetModuleHandleW(NULL), NULL);
    CHECK(window && IsWindow(window));
    CHECK(GetDesktopWindow() != NULL && GetParent(window) == NULL);
    CHECK(SetWindowLongA(window, (-21), 7) == 0);
    CHECK(GetWindowLongA(window, (-21)) == 7);
    CHECK(SetWindowLongW(window, (-21), 9) == 7);
    CHECK(GetWindowLongW(window, (-21)) == 9);
    LONG_PTR pointer = (LONG_PTR)&status;
    SetWindowLongPtrA(window, GWLP_USERDATA, pointer);
    CHECK(GetWindowLongPtrA(window, GWLP_USERDATA) == pointer);
    SetWindowLongPtrW(window, GWLP_USERDATA, pointer);
    CHECK(GetWindowLongPtrW(window, GWLP_USERDATA) == pointer);
    CHECK(GetWindowLongPtr(window, GWLP_USERDATA) == pointer);
    EnableWindow(window, FALSE);
    CHECK(!IsWindowEnabled(window));
    EnableWindow(window, TRUE);
    CHECK(IsWindowEnabled(window));
    CHECK(PostMessageA(window, WM_NULL, 0, 0));
    CHECK(PostMessageW(window, WM_NULL, 0, 0));
    UINT_PTR timer = SetTimer(window, 123, 1000, NULL);
    CHECK(timer && KillTimer(window, timer));
    HMODULE self = GetModuleHandleW(NULL);
    CHECK(LoadStringA(self, 65535, text, 256) == 0);
    CHECK(LoadStringW(self, 65535, wtext, 256) == 0);
    CHECK(!FindResourceA(self, "OBFH_MISSING", RT_RCDATA));
    CHECK(!FindResourceW(self, L"OBFH_MISSING", (LPCWSTR)RT_RCDATA));
    CHECK(!LoadResource(self, NULL));
    CHECK(SizeofResource(self, NULL) == 0);
    CHECK(LockResource(NULL) == NULL);
    DestroyWindow(window);
    SetLastError(0);
    CHECK(!SetConsoleTextAttribute(INVALID_HANDLE_VALUE, 7));
    CHECK(GetLastError() != 0);
    char temp[MAX_PATH], first[MAX_PATH], second[MAX_PATH], third[MAX_PATH];
    CHECK(GetTempPathA(MAX_PATH, temp));
    CHECK(GetTempFileNameA(temp, "ofh", 0, first));
    snprintf(second, sizeof second, "%s.copy", first);
    snprintf(third, sizeof third, "%s.move", first);
    CHECK(GetFileAttributesA(first) != INVALID_FILE_ATTRIBUTES);
    CHECK(CopyFileA(first, second, TRUE));
    CHECK(MoveFileExA(second, third, 0));
    WIN32_FIND_DATAA data;
    HANDLE find = FindFirstFileA(first, &data);
    CHECK(find != INVALID_HANDLE_VALUE);
    CHECK(!FindNextFileA(find, &data));
    CHECK(FindClose(find));
    CHECK(DeleteFileA(third));
    CHECK(DeleteFileA(first));
    wchar_t wf[MAX_PATH], ws[MAX_PATH], wt[MAX_PATH];
    CHECK(GetTempFileNameA(temp, "ofh", 0, first));
    snprintf(second, sizeof second, "%s.copy", first);
    snprintf(third, sizeof third, "%s.move", first);
    CHECK(MultiByteToWideChar(0u, 0, first, -1, wf, MAX_PATH));
    CHECK(MultiByteToWideChar(0u, 0, second, -1, ws, MAX_PATH));
    CHECK(MultiByteToWideChar(0u, 0, third, -1, wt, MAX_PATH));
    CHECK(GetFileAttributesW(wf) != INVALID_FILE_ATTRIBUTES);
    CHECK(CopyFileW(wf, ws, TRUE));
    CHECK(MoveFileExW(ws, wt, 0));
    WIN32_FIND_DATAW wd;
    find = FindFirstFileW(wf, &wd);
    CHECK(find != INVALID_HANDLE_VALUE);
    CHECK(!FindNextFileW(find, &wd));
    CHECK(FindClose(find));
    CHECK(DeleteFileW(wt));
    CHECK(DeleteFileW(wf));
    puts("WINAPI_EXTENDED_PASS");
    return 0;
}
