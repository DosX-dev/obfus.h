#include <errno.h>
#include <stdio.h>
#include "../include/obfus.h"
#undef if
#undef else
#undef for
#undef while
#undef return
#undef printf
#undef puts
#undef fflush
#undef CreateThread
#undef CreateEventA
#undef WaitForSingleObject
#undef WaitForMultipleObjects
#undef SetEvent
#undef CloseHandle
#undef SetLastError
#undef GetLastError

#if !NO_OBF && !OBFH_EDITOR_VIEW
static HANDLE start;
static int name_indices_valid(void) {
    unsigned char storage[20];
    char *name = (char *)storage + 1;
    for (unsigned n = 0; n < sizeof(storage); ++n) storage[n] = 0xA5;
    name[OBFH_NAME_INDEX(0)] = 1;
    name[OBFH_NAME_INDEX(1)] = 2;
    name[OBFH_NAME_INDEX(2)] = 3;
    name[OBFH_NAME_INDEX(3)] = 4;
    name[OBFH_NAME_INDEX(4)] = 5;
    name[OBFH_NAME_INDEX(5)] = 6;
    name[OBFH_NAME_INDEX(6)] = 7;
    name[OBFH_NAME_INDEX(7)] = 8;
    name[OBFH_NAME_INDEX(8)] = 9;
    name[OBFH_NAME_INDEX(9)] = 10;
    name[OBFH_NAME_INDEX(10)] = 11;
    name[OBFH_NAME_INDEX(11)] = 12;
    name[OBFH_NAME_INDEX(12)] = 13;
    name[OBFH_NAME_INDEX(13)] = 14;
    name[OBFH_NAME_INDEX(14)] = 15;
    name[OBFH_NAME_INDEX(15)] = 16;
    name[OBFH_NAME_INDEX(16)] = 17;
    name[OBFH_NAME_INDEX(17)] = 18;
    for (unsigned n = 0; n < 18; ++n)
        if ((unsigned char)name[n] != n + 1) return 0;
    if (storage[0] != 0xA5 || storage[19] != 0xA5) return 0;
    for (unsigned n = 0; n < sizeof(storage); ++n) storage[n] = 0xA5;
    name[OBFH_NAME_INDEX(17)] = 18;
    name[OBFH_NAME_INDEX(16)] = 17;
    name[OBFH_NAME_INDEX(15)] = 16;
    name[OBFH_NAME_INDEX(14)] = 15;
    name[OBFH_NAME_INDEX(13)] = 14;
    name[OBFH_NAME_INDEX(12)] = 13;
    name[OBFH_NAME_INDEX(11)] = 12;
    name[OBFH_NAME_INDEX(10)] = 11;
    name[OBFH_NAME_INDEX(9)] = 10;
    name[OBFH_NAME_INDEX(8)] = 9;
    name[OBFH_NAME_INDEX(7)] = 8;
    name[OBFH_NAME_INDEX(6)] = 7;
    name[OBFH_NAME_INDEX(5)] = 6;
    name[OBFH_NAME_INDEX(4)] = 5;
    name[OBFH_NAME_INDEX(3)] = 4;
    name[OBFH_NAME_INDEX(2)] = 3;
    name[OBFH_NAME_INDEX(1)] = 2;
    name[OBFH_NAME_INDEX(0)] = 1;
    for (unsigned n = 0; n < 18; ++n)
        if ((unsigned char)name[n] != n + 1) return 0;
    return storage[0] == 0xA5 && storage[19] == 0xA5;
}

static int letters_valid(void) {
    return _a == 'a' && _b == 'b' && _c == 'c' && _d == 'd' && _e == 'e' &&
           _f == 'f' && _g == 'g' && _h == 'h' && _i == 'i' && _j == 'j' &&
           _k == 'k' && _l == 'l' && _m == 'm' && _n == 'n' && _o == 'o' &&
           _p == 'p' && _q == 'q' && _r == 'r' && _s == 's' && _t == 't' &&
           _u == 'u' && _v == 'v' && _w == 'w' && _x == 'x' && _y == 'y' &&
           _z == 'z' && _S == 'S' && _L == 'L' && _A == 'A' && _I == 'I' &&
           _D == 'D' && _P == 'P';
}
static DWORD WINAPI initialize(LPVOID parameter) {
    if (WaitForSingleObject(start, 10000) != WAIT_OBJECT_0) return 1;
    SetLastError(0x4711u);
    errno = E2BIG;
    for (unsigned n = 0; n < 4096; ++n) {
        OBFH_LOADER;
        if (!letters_valid() || _0 != 0 || _2 != 2) return 2;
    }
    return GetLastError() != 0x4711u || errno != E2BIG;
}
#endif

int main(int argc, char **argv) {
#if !NO_OBF && !OBFH_EDITOR_VIEW
    if (obfh_alpha_low_state || obfh_alpha_high_state || obfh_alpha_upper_state ||
        _a == 'a' || _n == 'n' || _S == 'S' || _0 != 0 || _2 != 2) return 1;
    if (argc > 1 && argv[1][0] == 'c') {
        HANDLE threads[32];
        start = CreateEventA(NULL, TRUE, FALSE, NULL);
        if (!start) return 2;
        for (unsigned n = 0; n < 32; ++n) {
            threads[n] = CreateThread(NULL, 0, initialize, NULL, 0, NULL);
            if (!threads[n]) return 3;
        }
        if (!SetEvent(start) || WaitForMultipleObjects(32, threads, TRUE, 10000) != WAIT_OBJECT_0) return 4;
        for (unsigned n = 0; n < 32; ++n) {
            DWORD code;
            if (!GetExitCodeThread(threads[n], &code) || code) return 5;
            CloseHandle(threads[n]);
        }
        CloseHandle(start);
    } else if (argc > 1 && argv[1][0] == 'a') {
        if (!obfh_format_has_count("%n") || obfh_format_has_count("%%n") ||
            obfh_alpha_low_state || obfh_alpha_high_state != 2 || obfh_alpha_upper_state) return 6;
        void *allocation = malloc_proxy(16);
        if (!allocation || obfh_alpha_low_state != 2 || obfh_alpha_upper_state) return 7;
        free(allocation);
        OBFH_LOADER;
    } else {
        SetLastError(0x4711u);
        errno = E2BIG;
        OBFH_LOADER_FOR(OBFH_ALPHA_LOW);
        if (_a != 'a' || _m != 'm' || obfh_alpha_low_state != 2 ||
            obfh_alpha_high_state || obfh_alpha_upper_state || _n == 'n' || _S == 'S') return 8;
        for (unsigned n = 0; n < 4096; ++n) OBFH_LOADER_FOR(OBFH_ALPHA_LOW);
        if (_a != 'a' || _m != 'm' || GetLastError() != 0x4711u || errno != E2BIG) return 9;
        OBFH_LOADER;
        OBFH_LOADER;
    }
    if (!letters_valid()) return 10;
    if (!name_indices_valid()) return 11;
#else
    OBFH_LOADER;
    OBFH_LOADER;
#endif
    puts("CONSTANT_LOADER_PASS");
    return fflush(stdout) == EOF;
}
