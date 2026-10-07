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
        obfh_gui_cold(2, OBFH_GUI_COUNT, bad, 1);
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
        const unsigned char bytes[] = OBFH_GUI_NAME_AppendMenuA;
        SetLastError(0x12345678u);
        encoded = obfh_gui_cold(0, 0, bytes, sizeof bytes);
        CHECK(GetLastError() == 0x12345678u);
        rotate = OBFH_GUI_ROTATE(0);
        decoded = encoded - OBFH_GUI_BIAS(0);
        decoded = ((decoded >> rotate) | (decoded << (sizeof(ULONG_PTR) * 8 - rotate))) ^ OBFH_GUI_KEY(0);
        CHECK(decoded == (ULONG_PTR)native_export(user, "AppendMenuA"));
        CHECK(obfh_gui_slots[0].ready);
    }
    {
        const unsigned char bytes[] = OBFH_GUI_NAME_AppendMenuW;
        SetLastError(0x12345678u);
        encoded = obfh_gui_cold(0, 1, bytes, sizeof bytes);
        CHECK(GetLastError() == 0x12345678u);
        rotate = OBFH_GUI_ROTATE(1);
        decoded = encoded - OBFH_GUI_BIAS(1);
        decoded = ((decoded >> rotate) | (decoded << (sizeof(ULONG_PTR) * 8 - rotate))) ^ OBFH_GUI_KEY(1);
        CHECK(decoded == (ULONG_PTR)native_export(user, "AppendMenuW"));
        CHECK(obfh_gui_slots[1].ready);
    }
    {
        const unsigned char bytes[] = OBFH_GUI_NAME_CheckMenuItem;
        SetLastError(0x12345678u);
        encoded = obfh_gui_cold(0, 2, bytes, sizeof bytes);
        CHECK(GetLastError() == 0x12345678u);
        rotate = OBFH_GUI_ROTATE(2);
        decoded = encoded - OBFH_GUI_BIAS(2);
        decoded = ((decoded >> rotate) | (decoded << (sizeof(ULONG_PTR) * 8 - rotate))) ^ OBFH_GUI_KEY(2);
        CHECK(decoded == (ULONG_PTR)native_export(user, "CheckMenuItem"));
        CHECK(obfh_gui_slots[2].ready);
    }
    {
        const unsigned char bytes[] = OBFH_GUI_NAME_CloseClipboard;
        SetLastError(0x12345678u);
        encoded = obfh_gui_cold(0, 3, bytes, sizeof bytes);
        CHECK(GetLastError() == 0x12345678u);
        rotate = OBFH_GUI_ROTATE(3);
        decoded = encoded - OBFH_GUI_BIAS(3);
        decoded = ((decoded >> rotate) | (decoded << (sizeof(ULONG_PTR) * 8 - rotate))) ^ OBFH_GUI_KEY(3);
        CHECK(decoded == (ULONG_PTR)native_export(user, "CloseClipboard"));
        CHECK(obfh_gui_slots[3].ready);
    }
    {
        const unsigned char bytes[] = OBFH_GUI_NAME_CreateMenu;
        SetLastError(0x12345678u);
        encoded = obfh_gui_cold(0, 4, bytes, sizeof bytes);
        CHECK(GetLastError() == 0x12345678u);
        rotate = OBFH_GUI_ROTATE(4);
        decoded = encoded - OBFH_GUI_BIAS(4);
        decoded = ((decoded >> rotate) | (decoded << (sizeof(ULONG_PTR) * 8 - rotate))) ^ OBFH_GUI_KEY(4);
        CHECK(decoded == (ULONG_PTR)native_export(user, "CreateMenu"));
        CHECK(obfh_gui_slots[4].ready);
    }
    {
        const unsigned char bytes[] = OBFH_GUI_NAME_CreatePopupMenu;
        SetLastError(0x12345678u);
        encoded = obfh_gui_cold(0, 5, bytes, sizeof bytes);
        CHECK(GetLastError() == 0x12345678u);
        rotate = OBFH_GUI_ROTATE(5);
        decoded = encoded - OBFH_GUI_BIAS(5);
        decoded = ((decoded >> rotate) | (decoded << (sizeof(ULONG_PTR) * 8 - rotate))) ^ OBFH_GUI_KEY(5);
        CHECK(decoded == (ULONG_PTR)native_export(user, "CreatePopupMenu"));
        CHECK(obfh_gui_slots[5].ready);
    }
    {
        const unsigned char bytes[] = OBFH_GUI_NAME_CreateWindowExA;
        SetLastError(0x12345678u);
        encoded = obfh_gui_cold(0, 6, bytes, sizeof bytes);
        CHECK(GetLastError() == 0x12345678u);
        rotate = OBFH_GUI_ROTATE(6);
        decoded = encoded - OBFH_GUI_BIAS(6);
        decoded = ((decoded >> rotate) | (decoded << (sizeof(ULONG_PTR) * 8 - rotate))) ^ OBFH_GUI_KEY(6);
        CHECK(decoded == (ULONG_PTR)native_export(user, "CreateWindowExA"));
        CHECK(obfh_gui_slots[6].ready);
    }
    {
        const unsigned char bytes[] = OBFH_GUI_NAME_CreateWindowExW;
        SetLastError(0x12345678u);
        encoded = obfh_gui_cold(0, 7, bytes, sizeof bytes);
        CHECK(GetLastError() == 0x12345678u);
        rotate = OBFH_GUI_ROTATE(7);
        decoded = encoded - OBFH_GUI_BIAS(7);
        decoded = ((decoded >> rotate) | (decoded << (sizeof(ULONG_PTR) * 8 - rotate))) ^ OBFH_GUI_KEY(7);
        CHECK(decoded == (ULONG_PTR)native_export(user, "CreateWindowExW"));
        CHECK(obfh_gui_slots[7].ready);
    }
    {
        const unsigned char bytes[] = OBFH_GUI_NAME_DefWindowProcA;
        SetLastError(0x12345678u);
        encoded = obfh_gui_cold(0, 8, bytes, sizeof bytes);
        CHECK(GetLastError() == 0x12345678u);
        rotate = OBFH_GUI_ROTATE(8);
        decoded = encoded - OBFH_GUI_BIAS(8);
        decoded = ((decoded >> rotate) | (decoded << (sizeof(ULONG_PTR) * 8 - rotate))) ^ OBFH_GUI_KEY(8);
        CHECK(decoded == (ULONG_PTR)native_export(user, "DefWindowProcA"));
        CHECK(obfh_gui_slots[8].ready);
    }
    {
        const unsigned char bytes[] = OBFH_GUI_NAME_DefWindowProcW;
        SetLastError(0x12345678u);
        encoded = obfh_gui_cold(0, 9, bytes, sizeof bytes);
        CHECK(GetLastError() == 0x12345678u);
        rotate = OBFH_GUI_ROTATE(9);
        decoded = encoded - OBFH_GUI_BIAS(9);
        decoded = ((decoded >> rotate) | (decoded << (sizeof(ULONG_PTR) * 8 - rotate))) ^ OBFH_GUI_KEY(9);
        CHECK(decoded == (ULONG_PTR)native_export(user, "DefWindowProcW"));
        CHECK(obfh_gui_slots[9].ready);
    }
    {
        const unsigned char bytes[] = OBFH_GUI_NAME_DestroyWindow;
        SetLastError(0x12345678u);
        encoded = obfh_gui_cold(0, 10, bytes, sizeof bytes);
        CHECK(GetLastError() == 0x12345678u);
        rotate = OBFH_GUI_ROTATE(10);
        decoded = encoded - OBFH_GUI_BIAS(10);
        decoded = ((decoded >> rotate) | (decoded << (sizeof(ULONG_PTR) * 8 - rotate))) ^ OBFH_GUI_KEY(10);
        CHECK(decoded == (ULONG_PTR)native_export(user, "DestroyWindow"));
        CHECK(obfh_gui_slots[10].ready);
    }
    {
        const unsigned char bytes[] = OBFH_GUI_NAME_DispatchMessageA;
        SetLastError(0x12345678u);
        encoded = obfh_gui_cold(0, 11, bytes, sizeof bytes);
        CHECK(GetLastError() == 0x12345678u);
        rotate = OBFH_GUI_ROTATE(11);
        decoded = encoded - OBFH_GUI_BIAS(11);
        decoded = ((decoded >> rotate) | (decoded << (sizeof(ULONG_PTR) * 8 - rotate))) ^ OBFH_GUI_KEY(11);
        CHECK(decoded == (ULONG_PTR)native_export(user, "DispatchMessageA"));
        CHECK(obfh_gui_slots[11].ready);
    }
    {
        const unsigned char bytes[] = OBFH_GUI_NAME_DispatchMessageW;
        SetLastError(0x12345678u);
        encoded = obfh_gui_cold(0, 12, bytes, sizeof bytes);
        CHECK(GetLastError() == 0x12345678u);
        rotate = OBFH_GUI_ROTATE(12);
        decoded = encoded - OBFH_GUI_BIAS(12);
        decoded = ((decoded >> rotate) | (decoded << (sizeof(ULONG_PTR) * 8 - rotate))) ^ OBFH_GUI_KEY(12);
        CHECK(decoded == (ULONG_PTR)native_export(user, "DispatchMessageW"));
        CHECK(obfh_gui_slots[12].ready);
    }
    {
        const unsigned char bytes[] = OBFH_GUI_NAME_EmptyClipboard;
        SetLastError(0x12345678u);
        encoded = obfh_gui_cold(0, 13, bytes, sizeof bytes);
        CHECK(GetLastError() == 0x12345678u);
        rotate = OBFH_GUI_ROTATE(13);
        decoded = encoded - OBFH_GUI_BIAS(13);
        decoded = ((decoded >> rotate) | (decoded << (sizeof(ULONG_PTR) * 8 - rotate))) ^ OBFH_GUI_KEY(13);
        CHECK(decoded == (ULONG_PTR)native_export(user, "EmptyClipboard"));
        CHECK(obfh_gui_slots[13].ready);
    }
    {
        const unsigned char bytes[] = OBFH_GUI_NAME_GetClientRect;
        SetLastError(0x12345678u);
        encoded = obfh_gui_cold(0, 14, bytes, sizeof bytes);
        CHECK(GetLastError() == 0x12345678u);
        rotate = OBFH_GUI_ROTATE(14);
        decoded = encoded - OBFH_GUI_BIAS(14);
        decoded = ((decoded >> rotate) | (decoded << (sizeof(ULONG_PTR) * 8 - rotate))) ^ OBFH_GUI_KEY(14);
        CHECK(decoded == (ULONG_PTR)native_export(user, "GetClientRect"));
        CHECK(obfh_gui_slots[14].ready);
    }
    {
        const unsigned char bytes[] = OBFH_GUI_NAME_GetDC;
        SetLastError(0x12345678u);
        encoded = obfh_gui_cold(0, 15, bytes, sizeof bytes);
        CHECK(GetLastError() == 0x12345678u);
        rotate = OBFH_GUI_ROTATE(15);
        decoded = encoded - OBFH_GUI_BIAS(15);
        decoded = ((decoded >> rotate) | (decoded << (sizeof(ULONG_PTR) * 8 - rotate))) ^ OBFH_GUI_KEY(15);
        CHECK(decoded == (ULONG_PTR)native_export(user, "GetDC"));
        CHECK(obfh_gui_slots[15].ready);
    }
    {
        const unsigned char bytes[] = OBFH_GUI_NAME_GetDlgItem;
        SetLastError(0x12345678u);
        encoded = obfh_gui_cold(0, 16, bytes, sizeof bytes);
        CHECK(GetLastError() == 0x12345678u);
        rotate = OBFH_GUI_ROTATE(16);
        decoded = encoded - OBFH_GUI_BIAS(16);
        decoded = ((decoded >> rotate) | (decoded << (sizeof(ULONG_PTR) * 8 - rotate))) ^ OBFH_GUI_KEY(16);
        CHECK(decoded == (ULONG_PTR)native_export(user, "GetDlgItem"));
        CHECK(obfh_gui_slots[16].ready);
    }
    {
        const unsigned char bytes[] = OBFH_GUI_NAME_GetKeyState;
        SetLastError(0x12345678u);
        encoded = obfh_gui_cold(0, 17, bytes, sizeof bytes);
        CHECK(GetLastError() == 0x12345678u);
        rotate = OBFH_GUI_ROTATE(17);
        decoded = encoded - OBFH_GUI_BIAS(17);
        decoded = ((decoded >> rotate) | (decoded << (sizeof(ULONG_PTR) * 8 - rotate))) ^ OBFH_GUI_KEY(17);
        CHECK(decoded == (ULONG_PTR)native_export(user, "GetKeyState"));
        CHECK(obfh_gui_slots[17].ready);
    }
    {
        const unsigned char bytes[] = OBFH_GUI_NAME_GetMenu;
        SetLastError(0x12345678u);
        encoded = obfh_gui_cold(0, 18, bytes, sizeof bytes);
        CHECK(GetLastError() == 0x12345678u);
        rotate = OBFH_GUI_ROTATE(18);
        decoded = encoded - OBFH_GUI_BIAS(18);
        decoded = ((decoded >> rotate) | (decoded << (sizeof(ULONG_PTR) * 8 - rotate))) ^ OBFH_GUI_KEY(18);
        CHECK(decoded == (ULONG_PTR)native_export(user, "GetMenu"));
        CHECK(obfh_gui_slots[18].ready);
    }
    {
        const unsigned char bytes[] = OBFH_GUI_NAME_GetMessageA;
        SetLastError(0x12345678u);
        encoded = obfh_gui_cold(0, 19, bytes, sizeof bytes);
        CHECK(GetLastError() == 0x12345678u);
        rotate = OBFH_GUI_ROTATE(19);
        decoded = encoded - OBFH_GUI_BIAS(19);
        decoded = ((decoded >> rotate) | (decoded << (sizeof(ULONG_PTR) * 8 - rotate))) ^ OBFH_GUI_KEY(19);
        CHECK(decoded == (ULONG_PTR)native_export(user, "GetMessageA"));
        CHECK(obfh_gui_slots[19].ready);
    }
    {
        const unsigned char bytes[] = OBFH_GUI_NAME_GetMessageW;
        SetLastError(0x12345678u);
        encoded = obfh_gui_cold(0, 20, bytes, sizeof bytes);
        CHECK(GetLastError() == 0x12345678u);
        rotate = OBFH_GUI_ROTATE(20);
        decoded = encoded - OBFH_GUI_BIAS(20);
        decoded = ((decoded >> rotate) | (decoded << (sizeof(ULONG_PTR) * 8 - rotate))) ^ OBFH_GUI_KEY(20);
        CHECK(decoded == (ULONG_PTR)native_export(user, "GetMessageW"));
        CHECK(obfh_gui_slots[20].ready);
    }
    {
        const unsigned char bytes[] = OBFH_GUI_NAME_GetSysColor;
        SetLastError(0x12345678u);
        encoded = obfh_gui_cold(0, 21, bytes, sizeof bytes);
        CHECK(GetLastError() == 0x12345678u);
        rotate = OBFH_GUI_ROTATE(21);
        decoded = encoded - OBFH_GUI_BIAS(21);
        decoded = ((decoded >> rotate) | (decoded << (sizeof(ULONG_PTR) * 8 - rotate))) ^ OBFH_GUI_KEY(21);
        CHECK(decoded == (ULONG_PTR)native_export(user, "GetSysColor"));
        CHECK(obfh_gui_slots[21].ready);
    }
    {
        const unsigned char bytes[] = OBFH_GUI_NAME_GetSysColorBrush;
        SetLastError(0x12345678u);
        encoded = obfh_gui_cold(0, 22, bytes, sizeof bytes);
        CHECK(GetLastError() == 0x12345678u);
        rotate = OBFH_GUI_ROTATE(22);
        decoded = encoded - OBFH_GUI_BIAS(22);
        decoded = ((decoded >> rotate) | (decoded << (sizeof(ULONG_PTR) * 8 - rotate))) ^ OBFH_GUI_KEY(22);
        CHECK(decoded == (ULONG_PTR)native_export(user, "GetSysColorBrush"));
        CHECK(obfh_gui_slots[22].ready);
    }
    {
        const unsigned char bytes[] = OBFH_GUI_NAME_GetSystemMetrics;
        SetLastError(0x12345678u);
        encoded = obfh_gui_cold(0, 23, bytes, sizeof bytes);
        CHECK(GetLastError() == 0x12345678u);
        rotate = OBFH_GUI_ROTATE(23);
        decoded = encoded - OBFH_GUI_BIAS(23);
        decoded = ((decoded >> rotate) | (decoded << (sizeof(ULONG_PTR) * 8 - rotate))) ^ OBFH_GUI_KEY(23);
        CHECK(decoded == (ULONG_PTR)native_export(user, "GetSystemMetrics"));
        CHECK(obfh_gui_slots[23].ready);
    }
    {
        const unsigned char bytes[] = OBFH_GUI_NAME_GetWindowRect;
        SetLastError(0x12345678u);
        encoded = obfh_gui_cold(0, 24, bytes, sizeof bytes);
        CHECK(GetLastError() == 0x12345678u);
        rotate = OBFH_GUI_ROTATE(24);
        decoded = encoded - OBFH_GUI_BIAS(24);
        decoded = ((decoded >> rotate) | (decoded << (sizeof(ULONG_PTR) * 8 - rotate))) ^ OBFH_GUI_KEY(24);
        CHECK(decoded == (ULONG_PTR)native_export(user, "GetWindowRect"));
        CHECK(obfh_gui_slots[24].ready);
    }
    {
        const unsigned char bytes[] = OBFH_GUI_NAME_GetWindowTextA;
        SetLastError(0x12345678u);
        encoded = obfh_gui_cold(0, 25, bytes, sizeof bytes);
        CHECK(GetLastError() == 0x12345678u);
        rotate = OBFH_GUI_ROTATE(25);
        decoded = encoded - OBFH_GUI_BIAS(25);
        decoded = ((decoded >> rotate) | (decoded << (sizeof(ULONG_PTR) * 8 - rotate))) ^ OBFH_GUI_KEY(25);
        CHECK(decoded == (ULONG_PTR)native_export(user, "GetWindowTextA"));
        CHECK(obfh_gui_slots[25].ready);
    }
    {
        const unsigned char bytes[] = OBFH_GUI_NAME_GetWindowTextW;
        SetLastError(0x12345678u);
        encoded = obfh_gui_cold(0, 26, bytes, sizeof bytes);
        CHECK(GetLastError() == 0x12345678u);
        rotate = OBFH_GUI_ROTATE(26);
        decoded = encoded - OBFH_GUI_BIAS(26);
        decoded = ((decoded >> rotate) | (decoded << (sizeof(ULONG_PTR) * 8 - rotate))) ^ OBFH_GUI_KEY(26);
        CHECK(decoded == (ULONG_PTR)native_export(user, "GetWindowTextW"));
        CHECK(obfh_gui_slots[26].ready);
    }
    {
        const unsigned char bytes[] = OBFH_GUI_NAME_GetWindowTextLengthA;
        SetLastError(0x12345678u);
        encoded = obfh_gui_cold(0, 27, bytes, sizeof bytes);
        CHECK(GetLastError() == 0x12345678u);
        rotate = OBFH_GUI_ROTATE(27);
        decoded = encoded - OBFH_GUI_BIAS(27);
        decoded = ((decoded >> rotate) | (decoded << (sizeof(ULONG_PTR) * 8 - rotate))) ^ OBFH_GUI_KEY(27);
        CHECK(decoded == (ULONG_PTR)native_export(user, "GetWindowTextLengthA"));
        CHECK(obfh_gui_slots[27].ready);
    }
    {
        const unsigned char bytes[] = OBFH_GUI_NAME_GetWindowTextLengthW;
        SetLastError(0x12345678u);
        encoded = obfh_gui_cold(0, 28, bytes, sizeof bytes);
        CHECK(GetLastError() == 0x12345678u);
        rotate = OBFH_GUI_ROTATE(28);
        decoded = encoded - OBFH_GUI_BIAS(28);
        decoded = ((decoded >> rotate) | (decoded << (sizeof(ULONG_PTR) * 8 - rotate))) ^ OBFH_GUI_KEY(28);
        CHECK(decoded == (ULONG_PTR)native_export(user, "GetWindowTextLengthW"));
        CHECK(obfh_gui_slots[28].ready);
    }
    {
        const unsigned char bytes[] = OBFH_GUI_NAME_IsDialogMessageA;
        SetLastError(0x12345678u);
        encoded = obfh_gui_cold(0, 29, bytes, sizeof bytes);
        CHECK(GetLastError() == 0x12345678u);
        rotate = OBFH_GUI_ROTATE(29);
        decoded = encoded - OBFH_GUI_BIAS(29);
        decoded = ((decoded >> rotate) | (decoded << (sizeof(ULONG_PTR) * 8 - rotate))) ^ OBFH_GUI_KEY(29);
        CHECK(decoded == (ULONG_PTR)native_export(user, "IsDialogMessageA"));
        CHECK(obfh_gui_slots[29].ready);
    }
    {
        const unsigned char bytes[] = OBFH_GUI_NAME_IsDialogMessageW;
        SetLastError(0x12345678u);
        encoded = obfh_gui_cold(0, 30, bytes, sizeof bytes);
        CHECK(GetLastError() == 0x12345678u);
        rotate = OBFH_GUI_ROTATE(30);
        decoded = encoded - OBFH_GUI_BIAS(30);
        decoded = ((decoded >> rotate) | (decoded << (sizeof(ULONG_PTR) * 8 - rotate))) ^ OBFH_GUI_KEY(30);
        CHECK(decoded == (ULONG_PTR)native_export(user, "IsDialogMessageW"));
        CHECK(obfh_gui_slots[30].ready);
    }
    {
        const unsigned char bytes[] = OBFH_GUI_NAME_LoadCursorA;
        SetLastError(0x12345678u);
        encoded = obfh_gui_cold(0, 31, bytes, sizeof bytes);
        CHECK(GetLastError() == 0x12345678u);
        rotate = OBFH_GUI_ROTATE(31);
        decoded = encoded - OBFH_GUI_BIAS(31);
        decoded = ((decoded >> rotate) | (decoded << (sizeof(ULONG_PTR) * 8 - rotate))) ^ OBFH_GUI_KEY(31);
        CHECK(decoded == (ULONG_PTR)native_export(user, "LoadCursorA"));
        CHECK(obfh_gui_slots[31].ready);
    }
    {
        const unsigned char bytes[] = OBFH_GUI_NAME_LoadCursorW;
        SetLastError(0x12345678u);
        encoded = obfh_gui_cold(0, 32, bytes, sizeof bytes);
        CHECK(GetLastError() == 0x12345678u);
        rotate = OBFH_GUI_ROTATE(32);
        decoded = encoded - OBFH_GUI_BIAS(32);
        decoded = ((decoded >> rotate) | (decoded << (sizeof(ULONG_PTR) * 8 - rotate))) ^ OBFH_GUI_KEY(32);
        CHECK(decoded == (ULONG_PTR)native_export(user, "LoadCursorW"));
        CHECK(obfh_gui_slots[32].ready);
    }
    {
        const unsigned char bytes[] = OBFH_GUI_NAME_LoadIconA;
        SetLastError(0x12345678u);
        encoded = obfh_gui_cold(0, 33, bytes, sizeof bytes);
        CHECK(GetLastError() == 0x12345678u);
        rotate = OBFH_GUI_ROTATE(33);
        decoded = encoded - OBFH_GUI_BIAS(33);
        decoded = ((decoded >> rotate) | (decoded << (sizeof(ULONG_PTR) * 8 - rotate))) ^ OBFH_GUI_KEY(33);
        CHECK(decoded == (ULONG_PTR)native_export(user, "LoadIconA"));
        CHECK(obfh_gui_slots[33].ready);
    }
    {
        const unsigned char bytes[] = OBFH_GUI_NAME_LoadIconW;
        SetLastError(0x12345678u);
        encoded = obfh_gui_cold(0, 34, bytes, sizeof bytes);
        CHECK(GetLastError() == 0x12345678u);
        rotate = OBFH_GUI_ROTATE(34);
        decoded = encoded - OBFH_GUI_BIAS(34);
        decoded = ((decoded >> rotate) | (decoded << (sizeof(ULONG_PTR) * 8 - rotate))) ^ OBFH_GUI_KEY(34);
        CHECK(decoded == (ULONG_PTR)native_export(user, "LoadIconW"));
        CHECK(obfh_gui_slots[34].ready);
    }
    {
        const unsigned char bytes[] = OBFH_GUI_NAME_MessageBeep;
        SetLastError(0x12345678u);
        encoded = obfh_gui_cold(0, 35, bytes, sizeof bytes);
        CHECK(GetLastError() == 0x12345678u);
        rotate = OBFH_GUI_ROTATE(35);
        decoded = encoded - OBFH_GUI_BIAS(35);
        decoded = ((decoded >> rotate) | (decoded << (sizeof(ULONG_PTR) * 8 - rotate))) ^ OBFH_GUI_KEY(35);
        CHECK(decoded == (ULONG_PTR)native_export(user, "MessageBeep"));
        CHECK(obfh_gui_slots[35].ready);
    }
    {
        const unsigned char bytes[] = OBFH_GUI_NAME_MessageBoxA;
        SetLastError(0x12345678u);
        encoded = obfh_gui_cold(0, 36, bytes, sizeof bytes);
        CHECK(GetLastError() == 0x12345678u);
        rotate = OBFH_GUI_ROTATE(36);
        decoded = encoded - OBFH_GUI_BIAS(36);
        decoded = ((decoded >> rotate) | (decoded << (sizeof(ULONG_PTR) * 8 - rotate))) ^ OBFH_GUI_KEY(36);
        CHECK(decoded == (ULONG_PTR)native_export(user, "MessageBoxA"));
        CHECK(obfh_gui_slots[36].ready);
    }
    {
        const unsigned char bytes[] = OBFH_GUI_NAME_MessageBoxW;
        SetLastError(0x12345678u);
        encoded = obfh_gui_cold(0, 37, bytes, sizeof bytes);
        CHECK(GetLastError() == 0x12345678u);
        rotate = OBFH_GUI_ROTATE(37);
        decoded = encoded - OBFH_GUI_BIAS(37);
        decoded = ((decoded >> rotate) | (decoded << (sizeof(ULONG_PTR) * 8 - rotate))) ^ OBFH_GUI_KEY(37);
        CHECK(decoded == (ULONG_PTR)native_export(user, "MessageBoxW"));
        CHECK(obfh_gui_slots[37].ready);
    }
    {
        const unsigned char bytes[] = OBFH_GUI_NAME_MoveWindow;
        SetLastError(0x12345678u);
        encoded = obfh_gui_cold(0, 38, bytes, sizeof bytes);
        CHECK(GetLastError() == 0x12345678u);
        rotate = OBFH_GUI_ROTATE(38);
        decoded = encoded - OBFH_GUI_BIAS(38);
        decoded = ((decoded >> rotate) | (decoded << (sizeof(ULONG_PTR) * 8 - rotate))) ^ OBFH_GUI_KEY(38);
        CHECK(decoded == (ULONG_PTR)native_export(user, "MoveWindow"));
        CHECK(obfh_gui_slots[38].ready);
    }
    {
        const unsigned char bytes[] = OBFH_GUI_NAME_OpenClipboard;
        SetLastError(0x12345678u);
        encoded = obfh_gui_cold(0, 39, bytes, sizeof bytes);
        CHECK(GetLastError() == 0x12345678u);
        rotate = OBFH_GUI_ROTATE(39);
        decoded = encoded - OBFH_GUI_BIAS(39);
        decoded = ((decoded >> rotate) | (decoded << (sizeof(ULONG_PTR) * 8 - rotate))) ^ OBFH_GUI_KEY(39);
        CHECK(decoded == (ULONG_PTR)native_export(user, "OpenClipboard"));
        CHECK(obfh_gui_slots[39].ready);
    }
    {
        const unsigned char bytes[] = OBFH_GUI_NAME_PostQuitMessage;
        SetLastError(0x12345678u);
        encoded = obfh_gui_cold(0, 40, bytes, sizeof bytes);
        CHECK(GetLastError() == 0x12345678u);
        rotate = OBFH_GUI_ROTATE(40);
        decoded = encoded - OBFH_GUI_BIAS(40);
        decoded = ((decoded >> rotate) | (decoded << (sizeof(ULONG_PTR) * 8 - rotate))) ^ OBFH_GUI_KEY(40);
        CHECK(decoded == (ULONG_PTR)native_export(user, "PostQuitMessage"));
        CHECK(obfh_gui_slots[40].ready);
    }
    {
        const unsigned char bytes[] = OBFH_GUI_NAME_RegisterClassExA;
        SetLastError(0x12345678u);
        encoded = obfh_gui_cold(0, 41, bytes, sizeof bytes);
        CHECK(GetLastError() == 0x12345678u);
        rotate = OBFH_GUI_ROTATE(41);
        decoded = encoded - OBFH_GUI_BIAS(41);
        decoded = ((decoded >> rotate) | (decoded << (sizeof(ULONG_PTR) * 8 - rotate))) ^ OBFH_GUI_KEY(41);
        CHECK(decoded == (ULONG_PTR)native_export(user, "RegisterClassExA"));
        CHECK(obfh_gui_slots[41].ready);
    }
    {
        const unsigned char bytes[] = OBFH_GUI_NAME_RegisterClassExW;
        SetLastError(0x12345678u);
        encoded = obfh_gui_cold(0, 42, bytes, sizeof bytes);
        CHECK(GetLastError() == 0x12345678u);
        rotate = OBFH_GUI_ROTATE(42);
        decoded = encoded - OBFH_GUI_BIAS(42);
        decoded = ((decoded >> rotate) | (decoded << (sizeof(ULONG_PTR) * 8 - rotate))) ^ OBFH_GUI_KEY(42);
        CHECK(decoded == (ULONG_PTR)native_export(user, "RegisterClassExW"));
        CHECK(obfh_gui_slots[42].ready);
    }
    {
        const unsigned char bytes[] = OBFH_GUI_NAME_ReleaseDC;
        SetLastError(0x12345678u);
        encoded = obfh_gui_cold(0, 43, bytes, sizeof bytes);
        CHECK(GetLastError() == 0x12345678u);
        rotate = OBFH_GUI_ROTATE(43);
        decoded = encoded - OBFH_GUI_BIAS(43);
        decoded = ((decoded >> rotate) | (decoded << (sizeof(ULONG_PTR) * 8 - rotate))) ^ OBFH_GUI_KEY(43);
        CHECK(decoded == (ULONG_PTR)native_export(user, "ReleaseDC"));
        CHECK(obfh_gui_slots[43].ready);
    }
    {
        const unsigned char bytes[] = OBFH_GUI_NAME_SendMessageA;
        SetLastError(0x12345678u);
        encoded = obfh_gui_cold(0, 44, bytes, sizeof bytes);
        CHECK(GetLastError() == 0x12345678u);
        rotate = OBFH_GUI_ROTATE(44);
        decoded = encoded - OBFH_GUI_BIAS(44);
        decoded = ((decoded >> rotate) | (decoded << (sizeof(ULONG_PTR) * 8 - rotate))) ^ OBFH_GUI_KEY(44);
        CHECK(decoded == (ULONG_PTR)native_export(user, "SendMessageA"));
        CHECK(obfh_gui_slots[44].ready);
    }
    {
        const unsigned char bytes[] = OBFH_GUI_NAME_SendMessageW;
        SetLastError(0x12345678u);
        encoded = obfh_gui_cold(0, 45, bytes, sizeof bytes);
        CHECK(GetLastError() == 0x12345678u);
        rotate = OBFH_GUI_ROTATE(45);
        decoded = encoded - OBFH_GUI_BIAS(45);
        decoded = ((decoded >> rotate) | (decoded << (sizeof(ULONG_PTR) * 8 - rotate))) ^ OBFH_GUI_KEY(45);
        CHECK(decoded == (ULONG_PTR)native_export(user, "SendMessageW"));
        CHECK(obfh_gui_slots[45].ready);
    }
    {
        const unsigned char bytes[] = OBFH_GUI_NAME_SetClipboardData;
        SetLastError(0x12345678u);
        encoded = obfh_gui_cold(0, 46, bytes, sizeof bytes);
        CHECK(GetLastError() == 0x12345678u);
        rotate = OBFH_GUI_ROTATE(46);
        decoded = encoded - OBFH_GUI_BIAS(46);
        decoded = ((decoded >> rotate) | (decoded << (sizeof(ULONG_PTR) * 8 - rotate))) ^ OBFH_GUI_KEY(46);
        CHECK(decoded == (ULONG_PTR)native_export(user, "SetClipboardData"));
        CHECK(obfh_gui_slots[46].ready);
    }
    {
        const unsigned char bytes[] = OBFH_GUI_NAME_SetFocus;
        SetLastError(0x12345678u);
        encoded = obfh_gui_cold(0, 47, bytes, sizeof bytes);
        CHECK(GetLastError() == 0x12345678u);
        rotate = OBFH_GUI_ROTATE(47);
        decoded = encoded - OBFH_GUI_BIAS(47);
        decoded = ((decoded >> rotate) | (decoded << (sizeof(ULONG_PTR) * 8 - rotate))) ^ OBFH_GUI_KEY(47);
        CHECK(decoded == (ULONG_PTR)native_export(user, "SetFocus"));
        CHECK(obfh_gui_slots[47].ready);
    }
    {
        const unsigned char bytes[] = OBFH_GUI_NAME_SetWindowPos;
        SetLastError(0x12345678u);
        encoded = obfh_gui_cold(0, 48, bytes, sizeof bytes);
        CHECK(GetLastError() == 0x12345678u);
        rotate = OBFH_GUI_ROTATE(48);
        decoded = encoded - OBFH_GUI_BIAS(48);
        decoded = ((decoded >> rotate) | (decoded << (sizeof(ULONG_PTR) * 8 - rotate))) ^ OBFH_GUI_KEY(48);
        CHECK(decoded == (ULONG_PTR)native_export(user, "SetWindowPos"));
        CHECK(obfh_gui_slots[48].ready);
    }
    {
        const unsigned char bytes[] = OBFH_GUI_NAME_SetWindowTextA;
        SetLastError(0x12345678u);
        encoded = obfh_gui_cold(0, 49, bytes, sizeof bytes);
        CHECK(GetLastError() == 0x12345678u);
        rotate = OBFH_GUI_ROTATE(49);
        decoded = encoded - OBFH_GUI_BIAS(49);
        decoded = ((decoded >> rotate) | (decoded << (sizeof(ULONG_PTR) * 8 - rotate))) ^ OBFH_GUI_KEY(49);
        CHECK(decoded == (ULONG_PTR)native_export(user, "SetWindowTextA"));
        CHECK(obfh_gui_slots[49].ready);
    }
    {
        const unsigned char bytes[] = OBFH_GUI_NAME_SetWindowTextW;
        SetLastError(0x12345678u);
        encoded = obfh_gui_cold(0, 50, bytes, sizeof bytes);
        CHECK(GetLastError() == 0x12345678u);
        rotate = OBFH_GUI_ROTATE(50);
        decoded = encoded - OBFH_GUI_BIAS(50);
        decoded = ((decoded >> rotate) | (decoded << (sizeof(ULONG_PTR) * 8 - rotate))) ^ OBFH_GUI_KEY(50);
        CHECK(decoded == (ULONG_PTR)native_export(user, "SetWindowTextW"));
        CHECK(obfh_gui_slots[50].ready);
    }
    {
        const unsigned char bytes[] = OBFH_GUI_NAME_ShowWindow;
        SetLastError(0x12345678u);
        encoded = obfh_gui_cold(0, 51, bytes, sizeof bytes);
        CHECK(GetLastError() == 0x12345678u);
        rotate = OBFH_GUI_ROTATE(51);
        decoded = encoded - OBFH_GUI_BIAS(51);
        decoded = ((decoded >> rotate) | (decoded << (sizeof(ULONG_PTR) * 8 - rotate))) ^ OBFH_GUI_KEY(51);
        CHECK(decoded == (ULONG_PTR)native_export(user, "ShowWindow"));
        CHECK(obfh_gui_slots[51].ready);
    }
    {
        const unsigned char bytes[] = OBFH_GUI_NAME_TranslateMessage;
        SetLastError(0x12345678u);
        encoded = obfh_gui_cold(0, 52, bytes, sizeof bytes);
        CHECK(GetLastError() == 0x12345678u);
        rotate = OBFH_GUI_ROTATE(52);
        decoded = encoded - OBFH_GUI_BIAS(52);
        decoded = ((decoded >> rotate) | (decoded << (sizeof(ULONG_PTR) * 8 - rotate))) ^ OBFH_GUI_KEY(52);
        CHECK(decoded == (ULONG_PTR)native_export(user, "TranslateMessage"));
        CHECK(obfh_gui_slots[52].ready);
    }
    {
        const unsigned char bytes[] = OBFH_GUI_NAME_UnregisterClassA;
        SetLastError(0x12345678u);
        encoded = obfh_gui_cold(0, 53, bytes, sizeof bytes);
        CHECK(GetLastError() == 0x12345678u);
        rotate = OBFH_GUI_ROTATE(53);
        decoded = encoded - OBFH_GUI_BIAS(53);
        decoded = ((decoded >> rotate) | (decoded << (sizeof(ULONG_PTR) * 8 - rotate))) ^ OBFH_GUI_KEY(53);
        CHECK(decoded == (ULONG_PTR)native_export(user, "UnregisterClassA"));
        CHECK(obfh_gui_slots[53].ready);
    }
    {
        const unsigned char bytes[] = OBFH_GUI_NAME_UnregisterClassW;
        SetLastError(0x12345678u);
        encoded = obfh_gui_cold(0, 54, bytes, sizeof bytes);
        CHECK(GetLastError() == 0x12345678u);
        rotate = OBFH_GUI_ROTATE(54);
        decoded = encoded - OBFH_GUI_BIAS(54);
        decoded = ((decoded >> rotate) | (decoded << (sizeof(ULONG_PTR) * 8 - rotate))) ^ OBFH_GUI_KEY(54);
        CHECK(decoded == (ULONG_PTR)native_export(user, "UnregisterClassW"));
        CHECK(obfh_gui_slots[54].ready);
    }
    {
        const unsigned char bytes[] = OBFH_GUI_NAME_UpdateWindow;
        SetLastError(0x12345678u);
        encoded = obfh_gui_cold(0, 55, bytes, sizeof bytes);
        CHECK(GetLastError() == 0x12345678u);
        rotate = OBFH_GUI_ROTATE(55);
        decoded = encoded - OBFH_GUI_BIAS(55);
        decoded = ((decoded >> rotate) | (decoded << (sizeof(ULONG_PTR) * 8 - rotate))) ^ OBFH_GUI_KEY(55);
        CHECK(decoded == (ULONG_PTR)native_export(user, "UpdateWindow"));
        CHECK(obfh_gui_slots[55].ready);
    }
    {
        const unsigned char bytes[] = OBFH_GUI_NAME_CreateFontA;
        SetLastError(0x12345678u);
        encoded = obfh_gui_cold(1, 56, bytes, sizeof bytes);
        CHECK(GetLastError() == 0x12345678u);
        rotate = OBFH_GUI_ROTATE(56);
        decoded = encoded - OBFH_GUI_BIAS(56);
        decoded = ((decoded >> rotate) | (decoded << (sizeof(ULONG_PTR) * 8 - rotate))) ^ OBFH_GUI_KEY(56);
        CHECK(decoded == (ULONG_PTR)native_export(gdi, "CreateFontA"));
        CHECK(obfh_gui_slots[56].ready);
    }
    {
        const unsigned char bytes[] = OBFH_GUI_NAME_CreateFontW;
        SetLastError(0x12345678u);
        encoded = obfh_gui_cold(1, 57, bytes, sizeof bytes);
        CHECK(GetLastError() == 0x12345678u);
        rotate = OBFH_GUI_ROTATE(57);
        decoded = encoded - OBFH_GUI_BIAS(57);
        decoded = ((decoded >> rotate) | (decoded << (sizeof(ULONG_PTR) * 8 - rotate))) ^ OBFH_GUI_KEY(57);
        CHECK(decoded == (ULONG_PTR)native_export(gdi, "CreateFontW"));
        CHECK(obfh_gui_slots[57].ready);
    }
    {
        const unsigned char bytes[] = OBFH_GUI_NAME_CreateFontIndirectA;
        SetLastError(0x12345678u);
        encoded = obfh_gui_cold(1, 58, bytes, sizeof bytes);
        CHECK(GetLastError() == 0x12345678u);
        rotate = OBFH_GUI_ROTATE(58);
        decoded = encoded - OBFH_GUI_BIAS(58);
        decoded = ((decoded >> rotate) | (decoded << (sizeof(ULONG_PTR) * 8 - rotate))) ^ OBFH_GUI_KEY(58);
        CHECK(decoded == (ULONG_PTR)native_export(gdi, "CreateFontIndirectA"));
        CHECK(obfh_gui_slots[58].ready);
    }
    {
        const unsigned char bytes[] = OBFH_GUI_NAME_CreateFontIndirectW;
        SetLastError(0x12345678u);
        encoded = obfh_gui_cold(1, 59, bytes, sizeof bytes);
        CHECK(GetLastError() == 0x12345678u);
        rotate = OBFH_GUI_ROTATE(59);
        decoded = encoded - OBFH_GUI_BIAS(59);
        decoded = ((decoded >> rotate) | (decoded << (sizeof(ULONG_PTR) * 8 - rotate))) ^ OBFH_GUI_KEY(59);
        CHECK(decoded == (ULONG_PTR)native_export(gdi, "CreateFontIndirectW"));
        CHECK(obfh_gui_slots[59].ready);
    }
    {
        const unsigned char bytes[] = OBFH_GUI_NAME_DeleteObject;
        SetLastError(0x12345678u);
        encoded = obfh_gui_cold(1, 60, bytes, sizeof bytes);
        CHECK(GetLastError() == 0x12345678u);
        rotate = OBFH_GUI_ROTATE(60);
        decoded = encoded - OBFH_GUI_BIAS(60);
        decoded = ((decoded >> rotate) | (decoded << (sizeof(ULONG_PTR) * 8 - rotate))) ^ OBFH_GUI_KEY(60);
        CHECK(decoded == (ULONG_PTR)native_export(gdi, "DeleteObject"));
        CHECK(obfh_gui_slots[60].ready);
    }
    {
        const unsigned char bytes[] = OBFH_GUI_NAME_GetDeviceCaps;
        SetLastError(0x12345678u);
        encoded = obfh_gui_cold(1, 61, bytes, sizeof bytes);
        CHECK(GetLastError() == 0x12345678u);
        rotate = OBFH_GUI_ROTATE(61);
        decoded = encoded - OBFH_GUI_BIAS(61);
        decoded = ((decoded >> rotate) | (decoded << (sizeof(ULONG_PTR) * 8 - rotate))) ^ OBFH_GUI_KEY(61);
        CHECK(decoded == (ULONG_PTR)native_export(gdi, "GetDeviceCaps"));
        CHECK(obfh_gui_slots[61].ready);
    }
    {
        const unsigned char bytes[] = OBFH_GUI_NAME_SelectObject;
        SetLastError(0x12345678u);
        encoded = obfh_gui_cold(1, 62, bytes, sizeof bytes);
        CHECK(GetLastError() == 0x12345678u);
        rotate = OBFH_GUI_ROTATE(62);
        decoded = encoded - OBFH_GUI_BIAS(62);
        decoded = ((decoded >> rotate) | (decoded << (sizeof(ULONG_PTR) * 8 - rotate))) ^ OBFH_GUI_KEY(62);
        CHECK(decoded == (ULONG_PTR)native_export(gdi, "SelectObject"));
        CHECK(obfh_gui_slots[62].ready);
    }
    {
        const unsigned char bytes[] = OBFH_GUI_NAME_SetBkColor;
        SetLastError(0x12345678u);
        encoded = obfh_gui_cold(1, 63, bytes, sizeof bytes);
        CHECK(GetLastError() == 0x12345678u);
        rotate = OBFH_GUI_ROTATE(63);
        decoded = encoded - OBFH_GUI_BIAS(63);
        decoded = ((decoded >> rotate) | (decoded << (sizeof(ULONG_PTR) * 8 - rotate))) ^ OBFH_GUI_KEY(63);
        CHECK(decoded == (ULONG_PTR)native_export(gdi, "SetBkColor"));
        CHECK(obfh_gui_slots[63].ready);
    }
    {
        const unsigned char bytes[] = OBFH_GUI_NAME_SetTextColor;
        SetLastError(0x12345678u);
        encoded = obfh_gui_cold(1, 64, bytes, sizeof bytes);
        CHECK(GetLastError() == 0x12345678u);
        rotate = OBFH_GUI_ROTATE(64);
        decoded = encoded - OBFH_GUI_BIAS(64);
        decoded = ((decoded >> rotate) | (decoded << (sizeof(ULONG_PTR) * 8 - rotate))) ^ OBFH_GUI_KEY(64);
        CHECK(decoded == (ULONG_PTR)native_export(gdi, "SetTextColor"));
        CHECK(obfh_gui_slots[64].ready);
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
