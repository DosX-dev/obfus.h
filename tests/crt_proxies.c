#include <ctype.h>
#include <direct.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
static int original_system_available(void) { return system(NULL); }
#include "../include/obfus.h"
#define CHECK(value)            \
    do {                        \
        if (!(value)) return 1; \
    } while (0)
static void on_exit(void) { puts("CRT_ATEXIT_PASS"); }
static int format_args(char *out, const char *format, ...) {
    va_list args;
    va_start(args, format);
    int result = vsprintf(out, format, args);
    va_end(args);
    return result;
}
static int bounded_args(char *out, size_t size, const char *format, ...) {
    va_list args;
    va_start(args, format);
    int result = vsnprintf(out, size, format, args);
    va_end(args);
    return result;
}
int main(int argc, char **argv) {
    if (argc > 1 && !strcmp(argv[1], "gets")) {
        char line[128];
        CHECK(gets(line) == line && !strcmp(line, "proxy-input"));
        puts("CRT_GETS_PASS");
        return 0;
    }
    char cwd[4096], text[128];
    CHECK(getcwd(cwd, sizeof cwd) == cwd && cwd[0]);
    CHECK(getenv("PATH") && getenv("OBFH_MISSING_ENV_973242") == NULL);
    CHECK((system(NULL) != 0) == (original_system_available() != 0));
    for (int c = -1; c < 256; ++c) {
        CHECK(tolower(c) == (c >= 'A' && c <= 'Z' ? c + 32 : c));
        CHECK(toupper(c) == (c >= 'a' && c <= 'z' ? c - 32 : c));
    }
    unsigned char *memory = calloc(127, 1);
    CHECK(memory);
    for (int i = 0; i < 127; ++i) CHECK(memory[i] == 0);
    CHECK(memset(memory, 0xa5, 127) == memory);
    memory = realloc(memory, 257);
    CHECK(memory);
    for (int i = 0; i < 127; ++i) CHECK(memory[i] == 0xa5);
    free(memory);
    CHECK(format_args(text, "%s:%d:%.2f", "value", -17, 1.25) == 14);
    CHECK(!strcmp(text, "value:-17:1.25"));
    CHECK(bounded_args(text, sizeof text, "%s:%d", "bounded", 12) == 10);
    CHECK(!strcmp(text, "bounded:12"));
    CHECK(snprintf(text, sizeof text, "%d", 71) == 2 && !strcmp(text, "71"));
    CHECK(atexit(on_exit) == 0);
    puts("CRT_PROXIES_PASS");
    return 0;
}
