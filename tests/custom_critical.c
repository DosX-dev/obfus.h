#include <errno.h>
#include <limits.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
static long long native_signed(const char *s, char **e, int base) { return _strtoi64(s, e, base); }
static unsigned long long native_unsigned(const char *s, char **e, int base) { return _strtoui64(s, e, base); }
static long long native_decimal(const char *s) { return _atoi64(s); }
static int native_case(const char *a, const char *b) { return _stricmp(a, b); }
static int native_case_n(const char *a, const char *b, size_t n) { return _strnicmp(a, b, n); }
#include "../include/obfus.h"
#undef if
#undef else
#undef while
#undef for
#undef break
#undef return
#define CHECK(x) \
    do { \
        if (!(x)) { \
            (printf)("CRITICAL_FAIL:%d\n", __LINE__); \
            return 1; \
        } \
    } while (0)
static int sign(int x) { return (x > 0) - (x < 0); }
static int number(const char *s, int base) {
    char *a = (char *)s, *b = (char *)s;
    errno = 73;
    long long x = native_signed(s, &a, base);
    int error = errno;
    errno = 73;
    long long y = strtoll(s, &b, base);
    CHECK(x == y && a == b && error == errno);
    errno = 73;
    unsigned long long u = native_unsigned(s, &a, base);
    error = errno;
    errno = 73;
    unsigned long long v = _strtoui64(s, &b, base);
    CHECK(u == v && a == b && error == errno);
    errno = 73;
    x = native_decimal(s);
    error = errno;
    errno = 73;
    y = atoll(s);
    CHECK(x == y && error == errno);
    return 0;
}
static unsigned state = 12345;
static unsigned draw(void) {
    state = state * 1664525u + 1013904223u;
    return state;
}
static int strings(void) {
    for (unsigned i = 1; i <= 255; ++i) {
        char a[] = {(char)i, 0};
        for (unsigned j = 1; j <= 255; ++j) {
            char b[] = {(char)j, 0};
            CHECK(sign(_stricmp(a, b)) == sign(native_case(a, b)));
            CHECK(sign(strnicmp(a, b, 1)) == sign(native_case_n(a, b, 1)));
        }
    }
    for (unsigned i = 0; i < 512; ++i) {
        char a[33], b[33];
        for (unsigned j = 0; j < 32; ++j) a[j] = (char)(1 + draw() % 255), b[j] = (char)(1 + draw() % 255);
        a[draw() % 33] = 0;
        b[draw() % 33] = 0;
        a[32] = b[32] = 0;
        CHECK(sign(stricmp(a, b)) == sign(native_case(a, b)));
        for (size_t n = 0; n <= 33; ++n)
            CHECK(sign(_strnicmp(a, b, n)) == sign(native_case_n(a, b, n)));
    }
    return 0;
}
int main(void) {
    const char *edges[] = {"", " ", "abc", "+", "-", "0", "-0", "0x", "0xz", "0x1", "-0Xff!", "08", "  +42tail", "9223372036854775807", "9223372036854775808", "-9223372036854775808", "-9223372036854775809", "18446744073709551615", "18446744073709551616", "-18446744073709551616", "99999999999999999999999999999999999999", "-99999999999999999999999999999999999999"};
    for (int base = 0; base <= 36; ++base) {
        if (base == 1) continue;
        for (size_t i = 0; i < sizeof edges / sizeof edges[0]; ++i) CHECK(!number(edges[i], base));
    }
    CHECK(!number("12", -1) && !number("12", 1) && !number("12", 37) && !number(NULL, 10));
    for (unsigned i = 0; i < 1024; ++i) {
        char s[100];
        (sprintf)(s, "%s%u%u%u%u!", i & 1 ? "-" : "+", draw(), draw(), draw(), draw());
        CHECK(!number(s, i % 35 + 2));
    }
    const char *locales[] = {"C", "English_United States.1252", "German_Germany.1252", "Turkish_Turkey.1254"};
    unsigned available = 0;
    for (unsigned i = 0; i < 4; ++i) {
        if (!(setlocale)(LC_CTYPE, locales[i])) continue;
        ++available;
        CHECK(!strings() && !number(" \t-9223372036854775808!", 10));
    }
    CHECK(available >= 2);
    (setlocale)(LC_CTYPE, "C");
    unsigned a = 0, b = 0, c = 0;
    char *end;
    CHECK(_strtoi64((++a, "4294967296!"), (++b, &end), (++c, 10)) == 4294967296LL && *end == '!');
    CHECK(a == 1 && b == 1 && c == 1);
    a = b = c = 0;
    CHECK(!_strnicmp((++a, "AbC"), (++b, "aBc"), (++c, 3)) && a == 1 && b == 1 && c == 1);
    CHECK(strtoll("42", NULL, 10) == 42 && strtoull("18446744073709551615", NULL, 10) == ULLONG_MAX && _atoi64("-42") == -42);
    SYSTEM_INFO info;
    (GetSystemInfo)(&info);
    char *page = (VirtualAlloc)(NULL, info.dwPageSize * 2, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    DWORD old;
    CHECK(page && (VirtualProtect)(page + info.dwPageSize, info.dwPageSize, PAGE_NOACCESS, &old));
    char *edge = page + info.dwPageSize;
    edge[-1] = 'A';
    CHECK(!strnicmp(edge - 1, "a", 1) && !strnicmp(edge, edge, 0));
    edge[-1] = 0;
    CHECK(!stricmp(edge - 1, "") && !number(edge - 1, 0));
    (memcpy)(edge - 20, "9223372036854775807", 20);
    CHECK(!number(edge - 20, 10));
    (VirtualFree)(page, 0, MEM_RELEASE);
    (printf)("CUSTOM_CRITICAL_PASS locales=%u\n", available);
    return 0;
}
