#include <stdio.h>
#include <string.h>
#include <wchar.h>
#include <windows.h>
static void *ref_memcpy(void *a, const void *b, size_t n) { return memcpy(a, b, n); }
static void *ref_memset(void *a, int b, size_t n) { return memset(a, b, n); }
static void *ref_memmove(void *a, const void *b, size_t n) { return memmove(a, b, n); }
static int ref_memcmp(const void *a, const void *b, size_t n) { return memcmp(a, b, n); }
static char *ref_strcpy(char *a, const char *b) { return strcpy(a, b); }
static char *ref_strcat(char *a, const char *b) { return strcat(a, b); }
static char *ref_strncpy(char *a, const char *b, size_t n) { return strncpy(a, b, n); }
static char *ref_strncat(char *a, const char *b, size_t n) { return strncat(a, b, n); }
static char *ref_strchr(const char *a, int c) { return strchr(a, c); }
static char *ref_strrchr(const char *a, int c) { return strrchr(a, c); }
static size_t ref_wcslen(const wchar_t *a) { return wcslen(a); }
static int ref_wcscmp(const wchar_t *a, const wchar_t *b) { return wcscmp(a, b); }
static int ref_wcsncmp(const wchar_t *a, const wchar_t *b, size_t n) { return wcsncmp(a, b, n); }
static wchar_t *ref_wcschr(const wchar_t *a, wchar_t c) { return wcschr(a, c); }
static wchar_t *ref_wcsrchr(const wchar_t *a, wchar_t c) { return wcsrchr(a, c); }
static ULONG_PTR flags(void) {
    ULONG_PTR f;
    __asm__ __volatile__("pushf; pop %0"
                         : "=r"(f));
    return f;
}
static unsigned int rng = 7;
static unsigned int next(void) {
    rng ^= rng << 13;
    rng ^= rng >> 17;
    rng ^= rng << 5;
    return rng;
}
static int sign(int n) { return (n > 0) - (n < 0); }
#include "../include/obfus.h"
#define CHECK(x)                                         \
    do {                                                 \
        if (!(x)) {                                      \
            fprintf(stderr, "COPY_FAIL:%d\n", __LINE__); \
            return 1;                                    \
        }                                                \
    } while (0)
int main(void) {
    unsigned char a[320], b[320], source[160];
    char text[80], extra[80];
    wchar_t wide[80], other[80];
    for (unsigned int trial = 0; trial < 1024; ++trial) {
        size_t n = next() % 129, offset = next() % 16;
        int value = (int)next();
        for (size_t j = 0; j < sizeof a; ++j) a[j] = b[j] = (unsigned char)next();
        for (size_t j = 0; j < sizeof source; ++j) source[j] = (unsigned char)next();
        CHECK(memcpy(a + offset, source + offset, n) == a + offset);
        ref_memcpy(b + offset, source + offset, n);
        CHECK(!ref_memcmp(a, b, sizeof a));
        CHECK(memset(a + offset, value, n) == a + offset);
        ref_memset(b + offset, value, n);
        CHECK(!ref_memcmp(a, b, sizeof a));
        size_t src = next() % 120, dst = next() % 120;
        CHECK(memmove(a + dst, a + src, n) == a + dst);
        ref_memmove(b + dst, b + src, n);
        CHECK(!ref_memcmp(a, b, sizeof a));
        CHECK(!(flags() & 0x400u));
        size_t length = next() % 40, suffix = next() % 40;
        for (size_t j = 0; j < length; ++j) text[j] = (char)(1 + next() % 255);
        text[length] = 0;
        for (size_t j = 0; j < suffix; ++j) extra[j] = (char)(1 + next() % 255);
        extra[suffix] = 0;
        ref_memset(a, 0xa5, sizeof a);
        ref_memset(b, 0xa5, sizeof b);
        CHECK(strcpy((char *)a, text) == (char *)a);
        ref_strcpy((char *)b, text);
        CHECK(!ref_memcmp(a, b, sizeof a));
        CHECK(strcat((char *)a, extra) == (char *)a);
        ref_strcat((char *)b, extra);
        CHECK(!ref_memcmp(a, b, sizeof a));
        n = next() % 60;
        ref_memset(a, 0xa5, sizeof a);
        ref_memset(b, 0xa5, sizeof b);
        CHECK(strncpy((char *)a, text, n) == (char *)a);
        ref_strncpy((char *)b, text, n);
        CHECK(!ref_memcmp(a, b, sizeof a));
        ref_strcpy((char *)a, text);
        ref_strcpy((char *)b, text);
        CHECK(strncat((char *)a, extra, n) == (char *)a);
        ref_strncat((char *)b, extra, n);
        CHECK(!ref_memcmp(a, b, sizeof a));
        CHECK(strchr(text, value) == ref_strchr(text, value));
        CHECK(strrchr(text, value) == ref_strrchr(text, value));
        CHECK(strchr(text, 0) == text + length && strrchr(text, 0) == text + length);
        for (size_t j = 0; j < length; ++j) wide[j] = 1 + next() % 65535;
        wide[length] = 0;
        for (size_t j = 0; j < suffix; ++j) other[j] = 1 + next() % 65535;
        other[suffix] = 0;
        CHECK(wcslen(wide) == ref_wcslen(wide));
        CHECK(sign(wcscmp(wide, other)) == sign(ref_wcscmp(wide, other)));
        CHECK(sign(wcsncmp(wide, other, n)) == sign(ref_wcsncmp(wide, other, n)));
        CHECK(wcsncmp(wide, wide, n) == 0);
        wchar_t wanted = (wchar_t)value;
        CHECK(wcschr(wide, wanted) == ref_wcschr(wide, wanted));
        CHECK(wcsrchr(wide, wanted) == ref_wcsrchr(wide, wanted));
        CHECK(wcschr(wide, 0) == wide + length && wcsrchr(wide, 0) == wide + length);
    }
    for (size_t n = 0; n < 96; ++n) {
        for (size_t delta = 0; delta < 32; ++delta) {
            for (size_t j = 0; j < sizeof a; ++j) a[j] = b[j] = (unsigned char)j;
            CHECK(memmove(a + 32 + delta, a + 32, n) == a + 32 + delta);
            ref_memmove(b + 32 + delta, b + 32, n);
            CHECK(!ref_memcmp(a, b, sizeof a));
            CHECK(memmove(a + 32, a + 32 + delta, n) == a + 32);
            ref_memmove(b + 32, b + 32 + delta, n);
            CHECK(!ref_memcmp(a, b, sizeof a));
            CHECK(!(flags() & 0x400u));
        }
    }
    int d = 0, s = 0, c = 0, v = 0;
    CHECK(memcpy((++d, a), (++s, source), (++c, 4)) == a && d == 1 && s == 1 && c == 1);
    CHECK(memset((++d, a), (++v, 0x180), (++c, 4)) == a && d == 2 && v == 1 && c == 2);
    CHECK(a[0] == 0x80 && a[3] == 0x80);
    CHECK(memmove((++d, a + 1), (++s, a), (++c, 3)) == a + 1 && d == 3 && s == 2 && c == 3);
    CHECK(strcpy((++d, (char *)a), (++s, "abc")) == (char *)a && d == 4 && s == 3);
    CHECK(strcat((++d, (char *)a), (++s, "def")) == (char *)a && d == 5 && s == 4);
    CHECK(strncpy((++d, (char *)a), (++s, "abc"), (++c, 6)) == (char *)a && d == 6 && s == 5 && c == 4);
    CHECK(strncat((++d, (char *)a), (++s, "def"), (++c, 2)) == (char *)a && d == 7 && s == 6 && c == 5);
    CHECK(strchr((++s, "ab"), (++v, 'b')) != NULL && s == 7 && v == 2);
    CHECK(strrchr((++s, "ab"), (++v, 'b')) != NULL && s == 8 && v == 3);
    CHECK(wcslen((++s, L"ab")) == 2 && s == 9);
    CHECK(wcscmp((++s, L"ab"), (++d, L"ab")) == 0 && s == 10 && d == 8);
    CHECK(wcsncmp((++s, L"ab"), (++d, L"ab"), (++c, 1)) == 0 && s == 11 && d == 9 && c == 6);
    CHECK(wcschr((++s, L"ab"), (++v, L'b')) != NULL && s == 12 && v == 4);
    CHECK(wcsrchr((++s, L"ab"), (++v, L'b')) != NULL && s == 13 && v == 5);
    SYSTEM_INFO info;
    GetSystemInfo(&info);
    size_t page = info.dwPageSize;
    unsigned char *guard = VirtualAlloc(NULL, page * 4, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    CHECK(guard != NULL);
    DWORD old;
    CHECK(VirtualProtect(guard + page, page, PAGE_NOACCESS, &old));
    CHECK(VirtualProtect(guard + page * 3, page, PAGE_NOACCESS, &old));
    unsigned char *end = guard + page, *dest = guard + page * 3;
    CHECK(memcpy(dest, end, 0) == dest && memset(dest, 0, 0) == dest && memmove(dest, end, 0) == dest);
    CHECK(strncpy((char *)dest, (char *)end, 0) == (char *)dest);
    for (size_t n = 1; n < 65; ++n) {
        for (size_t j = 0; j < n; ++j) (end - n)[j] = (unsigned char)(j + 1);
        CHECK(memcpy(dest - n, end - n, n) == dest - n);
        CHECK(!ref_memcmp(dest - n, end - n, n));
        CHECK(memset(dest - n, 0xff, n) == dest - n && dest[-1] == 0xff);
    }
    /* Exact end boundaries, small overlap distances and every vector tail. */
    for (size_t n = 64; n <= 512; ++n) {
        for (size_t delta = 1; delta <= 31; ++delta) {
            for (size_t j = 0; j < n + delta; ++j) (end - n - delta)[j] = (unsigned char)(j * 13);
            ref_memmove(dest - n - delta, end - n - delta, n + delta);
            ref_memmove(dest - n, dest - n - delta, n);
            CHECK(memmove(end - n, end - n - delta, n) == end - n);
            CHECK(!ref_memcmp(end - n - delta, dest - n - delta, n + delta));
        }
    }
    end[-1] = 0;
    dest[-1] = 0;
    CHECK(strcpy((char *)dest - 1, (char *)end - 1) == (char *)dest - 1);
    CHECK(strcat((char *)dest - 1, (char *)end - 1) == (char *)dest - 1);
    CHECK(strncat((char *)dest - 1, (char *)end, 0) == (char *)dest - 1);
    CHECK(strchr((char *)end - 1, 0) == (char *)end - 1 && strrchr((char *)end - 1, 'x') == NULL);
    wchar_t *wend = (wchar_t *)end;
    wend[-1] = 0;
    CHECK(wcslen(wend - 1) == 0 && wcscmp(wend - 1, L"") == 0 && wcsncmp(wend, (wchar_t *)dest, 0) == 0);
    CHECK(wcschr(wend - 1, 0) == wend - 1 && wcsrchr(wend - 1, L'x') == NULL);
    CHECK(VirtualFree(guard, 0, MEM_RELEASE));
    puts("CUSTOM_COPY_PASS");
    return 0;
}
