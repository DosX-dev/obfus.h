/*
     ██████╗ ██████╗ ███████╗██╗   ██╗███████╗     ██╗
    ██╔═══██╗██╔══██╗██╔════╝██║   ██║██╔════╝     ██║
    ██║   ██║██████╔╝█████╗  ██║   ██║███████╗     ███████║
    ██║   ██║██╔══██╗██╔══╝  ██║   ██║╚════██║     ██╔══██║
    ╚██████╔╝██████╔╝██║     ╚██████╔╝███████║ ██╗ ██║  ██║
     ╚═════╝ ╚═════╝ ╚═╝      ╚═════╝ ╚══════╝ ╚═╝ ╚═╝  ╚═╝

          Very reliable armor for your C programs!
                  Coded by (C) DosX, 2024

 [Additional options]
 ~ cflow_v2       = more powerful Control Flow obfuscation (slowly!)
 ~ antidebug_v2   = use better dynamic anti-debugging protection
 ~ fake_signs     = adds fake signatures of various protectors or packers

 [Disabling default features]
 ~ no_cflow       = disable control flow obfuscation
 ~ no_antidebug   = disable antidebug protection

 ~ no_obf         = disable obfuscation

 GitHub:
 -> https://github.com/DosX-dev/obfus.h

*/

#if no_obf == 0

#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#if !defined __COUNTER__
#error You are using too old a compiler version!
#endif

#define SUPPORTED (__TINYC__ || __GNUC__)
#define SECTION_ATTRIBUTE(NAME) __attribute__((section(NAME)))

// Fake signatures ;)
#if defined(fake_signs) && (fake_signs != 0) && SUPPORTED

static const char FAKE_ENIGMAVM_1[] SECTION_ATTRIBUTE(".enigma1") = {0};
static const char FAKE_ENIGMAVM_2[] SECTION_ATTRIBUTE(".enigma2") = {0};
static const char FAKE_VMPROTECT_1[] SECTION_ATTRIBUTE(".vmp0") = {0};  // (now is open-source)
static const char FAKE_VMPROTECT_2[] SECTION_ATTRIBUTE(".vmp1") = {0};
static const char FAKE_VMPROTECT_3[] SECTION_ATTRIBUTE(".vmp2") = {0};

#define OBFH_SECTION_ATTRIBUTE SECTION_ATTRIBUTE("UPX0")  // OBFH section
static const char FAKE_UPX[] OBFH_SECTION_ATTRIBUTE = {0};

static const char FAKE_THEMIDA[] SECTION_ATTRIBUTE(".winlice") = {0};
static const char FAKE_PETITE[] SECTION_ATTRIBUTE(".petite") = {0};
static const char FAKE_RLP[] SECTION_ATTRIBUTE(".rlp") = {0};
static const char FAKE_SECUROM[] SECTION_ATTRIBUTE(".dsstext") = {0};
static const char FAKE_SQUISHY[] SECTION_ATTRIBUTE("logicoma") = {0};
static const char FAKE_THEARK_1[] SECTION_ATTRIBUTE("adr") = {0};
static const char FAKE_THEARK_2[] SECTION_ATTRIBUTE("have") = {0};
static const char FAKE_THEARK_3[] SECTION_ATTRIBUTE("30cm") = {0};
static const char FAKE_PETETRIS[] SECTION_ATTRIBUTE("PETETRIS") = {0};

static const char FAKE_ENIGMA[] SECTION_ATTRIBUTE(".data") = {0x45, 0x6e, 0x69, 0x67,
                                                              0x6d, 0x61, 0x20, 0x70,
                                                              0x72, 0x6f, 0x74, 0x65,
                                                              0x63, 0x74, 0x6f, 0x72,
                                                              0x20, 0x76, 0x01};

static const char FAKE_ALINYZE[] SECTION_ATTRIBUTE(".alien") = {0};
static const char FAKE_PWDPROTECT[] SECTION_ATTRIBUTE(".pwdprot") = {0};

static const char FAKE_DENUVO[] SECTION_ATTRIBUTE(".arch") = {0x64, 0x65, 0x6E, 0x75,
                                                              0x76, 0x6F, 0x5F, 0x61,
                                                              0x74, 0x64, 0x00, 0x00,
                                                              0x00, 0x00, 0x00, 0x00};

static const char FAKE_NUITKA[] SECTION_ATTRIBUTE(".rdata") = {0x4e, 0x55, 0x49, 0x54,
                                                               0x4b, 0x41, 0x5f, 0x4f,
                                                               0x4e, 0x45, 0x46, 0x49,
                                                               0x4c, 0x45, 0x5f, 0x50,
                                                               0x41, 0x52, 0x45, 0x4e,
                                                               0x54};

static const char FAKE_THEARK_4[] SECTION_ATTRIBUTE(".tw") = {0};
static const char FAKE_THEARK_5[] SECTION_ATTRIBUTE("logicoma") = {0};
static const char FAKE_OREANSVM[] SECTION_ATTRIBUTE(".vlizer") = {0};
static const char FAKE_SCREEN2EXE[] SECTION_ATTRIBUTE(".text") = {0x56, 0x69, 0x64, 0x65, 0x6f, 0x20, 0x63, 0x72, 0x65, 0x61, 0x74, 0x65, 0x64, 0x20, 0x62, 0x79, 0x20, 0x53, 0x43, 0x52, 0x45, 0x45, 0x4e, 0x32, 0x45, 0x58, 0x45, 0x2f, 0x53, 0x43, 0x52, 0x45, 0x45, 0x4e, 0x32, 0x53, 0x57, 0x46};
static const char FAKE_ASPACK_1[] SECTION_ATTRIBUTE(".aspack") = {0};
static const char FAKE_ASPACK_2[] SECTION_ATTRIBUTE(".adata") = {0};
static const char FAKE_WIBUCODEMETER_1[] SECTION_ATTRIBUTE("__wibu00") = {0};
static const char FAKE_WIBUCODEMETER_2[] SECTION_ATTRIBUTE("__wibu01") = {0};
static const char *FAKE_DONGLE[] = {"skeydrv.dll", "HASPDOSDRV",
                                    "MARXDEV1.SYS", "MxLPT_Sem",
                                    "nethasp.ini", "sense4.dll",
                                    "SNTNLUSB", "RNBOspro",
                                    "SSIVDDP.DLL", "WIBUKEY",
                                    "\\\\.\\WIZZKEYRL",
                                    "\\\\.\\NVKEY"};

#elif SUPPORTED

#define OBFH_SECTION_ATTRIBUTE SECTION_ATTRIBUTE(".obfh")  // OBFH section

#endif

// Thanks to @horsicq && @ac3ss0r
#define RND(min, max) (min + (((__COUNTER__ + (__LINE__ * __LINE__)) * 2654435761u) % (max - min + 1)))

/*
#if SUPPORTED
void breakerFunc() SECTION_ATTRIBUTE(".dosx") {
#else
void breakerFunc() {
#endif
    __asm__ __volatile(".byte 0x00");
}
*/

#if SUPPORTED
volatile static char _s_a[] OBFH_SECTION_ATTRIBUTE = "a", _s_b[] OBFH_SECTION_ATTRIBUTE = "b", _s_c[] OBFH_SECTION_ATTRIBUTE = "c", _s_d[] OBFH_SECTION_ATTRIBUTE = "d",
                            _s_e[] OBFH_SECTION_ATTRIBUTE = "e", _s_f[] OBFH_SECTION_ATTRIBUTE = "f", _s_g[] OBFH_SECTION_ATTRIBUTE = "g", _s_h[] OBFH_SECTION_ATTRIBUTE = "h",
                            _s_i[] OBFH_SECTION_ATTRIBUTE = "i", _s_j[] OBFH_SECTION_ATTRIBUTE = "j", _s_k[] OBFH_SECTION_ATTRIBUTE = "k", _s_l[] OBFH_SECTION_ATTRIBUTE = "l",
                            _s_m[] OBFH_SECTION_ATTRIBUTE = "m", _s_n[] OBFH_SECTION_ATTRIBUTE = "n", _s_o[] OBFH_SECTION_ATTRIBUTE = "o", _s_p[] OBFH_SECTION_ATTRIBUTE = "p",
                            _s_q[] OBFH_SECTION_ATTRIBUTE = "q", _s_r[] OBFH_SECTION_ATTRIBUTE = "r", _s_s[] OBFH_SECTION_ATTRIBUTE = "s", _s_t[] OBFH_SECTION_ATTRIBUTE = "t",
                            _s_u[] OBFH_SECTION_ATTRIBUTE = "u", _s_v[] OBFH_SECTION_ATTRIBUTE = "v", _s_w[] OBFH_SECTION_ATTRIBUTE = "w", _s_x[] OBFH_SECTION_ATTRIBUTE = "x",
                            _s_y[] OBFH_SECTION_ATTRIBUTE = "y", _s_z[] = "z",
                            _a OBFH_SECTION_ATTRIBUTE = 'a', _b OBFH_SECTION_ATTRIBUTE = 'b', _c OBFH_SECTION_ATTRIBUTE = 'c', _d OBFH_SECTION_ATTRIBUTE = 'd',
                            _e OBFH_SECTION_ATTRIBUTE = 'e', _f OBFH_SECTION_ATTRIBUTE = 'f', _g OBFH_SECTION_ATTRIBUTE = 'g', _h OBFH_SECTION_ATTRIBUTE = 'h',
                            _i OBFH_SECTION_ATTRIBUTE = 'i', _j OBFH_SECTION_ATTRIBUTE = 'j', _k OBFH_SECTION_ATTRIBUTE = 'k', _l OBFH_SECTION_ATTRIBUTE = 'l',
                            _m OBFH_SECTION_ATTRIBUTE = 'm', _n OBFH_SECTION_ATTRIBUTE = 'n', _o OBFH_SECTION_ATTRIBUTE = 'o', _p OBFH_SECTION_ATTRIBUTE = 'p',
                            _q OBFH_SECTION_ATTRIBUTE = 'q', _r OBFH_SECTION_ATTRIBUTE = 'r', _s OBFH_SECTION_ATTRIBUTE = 's', _t OBFH_SECTION_ATTRIBUTE = 't',
                            _u OBFH_SECTION_ATTRIBUTE = 'u', _v OBFH_SECTION_ATTRIBUTE = 'v', _w OBFH_SECTION_ATTRIBUTE = 'w', _x OBFH_SECTION_ATTRIBUTE = 'x',
                            _y OBFH_SECTION_ATTRIBUTE = 'y', _z OBFH_SECTION_ATTRIBUTE = 'z',
                            _S OBFH_SECTION_ATTRIBUTE = 'S', _L OBFH_SECTION_ATTRIBUTE = 'L', _A OBFH_SECTION_ATTRIBUTE = 'A', _I OBFH_SECTION_ATTRIBUTE = 'I',
                            _D OBFH_SECTION_ATTRIBUTE = 'D', _P OBFH_SECTION_ATTRIBUTE = 'P',
                            _0 OBFH_SECTION_ATTRIBUTE = 0, _1 OBFH_SECTION_ATTRIBUTE = 1, _2 OBFH_SECTION_ATTRIBUTE = 2, _3 OBFH_SECTION_ATTRIBUTE = 3, _4 OBFH_SECTION_ATTRIBUTE = 4,
                            _5 OBFH_SECTION_ATTRIBUTE = 5, _6 OBFH_SECTION_ATTRIBUTE = 6, _7 OBFH_SECTION_ATTRIBUTE = 7, _8 OBFH_SECTION_ATTRIBUTE = 8, _9 OBFH_SECTION_ATTRIBUTE = 9;
#else
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
#endif

#define BREAK_STACK_1                      \
    __asm__ __volatile("xorl %eax, %eax"); \
    __asm__ __volatile("jz 1f");           \
    __asm__ __volatile(".byte 0xE8");      \
    __asm__ __volatile("1:");

#define BREAK_STACK_2 \
    if (_0) __asm__ __volatile(".byte 0x00");

// Junk JMPOUT condition
#define BREAK_STACK_3                         \
    switch (_0) {                             \
        case RND(1, 1000):                    \
            __asm__ __volatile(".byte 0x00"); \
    }

#define BREAK_STACK_4                      \
    __asm__ __volatile("xorl %ebx, %ebx"); \
    __asm__ __volatile("xorl %edx, %edx"); \
    __asm__ __volatile("xorl %ebx, %edx"); \
    __asm__ __volatile("jz 1f");           \
    __asm__ __volatile("mov %eax, 4");     \
    __asm__ __volatile(".byte 0x00");      \
    __asm__ __volatile("1:");

// Junk 'MEMORY[*]'
#define BREAK_STACK_5                      \
    __asm__ __volatile("xorl %ebx, %ebx"); \
    __asm__ __volatile("xorl %eax, %eax"); \
    __asm__ __volatile("mov %eax, %ebx");  \
    __asm__ __volatile("mov %edx, %ebx");  \
    __asm__ __volatile("xorl %eax, %edx"); \
    __asm__ __volatile("jz 1f");           \
    __asm__ __volatile(".byte 0x20");      \
    __asm__ __volatile("1:");

#define BREAK_STACK_6                      \
    __asm__ __volatile("xorl %edx, %edx"); \
    __asm__ __volatile("xorl %eax, %eax"); \
    __asm__ __volatile("mov %eax, %edx");  \
    __asm__ __volatile("jz 1f");           \
    __asm__ __volatile(".byte 0xE8");      \
    __asm__ __volatile("1:");

#define BREAK_STACK_7                      \
    __asm__ __volatile("xorl %edx, %edx"); \
    __asm__ __volatile("jz 1f");           \
    __asm__ __volatile(".byte 0xE8");      \
    __asm__ __volatile("1:");

#if SUPPORTED
void junkFunc(int z, ...) OBFH_SECTION_ATTRIBUTE {
#else
void junkFunc(int z, ...) {
#endif
    __asm__ __volatile("nop");
    return;
}

void junkFuncEmpty() {
    BREAK_STACK_5;
    __asm__ __volatile("nop");
    return;
}

#define __CRASH                       \
    __asm__ __volatile(".byte 0xED"); \
    __asm__ __volatile("int $3");     \
    exit(1);

#define TRUE ((((_9 + _7 + (RND(0, 1000) * _0))) / _8) - _1)
#define FALSE ((_3 + _6 + (RND(0, 1000) * _0)) - _9)

#define FAKE_CPUID __asm__ __volatile("nop\ncpuid\nnop\n")

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
    BREAK_STACK_1;
    return malloc(size);
}
#define malloc(...) malloc_Proxy(__VA_ARGS__)

static char rndValueToProxy = RND(0, 10);

#if SUPPORTED
int int_Proxy(int value) OBFH_SECTION_ATTRIBUTE {
#else
int int_Proxy(int value) {
#endif
    BREAK_STACK_4;
    if (rndValueToProxy == value)
        return rndValueToProxy;

    junkFunc(RND(0, 100000), RND(0, 100000));

    FAKE_CPUID;

    return ((value * _1) + ((_4 * RND(0, 100000)) - _8) * _0);
}

#if SUPPORTED
double double_Proxy(double value) OBFH_SECTION_ATTRIBUTE {
#else
double double_Proxy(double value) {
#endif
    BREAK_STACK_1;
    junkFunc(RND(0, 1000), RND(0, 1000));
    FAKE_CPUID;
    return (value * _1);
}

#if SUPPORTED
int condition_True() OBFH_SECTION_ATTRIBUTE {
#else
int condition_True() {
#endif
    BREAK_STACK_1;
    FAKE_CPUID;
    return _1 && TRUE;
}

#if SUPPORTED
int condition_Proxy(int junk, int condition) OBFH_SECTION_ATTRIBUTE {
#else
int condition_Proxy(int junk, int condition) {
#endif
    BREAK_STACK_4;

    int result = int_Proxy(condition * _1);
    if (result == (FALSE * junk)) {
        return _8 - (_4 * _2) && !condition_True();
    } else if (result == TRUE) {
        return (condition_True() || FALSE || TRUE) && (FALSE + int_Proxy(_1));
    }

    FAKE_CPUID;
    return int_Proxy(condition);
}

// Control Flow (global)
#if no_cflow != 1

#if cflow_v2 == 1

// Control flow obfuscation for 'if' & 'for', V2 (strong!)
#define if(condition)                      \
    if (int_Proxy(RND(1, 1000000)) < _0) { \
        __asm__ __volatile(".byte 0x00");  \
    } else if (int_Proxy((RND(0, 1000)) > _0 && (RND(2, 1000) > condition_True() && condition_Proxy(RND(0, 1000000), condition) && RND(1, 99999999) > _0 && (int_Proxy(RND(0, 1000)) < RND(1001, 100000000)))) * TRUE)
#define for(data) for (data && int_Proxy(TRUE * (RND(0, 1000000))) + FALSE || TRUE)

#else

// Control flow obfuscation for 'if' & 'for', V1
#define if(condition) if ((RND(0, 1000)) > _0 && (RND(2, 1000) > condition_True() && condition_Proxy(RND(0, 1000000), condition) && RND(1, 9999999) > _0 && (int_Proxy(RND(0, 1000)) < RND(1001, 100000000))))
#define for(data) for (data && int_Proxy(TRUE * (RND(0, 10000))) + FALSE || _1)

#endif

#define else                                                                      \
    else if (_0 > RND(1, 1000)) {                                                 \
        junkFunc(RND(0, 1000));                                                   \
        __asm__ __volatile(".byte 0x3C");                                         \
    }                                                                             \
    else if (RND(0, 10) == (RND(11, 100))) {                                      \
        BREAK_STACK_3;                                                            \
        int_Proxy(_3 - RND(0, 10000));                                            \
    }                                                                             \
    else if (FALSE * RND(0, 1000)) {                                              \
        BREAK_STACK_1;                                                            \
    }                                                                             \
    else if (FALSE * (int_Proxy(RND(0, 1000)) ? RND(1, 99999) : RND(1, 99999))) { \
        __asm__ __volatile(".byte 0xEB");                                         \
    }                                                                             \
    else

#define while(condition) while ((RND(0, 1000)) > _0 && _8 > _3 && condition_True() && RND(1, 9999999999) > _0 && condition_Proxy(RND(0, 1000), condition) && _5)

#endif

#if SUPPORTED
char *getCharMask(int count) OBFH_SECTION_ATTRIBUTE {
#else
char *getCharMask(int count) {
#endif
    BREAK_STACK_1;
    static char mask[16];
    if (count <= _0 || count >= sizeof(mask)) {
        return NULL;
    }
    int i = (((_1 * _5) - _4) + _1) - _2;
    BREAK_STACK_1;
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
    BREAK_STACK_1;
    FAKE_CPUID;
    return WriteConsoleA(hConsoleOutput, lpBuffer, nNumberOfCharsToWrite, lpNumberOfCharsWritten, lpReserved);
}
#define WriteConsoleA(...) WriteConsoleA_Proxy(__VA_ARGS__)

// GetStdHandle
HANDLE GetStdHandle_Proxy(DWORD nStdHandle) {
    BREAK_STACK_1;
    FAKE_CPUID;
    return GetStdHandle(int_Proxy(nStdHandle));
}
#define GetStdHandle(...) GetStdHandle_Proxy(__VA_ARGS__)

// GetProcAddress
FARPROC GetProcAddress_Proxy(HMODULE hModule, LPCSTR lpProcName) {
    BREAK_STACK_1;
    FAKE_CPUID;
    return GetProcAddress(hModule, lpProcName);
}
#define GetProcAddress(...) GetProcAddress_Proxy(__VA_ARGS__)

HMODULE GetModuleHandleA_Proxy(LPCSTR lpModuleName) {
    BREAK_STACK_1;
    FAKE_CPUID;
    return GetModuleHandleA(lpModuleName);
}
#define GetModuleHandleA(...) GetModuleHandleA_Proxy(__VA_ARGS__)

// strcmp
int strcmp_custom(const char *str1, const char *str2) {
    BREAK_STACK_1;
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
    BREAK_STACK_1;
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
#if SUPPORTED
HMODULE LoadLibraryA_0(LPCSTR lpLibFileName) OBFH_SECTION_ATTRIBUTE {
#else
HMODULE LoadLibraryA_0(LPCSTR lpLibFileName) {
#endif
    switch (_0) {
        case 1:
            __asm__ __volatile(".byte 0x00");

            break;
        case 0:
            BREAK_STACK_3;
            // return LoadLibraryA(lpLibFileName);

            typedef HMODULE(WINAPI * LoadLibraryAFunc)(LPCSTR);
            static LoadLibraryAFunc loadLibraryA = NULL;
            if (loadLibraryA == NULL) {
                char libName[32];
                sprintf(libName, strcat(getCharMask(_6), "%d"), _k, _e, _r, _n, _e, _l, (_4 * _8));

                HMODULE hKernel32 = GetModuleHandleA(libName);
                if (hKernel32 != NULL) {
                    FAKE_CPUID;
                    char _L_char = _L;
                    junkFunc(_0 + RND(1, 5));
                    loadStr[_4] = int_Proxy(_0);
                    loadStr[_3] = int_Proxy(_d);
                    loadStr[_2] = int_Proxy(_a);
                    BREAK_STACK_2;
                    loadStr[_1] = int_Proxy(_o);
                    loadStr[_0] = int_Proxy(_L);

                    char *funcName = malloc(32);

                    FAKE_CPUID;

                    sprintf(funcName, strcat("Library", "%c"), _A);  // _A = LoadLibrary{'A'}
                    loadLibraryA = (LoadLibraryAFunc)GetProcAddress(hKernel32, strcat(loadStr, funcName));
                    free(funcName);

#if memcleaner
                    int *value = _1 * -1;
                    SetProcessWorkingSetSize(GetCurrentProcess(), value, value);
#endif
                }
            }
            if (loadLibraryA != NULL) {
                BREAK_STACK_1;
                return loadLibraryA(lpLibFileName);
            }
            return NULL;
    }
}

char *LoadLibraryA_1(LPCSTR lpLibFileName) {
    BREAK_STACK_6;
    return LoadLibraryA_0((LPCSTR)lpLibFileName);
}
char *LoadLibraryA_2(LPCSTR lpLibFileName) {
    BREAK_STACK_5;
    return LoadLibraryA_1((LPCSTR)lpLibFileName);
}
char *LoadLibraryA_3(LPCSTR lpLibFileName) {
    BREAK_STACK_4;
    return LoadLibraryA_2((LPCSTR)lpLibFileName);
}
char *LoadLibraryA_4(LPCSTR lpLibFileName) {
    BREAK_STACK_3;
    return LoadLibraryA_3((LPCSTR)lpLibFileName);
}
char *LoadLibraryA_5(LPCSTR lpLibFileName) {
    BREAK_STACK_2;
    return LoadLibraryA_4((LPCSTR)lpLibFileName);
}
char *LoadLibraryA_Proxy(LPCSTR lpLibFileName) {
    BREAK_STACK_1;
    return LoadLibraryA_5((LPCSTR)lpLibFileName);
}
#define LoadLibraryA(...) LoadLibraryA_Proxy(__VA_ARGS__)

// Anti-Debug (global)
#if no_antidebug != 1
#if SUPPORTED
int IsDebuggerPresent_Proxy() OBFH_SECTION_ATTRIBUTE {
#else
int IsDebuggerPresent_Proxy() {
#endif
    BREAK_STACK_1;
    NOP_FLOOD;
    BREAK_STACK_2;
#if antidebug_v2 == 1

    // Dynamic antidebugger
    char result[32];
    sprintf(result, strcat(getCharMask(_6), "%d"), _k, _e, _r, _n, _e, _l, (_6 * _6 - _4));

    char funcName[18];
    funcName[_9 + _8] = _0;

    funcName[_9 + _7 * _1] = _t;
    funcName[_2 + _5 * _1] = _g;
    funcName[_0 * _8 * _1] = _I;
    funcName[_1 + _0 * _1] = _s;
    funcName[_7 * _2 * _1] = _e;
    funcName[_3 * _3 * _1] = _r;
    funcName[_9 + _4 * _1] = _s;
    funcName[_5 * _3 * _1] = _n;
    BREAK_STACK_3;
    funcName[_1 + _1 * _1] = _D;
    funcName[_1 + _2 * _1] = _e;
    funcName[_5 * _2 * _1] = _P;
    funcName[_2 + _2 * _1] = _b;
    funcName[_3 + _2 * _1] = _u;
    funcName[_4 * _2 * _1] = _e;
    funcName[_2 + _9 * _1] = _r;
    funcName[_3 * _2 * _1] = _g;
    funcName[_6 * _2 * _1] = _e;

    return ((BOOL(*)())GetProcAddress(LoadLibraryA(result), funcName))();

#else

    // Standart antidebugger
    NOP_FLOOD;
    return IsDebuggerPresent();

#endif
    // return ((int (*)())GetProcAddress(LoadLibraryA("kernel32.dll"), "IsDebuggerPresent"))();
}

/*
void antiDebugMessage() {
    typedef int(WINAPI * MessageBoxAType)(HWND, LPCSTR, LPCSTR, UINT);
    MessageBoxAType MessageBoxA = (MessageBoxAType)GetProcAddress(LoadLibraryA("user32.dll"), "MessageBoxA");
    if (MessageBoxA != NULL) MessageBoxA(NULL, "Debugging prevented.", "", 0x10);
}
*/

void crash() {
    BREAK_STACK_1;
    __asm__ __volatile("int $3");
    __asm__ __volatile(".byte 0xED, 0x00");
}

void loop() {
    while (1) {
    }
}

#define ANTI_DEBUG                                                                                 \
    if (IsDebuggerPresent() || int_Proxy(_0 / !IsDebuggerPresent_Proxy() * (_1 + _0 + _1) / _2)) { \
        double_Proxy(RND(1, 999));                                                                 \
        /* antiDebugMessage(); */                                                                  \
        loop();                                                                                    \
        while (1) {                                                                                \
        };                                                                                         \
        __asm__ __volatile(".byte 0xED");                                                          \
        BREAK_STACK_1;                                                                             \
        __asm__ __volatile(".byte 0x66, 0xC1, 0xE8, 0x05");                                        \
        __asm__ __volatile(".byte 0x00");                                                          \
        __asm__ __volatile("ret");                                                                 \
        crash();                                                                                   \
    } else {                                                                                       \
        0.0 / IsDebuggerPresent();                                                                 \
    };

#else
#define ANTI_DEBUG 0
#endif

#if SUPPORTED
char *getStdLibName() OBFH_SECTION_ATTRIBUTE {
#else
char *getStdLibName() {
#endif
    BREAK_STACK_1;
    NOP_FLOOD;
    junkFunc(_0 + _3);
    junkFunc(_3 - _2);

    // char *msvcrtName = malloc(_7);
    static char msvcrtName[7] = "\0\0\0\0\0\0\0";

    ANTI_DEBUG;

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
    BREAK_STACK_3;
    return getStdLibName();
}
char *getStdLibName_2() {
    BREAK_STACK_7;
    return getStdLibName_1();
}
char *getStdLibName_3() {
    BREAK_STACK_6;
    return getStdLibName_2();
}
char *getStdLibName_4() {
    BREAK_STACK_5;
    return getStdLibName_3();
}
char *getStdLibName_5() {
    BREAK_STACK_4;
    return getStdLibName_4();
}
char *getStdLibName_6() {
    BREAK_STACK_3;
    return getStdLibName_5();
}
char *getStdLibName_7() {
    BREAK_STACK_2;
    return getStdLibName_6();
}
char *getStdLibName_8() {
    BREAK_STACK_1;
    return getStdLibName_7();
}
char *getStdLibName_9() {
    BREAK_STACK_4;
    return getStdLibName_8();
}
char *getStdLibName_10() {
    BREAK_STACK_7;
    return getStdLibName_9();
}
char *getStdLibName_11() {
    BREAK_STACK_2;
    return getStdLibName_10();
}
char *getStdLibName_12() {
    BREAK_STACK_6;
    return getStdLibName_11();
}
char *getStdLibName_13() {
    BREAK_STACK_1;
    return getStdLibName_12();
}
char *getStdLibName_14() {
    BREAK_STACK_1;
    return getStdLibName_13();
}
char *getStdLibName_15() {
    BREAK_STACK_1;
    return getStdLibName_14();
}
char *getStdLibName_16() {
    BREAK_STACK_7;
    return getStdLibName_15();
}
char *getStdLibName_Proxy() {
    BREAK_STACK_7;
    return getStdLibName_16();
}

// printf
void printf_custom(int junk, const char *format, ...) {
    BREAK_STACK_1;
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
        BREAK_STACK_1;                            \
        getStdLibName();                          \
        junkFunc((RND(0, 1000) * 3) < _0);        \
        printf_custom(RND(0, 1000), __VA_ARGS__); \
    } while (_0 > (RND(0, 100000000000) * _2) + 82)

// scanf
char *getScanfName_Proxy() {
    BREAK_STACK_1;
    FAKE_CPUID;
    return "scanf";
    // return ({ char result[32]; sprintf(result, getCharMask(_5), _s, _c, _a, _n, _f); result; });
}
#define scanf(...) ((void *(*)())GetProcAddress(LoadLibraryA_Proxy(getStdLibName_Proxy()), getScanfName_Proxy()))(__VA_ARGS__)

// sprintf
char *getSprintfName_Proxy() {
    BREAK_STACK_1;
    FAKE_CPUID;
    return "sprintf";
    // return ({ char result[32]; sprintf(result, getCharMask(_7), _s, _p, _r, _i, _n, _t, _f); result; });
}
#define sprintf(...) ((void *(*)())GetProcAddress(LoadLibraryA_Proxy(getStdLibName_Proxy()), getSprintfName_Proxy()))(__VA_ARGS__)

// fclose
char *getFcloseName_Proxy() {
    BREAK_STACK_1;
    FAKE_CPUID;
    return "fclose";
    // return ({ char result[32]; sprintf(result, getCharMask(_6), _f, _c, _l, _o, _s, _e); result; });
}
#define fclose(...) ((void *(*)())GetProcAddress(LoadLibraryA_Proxy(getStdLibName_Proxy()), getFcloseName_Proxy()))(__VA_ARGS__)

// fopen
char *getFopenName_Proxy() {
    BREAK_STACK_1;
    FAKE_CPUID;
    return "fopen";
    // return ({ char result[32]; sprintf(result, getCharMask(_5), _f, _o, _p, _e, _n); result; });
}
#define fopen(...) ((FILE * (*)()) GetProcAddress(LoadLibraryA_Proxy(getStdLibName_Proxy()), getFopenName_Proxy()))(__VA_ARGS__)

// fread
char *getFreadName_Proxy() {
    BREAK_STACK_1;
    FAKE_CPUID;
    return "fread";
    // return ({ char result[32]; sprintf(result, getCharMask(_5), _f, _r, _e, _a, _d); result; });
}
#define fread(...) ((size_t(*)())GetProcAddress(LoadLibraryA_Proxy(getStdLibName_Proxy()), getFreadName_Proxy()))(__VA_ARGS__)

// fwrite
char *getFwriteName_Proxy() {
    BREAK_STACK_1;
    FAKE_CPUID;
    return "fwrite";
    // return ({ char result[32]; sprintf(result, getCharMask(_6), _f, _w, _r, _i, _t, _e); result; });
}
#define fwrite(...) ((size_t(*)())GetProcAddress(LoadLibraryA_Proxy(getStdLibName_Proxy()), getFwriteName_Proxy()))(__VA_ARGS__)

// exit
char *getExitName_Proxy() {
    BREAK_STACK_1;
    FAKE_CPUID;
    return "exit";
    // return ({ char result[32]; sprintf(result, getCharMask(_4), _e, _x, _i, _t); result; });
}
#define exit(...) ((size_t(*)())GetProcAddress(LoadLibraryA_Proxy(getStdLibName_Proxy()), getExitName_Proxy()))(__VA_ARGS__)

// strcpy
char *getStrcpyName_Proxy() {
    BREAK_STACK_1;
    FAKE_CPUID;
    return "strcpy";
    // return ({ char result[32]; sprintf(result, getCharMask(_6), _s, _t, _r, _c, _p, _y); result; });
}
#define strcpy(...) ((char *(*)())GetProcAddress(LoadLibraryA_Proxy(getStdLibName_Proxy()), getStrcpyName_Proxy()))(__VA_ARGS__)

// strtok
char *getStrtokName_Proxy() {
    BREAK_STACK_1;
    FAKE_CPUID;
    return "strtok";
    // return ({ char result[32]; sprintf(result, getCharMask(_6), _s, _t, _r, _t, _o, _k); result; });
}
#define strtok(...) ((char *(*)())GetProcAddress(LoadLibraryA_Proxy(getStdLibName_Proxy()), getStrtokName_Proxy()))(__VA_ARGS__)

// memset
void *memset_Proxy(void *ptr, int value, size_t num) {
    BREAK_STACK_1;
    return memset(ptr, value * _1, num);
}
#define memset(...) memset_Proxy(__VA_ARGS__)

// memcpy
char *getMemcpyName_Proxy() {
    BREAK_STACK_1;
    FAKE_CPUID;
    return "memcpy";
    // return ({ char result[32]; sprintf(result, getCharMask(_6), _m, _e, _m, _c, _p, _y); result; });
}
#define memcpy(...) ((void *(*)())GetProcAddress(LoadLibraryA_Proxy(getStdLibName_Proxy()), getMemcpyName_Proxy()))(__VA_ARGS__)

// strchr
char *getStrchrName_Proxy() {
    BREAK_STACK_1;
    FAKE_CPUID;
    return "strchr";
    // return ({ char result[32]; sprintf(result, getCharMask(_6), _s, _t, _r, _c, _h, _r); result; });
}
#define strchr(...) ((char *(*)())GetProcAddress(LoadLibraryA_Proxy(getStdLibName_Proxy()), getStrchrName_Proxy()))(__VA_ARGS__)

// strrchr
char *getStrrchrName_Proxy() {
    BREAK_STACK_1;
    FAKE_CPUID;
    return "strrchr";
    // return ({ char result[32]; sprintf(result, getCharMask(_7), _s, _t, _r, _r, _c, _h, _r); result; });
}
#define strrchr(...) ((char *(*)())GetProcAddress(LoadLibraryA_Proxy(getStdLibName_Proxy()), getStrrchrName_Proxy()))(__VA_ARGS__)

// rand
char *getRandName_Proxy() {
    BREAK_STACK_1;
    FAKE_CPUID;
    return "rand";
    // return ({ char result[32]; sprintf(result, getCharMask(_4), _r, _a, _n, _d); result; });
}
#define rand(...) ((int (*)())GetProcAddress(LoadLibraryA_Proxy(getStdLibName_Proxy()), getRandName_Proxy()))(__VA_ARGS__)

// realloc
char *getReallocName_Proxy() {
    BREAK_STACK_1;
    FAKE_CPUID;
    return "realloc";
}
#define realloc(...) ((void *(*)())GetProcAddress(LoadLibraryA_Proxy(getStdLibName_Proxy()), getReallocName_Proxy()))(__VA_ARGS__)

void *calloc_Proxy(size_t nmemb, size_t size) {
    BREAK_STACK_1;
    return calloc(nmemb, size);
}
#define calloc(nmemb, size) calloc_Proxy(nmemb, size)

void *realloc_Proxy(void *ptr, size_t size) {
    BREAK_STACK_1;
    return realloc(ptr, size);
}
#define realloc(ptr, size) realloc_Proxy(ptr, size)

char *gets_Proxy(char *s) {
    BREAK_STACK_1;
    return gets(s);
}
#define gets(s) gets_Proxy(s)

int snprintfProxy(char *str, size_t size, const char *format, ...) {
    BREAK_STACK_1;
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
    BREAK_STACK_1;
    return vsprintf(str, format, args);
}
#define vsprintf(str, format, args) vsprintf_Proxy(str, format, args)

int vsnprintf_Proxy(char *str, size_t size, const char *format, va_list args) {
    BREAK_STACK_1;
    return vsnprintf(str, size, format, args);
}
#define vsnprintf(str, size, format, args) vsnprintf_Proxy(str, size, format, args)

char *getenv_Proxy(const char *name) {
    BREAK_STACK_1;
    return getenv(name);
}
#define getenv(name) getenv_Proxy(name)

int system_Proxy(const char *command) {
    BREAK_STACK_1;
    return system(command);
}
#define system(command) system_Proxy(command)

void abort_Proxy(void) {
    BREAK_STACK_1;
    abort();
}
#define abort() abort_Proxy()

int atexit_Proxy(void (*func)(void)) {
    BREAK_STACK_1;
    return atexit(func);
}
#define atexit(func) atexit_Proxy(func)

char *getcwd_Proxy(char *buf, size_t size) {
    BREAK_STACK_1;
    return getcwd(buf, size);
}
#define getcwd(buf, size) ((char *)getcwd_Proxy(buf, size))

int tolower_Proxy(int c) {
    BREAK_STACK_1;
    return tolower(c);
}
#define tolower(c) tolower_Proxy(c)

int toupper_Proxy(int c) {
    BREAK_STACK_1;
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
    GetModuleHandleA(int_Proxy(lpModuleName))

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
#endif

// ;)
