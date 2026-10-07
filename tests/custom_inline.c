#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <windows.h>
/* TCC ships strnlen with its declaration disabled; declare the CRT reference explicitly. */
extern size_t __cdecl strnlen(const char *, size_t);
static int compare_int(const void *a, const void *b) { return (*(const int *)a > *(const int *)b) - (*(const int *)a < *(const int *)b); }
#include "../include/obfus.h"
#undef if
#undef while
#undef for
#undef break
#undef else
#define if(...) OBFH_FORBIDDEN_INTERNAL_IF
#define while(...) OBFH_FORBIDDEN_INTERNAL_WHILE
#define for(...) OBFH_FORBIDDEN_INTERNAL_FOR
#define break OBFH_FORBIDDEN_INTERNAL_BREAK
#define else OBFH_FORBIDDEN_INTERNAL_ELSE
int main(void) {
    char text[] = "abcabc";
    char copy[64];
    wchar_t wide[] = L"abcabc";
    wchar_t wide_copy[64];
    int values[] = {0, 2, 4, 6}, key = 4;
    unsigned int fail = 0;
    fail |= !__builtin_types_compatible_p(__typeof__(memchr(text, 0, 6)), void *);
    fail |= !__builtin_types_compatible_p(__typeof__(memcmp(text, text, 6)), int);
    fail |= !__builtin_types_compatible_p(__typeof__(strncmp(text, text, 6)), int);
    fail |= !__builtin_types_compatible_p(__typeof__(strspn(text, "abc")), size_t);
    fail |= !__builtin_types_compatible_p(__typeof__(strcspn(text, "abc")), size_t);
    fail |= !__builtin_types_compatible_p(__typeof__(strpbrk(text, "abc")), char *);
    fail |= !__builtin_types_compatible_p(__typeof__(strstr(text, "abc")), char *);
    fail |= !__builtin_types_compatible_p(__typeof__(strcmp(text, text)), int);
    fail |= !__builtin_types_compatible_p(__typeof__(strlen(text)), size_t);
#if NO_OBF != 1
    fail |= strcmp("a", "z") != -1;
    fail |= strcmp("z", "a") != 1;
    fail |= strcmp("", "") != 0;
#endif
    fail |= !__builtin_types_compatible_p(__typeof__(strchr(text, 0)), char *);
    fail |= !__builtin_types_compatible_p(__typeof__(strrchr(text, 0)), char *);
    fail |= !__builtin_types_compatible_p(__typeof__(strcpy(copy, text)), char *);
    fail |= !__builtin_types_compatible_p(__typeof__(strcat(copy, text)), char *);
    fail |= !__builtin_types_compatible_p(__typeof__(strncpy(copy, text, 6)), char *);
    fail |= !__builtin_types_compatible_p(__typeof__(strncat(copy, text, 6)), char *);
    fail |= !__builtin_types_compatible_p(__typeof__(memcpy(copy, text, 6)), void *);
    fail |= !__builtin_types_compatible_p(__typeof__(memset(copy, 0, 6)), void *);
    fail |= !__builtin_types_compatible_p(__typeof__(memmove(copy, text, 6)), void *);
    fail |= !__builtin_types_compatible_p(__typeof__(wcslen(wide)), size_t);
    fail |= !__builtin_types_compatible_p(__typeof__(wcscmp(wide, wide)), int);
    fail |= !__builtin_types_compatible_p(__typeof__(wcsncmp(wide, wide, 6)), int);
    fail |= !__builtin_types_compatible_p(__typeof__(wcschr(wide, 0)), wchar_t *);
    fail |= !__builtin_types_compatible_p(__typeof__(wcsrchr(wide, 0)), wchar_t *);
    fail |= strcpy(copy, text) != copy;
    fail |= strcat(strcpy(copy, "ab"), "cd") != copy;
    fail |= strcmp(copy, "abcd") != 0;
    fail |= strncpy(copy, "a", sizeof copy) != copy || copy[63] != 0;
    fail |= strncat(copy, "bcdef", 2) != copy || strcmp(copy, "abc") != 0;
    fail |= strchr(strrchr(text, 'b'), 'c') != text + 5;
    fail |= memcpy(copy, text, 7) != copy;
    fail |= memmove(copy + 1, copy, 6) != copy + 1;
    fail |= memcmp(copy, "aabcabc", 7) != 0;
    fail |= memset(copy, 0x180, sizeof copy) != copy || (unsigned char)copy[63] != 0x80;
    fail |= wcslen(wide) != 6 || wcscmp(wide, L"abcabc") != 0;
    fail |= wcsncmp(wide, L"abcxyz", 3) != 0;
    fail |= wcschr(wcsrchr(wide, L'b'), L'c') != wide + 5;
    fail |= memchr(text, 'b', 6) != text + 1;
    fail |= strspn(text, "abc") != 6;
    fail |= strcspn(text, "b") != 1;
    fail |= strpbrk(text, "c") != text + 2;
    fail |= memcmp(text, "abc", 3) != 0;
    fail |= strncmp(text, "abc", 3) != 0;
    fail |= strstr(strstr(text, "bc"), "ca") != text + 2;
    fail |= strcmp(text, "abcabc") != 0;
    fail |= strlen(text) != 6;
    fail |= GetProcAddress(GetModuleHandleA("kernel32.dll"), "GetTickCount") == NULL;
    fail |= !__builtin_types_compatible_p(__typeof__(wcscpy(wide_copy, wide)), wchar_t *);
    fail |= !__builtin_types_compatible_p(__typeof__(wcscat(wide_copy, wide)), wchar_t *);
    fail |= !__builtin_types_compatible_p(__typeof__(wcsncpy(wide_copy, wide, 6)), wchar_t *);
    fail |= !__builtin_types_compatible_p(__typeof__(wcsncat(wide_copy, wide, 6)), wchar_t *);
    fail |= !__builtin_types_compatible_p(__typeof__(wmemcpy(wide_copy, wide, 6)), wchar_t *);
    fail |= !__builtin_types_compatible_p(__typeof__(wmemmove(wide_copy, wide, 6)), wchar_t *);
    fail |= !__builtin_types_compatible_p(__typeof__(wmemset(wide_copy, 0, 6)), wchar_t *);
    fail |= !__builtin_types_compatible_p(__typeof__(wmemcmp(wide, wide, 6)), int);
    fail |= !__builtin_types_compatible_p(__typeof__(wmemchr(wide, 0, 6)), wchar_t *);
    fail |= !__builtin_types_compatible_p(__typeof__(wcsstr(wide, wide)), wchar_t *);
    fail |= !__builtin_types_compatible_p(__typeof__(wcsspn(wide, wide)), size_t);
    fail |= !__builtin_types_compatible_p(__typeof__(wcscspn(wide, wide)), size_t);
    fail |= !__builtin_types_compatible_p(__typeof__(wcspbrk(wide, wide)), wchar_t *);
    fail |= !__builtin_types_compatible_p(__typeof__(strnlen(text, 6)), size_t);
    fail |= !__builtin_types_compatible_p(__typeof__(wcsnlen(wide, 6)), size_t);
    fail |= !__builtin_types_compatible_p(__typeof__(bsearch(&key, values, 4, sizeof(int), compare_int)), void *);
    fail |= wcscat(wcscpy(wide_copy, L"ab"), L"cd") != wide_copy || wcscmp(wide_copy, L"abcd") != 0;
    fail |= wcsncpy(wide_copy, L"a", 64) != wide_copy || wide_copy[63] != 0;
    fail |= wcsncat(wide_copy, L"bcdef", 2) != wide_copy || wcscmp(wide_copy, L"abc") != 0;
    fail |= wmemcpy(wide_copy, wide, 7) != wide_copy;
    fail |= wmemmove(wide_copy + 1, wide_copy, 6) != wide_copy + 1;
    fail |= wmemcmp(wide_copy, L"aabcabc", 7) != 0;
    fail |= wmemset(wide_copy, 0x1234, 64) != wide_copy || wide_copy[63] != 0x1234;
    fail |= wmemchr(wide_copy, 0x1234, 64) != wide_copy;
    fail |= wcsstr(wcsstr(wide, L"bc"), L"ca") != wide + 2;
    fail |= wcsspn(wide, L"ab") != 2 || wcscspn(wide, L"b") != 1 || wcspbrk(wide, L"c") != wide + 2;
    fail |= strnlen(text, 3) != 3 || wcsnlen(wide, 4) != 4;
    fail |= bsearch(&key, values, 4, sizeof(int), compare_int) != values + 2;
    ExitProcess(fail);
}
