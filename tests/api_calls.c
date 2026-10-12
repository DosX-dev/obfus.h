#include <stdio.h>
#include <string.h>
#include <windows.h>
extern int WINAPI MultiByteToWideChar(UINT, DWORD, LPCSTR, int, LPWSTR, int);
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
#include "../include/obfus.h"
#if NO_OBF != 1
static OBFH_GUI_SLOT test_gui_slots[OBFH_GUI_COUNT];
#endif
#undef if
#undef else
#undef for
#undef while
#undef break
#define CHECK(x) \
    do { \
        if (!(x)) { \
            fprintf(stderr, "API_FAIL:%d: %s\n", __LINE__, #x); \
            return 1; \
        } \
    } while (0)
#define ADDRESS(index, module, api) \
    do { \
        unsigned char bytes[64]; \
        size_t length = OBFH_GUI_NAME_##api(bytes); \
        SetLastError(0x12345678u); \
        ULONG_PTR v = obfh_gui_cold(module, &test_gui_slots[index], index, bytes, length); \
        CHECK(GetLastError() == 0x12345678u); \
        unsigned r = OBFH_GUI_ROTATE(index); \
        v -= OBFH_GUI_BIAS(index); \
        v = ((v >> r) | (v << (sizeof(ULONG_PTR) * 8 - r))) ^ OBFH_GUI_KEY(index); \
        CHECK(v == (ULONG_PTR)native_export(modules[module], #api)); \
    } while (0)
static DWORD WINAPI worker(void *event) { return SetEvent(event) ? 0 : 1; }
static int painted;
static LRESULT CALLBACK procedure(HWND window, UINT message, WPARAM w, LPARAM l) {
    if (message == WM_PAINT) {
        PAINTSTRUCT ps;
        HDC dc = BeginPaint(window, &ps);
        if (dc) ++painted;
        EndPaint(window, &ps);
        return 0;
    }
    return DefWindowProcW(window, message, w, l);
}
int main(void) {
#if NO_OBF != 1
    HMODULE modules[4] = {LoadLibraryA("user32.dll"), LoadLibraryA("gdi32.dll"), LoadLibraryA("kernel32.dll"), LoadLibraryA("advapi32.dll")};
    ADDRESS(87, 2, CreateFileA);
    ADDRESS(88, 2, CreateFileW);
    ADDRESS(89, 2, ReadFile);
    ADDRESS(90, 2, WriteFile);
    ADDRESS(91, 2, CloseHandle);
    ADDRESS(92, 2, GetFileSizeEx);
    ADDRESS(93, 2, SetFilePointerEx);
    ADDRESS(94, 2, VirtualAlloc);
    ADDRESS(95, 2, VirtualProtect);
    ADDRESS(96, 2, VirtualFree);
    ADDRESS(97, 2, GetProcessHeap);
    ADDRESS(98, 2, HeapAlloc);
    ADDRESS(99, 2, HeapReAlloc);
    ADDRESS(100, 2, HeapFree);
    ADDRESS(101, 2, GetModuleFileNameA);
    ADDRESS(102, 2, GetModuleFileNameW);
    ADDRESS(103, 2, GetEnvironmentVariableA);
    ADDRESS(104, 2, GetEnvironmentVariableW);
    ADDRESS(105, 2, GetCurrentDirectoryA);
    ADDRESS(106, 2, GetCurrentDirectoryW);
    ADDRESS(107, 2, GetTempPathA);
    ADDRESS(108, 2, GetTempPathW);
    ADDRESS(109, 2, CreateThread);
    ADDRESS(110, 2, WaitForSingleObject);
    ADDRESS(111, 2, WaitForMultipleObjects);
    ADDRESS(112, 2, CreateEventA);
    ADDRESS(113, 2, CreateEventW);
    ADDRESS(114, 2, SetEvent);
    ADDRESS(115, 2, ResetEvent);
    ADDRESS(116, 0, BeginPaint);
    ADDRESS(117, 0, EndPaint);
    ADDRESS(118, 0, DrawTextA);
    ADDRESS(119, 0, DrawTextW);
    ADDRESS(120, 1, TextOutA);
    ADDRESS(121, 1, TextOutW);
    ADDRESS(122, 1, BitBlt);
    ADDRESS(123, 1, CreateCompatibleDC);
    ADDRESS(124, 1, CreateCompatibleBitmap);
    ADDRESS(125, 1, GetStockObject);
    ADDRESS(126, 3, RegOpenKeyExA);
    ADDRESS(127, 3, RegOpenKeyExW);
    ADDRESS(128, 3, RegQueryValueExA);
    ADDRESS(129, 3, RegQueryValueExW);
    ADDRESS(130, 3, RegSetValueExA);
    ADDRESS(131, 3, RegSetValueExW);
    ADDRESS(132, 3, RegCloseKey);
    ADDRESS(133, 2, LoadLibraryW);
    ADDRESS(134, 2, LoadLibraryExA);
    ADDRESS(135, 2, LoadLibraryExW);
    for (int i = 0; i < 4; ++i) CHECK(FreeLibrary(modules[i]));
#endif
    char temp[MAX_PATH], path[MAX_PATH];
    wchar_t wide[MAX_PATH];
    CHECK(GetTempPathA(MAX_PATH, temp) > 0);
    CHECK(GetTempPathW(MAX_PATH, wide) > 0);
    CHECK(GetTempPath(MAX_PATH, (TCHAR *)wide) > 0);
    CHECK(GetTempFileNameA(temp, "obf", 0, path));
    CHECK(MultiByteToWideChar(0, 0, path, -1, wide, MAX_PATH) > 0);
    HANDLE file = CreateFileA(path, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_TEMPORARY, NULL);
    CHECK(file != INVALID_HANDLE_VALUE);
    const char payload[] = "cached file APIs";
    char buffer[64] = {0};
    DWORD count;
    CHECK(WriteFile(file, payload, sizeof payload, &count, NULL) && count == sizeof payload);
    LARGE_INTEGER size, zero;
    zero.QuadPart = 0;
    CHECK(GetFileSizeEx(file, &size) && size.QuadPart == sizeof payload);
    CHECK(SetFilePointerEx(file, zero, NULL, FILE_BEGIN));
    CHECK(ReadFile(file, buffer, sizeof buffer, &count, NULL) && count == sizeof payload && !strcmp(buffer, payload));
    CHECK(CloseHandle(file));
    file = CreateFileW(wide, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
    CHECK(file != INVALID_HANDLE_VALUE);
    CHECK(CloseHandle(file));
    CHECK(DeleteFileW(wide));
    void *page = VirtualAlloc(NULL, 4096, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    CHECK(page);
    ((char *)page)[0] = 42;
    DWORD old;
    CHECK(VirtualProtect(page, 4096, PAGE_READONLY, &old) && old == PAGE_READWRITE);
    CHECK(((char *)page)[0] == 42);
    CHECK(VirtualFree(page, 0, MEM_RELEASE));
    HANDLE heap = GetProcessHeap();
    CHECK(heap);
    unsigned char *block = HeapAlloc(heap, HEAP_ZERO_MEMORY, 64);
    CHECK(block);
    for (int i = 0; i < 64; ++i) CHECK(!block[i]);
    block[0] = 123;
    block = HeapReAlloc(heap, HEAP_ZERO_MEMORY, block, 128);
    CHECK(block && block[0] == 123);
    for (int i = 64; i < 128; ++i) CHECK(!block[i]);
    CHECK(HeapFree(heap, 0, block));
    CHECK(GetModuleFileNameA(NULL, path, MAX_PATH) > 0);
    CHECK(GetModuleFileNameW(NULL, wide, MAX_PATH) > 0);
    CHECK(GetModuleFileName(NULL, (TCHAR *)wide, MAX_PATH) > 0);
    CHECK(GetCurrentDirectoryA(MAX_PATH, path) > 0);
    CHECK(GetCurrentDirectoryW(MAX_PATH, wide) > 0);
    CHECK(GetCurrentDirectory(MAX_PATH, (TCHAR *)wide) > 0);
    CHECK(SetEnvironmentVariableA("OBFH_API_CHILD_TEST", "value"));
    CHECK(GetEnvironmentVariableA("OBFH_API_CHILD_TEST", buffer, sizeof buffer) == 5 && !strcmp(buffer, "value"));
    CHECK(GetEnvironmentVariableW(L"OBFH_API_CHILD_TEST", wide, MAX_PATH) == 5 && !wcscmp(wide, L"value"));
    CHECK(SetEnvironmentVariableA("OBFH_API_CHILD_TEST", NULL));
    HANDLE event = CreateEventA(NULL, TRUE, FALSE, NULL), other = CreateEventW(NULL, TRUE, FALSE, NULL);
    CHECK(event && other);
    CHECK(WaitForSingleObject(event, 0) == WAIT_TIMEOUT);
    CHECK(SetEvent(event) && WaitForSingleObject(event, 0) == WAIT_OBJECT_0);
    CHECK(ResetEvent(event));
    HANDLE thread = CreateThread(NULL, 0, worker, event, 0, NULL);
    CHECK(thread);
    HANDLE handles[2] = {event, thread};
    CHECK(WaitForMultipleObjects(2, handles, TRUE, 5000) == WAIT_OBJECT_0);
    DWORD code;
    CHECK(GetExitCodeThread(thread, &code) && !code);
    CHECK(CloseHandle(thread) && CloseHandle(event) && CloseHandle(other));
    HMODULE module = LoadLibraryW(L"kernel32.dll");
    CHECK(module && FreeLibrary(module));
    module = LoadLibraryExA("kernel32.dll", NULL, 0);
    CHECK(module && FreeLibrary(module));
    module = LoadLibraryExW(L"kernel32.dll", NULL, 0);
    CHECK(module && FreeLibrary(module));
    // Only this process's uniquely named HKCU test key is created/deleted.
    sprintf(path, "Software\\obfh-api-%lu", (unsigned long)GetCurrentProcessId());
    MultiByteToWideChar(0, 0, path, -1, wide, MAX_PATH);
    HKEY key;
    DWORD disposition;
    CHECK(RegCreateKeyExW(HKEY_CURRENT_USER, wide, 0, NULL, REG_OPTION_VOLATILE, KEY_READ | KEY_WRITE, NULL, &key, &disposition) == ERROR_SUCCESS);
    CHECK(disposition == REG_CREATED_NEW_KEY);
    DWORD number = 0x12345678u, type, length = sizeof number, read = 0;
    CHECK(RegSetValueExA(key, "Number", 0, REG_DWORD, (const BYTE *)&number, sizeof number) == ERROR_SUCCESS);
    CHECK(RegQueryValueExW(key, L"Number", NULL, &type, (BYTE *)&read, &length) == ERROR_SUCCESS && read == number && type == REG_DWORD);
    number = 77;
    CHECK(RegSetValueExW(key, L"Number", 0, REG_DWORD, (const BYTE *)&number, sizeof number) == ERROR_SUCCESS);
    length = sizeof read;
    CHECK(RegQueryValueExA(key, "Number", NULL, &type, (BYTE *)&read, &length) == ERROR_SUCCESS && read == 77);
    CHECK(RegCloseKey(key) == ERROR_SUCCESS);
    CHECK(RegOpenKeyExA(HKEY_CURRENT_USER, path, 0, KEY_READ, &key) == ERROR_SUCCESS);
    CHECK(RegCloseKey(key) == ERROR_SUCCESS);
    CHECK(RegOpenKeyExW(HKEY_CURRENT_USER, wide, 0, KEY_READ, &key) == ERROR_SUCCESS);
    CHECK(RegCloseKey(key) == ERROR_SUCCESS);
    CHECK(RegDeleteKeyW(HKEY_CURRENT_USER, wide) == ERROR_SUCCESS);
    HDC screen = GetDC(NULL), dc = CreateCompatibleDC(screen), dest = CreateCompatibleDC(screen);
    CHECK(screen && dc && dest);
    HBITMAP bitmap = CreateCompatibleBitmap(screen, 32, 32), copy = CreateCompatibleBitmap(screen, 32, 32);
    CHECK(bitmap && copy);
    HGDIOBJ old1 = SelectObject(dc, bitmap), old2 = SelectObject(dest, copy);
    CHECK(old1 && old2);
    RECT rect = {0, 0, 32, 32};
    char text[] = "test";
    wchar_t textw[] = L"test";
    CHECK(DrawTextA(dc, text, -1, &rect, DT_CALCRECT) > 0);
    CHECK(DrawTextW(dc, textw, -1, &rect, DT_CALCRECT) > 0);
    CHECK(TextOutA(dc, 0, 0, text, 4) && TextOutW(dc, 0, 16, textw, 4));
    rect.left = rect.top = 0;
    rect.right = rect.bottom = 32;
    CHECK(FillRect(dc, &rect, (HBRUSH)GetStockObject(BLACK_BRUSH)));
    CHECK(BitBlt(dest, 0, 0, 32, 32, dc, 0, 0, SRCCOPY));
    CHECK(GetPixel(dest, 1, 1) == RGB(0, 0, 0));
    SelectObject(dc, old1);
    SelectObject(dest, old2);
    CHECK(DeleteObject(bitmap) && DeleteObject(copy));
    CHECK(DeleteDC(dc) && DeleteDC(dest));
    CHECK(ReleaseDC(NULL, screen));
    WNDCLASSEXW cls = {0};
    cls.cbSize = sizeof cls;
    cls.lpfnWndProc = procedure;
    cls.hInstance = GetModuleHandleW(NULL);
    cls.lpszClassName = L"ObfhApiPaint";
    CHECK(RegisterClassExW(&cls));
    HWND window = CreateWindowExW(0, cls.lpszClassName, L"test", WS_OVERLAPPED, 0, 0, 32, 32, NULL, NULL, cls.hInstance, NULL);
    CHECK(window);
    CHECK(InvalidateRect(window, NULL, TRUE));
    SendMessageW(window, WM_PAINT, 0, 0);
    CHECK(painted == 1);
    CHECK(DestroyWindow(window));
    CHECK(UnregisterClassW(cls.lpszClassName, cls.hInstance));
    puts("API_CALLS_PASS");
    return 0;
}
