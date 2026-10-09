#include <ctype.h>
#include <direct.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
static void *original_memchr(const void *a, int value, size_t n) { return memchr(a, value, n); }
static size_t original_strspn(const char *a, const char *b) { return strspn(a, b); }
static size_t original_strcspn(const char *a, const char *b) { return strcspn(a, b); }
static char *original_strpbrk(const char *a, const char *b) { return strpbrk(a, b); }
static void original_perror(const char *message) { perror(message); }
static int original_strcmp(const char *a, const char *b) { return strcmp(a, b); }
static size_t original_strlen(const char *a) { return strlen(a); }
static int original_memcmp(const void *a, const void *b, size_t n) { return memcmp(a, b, n); }
static int original_strncmp(const char *a, const char *b, size_t n) { return strncmp(a, b, n); }
static char *original_strstr(const char *a, const char *b) { return strstr(a, b); }
static int original_system_available(void) { return system(NULL); }
#include "../include/obfus.h"
#define CHECK(value) \
    do { \
        if (!(value)) { \
            fprintf(stderr, "CRT check failed: %d\n", __LINE__); \
            return 1; \
        } \
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
static int sign_of(int value) { return (value > 0) - (value < 0); }
static int local_string_kernels(void) {
    unsigned int state = 1234567u;
    unsigned char a[64], b[64];
    for (int trial = 0; trial < 2048; ++trial) {
        for (int i = 0; i < 64; ++i) {
            state = state * 1664525u + 1013904223u;
            a[i] = (unsigned char)(state >> 24);
            b[i] = (trial & 1) ? a[i] : (unsigned char)(state >> 16);
        }
        size_t n = (size_t)trial % 65;
        CHECK(sign_of(memcmp(a, b, n)) == sign_of(original_memcmp(a, b, n)));
        CHECK(memchr(a, trial - 1024, n) == original_memchr(a, trial - 1024, n));
        a[trial % 64] = 0;
        b[(trial * 3) % 64] = 0;
        a[63] = b[63] = 0;
        CHECK(sign_of(strncmp((char *)a, (char *)b, n)) == sign_of(original_strncmp((char *)a, (char *)b, n)));
        CHECK(sign_of(strcmp((char *)a, (char *)b)) == sign_of(original_strcmp((char *)a, (char *)b)));
        CHECK(strlen((char *)a) == original_strlen((char *)a));
        CHECK(strspn((char *)a, (char *)b) == original_strspn((char *)a, (char *)b));
        CHECK(strcspn((char *)a, (char *)b) == original_strcspn((char *)a, (char *)b));
        CHECK(strpbrk((char *)a, (char *)b) == original_strpbrk((char *)a, (char *)b));
        const char *pattern = (trial & 1) ? (char *)a + trial % 64 : (char *)b;
        CHECK(strstr((char *)a, pattern) == original_strstr((char *)a, pattern));
    }
    char *pages = VirtualAlloc(NULL, 8192, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    CHECK(pages);
    DWORD old;
    CHECK(VirtualProtect(pages + 4096, 4096, PAGE_NOACCESS, &old));
    char *last = pages + 4095;
    *last = 0;
    CHECK(memchr(last + 1, 0, 0) == NULL);
    CHECK(memchr(last, 256, 1) == last);
    CHECK(strspn(last, "x") == 0 && strcspn(last, "x") == 0 && strpbrk(last, "x") == NULL);
    CHECK(strspn("abc", last) == 0 && strcspn("abc", last) == 3 && strpbrk("abc", last) == NULL);
    CHECK(memcmp(last + 1, last + 1, 0) == 0);
    CHECK(strncmp(last + 1, last + 1, 0) == 0);
    CHECK(memcmp(last, last, 1) == 0 && strncmp(last, "", 64) == 0);
    CHECK(strstr(last, "") == last && strstr(last, "x") == NULL);
    last[-1] = 'x';
    CHECK(strstr(last - 1, "x") == last - 1);
    CHECK(strstr(last - 1, "xx") == NULL);
    CHECK(VirtualFree(pages, 0, MEM_RELEASE));
    int calls = 0;
    CHECK(memchr((++calls, a), 0, 0) == NULL && calls == 1);
    CHECK(strspn((++calls, "abc"), "ac") == 1 && calls == 2);
    CHECK(strcspn((++calls, "abc"), "c") == 2 && calls == 3);
    CHECK(strpbrk((++calls, "abc"), "c")[0] == 'c' && calls == 4);
    CHECK(strcmp((++calls, "abc"), "abc") == 0 && calls == 5);
    CHECK(strlen((++calls, "abc")) == 3 && calls == 6);
    CHECK(memcmp((unsigned char[]){0, 128, 255}, (unsigned char[]){0, 128, 255}, 3) == 0);
    CHECK(memchr((unsigned char[]){0, 128, 255}, 255, 3) != NULL);
    char nested[] = "abcabc";
    CHECK(strstr(strstr(nested, "bc"), "ca") == nested + 2);
    CHECK(memchr(memchr(nested, 'b', 6), 'c', 5) == nested + 2);
    for (int i = 0; i < 4; ++i) {
        if (memcmp(nested, "abc", 3) == 0)
            if (strlen(nested) == 6)
                continue;
            else
                return 1;
        return 1;
    }
    calls = 0;
    CHECK(memcmp((++calls, a), b, 0) == 0 && calls == 1);
    CHECK(strncmp((++calls, "a"), "a", 1) == 0 && calls == 2);
    CHECK(strstr((++calls, "abc"), "b")[0] == 'b' && calls == 3);
    return 0;
}
static int crt_name_builders(void) {
#if !NO_OBF
    char name[32];
    memset(name, 0xa5, sizeof name);
    CHECK(getKernel32Name_proxy(name) == name && !strcmp(name, "kernel32"));
    CHECK((unsigned char)name[9] == 0xa5);
    memset(name, 0xa5, sizeof name);
    CHECK(getUser32Name_proxy(name) == name && !strcmp(name, "user32"));
    CHECK((unsigned char)name[7] == 0xa5);
    memset(name, 0xa5, sizeof name);
    CHECK(getGdi32Name_proxy(name) == name && !strcmp(name, "gdi32"));
    CHECK((unsigned char)name[6] == 0xa5);
    memset(name, 0xa5, sizeof name);
    CHECK(getAdvapi32Name_proxy(name) == name && !strcmp(name, "advapi32"));
    CHECK((unsigned char)name[9] == 0xa5);
    memset(name, 0xa5, sizeof name);
    CHECK(getLoaderName_proxy(name) == name && !strcmp(name, "LoadLibraryA"));
    CHECK((unsigned char)name[13] == 0xa5);
    memset(name, 0xa5, sizeof name);
    CHECK(getDebuggerName_proxy(name) == name && !strcmp(name, "IsDebuggerPresent"));
    CHECK((unsigned char)name[18] == 0xa5);
    memset(name, 0xa5, sizeof name);
    CHECK(getScanfName_proxy(name) == name && !strcmp(name, "scanf"));
    CHECK((unsigned char)name[6] == 0xa5);
    memset(name, 0xa5, sizeof name);
    CHECK(getFreeName_proxy(name) == name && !strcmp(name, "free"));
    CHECK((unsigned char)name[5] == 0xa5);
    memset(name, 0xa5, sizeof name);
    CHECK(getAtofName_proxy(name) == name && !strcmp(name, "atof"));
    CHECK((unsigned char)name[5] == 0xa5);
    memset(name, 0xa5, sizeof name);
    CHECK(getStrtodName_proxy(name) == name && !strcmp(name, "strtod"));
    CHECK((unsigned char)name[7] == 0xa5);
    memset(name, 0xa5, sizeof name);
    CHECK(getSrandName_proxy(name) == name && !strcmp(name, "srand"));
    CHECK((unsigned char)name[6] == 0xa5);
    memset(name, 0xa5, sizeof name);
    CHECK(getFgetsName_proxy(name) == name && !strcmp(name, "fgets"));
    CHECK((unsigned char)name[6] == 0xa5);
    memset(name, 0xa5, sizeof name);
    CHECK(getFputsName_proxy(name) == name && !strcmp(name, "fputs"));
    CHECK((unsigned char)name[6] == 0xa5);
    memset(name, 0xa5, sizeof name);
    CHECK(getFprintfName_proxy(name) == name && !strcmp(name, "fprintf"));
    CHECK((unsigned char)name[8] == 0xa5);
    memset(name, 0xa5, sizeof name);
    CHECK(getFflushName_proxy(name) == name && !strcmp(name, "fflush"));
    CHECK((unsigned char)name[7] == 0xa5);
    memset(name, 0xa5, sizeof name);
    CHECK(getFseekName_proxy(name) == name && !strcmp(name, "fseek"));
    CHECK((unsigned char)name[6] == 0xa5);
    memset(name, 0xa5, sizeof name);
    CHECK(getFtellName_proxy(name) == name && !strcmp(name, "ftell"));
    CHECK((unsigned char)name[6] == 0xa5);
    memset(name, 0xa5, sizeof name);
    CHECK(getFgetcName_proxy(name) == name && !strcmp(name, "fgetc"));
    CHECK((unsigned char)name[6] == 0xa5);
    memset(name, 0xa5, sizeof name);
    CHECK(getFputcName_proxy(name) == name && !strcmp(name, "fputc"));
    CHECK((unsigned char)name[6] == 0xa5);
    memset(name, 0xa5, sizeof name);
    CHECK(getGetcharName_proxy(name) == name && !strcmp(name, "getchar"));
    CHECK((unsigned char)name[8] == 0xa5);
    memset(name, 0xa5, sizeof name);
    CHECK(getPutcharName_proxy(name) == name && !strcmp(name, "putchar"));
    CHECK((unsigned char)name[8] == 0xa5);
    memset(name, 0xa5, sizeof name);
    CHECK(getFeofName_proxy(name) == name && !strcmp(name, "feof"));
    CHECK((unsigned char)name[5] == 0xa5);
    memset(name, 0xa5, sizeof name);
    CHECK(getFerrorName_proxy(name) == name && !strcmp(name, "ferror"));
    CHECK((unsigned char)name[7] == 0xa5);
    memset(name, 0xa5, sizeof name);
    CHECK(getClearerrName_proxy(name) == name && !strcmp(name, "clearerr"));
    CHECK((unsigned char)name[9] == 0xa5);
    memset(name, 0xa5, sizeof name);
    CHECK(getRewindName_proxy(name) == name && !strcmp(name, "rewind"));
    CHECK((unsigned char)name[7] == 0xa5);
    memset(name, 0xa5, sizeof name);
    CHECK(getRemoveName_proxy(name) == name && !strcmp(name, "remove"));
    CHECK((unsigned char)name[7] == 0xa5);
    memset(name, 0xa5, sizeof name);
    CHECK(getRenameName_proxy(name) == name && !strcmp(name, "rename"));
    CHECK((unsigned char)name[7] == 0xa5);
    memset(name, 0xa5, sizeof name);
    CHECK(getFreopenName_proxy(name) == name && !strcmp(name, "freopen"));
    CHECK((unsigned char)name[8] == 0xa5);
    memset(name, 0xa5, sizeof name);
    CHECK(getQsortName_proxy(name) == name && !strcmp(name, "qsort"));
    CHECK((unsigned char)name[6] == 0xa5);
    memset(name, 0xa5, sizeof name);
    CHECK(getSprintfName_proxy(name) == name && !strcmp(name, "sprintf"));
    CHECK((unsigned char)name[8] == 0xa5);
    memset(name, 0xa5, sizeof name);
    CHECK(getFcloseName_proxy(name) == name && !strcmp(name, "fclose"));
    CHECK((unsigned char)name[7] == 0xa5);
    memset(name, 0xa5, sizeof name);
    CHECK(getFopenName_proxy(name) == name && !strcmp(name, "fopen"));
    CHECK((unsigned char)name[6] == 0xa5);
    memset(name, 0xa5, sizeof name);
    CHECK(getFreadName_proxy(name) == name && !strcmp(name, "fread"));
    CHECK((unsigned char)name[6] == 0xa5);
    memset(name, 0xa5, sizeof name);
    CHECK(getFwriteName_proxy(name) == name && !strcmp(name, "fwrite"));
    CHECK((unsigned char)name[7] == 0xa5);
    memset(name, 0xa5, sizeof name);
    CHECK(getExitName_proxy(name) == name && !strcmp(name, "exit"));
    CHECK((unsigned char)name[5] == 0xa5);
    memset(name, 0xa5, sizeof name);
    CHECK(getStrtokName_proxy(name) == name && !strcmp(name, "strtok"));
    CHECK((unsigned char)name[7] == 0xa5);
    memset(name, 0xa5, sizeof name);
    CHECK(getRandName_proxy(name) == name && !strcmp(name, "rand"));
    CHECK((unsigned char)name[5] == 0xa5);
    memset(name, 0xa5, sizeof name);
    CHECK(getReallocName_proxy(name) == name && !strcmp(name, "realloc"));
    CHECK((unsigned char)name[8] == 0xa5);
    memset(name, 0xa5, sizeof name);
    CHECK(getStdLibName_proxy(name, 6) == NULL && (unsigned char)name[0] == 0xa5);
    CHECK(getStdLibName_proxy(name, 7) == name && !strcmp(name, "msvcrt"));
    CHECK((unsigned char)name[7] == 0xa5);
#endif
    return 0;
}
static int common_crt_calls(void) {
    int calls = 0;
    char text[64], copy[8], *end;
    const unsigned char low[] = {0, 1, 127, 255}, high[] = {0, 1, 128, 0};
    CHECK(memcmp(low, high, sizeof low) < 0);
    CHECK(memcmp(low, high, 0) == 0);
    CHECK(strncmp("abcX", "abcY", 3) == 0 && strncmp("a", "b", 1) < 0);
    CHECK(strstr("prefix:needle", "needle") != NULL && strstr("abc", "z") == NULL);
    CHECK(strstr("abc", "")[0] == 'a');
    CHECK(strncpy(copy, "hi", sizeof copy) == copy && copy[2] == 0 && copy[7] == 0);
    CHECK(strncpy(copy, "12345678", 3) == copy && copy[2] == '3' && copy[3] == 0);
    text[0] = 0;
    CHECK(strcat(text, "ab") == text);
    CHECK(strncat(text, "cdef", 2) == text && !strcmp(text, "abcd"));
    CHECK(atoi((++calls, " -17")) == -17 && calls == 1);
    CHECK(atol("2147483647") == 2147483647L);
    CHECK(atof("-1.25") == -1.25);
    CHECK(strtol("-123tail", &end, 10) == -123L && !strcmp(end, "tail"));
    CHECK(strtoul("FFFFFFFF!", &end, 16) == 0xffffffffUL && *end == '!');
    CHECK(strtod("1.25end", &end) == 1.25 && !strcmp(end, "end"));
    CHECK(strtol("none", &end, 10) == 0 && *end == 'n');
    srand(9732u);
    int first = rand();
    srand(9732u);
    CHECK(rand() == first);
    void *memory = malloc(8);
    CHECK(memory);
    free((++calls, memory));
    CHECK(calls == 2);
    free(NULL);
    char directory[MAX_PATH], path[MAX_PATH];
    CHECK(GetTempPathA(sizeof directory, directory) > 0);
    CHECK(snprintf(path, sizeof path, "%sobfh-crt-%lu.tmp", directory, (unsigned long)GetCurrentProcessId()) > 0);
    FILE *stream = fopen(path, "w+b");
    CHECK(stream);
    CHECK(fputs((++calls, "literal %n"), stream) >= 0 && calls == 3);
    CHECK(fputc('!', stream) == '!');
    CHECK(fprintf(stream, ":%d\n", 17) == 4);
    CHECK(fflush(stream) == 0 && ftell(stream) == 15L);
    CHECK(fseek(stream, 0L, SEEK_SET) == 0);
    CHECK(fgets(text, sizeof text, stream) == text && !strcmp(text, "literal %n!:17\n"));
    CHECK(fgetc(stream) == EOF && feof(stream));
    CHECK(fseek(stream, -4L, SEEK_END) == 0 && fgetc(stream) == ':');
    CHECK(fclose(stream) == 0);
    CHECK(remove(path) == 0);
    return 0;
}
static int compare_numbers(const void *a, const void *b) {
    int left = *(const int *)a, right = *(const int *)b;
    return (left > right) - (left < right);
}
static int more_crt_calls(void) {
    int numbers[] = {7, -3, 0, 7, 12, -19, 4}, key = 4, missing = 9, calls = 0;
    qsort((++calls, numbers), 7, sizeof numbers[0], compare_numbers);
    CHECK(calls == 1 && numbers[0] == -19 && numbers[6] == 12);
    for (int i = 1; i < 7; ++i) CHECK(numbers[i - 1] <= numbers[i]);
    CHECK(*(int *)bsearch(&key, numbers, 7, sizeof numbers[0], compare_numbers) == 4);
    CHECK(bsearch(&missing, numbers, 7, sizeof numbers[0], compare_numbers) == NULL);
    CHECK(bsearch(&key, numbers, 0, sizeof numbers[0], compare_numbers) == NULL);
    char directory[MAX_PATH], first[MAX_PATH], second[MAX_PATH];
    CHECK(GetTempPathA(sizeof directory, directory) > 0);
    snprintf(first, sizeof first, "%sobfh-extra-%lu-a.tmp", directory, (unsigned long)GetCurrentProcessId());
    snprintf(second, sizeof second, "%sobfh-extra-%lu-b.tmp", directory, (unsigned long)GetCurrentProcessId());
    FILE *stream = fopen(first, "w+b");
    CHECK(stream);
    CHECK(fputs("AB", stream) >= 0 && fflush(stream) == 0);
    rewind(stream);
    CHECK(!feof(stream) && !ferror(stream));
    CHECK(fgetc(stream) == 'A' && fgetc(stream) == 'B' && fgetc(stream) == EOF);
    CHECK(feof(stream) && !ferror(stream));
    clearerr(stream);
    CHECK(!feof(stream) && !ferror(stream));
    CHECK(freopen(first, "rb", stream) == stream);
    CHECK(fputc('X', stream) == EOF && ferror(stream));
    clearerr(stream);
    CHECK(!ferror(stream));
    rewind(stream);
    CHECK(fgetc(stream) == 'A');
    CHECK(fclose(stream) == 0);
    CHECK(rename(first, second) == 0);
    CHECK(remove((++calls, second)) == 0 && calls == 2);
    return 0;
}
int main(int argc, char **argv) {
    if (argc > 1 && !strcmp(argv[1], "chars")) {
        CHECK(getchar() == 'Q');
        CHECK(getchar() == EOF && feof(stdin));
        CHECK(putchar('Z') == 'Z');
        return 0;
    }
    if (argc > 1 && !strcmp(argv[1], "perror")) {
        errno = ENOENT;
        perror("OBFH_PERROR_TEST");
        errno = ENOENT;
        original_perror("OBFH_PERROR_TEST");
        return 0;
    }
    if (argc > 1 && !strcmp(argv[1], "puts-error")) {
        CHECK(freopen("NUL", "r", stdout) == stdout);
        CHECK(setvbuf(stdout, NULL, _IONBF, 0) == 0);
        CHECK(puts("read-only stream") == EOF && ferror(stdout));
        return 0;
    }
    if (argc > 1 && !strcmp(argv[1], "puts")) {
        int calls = 0;
        CHECK(puts((++calls, "literal %s %n %%")) >= 0 && calls == 1);
        CHECK(puts("") >= 0);
        CHECK(puts("line\n") >= 0);
        CHECK(puts("CRT_PUTS_PASS") >= 0);
        return 0;
    }
    if (argc > 1 && !strcmp(argv[1], "gets")) {
        char line[128];
        CHECK(gets(line) == line && !strcmp(line, "proxy-input"));
        puts("CRT_GETS_PASS");
        return 0;
    }
    CHECK(local_string_kernels() == 0);
    CHECK(crt_name_builders() == 0);
    CHECK(common_crt_calls() == 0);
    CHECK(more_crt_calls() == 0);
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
