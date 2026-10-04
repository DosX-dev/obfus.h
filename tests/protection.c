#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <windows.h>
static int reference_printf(const char *format, ...) {
    va_list args;
    va_start(args, format);
    int result = vprintf(format, args);
    va_end(args);
    return result;
}
#include "../include/obfus.h"
#define CHECK(x)                                                  \
    do {                                                          \
        if (!(x)) {                                               \
            fprintf(stderr, "protection check failed: %s\n", #x); \
            return 1;                                             \
        }                                                         \
    } while (0)
#if !NO_OBF
static int shifted(int shift) { RET_BY_VAR(shift); }
static double addressed(double address) { RET_BY_VAR(address); }
#endif
int main(void) {
    double fractional = 0.25;
    int calls = 0;
    if (fractional) ++calls;
    CHECK(calls == 1);
    if (0.0) ++calls;
    CHECK(calls == 1);
    int *pointer = &calls;
    if (pointer) ++calls;
    CHECK(calls == 2);
#if !NO_OBF
#if NO_CFLOW != 1
    CHECK(OBFUS_CONDITION_BLOCK(0.25));
    CHECK(!OBFUS_CONDITION_BLOCK(0.0));
    CHECK(OBFUS_CONDITION_BLOCK(pointer));
#endif
    CHECK(obfh_double_proxy(3.25) == 3.25);
    CHECK(shifted(31) == 31 && addressed(3.25) == 3.25);
    char mask[32], name[11];
    CHECK(getCharMask(6, mask, sizeof mask) == mask);
    CHECK(strlen(mask) == 12);
    CHECK(strcmp(getStdLibName_proxy(name, sizeof name), "msvcrt.dll") == 0);
    HANDLE originalOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    FreeConsole();
    CHECK(AllocConsole());
    ShowWindow(GetConsoleWindow(), SW_HIDE);
    HANDLE screen = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
    CHECK(screen != INVALID_HANDLE_VALUE);
    COORD size = {256, 256}, origin = {0, 0};
    CHECK(SetConsoleScreenBufferSize(screen, size));
    CHECK(SetStdHandle(STD_OUTPUT_HANDLE, screen));
    char text[4097], readback[4097];
    memset(text, 'Q', 4096);
    text[4096] = 0;
    CHECK(printf("%s", text) == 4096);
    DWORD readCount;
    CHECK(ReadConsoleOutputCharacterA(screen, readback, 4096, origin, &readCount));
    CHECK(readCount == 4096);
    for (int i = 0; i < 4096; ++i) CHECK(readback[i] == 'Q');
    int (*enableCount)(int) = (int (*)(int))GetProcAddress(GetModuleHandleA("msvcrt.dll"), "_set_printf_count_output");
    int previousCountMode = enableCount ? enableCount(1) : 0;
    union {
        int count;
        char text[16];
    } alias;
    strcpy(alias.text, "abcdef");
    int expectedResult = reference_printf("%s%n", alias.text, &alias.count);
    int expectedCount = alias.count;
    strcpy(alias.text, "abcdef");
    CHECK(printf("%s%n", alias.text, &alias.count) == expectedResult);
    CHECK(alias.count == expectedCount);
    short shortCount = -1;
    signed char byteCount = -1;
    long long wideCount = -1;
    expectedResult = reference_printf("abc%hn%hhn%lln", &shortCount, &byteCount, &wideCount);
    short expectedShort = shortCount;
    signed char expectedByte = byteCount;
    long long expectedWide = wideCount;
    shortCount = -1;
    byteCount = -1;
    wideCount = -1;
    CHECK(printf("abc%hn%hhn%lln", &shortCount, &byteCount, &wideCount) == expectedResult);
    CHECK(shortCount == expectedShort && byteCount == expectedByte && wideCount == expectedWide);
    if (enableCount) enableCount(previousCountMode);
    CHECK(!obfh_format_has_count("literal n: %%n %s %d"));
    CHECK(obfh_format_has_count("%n") && obfh_format_has_count("%hn") && obfh_format_has_count("%lln"));
    CHECK(SetStdHandle(STD_OUTPUT_HANDLE, originalOutput));
    CHECK(CloseHandle(screen));
    CHECK(FreeConsole());
    CHECK(SetStdHandle(STD_OUTPUT_HANDLE, originalOutput));
    DWORD before, after;
    CHECK(GetProcessHandleCount(GetCurrentProcess(), &before));
    for (int i = 0; i < 200; ++i) {
        HMODULE module = LoadLibraryA("kernel32.dll");
        CHECK(module);
        CHECK(GetProcAddress(module, "GetCurrentProcessId"));
        CHECK(FreeLibrary(module));
    }
    CHECK(GetProcessHandleCount(GetCurrentProcess(), &after));
    CHECK(after <= before + 2);
#endif
    puts("PROTECTION_PASS");
    return 0;
}
