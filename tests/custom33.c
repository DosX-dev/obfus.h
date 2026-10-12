#include <errno.h>
#include <limits.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <windows.h>
static long ref_wlong(const wchar_t *s, wchar_t **e, int b) { return wcstol(s, e, b); }
static unsigned long ref_wulong(const wchar_t *s, wchar_t **e, int b) { return wcstoul(s, e, b); }
static long long ref_w64(const wchar_t *s, wchar_t **e, int b) { return _wcstoi64(s, e, b); }
static unsigned long long ref_wu64(const wchar_t *s, wchar_t **e, int b) { return _wcstoui64(s, e, b); }
static int ref_wcase(const wchar_t *a, const wchar_t *b) { return _wcsicmp(a, b); }
static int ref_wncase(const wchar_t *a, const wchar_t *b, size_t n) { return _wcsnicmp(a, b, n); }
static int ref_mcase(const void *a, const void *b, size_t n) { return _memicmp(a, b, n); }
static char *ref_lower(char *s) { return _strlwr(s); }
static char *ref_upper(char *s) { return _strupr(s); }
static wchar_t *ref_wlower(wchar_t *s) { return _wcslwr(s); }
static wchar_t *ref_wupper(wchar_t *s) { return _wcsupr(s); }
static char *ref_set(char *s, int v) { return _strset(s, v); }
static char *ref_nset(char *s, int v, size_t n) { return _strnset(s, v, n); }
static wchar_t *ref_wset(wchar_t *s, wchar_t v) { return _wcsset(s, v); }
static wchar_t *ref_wnset(wchar_t *s, wchar_t v, size_t n) { return _wcsnset(s, v, n); }
#define NATIVE_TEXT(name, type, char_type) \
    static char_type *ref##name(type v, char_type *b, int r) { return name(v, b, r); }
NATIVE_TEXT(_itoa, int, char)
NATIVE_TEXT(_ltoa, long, char)
NATIVE_TEXT(_ultoa, unsigned long, char)
    NATIVE_TEXT(_i64toa, long long, char) NATIVE_TEXT(_ui64toa, unsigned long long, char)
        NATIVE_TEXT(_itow, int, wchar_t) NATIVE_TEXT(_ltow, long, wchar_t) NATIVE_TEXT(_ultow, unsigned long, wchar_t)
            NATIVE_TEXT(_i64tow, long long, wchar_t) NATIVE_TEXT(_ui64tow, unsigned long long, wchar_t) static long ref_labs(long v) { return labs(v); }
static long long ref_abs64(long long v) { return _abs64(v); }
static div_t ref_div(int n, int d) { return div(n, d); }
static ldiv_t ref_ldiv(long n, long d) { return ldiv(n, d); }
static char *(__cdecl *ref_tok)(char *, const char *, char **);
static wchar_t *(__cdecl *ref_wtok)(wchar_t *, const wchar_t *, wchar_t **);
static lldiv_t(__cdecl *ref_lldiv)(long long, long long);
#include "../include/obfus.h"
#undef if
#undef else
#undef case
#undef for
#undef while
#undef break
#undef continue
#undef return
/* The bundled import library omits these native exports. Baseline/editor probes
 * bridge only those exports; protected builds exercise the real header macros. */
#if NO_OBF
#define strtok_s(...) ref_tok(__VA_ARGS__)
#define wcstok_s(...) ref_wtok(__VA_ARGS__)
#define lldiv(...) ref_lldiv(__VA_ARGS__)
#endif
#define CHECK(x) \
    do { \
        if (!(x)) { \
            (printf)("CUSTOM33_FAIL:%d\n", __LINE__); \
            return 1; \
        } \
    } while (0)
static unsigned state = 1234567;
static unsigned draw(void) {
    state = state * 1664525u + 1013904223u;
    return state;
}
static int sign(int x) { return (x > 0) - (x < 0); }
static int parse(const wchar_t *s, int base) {
    wchar_t *a = (wchar_t *)s, *b = (wchar_t *)s;
    errno = 73;
    long x = ref_wlong(s, &a, base);
    int e = errno;
    errno = 73;
    long y = wcstol(s, &b, base);
    CHECK(x == y && a == b && e == errno);
    errno = 73;
    unsigned long u = ref_wulong(s, &a, base);
    e = errno;
    errno = 73;
    unsigned long v = wcstoul(s, &b, base);
    CHECK(u == v && a == b && e == errno);
    errno = 73;
    long long q = ref_w64(s, &a, base);
    e = errno;
    errno = 73;
    long long r = _wcstoi64(s, &b, base);
    CHECK(q == r && a == b && e == errno);
    errno = 73;
    unsigned long long k = ref_wu64(s, &a, base);
    e = errno;
    errno = 73;
    unsigned long long l = _wcstoui64(s, &b, base);
    CHECK(k == l && a == b && e == errno);
    return 0;
}
static int numbers(void) {
    const wchar_t *edges[] = {L"", L" ", L"+", L"-", L"0", L"0x", L"0xz", L"0x1", L"09", L"-2147483648", L"2147483648", L"-9223372036854775808", L"9223372036854775808", L"18446744073709551615", L"18446744073709551616", L"-99999999999999999999999999999999999", L"\x2003-\xff11\xff12!", L"\x0660xff", L"\xff10x1"};
    for (int base = 0; base <= 36; ++base) {
        if (base == 1) continue;
        for (size_t i = 0; i < sizeof(edges) / sizeof(edges[0]); ++i) CHECK(!parse(edges[i], base));
    }
    for (unsigned code = 0; code < 65536; ++code) {
        wchar_t s[] = {(wchar_t)code, L'7', 0};
        CHECK(!parse(s, 10));
    }
    return 0;
}
static int text(void) {
    char a[72], b[72];
    wchar_t wa[72], wb[72];
    for (int radix = 2; radix <= 36; ++radix)
        for (unsigned i = 0; i < 64; ++i) {
            unsigned long long v = ((unsigned long long)draw() << 32) | draw();
            if (i == 0) v = 0;
            if (i == 1) v = ULLONG_MAX;
            if (i == 2) v = 1ULL << 63;
#define TEXT_CHECK(name, type) \
    do { \
        (memset)(a, 0xa5, sizeof a); \
        (memset)(b, 0xa5, sizeof b); \
        CHECK(name((type)v, a + 1, radix) == a + 1); \
        ref##name((type)v, b + 1, radix); \
        CHECK(!(memcmp)(a, b, sizeof a)); \
    } while (0)
#define WTEXT_CHECK(name, type) \
    do { \
        (memset)(wa, 0xa5, sizeof wa); \
        (memset)(wb, 0xa5, sizeof wb); \
        CHECK(name((type)v, wa + 1, radix) == wa + 1); \
        ref##name((type)v, wb + 1, radix); \
        CHECK(!(memcmp)(wa, wb, sizeof wa)); \
    } while (0)
            TEXT_CHECK(_itoa, int);
            TEXT_CHECK(_ltoa, long);
            TEXT_CHECK(_ultoa, unsigned long);
            TEXT_CHECK(_i64toa, long long);
            TEXT_CHECK(_ui64toa, unsigned long long);
            WTEXT_CHECK(_itow, int);
            WTEXT_CHECK(_ltow, long);
            WTEXT_CHECK(_ultow, unsigned long);
            WTEXT_CHECK(_i64tow, long long);
            WTEXT_CHECK(_ui64tow, unsigned long long);
        }
    long ns[] = {LONG_MIN, LONG_MIN + 1, -73, -1, 0, 1, 73, LONG_MAX};
    long long ls[] = {LLONG_MIN, LLONG_MIN + 1, -73, -1, 0, 1, 73, LLONG_MAX};
    for (unsigned i = 0; i < 8; ++i) {
        if (ns[i] != LONG_MIN) CHECK(labs(ns[i]) == ref_labs(ns[i]));
        if (ls[i] != LLONG_MIN) CHECK(llabs(ls[i]) == ref_abs64(ls[i]) && _abs64(ls[i]) == ref_abs64(ls[i]));
        for (unsigned j = 0; j < 8; ++j) {
            if (!ns[j] || (ns[i] == LONG_MIN && ns[j] == -1)) continue;
            div_t a = div((int)ns[i], (int)ns[j]), b = ref_div((int)ns[i], (int)ns[j]);
            ldiv_t c = ldiv(ns[i], ns[j]), d = ref_ldiv(ns[i], ns[j]);
            CHECK(a.quot == b.quot && a.rem == b.rem && c.quot == d.quot && c.rem == d.rem);
            if (!ls[j] || (ls[i] == LLONG_MIN && ls[j] == -1)) continue;
            lldiv_t e = lldiv(ls[i], ls[j]), f = ref_lldiv(ls[i], ls[j]);
            CHECK(e.quot == f.quot && e.rem == f.rem);
        }
    }
    return 0;
}
static int strings(void) {
    for (unsigned n = 0; n <= 64; ++n) {
        char a[68], b[68], c[68];
        wchar_t wa[68], wb[68], wc[68];
        (memset)(a, 0xa5, sizeof a);
        (memset)(wa, 0xa5, sizeof wa);
        for (unsigned j = 0; j < n; ++j) a[j + 1] = (char)(1 + draw() % 255), wa[j + 1] = (wchar_t)(1 + draw() % 65535);
        a[n + 1] = 0;
        wa[n + 1] = 0;
        (memcpy)(b, a, sizeof a);
        (memcpy)(c, a, sizeof a);
        (memcpy)(wb, wa, sizeof wa);
        (memcpy)(wc, wa, sizeof wa);
        CHECK(_strlwr(b + 1) == b + 1 && ref_lower(c + 1) == c + 1);
        if ((memcmp)(b, c, sizeof b)) {
            (printf)("LOWER locale=%s n=%u\n", (setlocale)(LC_CTYPE, NULL), n);
            for (unsigned j = 1; j <= n; ++j) (printf)("%02x:%02x/%02x ", (unsigned char)a[j], (unsigned char)b[j], (unsigned char)c[j]);
            (puts)("");
            return 1;
        }
        (memcpy)(b, a, sizeof a);
        (memcpy)(c, a, sizeof a);
        CHECK(_strupr(b + 1) == b + 1 && ref_upper(c + 1) == c + 1 && !(memcmp)(b, c, sizeof b));
        CHECK(_wcslwr(wb + 1) == wb + 1 && ref_wlower(wc + 1) == wc + 1 && !(memcmp)(wb, wc, sizeof wb));
        (memcpy)(wb, wa, sizeof wa);
        (memcpy)(wc, wa, sizeof wa);
        CHECK(_wcsupr(wb + 1) == wb + 1 && ref_wupper(wc + 1) == wc + 1 && !(memcmp)(wb, wc, sizeof wb));
        CHECK(sign(_memicmp(a + 1, b + 1, n)) == sign(ref_mcase(a + 1, b + 1, n)));
        CHECK(sign(_wcsicmp(wa + 1, wb + 1)) == sign(ref_wcase(wa + 1, wb + 1)));
        for (unsigned count = 0; count <= n + 1; ++count) {
            CHECK(sign(_wcsnicmp(wa + 1, wb + 1, count)) == sign(ref_wncase(wa + 1, wb + 1, count)));
            int value = count % 3 == 0 ? 0 : count % 3 == 1 ? 0x180
                                                            : -1;
            (memcpy)(b, a, sizeof a);
            (memcpy)(c, a, sizeof a);
            (memcpy)(wb, wa, sizeof wa);
            (memcpy)(wc, wa, sizeof wa);
            CHECK(_strnset(b + 1, value, count) == b + 1 && ref_nset(c + 1, value, count) == c + 1 && !(memcmp)(b, c, sizeof b));
            CHECK(_wcsnset(wb + 1, (wchar_t)value, count) == wb + 1 && ref_wnset(wc + 1, (wchar_t)value, count) == wc + 1 && !(memcmp)(wb, wc, sizeof wb));
        }
        (memcpy)(b, a, sizeof a);
        (memcpy)(c, a, sizeof a);
        (memcpy)(wb, wa, sizeof wa);
        (memcpy)(wc, wa, sizeof wa);
        CHECK(_strset(b + 1, 0) == b + 1 && ref_set(c + 1, 0) == c + 1 && !(memcmp)(b, c, sizeof b));
        CHECK(_wcsset(wb + 1, 0xffff) == wb + 1 && ref_wset(wc + 1, 0xffff) == wc + 1 && !(memcmp)(wb, wc, sizeof wb));
    }
    return 0;
}
static int tokens(void) {
    const char *cases[] = {"", "a", ",,,", ",a,,b;c,", "a,b;c", "a,,b", "a,b,"};
    const char *sets[] = {",", ";", ",;", "", "ab", ","};
    for (unsigned i = 0; i < 7; ++i) {
        char a[40], b[40], *ac = NULL, *bc = NULL;
        wchar_t wa[40], wb[40], *wac = NULL, *wbc = NULL;
        (memset)(a, 0xa5, sizeof a);
        (memcpy)(b, a, sizeof a);
        (strcpy)(a, cases[i]);
        (strcpy)(b, cases[i]);
        for (unsigned j = 0; j < 40; ++j) wa[j] = wb[j] = (unsigned char)a[j];
        for (unsigned j = 0; j < 6; ++j) {
            wchar_t set[8];
            for (unsigned k = 0; k < 8; ++k) {
                set[k] = (unsigned char)sets[j][k];
                if (!set[k]) break;
            }
            char *x = strtok_s(j ? NULL : a, sets[j], &ac), *y = ref_tok(j ? NULL : b, sets[j], &bc);
            wchar_t *u = wcstok_s(j ? NULL : wa, set, &wac), *v = ref_wtok(j ? NULL : wb, set, &wbc);
            CHECK((x ? x - a : -1) == (y ? y - b : -1) && (u ? u - wa : -1) == (v ? v - wb : -1));
            CHECK(ac - a == bc - b && wac - wa == wbc - wb && !(memcmp)(a, b, sizeof a) && !(memcmp)(wa, wb, sizeof wa));
        }
    }
    char *ctx = NULL;
    wchar_t *wctx = NULL;
    errno = 73;
    CHECK(!strtok_s(NULL, ",", &ctx) && errno == EINVAL && !ctx);
    errno = 73;
    CHECK(!wcstok_s(NULL, L",", &wctx) && errno == EINVAL && !wctx);
    errno = 73;
    CHECK(!strtok_s("a", NULL, &ctx) && errno == EINVAL);
    errno = 73;
    CHECK(!wcstok_s(L"a", L",", NULL) && errno == EINVAL);
    /* Native/custom continuation must use the exact same caller context. */
    char mixed[] = "a,b,c";
    ctx = NULL;
    CHECK(!(strcmp)(strtok_s(mixed, ",", &ctx), "a"));
    CHECK(!(strcmp)(ref_tok(NULL, ",", &ctx), "b"));
    CHECK(!(strcmp)(strtok_s(NULL, ",", &ctx), "c"));
    return 0;
}
static int guards(void) {
    SYSTEM_INFO info;
    (GetSystemInfo)(&info);
    DWORD old;
    char *p = (VirtualAlloc)(NULL, info.dwPageSize * 2, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    CHECK(p && (VirtualProtect)(p + info.dwPageSize, info.dwPageSize, PAGE_NOACCESS, &old));
    char *e = p + info.dwPageSize;
    wchar_t *we = (wchar_t *)e;
    CHECK(!_memicmp(e, e, 0) && !_wcsnicmp(we, we, 0) && _strnset(e, 'x', 0) == e && _wcsnset(we, L'x', 0) == we);
    e[-1] = 'A';
    CHECK(!_memicmp(e - 1, "a", 1));
    we[-1] = 0;
    CHECK(!parse(we - 1, 10));
    char *ctx = NULL;
    (memcpy)(e - 4, "a,b", 4);
    CHECK(strtok_s(e - 4, ",", &ctx) == e - 4 && strtok_s(NULL, ",", &ctx) == e - 2 && !strtok_s(NULL, ",", &ctx));
    wchar_t *wctx = NULL;
    (memcpy)(we - 4, L"a,b", 8);
    CHECK(wcstok_s(we - 4, L",", &wctx) == we - 4 && wcstok_s(NULL, L",", &wctx) == we - 2 && !wcstok_s(NULL, L",", &wctx));
    CHECK(_ui64toa(ULLONG_MAX, e - 65, 2) == e - 65 && e[-1] == 0);
    CHECK(_ui64tow(ULLONG_MAX, we - 65, 2) == we - 65 && we[-1] == 0);
    (VirtualFree)(p, 0, MEM_RELEASE);
    return 0;
}
static int side_effects(void) {
    char buffer[80], *ctx = NULL;
    wchar_t wide[80], *wctx = NULL, *end;
    unsigned a = 0, b = 0, c = 0;
    CHECK(_itoa((++a, -7), (++b, buffer), (++c, 10)) == buffer && a == 1 && b == 1 && c == 1 && !(strcmp)(buffer, "-7"));
    a = b = c = 0;
    CHECK(_wcstoi64((++a, L"4294967296!"), (++b, &end), (++c, 10)) == 4294967296LL && *end == L'!' && a == 1 && b == 1 && c == 1);
    a = b = c = 0;
    (strcpy)(buffer, "AbC");
    CHECK(_strnset((++a, buffer), (++b, 'x'), (++c, 2)) == buffer && a == 1 && b == 1 && c == 1 && !(strcmp)(buffer, "xxC"));
    a = b = 0;
    div_t result = div((++a, -7), (++b, 3));
    CHECK(result.quot == -2 && result.rem == -1 && a == 1 && b == 1);
    a = 0;
    CHECK(_abs64((++a, -4294967296LL)) == 4294967296LL && a == 1);
    a = 0;
    (strcpy)(buffer, "AbC");
    CHECK(_strlwr((++a, buffer)) == buffer && a == 1 && !(strcmp)(buffer, "abc"));
    a = b = c = 0;
    (strcpy)(buffer, "a,b");
    CHECK(strtok_s((++a, buffer), (++b, ","), (++c, &ctx)) == buffer && a == 1 && b == 1 && c == 1);
    a = b = c = 0;
    (wcscpy)(wide, L"a,b");
    CHECK(wcstok_s((++a, wide), (++b, L","), (++c, &wctx)) == wide && a == 1 && b == 1 && c == 1);
    CHECK(__builtin_types_compatible_p(__typeof__(div(1, 2)), div_t));
    CHECK(__builtin_types_compatible_p(__typeof__(ldiv(1, 2)), ldiv_t));
    CHECK(__builtin_types_compatible_p(__typeof__(lldiv(1, 2)), lldiv_t));
    return 0;
}
int main(void) {
    HMODULE m = (GetModuleHandleA)("msvcrt.dll"), u = (LoadLibraryA)("ucrtbase.dll");
    ref_tok = (void *)(GetProcAddress)(m, "strtok_s");
    ref_wtok = (void *)(GetProcAddress)(m, "wcstok_s");
    ref_lldiv = (void *)(GetProcAddress)(u, "lldiv");
    CHECK(ref_tok && ref_wtok && ref_lldiv);
    CHECK(!numbers() && !text() && !tokens() && !guards() && !side_effects());
    const char *locales[] = {"C", "English_United States.1252", "German_Germany.1252", "Japanese_Japan.932", "Chinese_China.936"};
    unsigned tested = 0;
    for (unsigned i = 0; i < 5; ++i) {
        if (!(setlocale)(LC_CTYPE, locales[i])) continue;
        ++tested;
        CHECK(!strings());
    }
    CHECK(tested >= 2);
    (setlocale)(LC_CTYPE, "C");
    (printf)("CUSTOM33_PASS locales=%u\n", tested);
    return 0;
}
