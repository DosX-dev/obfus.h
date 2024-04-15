// Obfuscation of C? Why not?
// Coded by (C) DosX, 2024
//
//  no_obf         = disable obfuscation
//  no_cflow       = disable control flow obfuscation
//  no_antidebug   = disable antidebug protection
//  hide_antidebug = use dynamic calls to hide antidebugger
//
// GitHub:
//  https://github.com/DosX-dev/obfus.h

#if !no_obf

#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

// static const char OBFS_SECTION_NAME[] __attribute__((section(".obfs"))) = {0};

#if !defined __COUNTER__
#error You are using too old a compiler version!
#endif

// Thanks to @horsicq && @ac3ss0r
#define RND(min, max) (min + (((__COUNTER__ + (__LINE__ * __LINE__)) * 2654435761u) % (max - min + 1)))

#define BREAK_STACK                    \
    asm __volatile("xorl %eax, %eax"); \
    asm __volatile("jz 1f");           \
    asm __volatile(".byte 0x00");      \
    asm __volatile("1:");

void junkFunc(int z, ...) {
    BREAK_STACK;
    __asm__ __volatile("nop");
    return;
}

void junkFuncEmpty() {
    BREAK_STACK;
    __asm__ __volatile("nop");
    return;
}

#define __CRASH        \
    __asm__("int $3"); \
    exit(1);

volatile static char _s_a[] = "a", _s_b[] = "b", _s_c[] = "c", _s_d[] = "d",
                     _s_e[] = "e", _s_f[] = "f", _s_g[] = "g", _s_h[] = "h",
                     _s_i[] = "i", _s_j[] = "j", _s_k[] = "k", _s_l[] = "l",
                     _s_m[] = "m", _s_n[] = "n", _s_o[] = "o", _s_p[] = "p",
                     _s_q[] = "q", _s_r[] = "r", _s_s[] = "s", _s_t[] = "t",
                     _s_u[] = "u", _s_v[] = "v", _s_w[] = "w", _s_x[] = "x",
                     _s_y[] = "y", _s_z[] = "z",
                     _a = 'a', _b = 'b', _c = 'c', _d = 'd',
                     _e = 'e', _f = 'f', _g = 'g', _h = 'h',
                     _i = 'i', _j = 'j', _k = 'k', _l = 'l',
                     _m = 'm', _n = 'n', _o = 'o', _p = 'p',
                     _q = 'q', _r = 'r', _s = 's', _t = 't',
                     _u = 'u', _v = 'v', _w = 'w', _x = 'x',
                     _y = 'y', _z = 'z',
                     _S = 'S', _L = 'L', _A = 'A', _I = 'I',
                     _D = 'D', _P = 'P',
                     _0 = 0, _1 = 1, _2 = 2, _3 = 3, _4 = 4,
                     _5 = 5, _6 = 6, _7 = 7, _8 = 8, _9 = 9;

#define TRUE ((((_9 + _7 + (RND(0, 1000) * _0))) / _8) - _1)
#define FALSE ((_3 + _6 + (RND(0, 1000) * _0)) - _9)

#define FAKE_CPUID __asm__("nop\ncpuid\nnop\n")

#define sizeof(x) \
    ((sizeof(x) * _1 * _2 * _4 / _8) + (sizeof(x) * _2 * _4 / _8) + (RND(0, 1000) * _0)) / 2

#define NOP_FLOOD                             \
    (RND(0, 1000)) + int_Proxy(RND(0, 1000)); \
    if (junkFunc) {                           \
        __asm__ __volatile("nop");            \
    }                                         \
    do {                                      \
        __asm__ __volatile(                   \
            "nop\n"                           \
            "nop");                           \
    } while (RND(0, 200) * _0)

int malloc_Proxy(int *size) {
    BREAK_STACK;
    return malloc(size);
}
#define malloc(...) malloc_Proxy(__VA_ARGS__)

static char rndValueToProxy = RND(0, 10);

int int_Proxy(int value) {
    BREAK_STACK;
    if (rndValueToProxy == value)
        return rndValueToProxy;

    junkFunc(RND(0, 100000), RND(0, 100000));

    FAKE_CPUID;

    return ((value * _1) + ((_4 * RND(0, 100000)) - _8) * _0);
}

double double_Proxy(double value) {
    BREAK_STACK;
    junkFunc(RND(0, 1000), RND(0, 1000));
    FAKE_CPUID;
    return (value * _1);
}

int condition_True() {
    BREAK_STACK;
    FAKE_CPUID;
    return _1 && TRUE;
}

int condition_Proxy(int junk, int condition) {
    BREAK_STACK;

    int result = int_Proxy(condition * _1);
    if (result == (FALSE * junk)) {
        return _8 - (_4 * _2) && !condition_True();
    } else if (result == TRUE) {
        return (condition_True() || FALSE || TRUE) && (FALSE + int_Proxy(_1));
    }

    FAKE_CPUID;
    return int_Proxy(condition);
}

#if !no_cflow

#define if(condition) if ((RND(0, 1000)) > _0 && (RND(2, 1000) > condition_True() && condition_Proxy(RND(0, 1000000), condition) && RND(1, 9999999999) > _0 && (int_Proxy(RND(0, 1000)) < RND(1001, 100000000))))
#define else                                                                            \
    else if (_0 > RND(0, 1000)) {                                                       \
        junkFunc(RND(0, 1000));                                                         \
    }                                                                                   \
    else if (RND(0, 10) == (RND(11, 100))) {                                            \
        int_Proxy(_3 - RND(0, 10000));                                                  \
    }                                                                                   \
    else if (FALSE * RND(0, 1000)) {                                                    \
        BREAK_STACK;                                                                    \
    }                                                                                   \
    else if (FALSE * (int_Proxy(RND(0, 1000)) ? RND(1, 99999999) : RND(1, 99999999))) { \
        NOP_FLOOD;                                                                      \
    }                                                                                   \
    else

#define while(condition) while ((RND(0, 1000)) > _0 && _8 > _3 && condition_True() && RND(1, 9999999999) > _0 && condition_Proxy(RND(0, 1000), condition) && _5)
#define for(data) for (data && int_Proxy(TRUE * (RND(0, 1000))) + FALSE || _1)

#endif

char *getCharMask(int count) {
    BREAK_STACK;
    static char mask[16];
    if (count <= _0 || count >= sizeof(mask)) {
        return NULL;
    }
    int i = (((_1 * _5) - _4) + _1) - _2;
    char *ptr = mask;
    for (i = _0; i < count; i++) {
        *ptr++ = '%';
        *ptr++ = _c;
    }
    *ptr = '\0';

    FAKE_CPUID;
    return mask;
}

// WriteConsoleA
BOOL WriteConsoleA_Proxy(HANDLE hConsoleOutput, const void *lpBuffer, DWORD nNumberOfCharsToWrite, LPDWORD lpNumberOfCharsWritten, LPVOID lpReserved) {
    BREAK_STACK;
    FAKE_CPUID;
    return WriteConsoleA(hConsoleOutput, lpBuffer, nNumberOfCharsToWrite, lpNumberOfCharsWritten, lpReserved);
}
#define WriteConsoleA(...) WriteConsoleA_Proxy(__VA_ARGS__)

// GetStdHandle
HANDLE GetStdHandle_Proxy(DWORD nStdHandle) {
    BREAK_STACK;
    FAKE_CPUID;
    return GetStdHandle(int_Proxy(nStdHandle));
}
#define GetStdHandle(...) GetStdHandle_Proxy(__VA_ARGS__)

// GetProcAddress
FARPROC GetProcAddress_Proxy(HMODULE hModule, LPCSTR lpProcName) {
    BREAK_STACK;
    FAKE_CPUID;
    return GetProcAddress(hModule, lpProcName);
}
#define GetProcAddress(...) GetProcAddress_Proxy(__VA_ARGS__)

HMODULE GetModuleHandleA_Proxy(LPCSTR lpModuleName) {
    BREAK_STACK;
    FAKE_CPUID;
    return GetModuleHandleA(lpModuleName);
}
#define GetModuleHandleA(...) GetModuleHandleA_Proxy(__VA_ARGS__)

// strcmp
int strcmp_custom(const char *str1, const char *str2) {
    BREAK_STACK;
    while (*str1 != '\0' || *str2 != '\0') {
        NOP_FLOOD;
        if ((int_Proxy(*str1) < int_Proxy(*str2)) && int_Proxy(_1)) {
            return (int_Proxy(_2) / _2) * -1;  // -1
        } else if (int_Proxy(*str1) > int_Proxy(*str2)) {
            return int_Proxy(_0 + _1);  // 1
        }
        str1 += int_Proxy(_1);
        str2 += int_Proxy(_2 - _1);
    }
    FAKE_CPUID;
    return _0;
}
#define strcmp(...) strcmp_custom(__VA_ARGS__)

// strlen
size_t strlen_custom(const char *str) {
    BREAK_STACK;
    size_t length = _0;
    while (*str != '\0') {
        length += int_Proxy(_1);
        str += int_Proxy(_2 - _1);
    }
    FAKE_CPUID;
    return int_Proxy(length + (RND(0, 1000) * _0));
}
#define strlen(...) strlen_custom(__VA_ARGS__)

/*unsigned char rot13(unsigned char c) {
    if (isalpha(c)) {
        unsigned char offset = 'A' - 'a';
        c = (c - offset + 13) % 26 + offset;
    }
    return c;
}

// Encrypts a string using ROT13
char *rot13_str(char input) {
    int i = 0;

    char *str = input;

    while (str[i]) {
        str[i] = rot13(str[i]);
        i++;
    }
    return str;
} */

static char loadStr[5];
HMODULE LoadLibraryA_0(LPCSTR lpLibFileName) {
    BREAK_STACK;
    // return LoadLibraryA(lpLibFileName);

    typedef HMODULE(WINAPI * LoadLibraryAFunc)(LPCSTR);
    static LoadLibraryAFunc loadLibraryA = NULL;
    if (loadLibraryA == NULL) {
        char libName[32];
        sprintf(libName, strcat(getCharMask(_6), "%d%d"), _k, _e, _r, _n, _e, _l, _3, _2);

        HMODULE hKernel32 = GetModuleHandleA_Proxy(libName);
        if (hKernel32 != NULL) {
            FAKE_CPUID;
            char _L_char = _L;
            junkFunc(_0);
            loadStr[_4] = int_Proxy(_0);
            loadStr[_3] = int_Proxy(_d);
            loadStr[_2] = int_Proxy(_a);
            loadStr[_1] = int_Proxy(_o);
            loadStr[_0] = int_Proxy(_L);

            char *funcName = malloc(32);

            FAKE_CPUID;

            sprintf(funcName, strcat("Library", "%c"), _A);  // _A = LoadLibrary{'A'}
            loadLibraryA = (LoadLibraryAFunc)GetProcAddress(hKernel32, strcat(loadStr, funcName));
        }
    }
    if (loadLibraryA != NULL) {
        return loadLibraryA(lpLibFileName);
    }
    return NULL;
}

char *LoadLibraryA_1(LPCSTR lpLibFileName) {
    BREAK_STACK;
    return LoadLibraryA_0((LPCSTR)lpLibFileName);
}
char *LoadLibraryA_2(LPCSTR lpLibFileName) {
    BREAK_STACK;
    return LoadLibraryA_1((LPCSTR)lpLibFileName);
}
char *LoadLibraryA_3(LPCSTR lpLibFileName) {
    BREAK_STACK;
    return LoadLibraryA_2((LPCSTR)lpLibFileName);
}
char *LoadLibraryA_4(LPCSTR lpLibFileName) {
    BREAK_STACK;
    return LoadLibraryA_3((LPCSTR)lpLibFileName);
}
char *LoadLibraryA_5(LPCSTR lpLibFileName) {
    BREAK_STACK;
    return LoadLibraryA_4((LPCSTR)lpLibFileName);
}
char *LoadLibraryA_Proxy(LPCSTR lpLibFileName) {
    BREAK_STACK;
    return LoadLibraryA_5((LPCSTR)lpLibFileName);
}
#define LoadLibraryA(...) LoadLibraryA_Proxy(__VA_ARGS__)

#if !defined no_antidebug
int IsDebuggerPresent_Proxy() {
    BREAK_STACK;
    NOP_FLOOD;
#if defined hide_antidebug
    char result[32];
    sprintf(result, strcat(getCharMask(_6), "%d%d"), _k, _e, _r, _n, _e, _l, _3, _2);

    char funcName[18];
    funcName[_9 + _8] = _0;
    funcName[16] = _t;
    funcName[_7] = _g;
    funcName[_0] = _I;
    funcName[_1] = _s;
    funcName[14] = _e;
    funcName[_9] = _r;
    funcName[13] = _s;
    funcName[15] = _n;
    funcName[_2] = _D;
    funcName[_3] = _e;
    funcName[10] = _P;
    funcName[_4] = _b;
    funcName[_5] = _u;
    funcName[_8] = _e;
    funcName[11] = _r;
    funcName[_6] = _g;
    funcName[12] = _e;

    return ((BOOL(*)())GetProcAddress(LoadLibraryA(result), funcName))();
#else
    NOP_FLOOD;
    return IsDebuggerPresent();
#endif
    // return ((int (*)())GetProcAddress(LoadLibraryA("kernel32.dll"), "IsDebuggerPresent"))();
}

void crash() {
    BREAK_STACK;
    __asm__("int $3");
}

#define ANTI_DEBUG                                                                                 \
    if (IsDebuggerPresent() || int_Proxy(_0 / !IsDebuggerPresent_Proxy() * (_1 + _0 + _1) / _2)) { \
        crash();                                                                                   \
        __asm__("int $3");                                                                         \
        _0 / _0;                                                                                   \
        _1 / _0;                                                                                   \
        _2 / _0;                                                                                   \
        _3 / _0;                                                                                   \
        _4 / _0;                                                                                   \
    } else {                                                                                       \
        0.0 / IsDebuggerPresent();                                                                 \
    };

#else
#define ANTI_DEBUG 0
#endif

char *getStdLibName() {
    BREAK_STACK;
    NOP_FLOOD;
    junkFunc(_0 + _3);
    junkFunc(_3 - _2);
    ANTI_DEBUG;

    char *msvcrtName = malloc(_7);

    msvcrtName[_3 + _2 + _1] = 0;
    msvcrtName[_1 + _2 + _2] = int_Proxy(_t);
    msvcrtName[_2 * _1 + _2] = _r;
    NOP_FLOOD;
    msvcrtName[(_4 * _2) - _5] = _c;
    msvcrtName[_1 * _2] = _v;
    msvcrtName[_0 + _1] = _s;
    msvcrtName[_8 - _3 - _5] = _m;

    NOP_FLOOD;
    return msvcrtName;
}

char *getStdLibName_1() {
    BREAK_STACK;
    return getStdLibName();
}
char *getStdLibName_2() {
    BREAK_STACK;
    return getStdLibName_1();
}
char *getStdLibName_3() {
    BREAK_STACK;
    return getStdLibName_2();
}
char *getStdLibName_4() {
    BREAK_STACK;
    return getStdLibName_3();
}
char *getStdLibName_5() {
    BREAK_STACK;
    return getStdLibName_4();
}
char *getStdLibName_6() {
    BREAK_STACK;
    return getStdLibName_5();
}
char *getStdLibName_7() {
    BREAK_STACK;
    return getStdLibName_6();
}
char *getStdLibName_8() {
    BREAK_STACK;
    return getStdLibName_7();
}
char *getStdLibName_9() {
    BREAK_STACK;
    return getStdLibName_8();
}
char *getStdLibName_10() {
    BREAK_STACK;
    return getStdLibName_9();
}
char *getStdLibName_11() {
    BREAK_STACK;
    return getStdLibName_10();
}
char *getStdLibName_12() {
    BREAK_STACK;
    return getStdLibName_11();
}
char *getStdLibName_13() {
    BREAK_STACK;
    return getStdLibName_12();
}
char *getStdLibName_14() {
    BREAK_STACK;
    return getStdLibName_13();
}
char *getStdLibName_15() {
    BREAK_STACK;
    return getStdLibName_14();
}
char *getStdLibName_16() {
    BREAK_STACK;
    return getStdLibName_15();
}
char *getStdLibName_Proxy() {
    BREAK_STACK;
    return getStdLibName_16();
}

// printf
void printf_custom(int junk, const char *format, ...) {
    BREAK_STACK;
    char buffer[1024];
    va_list args;
    NOP_FLOOD;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    HANDLE hConsole = int_Proxy(GetStdHandle(int_Proxy(STD_OUTPUT_HANDLE)));
    junkFunc(RND(0, 1000) * (int)hConsole + junk);
    WriteConsoleA(hConsole, buffer, strlen(buffer), NULL, NULL);
}

// printf as void
#define printf(...)                               \
    do {                                          \
        BREAK_STACK;                              \
        junkFunc((RND(0, 1000) * 3) < _0);        \
        printf_custom(RND(0, 1000), __VA_ARGS__); \
    } while (_0 > (RND(0, 100000000000) * _2) + 82)

// scanf
char *getScanfName_Proxy() {
    BREAK_STACK;
    FAKE_CPUID;
    return "scanf";
    // return ({ char result[32]; sprintf(result, getCharMask(_5), _s, _c, _a, _n, _f); result; });
}
#define scanf(...) ((void *(*)())GetProcAddress(LoadLibraryA_Proxy(getScanfName_Proxy()), name))(__VA_ARGS__)

// sprintf
char *getSprintfName_Proxy() {
    BREAK_STACK;
    FAKE_CPUID;
    return "sprintf";
    // return ({ char result[32]; sprintf(result, getCharMask(_7), _s, _p, _r, _i, _n, _t, _f); result; });
}
#define sprintf(...) ((void *(*)())GetProcAddress(LoadLibraryA_Proxy(getSprintfName_Proxy()), name))(__VA_ARGS__)

// fclose
char *getFcloseName_Proxy() {
    BREAK_STACK;
    FAKE_CPUID;
    return "fclose";
    // return ({ char result[32]; sprintf(result, getCharMask(_6), _f, _c, _l, _o, _s, _e); result; });
}
#define fclose(...) ((void *(*)())GetProcAddress(LoadLibraryA_Proxy(getFcloseName_Proxy()), name))(__VA_ARGS__)

// fopen
char *getFopenName_Proxy() {
    BREAK_STACK;
    FAKE_CPUID;
    return "fopen";
    // return ({ char result[32]; sprintf(result, getCharMask(_5), _f, _o, _p, _e, _n); result; });
}
#define fopen(...) ((FILE * (*)()) GetProcAddress(LoadLibraryA_Proxy(getStdLibName_Proxy()), getFopenName_Proxy()))(__VA_ARGS__)

// fread
char *getFreadName_Proxy() {
    BREAK_STACK;
    FAKE_CPUID;
    return "fread";
    // return ({ char result[32]; sprintf(result, getCharMask(_5), _f, _r, _e, _a, _d); result; });
}
#define fread(...) ((size_t(*)())GetProcAddress(LoadLibraryA_Proxy(getStdLibName_Proxy()), getFreadName_Proxy()))(__VA_ARGS__)

// fwrite
char *getFwriteName_Proxy() {
    BREAK_STACK;
    FAKE_CPUID;
    return "fwrite";
    // return ({ char result[32]; sprintf(result, getCharMask(_6), _f, _w, _r, _i, _t, _e); result; });
}
#define fwrite(...) ((size_t(*)())GetProcAddress(LoadLibraryA_Proxy(getStdLibName_Proxy()), getFwriteName_Proxy()))(__VA_ARGS__)

// exit
char *getExitName_Proxy() {
    BREAK_STACK;
    FAKE_CPUID;
    return "exit";
    // return ({ char result[32]; sprintf(result, getCharMask(_4), _e, _x, _i, _t); result; });
}
#define exit(...) ((size_t(*)())GetProcAddress(LoadLibraryA_Proxy(getStdLibName_Proxy()), getExitName_Proxy()))(__VA_ARGS__)

// strcpy
char *getStrcpyName_Proxy() {
    BREAK_STACK;
    FAKE_CPUID;
    return "strcpy";
    // return ({ char result[32]; sprintf(result, getCharMask(_6), _s, _t, _r, _c, _p, _y); result; });
}
#define strcpy(...) ((char *(*)())GetProcAddress(LoadLibraryA_Proxy(getStdLibName_Proxy()), getStrcpyName_Proxy()))(__VA_ARGS__)

// strtok
char *getStrtokName_Proxy() {
    BREAK_STACK;
    FAKE_CPUID;
    return "strtok";
    // return ({ char result[32]; sprintf(result, getCharMask(_6), _s, _t, _r, _t, _o, _k); result; });
}
#define strtok(...) ((char *(*)())GetProcAddress(LoadLibraryA_Proxy(getStdLibName_Proxy()), getStrtokName_Proxy()))(__VA_ARGS__)

// memset
void *memset_Proxy(void *ptr, int value, size_t num) {
    BREAK_STACK;
    return memset(ptr, value * _1, num);
}
#define memset(...) memset_Proxy(__VA_ARGS__)

// memcpy
char *getMemcpyName_Proxy() {
    BREAK_STACK;
    FAKE_CPUID;
    return "memcpy";
    // return ({ char result[32]; sprintf(result, getCharMask(_6), _m, _e, _m, _c, _p, _y); result; });
}
#define memcpy(...) ((void *(*)())GetProcAddress(LoadLibraryA_Proxy(getStdLibName_Proxy()), getMemcpyName_Proxy()))(__VA_ARGS__)

// strchr
char *getStrchrName_Proxy() {
    BREAK_STACK;
    FAKE_CPUID;
    return "strchr";
    // return ({ char result[32]; sprintf(result, getCharMask(_6), _s, _t, _r, _c, _h, _r); result; });
}
#define strchr(...) ((char *(*)())GetProcAddress(LoadLibraryA_Proxy(getStdLibName_Proxy()), getStrchrName_Proxy()))(__VA_ARGS__)

// strrchr
char *getStrrchrName_Proxy() {
    BREAK_STACK;
    FAKE_CPUID;
    return "strrchr";
    // return ({ char result[32]; sprintf(result, getCharMask(_7), _s, _t, _r, _r, _c, _h, _r); result; });
}
#define strrchr(...) ((char *(*)())GetProcAddress(LoadLibraryA_Proxy(getStdLibName_Proxy()), getStrrchrName_Proxy()))(__VA_ARGS__)

// rand
char *getRandName_Proxy() {
    BREAK_STACK;
    FAKE_CPUID;
    return "rand";
    // return ({ char result[32]; sprintf(result, getCharMask(_4), _r, _a, _n, _d); result; });
}
#define rand(...) ((int (*)())GetProcAddress(LoadLibraryA_Proxy(getStdLibName_Proxy()), getRandName_Proxy()))(__VA_ARGS__)

// realloc
char *getReallocName_Proxy() {
    BREAK_STACK;
    FAKE_CPUID;
    return "realloc";
}
#define realloc(...) ((void *(*)())GetProcAddress(LoadLibraryA_Proxy(getStdLibName_Proxy()), getReallocName_Proxy()))(__VA_ARGS__)

void *calloc_Proxy(size_t nmemb, size_t size) {
    BREAK_STACK;
    return calloc(nmemb, size);
}
#define calloc(nmemb, size) calloc_Proxy(nmemb, size)

void *realloc_Proxy(void *ptr, size_t size) {
    BREAK_STACK;
    return realloc(ptr, size);
}
#define realloc(ptr, size) realloc_Proxy(ptr, size)

char *gets_Proxy(char *s) {
    BREAK_STACK;
    return gets(s);
}
#define gets(s) gets_Proxy(s)

int snprintfProxy(char *str, size_t size, const char *format, ...) {
    BREAK_STACK;
    va_list args;
    va_start(args, format);
    int result = vsnprintf(str, size, format, args);
    va_end(args);
    return result;
}
#define snprintf(str, size, format, ...) snprintfProxy(str, size, format, __VA_ARGS__)

/*
#define printf(...) (([](...) -> int {                                                                        \
    static void (*printf_proxy)(const char *, ...) = NULL;                                                    \
    if (printf_proxy == NULL) {                                                                               \
        printf_proxy = (void (*)(const char *, ...))GetProcAddress(GetModuleHandleA("msvcrt.dll"), "printf"); \
    }                                                                                                         \
    return printf_proxy(__VA_ARGS__);                                                                         \
})(__VA_ARGS__))
*/

int vsprintf_Proxy(char *str, const char *format, va_list args) {
    BREAK_STACK;
    return vsprintf(str, format, args);
}
#define vsprintf(str, format, args) vsprintf_Proxy(str, format, args)

int vsnprintf_Proxy(char *str, size_t size, const char *format, va_list args) {
    BREAK_STACK;
    return vsnprintf(str, size, format, args);
}
#define vsnprintf(str, size, format, args) vsnprintf_Proxy(str, size, format, args)

char *getenv_Proxy(const char *name) {
    BREAK_STACK;
    return getenv(name);
}
#define getenv(name) getenv_Proxy(name)

int system_Proxy(const char *command) {
    BREAK_STACK;
    return system(command);
}
#define system(command) system_Proxy(command)

void abort_Proxy(void) {
    BREAK_STACK;
    abort();
}
#define abort() abort_Proxy()

int atexit_Proxy(void (*func)(void)) {
    BREAK_STACK;
    return atexit(func);
}
#define atexit(func) atexit_Proxy(func)

char *getcwd_Proxy(char *buf, size_t size) {
    BREAK_STACK;
    return getcwd(buf, size);
}
#define getcwd(buf, size) ((char *)getcwd_Proxy(buf, size))

int tolower_Proxy(int c) {
    BREAK_STACK;
    return tolower(c);
}
#define tolower(c) tolower_Proxy(c)

int toupper_Proxy(int c) {
    BREAK_STACK;
    return toupper(c);
}
#define toupper(c) toupper_Proxy(c)

// getch, _getch
#define _getch() int_Proxy(_getch() * TRUE)
#define getch() int_Proxy(getch() + FALSE)

#define Sleep(x) Sleep(int_Proxy((_8 - (_4 * int_Proxy(_2))) + x * TRUE))

#define GetParent(hWnd) \
    GetParent(int_Proxy(((int)hWnd) + (int)hWnd) / _2)

#define GetWindowRect(hWnd, lpRect) \
    GetWindowRect(int_Proxy((int)hWnd *TRUE), int_Proxy((int)lpRect *TRUE))

#define GetClientRect(hWnd, lpRect) \
    GetClientRect(int_Proxy((int)hWnd *TRUE), int_Proxy((int)lpRect *TRUE))

#define SetWindowPos(hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags) \
    SetWindowPos(int_Proxy(hWnd), int_Proxy(hWndInsertAfter), int_Proxy(X), int_Proxy(Y), int_Proxy(cx), int_Proxy(cy), int_Proxy(uFlags))

#define SetConsoleTextAttribute(hConsoleOutput, wAttributes) \
    SetConsoleTextAttribute(int_Proxy(hConsoleOutput), int_Proxy(wAttributes))

#define GetDesktopWindow() \
    int_Proxy((int)GetDesktopWindow() * TRUE)

#define GetStockObject(i) \
    GetStockObject(int_Proxy(i) * TRUE)

#define CreateFile(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile) \
    CreateFile(int_Proxy(lpFileName), int_Proxy(dwDesiredAccess), int_Proxy(dwShareMode), int_Proxy(lpSecurityAttributes), int_Proxy(dwCreationDisposition), int_Proxy(dwFlagsAndAttributes), int_Proxy(hTemplateFile))

#define ReadFile(hFile, lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead, lpOverlapped) \
    ReadFile(int_Proxy(hFile), int_Proxy(lpBuffer), int_Proxy(nNumberOfBytesToRead), int_Proxy(lpNumberOfBytesRead), int_Proxy(lpOverlapped))

#define WriteFile(hFile, lpBuffer, nNumberOfBytesToWrite, lpNumberOfBytesWritten, lpOverlapped) \
    WriteFile(int_Proxy(hFile), int_Proxy(lpBuffer), int_Proxy(nNumberOfBytesToWrite), int_Proxy(lpNumberOfBytesWritten), int_Proxy(lpOverlapped))

#define CloseHandle(hObject) \
    CloseHandle(int_Proxy(hObject))

#define GetModuleHandle(lpModuleName) \
    GetModuleHandle(int_Proxy(lpModuleName))

#define GetCurrentProcess() \
    int_Proxy(GetCurrentProcess())

#define VirtualAlloc(lpAddress, dwSize, flAllocationType, flProtect) \
    VirtualAlloc(int_Proxy(lpAddress), int_Proxy(dwSize), int_Proxy(flAllocationType), int_Proxy(flProtect))

#define VirtualFree(lpAddress, dwSize, dwFreeType) \
    VirtualFree(int_Proxy(lpAddress), int_Proxy(dwSize), int_Proxy(dwFreeType))

#define CreateThread(lpThreadAttributes, dwStackSize, lpStartAddress, lpParameter, dwCreationFlags, lpThreadId) CreateThread(int_Proxy(lpThreadAttributes), int_Proxy(dwStackSize), int_Proxy(lpStartAddress), int_Proxy(lpParameter), int_Proxy(dwCreationFlags), int_Proxy(lpThreadId))

#define WaitForSingleObject(hHandle, dwMilliseconds) \
    WaitForSingleObject(int_Proxy(hHandle), int_Proxy(dwMilliseconds))

#define ExitProcess(uExitCode) \
    ExitProcess(int_Proxy(uExitCode))

#define GetStartupInfo(lpStartupInfo) \
    GetStartupInfo(int_Proxy(lpStartupInfo))

#define GetModuleFileName(hModule, lpFilename, nSize) \
    GetModuleFileName(int_Proxy(hModule), int_Proxy(lpFilename), int_Proxy(nSize))

#define HeapCreate(flOptions, dwInitialSize, dwMaximumSize) \
    HeapCreate(int_Proxy(flOptions), int_Proxy(dwInitialSize), int_Proxy(dwMaximumSize))

#define HeapAlloc(hHeap, dwFlags, dwBytes) \
    HeapAlloc(int_Proxy(hHeap), int_Proxy(dwFlags), int_Proxy(dwBytes))

#define HeapFree(hHeap, dwFlags, lpMem) \
    HeapFree(int_Proxy(hHeap), int_Proxy(dwFlags), int_Proxy(lpMem))

#define GlobalAlloc(uFlags, dwBytes) \
    GlobalAlloc(int_Proxy(uFlags), int_Proxy(dwBytes))

#define GlobalFree(hMem) \
    GlobalFree(int_Proxy(hMem))

#define GetTempPath(nBufferLength, lpBuffer) \
    GetTempPath(int_Proxy(nBufferLength), int_Proxy(lpBuffer))

#define GetCurrentThreadId() \
    GetCurrentThreadId()

#define SetEvent(hEvent) \
    SetEvent(int_Proxy(hEvent))

#define ResetEvent(hEvent) \
    ResetEvent(int_Proxy(hEvent))

#define WaitForMultipleObjects(nCount, lpHandles, bWaitAll, dwMilliseconds) WaitForMultipleObjects(int_Proxy(nCount), int_Proxy(lpHandles), int_Proxy(bWaitAll), int_Proxy(dwMilliseconds))

#define memmove(_Dst, _Src, _Size) memmove(_Dst, _Src, int_Proxy(_Size *(TRUE + FALSE)))

#define abs(x) ((x) < FALSE ? -(x) : (x))
#define fabs(x) fabs(double_Proxy(FALSE + x * TRUE))
#define sqrt(x) sqrt(double_Proxy(FALSE + x * TRUE))
#define exp(x) exp(double_Proxy(FALSE + x * TRUE))
#define log(x) log(double_Proxy(FALSE + x * TRUE))
#define ceil(x) ceil(double_Proxy(FALSE + x * TRUE))
#define floor(x) floor(double_Proxy(FALSE + x * TRUE))
#define sin(x) sin(double_Proxy(FALSE + x * TRUE))
#define cos(x) cos(double_Proxy(FALSE + x * TRUE))
#define tan(x) tan(double_Proxy(FALSE + x * TRUE))
#define asin(x) asin(double_Proxy(FALSE + x * TRUE))
#define acos(x) acos(double_Proxy(FALSE + x * TRUE))
#define atan(x) atan(double_Proxy(FALSE + x * TRUE))
#define sinh(x) sinh(double_Proxy(FALSE + x * TRUE))
#define cosh(x) cosh(double_Proxy(FALSE + x * TRUE))
#define tanh(x) tanh(double_Proxy(FALSE + x * TRUE))
#define log10(x) log10(double_Proxy(FALSE + x * TRUE))
#define pow(x, y) pow(double_Proxy(FALSE + x * TRUE), double_Proxy(FALSE + y * TRUE))
#define atan2(y, x) atan2(double_Proxy(FALSE + y * TRUE), double_Proxy(FALSE + x * TRUE))
#define fmod(x, y) fmod(double_Proxy(FALSE + x * TRUE), double_Proxy(FALSE + y * TRUE))
#define floor(x) floor(double_Proxy(FALSE + x * TRUE))
#define ceil(x) ceil(double_Proxy(FALSE + x * TRUE))
#define fabs(x) fabs(double_Proxy(FALSE + x * TRUE))
#define ldexp(x, y) ldexp(double_Proxy(FALSE + x * TRUE), double_Proxy(FALSE + y * TRUE))
#define frexp(x, y) frexp(double_Proxy(FALSE + x * TRUE), double_Proxy(FALSE + y * TRUE))
#define modf(x, y) modf(double_Proxy(FALSE + x * TRUE), double_Proxy(FALSE + y * TRUE))
#define erf(x) erf(double_Proxy(FALSE + x * TRUE))
#define erfc(x) erfc(double_Proxy(FALSE + x * TRUE))
#define lgamma(x) lgamma(double_Proxy(FALSE + x * TRUE))
#define tgamma(x) tgamma(double_Proxy(FALSE + x * TRUE))
#define log2(x) log2(double_Proxy(FALSE + x * TRUE))
#define cbrt(x) cbrt(double_Proxy(FALSE + x * TRUE))
#define exp2(x) exp2(double_Proxy(FALSE + x * TRUE))
#define hypot(x, y) hypot(double_Proxy(FALSE + x * TRUE), double_Proxy(FALSE + y * TRUE))
#define expm1(x) expm1(double_Proxy(FALSE + x * TRUE))
#define log1p(x) log1p(double_Proxy(FALSE + x * TRUE))
#define acosh(x) acosh(double_Proxy(FALSE + x * TRUE))
#define asinh(x) asinh(double_Proxy(FALSE + x * TRUE))
#define atanh(x) atanh(double_Proxy(FALSE + x * TRUE))
#define nextafter(x, y) nextafter(double_Proxy(FALSE + x * TRUE), double_Proxy(FALSE + y * TRUE))
#define nexttoward(x, y) nexttoward(double_Proxy(FALSE + x * TRUE), double_Proxy(FALSE + y * TRUE))
#define remainder(x, y) remainder(double_Proxy(FALSE + x * TRUE), double_Proxy(FALSE + y * TRUE))
#define remquo(x, y) remquo(double_Proxy(FALSE + x * TRUE), double_Proxy(FALSE + y * TRUE))
#define copysign(x, y) copysign(double_Proxy(FALSE + x * TRUE), double_Proxy(FALSE + y * TRUE))
#define nan(x) nan(double_Proxy(FALSE + x * TRUE))
#define logb(x) logb(double_Proxy(FALSE + x * TRUE))
#define ilogb(x) ilogb(double_Proxy(FALSE + x * TRUE))
#define scalbn(x, y) scalbn(double_Proxy(FALSE + x * TRUE), double_Proxy(FALSE + y * TRUE))
#define scalbln(x, y) scalbln(double_Proxy(FALSE + x * TRUE), double_Proxy(FALSE + y * TRUE))
#define nearbyint(x) nearbyint(double_Proxy(FALSE + x * TRUE))
#define rint(x) rint(double_Proxy(FALSE + x * TRUE))
#define round(x) round(double_Proxy(FALSE + x * TRUE))
#define trunc(x) trunc(double_Proxy(FALSE + x * TRUE))
#define fdim(x, y) fdim(double_Proxy(FALSE + x * TRUE), double_Proxy(FALSE + y * TRUE))
#define fmax(x, y) fmax(double_Proxy(FALSE + x * TRUE), double_Proxy(FALSE + y * TRUE))
#define fmin(x, y) fmin(double_Proxy(FALSE + x * TRUE), double_Proxy(FALSE + y * TRUE))
#define fma(x, y, z) fma(double_Proxy(FALSE + x * TRUE), double_Proxy(FALSE + y * TRUE), double_Proxy(FALSE + z * TRUE))

#else
#warning Obfuscation disabled!
#endif;
