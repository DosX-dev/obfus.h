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
static DWORD native_thread_id(void) { return GetCurrentThreadId(); }
static DWORD native_error(void) { return GetLastError(); }
static int native_muldiv(int a, int b, int c) { return MulDiv(a, b, c); }
static HMODULE native_module(const char *n) { return GetModuleHandleA(n); }
static LPSTR native_command(void) { return GetCommandLineA(); }
static BOOL native_console(HANDLE h, DWORD *m) { return GetConsoleMode(h, m); }
static BOOL native_write(HANDLE h, DWORD *n) { return WriteConsoleA(h, "x", 1, n, NULL); }
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
            fprintf(stderr, "KERNEL_FAIL:%d: %s\n", __LINE__, #x); \
            return 1; \
        } \
    } while (0)
#define ADDRESS(index, api) \
    do { \
        unsigned char name[64]; \
        size_t length = OBFH_GUI_NAME_##api(name); \
        SetLastError(0x12345678u); \
        ULONG_PTR value = obfh_gui_cold(2, &test_gui_slots[index], index, name, length); \
        CHECK(GetLastError() == 0x12345678u); \
        unsigned r = OBFH_GUI_ROTATE(index); \
        value -= OBFH_GUI_BIAS(index); \
        value = ((value >> r) | (value << (sizeof(ULONG_PTR) * 8 - r))) ^ OBFH_GUI_KEY(index); \
        CHECK(value == (ULONG_PTR)native_export(kernel, #api)); \
    } while (0)
int main(int argc, char **argv) {
    if (argc > 1 && argv[1][0] == 'x') ExitProcess(23);
    DWORD thread = native_thread_id();
    SetLastError(0x13572468u);
    CHECK(GetCurrentThreadId() == thread);
    CHECK(native_error() == 0x13572468u);
    CHECK(GetCurrentThreadId() == thread);
    CHECK(native_error() == 0x13572468u);
    // The first protected LastError call itself resolves lazily and preserves TLS.
    SetLastError(0xA5B61234u);
    CHECK(GetLastError() == 0xA5B61234u);
    CHECK(native_error() == 0xA5B61234u);
    SetLastError(0x01020304u);
    CHECK(GetLastError() == 0x01020304u);
#if NO_OBF != 1
    HMODULE kernel = native_module("kernel32.dll");
    CHECK(kernel);
    ADDRESS(136, GetCurrentThreadId);
    ADDRESS(65, ExitProcess);
    ADDRESS(66, GetLastError);
    ADDRESS(67, FreeLibrary);
    ADDRESS(68, SetLastError);
    ADDRESS(69, WriteConsoleA);
    ADDRESS(70, GetStdHandle);
    ADDRESS(71, GetModuleHandleA);
    ADDRESS(72, GetModuleHandleExA);
    ADDRESS(73, VirtualQuery);
    ADDRESS(74, GetConsoleMode);
    ADDRESS(75, MulDiv);
    ADDRESS(76, GlobalAlloc);
    ADDRESS(77, GlobalLock);
    ADDRESS(78, GlobalFree);
    ADDRESS(79, GlobalUnlock);
    ADDRESS(80, GetStartupInfoA);
    ADDRESS(81, GetCommandLineA);
    ADDRESS(82, WriteConsoleW);
    ADDRESS(83, GetModuleHandleW);
    ADDRESS(84, GetModuleHandleExW);
    ADDRESS(85, GetStartupInfoW);
    ADDRESS(86, GetCommandLineW);
#endif
    for (int a = -100; a <= 100; a += 10)
        for (int b = -11; b <= 11; b += 2)
            for (int d = -7; d <= 7; d += 2) CHECK(MulDiv(a, b, d) == native_muldiv(a, b, d));
    CHECK(MulDiv(2147483647, 2147483647, 1) == native_muldiv(2147483647, 2147483647, 1));
    CHECK(MulDiv(1, 2, 0) == native_muldiv(1, 2, 0));
    int calls = 0;
    CHECK(MulDiv(++calls, 2, 1) == 2 && calls == 1);
    CHECK(GetModuleHandleA(NULL) == native_module(NULL));
    CHECK(GetModuleHandleW(NULL) == native_module(NULL));
    HMODULE module = NULL;
    CHECK(GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, "kernel32.dll", &module));
    CHECK(module == native_module("kernel32.dll"));
    CHECK(GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, L"kernel32.dll", &module));
    HMODULE loaded = LoadLibraryA("kernel32.dll");
    CHECK(loaded && FreeLibrary(loaded));
    CHECK(!strcmp(GetCommandLineA(), native_command()));
    CHECK(GetCommandLineW() != NULL);
    CHECK(GetCommandLine() != NULL);
    STARTUPINFOA startup = {0};
    GetStartupInfoA(&startup);
    CHECK(startup.cb == sizeof(startup));
    STARTUPINFOW wide = {0};
    GetStartupInfoW(&wide);
    CHECK(wide.cb == sizeof(wide));
    STARTUPINFO generic = {0};
    GetStartupInfo(&generic);
    CHECK(generic.cb == sizeof(generic));
    MEMORY_BASIC_INFORMATION page;
    CHECK(VirtualQuery(&page, &page, sizeof page) == sizeof page);
    CHECK(page.State == MEM_COMMIT);
    CHECK(GetStdHandle(STD_OUTPUT_HANDLE) == (HANDLE)(LONG_PTR)GetStdHandle(STD_OUTPUT_HANDLE));
    DWORD mode = 0, count = 0;
    SetLastError(0);
    BOOL result = GetConsoleMode(INVALID_HANDLE_VALUE, &mode);
    DWORD error = GetLastError();
    SetLastError(0);
    CHECK(result == native_console(INVALID_HANDLE_VALUE, &mode));
    CHECK(error == native_error());
    SetLastError(0);
    result = WriteConsoleA(INVALID_HANDLE_VALUE, "x", 1, &count, NULL);
    error = GetLastError();
    SetLastError(0);
    CHECK(result == native_write(INVALID_HANDLE_VALUE, &count));
    CHECK(error == native_error());
    CHECK(!WriteConsoleW(INVALID_HANDLE_VALUE, L"x", 1, &count, NULL));
    for (int i = 0; i < 100; ++i) {
        HGLOBAL h = GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, 512);
        CHECK(h);
        unsigned char *p = GlobalLock(h);
        CHECK(p);
        for (int j = 0; j < 512; ++j) CHECK(!p[j]);
        p[511] = 123;
        SetLastError(0);
        CHECK(!GlobalUnlock(h));
        CHECK(GetLastError() == 0);
        CHECK(GlobalFree(h) == NULL);
    }
    CHECK(__builtin_types_compatible_p(__typeof__(VirtualQuery(NULL, NULL, 0)), SIZE_T));
    CHECK(__builtin_types_compatible_p(__typeof__(GetCommandLineW()), LPWSTR));
    CHECK(__builtin_types_compatible_p(__typeof__(GetModuleHandleExA(0, NULL, NULL)), BOOL));
    CHECK(__builtin_types_compatible_p(__typeof__(ExitProcess(0)), void));
    puts("KERNEL_CALLS_PASS");
    return 0;
}
