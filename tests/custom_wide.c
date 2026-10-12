#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <windows.h>
/* TCC ships strnlen with its declaration disabled; declare the CRT reference explicitly. */
extern size_t __cdecl strnlen(const char *, size_t);
/* Reference calls are compiled before public interception. */
static wchar_t *ref_wcscpy(wchar_t *d, const wchar_t *s) { return wcscpy(d, s); }
static wchar_t *ref_wcscat(wchar_t *d, const wchar_t *s) { return wcscat(d, s); }
static wchar_t *ref_wcsncpy(wchar_t *d, const wchar_t *s, size_t n) { return wcsncpy(d, s, n); }
static wchar_t *ref_wcsncat(wchar_t *d, const wchar_t *s, size_t n) { return wcsncat(d, s, n); }
static wchar_t *ref_wmemcpy(wchar_t *d, const wchar_t *s, size_t n) { return wmemcpy(d, s, n); }
static wchar_t *ref_wmemmove(wchar_t *d, const wchar_t *s, size_t n) { return wmemmove(d, s, n); }
static wchar_t *ref_wmemset(wchar_t *d, wchar_t c, size_t n) { return wmemset(d, c, n); }
static int ref_wmemcmp(const wchar_t *a, const wchar_t *b, size_t n) { return wmemcmp(a, b, n); }
static wchar_t *ref_wmemchr(const wchar_t *s, wchar_t c, size_t n) { return wmemchr(s, c, n); }
static wchar_t *ref_wcsstr(const wchar_t *s, const wchar_t *p) { return wcsstr(s, p); }
static size_t ref_wcsspn(const wchar_t *s, const wchar_t *p) { return wcsspn(s, p); }
static size_t ref_wcscspn(const wchar_t *s, const wchar_t *p) { return wcscspn(s, p); }
static wchar_t *ref_wcspbrk(const wchar_t *s, const wchar_t *p) { return wcspbrk(s, p); }
static size_t ref_strnlen(const char *s, size_t n) { return strnlen(s, n); }
static size_t ref_wcsnlen(const wchar_t *s, size_t n) { return wcsnlen(s, n); }
static void *ref_bsearch(const void *k, const void *b, size_t n, size_t z, int (*f)(const void *, const void *)) { return bsearch(k, b, n, z, f); }
static int bytes_equal(const void *a, const void *b, size_t n) { return memcmp(a, b, n) == 0; }
static unsigned int rng = 12345;
static unsigned int next(void) {
    rng ^= rng << 13;
    rng ^= rng >> 17;
    rng ^= rng << 5;
    return rng;
}
static int sign(int v) { return (v > 0) - (v < 0); }
struct record {
    int key;
    unsigned char payload[9];
};
static unsigned int calls;
static int compare(const void *k, const void *item) {
    ++calls;
    int a = *(const int *)k, b = ((const struct record *)item)->key;
    return a < b ? INT_MIN : a > b ? INT_MAX
                                   : 0;
}
static int fail_compare(const void *a, const void *b) {
    ExitProcess(99);
    return 0;
}
#include "../include/obfus.h"
#define CHECK(x) \
    do { \
        if (!(x)) { \
            fprintf(stderr, "WIDE_FAIL:%d\n", __LINE__); \
            return 1; \
        } \
    } while (0)
static int inner_key = 4, inner_values[] = {0, 2, 4, 6, 8};
static int compare_int(const void *a, const void *b) { return (*(const int *)a > *(const int *)b) - (*(const int *)a < *(const int *)b); }
static int reentrant_compare(const void *a, const void *b) {
    int *p = bsearch(&inner_key, inner_values, 5, sizeof(int), compare_int);
    if (!p || *p != 4) ExitProcess(98);
    return compare(a, b);
}
int main(void) {
    wchar_t a[192], b[192], source[128], text[80], extra[80];
    char narrow[80];
    for (unsigned int trial = 0; trial < 512; ++trial) {
        size_t length = next() % 40, suffix = next() % 40, n = next() % 96;
        for (size_t j = 0; j < length; ++j) {
            text[j] = (wchar_t)(1 + next() % 65535);
            narrow[j] = (char)(1 + next() % 255);
        }
        text[length] = 0;
        narrow[length] = 0;
        for (size_t j = 0; j < suffix; ++j) extra[j] = (wchar_t)(1 + next() % 65535);
        extra[suffix] = 0;
        ref_wmemset(a, 0xa55a, 192);
        ref_wmemset(b, 0xa55a, 192);
        CHECK(wcscpy(a, text) == a);
        ref_wcscpy(b, text);
        CHECK(bytes_equal(a, b, sizeof a));
        CHECK(wcscat(a, extra) == a);
        ref_wcscat(b, extra);
        CHECK(bytes_equal(a, b, sizeof a));
        ref_wmemset(a, 0xa55a, 192);
        ref_wmemset(b, 0xa55a, 192);
        CHECK(wcsncpy(a, text, n) == a);
        ref_wcsncpy(b, text, n);
        CHECK(bytes_equal(a, b, sizeof a));
        ref_wcscpy(a, text);
        ref_wcscpy(b, text);
        CHECK(wcsncat(a, extra, n) == a);
        ref_wcsncat(b, extra, n);
        CHECK(bytes_equal(a, b, sizeof a));
        CHECK(wcsstr(text, extra) == ref_wcsstr(text, extra));
        CHECK(wcsstr(text, text) == text && wcsstr(text, L"") == text);
        CHECK(wcsstr(text, text + length / 2) == ref_wcsstr(text, text + length / 2));
        CHECK(wcsspn(text, extra) == ref_wcsspn(text, extra));
        CHECK(wcscspn(text, extra) == ref_wcscspn(text, extra));
        CHECK(wcspbrk(text, extra) == ref_wcspbrk(text, extra));
        CHECK(wcsspn(text, text) == length && wcscspn(text, L"") == length);
        CHECK(strnlen(narrow, n) == ref_strnlen(narrow, n));
        CHECK(wcsnlen(text, n) == ref_wcsnlen(text, n));
        for (size_t j = 0; j < 128; ++j) source[j] = (wchar_t)next();
        for (size_t j = 0; j < 192; ++j) a[j] = b[j] = (wchar_t)next();
        size_t offset = next() % 16;
        CHECK(wmemcpy(a + offset, source + offset, n) == a + offset);
        ref_wmemcpy(b + offset, source + offset, n);
        CHECK(bytes_equal(a, b, sizeof a));
        wchar_t value = (wchar_t)next();
        CHECK(wmemset(a + offset, value, n) == a + offset);
        ref_wmemset(b + offset, value, n);
        CHECK(bytes_equal(a, b, sizeof a));
        size_t src = next() % 64, dst = next() % 64;
        CHECK(wmemmove(a + dst, a + src, n) == a + dst);
        ref_wmemmove(b + dst, b + src, n);
        CHECK(bytes_equal(a, b, sizeof a));
        CHECK(wmemchr(source, value, n) == ref_wmemchr(source, value, n));
        CHECK(wmemchr(source, source[0], 128) == source);
        CHECK(sign(wmemcmp(a, source, n)) == sign(ref_wmemcmp(a, source, n)));
        CHECK(wmemcmp(source, source, n) == 0);
    }
    /* Memory comparison must use wchar_t values, not little-endian byte order. */
    wchar_t high[] = {0x100, 0xffff, 0}, low[] = {0xff, 0x8000, 0};
    CHECK(wmemcmp(high, low, 1) > 0 && wmemcmp(low, high, 1) < 0);
    CHECK(wmemcmp(high + 1, low + 1, 1) > 0 && wmemcmp(high, low, 0) == 0);
    wchar_t repeated[] = L"aaaaab";
    CHECK(wcsstr(repeated, L"aaab") == repeated + 2);
    struct record sorted[64];
    for (size_t j = 0; j < 64; ++j) sorted[j].key = (int)j * 3 - 60;
    for (size_t count = 0; count <= 64; ++count) {
        for (int key = -65; key <= 135; ++key) {
            calls = 0;
            struct record *p = bsearch(&key, sorted, count, sizeof sorted[0], compare);
            unsigned int work = calls;
            struct record *q = ref_bsearch(&key, sorted, count, sizeof sorted[0], compare);
            CHECK((p != NULL) == (q != NULL));
            CHECK(!p || (p >= sorted && p < sorted + count && p->key == key));
            CHECK(work <= 7 && (count || !work));
        }
    }
    for (size_t j = 0; j < 64; ++j) sorted[j].key = (int)(j / 3);
    for (int key = -1; key < 24; ++key) {
        struct record *p = bsearch(&key, sorted, 64, sizeof sorted[0], compare);
        CHECK((key >= 0 && key <= 21) ? p && p->key == key : !p);
    }
    int key = 12;
    CHECK(bsearch(&key, sorted, 64, sizeof sorted[0], reentrant_compare) != NULL);
    /* Every macro argument is captured once; independent counters avoid evaluation-order assumptions. */
    int d = 0, s = 0, n = 0, v = 0, k = 0, f = 0;
    CHECK(wcscpy((++d, a), (++s, L"ab")) == a && d == 1 && s == 1);
    CHECK(wcscat((++d, a), (++s, L"cd")) == a && d == 2 && s == 2);
    CHECK(wcsncpy((++d, a), (++s, L"x"), (++n, 8)) == a && d == 3 && s == 3 && n == 1);
    CHECK(wcsncat((++d, a), (++s, L"yz"), (++n, 1)) == a && d == 4 && s == 4 && n == 2);
    CHECK(wmemcpy((++d, a), (++s, source), (++n, 8)) == a && d == 5 && s == 5 && n == 3);
    CHECK(wmemmove((++d, a + 1), (++s, a), (++n, 8)) == a + 1 && d == 6 && s == 6 && n == 4);
    CHECK(wmemset((++d, a), (++v, 0x1234), (++n, 8)) == a && d == 7 && v == 1 && n == 5);
    CHECK(wmemcmp((++d, a), (++s, a), (++n, 8)) == 0 && d == 8 && s == 7 && n == 6);
    CHECK(wmemchr((++s, a), (++v, 0x1234), (++n, 8)) == a && s == 8 && v == 2 && n == 7);
    CHECK(wcsstr((++s, L"abc"), (++d, L"bc")) != NULL && s == 9 && d == 9);
    CHECK(wcsspn((++s, L"abc"), (++d, L"ab")) == 2 && s == 10 && d == 10);
    CHECK(wcscspn((++s, L"abc"), (++d, L"c")) == 2 && s == 11 && d == 11);
    CHECK(wcspbrk((++s, L"abc"), (++d, L"b")) != NULL && s == 12 && d == 12);
    CHECK(strnlen((++s, "ab"), (++n, 1)) == 1 && s == 13 && n == 8);
    CHECK(wcsnlen((++s, L"ab"), (++n, 1)) == 1 && s == 14 && n == 9);
    CHECK(bsearch((++k, &key), (++d, sorted), (++n, 64), (++v, sizeof sorted[0]), (++f, compare)) != NULL && k == 1 && d == 13 && n == 10 && v == 3 && f == 1);
    SYSTEM_INFO info;
    GetSystemInfo(&info);
    size_t page = info.dwPageSize;
    unsigned char *pages = VirtualAlloc(NULL, page * 4, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    CHECK(pages != NULL);
    DWORD old;
    CHECK(VirtualProtect(pages + page, page, PAGE_NOACCESS, &old));
    CHECK(VirtualProtect(pages + page * 3, page, PAGE_NOACCESS, &old));
    wchar_t *end = (wchar_t *)(pages + page), *dest = (wchar_t *)(pages + page * 3);
    CHECK(wmemcpy(dest, end, 0) == dest && wmemmove(dest, end, 0) == dest && wmemset(dest, 0xffff, 0) == dest);
    CHECK(wmemcmp(end, dest, 0) == 0 && wmemchr(end, 0, 0) == NULL);
    CHECK(wcsncpy(dest, end, 0) == dest && wcsnlen(end, 0) == 0 && strnlen((char *)end, 0) == 0);
    CHECK(bsearch(end, dest, 0, sizeof(int), fail_compare) == NULL);
    for (size_t count = 1; count <= 128; ++count) {
        for (size_t j = 0; j < count; ++j) (end - count)[j] = (wchar_t)(j + 1);
        CHECK(wmemcpy(dest - count, end - count, count) == dest - count);
        CHECK(bytes_equal(dest - count, end - count, count * sizeof(wchar_t)));
        CHECK(wmemset(dest - count, 0xfedc, count) == dest - count && dest[-1] == 0xfedc);
        CHECK(wcsnlen(end - count, count) == count); /* byte buffer filled below */
        for (size_t delta = 1; delta <= 15; ++delta) {
            for (size_t j = 0; j < count + delta; ++j) (end - count - delta)[j] = (wchar_t)(j * 17);
            ref_wmemmove(dest - count - delta, end - count - delta, count + delta);
            ref_wmemmove(dest - count, dest - count - delta, count);
            CHECK(wmemmove(end - count, end - count - delta, count) == end - count);
            CHECK(bytes_equal(end - count - delta, dest - count - delta, (count + delta) * sizeof(wchar_t)));
        }
    }
    memset((char *)end - 127, 0x80, 127);
    CHECK(strnlen((char *)end - 127, 127) == 127);
    end[-1] = 0;
    dest[-1] = 0;
    CHECK(wcscpy(dest - 1, end - 1) == dest - 1 && wcscat(dest - 1, end - 1) == dest - 1);
    CHECK(wcsncat(dest - 1, end, 0) == dest - 1);
    CHECK(wcsspn(end - 1, L"a") == 0 && wcscspn(end - 1, L"a") == 0 && wcspbrk(end - 1, L"a") == NULL);
    CHECK(wcsstr(end - 1, L"a") == NULL && wcsstr(end - 1, end - 1) == end - 1);
    end[-3] = L'a';
    end[-2] = L'a';
    end[-1] = 0;
    CHECK(wcsstr(end - 3, L"aaa") == NULL && wcsstr(end - 3, L"aa") == end - 3);
    CHECK(VirtualFree(pages, 0, MEM_RELEASE));
    puts("CUSTOM_WIDE_PASS");
    return 0;
}
