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
static DWORD native_color(int n) { return GetSysColor(n); }
static int native_metrics(int n) { return GetSystemMetrics(n); }
static int native_caps(HDC d, int n) { return GetDeviceCaps(d, n); }
static BOOL native_move(HWND h, HWND a, int x, int y, int cx, int cy, UINT f) { return SetWindowPos(h, a, x, y, cx, cy, f); }
#include "../include/obfus.h"
#if NO_OBF != 1
static OBFH_GUI_SLOT test_gui_slots[OBFH_GUI_COUNT];
#endif
#define CHECK(x)                                                \
    do {                                                        \
        if (!(x)) {                                             \
            fprintf(stderr, "GUI_FAIL:%d: %s\n", __LINE__, #x); \
            return 1;                                           \
        }                                                       \
    } while (0)
static int creates, callbacks;
static LRESULT CALLBACK procedure(HWND h, UINT m, WPARAM w, LPARAM l) {
    if (m == WM_CREATE) {
        ++creates;
        return GetSysColor(COLOR_WINDOW) != 0 ? 0 : 0;
    }
    if (m == WM_APP + 1) {
        ++callbacks;
        if (w) return SendMessageW(h, WM_APP + 1, 0, 0) + 1;
        return 17;
    }
    return DefWindowProcW(h, m, w, l);
}
static DWORD WINAPI race(void *argument) {
    HANDLE start = (HANDLE)argument;
    WaitForSingleObject(start, INFINITE);
    native_metrics(SM_CXSCREEN);
    for (int i = 0; i < 100; ++i) {
        SetLastError(0x12340000u + i);
        int a = GetSystemMetrics(SM_CXSCREEN);
        DWORD error = GetLastError();
        SetLastError(0x12340000u + i);
        int b = native_metrics(SM_CXSCREEN);
        DWORD expected = GetLastError();
        if (a != b || error != expected) return 1;
    }
    return 0;
}
int main(int argc, char **argv) {
#if NO_OBF != 1
    if (argc > 1 && argv[1][0] == 'x') {
        const unsigned char bad[] = {1};
        obfh_gui_cold(2, NULL, 0, bad, 1);
        return 99;
    }
#endif
    // First-use races precede address checks; each thread has its own LastError.
    HANDLE start = CreateEventA(NULL, TRUE, FALSE, NULL), threads[8];
    CHECK(start != NULL);
    for (int i = 0; i < 8; ++i) {
        threads[i] = CreateThread(NULL, 0, race, start, 0, NULL);
        CHECK(threads[i] != NULL);
    }
    SetEvent(start);
    CHECK(WaitForMultipleObjects(8, threads, TRUE, 10000) == WAIT_OBJECT_0);
    for (int i = 0; i < 8; ++i) {
        DWORD result;
        CHECK(GetExitCodeThread(threads[i], &result) && !result);
        CloseHandle(threads[i]);
    }
    CloseHandle(start);
#if NO_OBF != 1
    HMODULE user = LoadLibraryA("user32.dll"), gdi = LoadLibraryA("gdi32.dll");
    CHECK(user && gdi);
    ULONG_PTR encoded, decoded;
    unsigned int rotate;
    {
        unsigned char bytes[64];
        size_t length = OBFH_GUI_NAME_AppendMenuA(bytes);
        SetLastError(0x12345678u);
        encoded = obfh_gui_cold(0, &test_gui_slots[0], 0, bytes, length);
        CHECK(GetLastError() == 0x12345678u);
        rotate = OBFH_GUI_ROTATE(0);
        decoded = encoded - OBFH_GUI_BIAS(0);
        decoded = ((decoded >> rotate) | (decoded << (sizeof(ULONG_PTR) * 8 - rotate))) ^ OBFH_GUI_KEY(0);
        CHECK(decoded == (ULONG_PTR)native_export(user, "AppendMenuA"));
        CHECK(test_gui_slots[0].ready);
    }
    {
        unsigned char bytes[64];
        size_t length = OBFH_GUI_NAME_AppendMenuW(bytes);
        SetLastError(0x12345678u);
        encoded = obfh_gui_cold(0, &test_gui_slots[1], 1, bytes, length);
        CHECK(GetLastError() == 0x12345678u);
        rotate = OBFH_GUI_ROTATE(1);
        decoded = encoded - OBFH_GUI_BIAS(1);
        decoded = ((decoded >> rotate) | (decoded << (sizeof(ULONG_PTR) * 8 - rotate))) ^ OBFH_GUI_KEY(1);
        CHECK(decoded == (ULONG_PTR)native_export(user, "AppendMenuW"));
        CHECK(test_gui_slots[1].ready);
    }
    {
        unsigned char bytes[64];
        size_t length = OBFH_GUI_NAME_CheckMenuItem(bytes);
        SetLastError(0x12345678u);
        encoded = obfh_gui_cold(0, &test_gui_slots[2], 2, bytes, length);
        CHECK(GetLastError() == 0x12345678u);
        rotate = OBFH_GUI_ROTATE(2);
        decoded = encoded - OBFH_GUI_BIAS(2);
        decoded = ((decoded >> rotate) | (decoded << (sizeof(ULONG_PTR) * 8 - rotate))) ^ OBFH_GUI_KEY(2);
        CHECK(decoded == (ULONG_PTR)native_export(user, "CheckMenuItem"));
        CHECK(test_gui_slots[2].ready);
    }
    {
        unsigned char bytes[64];
        size_t length = OBFH_GUI_NAME_CloseClipboard(bytes);
        SetLastError(0x12345678u);
        encoded = obfh_gui_cold(0, &test_gui_slots[3], 3, bytes, length);
        CHECK(GetLastError() == 0x12345678u);
        rotate = OBFH_GUI_ROTATE(3);
        decoded = encoded - OBFH_GUI_BIAS(3);
        decoded = ((decoded >> rotate) | (decoded << (sizeof(ULONG_PTR) * 8 - rotate))) ^ OBFH_GUI_KEY(3);
        CHECK(decoded == (ULONG_PTR)native_export(user, "CloseClipboard"));
        CHECK(test_gui_slots[3].ready);
    }
    {
        unsigned char bytes[64];
        size_t length = OBFH_GUI_NAME_CreateMenu(bytes);
        SetLastError(0x12345678u);
        encoded = obfh_gui_cold(0, &test_gui_slots[4], 4, bytes, length);
        CHECK(GetLastError() == 0x12345678u);
        rotate = OBFH_GUI_ROTATE(4);
        decoded = encoded - OBFH_GUI_BIAS(4);
        decoded = ((decoded >> rotate) | (decoded << (sizeof(ULONG_PTR) * 8 - rotate))) ^ OBFH_GUI_KEY(4);
        CHECK(decoded == (ULONG_PTR)native_export(user, "CreateMenu"));
        CHECK(test_gui_slots[4].ready);
    }
    {
        unsigned char bytes[64];
        size_t length = OBFH_GUI_NAME_CreatePopupMenu(bytes);
        SetLastError(0x12345678u);
        encoded = obfh_gui_cold(0, &test_gui_slots[5], 5, bytes, length);
        CHECK(GetLastError() == 0x12345678u);
        rotate = OBFH_GUI_ROTATE(5);
        decoded = encoded - OBFH_GUI_BIAS(5);
        decoded = ((decoded >> rotate) | (decoded << (sizeof(ULONG_PTR) * 8 - rotate))) ^ OBFH_GUI_KEY(5);
        CHECK(decoded == (ULONG_PTR)native_export(user, "CreatePopupMenu"));
        CHECK(test_gui_slots[5].ready);
    }
    {
        unsigned char bytes[64];
        size_t length = OBFH_GUI_NAME_CreateWindowExA(bytes);
        SetLastError(0x12345678u);
        encoded = obfh_gui_cold(0, &test_gui_slots[6], 6, bytes, length);
        CHECK(GetLastError() == 0x12345678u);
        rotate = OBFH_GUI_ROTATE(6);
        decoded = encoded - OBFH_GUI_BIAS(6);
        decoded = ((decoded >> rotate) | (decoded << (sizeof(ULONG_PTR) * 8 - rotate))) ^ OBFH_GUI_KEY(6);
        CHECK(decoded == (ULONG_PTR)native_export(user, "CreateWindowExA"));
        CHECK(test_gui_slots[6].ready);
    }
    {
        unsigned char bytes[64];
        size_t length = OBFH_GUI_NAME_CreateWindowExW(bytes);
        SetLastError(0x12345678u);
        encoded = obfh_gui_cold(0, &test_gui_slots[7], 7, bytes, length);
        CHECK(GetLastError() == 0x12345678u);
        rotate = OBFH_GUI_ROTATE(7);
        decoded = encoded - OBFH_GUI_BIAS(7);
        decoded = ((decoded >> rotate) | (decoded << (sizeof(ULONG_PTR) * 8 - rotate))) ^ OBFH_GUI_KEY(7);
        CHECK(decoded == (ULONG_PTR)native_export(user, "CreateWindowExW"));
        CHECK(test_gui_slots[7].ready);
    }
    {
        unsigned char bytes[64];
        size_t length = OBFH_GUI_NAME_DefWindowProcA(bytes);
        SetLastError(0x12345678u);
        encoded = obfh_gui_cold(0, &test_gui_slots[8], 8, bytes, length);
        CHECK(GetLastError() == 0x12345678u);
        rotate = OBFH_GUI_ROTATE(8);
        decoded = encoded - OBFH_GUI_BIAS(8);
        decoded = ((decoded >> rotate) | (decoded << (sizeof(ULONG_PTR) * 8 - rotate))) ^ OBFH_GUI_KEY(8);
        CHECK(decoded == (ULONG_PTR)native_export(user, "DefWindowProcA"));
        CHECK(test_gui_slots[8].ready);
    }
    {
        unsigned char bytes[64];
        size_t length = OBFH_GUI_NAME_DefWindowProcW(bytes);
        SetLastError(0x12345678u);
        encoded = obfh_gui_cold(0, &test_gui_slots[9], 9, bytes, length);
        CHECK(GetLastError() == 0x12345678u);
        rotate = OBFH_GUI_ROTATE(9);
        decoded = encoded - OBFH_GUI_BIAS(9);
        decoded = ((decoded >> rotate) | (decoded << (sizeof(ULONG_PTR) * 8 - rotate))) ^ OBFH_GUI_KEY(9);
        CHECK(decoded == (ULONG_PTR)native_export(user, "DefWindowProcW"));
        CHECK(test_gui_slots[9].ready);
    }
    {
        unsigned char bytes[64];
        size_t length = OBFH_GUI_NAME_DestroyWindow(bytes);
        SetLastError(0x12345678u);
        encoded = obfh_gui_cold(0, &test_gui_slots[10], 10, bytes, length);
        CHECK(GetLastError() == 0x12345678u);
        rotate = OBFH_GUI_ROTATE(10);
        decoded = encoded - OBFH_GUI_BIAS(10);
        decoded = ((decoded >> rotate) | (decoded << (sizeof(ULONG_PTR) * 8 - rotate))) ^ OBFH_GUI_KEY(10);
        CHECK(decoded == (ULONG_PTR)native_export(user, "DestroyWindow"));
        CHECK(test_gui_slots[10].ready);
    }
    {
        unsigned char bytes[64];
        size_t length = OBFH_GUI_NAME_DispatchMessageA(bytes);
        SetLastError(0x12345678u);
        encoded = obfh_gui_cold(0, &test_gui_slots[11], 11, bytes, length);
        CHECK(GetLastError() == 0x12345678u);
        rotate = OBFH_GUI_ROTATE(11);
        decoded = encoded - OBFH_GUI_BIAS(11);
        decoded = ((decoded >> rotate) | (decoded << (sizeof(ULONG_PTR) * 8 - rotate))) ^ OBFH_GUI_KEY(11);
        CHECK(decoded == (ULONG_PTR)native_export(user, "DispatchMessageA"));
        CHECK(test_gui_slots[11].ready);
    }
    {
        unsigned char bytes[64];
        size_t length = OBFH_GUI_NAME_DispatchMessageW(bytes);
        SetLastError(0x12345678u);
        encoded = obfh_gui_cold(0, &test_gui_slots[12], 12, bytes, length);
        CHECK(GetLastError() == 0x12345678u);
        rotate = OBFH_GUI_ROTATE(12);
        decoded = encoded - OBFH_GUI_BIAS(12);
        decoded = ((decoded >> rotate) | (decoded << (sizeof(ULONG_PTR) * 8 - rotate))) ^ OBFH_GUI_KEY(12);
        CHECK(decoded == (ULONG_PTR)native_export(user, "DispatchMessageW"));
        CHECK(test_gui_slots[12].ready);
    }
    {
        unsigned char bytes[64];
        size_t length = OBFH_GUI_NAME_EmptyClipboard(bytes);
        SetLastError(0x12345678u);
        encoded = obfh_gui_cold(0, &test_gui_slots[13], 13, bytes, length);
        CHECK(GetLastError() == 0x12345678u);
        rotate = OBFH_GUI_ROTATE(13);
        decoded = encoded - OBFH_GUI_BIAS(13);
        decoded = ((decoded >> rotate) | (decoded << (sizeof(ULONG_PTR) * 8 - rotate))) ^ OBFH_GUI_KEY(13);
        CHECK(decoded == (ULONG_PTR)native_export(user, "EmptyClipboard"));
        CHECK(test_gui_slots[13].ready);
    }
    {
        unsigned char bytes[64];
        size_t length = OBFH_GUI_NAME_GetClientRect(bytes);
        SetLastError(0x12345678u);
        encoded = obfh_gui_cold(0, &test_gui_slots[14], 14, bytes, length);
        CHECK(GetLastError() == 0x12345678u);
        rotate = OBFH_GUI_ROTATE(14);
        decoded = encoded - OBFH_GUI_BIAS(14);
        decoded = ((decoded >> rotate) | (decoded << (sizeof(ULONG_PTR) * 8 - rotate))) ^ OBFH_GUI_KEY(14);
        CHECK(decoded == (ULONG_PTR)native_export(user, "GetClientRect"));
        CHECK(test_gui_slots[14].ready);
    }
    {
        unsigned char bytes[64];
        size_t length = OBFH_GUI_NAME_GetDC(bytes);
        SetLastError(0x12345678u);
        encoded = obfh_gui_cold(0, &test_gui_slots[15], 15, bytes, length);
        CHECK(GetLastError() == 0x12345678u);
        rotate = OBFH_GUI_ROTATE(15);
        decoded = encoded - OBFH_GUI_BIAS(15);
        decoded = ((decoded >> rotate) | (decoded << (sizeof(ULONG_PTR) * 8 - rotate))) ^ OBFH_GUI_KEY(15);
        CHECK(decoded == (ULONG_PTR)native_export(user, "GetDC"));
        CHECK(test_gui_slots[15].ready);
    }
    {
        unsigned char bytes[64];
        size_t length = OBFH_GUI_NAME_GetDlgItem(bytes);
        SetLastError(0x12345678u);
        encoded = obfh_gui_cold(0, &test_gui_slots[16], 16, bytes, length);
        CHECK(GetLastError() == 0x12345678u);
        rotate = OBFH_GUI_ROTATE(16);
        decoded = encoded - OBFH_GUI_BIAS(16);
        decoded = ((decoded >> rotate) | (decoded << (sizeof(ULONG_PTR) * 8 - rotate))) ^ OBFH_GUI_KEY(16);
        CHECK(decoded == (ULONG_PTR)native_export(user, "GetDlgItem"));
        CHECK(test_gui_slots[16].ready);
    }
    {
        unsigned char bytes[64];
        size_t length = OBFH_GUI_NAME_GetKeyState(bytes);
        SetLastError(0x12345678u);
        encoded = obfh_gui_cold(0, &test_gui_slots[17], 17, bytes, length);
        CHECK(GetLastError() == 0x12345678u);
        rotate = OBFH_GUI_ROTATE(17);
        decoded = encoded - OBFH_GUI_BIAS(17);
        decoded = ((decoded >> rotate) | (decoded << (sizeof(ULONG_PTR) * 8 - rotate))) ^ OBFH_GUI_KEY(17);
        CHECK(decoded == (ULONG_PTR)native_export(user, "GetKeyState"));
        CHECK(test_gui_slots[17].ready);
    }
    {
        unsigned char bytes[64];
        size_t length = OBFH_GUI_NAME_GetMenu(bytes);
        SetLastError(0x12345678u);
        encoded = obfh_gui_cold(0, &test_gui_slots[18], 18, bytes, length);
        CHECK(GetLastError() == 0x12345678u);
        rotate = OBFH_GUI_ROTATE(18);
        decoded = encoded - OBFH_GUI_BIAS(18);
        decoded = ((decoded >> rotate) | (decoded << (sizeof(ULONG_PTR) * 8 - rotate))) ^ OBFH_GUI_KEY(18);
        CHECK(decoded == (ULONG_PTR)native_export(user, "GetMenu"));
        CHECK(test_gui_slots[18].ready);
    }
    {
        unsigned char bytes[64];
        size_t length = OBFH_GUI_NAME_GetMessageA(bytes);
        SetLastError(0x12345678u);
        encoded = obfh_gui_cold(0, &test_gui_slots[19], 19, bytes, length);
        CHECK(GetLastError() == 0x12345678u);
        rotate = OBFH_GUI_ROTATE(19);
        decoded = encoded - OBFH_GUI_BIAS(19);
        decoded = ((decoded >> rotate) | (decoded << (sizeof(ULONG_PTR) * 8 - rotate))) ^ OBFH_GUI_KEY(19);
        CHECK(decoded == (ULONG_PTR)native_export(user, "GetMessageA"));
        CHECK(test_gui_slots[19].ready);
    }
    {
        unsigned char bytes[64];
        size_t length = OBFH_GUI_NAME_GetMessageW(bytes);
        SetLastError(0x12345678u);
        encoded = obfh_gui_cold(0, &test_gui_slots[20], 20, bytes, length);
        CHECK(GetLastError() == 0x12345678u);
        rotate = OBFH_GUI_ROTATE(20);
        decoded = encoded - OBFH_GUI_BIAS(20);
        decoded = ((decoded >> rotate) | (decoded << (sizeof(ULONG_PTR) * 8 - rotate))) ^ OBFH_GUI_KEY(20);
        CHECK(decoded == (ULONG_PTR)native_export(user, "GetMessageW"));
        CHECK(test_gui_slots[20].ready);
    }
    {
        unsigned char bytes[64];
        size_t length = OBFH_GUI_NAME_GetSysColor(bytes);
        SetLastError(0x12345678u);
        encoded = obfh_gui_cold(0, &test_gui_slots[21], 21, bytes, length);
        CHECK(GetLastError() == 0x12345678u);
        rotate = OBFH_GUI_ROTATE(21);
        decoded = encoded - OBFH_GUI_BIAS(21);
        decoded = ((decoded >> rotate) | (decoded << (sizeof(ULONG_PTR) * 8 - rotate))) ^ OBFH_GUI_KEY(21);
        CHECK(decoded == (ULONG_PTR)native_export(user, "GetSysColor"));
        CHECK(test_gui_slots[21].ready);
    }
    {
        unsigned char bytes[64];
        size_t length = OBFH_GUI_NAME_GetSysColorBrush(bytes);
        SetLastError(0x12345678u);
        encoded = obfh_gui_cold(0, &test_gui_slots[22], 22, bytes, length);
        CHECK(GetLastError() == 0x12345678u);
        rotate = OBFH_GUI_ROTATE(22);
        decoded = encoded - OBFH_GUI_BIAS(22);
        decoded = ((decoded >> rotate) | (decoded << (sizeof(ULONG_PTR) * 8 - rotate))) ^ OBFH_GUI_KEY(22);
        CHECK(decoded == (ULONG_PTR)native_export(user, "GetSysColorBrush"));
        CHECK(test_gui_slots[22].ready);
    }
    {
        unsigned char bytes[64];
        size_t length = OBFH_GUI_NAME_GetSystemMetrics(bytes);
        SetLastError(0x12345678u);
        encoded = obfh_gui_cold(0, &test_gui_slots[23], 23, bytes, length);
        CHECK(GetLastError() == 0x12345678u);
        rotate = OBFH_GUI_ROTATE(23);
        decoded = encoded - OBFH_GUI_BIAS(23);
        decoded = ((decoded >> rotate) | (decoded << (sizeof(ULONG_PTR) * 8 - rotate))) ^ OBFH_GUI_KEY(23);
        CHECK(decoded == (ULONG_PTR)native_export(user, "GetSystemMetrics"));
        CHECK(test_gui_slots[23].ready);
    }
    {
        unsigned char bytes[64];
        size_t length = OBFH_GUI_NAME_GetWindowRect(bytes);
        SetLastError(0x12345678u);
        encoded = obfh_gui_cold(0, &test_gui_slots[24], 24, bytes, length);
        CHECK(GetLastError() == 0x12345678u);
        rotate = OBFH_GUI_ROTATE(24);
        decoded = encoded - OBFH_GUI_BIAS(24);
        decoded = ((decoded >> rotate) | (decoded << (sizeof(ULONG_PTR) * 8 - rotate))) ^ OBFH_GUI_KEY(24);
        CHECK(decoded == (ULONG_PTR)native_export(user, "GetWindowRect"));
        CHECK(test_gui_slots[24].ready);
    }
    {
        unsigned char bytes[64];
        size_t length = OBFH_GUI_NAME_GetWindowTextA(bytes);
        SetLastError(0x12345678u);
        encoded = obfh_gui_cold(0, &test_gui_slots[25], 25, bytes, length);
        CHECK(GetLastError() == 0x12345678u);
        rotate = OBFH_GUI_ROTATE(25);
        decoded = encoded - OBFH_GUI_BIAS(25);
        decoded = ((decoded >> rotate) | (decoded << (sizeof(ULONG_PTR) * 8 - rotate))) ^ OBFH_GUI_KEY(25);
        CHECK(decoded == (ULONG_PTR)native_export(user, "GetWindowTextA"));
        CHECK(test_gui_slots[25].ready);
    }
    {
        unsigned char bytes[64];
        size_t length = OBFH_GUI_NAME_GetWindowTextW(bytes);
        SetLastError(0x12345678u);
        encoded = obfh_gui_cold(0, &test_gui_slots[26], 26, bytes, length);
        CHECK(GetLastError() == 0x12345678u);
        rotate = OBFH_GUI_ROTATE(26);
        decoded = encoded - OBFH_GUI_BIAS(26);
        decoded = ((decoded >> rotate) | (decoded << (sizeof(ULONG_PTR) * 8 - rotate))) ^ OBFH_GUI_KEY(26);
        CHECK(decoded == (ULONG_PTR)native_export(user, "GetWindowTextW"));
        CHECK(test_gui_slots[26].ready);
    }
    {
        unsigned char bytes[64];
        size_t length = OBFH_GUI_NAME_GetWindowTextLengthA(bytes);
        SetLastError(0x12345678u);
        encoded = obfh_gui_cold(0, &test_gui_slots[27], 27, bytes, length);
        CHECK(GetLastError() == 0x12345678u);
        rotate = OBFH_GUI_ROTATE(27);
        decoded = encoded - OBFH_GUI_BIAS(27);
        decoded = ((decoded >> rotate) | (decoded << (sizeof(ULONG_PTR) * 8 - rotate))) ^ OBFH_GUI_KEY(27);
        CHECK(decoded == (ULONG_PTR)native_export(user, "GetWindowTextLengthA"));
        CHECK(test_gui_slots[27].ready);
    }
    {
        unsigned char bytes[64];
        size_t length = OBFH_GUI_NAME_GetWindowTextLengthW(bytes);
        SetLastError(0x12345678u);
        encoded = obfh_gui_cold(0, &test_gui_slots[28], 28, bytes, length);
        CHECK(GetLastError() == 0x12345678u);
        rotate = OBFH_GUI_ROTATE(28);
        decoded = encoded - OBFH_GUI_BIAS(28);
        decoded = ((decoded >> rotate) | (decoded << (sizeof(ULONG_PTR) * 8 - rotate))) ^ OBFH_GUI_KEY(28);
        CHECK(decoded == (ULONG_PTR)native_export(user, "GetWindowTextLengthW"));
        CHECK(test_gui_slots[28].ready);
    }
    {
        unsigned char bytes[64];
        size_t length = OBFH_GUI_NAME_IsDialogMessageA(bytes);
        SetLastError(0x12345678u);
        encoded = obfh_gui_cold(0, &test_gui_slots[29], 29, bytes, length);
        CHECK(GetLastError() == 0x12345678u);
        rotate = OBFH_GUI_ROTATE(29);
        decoded = encoded - OBFH_GUI_BIAS(29);
        decoded = ((decoded >> rotate) | (decoded << (sizeof(ULONG_PTR) * 8 - rotate))) ^ OBFH_GUI_KEY(29);
        CHECK(decoded == (ULONG_PTR)native_export(user, "IsDialogMessageA"));
        CHECK(test_gui_slots[29].ready);
    }
    {
        unsigned char bytes[64];
        size_t length = OBFH_GUI_NAME_IsDialogMessageW(bytes);
        SetLastError(0x12345678u);
        encoded = obfh_gui_cold(0, &test_gui_slots[30], 30, bytes, length);
        CHECK(GetLastError() == 0x12345678u);
        rotate = OBFH_GUI_ROTATE(30);
        decoded = encoded - OBFH_GUI_BIAS(30);
        decoded = ((decoded >> rotate) | (decoded << (sizeof(ULONG_PTR) * 8 - rotate))) ^ OBFH_GUI_KEY(30);
        CHECK(decoded == (ULONG_PTR)native_export(user, "IsDialogMessageW"));
        CHECK(test_gui_slots[30].ready);
    }
    {
        unsigned char bytes[64];
        size_t length = OBFH_GUI_NAME_LoadCursorA(bytes);
        SetLastError(0x12345678u);
        encoded = obfh_gui_cold(0, &test_gui_slots[31], 31, bytes, length);
        CHECK(GetLastError() == 0x12345678u);
        rotate = OBFH_GUI_ROTATE(31);
        decoded = encoded - OBFH_GUI_BIAS(31);
        decoded = ((decoded >> rotate) | (decoded << (sizeof(ULONG_PTR) * 8 - rotate))) ^ OBFH_GUI_KEY(31);
        CHECK(decoded == (ULONG_PTR)native_export(user, "LoadCursorA"));
        CHECK(test_gui_slots[31].ready);
    }
    {
        unsigned char bytes[64];
        size_t length = OBFH_GUI_NAME_LoadCursorW(bytes);
        SetLastError(0x12345678u);
        encoded = obfh_gui_cold(0, &test_gui_slots[32], 32, bytes, length);
        CHECK(GetLastError() == 0x12345678u);
        rotate = OBFH_GUI_ROTATE(32);
        decoded = encoded - OBFH_GUI_BIAS(32);
        decoded = ((decoded >> rotate) | (decoded << (sizeof(ULONG_PTR) * 8 - rotate))) ^ OBFH_GUI_KEY(32);
        CHECK(decoded == (ULONG_PTR)native_export(user, "LoadCursorW"));
        CHECK(test_gui_slots[32].ready);
    }
    {
        unsigned char bytes[64];
        size_t length = OBFH_GUI_NAME_LoadIconA(bytes);
        SetLastError(0x12345678u);
        encoded = obfh_gui_cold(0, &test_gui_slots[33], 33, bytes, length);
        CHECK(GetLastError() == 0x12345678u);
        rotate = OBFH_GUI_ROTATE(33);
        decoded = encoded - OBFH_GUI_BIAS(33);
        decoded = ((decoded >> rotate) | (decoded << (sizeof(ULONG_PTR) * 8 - rotate))) ^ OBFH_GUI_KEY(33);
        CHECK(decoded == (ULONG_PTR)native_export(user, "LoadIconA"));
        CHECK(test_gui_slots[33].ready);
    }
    {
        unsigned char bytes[64];
        size_t length = OBFH_GUI_NAME_LoadIconW(bytes);
        SetLastError(0x12345678u);
        encoded = obfh_gui_cold(0, &test_gui_slots[34], 34, bytes, length);
        CHECK(GetLastError() == 0x12345678u);
        rotate = OBFH_GUI_ROTATE(34);
        decoded = encoded - OBFH_GUI_BIAS(34);
        decoded = ((decoded >> rotate) | (decoded << (sizeof(ULONG_PTR) * 8 - rotate))) ^ OBFH_GUI_KEY(34);
        CHECK(decoded == (ULONG_PTR)native_export(user, "LoadIconW"));
        CHECK(test_gui_slots[34].ready);
    }
    {
        unsigned char bytes[64];
        size_t length = OBFH_GUI_NAME_MessageBeep(bytes);
        SetLastError(0x12345678u);
        encoded = obfh_gui_cold(0, &test_gui_slots[35], 35, bytes, length);
        CHECK(GetLastError() == 0x12345678u);
        rotate = OBFH_GUI_ROTATE(35);
        decoded = encoded - OBFH_GUI_BIAS(35);
        decoded = ((decoded >> rotate) | (decoded << (sizeof(ULONG_PTR) * 8 - rotate))) ^ OBFH_GUI_KEY(35);
        CHECK(decoded == (ULONG_PTR)native_export(user, "MessageBeep"));
        CHECK(test_gui_slots[35].ready);
    }
    {
        unsigned char bytes[64];
        size_t length = OBFH_GUI_NAME_MessageBoxA(bytes);
        SetLastError(0x12345678u);
        encoded = obfh_gui_cold(0, &test_gui_slots[36], 36, bytes, length);
        CHECK(GetLastError() == 0x12345678u);
        rotate = OBFH_GUI_ROTATE(36);
        decoded = encoded - OBFH_GUI_BIAS(36);
        decoded = ((decoded >> rotate) | (decoded << (sizeof(ULONG_PTR) * 8 - rotate))) ^ OBFH_GUI_KEY(36);
        CHECK(decoded == (ULONG_PTR)native_export(user, "MessageBoxA"));
        CHECK(test_gui_slots[36].ready);
    }
    {
        unsigned char bytes[64];
        size_t length = OBFH_GUI_NAME_MessageBoxW(bytes);
        SetLastError(0x12345678u);
        encoded = obfh_gui_cold(0, &test_gui_slots[37], 37, bytes, length);
        CHECK(GetLastError() == 0x12345678u);
        rotate = OBFH_GUI_ROTATE(37);
        decoded = encoded - OBFH_GUI_BIAS(37);
        decoded = ((decoded >> rotate) | (decoded << (sizeof(ULONG_PTR) * 8 - rotate))) ^ OBFH_GUI_KEY(37);
        CHECK(decoded == (ULONG_PTR)native_export(user, "MessageBoxW"));
        CHECK(test_gui_slots[37].ready);
    }
    {
        unsigned char bytes[64];
        size_t length = OBFH_GUI_NAME_MoveWindow(bytes);
        SetLastError(0x12345678u);
        encoded = obfh_gui_cold(0, &test_gui_slots[38], 38, bytes, length);
        CHECK(GetLastError() == 0x12345678u);
        rotate = OBFH_GUI_ROTATE(38);
        decoded = encoded - OBFH_GUI_BIAS(38);
        decoded = ((decoded >> rotate) | (decoded << (sizeof(ULONG_PTR) * 8 - rotate))) ^ OBFH_GUI_KEY(38);
        CHECK(decoded == (ULONG_PTR)native_export(user, "MoveWindow"));
        CHECK(test_gui_slots[38].ready);
    }
    {
        unsigned char bytes[64];
        size_t length = OBFH_GUI_NAME_OpenClipboard(bytes);
        SetLastError(0x12345678u);
        encoded = obfh_gui_cold(0, &test_gui_slots[39], 39, bytes, length);
        CHECK(GetLastError() == 0x12345678u);
        rotate = OBFH_GUI_ROTATE(39);
        decoded = encoded - OBFH_GUI_BIAS(39);
        decoded = ((decoded >> rotate) | (decoded << (sizeof(ULONG_PTR) * 8 - rotate))) ^ OBFH_GUI_KEY(39);
        CHECK(decoded == (ULONG_PTR)native_export(user, "OpenClipboard"));
        CHECK(test_gui_slots[39].ready);
    }
    {
        unsigned char bytes[64];
        size_t length = OBFH_GUI_NAME_PostQuitMessage(bytes);
        SetLastError(0x12345678u);
        encoded = obfh_gui_cold(0, &test_gui_slots[40], 40, bytes, length);
        CHECK(GetLastError() == 0x12345678u);
        rotate = OBFH_GUI_ROTATE(40);
        decoded = encoded - OBFH_GUI_BIAS(40);
        decoded = ((decoded >> rotate) | (decoded << (sizeof(ULONG_PTR) * 8 - rotate))) ^ OBFH_GUI_KEY(40);
        CHECK(decoded == (ULONG_PTR)native_export(user, "PostQuitMessage"));
        CHECK(test_gui_slots[40].ready);
    }
    {
        unsigned char bytes[64];
        size_t length = OBFH_GUI_NAME_RegisterClassExA(bytes);
        SetLastError(0x12345678u);
        encoded = obfh_gui_cold(0, &test_gui_slots[41], 41, bytes, length);
        CHECK(GetLastError() == 0x12345678u);
        rotate = OBFH_GUI_ROTATE(41);
        decoded = encoded - OBFH_GUI_BIAS(41);
        decoded = ((decoded >> rotate) | (decoded << (sizeof(ULONG_PTR) * 8 - rotate))) ^ OBFH_GUI_KEY(41);
        CHECK(decoded == (ULONG_PTR)native_export(user, "RegisterClassExA"));
        CHECK(test_gui_slots[41].ready);
    }
    {
        unsigned char bytes[64];
        size_t length = OBFH_GUI_NAME_RegisterClassExW(bytes);
        SetLastError(0x12345678u);
        encoded = obfh_gui_cold(0, &test_gui_slots[42], 42, bytes, length);
        CHECK(GetLastError() == 0x12345678u);
        rotate = OBFH_GUI_ROTATE(42);
        decoded = encoded - OBFH_GUI_BIAS(42);
        decoded = ((decoded >> rotate) | (decoded << (sizeof(ULONG_PTR) * 8 - rotate))) ^ OBFH_GUI_KEY(42);
        CHECK(decoded == (ULONG_PTR)native_export(user, "RegisterClassExW"));
        CHECK(test_gui_slots[42].ready);
    }
    {
        unsigned char bytes[64];
        size_t length = OBFH_GUI_NAME_ReleaseDC(bytes);
        SetLastError(0x12345678u);
        encoded = obfh_gui_cold(0, &test_gui_slots[43], 43, bytes, length);
        CHECK(GetLastError() == 0x12345678u);
        rotate = OBFH_GUI_ROTATE(43);
        decoded = encoded - OBFH_GUI_BIAS(43);
        decoded = ((decoded >> rotate) | (decoded << (sizeof(ULONG_PTR) * 8 - rotate))) ^ OBFH_GUI_KEY(43);
        CHECK(decoded == (ULONG_PTR)native_export(user, "ReleaseDC"));
        CHECK(test_gui_slots[43].ready);
    }
    {
        unsigned char bytes[64];
        size_t length = OBFH_GUI_NAME_SendMessageA(bytes);
        SetLastError(0x12345678u);
        encoded = obfh_gui_cold(0, &test_gui_slots[44], 44, bytes, length);
        CHECK(GetLastError() == 0x12345678u);
        rotate = OBFH_GUI_ROTATE(44);
        decoded = encoded - OBFH_GUI_BIAS(44);
        decoded = ((decoded >> rotate) | (decoded << (sizeof(ULONG_PTR) * 8 - rotate))) ^ OBFH_GUI_KEY(44);
        CHECK(decoded == (ULONG_PTR)native_export(user, "SendMessageA"));
        CHECK(test_gui_slots[44].ready);
    }
    {
        unsigned char bytes[64];
        size_t length = OBFH_GUI_NAME_SendMessageW(bytes);
        SetLastError(0x12345678u);
        encoded = obfh_gui_cold(0, &test_gui_slots[45], 45, bytes, length);
        CHECK(GetLastError() == 0x12345678u);
        rotate = OBFH_GUI_ROTATE(45);
        decoded = encoded - OBFH_GUI_BIAS(45);
        decoded = ((decoded >> rotate) | (decoded << (sizeof(ULONG_PTR) * 8 - rotate))) ^ OBFH_GUI_KEY(45);
        CHECK(decoded == (ULONG_PTR)native_export(user, "SendMessageW"));
        CHECK(test_gui_slots[45].ready);
    }
    {
        unsigned char bytes[64];
        size_t length = OBFH_GUI_NAME_SetClipboardData(bytes);
        SetLastError(0x12345678u);
        encoded = obfh_gui_cold(0, &test_gui_slots[46], 46, bytes, length);
        CHECK(GetLastError() == 0x12345678u);
        rotate = OBFH_GUI_ROTATE(46);
        decoded = encoded - OBFH_GUI_BIAS(46);
        decoded = ((decoded >> rotate) | (decoded << (sizeof(ULONG_PTR) * 8 - rotate))) ^ OBFH_GUI_KEY(46);
        CHECK(decoded == (ULONG_PTR)native_export(user, "SetClipboardData"));
        CHECK(test_gui_slots[46].ready);
    }
    {
        unsigned char bytes[64];
        size_t length = OBFH_GUI_NAME_SetFocus(bytes);
        SetLastError(0x12345678u);
        encoded = obfh_gui_cold(0, &test_gui_slots[47], 47, bytes, length);
        CHECK(GetLastError() == 0x12345678u);
        rotate = OBFH_GUI_ROTATE(47);
        decoded = encoded - OBFH_GUI_BIAS(47);
        decoded = ((decoded >> rotate) | (decoded << (sizeof(ULONG_PTR) * 8 - rotate))) ^ OBFH_GUI_KEY(47);
        CHECK(decoded == (ULONG_PTR)native_export(user, "SetFocus"));
        CHECK(test_gui_slots[47].ready);
    }
    {
        unsigned char bytes[64];
        size_t length = OBFH_GUI_NAME_SetWindowPos(bytes);
        SetLastError(0x12345678u);
        encoded = obfh_gui_cold(0, &test_gui_slots[48], 48, bytes, length);
        CHECK(GetLastError() == 0x12345678u);
        rotate = OBFH_GUI_ROTATE(48);
        decoded = encoded - OBFH_GUI_BIAS(48);
        decoded = ((decoded >> rotate) | (decoded << (sizeof(ULONG_PTR) * 8 - rotate))) ^ OBFH_GUI_KEY(48);
        CHECK(decoded == (ULONG_PTR)native_export(user, "SetWindowPos"));
        CHECK(test_gui_slots[48].ready);
    }
    {
        unsigned char bytes[64];
        size_t length = OBFH_GUI_NAME_SetWindowTextA(bytes);
        SetLastError(0x12345678u);
        encoded = obfh_gui_cold(0, &test_gui_slots[49], 49, bytes, length);
        CHECK(GetLastError() == 0x12345678u);
        rotate = OBFH_GUI_ROTATE(49);
        decoded = encoded - OBFH_GUI_BIAS(49);
        decoded = ((decoded >> rotate) | (decoded << (sizeof(ULONG_PTR) * 8 - rotate))) ^ OBFH_GUI_KEY(49);
        CHECK(decoded == (ULONG_PTR)native_export(user, "SetWindowTextA"));
        CHECK(test_gui_slots[49].ready);
    }
    {
        unsigned char bytes[64];
        size_t length = OBFH_GUI_NAME_SetWindowTextW(bytes);
        SetLastError(0x12345678u);
        encoded = obfh_gui_cold(0, &test_gui_slots[50], 50, bytes, length);
        CHECK(GetLastError() == 0x12345678u);
        rotate = OBFH_GUI_ROTATE(50);
        decoded = encoded - OBFH_GUI_BIAS(50);
        decoded = ((decoded >> rotate) | (decoded << (sizeof(ULONG_PTR) * 8 - rotate))) ^ OBFH_GUI_KEY(50);
        CHECK(decoded == (ULONG_PTR)native_export(user, "SetWindowTextW"));
        CHECK(test_gui_slots[50].ready);
    }
    {
        unsigned char bytes[64];
        size_t length = OBFH_GUI_NAME_ShowWindow(bytes);
        SetLastError(0x12345678u);
        encoded = obfh_gui_cold(0, &test_gui_slots[51], 51, bytes, length);
        CHECK(GetLastError() == 0x12345678u);
        rotate = OBFH_GUI_ROTATE(51);
        decoded = encoded - OBFH_GUI_BIAS(51);
        decoded = ((decoded >> rotate) | (decoded << (sizeof(ULONG_PTR) * 8 - rotate))) ^ OBFH_GUI_KEY(51);
        CHECK(decoded == (ULONG_PTR)native_export(user, "ShowWindow"));
        CHECK(test_gui_slots[51].ready);
    }
    {
        unsigned char bytes[64];
        size_t length = OBFH_GUI_NAME_TranslateMessage(bytes);
        SetLastError(0x12345678u);
        encoded = obfh_gui_cold(0, &test_gui_slots[52], 52, bytes, length);
        CHECK(GetLastError() == 0x12345678u);
        rotate = OBFH_GUI_ROTATE(52);
        decoded = encoded - OBFH_GUI_BIAS(52);
        decoded = ((decoded >> rotate) | (decoded << (sizeof(ULONG_PTR) * 8 - rotate))) ^ OBFH_GUI_KEY(52);
        CHECK(decoded == (ULONG_PTR)native_export(user, "TranslateMessage"));
        CHECK(test_gui_slots[52].ready);
    }
    {
        unsigned char bytes[64];
        size_t length = OBFH_GUI_NAME_UnregisterClassA(bytes);
        SetLastError(0x12345678u);
        encoded = obfh_gui_cold(0, &test_gui_slots[53], 53, bytes, length);
        CHECK(GetLastError() == 0x12345678u);
        rotate = OBFH_GUI_ROTATE(53);
        decoded = encoded - OBFH_GUI_BIAS(53);
        decoded = ((decoded >> rotate) | (decoded << (sizeof(ULONG_PTR) * 8 - rotate))) ^ OBFH_GUI_KEY(53);
        CHECK(decoded == (ULONG_PTR)native_export(user, "UnregisterClassA"));
        CHECK(test_gui_slots[53].ready);
    }
    {
        unsigned char bytes[64];
        size_t length = OBFH_GUI_NAME_UnregisterClassW(bytes);
        SetLastError(0x12345678u);
        encoded = obfh_gui_cold(0, &test_gui_slots[54], 54, bytes, length);
        CHECK(GetLastError() == 0x12345678u);
        rotate = OBFH_GUI_ROTATE(54);
        decoded = encoded - OBFH_GUI_BIAS(54);
        decoded = ((decoded >> rotate) | (decoded << (sizeof(ULONG_PTR) * 8 - rotate))) ^ OBFH_GUI_KEY(54);
        CHECK(decoded == (ULONG_PTR)native_export(user, "UnregisterClassW"));
        CHECK(test_gui_slots[54].ready);
    }
    {
        unsigned char bytes[64];
        size_t length = OBFH_GUI_NAME_UpdateWindow(bytes);
        SetLastError(0x12345678u);
        encoded = obfh_gui_cold(0, &test_gui_slots[55], 55, bytes, length);
        CHECK(GetLastError() == 0x12345678u);
        rotate = OBFH_GUI_ROTATE(55);
        decoded = encoded - OBFH_GUI_BIAS(55);
        decoded = ((decoded >> rotate) | (decoded << (sizeof(ULONG_PTR) * 8 - rotate))) ^ OBFH_GUI_KEY(55);
        CHECK(decoded == (ULONG_PTR)native_export(user, "UpdateWindow"));
        CHECK(test_gui_slots[55].ready);
    }
    {
        unsigned char bytes[64];
        size_t length = OBFH_GUI_NAME_CreateFontA(bytes);
        SetLastError(0x12345678u);
        encoded = obfh_gui_cold(1, &test_gui_slots[56], 56, bytes, length);
        CHECK(GetLastError() == 0x12345678u);
        rotate = OBFH_GUI_ROTATE(56);
        decoded = encoded - OBFH_GUI_BIAS(56);
        decoded = ((decoded >> rotate) | (decoded << (sizeof(ULONG_PTR) * 8 - rotate))) ^ OBFH_GUI_KEY(56);
        CHECK(decoded == (ULONG_PTR)native_export(gdi, "CreateFontA"));
        CHECK(test_gui_slots[56].ready);
    }
    {
        unsigned char bytes[64];
        size_t length = OBFH_GUI_NAME_CreateFontW(bytes);
        SetLastError(0x12345678u);
        encoded = obfh_gui_cold(1, &test_gui_slots[57], 57, bytes, length);
        CHECK(GetLastError() == 0x12345678u);
        rotate = OBFH_GUI_ROTATE(57);
        decoded = encoded - OBFH_GUI_BIAS(57);
        decoded = ((decoded >> rotate) | (decoded << (sizeof(ULONG_PTR) * 8 - rotate))) ^ OBFH_GUI_KEY(57);
        CHECK(decoded == (ULONG_PTR)native_export(gdi, "CreateFontW"));
        CHECK(test_gui_slots[57].ready);
    }
    {
        unsigned char bytes[64];
        size_t length = OBFH_GUI_NAME_CreateFontIndirectA(bytes);
        SetLastError(0x12345678u);
        encoded = obfh_gui_cold(1, &test_gui_slots[58], 58, bytes, length);
        CHECK(GetLastError() == 0x12345678u);
        rotate = OBFH_GUI_ROTATE(58);
        decoded = encoded - OBFH_GUI_BIAS(58);
        decoded = ((decoded >> rotate) | (decoded << (sizeof(ULONG_PTR) * 8 - rotate))) ^ OBFH_GUI_KEY(58);
        CHECK(decoded == (ULONG_PTR)native_export(gdi, "CreateFontIndirectA"));
        CHECK(test_gui_slots[58].ready);
    }
    {
        unsigned char bytes[64];
        size_t length = OBFH_GUI_NAME_CreateFontIndirectW(bytes);
        SetLastError(0x12345678u);
        encoded = obfh_gui_cold(1, &test_gui_slots[59], 59, bytes, length);
        CHECK(GetLastError() == 0x12345678u);
        rotate = OBFH_GUI_ROTATE(59);
        decoded = encoded - OBFH_GUI_BIAS(59);
        decoded = ((decoded >> rotate) | (decoded << (sizeof(ULONG_PTR) * 8 - rotate))) ^ OBFH_GUI_KEY(59);
        CHECK(decoded == (ULONG_PTR)native_export(gdi, "CreateFontIndirectW"));
        CHECK(test_gui_slots[59].ready);
    }
    {
        unsigned char bytes[64];
        size_t length = OBFH_GUI_NAME_DeleteObject(bytes);
        SetLastError(0x12345678u);
        encoded = obfh_gui_cold(1, &test_gui_slots[60], 60, bytes, length);
        CHECK(GetLastError() == 0x12345678u);
        rotate = OBFH_GUI_ROTATE(60);
        decoded = encoded - OBFH_GUI_BIAS(60);
        decoded = ((decoded >> rotate) | (decoded << (sizeof(ULONG_PTR) * 8 - rotate))) ^ OBFH_GUI_KEY(60);
        CHECK(decoded == (ULONG_PTR)native_export(gdi, "DeleteObject"));
        CHECK(test_gui_slots[60].ready);
    }
    {
        unsigned char bytes[64];
        size_t length = OBFH_GUI_NAME_GetDeviceCaps(bytes);
        SetLastError(0x12345678u);
        encoded = obfh_gui_cold(1, &test_gui_slots[61], 61, bytes, length);
        CHECK(GetLastError() == 0x12345678u);
        rotate = OBFH_GUI_ROTATE(61);
        decoded = encoded - OBFH_GUI_BIAS(61);
        decoded = ((decoded >> rotate) | (decoded << (sizeof(ULONG_PTR) * 8 - rotate))) ^ OBFH_GUI_KEY(61);
        CHECK(decoded == (ULONG_PTR)native_export(gdi, "GetDeviceCaps"));
        CHECK(test_gui_slots[61].ready);
    }
    {
        unsigned char bytes[64];
        size_t length = OBFH_GUI_NAME_SelectObject(bytes);
        SetLastError(0x12345678u);
        encoded = obfh_gui_cold(1, &test_gui_slots[62], 62, bytes, length);
        CHECK(GetLastError() == 0x12345678u);
        rotate = OBFH_GUI_ROTATE(62);
        decoded = encoded - OBFH_GUI_BIAS(62);
        decoded = ((decoded >> rotate) | (decoded << (sizeof(ULONG_PTR) * 8 - rotate))) ^ OBFH_GUI_KEY(62);
        CHECK(decoded == (ULONG_PTR)native_export(gdi, "SelectObject"));
        CHECK(test_gui_slots[62].ready);
    }
    {
        unsigned char bytes[64];
        size_t length = OBFH_GUI_NAME_SetBkColor(bytes);
        SetLastError(0x12345678u);
        encoded = obfh_gui_cold(1, &test_gui_slots[63], 63, bytes, length);
        CHECK(GetLastError() == 0x12345678u);
        rotate = OBFH_GUI_ROTATE(63);
        decoded = encoded - OBFH_GUI_BIAS(63);
        decoded = ((decoded >> rotate) | (decoded << (sizeof(ULONG_PTR) * 8 - rotate))) ^ OBFH_GUI_KEY(63);
        CHECK(decoded == (ULONG_PTR)native_export(gdi, "SetBkColor"));
        CHECK(test_gui_slots[63].ready);
    }
    {
        unsigned char bytes[64];
        size_t length = OBFH_GUI_NAME_SetTextColor(bytes);
        SetLastError(0x12345678u);
        encoded = obfh_gui_cold(1, &test_gui_slots[64], 64, bytes, length);
        CHECK(GetLastError() == 0x12345678u);
        rotate = OBFH_GUI_ROTATE(64);
        decoded = encoded - OBFH_GUI_BIAS(64);
        decoded = ((decoded >> rotate) | (decoded << (sizeof(ULONG_PTR) * 8 - rotate))) ^ OBFH_GUI_KEY(64);
        CHECK(decoded == (ULONG_PTR)native_export(gdi, "SetTextColor"));
        CHECK(test_gui_slots[64].ready);
    }
#endif
    // Exercise all four decode layouts across independently selected call sites.
    DWORD expected = native_color(COLOR_WINDOW);
    CHECK(GetSysColor(COLOR_WINDOW) == expected);
    CHECK(GetSysColor(COLOR_WINDOW) == expected);
    CHECK(GetSysColor(COLOR_WINDOW) == expected);
    CHECK(GetSysColor(COLOR_WINDOW) == expected);
    CHECK(GetSysColor(COLOR_WINDOW) == expected);
    CHECK(GetSysColor(COLOR_WINDOW) == expected);
    CHECK(GetSysColor(COLOR_WINDOW) == expected);
    CHECK(GetSysColor(COLOR_WINDOW) == expected);
    CHECK(GetSysColor(COLOR_WINDOW) == expected);
    CHECK(GetSysColor(COLOR_WINDOW) == expected);
    CHECK(GetSysColor(COLOR_WINDOW) == expected);
    CHECK(GetSysColor(COLOR_WINDOW) == expected);
    int side = 0;
    CHECK(GetSysColor((++side, COLOR_WINDOW)) == expected && side == 1);
    WNDCLASSEXW wc = {0};
    wc.cbSize = sizeof wc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpfnWndProc = procedure;
    wc.lpszClassName = L"obfh_gui_dynamic";
    CHECK(RegisterClassExW(&wc) != 0);
    HWND window = CreateWindowExW(0, wc.lpszClassName, L"Unicode \x03c0", WS_OVERLAPPEDWINDOW, 0, 0, 320, 240, NULL, NULL, wc.hInstance, NULL);
    CHECK(window && creates == 1);
    wchar_t wide[64];
    char ansi[64];
    CHECK(SetWindowTextA(window, "ascii"));
    CHECK(GetWindowTextA(window, ansi, 64) == 5 && strcmp(ansi, "ascii") == 0);
    CHECK(SetWindowTextW(window, L"wide \x03c0"));
    CHECK(GetWindowTextW(window, wide, 64) == 6 && wcscmp(wide, L"wide \x03c0") == 0);
    CHECK(GetWindowTextLengthA(window) > 0 && GetWindowTextLengthW(window) == 6);
#ifdef UNICODE
    CHECK(SetWindowText(window, L"generic") && GetWindowText(window, wide, 64) == 7);
#else
    CHECK(SetWindowText(window, "generic") && GetWindowText(window, ansi, 64) == 7);
#endif
    CHECK(SendMessageW(window, WM_APP + 1, 1, 0) == 18 && callbacks == 2);
    CHECK(SendMessageA(window, WM_APP + 1, 0, 0) == 17 && callbacks == 3);
    // API behavior includes genuine failure results and their LastError values.
    SetLastError(0x9999);
    BOOL ref = native_move(NULL, NULL, 0, 0, 10, 10, 0);
    DWORD ref_error = GetLastError();
    SetLastError(0x9999);
    BOOL actual = SetWindowPos(NULL, NULL, 0, 0, 10, 10, 0);
    CHECK(actual == ref && GetLastError() == ref_error);
    CHECK(SetWindowPos(window, NULL, 10, 10, 350, 250, SWP_NOACTIVATE | SWP_NOZORDER));
    RECT rect;
    CHECK(GetWindowRect(window, &rect) && GetClientRect(window, &rect));
    HDC dc = GetDC(window);
    CHECK(dc != NULL);
    CHECK(GetDeviceCaps(dc, LOGPIXELSX) == native_caps(dc, LOGPIXELSX));
    HFONT a = CreateFontA(16, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, "Segoe UI");
    HFONT b = CreateFontW(16, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, L"Segoe UI");
    CHECK(a && b);
    HGDIOBJ previous = SelectObject(dc, a);
    CHECK(previous != NULL);
    CHECK(SelectObject(dc, previous) == a);
    LOGFONTA la = {0};
    la.lfHeight = 17;
    strcpy(la.lfFaceName, "Segoe UI");
    HFONT c = CreateFontIndirectA(&la);
    LOGFONTW lw = {0};
    lw.lfHeight = 17;
    wcscpy(lw.lfFaceName, L"Segoe UI");
    HFONT d = CreateFontIndirectW(&lw);
    CHECK(c && d);
    SetBkColor(dc, RGB(1, 2, 3));
    SetTextColor(dc, RGB(4, 5, 6));
    CHECK(ReleaseDC(window, dc));
    CHECK(DeleteObject(a) && DeleteObject(b) && DeleteObject(c) && DeleteObject(d));
    HMENU menu = CreateMenu(), popup = CreatePopupMenu();
    CHECK(menu && popup);
    CHECK(AppendMenuA(popup, MF_STRING, 1, "one"));
    CHECK(AppendMenuW(menu, MF_POPUP, (UINT_PTR)popup, L"menu"));
    DestroyMenu(menu);
    CHECK(DestroyWindow(window));
    CHECK(UnregisterClassW(wc.lpszClassName, wc.hInstance));
    puts("GUI_CALLS_PASS");
    return 0;
}
