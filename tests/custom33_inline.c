#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <windows.h>

#include "../include/obfus.h"
#undef if
#undef else
#undef for
#undef while
#undef break
#undef continue
#define if(...) OBFH_FORBIDDEN_INTERNAL_IF
#define else OBFH_FORBIDDEN_INTERNAL_ELSE
#define for(...) OBFH_FORBIDDEN_INTERNAL_FOR
#define while(...) OBFH_FORBIDDEN_INTERNAL_WHILE
#define break OBFH_FORBIDDEN_INTERNAL_BREAK
#define continue OBFH_FORBIDDEN_INTERNAL_CONTINUE
int main(void) {
    char a[80] = "AbC", b[80] = "abc", *ctx = NULL, *tok;
    wchar_t wa[80] = L"AbC", wb[80] = L"abc", *wctx = NULL, *wend, *wtok;
    unsigned fail = 0;
    fail |= !__builtin_types_compatible_p(__typeof__(wcstol(L"42", &wend, 10)), long);
    (void)(wcstol(L"42", &wend, 10));
    fail |= !__builtin_types_compatible_p(__typeof__(wcstoul(L"42", &wend, 10)), unsigned long);
    (void)(wcstoul(L"42", &wend, 10));
    fail |= !__builtin_types_compatible_p(__typeof__(_wcstoi64(L"42", &wend, 10)), long long);
    (void)(_wcstoi64(L"42", &wend, 10));
    fail |= !__builtin_types_compatible_p(__typeof__(_wcstoui64(L"42", &wend, 10)), unsigned long long);
    (void)(_wcstoui64(L"42", &wend, 10));
    fail |= !__builtin_types_compatible_p(__typeof__(_wcsicmp(wa, wb)), int);
    (void)(_wcsicmp(wa, wb));
    fail |= !__builtin_types_compatible_p(__typeof__(_wcsnicmp(wa, wb, 3)), int);
    (void)(_wcsnicmp(wa, wb, 3));
    fail |= !__builtin_types_compatible_p(__typeof__(_memicmp(a, b, 3)), int);
    (void)(_memicmp(a, b, 3));
    fail |= !__builtin_types_compatible_p(__typeof__(_strlwr(a)), char *);
    (void)(_strlwr(a));
    fail |= !__builtin_types_compatible_p(__typeof__(_strupr(a)), char *);
    (void)(_strupr(a));
    fail |= !__builtin_types_compatible_p(__typeof__(_wcslwr(wa)), wchar_t *);
    (void)(_wcslwr(wa));
    fail |= !__builtin_types_compatible_p(__typeof__(_wcsupr(wa)), wchar_t *);
    (void)(_wcsupr(wa));
    fail |= !__builtin_types_compatible_p(__typeof__(_strset(a, 88)), char *);
    (void)(_strset(a, 88));
    fail |= !__builtin_types_compatible_p(__typeof__(_strnset(a, 88, 2)), char *);
    (void)(_strnset(a, 88, 2));
    fail |= !__builtin_types_compatible_p(__typeof__(_wcsset(wa, 88)), wchar_t *);
    (void)(_wcsset(wa, 88));
    fail |= !__builtin_types_compatible_p(__typeof__(_wcsnset(wa, 88, 2)), wchar_t *);
    (void)(_wcsnset(wa, 88, 2));
    fail |= !__builtin_types_compatible_p(__typeof__(labs(-7)), long);
    (void)(labs(-7));
    fail |= !__builtin_types_compatible_p(__typeof__(llabs(-7)), long long);
    (void)(llabs(-7));
    fail |= !__builtin_types_compatible_p(__typeof__(_abs64(-7)), long long);
    (void)(_abs64(-7));
    fail |= !__builtin_types_compatible_p(__typeof__(div(-7, 3)), div_t);
    (void)(div(-7, 3));
    fail |= !__builtin_types_compatible_p(__typeof__(ldiv(-7, 3)), ldiv_t);
    (void)(ldiv(-7, 3));
    fail |= !__builtin_types_compatible_p(__typeof__(lldiv(-7, 3)), lldiv_t);
    (void)(lldiv(-7, 3));
    fail |= !__builtin_types_compatible_p(__typeof__(_itoa((int)42, a, 10)), char *);
    (void)(_itoa((int)42, a, 10));
    fail |= !__builtin_types_compatible_p(__typeof__(_ltoa((long)42, a, 10)), char *);
    (void)(_ltoa((long)42, a, 10));
    fail |= !__builtin_types_compatible_p(__typeof__(_ultoa((unsigned long)42, a, 10)), char *);
    (void)(_ultoa((unsigned long)42, a, 10));
    fail |= !__builtin_types_compatible_p(__typeof__(_i64toa((long long)42, a, 10)), char *);
    (void)(_i64toa((long long)42, a, 10));
    fail |= !__builtin_types_compatible_p(__typeof__(_ui64toa((unsigned long long)42, a, 10)), char *);
    (void)(_ui64toa((unsigned long long)42, a, 10));
    fail |= !__builtin_types_compatible_p(__typeof__(_itow((int)42, wa, 10)), wchar_t *);
    (void)(_itow((int)42, wa, 10));
    fail |= !__builtin_types_compatible_p(__typeof__(_ltow((long)42, wa, 10)), wchar_t *);
    (void)(_ltow((long)42, wa, 10));
    fail |= !__builtin_types_compatible_p(__typeof__(_ultow((unsigned long)42, wa, 10)), wchar_t *);
    (void)(_ultow((unsigned long)42, wa, 10));
    fail |= !__builtin_types_compatible_p(__typeof__(_i64tow((long long)42, wa, 10)), wchar_t *);
    (void)(_i64tow((long long)42, wa, 10));
    fail |= !__builtin_types_compatible_p(__typeof__(_ui64tow((unsigned long long)42, wa, 10)), wchar_t *);
    (void)(_ui64tow((unsigned long long)42, wa, 10));
    fail |= !__builtin_types_compatible_p(__typeof__(strtok_s(a, ",", &ctx)), char *);
    (void)(strtok_s(a, ",", &ctx));
    fail |= !__builtin_types_compatible_p(__typeof__(wcstok_s(wa, L",", &wctx)), wchar_t *);
    (void)(wcstok_s(wa, L",", &wctx));
    fail |= div(-7, 3).rem != -1 || lldiv(-7, 3).quot != -2;
    fail |= labs(-7) != 7 || llabs(-4294967296LL) != 4294967296LL;
    fail |= strcmp(_itoa(-7, a, 10), "-7") != 0;
    fail |= _wcstoi64(_i64tow(4294967296LL, wa, 10), NULL, 10) != 4294967296LL;
    ExitProcess(fail);
}
