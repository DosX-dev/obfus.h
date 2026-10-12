#include <errno.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <windows.h>
static long ref_long(const char *s, char **e, int b) { return strtol(s, e, b); }
static unsigned long ref_ulong(const char *s, char **e, int b) { return strtoul(s, e, b); }
static int ref_atoi(const char *s) { return atoi(s); }
static long ref_atol(const char *s) { return atol(s); }
static char *ref_token(char *s, const char *d) { return strtok(s, d); }
static char *ref_reverse(char *s) { return _strrev(s); }
static wchar_t *ref_wreverse(wchar_t *s) { return _wcsrev(s); }
#include "../include/obfus.h"
#define CHECK(x) \
    do { \
        if (!(x)) { \
            (fprintf)(stderr, "EXTRA_FAIL:%d\n", __LINE__); \
            return 1; \
        } \
    } while (0)
static int compare(const char *s, int base) {
    char *a, *b;
    int *error = _errno(), ea, eb;
    *error = 73;
    long x = ref_long(s, &a, base);
    ea = *error;
    *error = 73;
    long y = strtol(s, &b, base);
    eb = *error;
    if (x != y || a != b || ea != eb) {
        (printf)("LONG [%s] base=%d %ld/%ld end=%d/%d errno=%d/%d\n", s, base, x, y, (int)(a - s), (int)(b - s), ea, eb);
        return 1;
    }
    *error = 73;
    unsigned long u = ref_ulong(s, &a, base);
    ea = *error;
    *error = 73;
    unsigned long v = strtoul(s, &b, base);
    eb = *error;
    if (u != v || a != b || ea != eb) {
        (printf)("ULONG [%s] base=%d %lu/%lu end=%d/%d errno=%d/%d\n", s, base, u, v, (int)(a - s), (int)(b - s), ea, eb);
        return 1;
    }
    *error = 73;
    int i = ref_atoi(s);
    ea = *error;
    *error = 73;
    int j = atoi(s);
    eb = *error;
    if (i != j || ea != eb) return 1;
    *error = 73;
    x = ref_atol(s);
    ea = *error;
    *error = 73;
    y = atol(s);
    eb = *error;
    return x != y || ea != eb;
}
static DWORD WINAPI worker(void *unused) {
    char s[] = "a,b;c";
    char *p = strtok(s, ",");
    if (!p || strcmp(p, "a")) return 1;
    Sleep(1);
    p = ref_token(NULL, ";");
    return !p || strcmp(p, "b") || !strtok(NULL, ";") || strtok(NULL, ";") != NULL;
}
#if !NO_OBF && !OBFH_EDITOR_VIEW
static int allocation_attempts;
static void *failed_allocation(size_t size) {
    ++allocation_attempts;
    return NULL;
}
#undef malloc
#define malloc(size) failed_allocation(size)
static int allocation_failure(void) {
    return strdup("a") != NULL || _wcsdup(L"a") != NULL || allocation_attempts != 2;
}
#undef malloc
#define malloc(...) malloc_proxy(__VA_ARGS__)
#endif
int main(void) {
    const char *cases[] = {"", " ", "+", "-", "0", "00", "0x", "0Xg", "0x1Z", "-0x80000000!", "2147483647", "2147483648", "-2147483648", "-2147483649", "4294967295", "4294967296", "-4294967295", "999999999999999999999999999999999", " \t\r\n\v\f+123tail", "09", "zZ",
                           "\xa0"
                           "12"};
    for (int n = 0; n < sizeof(cases) / sizeof(*cases); ++n)
        for (int base = 0; base <= 36; ++base) {
            if (base == 1) continue;
            CHECK(!compare(cases[n], base));
        }
    CHECK(!compare("12", -1) && !compare("12", 1) && !compare("12", 37));
    CHECK(!compare(NULL, 10));
    unsigned int random = 1;
    char s[80];
    for (int n = 0; n < 600; ++n) {
        random = random * 1664525u + 1013904223u;
        (sprintf)(s, "%s%lu%lu!", n & 1 ? "-" : " +", (unsigned long)random, (unsigned long)(random ^ 0xabcdefu));
        CHECK(!compare(s, n % 37 == 1 ? 10 : n % 37));
    }
    const char *locales[] = {"C", ".1252", ".1251"};
    for (int l = 0; l < 3; ++l)
        if (setlocale(LC_CTYPE, locales[l])) {
            for (int c = 1; c < 256; ++c) {
                s[0] = c;
                s[1] = '1';
                s[2] = '2';
                s[3] = 0;
                CHECK(!compare(s, 10));
            }
        }
    setlocale(LC_ALL, "C");
    char *end;
    CHECK(strtol("123", NULL, 10) == 123);
    char **ep = &end;
    const char *sp = "12";
    int base = 10;
    CHECK(strtol(sp++, ep++, base++) == 12 && sp[0] == '2' && ep == &end + 1 && base == 11);
    const char *dup_source = "duplicate";
    char *dup = strdup(dup_source++);
    CHECK(dup && strcmp(dup, "duplicate") == 0 && *dup_source == 'u');
    free(dup);
    dup = _strdup("");
    CHECK(dup && !*dup);
    free(dup);
    CHECK(_strdup(NULL) == NULL && _wcsdup(NULL) == NULL);
    wchar_t *wd = _wcsdup(L"wide \x0410");
    CHECK(wd && wcscmp(wd, L"wide \x0410") == 0);
    free(wd);
#if !NO_OBF
#if !OBFH_EDITOR_VIEW
    CHECK(!allocation_failure());
#endif
    CHECK(strnlen_s(NULL, 99) == 0 && wcsnlen_s(NULL, 99) == 0);
    char raw[] = {'a', 'b', 'c'};
    wchar_t wraw[] = {L'a', L'b', L'c'};
    CHECK(strnlen_s(raw, 3) == 3 && strnlen_s(raw, 0) == 0 && wcsnlen_s(wraw, 3) == 3);
    CHECK(strnlen_s("ab", 9) == 2 && wcsnlen_s(L"ab", 9) == 2);
    SYSTEM_INFO info;
    (GetSystemInfo)(&info);
    char *pages = (VirtualAlloc)(NULL, info.dwPageSize * 2, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    DWORD old;
    CHECK(pages && (VirtualProtect)(pages + info.dwPageSize, info.dwPageSize, PAGE_NOACCESS, &old));
    char *edge = pages + info.dwPageSize - 3;
    edge[0] = edge[1] = edge[2] = 'x';
    CHECK(strnlen_s(edge, 3) == 3);
    wchar_t *wedge = (wchar_t *)(pages + info.dwPageSize) - 3;
    wedge[0] = wedge[1] = wedge[2] = L'x';
    CHECK(wcsnlen_s(wedge, 3) == 3);
    (VirtualFree)(pages, 0, MEM_RELEASE);
#endif
    for (int n = 0; n < 40; ++n) {
        char a[41], b[41];
        wchar_t wa[41], wb[41];
        for (int i = 0; i < n; ++i) a[i] = b[i] = 'A' + i % 26, wa[i] = wb[i] = L'A' + i % 26;
        a[n] = b[n] = 0;
        wa[n] = wb[n] = 0;
        CHECK(strrev(a) == a && !strcmp(a, ref_reverse(b)));
        CHECK(_wcsrev(wa) == wa && !wcscmp(wa, ref_wreverse(wb)));
    }
    char tokens[] = ",,a,b;c";
    CHECK(!strcmp(strtok(tokens, ","), "a"));
    CHECK(!strcmp(ref_token(NULL, ";"), "b"));
    CHECK(!strcmp(strtok(NULL, ""), "c") && !ref_token(NULL, ","));
    HANDLE threads[4];
    for (int i = 0; i < 4; ++i) threads[i] = CreateThread(NULL, 0, worker, NULL, 0, NULL);
    CHECK(WaitForMultipleObjects(4, threads, TRUE, 10000) == WAIT_OBJECT_0);
    for (int i = 0; i < 4; ++i) {
        DWORD result;
        CHECK(GetExitCodeThread(threads[i], &result) && !result);
        CloseHandle(threads[i]);
    }
    (puts)("CUSTOM_EXTRA_PASS");
    return 0;
}
