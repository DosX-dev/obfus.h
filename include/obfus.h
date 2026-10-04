/*
         ██████╗ ██████╗ ███████╗██╗   ██╗███████╗     ██╗
        ██╔═══██╗██╔══██╗██╔════╝██║   ██║██╔════╝     ██║
        ██║   ██║██████╔╝█████╗  ██║   ██║███████╗     ███████║
        ██║   ██║██╔══██╗██╔══╝  ██║   ██║╚════██║     ██╔══██║
        ╚██████╔╝██████╔╝██║     ╚██████╔╝███████║ ██╗ ██║  ██║
         ╚═════╝ ╚═════╝ ╚═╝      ╚═════╝ ╚══════╝ ╚═╝ ╚═╝  ╚═╝
              Very reliable armor for your C programs!
                    Coded by (C) DosX, 2026

 [Additional options]
 ~ CFLOW_V2       = more powerful Control Flow obfuscation (slowly!)
 ~ ANTIDEBUG_V2   = use better dynamic anti-debugging protection
 ~ FAKE_SIGNS     = adds fake signatures of various protectors or packers

 [Advanced code protection]
 ~ VIRT           = allows you to use the functions of a math VM

 [Disabling default features]
 ~ NO_CFLOW       = disable control flow obfuscation
 ~ NO_ANTIDEBUG   = disable antidebug protection

 ~ NO_OBF         = disable obfuscation

 GitHub:
 -> https://github.com/DosX-dev/obfus.h

 (Full documentation and examples are available on the GitHub page)
*/

#ifndef OBFH
#define OBFH

#if !__TINYC__ && !__GNUC__ && !__MINGW32__
#define __attribute__(...)
#endif

// if virtualization disabled
#if NO_OBF == 1 || VIRT != 1
#define VM_ADD(num1, num2) ((num1) + (num2))
#define VM_SUB(num1, num2) ((num1) - (num2))
#define VM_MUL(num1, num2) ((num1) * (num2))
#define VM_DIV(num1, num2) ((num1) / (num2))
#define VM_MOD(num1, num2) ((num1) % (num2))
#define VM_EQU(num1, num2) ((num1) == (num2))
#define VM_NEQ(num1, num2) ((num1) != (num2))
#define VM_LSS(num1, num2) ((num1) < (num2))
#define VM_GTR(num1, num2) ((num1) > (num2))
#define VM_LEQ(num1, num2) ((num1) <= (num2))
#define VM_GEQ(num1, num2) ((num1) >= (num2))
#define VM_ADD_DBL(num1, num2) ((num1) + (num2))
#define VM_SUB_DBL(num1, num2) ((num1) - (num2))
#define VM_MUL_DBL(num1, num2) ((num1) * (num2))
#define VM_DIV_DBL(num1, num2) ((num1) / (num2))
#define VM_LSS_DBL(num1, num2) ((num1) < (num2))
#define VM_GTR_DBL(num1, num2) ((num1) > (num2))
#define VM_IF(condition) if (condition)
#define VM_ELSE_IF(condition) else if (condition)
#define VM_ELSE else
#define VM_OBF_INT(num) (num)
#define VM_OBF_DBL(num) (num)
#endif

#if NO_OBF == 1
#define HIDE_STRING(str) str
#define ANTI_DEBUG 0
#endif

#if !NO_OBF

#include <conio.h>
#include <ctype.h>
#include <direct.h>
#include <limits.h>
#include <math.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#if defined _MSC_VER
#warning obfus.h doesn't support Visual C/C++. You can use [obfusheader.h] by ac3ss0r to obfuscate this app (https://github.com/ac3ss0r/obfusheader.h)
#endif

#if !defined __COUNTER__
#error You are using too old a compiler version!
#endif

#ifndef __asm__
#define __asm__(...)
#endif

#define SECTION_ATTRIBUTE(NAME) __attribute__((section(NAME)))

#define DATA_SECTION_ATTRIBUTE SECTION_ATTRIBUTE(".data")  // Data section

// Fake signatures ;)
#if FAKE_SIGNS == 1

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

static const char FAKE_SCREEN2EXE[] SECTION_ATTRIBUTE(".text") = {0x56, 0x69, 0x64, 0x65,
                                                                  0x6f, 0x20, 0x63, 0x72,
                                                                  0x65, 0x61, 0x74, 0x65,
                                                                  0x64, 0x20, 0x62, 0x79,
                                                                  0x20, 0x53, 0x43, 0x52,
                                                                  0x45, 0x45, 0x4e, 0x32,
                                                                  0x45, 0x58, 0x45, 0x2f,
                                                                  0x53, 0x43, 0x52, 0x45,
                                                                  0x45, 0x4e, 0x32, 0x53,
                                                                  0x57, 0x46};

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

#else

#define OBFH_SECTION_ATTRIBUTE SECTION_ATTRIBUTE(".obfh")  // OBFH section

#endif

// Thanks to @horsicq && @ac3ss0r
#define RND(min, max) \
    (min + (((__COUNTER__ + (__LINE__ * __LINE__)) * 2654435761u) % (max - min + 1)))

#define STACK_STRING(str) ((char[]){str})

#define HIDE_STRING(str) \
    (_0 < RND(1, 255) ? obfh_process_hidden_string(STACK_STRING("\0" str "\0"), (float)__s_rdtsc(RND(0, 255)) != 0.1) : (char *)(ULONG_PTR)((float)__s_rdtsc(RND(0, 255)) == RND(0, 255)))

typedef enum {
    SALT_SHIFT = RND(0xBAD, 0xBEEF)
} VAR_ADDR_SHIFT;

// Mutate the address as a pointer-width integer, preserving the value type.
#define RET_BY_VAR(value)                                                        \
    {                                                                            \
        volatile ULONG_PTR __obfh_ret_shift = SALT_SHIFT;                        \
        ULONG_PTR __obfh_ret_address = (ULONG_PTR) & (value) ^ __obfh_ret_shift; \
        return *(__typeof__(&(value)))(__obfh_ret_address ^ __obfh_ret_shift);   \
    }

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
                            _0 DATA_SECTION_ATTRIBUTE = 0, _1 DATA_SECTION_ATTRIBUTE = 1, _2 DATA_SECTION_ATTRIBUTE = 2, _3 DATA_SECTION_ATTRIBUTE = 3, _4 DATA_SECTION_ATTRIBUTE = 4,
                            _5 DATA_SECTION_ATTRIBUTE = 5, _6 DATA_SECTION_ATTRIBUTE = 6, _7 DATA_SECTION_ATTRIBUTE = 7, _8 DATA_SECTION_ATTRIBUTE = 8, _9 DATA_SECTION_ATTRIBUTE = 9;

#define __obfh_asm__(...) __asm__ __volatile(__VA_ARGS__)

// CPUID/junk instructions explicitly declare their register and flag effects.
#define BREAK_STACK_1        \
    __obfh_asm__(            \
        "xorl %%eax, %%eax;" \
        "jz 1f;"             \
        ".byte 0xE8;"        \
        "1:"                 \
        "cpuid;"             \
        :                    \
        :                    \
        : "eax", "ebx", "ecx", "edx", "cc", "memory")

#define BREAK_STACK_2                  \
    if (_0) __obfh_asm__(".byte 0x00;" \
                         :             \
                         :             \
                         : "eax", "ebx", "ecx", "edx", "cc", "memory")

#define BREAK_STACK_3                                                   \
    switch (_0) {                                                       \
        case RND(1, 1000):                                              \
            __obfh_asm__(".byte 0x00, 0x00;"                            \
                         :                                              \
                         :                                              \
                         : "eax", "ebx", "ecx", "edx", "cc", "memory"); \
    }

#define BREAK_STACK_4        \
    __obfh_asm__(            \
        "xorl %%ebx, %%ebx;" \
        "xorl %%edx, %%edx;" \
        "xorl %%ebx, %%edx;" \
        "jz 1f;"             \
        "mov $4, %%eax;"     \
        ".byte 0x00;"        \
        "1:"                 \
        "cpuid;"             \
        :                    \
        :                    \
        : "eax", "ebx", "ecx", "edx", "cc", "memory")

#define BREAK_STACK_5        \
    __obfh_asm__(            \
        "xorl %%ebx, %%ebx;" \
        "xorl %%eax, %%eax;" \
        "mov %%eax, %%ebx;"  \
        "mov %%edx, %%ebx;"  \
        "xorl %%eax, %%edx;" \
        "jz 1f;"             \
        ".byte 0x20;"        \
        "1:"                 \
        "cpuid;"             \
        :                    \
        :                    \
        : "eax", "ebx", "ecx", "edx", "cc", "memory")

#define BREAK_STACK_6        \
    __obfh_asm__(            \
        "xorl %%edx, %%edx;" \
        "xorl %%eax, %%eax;" \
        "mov %%eax, %%edx;"  \
        "jz 1f;"             \
        ".byte 0xE8;"        \
        "1:"                 \
        "cpuid;"             \
        :                    \
        :                    \
        : "eax", "ebx", "ecx", "edx", "cc", "memory")

#define BREAK_STACK_7        \
    __obfh_asm__(            \
        "xorl %%edx, %%edx;" \
        "jz 1f;"             \
        ".byte 0xE8;"        \
        "1:"                 \
        "cpuid;"             \
        :                    \
        :                    \
        : "eax", "ebx", "ecx", "edx", "cc", "memory")

#define BREAK_STACK_8        \
    __obfh_asm__(            \
        "xorl %%eax, %%eax;" \
        "jz 1f;"             \
        ".byte 0x50;"        \
        "1:"                 \
        "cpuid;"             \
        :                    \
        :                    \
        : "eax", "ebx", "ecx", "edx", "cc", "memory")

#define BREAK_STACK_9        \
    __obfh_asm__(            \
        "xorl %%edx, %%edx;" \
        "jz 1f;"             \
        ".byte 0x00, 0x00;"  \
        "1:"                 \
        "cpuid;"             \
        :                    \
        :                    \
        : "eax", "ebx", "ecx", "edx", "cc", "memory")

#if defined(__x86_64__)
#define BAD_JMP __obfh_asm__("cpuid; mov %eax, %rax; mov %ebx, %edx; .byte 0xFF, 0x25, 0xF1, 0xF2, 0xF3, 0xF4;")
#else
#define BAD_JMP __obfh_asm__(".byte 0xEB, 0xE1;")
#endif

#define BAD_CALL __obfh_asm__(".byte 0xB8;")

void obfh_junk_func_args(int z, ...) OBFH_SECTION_ATTRIBUTE {
    BREAK_STACK_1;
    __obfh_asm__("nop;");
    return;
}

void obfh_junk_func() DATA_SECTION_ATTRIBUTE {
    BREAK_STACK_5;
    __obfh_asm__("nop;");
    return;
}

#define __CRASH       \
    __obfh_asm__(     \
        ".byte 0xED;" \
        "int $3;");   \
    exit(1);

#define TRUE ((((_9 + _7 + (RND(0, 1000) * _0))) / _8) - _1)
#define FALSE (((_3 + _6 + (RND(0, 1000) * _0)) - _9) * RND(0, 255))

#define FAKE_CPUID __obfh_asm__( \
    "nop;"                       \
    "cpuid;"                     \
    "nop;"                       \
    :                            \
    :                            \
    : "eax", "ebx", "ecx", "edx", "cc", "memory")

#define NOP_FLOOD                                  \
    (RND(0, 1000)) + obfh_int_proxy(RND(0, 1000)); \
    if (obfh_junk_func_args) {                     \
        __obfh_asm__("nop;");                      \
    }                                              \
    do {                                           \
        __obfh_asm__(                              \
            "nop;"                                 \
            "nop;");                               \
    } while (RND(0, 200) * _0)

void *malloc_proxy(size_t size) {
    BREAK_STACK_1;
    return malloc(size);
}
#define malloc(...) malloc_proxy(__VA_ARGS__)

static float rndValueToProxy = RND(0, 10);

int obfh_int_proxy(int value) OBFH_SECTION_ATTRIBUTE {
    RET_BY_VAR(value);
}

// Preserve pointer and SIZE_T width on both Windows targets.
ULONG_PTR obfh_uintptr_proxy(ULONG_PTR value) OBFH_SECTION_ATTRIBUTE {
    RET_BY_VAR(value);
}

#define OBFH_PTR(type, value) ((type)obfh_uintptr_proxy((ULONG_PTR)(value)))

double obfh_double_proxy(double value) OBFH_SECTION_ATTRIBUTE {
    RET_BY_VAR(value);
}

float obfh_condition_true();

// Hidden string access
char *obfh_process_hidden_string(char *string, ...) OBFH_SECTION_ATTRIBUTE {
    BREAK_STACK_1;

    if (!obfh_condition_true() || _0) {
        BAD_JMP;
    }

    // ['\0', 's', 't', 'r', 'i', 'n', 'g'] => "string"
    // STACK_STRING storage belongs to the HIDE_STRING call site.
    return string + 1;
}

float obfh_condition_true() OBFH_SECTION_ATTRIBUTE {
    BREAK_STACK_1;
    return _1 && TRUE;
}

int obfh_condition_proxy(float junk, float condition, ...) OBFH_SECTION_ATTRIBUTE {
    RET_BY_VAR(condition);
}

long double __s_rdtsc(float junk, ...) OBFH_SECTION_ATTRIBUTE {
    {
        unsigned int lo, hi;
        __obfh_asm__(".byte 0x0f, 0x31;"  // rdtsc
                     : "=a"(lo), "=d"(hi)
                     :
                     : "memory");

        unsigned long long rdtsc_result = ((unsigned long long)hi << 32) | lo;

        if (rdtsc_result == obfh_int_proxy(0)) __obfh_asm__(".byte 0xE8;");
    }

    unsigned long long time;
    unsigned int low, high;
    static volatile LONG rdtscpAvailable = -1;
    LONG supported = rdtscpAvailable;
    if (supported < 0) {
        unsigned int leaf, b, c, d;
        __obfh_asm__("cpuid;"
                     : "=a"(leaf), "=b"(b), "=c"(c), "=d"(d)
                     : "a"(0x80000000u)
                     : "cc", "memory");
        if (leaf >= 0x80000001u) {
            __obfh_asm__("cpuid;"
                         : "=a"(leaf), "=b"(b), "=c"(c), "=d"(d)
                         : "a"(0x80000001u)
                         : "cc", "memory");
        } else
            d = 0;
        supported = !!(d & (1u << 27));
        InterlockedCompareExchange(&rdtscpAvailable, supported, -1);
    }
    if (supported) {
        unsigned int auxiliary;
        __obfh_asm__(".byte 0x0f, 0x01, 0xf9;"  // rdtscp
                     : "=a"(low), "=d"(high), "=c"(auxiliary)
                     :
                     : "memory");
    } else {
        __obfh_asm__(".byte 0x0f, 0x31;"
                     : "=a"(low), "=d"(high)
                     :
                     : "memory");
    }
    time = ((unsigned long long)high << 32) | low;
    return time;
}

// =============================================================
// Control Flow (global)
#if NO_CFLOW != 1

// Every intermediate contributes to the branch token; values stay exactly
// representable in float on both TCC targets. No shared state or VM required.
#define OBFH_FLOW_BASE(site) (((site)&1023u) + 17u)
#define OBFH_FLOW_STEP(site) ((((site) >> 5) & 31u) * 2u + 1u)
#define OBFH_FLOW_FIRST(value, site) (((site)&1u) ? (((value)*3u + ((site)&255u)) ^ ((site)&4095u)) : (((value)*5u + ((site)&127u)) ^ (((site) >> 1) & 4095u)))
#if CFLOW_V2
#define OBFH_FLOW_FINAL(value, site) (((((value)*5u + 7u) ^ (((site) >> 3) & 2047u)) * 3u + 11u) ^ ((site)&8191u))
#else
#define OBFH_FLOW_FINAL(value, site) (value)
#endif

double obfh_flow_token(float encoded, unsigned int site) OBFH_SECTION_ATTRIBUTE {
    // Retain the misleading byte without serializing every branch with CPUID.
    __obfh_asm__("xorl %%eax, %%eax; jz 1f; .byte 0xE8; 1:"
                 :
                 :
                 : "eax", "cc", "memory");
    volatile unsigned int input = (unsigned int)obfh_double_proxy((double)encoded);
    // Keep a misleading failure path without putting a timestamp on every if.
    if (obfh_int_proxy((int)input) > 4095) {
        BAD_CALL;
    }
    volatile unsigned int stage = OBFH_FLOW_FIRST(input, site);
    volatile float converted = (float)obfh_int_proxy((int)stage);
#if CFLOW_V2
    volatile unsigned int second = ((unsigned int)obfh_double_proxy((double)converted) * 5u + 7u) ^ ((site >> 3) & 2047u);
    volatile float intermediate = (float)obfh_int_proxy((int)second);
    stage = ((unsigned int)obfh_double_proxy((double)intermediate) * 3u + 11u) ^ (site & 8191u);
    converted = (float)obfh_int_proxy((int)stage);
    BREAK_STACK_2;
#endif
    return obfh_double_proxy((double)(float)obfh_condition_proxy((float)(site & 255u), converted, site));
}

#define OBFH_FLOW_CONDITION(condition, site_value) ({                                                                                                                        \
    unsigned int __obfh_flow_site = (site_value);                                                                                                                            \
    unsigned int __obfh_flow_input = OBFH_FLOW_BASE(__obfh_flow_site) + !!(condition)*OBFH_FLOW_STEP(__obfh_flow_site);                                                      \
    double __obfh_flow_result = obfh_flow_token((float)__obfh_flow_input, __obfh_flow_site);                                                                                 \
    __obfh_flow_result == (double)OBFH_FLOW_FINAL(OBFH_FLOW_FIRST(OBFH_FLOW_BASE(__obfh_flow_site) + OBFH_FLOW_STEP(__obfh_flow_site), __obfh_flow_site), __obfh_flow_site); \
})

// if
#define if(cond) if (OBFH_FLOW_CONDITION(cond, RND(1, 65535)))

// else
#define else      \
    else if (0) { \
        BAD_CALL; \
    }             \
    else

#define OBFUS_CONDITION_BLOCK(...) OBFH_FLOW_CONDITION((__VA_ARGS__), RND(1, 65535))

// break
#define break                                                  \
    {                                                          \
        if (OBFUS_CONDITION_BLOCK(RND(1, 255))) BREAK_STACK_1; \
        break;                                                 \
    }

// switch
#define switch(...)                         \
    if (OBFUS_CONDITION_BLOCK(RND(1, 255))) \
        switch (__VA_ARGS__)

// while
#define while(...) while ((float)__s_rdtsc(RND(0, 255)) != 0.1 && (&__s_rdtsc != !&__s_rdtsc) && (__VA_ARGS__))

// for
#define for(...)                            \
    if (OBFUS_CONDITION_BLOCK(RND(1, 255))) \
        for (__VA_ARGS__)

#endif
// =============================================================

// =============================================================
// Virtualization (global)
#if VIRT == 1
typedef enum {
    OP__ADD = RND(0, 900) * __COUNTER__ * 5,
    OP__SUB = RND(1000, 1900) * __COUNTER__ * 5,
    OP__MUL = RND(2000, 2900) * __COUNTER__ * 5,
    OP__DIV = RND(3000, 3900) * __COUNTER__ * 5,
    OP__MOD = RND(4000, 4900) * __COUNTER__ * 5,
    OP__EQU = RND(5000, 5900) * __COUNTER__ * 5,
    OP__NEQ = RND(6000, 6900) * __COUNTER__ * 5,
    OP__GTR = RND(7000, 7900) * __COUNTER__ * 5,
    OP__LSS = RND(8000, 8900) * __COUNTER__ * 5,
    OP__LEQ = RND(9000, 9900) * __COUNTER__ * 5,
    OP__GEQ = RND(10000, 10900) * __COUNTER__ * 5,
    OP__NOP = RND(11000, 11900) * __COUNTER__ * 5,
    OP__BRANCH = RND(12000, 12900) * __COUNTER__ * 5
} CMD;

typedef enum {
    SALT_CMD = RND(100, 900),
    SALT_NUM1 = RND(16, 48),
    SALT_NUM2 = RND(16, 48)
} VM_SALT;

static int _salt = SALT_CMD;

#define _VM_DEMUTATOR_KEY (__COUNTER__) / 5
#define _VM_MUTATOR_KEY (__COUNTER__ - 1) / 5

#define _VM_ENCRYPT_INT(value) ((value - _VM_MUTATOR_KEY) * ~SALT_CMD)
#define _ENC_OP__ADD _VM_ENCRYPT_INT(OP__ADD)
#define _ENC_OP__SUB _VM_ENCRYPT_INT(OP__SUB)
#define _ENC_OP__MUL _VM_ENCRYPT_INT(OP__MUL)
#define _ENC_OP__DIV _VM_ENCRYPT_INT(OP__DIV)
#define _ENC_OP__MOD _VM_ENCRYPT_INT(OP__MOD)
#define _ENC_OP__EQU _VM_ENCRYPT_INT(OP__EQU)
#define _ENC_OP__NEQ _VM_ENCRYPT_INT(OP__NEQ)
#define _ENC_OP__GTR _VM_ENCRYPT_INT(OP__GTR)
#define _ENC_OP__LSS _VM_ENCRYPT_INT(OP__LSS)
#define _ENC_OP__LEQ _VM_ENCRYPT_INT(OP__LEQ)
#define _ENC_OP__GEQ _VM_ENCRYPT_INT(OP__GEQ)
#define _ENC_OP__NOP _VM_ENCRYPT_INT(OP__NOP)
#define _ENC_OP__BRANCH _VM_ENCRYPT_INT(OP__BRANCH)

#define VM_ADD(num1, num2) (long)Obfh_VirtualMachine(_VM_DEMUTATOR_KEY, _ENC_OP__ADD, (long double)(num1) * -1 + SALT_NUM1, RND(1, 500), (long double)(num2) * -1 + SALT_NUM2, RND(1, 500))
#define VM_SUB(num1, num2) (long)Obfh_VirtualMachine(_VM_DEMUTATOR_KEY, _ENC_OP__SUB, (long double)(num1) * -1 + SALT_NUM1, RND(1, 500), (long double)(num2) * -1 + SALT_NUM2, RND(1, 500))
#define VM_MUL(num1, num2) (long)Obfh_VirtualMachine(_VM_DEMUTATOR_KEY, _ENC_OP__MUL, (long double)(num1) * -1 + SALT_NUM1, RND(1, 500), (long double)(num2) * -1 + SALT_NUM2, RND(1, 500))
#define VM_DIV(num1, num2) (long)Obfh_VirtualMachine(_VM_DEMUTATOR_KEY, _ENC_OP__DIV, (long double)(num1) * -1 + SALT_NUM1, RND(1, 500), (long double)(num2) * -1 + SALT_NUM2, RND(1, 500))
#define VM_MOD(num1, num2) (long)Obfh_VirtualMachine(_VM_DEMUTATOR_KEY, _ENC_OP__MOD, (long double)(num1) * -1 + SALT_NUM1, RND(1, 500), (long double)(num2) * -1 + SALT_NUM2, RND(1, 500))
#define VM_EQU(num1, num2) (long)Obfh_VirtualMachine(_VM_DEMUTATOR_KEY, _ENC_OP__EQU, (long double)(num1) * -1 + SALT_NUM1, RND(1, 500), (long double)(num2) * -1 + SALT_NUM2, RND(1, 500))
#define VM_NEQ(num1, num2) (long)Obfh_VirtualMachine(_VM_DEMUTATOR_KEY, _ENC_OP__NEQ, (long double)(num1) * -1 + SALT_NUM1, RND(1, 500), (long double)(num2) * -1 + SALT_NUM2, RND(1, 500))
#define VM_LSS(num1, num2) (long)Obfh_VirtualMachine(_VM_DEMUTATOR_KEY, _ENC_OP__LSS, (long double)(num1) * -1 + SALT_NUM1, RND(1, 500), (long double)(num2) * -1 + SALT_NUM2, RND(1, 500))
#define VM_GTR(num1, num2) (long)Obfh_VirtualMachine(_VM_DEMUTATOR_KEY, _ENC_OP__GTR, (long double)(num1) * -1 + SALT_NUM1, RND(1, 500), (long double)(num2) * -1 + SALT_NUM2, RND(1, 500))
#define VM_LEQ(num1, num2) (long)Obfh_VirtualMachine(_VM_DEMUTATOR_KEY, _ENC_OP__LEQ, (long double)(num1) * -1 + SALT_NUM1, RND(1, 500), (long double)(num2) * -1 + SALT_NUM2, RND(1, 500))
#define VM_GEQ(num1, num2) (long)Obfh_VirtualMachine(_VM_DEMUTATOR_KEY, _ENC_OP__GEQ, (long double)(num1) * -1 + SALT_NUM1, RND(1, 500), (long double)(num2) * -1 + SALT_NUM2, RND(1, 500))
#define VM_OBF_INT(num1) (VM_MUL(RND(1, 999), 0) ? RND(1, 9999) : (long)Obfh_VirtualMachine(_VM_DEMUTATOR_KEY, _ENC_OP__NOP, (long double)(num1) * -1 + SALT_NUM1, RND(1, 500), RND(1, 99999999) * -1 + SALT_NUM2, RND(1, 500)))

#define VM_ADD_DBL(num1, num2) Obfh_VirtualMachine(_VM_DEMUTATOR_KEY, _ENC_OP__ADD, (double)(num1) * -1 + SALT_NUM1, RND(1, 500), (double)(num2) * -1 + SALT_NUM2, RND(1, 500))
#define VM_SUB_DBL(num1, num2) Obfh_VirtualMachine(_VM_DEMUTATOR_KEY, _ENC_OP__SUB, (double)(num1) * -1 + SALT_NUM1, RND(1, 500), (double)(num2) * -1 + SALT_NUM2, RND(1, 500))
#define VM_MUL_DBL(num1, num2) Obfh_VirtualMachine(_VM_DEMUTATOR_KEY, _ENC_OP__MUL, (double)(num1) * -1 + SALT_NUM1, RND(1, 500), (double)(num2) * -1 + SALT_NUM2, RND(1, 500))
#define VM_DIV_DBL(num1, num2) Obfh_VirtualMachine(_VM_DEMUTATOR_KEY, _ENC_OP__DIV, (double)(num1) * -1 + SALT_NUM1, RND(1, 500), (double)(num2) * -1 + SALT_NUM2, RND(1, 500))
#define VM_LSS_DBL(num1, num2) (long)Obfh_VirtualMachine(_VM_DEMUTATOR_KEY, _ENC_OP__LSS, (double)(num1) * -1 + SALT_NUM1, RND(1, 500), (double)(num2) * -1 + SALT_NUM2, RND(1, 500))
#define VM_GTR_DBL(num1, num2) (long)Obfh_VirtualMachine(_VM_DEMUTATOR_KEY, _ENC_OP__GTR, (double)(num1) * -1 + SALT_NUM1, RND(1, 500), (double)(num2) * -1 + SALT_NUM2, RND(1, 500))
#define VM_OBF_DBL(num1) (VM_MUL(RND(1, 999), 0) ? RND(1, 9999) : Obfh_VirtualMachine(_VM_DEMUTATOR_KEY, _ENC_OP__NOP, (long double)(num1) * -1 + SALT_NUM1, RND(1, 500), RND(1, 99999999) * -1 + SALT_NUM2, RND(1, 500)))

// Each condition is evaluated once before entering the floating VM path.
int obfh_vm_branch(long double key, int command, float condition, unsigned int site, unsigned int kind) OBFH_SECTION_ATTRIBUTE;
long double obfh_vm_branch_program(long double condition, unsigned int nonce, unsigned int site, unsigned int kind) OBFH_SECTION_ATTRIBUTE;
#define VM_IF(condition) if (obfh_vm_branch(_VM_DEMUTATOR_KEY, _ENC_OP__BRANCH, !!(condition), RND(1, 65535), _0))
#define VM_ELSE_IF(condition) else if (obfh_vm_branch(_VM_DEMUTATOR_KEY, _ENC_OP__BRANCH, !!(condition), RND(1, 65535), _1))
#define VM_ELSE else if (obfh_vm_branch(_VM_DEMUTATOR_KEY, _ENC_OP__BRANCH, !!obfh_condition_true(), RND(1, 65535), _2))

long double Obfh_VirtualMachine(long double uni_key, int command, long double num1, long double junk_2, long double num2, long double junk_3) OBFH_SECTION_ATTRIBUTE {
    volatile long double obfhVmResult = 0;
    BREAK_STACK_1;
    goto firstFakePoint;

    // Restore values
restoreCommand:
    BREAK_STACK_1;
    command /= ~_salt;
    command += uni_key;
    goto restoreNum2;

restoreNum1:
    BREAK_STACK_1;
    num1 -= SALT_NUM1;
    num1 *= (-1 * _1);
    goto letsExecute;

restoreNum2:
    BREAK_STACK_1;
    num2 -= SALT_NUM2;
    num2 *= (-1 * _1);
    goto restoreNum1;

firstFakePoint:
    BREAK_STACK_2;
    goto secondFakePoint;

letsExecute:

    switch (command) {
        case -1 * __LINE__:
            goto restoreCommand;
        case -2 * __LINE__:
            goto firstFakePoint;
        case -3 * __LINE__:
            return _0 * ~_1 + junk_2;
        case -4 * __LINE__:
            goto restoreNum2;
        case -5 * __LINE__:
            goto restoreNum1;
        case -6 * __LINE__:
            __obfh_asm__(".byte 0xFF, 0x25;");  // fake JMP
        case -7 * __LINE__:

#if defined(__x86_64__) || defined(_M_X64)  // fake code, just for decompiler break
            __obfh_asm__(
                "mov %%rax, %%rbx;"
                "xor %%rcx, %%rax;"
                "shr $8, %%rdx;"
                "shl $4, %%rax;"
                "push %%rbx;"
                "pop %%rbx;"
                "inc %%rax;"
                "dec %%rdx;"
                :
                :
                : "rax",
                  "rbx", "rcx", "rdx");
#elif defined(__i386__) || defined(_M_IX86)
            __obfh_asm__(
                "mov %%ebx, %%eax;"
                "add %%ecx, %%eax;"
                "sub %%edx, %%ebx;"
                "shl %%cl, %%ecx;"
                "push %%ebx;"
                "pop %%ebx;"
                "sar %%cl, %%ecx;"
                "or %%edx, %%eax;"
                "dec %%edx;"
                :
                :
                : "eax",
                  "ebx", "ecx", "edx");
#else
#endif
        case -8 * __LINE__:
            BAD_JMP;

        case OP__ADD:  // plus
            obfhVmResult = (num1 + num2) + VM_MUL(junk_3, _0);
            goto afterCalc;
        case OP__SUB:  // minus
            obfhVmResult = (num1 - num2) + VM_MUL(junk_3, _0);
            goto afterCalc;
        case OP__MUL:  // multiply
            if (num1 == _0 || num2 == _0)
                obfhVmResult = _0;
            else
                return num1 * num2;

            goto afterCalc;
        case OP__DIV:  // divide
            if (num2 != _0)
                obfhVmResult = num1 / num2;
            else
                obfhVmResult = VM_ADD(_0, _0);
            goto afterCalc;
        case OP__MOD:  // modulo
            if ((int)num2 != 0 && !((int)num1 == INT_MIN && (int)num2 == -1))
                obfhVmResult = (int)num1 % (int)num2;
            else
                obfhVmResult = _0;
            goto afterCalc;
        case OP__EQU:            // equal
            if (num1 == num2) {  // 1 + 0 = 1
                obfhVmResult = VM_ADD(_1, _0);
            } else {
                obfhVmResult = _0;
            }
            goto afterCalc;
        case OP__NEQ:            // not equal
            if (num1 != num2) {  // 1 + 0 = 1
                obfhVmResult = VM_ADD(_0, _1) + VM_MUL(junk_2, _0);
            } else {
                obfhVmResult = VM_MUL(junk_3, _0);
            }
            goto afterCalc;
        case OP__LSS:
            obfhVmResult = num1 != num2 && !(num1 + VM_MUL(junk_2, _0) > num2);
            goto afterCalc;
        case OP__GTR:
            obfhVmResult = num1 != num2 && !(num1 + VM_MUL(junk_2, _0) < num2);
            goto afterCalc;
        case OP__LEQ:
            obfhVmResult = !(num1 + VM_MUL(junk_2, _0) > num2);
            goto afterCalc;
        case OP__GEQ:
            obfhVmResult = (num1 + VM_MUL(junk_2, _0) > num2) || (num1 == num2);
            goto afterCalc;
        case OP__BRANCH:
            obfhVmResult = obfh_vm_branch_program(num1, (unsigned int)num2, (unsigned int)junk_2, (unsigned int)junk_3);
            goto afterCalc;
        case OP__NOP:
            obfhVmResult = num1;
            goto afterCalc;
        default:
            obfhVmResult = _0 * (uni_key * _3);
            BAD_JMP;
    }
    BREAK_STACK_8;

    long double result = uni_key;
afterCalc:

    goto saveValueToLocal;
resetResult:
    obfhVmResult = 0;
    goto returnValue;
saveValueToLocal:
    result = obfhVmResult;
    goto resetResult;

returnValue:
    return result;

    __obfh_asm__(".byte 0xFF, 0xE0;");  // fake indirect JMP (EAX on x86, RAX on x64)

secondFakePoint:
    BREAK_STACK_7;
    goto restoreCommand;
}

// A local encoded instruction pointer selects one of three decision programs.
long double obfh_vm_branch_program(long double condition, unsigned int nonce, unsigned int site, unsigned int kind) OBFH_SECTION_ATTRIBUTE {
    BREAK_STACK_1;
    unsigned int mask = (nonce * 33u ^ site * 17u ^ kind * 257u) & 0xfffffu;
    unsigned int variant = (nonce ^ site ^ kind) % 3u;
    volatile unsigned int pc = (variant + 1u) ^ mask;
    volatile unsigned int token = _0;
    for (;;) {
        switch (pc ^ mask) {
            case 1:
                pc = (VM_GTR_DBL(condition, _0) ? 4u : 5u) ^ mask;
                break;
            case 2:
                pc = (VM_EQU((long)condition, _0) ? 5u : 4u) ^ mask;
                break;
            case 3:
                pc = (VM_EQU((long)VM_ADD_DBL(condition, nonce), nonce) ? 5u : 4u) ^ mask;
                break;
            case 4:
                token = (unsigned int)VM_ADD(VM_MUL(nonce, _8), _5);
                pc = 6u ^ mask;
                break;
            case 5:
                token = (unsigned int)VM_ADD(VM_MUL(nonce, _8), _2);
                pc = 6u ^ mask;
                break;
            case 6: {
                // All token bits fit exactly in float, including on x86.
                float converted = (float)obfh_int_proxy((int)token);
                return (unsigned int)obfh_double_proxy((double)converted) ^ mask;
            }
            default:
                BAD_JMP;
                return _0;
        }
    }
}

int obfh_vm_branch(long double key, int command, float condition, unsigned int site, unsigned int kind) OBFH_SECTION_ATTRIBUTE {
    BREAK_STACK_7;
    unsigned int low, high;
    __obfh_asm__(".byte 0x0f, 0x31;"
                 : "=a"(low), "=d"(high)
                 :
                 : "memory");
    unsigned int nonce = ((low ^ high ^ site) & 0xfffffu) + 1u;
    unsigned int mask = (nonce * 33u ^ site * 17u ^ kind * 257u) & 0xfffffu;
    float converted = (float)obfh_double_proxy((double)condition);
    long double response = Obfh_VirtualMachine(key, command, -(long double)converted + SALT_NUM1,
                                               site, -(long double)nonce + SALT_NUM2, kind);
    float expected = (float)((nonce * 8u + 5u) ^ mask);
    long verified = VM_EQU((long)response, (long)obfh_double_proxy((double)expected));
    return obfh_condition_proxy((float)site, (float)verified, nonce);
}

#endif
// =============================================================

// Caller-owned storage keeps the mask valid and avoids shared-buffer races.
char *getCharMask(int count, char *mask, size_t capacity) OBFH_SECTION_ATTRIBUTE {
    BREAK_STACK_1;
    if (!mask || !capacity || count < 0 || (size_t)count > (capacity - 1) / 2) return NULL;
    int i = (((_1 * _5) - _4) + _1) - _2;
    BREAK_STACK_1;
    char *ptr = mask;
    for (i = _0; i < count; ++i) {
        *ptr++ = '%';
        *ptr++ = _c;
    }
    *ptr = _0;
    BREAK_STACK_8;
    FAKE_CPUID;
    return mask;
}

// WriteConsoleA
BOOL WriteConsoleA_proxy(HANDLE hConsoleOutput, const void *lpBuffer, DWORD nNumberOfCharsToWrite, LPDWORD lpNumberOfCharsWritten, LPVOID lpReserved) OBFH_SECTION_ATTRIBUTE {
    BREAK_STACK_1;
    FAKE_CPUID;
    return WriteConsoleA(hConsoleOutput, lpBuffer, nNumberOfCharsToWrite, lpNumberOfCharsWritten, lpReserved);
}
#define WriteConsoleA(...) WriteConsoleA_proxy(__VA_ARGS__)

// GetStdHandle
HANDLE GetStdHandle_proxy(DWORD nStdHandle) OBFH_SECTION_ATTRIBUTE {
    BREAK_STACK_1;
    FAKE_CPUID;
    return GetStdHandle(obfh_int_proxy(nStdHandle));
}
#define GetStdHandle(...) GetStdHandle_proxy(__VA_ARGS__)

HMODULE GetModuleHandleA_proxy(LPCSTR lpModuleName) OBFH_SECTION_ATTRIBUTE {
    BREAK_STACK_9;
    FAKE_CPUID;
    return GetModuleHandleA(lpModuleName);
}
#define GetModuleHandleA(...) GetModuleHandleA_proxy(__VA_ARGS__)

// strcmp
int strcmp_custom(const char *str1, const char *str2) OBFH_SECTION_ATTRIBUTE {
    BREAK_STACK_1;
    while (*str1 != '\0' || *str2 != '\0') {
        NOP_FLOOD;
        if ((obfh_int_proxy((unsigned char)*str1) < obfh_int_proxy((unsigned char)*str2)) && obfh_int_proxy(_1)) {
            return (obfh_int_proxy(_2) / _2) * -1;  // -1
        } else if (obfh_int_proxy((unsigned char)*str1) > obfh_int_proxy((unsigned char)*str2)) {
            return obfh_int_proxy(_0 + _1);  // 1
        }
        str1 += obfh_int_proxy(_1);
        str2 += obfh_int_proxy(_2 - _1);
    }
    FAKE_CPUID;
    return _0;
}
#define strcmp(...) strcmp_custom(__VA_ARGS__)

// strlen
size_t strlen_custom(const char *str) OBFH_SECTION_ATTRIBUTE {
    BREAK_STACK_1;
    size_t length = _0;
    while (*str != '\0') {
        length += obfh_int_proxy(_1);
        str += obfh_int_proxy(_2 - _1);
    }
    FAKE_CPUID;
    return obfh_uintptr_proxy(length + (RND(0, 1000) * _0));
}
#define strlen(...) strlen_custom(__VA_ARGS__)

// Forward declaration for forwarded-export module loading.
HMODULE LoadLibraryA_proxy(LPCSTR lpLibFileName);

// Bounded string scan used by the export parser.
const char *obfh_find_zero(const void *buffer, size_t count) {
    BREAK_STACK_1;
    const char *bytes = buffer;
    for (size_t i = _0; i < count; ++i)
        if (bytes[i] == _0) return bytes + i;
    BREAK_STACK_1;
    return NULL;
}
// Check an RVA range against the loaded image size.
int obfh_image_range(DWORD size, DWORD rva, size_t length) {
    return rva <= size && length <= (size_t)(size - rva);
}
// GetProcAddress: custom PE export lookup, including ordinals and forwarders.
FARPROC obfh_find_export(HMODULE hModule, LPCSTR lpProcName, unsigned int depth) OBFH_SECTION_ATTRIBUTE {
    BREAK_STACK_2;
    BREAK_STACK_1;
    obfh_junk_func_args(RND(0, 885));
    BREAK_STACK_1;
    if (!hModule || !lpProcName || depth >= 32 || ((ULONG_PTR)hModule & 3)) return NULL;
    // Module handles must designate OS-loaded modules, as required by WinAPI.
    BYTE *base = (BYTE *)hModule;
    PIMAGE_DOS_HEADER dos = (PIMAGE_DOS_HEADER)base;
    if (dos->e_magic != IMAGE_DOS_SIGNATURE || dos->e_lfanew < sizeof(*dos)) return NULL;
    PIMAGE_NT_HEADERS nt = (PIMAGE_NT_HEADERS)(base + dos->e_lfanew);
    if (nt->Signature != IMAGE_NT_SIGNATURE || nt->OptionalHeader.NumberOfRvaAndSizes <= IMAGE_DIRECTORY_ENTRY_EXPORT) return NULL;
    DWORD size = nt->OptionalHeader.SizeOfImage;
    IMAGE_DATA_DIRECTORY exports = nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];
    if (!exports.VirtualAddress || !obfh_image_range(size, exports.VirtualAddress, exports.Size) ||
        exports.Size < sizeof(IMAGE_EXPORT_DIRECTORY)) return NULL;
    PIMAGE_EXPORT_DIRECTORY table = (PIMAGE_EXPORT_DIRECTORY)(base + exports.VirtualAddress);
    if (table->NumberOfNames > size / sizeof(DWORD) || table->NumberOfNames > size / sizeof(WORD) ||
        table->NumberOfFunctions > size / sizeof(DWORD)) return NULL;
    if (!obfh_image_range(size, table->AddressOfNames, table->NumberOfNames * sizeof(DWORD)) ||
        !obfh_image_range(size, table->AddressOfNameOrdinals, table->NumberOfNames * sizeof(WORD)) ||
        !obfh_image_range(size, table->AddressOfFunctions, table->NumberOfFunctions * sizeof(DWORD))) return NULL;
    DWORD *names = (DWORD *)(base + table->AddressOfNames);
    WORD *ordinals = (WORD *)(base + table->AddressOfNameOrdinals);
    DWORD *functions = (DWORD *)(base + table->AddressOfFunctions);
    DWORD index = table->NumberOfFunctions;
    if ((ULONG_PTR)lpProcName <= 0xffff) {
        DWORD ordinal = (DWORD)(ULONG_PTR)lpProcName;
        if (ordinal < table->Base || ordinal - table->Base >= table->NumberOfFunctions) return NULL;
        index = ordinal - table->Base;
    } else {
        for (DWORD i = 0; i < table->NumberOfNames; ++i) {
            if (!obfh_image_range(size, names[i], 1) || !obfh_find_zero(base + names[i], size - names[i])) return NULL;
            if (strcmp(lpProcName, (const char *)(base + names[i])) == 0) {
                index = ordinals[i];
                break;
            }
        }
    }
    BREAK_STACK_2;
    if (index >= table->NumberOfFunctions) return NULL;
    DWORD rva = functions[index];
    if (!rva || !obfh_image_range(size, rva, 1)) return NULL;
    if (rva >= exports.VirtualAddress && rva - exports.VirtualAddress < exports.Size) {
        // Forwarded export: MODULE.symbol or MODULE.#ordinal.
        const char *forward = (const char *)(base + rva);
        size_t limit = exports.Size - (rva - exports.VirtualAddress);
        const char *end = obfh_find_zero(forward, limit);
        if (!end) return NULL;
        const char *dot = NULL;
        for (const char *cursor = forward; cursor < end; ++cursor)
            if (*cursor == '.') dot = cursor;
        if (!dot || dot == forward || dot + 1 == end) return NULL;
        char moduleName[MAX_PATH];
        size_t length = dot - forward;
        if (length >= sizeof(moduleName)) return NULL;
        for (size_t i = _0; i < length; ++i) moduleName[i] = forward[i];
        moduleName[length] = 0;
        HMODULE target = GetModuleHandleA(moduleName);
        if (!target) target = LoadLibraryA_proxy(moduleName);
        if (!target) return NULL;
        const char *symbol = dot + 1;
        if (*symbol == '#') {
            unsigned int ordinal = 0;
            if (++symbol == end) return NULL;
            for (; symbol < end; ++symbol) {
                if (*symbol < '0' || *symbol > '9' || ordinal > (65535u - (*symbol - '0')) / 10) return NULL;
                ordinal = ordinal * 10 + (*symbol - '0');
            }
            if (!ordinal) return NULL;
            return obfh_find_export(target, (LPCSTR)(ULONG_PTR)ordinal, depth + 1);
        }
        return obfh_find_export(target, symbol, depth + 1);
    }
    FAKE_CPUID;
    return (FARPROC)(base + rva);
}
FARPROC GetProcAddress_custom(HMODULE hModule, LPCSTR lpProcName) OBFH_SECTION_ATTRIBUTE {
    FARPROC result = obfh_find_export(hModule, lpProcName, 0);
    BREAK_STACK_1;
    return result;
}
#define GetProcAddress(...) GetProcAddress_custom(__VA_ARGS__)

// LoadLibraryA: dynamic loader resolution and proxy chain.
HMODULE LoadLibraryA_0(LPCSTR lpLibFileName) OBFH_SECTION_ATTRIBUTE {
    switch (_0) {
        case 1:
            __obfh_asm__(".byte 0x74;");
            break;
        case 0: {
            BREAK_STACK_3;
            typedef HMODULE(WINAPI * LoadLibraryAFunc)(LPCSTR);
            static PVOID volatile cachedLoader;
            LoadLibraryAFunc loader = (LoadLibraryAFunc)InterlockedCompareExchangePointer(&cachedLoader, NULL, NULL);
            if (!loader) {
                char mask[32], libName[32], funcName[32];
                char *format = getCharMask(_6, mask, sizeof mask);
                format[_6 * _2] = '%';
                format[_6 * _2 + _1] = _d;
                format[_6 * _2 + _2] = _0;
                sprintf(libName, format, _k, _e, _r, _n, _e, _l, _4 * _8);
                HMODULE kernel = GetModuleHandleA(libName);
                if (!kernel) return NULL;
                FAKE_CPUID;
                char charL = _L;
                obfh_junk_func_args(_0 + RND(1, 5));
                BREAK_STACK_2;
                format = getCharMask(_4, mask, sizeof mask);
                sprintf(funcName, format, obfh_int_proxy(charL), obfh_int_proxy(_o), obfh_int_proxy(_a), obfh_int_proxy(_d));
                char tail[] = {_L, _i, _b, _r, _a, _r, _y, _A, _0};
                for (int i = _0; i <= _8; ++i) funcName[_4 + i] = tail[i];
                FAKE_CPUID;
                loader = (LoadLibraryAFunc)GetProcAddress(kernel, funcName);
                if (loader) InterlockedCompareExchangePointer(&cachedLoader, (PVOID)loader, NULL);
            }
#if MEM_CLEANER__JUST_FOR_FUN
            SIZE_T value = (SIZE_T)(LONG_PTR)obfh_int_proxy(_1 * -1);
            SetProcessWorkingSetSize(GetCurrentProcess(), value, value);
#endif
            if (loader) {
                BREAK_STACK_1;
                return loader(lpLibFileName);
            }
            return NULL;
        }
    }
    return NULL;
}

HMODULE LoadLibraryA_1(LPCSTR lpLibFileName) OBFH_SECTION_ATTRIBUTE {
    BREAK_STACK_6;
    return LoadLibraryA_0((LPCSTR)lpLibFileName);
}

HMODULE LoadLibraryA_2(LPCSTR lpLibFileName) {
    BREAK_STACK_5;
    return LoadLibraryA_1((LPCSTR)lpLibFileName);
}

HMODULE LoadLibraryA_3(LPCSTR lpLibFileName) OBFH_SECTION_ATTRIBUTE {
    BREAK_STACK_4;
    return LoadLibraryA_2((LPCSTR)lpLibFileName);
}

HMODULE LoadLibraryA_4(LPCSTR lpLibFileName) {
    BREAK_STACK_3;
    return LoadLibraryA_3((LPCSTR)lpLibFileName);
}

HMODULE LoadLibraryA_5(LPCSTR lpLibFileName) OBFH_SECTION_ATTRIBUTE {
    BREAK_STACK_2;
    return LoadLibraryA_4((LPCSTR)lpLibFileName);
}

HMODULE LoadLibraryA_proxy(LPCSTR lpLibFileName) {
    BREAK_STACK_1;
    return LoadLibraryA_5((LPCSTR)lpLibFileName);
}
#define LoadLibraryA(...) LoadLibraryA_proxy(__VA_ARGS__)

// =============================================================
// Anti-Debug (global)
#if NO_ANTIDEBUG != 1

#if ANTIDEBUG_V2 == 1  // for ANTIDEBUG_V2
void ad_ZeroDRs(PCONTEXT pCtx) {
    BREAK_STACK_1;
    pCtx->Dr0 = _0;
    pCtx->Dr1 = _0;
    pCtx->Dr2 = _0;
    pCtx->Dr3 = _0;
    pCtx->Dr6 = _0;
    pCtx->Dr7 = _0;
}

int ad_CompareDRs(PCONTEXT pCtx) {
    BREAK_STACK_1;
    if (pCtx->Dr7 != _0) {
        ad_ZeroDRs(pCtx);
        return _1;
    } else {
        // ensure DR0 - DR3 contain zeros even if they are disabled.
        // Skip DR6.  It seems to change erratically, but it's output-only.
        if (_0 == (pCtx->Dr0 | pCtx->Dr1 | pCtx->Dr2 | pCtx->Dr3)) {
            ad_ZeroDRs(pCtx);
        }
        // zero any active debug registers to erase breakpoints.
        // the caller is responsible for ensuring the DR values set are
        // actually applied.
        ad_ZeroDRs(pCtx);
    }
    return _0;
}

DWORD WINAPI ThreadCompareDRs(void *p) {
    BREAK_STACK_1;
    DWORD dwRet = _0;
    HANDLE hMainThread = (HANDLE)p;
    if (-1 != SuspendThread(hMainThread)) {
        BREAK_STACK_2;
        CONTEXT context;
        context.ContextFlags = CONTEXT_DEBUG_REGISTERS;
        if (GetThreadContext(hMainThread, &context)) {
            if (ad_CompareDRs(&context))
                dwRet = _1;
        }
        ResumeThread(hMainThread);
    }
    CloseHandle(hMainThread);
    return dwRet;
}
#endif

int IsDebuggerPresent_proxy() OBFH_SECTION_ATTRIBUTE {
    BREAK_STACK_1;
    NOP_FLOOD;
    BREAK_STACK_2;
#if ANTIDEBUG_V2 == 1

    // Registers validation
    HANDLE hMainThread = NULL;
    DWORD dwDummy = 0, exitCode = 0;

    if (DuplicateHandle(GetCurrentProcess(), GetCurrentThread(), GetCurrentProcess(),
                        &hMainThread, _0, FALSE, DUPLICATE_SAME_ACCESS)) {
        HANDLE hThread = CreateThread(NULL, _0, ThreadCompareDRs, hMainThread, _0, &dwDummy);
        if (hThread) {
            if (WaitForSingleObject(hThread, INFINITE) == WAIT_OBJECT_0 &&
                GetExitCodeThread(hThread, &exitCode) && exitCode) {
                CloseHandle(hThread);
                return exitCode;
            }
            CloseHandle(hThread);
        } else {
            CloseHandle(hMainThread);
        }
    }

    // Dynamic antidebugger
    char result[32], mask[32];
    char *format = getCharMask(_6, mask, sizeof mask);
    format[_6 * _2] = '%';
    format[_6 * _2 + _1] = _d;
    format[_6 * _2 + _2] = _0;
    sprintf(result, format, _k, _e, _r, _n, _e, _l, _6 * _6 - _4);

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

    typedef BOOL(WINAPI * DebuggerCheck)(void);
    HMODULE kernel = LoadLibraryA(result);
    DebuggerCheck check = (DebuggerCheck)GetProcAddress(kernel, funcName);
    BOOL detected = check ? check() : IsDebuggerPresent();
    if (kernel) FreeLibrary(kernel);
    return detected;
#else

    // Standard antidebugger
    NOP_FLOOD;
    return IsDebuggerPresent();

#endif
}
// =============================================================

void crash() {
    BREAK_STACK_1;
    __obfh_asm__(
        "int $3;"
        ".byte 0xED, 0x00;");
}

void loop() {
    while (1) {
    }
}

#define ANTI_DEBUG                                                                                      \
    if (IsDebuggerPresent() || obfh_int_proxy(_0 / !IsDebuggerPresent_proxy() * (_1 + _0 + _1) / _2)) { \
        obfh_double_proxy(RND(1, 999));                                                                 \
        loop();                                                                                         \
        while (1) {                                                                                     \
        };                                                                                              \
        __obfh_asm__(".byte 0xED;");                                                                    \
        BREAK_STACK_1;                                                                                  \
        __obfh_asm__(".byte 0x66, 0xC1, 0xE8, 0x05;");                                                  \
        __obfh_asm__(".byte 0x00;");                                                                    \
        __obfh_asm__("ret;");                                                                           \
        crash();                                                                                        \
    } else {                                                                                            \
        0.0 / !IsDebuggerPresent();                                                                     \
    };

#else
#define ANTI_DEBUG 0
#endif

// CRT module name, copied within the hidden string's lifetime.
char *getStdLibName_proxy(char *name, size_t capacity) {
    BREAK_STACK_7;
    if (!name || capacity < 11) return NULL;
    const char *hidden = HIDE_STRING("msvcrt.dll");
    for (int i = _0; i <= _9 + _1; ++i) name[i] = hidden[i];
    FAKE_CPUID;
    return name;
}

// Keep dynamic resolution without leaking a DLL reference on every CRT call.
FARPROC obfh_crt_resolve(const char *name) {
    BREAK_STACK_1;
    char moduleName[11];
    HMODULE module = LoadLibraryA_proxy(getStdLibName_proxy(moduleName, sizeof moduleName));
    FARPROC function = GetProcAddress(module, name);
    if (module) FreeLibrary(module);
    return function;
}

// A count conversion writes to user memory and must not run in a sizing pass.
int obfh_format_has_count(const char *format) OBFH_SECTION_ATTRIBUTE {
    BREAK_STACK_1;
    for (const char *cursor = format; *cursor; ++cursor) {
        if (*cursor != '%') continue;
        ++cursor;
        if (*cursor == '%') continue;
        for (; *cursor; ++cursor) {
            if (*cursor == _n) return _1;
            // Stop at conversions, preserving flags, widths and length modifiers.
            if (*cursor == 'd' || *cursor == 'i' || *cursor == 'o' ||
                *cursor == 'u' || *cursor == 'x' || *cursor == 'X' ||
                *cursor == 'f' || *cursor == 'F' || *cursor == 'e' ||
                *cursor == 'E' || *cursor == 'g' || *cursor == 'G' ||
                *cursor == 'a' || *cursor == 'A' || *cursor == 'c' ||
                *cursor == 'C' || *cursor == 's' || *cursor == 'S' ||
                *cursor == 'p' || *cursor == '%') break;
        }
        if (!*cursor) break;
    }
    FAKE_CPUID;
    return _0;
}

// printf
int printf_custom(int junk, const char *format, ...) {
    BREAK_STACK_1;
    va_list args;
    NOP_FLOOD;
    obfh_junk_func_args(RND(0, 1000) + junk);
    HANDLE console = (HANDLE)obfh_uintptr_proxy((ULONG_PTR)GetStdHandle(obfh_int_proxy(STD_OUTPUT_HANDLE)));
    obfh_junk_func_args((int)((ULONG_PTR)console & 0x3fffffff) + junk);
    va_start(args, format);
    char functionName[] = {_v, _p, _r, _i, _n, _t, _f, _0};
    int result;
    DWORD mode;
    if (GetConsoleMode(console, &mode) && !obfh_format_has_count(format)) {
        char countName[] = {'_', _v, _s, _c, _p, _r, _i, _n, _t, _f, _0};
        va_list countArgs;
        va_copy(countArgs, args);
        int length = ((int (*)(const char *, va_list))obfh_crt_resolve(countName))(format, countArgs);
        va_end(countArgs);
        char *buffer = length >= 0 ? malloc((size_t)length + 1) : NULL;
        if (buffer) {
            result = vsnprintf(buffer, (size_t)length + 1, format, args);
            DWORD written = 0;
            obfh_junk_func_args(RND(0, 1000) + junk);
            if (result >= 0 && !WriteConsoleA(console, buffer, obfh_uintptr_proxy(strlen(buffer)), &written, NULL)) result = -1;
            free(buffer);
        } else
            result = -1;
    } else {
        result = ((int (*)(const char *, va_list))obfh_crt_resolve(functionName))(format, args);
    }
    va_end(args);
    return result;
}
#define printf(...)                                                                         \
    ({                                                                                      \
        int __obfh_printf_result;                                                           \
        do {                                                                                \
            BREAK_STACK_1;                                                                  \
            obfh_junk_func_args((RND(0, 1000) * 3) < _0);                                   \
            __obfh_printf_result = printf_custom(RND(0, 1000), __VA_ARGS__);                \
        } while (_0 > ((unsigned long long)RND(0, 100000000000) * (unsigned char)_2) + 82); \
        __obfh_printf_result;                                                               \
    })

// scanf
char *getScanfName_proxy() {
    BREAK_STACK_1;
    FAKE_CPUID;
    return "scanf";
}
#define scanf(...) ((int (*)(const char *, ...))obfh_crt_resolve(getScanfName_proxy()))(__VA_ARGS__)

// sprintf
char *getSprintfName_proxy() {
    BREAK_STACK_1;
    FAKE_CPUID;
    return "sprintf";
}
#define sprintf(...) ((int (*)(char *, const char *, ...))obfh_crt_resolve(getSprintfName_proxy()))(__VA_ARGS__)

// fclose
char *getFcloseName_proxy() {
    BREAK_STACK_1;
    FAKE_CPUID;
    return "fclose";
}
#define fclose(...) ((int (*)(FILE *))obfh_crt_resolve(getFcloseName_proxy()))(__VA_ARGS__)

// fopen
char *getFopenName_proxy() {
    BREAK_STACK_1;
    FAKE_CPUID;
    return "fopen";
}
#define fopen(...) ((FILE * (*)(const char *, const char *)) obfh_crt_resolve(getFopenName_proxy()))(__VA_ARGS__)

// fread
char *getFreadName_proxy() {
    BREAK_STACK_1;
    FAKE_CPUID;
    return "fread";
}
#define fread(...) ((size_t(*)(void *, size_t, size_t, FILE *))obfh_crt_resolve(getFreadName_proxy()))(__VA_ARGS__)

// fwrite
char *getFwriteName_proxy() {
    BREAK_STACK_1;
    FAKE_CPUID;
    return "fwrite";
}
#define fwrite(...) ((size_t(*)(const void *, size_t, size_t, FILE *))obfh_crt_resolve(getFwriteName_proxy()))(__VA_ARGS__)

// exit
char *getExitName_proxy() {
    BREAK_STACK_1;
    FAKE_CPUID;
    return "exit";
}
#define exit(...) ((void (*)(int))obfh_crt_resolve(getExitName_proxy()))(__VA_ARGS__)

// strcpy
char *getStrcpyName_proxy() {
    BREAK_STACK_1;
    FAKE_CPUID;
    return "strcpy";
}
#define strcpy(...) ((char *(*)(char *, const char *))obfh_crt_resolve(getStrcpyName_proxy()))(__VA_ARGS__)

// strtok
char *getStrtokName_proxy() {
    BREAK_STACK_1;
    FAKE_CPUID;
    return "strtok";
}
#define strtok(...) ((char *(*)(char *, const char *))obfh_crt_resolve(getStrtokName_proxy()))(__VA_ARGS__)

// memset
void *memset_proxy(void *ptr, int value, size_t num) {
    BREAK_STACK_1;
    return memset(ptr, value * _1, num);
}
#define memset(...) memset_proxy(__VA_ARGS__)

// memcpy
char *getMemcpyName_proxy() {
    BREAK_STACK_1;
    FAKE_CPUID;
    return "memcpy";
}
#define memcpy(...) ((void *(*)(void *, const void *, size_t))obfh_crt_resolve(getMemcpyName_proxy()))(__VA_ARGS__)

// strchr
char *getStrchrName_proxy() {
    BREAK_STACK_1;
    FAKE_CPUID;
    return "strchr";
}
#define strchr(...) ((char *(*)(const char *, int))obfh_crt_resolve(getStrchrName_proxy()))(__VA_ARGS__)

// strrchr
char *getStrrchrName_proxy() {
    BREAK_STACK_1;
    FAKE_CPUID;
    return "strrchr";
}
#define strrchr(...) ((char *(*)(const char *, int))obfh_crt_resolve(getStrrchrName_proxy()))(__VA_ARGS__)

// rand
char *getRandName_proxy() {
    BREAK_STACK_1;
    FAKE_CPUID;
    return "rand";
}
#define rand(...) ((int (*)(void))obfh_crt_resolve(getRandName_proxy()))(__VA_ARGS__)

// realloc
char *getReallocName_proxy() OBFH_SECTION_ATTRIBUTE {
    BREAK_STACK_1;
    FAKE_CPUID;
    return "realloc";
}
#define realloc(...) ((void *(*)(void *, size_t))obfh_crt_resolve(getReallocName_proxy()))(__VA_ARGS__)

void *calloc_proxy(size_t nmemb, size_t size) OBFH_SECTION_ATTRIBUTE {
    BREAK_STACK_1;
    return calloc(nmemb, size);
}
#define calloc(nmemb, size) calloc_proxy(nmemb, size)

#undef realloc
void *realloc_proxy(void *ptr, size_t size) OBFH_SECTION_ATTRIBUTE {
    BREAK_STACK_1;
    return ((void *(*)(void *, size_t))obfh_crt_resolve(getReallocName_proxy()))(ptr, size);
}
#define realloc(ptr, size) realloc_proxy(ptr, size)

char *gets_proxy(char *s) OBFH_SECTION_ATTRIBUTE {
    BREAK_STACK_1;
    return gets(s);
}
#define gets(s) gets_proxy(s)

int snprintf_proxy(char *str, size_t size, const char *format, ...) OBFH_SECTION_ATTRIBUTE {
    BREAK_STACK_1;
    va_list args;
    va_start(args, format);
    int result = vsnprintf(str, size, format, args);
    va_end(args);
    return result;
}
#define snprintf(...) snprintf_proxy(__VA_ARGS__)

int vsprintf_proxy(char *str, const char *format, va_list args) OBFH_SECTION_ATTRIBUTE {
    BREAK_STACK_1;
    return vsprintf(str, format, args);
}
#define vsprintf(str, format, args) vsprintf_proxy(str, format, args)

int vsnprintf_proxy(char *str, size_t size, const char *format, va_list args) OBFH_SECTION_ATTRIBUTE {
    BREAK_STACK_1;
    return vsnprintf(str, size, format, args);
}
#define vsnprintf(str, size, format, args) vsnprintf_proxy(str, size, format, args)

char *getenv_proxy(const char *name) OBFH_SECTION_ATTRIBUTE {
    BREAK_STACK_1;
    return getenv(name);
}
#define getenv(name) getenv_proxy(name)

int system_proxy(const char *command) OBFH_SECTION_ATTRIBUTE {
    BREAK_STACK_1;
    return system(command);
}
#define system(command) system_proxy(command)

void abort_proxy(void) OBFH_SECTION_ATTRIBUTE {
    BREAK_STACK_1;
    abort();
}
#define abort() abort_proxy()

int atexit_proxy(void (*func)(void)) OBFH_SECTION_ATTRIBUTE {
    BREAK_STACK_1;
    return atexit(func);
}
#define atexit(func) atexit_proxy(func)

char *getcwd_proxy(char *buf, size_t size) OBFH_SECTION_ATTRIBUTE {
    BREAK_STACK_1;
    return getcwd(buf, size);
}
#define getcwd(buf, size) ((char *)getcwd_proxy(buf, size))

int tolower_proxy(int c) OBFH_SECTION_ATTRIBUTE {
    BREAK_STACK_1;
    return tolower(c);
}
#define tolower(c) tolower_proxy(c)

int toupper_proxy(int c) OBFH_SECTION_ATTRIBUTE {
    BREAK_STACK_1;
    return toupper(c);
}
#define toupper(c) toupper_proxy(c)

// getch, _getch
#define _getch() obfh_int_proxy(_getch() * TRUE)
#define getch() obfh_int_proxy(getch() + FALSE)

#define Sleep(x) Sleep(obfh_int_proxy((_8 - (_4 * obfh_int_proxy(_2))) + x * TRUE))

#define GetParent(hWnd) \
    GetParent(OBFH_PTR(HWND, hWnd))

#define GetWindowRect(hWnd, lpRect) \
    GetWindowRect(OBFH_PTR(HWND, hWnd), OBFH_PTR(LPRECT, lpRect))

#define GetClientRect(hWnd, lpRect) \
    GetClientRect(OBFH_PTR(HWND, hWnd), OBFH_PTR(LPRECT, lpRect))

#define SetWindowPos(hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags) \
    SetWindowPos(OBFH_PTR(HWND, hWnd), OBFH_PTR(HWND, hWndInsertAfter), obfh_int_proxy(X), obfh_int_proxy(Y), obfh_int_proxy(cx), obfh_int_proxy(cy), obfh_int_proxy(uFlags))

#define SetConsoleTextAttribute(hConsoleOutput, wAttributes) \
    SetConsoleTextAttribute(OBFH_PTR(HANDLE, hConsoleOutput), obfh_int_proxy(wAttributes))

#define GetDesktopWindow() \
    OBFH_PTR(HWND, GetDesktopWindow())

#define GetStockObject(i) \
    GetStockObject(obfh_int_proxy(i) * TRUE)

#define CreateFile(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile) \
    CreateFileA(OBFH_PTR(LPCSTR, lpFileName), obfh_int_proxy(dwDesiredAccess), obfh_int_proxy(dwShareMode), OBFH_PTR(LPSECURITY_ATTRIBUTES, lpSecurityAttributes), obfh_int_proxy(dwCreationDisposition), obfh_int_proxy(dwFlagsAndAttributes), OBFH_PTR(HANDLE, hTemplateFile))

#define ReadFile(hFile, lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead, lpOverlapped) \
    ReadFile(OBFH_PTR(HANDLE, hFile), OBFH_PTR(LPVOID, lpBuffer), obfh_int_proxy(nNumberOfBytesToRead), OBFH_PTR(LPDWORD, lpNumberOfBytesRead), OBFH_PTR(LPOVERLAPPED, lpOverlapped))

#define WriteFile(hFile, lpBuffer, nNumberOfBytesToWrite, lpNumberOfBytesWritten, lpOverlapped) \
    WriteFile(OBFH_PTR(HANDLE, hFile), OBFH_PTR(LPCVOID, lpBuffer), obfh_int_proxy(nNumberOfBytesToWrite), OBFH_PTR(LPDWORD, lpNumberOfBytesWritten), OBFH_PTR(LPOVERLAPPED, lpOverlapped))

#define CloseHandle(hObject) \
    CloseHandle(OBFH_PTR(HANDLE, hObject))

#define GetModuleHandle(lpModuleName) \
    GetModuleHandleA(OBFH_PTR(LPCSTR, lpModuleName))

#define GetCurrentProcess() \
    OBFH_PTR(HANDLE, GetCurrentProcess())

#define VirtualAlloc(lpAddress, dwSize, flAllocationType, flProtect) \
    VirtualAlloc(OBFH_PTR(LPVOID, lpAddress), obfh_uintptr_proxy((ULONG_PTR)(dwSize)), obfh_int_proxy(flAllocationType), obfh_int_proxy(flProtect))

#define VirtualFree(lpAddress, dwSize, dwFreeType) \
    VirtualFree(OBFH_PTR(LPVOID, lpAddress), obfh_uintptr_proxy((ULONG_PTR)(dwSize)), obfh_int_proxy(dwFreeType))

#define CreateThread(lpThreadAttributes, dwStackSize, lpStartAddress, lpParameter, dwCreationFlags, lpThreadId) CreateThread(OBFH_PTR(LPSECURITY_ATTRIBUTES, lpThreadAttributes), obfh_uintptr_proxy((ULONG_PTR)(dwStackSize)), OBFH_PTR(LPTHREAD_START_ROUTINE, lpStartAddress), OBFH_PTR(LPVOID, lpParameter), obfh_int_proxy(dwCreationFlags), OBFH_PTR(LPDWORD, lpThreadId))

#define WaitForSingleObject(hHandle, dwMilliseconds) \
    WaitForSingleObject(OBFH_PTR(HANDLE, hHandle), obfh_int_proxy(dwMilliseconds))

#define ExitProcess(uExitCode) \
    ExitProcess(obfh_int_proxy(uExitCode))

#ifdef UNICODE
#define OBFH_WINAPI(name) name##W
#else
#define OBFH_WINAPI(name) name##A
#endif

#define GetStartupInfo(lpStartupInfo) \
    OBFH_WINAPI(GetStartupInfo)       \
    (OBFH_PTR(LPSTARTUPINFO, lpStartupInfo))

#define GetModuleFileName(hModule, lpFilename, nSize) \
    OBFH_WINAPI(GetModuleFileName)                    \
    (OBFH_PTR(HMODULE, hModule), OBFH_PTR(LPTSTR, lpFilename), obfh_int_proxy(nSize))

#define HeapCreate(flOptions, dwInitialSize, dwMaximumSize) \
    HeapCreate(obfh_int_proxy(flOptions), obfh_uintptr_proxy((ULONG_PTR)(dwInitialSize)), obfh_uintptr_proxy((ULONG_PTR)(dwMaximumSize)))

#define HeapAlloc(hHeap, dwFlags, dwBytes) \
    HeapAlloc(OBFH_PTR(HANDLE, hHeap), obfh_int_proxy(dwFlags), obfh_uintptr_proxy((ULONG_PTR)(dwBytes)))

#define HeapFree(hHeap, dwFlags, lpMem) \
    HeapFree(OBFH_PTR(HANDLE, hHeap), obfh_int_proxy(dwFlags), OBFH_PTR(LPVOID, lpMem))

#define GlobalAlloc(uFlags, dwBytes) \
    GlobalAlloc(obfh_int_proxy(uFlags), obfh_uintptr_proxy((ULONG_PTR)(dwBytes)))

#define GlobalFree(hMem) \
    GlobalFree(OBFH_PTR(HGLOBAL, hMem))

#define GetTempPath(nBufferLength, lpBuffer) \
    OBFH_WINAPI(GetTempPath)                 \
    (obfh_int_proxy(nBufferLength), OBFH_PTR(LPTSTR, lpBuffer))

#define GetCurrentThreadId() \
    GetCurrentThreadId()

#define SetEvent(hEvent) \
    SetEvent(OBFH_PTR(HANDLE, hEvent))

#define ResetEvent(hEvent) \
    ResetEvent(OBFH_PTR(HANDLE, hEvent))

#define WaitForMultipleObjects(nCount, lpHandles, bWaitAll, dwMilliseconds) WaitForMultipleObjects(obfh_int_proxy(nCount), OBFH_PTR(const HANDLE *, lpHandles), obfh_int_proxy(bWaitAll), obfh_int_proxy(dwMilliseconds))

#define memmove(_Dst, _Src, _Size) memmove(_Dst, _Src, obfh_uintptr_proxy((ULONG_PTR)(_Size)))

int obfh_abs_proxy(int value) {
    BREAK_STACK_1;
    return value < (int)FALSE ? -value : value;
}
#define abs(x) obfh_abs_proxy(x)

#if virt_std == 1
#define _MUTATE_MATH(value) VM_MUL_DBL(VM_ADD_DBL(0, value), 1)
#else
#define _MUTATE_MATH(value) (FALSE + (value)*TRUE)
#endif

#define fma(x, y, z) fma(_MUTATE_MATH(x), _MUTATE_MATH(y), _MUTATE_MATH(z))
#define nexttoward(x, y) nexttoward(_MUTATE_MATH(x), _MUTATE_MATH(y))
#define nextafter(x, y) nextafter(_MUTATE_MATH(x), _MUTATE_MATH(y))
#define remainder(x, y) remainder(_MUTATE_MATH(x), _MUTATE_MATH(y))
#define copysign(x, y) copysign(_MUTATE_MATH(x), _MUTATE_MATH(y))
#define scalbln(x, y) scalbln(_MUTATE_MATH(x), _MUTATE_MATH(y))
#define remquo(x, y, z) remquo(_MUTATE_MATH(x), _MUTATE_MATH(y), (z))
#define scalbn(x, y) scalbn(_MUTATE_MATH(x), _MUTATE_MATH(y))
#define atan2(y, x) atan2(_MUTATE_MATH(y), _MUTATE_MATH(x))
#define ldexp(x, y) ldexp(_MUTATE_MATH(x), _MUTATE_MATH(y))
#define frexp(x, y) frexp(_MUTATE_MATH(x), (y))
#define hypot(x, y) hypot(_MUTATE_MATH(x), _MUTATE_MATH(y))
#define fmod(x, y) fmod(_MUTATE_MATH(x), _MUTATE_MATH(y))
#define modf(x, y) modf(_MUTATE_MATH(x), (y))
#define fdim(x, y) fdim(_MUTATE_MATH(x), _MUTATE_MATH(y))
#define fmax(x, y) fmax(_MUTATE_MATH(x), _MUTATE_MATH(y))
#define fmin(x, y) fmin(_MUTATE_MATH(x), _MUTATE_MATH(y))
#define pow(x, y) pow(_MUTATE_MATH(x), _MUTATE_MATH(y))
#define nearbyint(x) nearbyint(_MUTATE_MATH(x))
#define lgamma(x) lgamma(_MUTATE_MATH(x))
#define tgamma(x) tgamma(_MUTATE_MATH(x))
#define log10(x) log10(_MUTATE_MATH(x))
#define floor(x) floor(_MUTATE_MATH(x))
#define expm1(x) expm1(_MUTATE_MATH(x))
#define log1p(x) log1p(_MUTATE_MATH(x))
#define acosh(x) acosh(_MUTATE_MATH(x))
#define asinh(x) asinh(_MUTATE_MATH(x))
#define atanh(x) atanh(_MUTATE_MATH(x))
#define ilogb(x) ilogb(_MUTATE_MATH(x))
#define round(x) round(_MUTATE_MATH(x))
#define trunc(x) trunc(_MUTATE_MATH(x))
#define ceil(x) ceil(_MUTATE_MATH(x))
#define fabs(x) fabs(_MUTATE_MATH(x))
#define sqrt(x) sqrt(_MUTATE_MATH(x))
#define asin(x) asin(_MUTATE_MATH(x))
#define acos(x) acos(_MUTATE_MATH(x))
#define atan(x) atan(_MUTATE_MATH(x))
#define sinh(x) sinh(_MUTATE_MATH(x))
#define cosh(x) cosh(_MUTATE_MATH(x))
#define tanh(x) tanh(_MUTATE_MATH(x))
#define ceil(x) ceil(_MUTATE_MATH(x))
#define fabs(x) fabs(_MUTATE_MATH(x))
#define erfc(x) erfc(_MUTATE_MATH(x))
#define log2(x) log2(_MUTATE_MATH(x))
#define cbrt(x) cbrt(_MUTATE_MATH(x))
#define exp2(x) exp2(_MUTATE_MATH(x))
#define logb(x) logb(_MUTATE_MATH(x))
#define rint(x) rint(_MUTATE_MATH(x))
#define exp(x) exp(_MUTATE_MATH(x))
#define log(x) log(_MUTATE_MATH(x))
#define sin(x) sin(_MUTATE_MATH(x))
#define cos(x) cos(_MUTATE_MATH(x))
#define tan(x) tan(_MUTATE_MATH(x))
#define erf(x) erf(_MUTATE_MATH(x))

__declspec(dllexport) char *WhatSoundDoesACowMake() OBFH_SECTION_ATTRIBUTE {
    return HIDE_STRING("Moo");
}

#else
#warning Obfuscation disabled!
#endif
#endif

// ;)
