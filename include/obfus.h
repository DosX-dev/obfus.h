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

/*
 * Layout follows preprocessing dependencies, not alphabetical order.
 * 01-09: configuration, entropy, native decoys and shared value helpers.
 * 10-12: CFLOW/VM internals and inline kernels using native control flow.
 * 13:    keyword interception boundary; later function bodies use protection.
 * 14-23: public VM interface, platform/CRT adapters and math aliases.
 *
 * Keep helpers that need native if/for/while above section 13. Macro definitions
 * expand at their use sites: moving a function across that boundary changes it.
 * Keep each proxy and its public alias together. ASM operand order, local labels
 * and captured random draws are contracts, even in skipped code.
 */

// ============================================================================
// 01. Compiler setup, editor interface and public fallbacks
// ============================================================================

#if !__TINYC__ && !__GNUC__ && !__MINGW32__
#define __attribute__(...)
#endif

// Editor detection never substitutes a real TCC build.
#if defined(__INTELLISENSE__) && !defined(__TINYC__)
#define OBFH_EDITOR_VIEW 1
#else
#define OBFH_EDITOR_VIEW 0
#endif

// Shared entropy settings for compilation and editor expressions.
#if !NO_OBF || OBFH_EDITOR_VIEW
#ifndef OBFH_BUILD_SEED
#define OBFH_BUILD_SEED 0u
#endif
#define RND(min, max) \
    ((min) + ((__COUNTER__ + __LINE__ * __LINE__ + (unsigned int)OBFH_BUILD_SEED) * 2654435761u % ((max) - (min) + 1)))
#endif

// Platform declarations used by the implementation and editor aliases.
#if !NO_OBF || OBFH_EDITOR_VIEW
#include <conio.h>
#include <ctype.h>
#include <direct.h>
#include <errno.h>
#include <limits.h>
#include <math.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <windows.h>
#endif

// Lightweight editor interface; runtime protection is defined below.
#if OBFH_EDITOR_VIEW

// Value transport and section annotations.
#define STACK_STRING(str) ((char[]){str})
#define RET_BY_VAR(value) \
    { return (value); }
#define SECTION_ATTRIBUTE(name)
#define DATA_SECTION_ATTRIBUTE
#define TEXT_SECTION_ATTRIBUTE
#define BAD_JMP ((void)0)
#define BAD_CALL ((void)0)
#define OBFUS_CONDITION_BLOCK(...) (__VA_ARGS__)

// Memory operations.
#define memchr_custom(...) memchr(__VA_ARGS__)
#define memcmp_custom(...) memcmp(__VA_ARGS__)
#define memcpy_custom(...) memcpy(__VA_ARGS__)
#define memmove_custom(...) memmove(__VA_ARGS__)
#define memset_custom(...) memset(__VA_ARGS__)
#define wmemchr_custom(...) wmemchr(__VA_ARGS__)
#define wmemcmp_custom(...) wmemcmp(__VA_ARGS__)
#define wmemcpy_custom(...) wmemcpy(__VA_ARGS__)
#define wmemmove_custom(...) wmemmove(__VA_ARGS__)
#define wmemset_custom(...) wmemset(__VA_ARGS__)

// Narrow strings.
#define strlen_custom(...) strlen(__VA_ARGS__)
#define strnlen_custom(...) strnlen(__VA_ARGS__)
#define strcmp_custom(...) strcmp(__VA_ARGS__)
#define strncmp_custom(...) strncmp(__VA_ARGS__)
#define strcpy_custom(...) strcpy(__VA_ARGS__)
#define strncpy_custom(...) strncpy(__VA_ARGS__)
#define strcat_custom(...) strcat(__VA_ARGS__)
#define strncat_custom(...) strncat(__VA_ARGS__)
#define strchr_custom(...) strchr(__VA_ARGS__)
#define strrchr_custom(...) strrchr(__VA_ARGS__)
#define strstr_custom(...) strstr(__VA_ARGS__)
#define strspn_custom(...) strspn(__VA_ARGS__)
#define strcspn_custom(...) strcspn(__VA_ARGS__)
#define strpbrk_custom(...) strpbrk(__VA_ARGS__)

// Wide strings.
#define wcslen_custom(...) wcslen(__VA_ARGS__)
#define wcsnlen_custom(...) wcsnlen(__VA_ARGS__)
#define wcscmp_custom(...) wcscmp(__VA_ARGS__)
#define wcsncmp_custom(...) wcsncmp(__VA_ARGS__)
#define wcscpy_custom(...) wcscpy(__VA_ARGS__)
#define wcsncpy_custom(...) wcsncpy(__VA_ARGS__)
#define wcscat_custom(...) wcscat(__VA_ARGS__)
#define wcsncat_custom(...) wcsncat(__VA_ARGS__)
#define wcschr_custom(...) wcschr(__VA_ARGS__)
#define wcsrchr_custom(...) wcsrchr(__VA_ARGS__)
#define wcsstr_custom(...) wcsstr(__VA_ARGS__)
#define wcsspn_custom(...) wcsspn(__VA_ARGS__)
#define wcscspn_custom(...) wcscspn(__VA_ARGS__)
#define wcspbrk_custom(...) wcspbrk(__VA_ARGS__)

// Search, output and platform aliases.
#define bsearch_custom(...) bsearch(__VA_ARGS__)
#define printf_custom(...) printf(__VA_ARGS__)
#define GetProcAddress_custom(...) GetProcAddress(__VA_ARGS__)

#endif  // OBFH_EDITOR_VIEW

// VM editor expressions preserve protected result types and single evaluation.
// These expressions describe the interface; they are not runtime VM handlers.
#if OBFH_EDITOR_VIEW && NO_OBF != 1 && VIRT == 1
#define VM_ADD(a, b) ((long)((long double)(a) + (long double)(b)))
#define VM_SUB(a, b) ((long)((long double)(a) - (long double)(b)))
#define VM_MUL(a, b) ((long)((long double)(a) * (long double)(b)))
#define VM_DIV(a, b) ((long)((long double)(a) / (long double)(b)))
#define VM_EQU(a, b) ((long)((long double)(a) == (long double)(b)))
#define VM_NEQ(a, b) ((long)((long double)(a) != (long double)(b)))
#define VM_LSS(a, b) ((long)((long double)(a) < (long double)(b)))
#define VM_GTR(a, b) ((long)((long double)(a) > (long double)(b)))
#define VM_LEQ(a, b) ((long)((long double)(a) <= (long double)(b)))
#define VM_GEQ(a, b) ((long)((long double)(a) >= (long double)(b)))
#define VM_MOD(a, b) ((long)((int)(a) % (int)(b)))
#define VM_ADD_DBL(a, b) ((long double)((double)(a) + (double)(b)))
#define VM_SUB_DBL(a, b) ((long double)((double)(a) - (double)(b)))
#define VM_MUL_DBL(a, b) ((long double)((double)(a) * (double)(b)))
#define VM_DIV_DBL(a, b) ((long double)((double)(a) / (double)(b)))
#define VM_LSS_DBL(a, b) ((long)((double)(a) < (double)(b)))
#define VM_GTR_DBL(a, b) ((long)((double)(a) > (double)(b)))
#define VM_OBF_INT(value) ((long)(value))
#define VM_OBF_DBL(value) ((long double)(value))
#define VM_IF(condition) if (condition)
#define VM_ELSE_IF(condition) else if (condition)
#define VM_ELSE else
// Native expressions when virtualization is disabled.
#elif NO_OBF == 1 || VIRT != 1 || OBFH_EDITOR_VIEW
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

// Disabled protection and editor-only placeholders.
#if NO_OBF == 1 || OBFH_EDITOR_VIEW
#define HIDE_STRING(str) str
#define BREAK_STACK_CFLOW ((void)0)
#define STACK_PROXY_FUNCTIONS ((void)0)
#define PHANTOM_NOP ((void)0)
#define ANTI_DEBUG 0
#endif

#if !NO_OBF && !OBFH_EDITOR_VIEW

// ============================================================================
// 02. Platform compatibility and section placement
// ============================================================================

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
#define TEXT_SECTION_ATTRIBUTE SECTION_ATTRIBUTE(".text")  // Text section

// Optional packer/protector signature data.
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

// TCC encodes custom-section function RVAs relative to .text. Keep code in
// .text when publishing unwind-backed decoys; protected data stays separate.
#if defined(__TINYC__) && defined(__x86_64__) && defined(_WIN32) && !NO_PDATA_DECOYS
#define OBFH_CODE_SECTION_ATTRIBUTE TEXT_SECTION_ATTRIBUTE
#define OBFH_DATA_CODE_SECTION_ATTRIBUTE TEXT_SECTION_ATTRIBUTE
#else
#define OBFH_CODE_SECTION_ATTRIBUTE OBFH_SECTION_ATTRIBUTE
#define OBFH_DATA_CODE_SECTION_ATTRIBUTE DATA_SECTION_ATTRIBUTE
#endif

// ============================================================================
// 03. Compile-time entropy, strings and typed address transport
// ============================================================================

#define STACK_STRING(str) ((char[]){str})

#define HIDE_STRING(str) \
    (OBFH_HIDE_JUNK(), (_0 < RND(1, 255) ? obfh_process_hidden_string(STACK_STRING("\0" str "\0"), (float)__s_rdtsc(RND(0, 255)) != 0.1) : (char *)(ULONG_PTR)((float)__s_rdtsc(RND(0, 255)) == RND(0, 255))))

typedef enum {
    SALT_SHIFT = RND(0xBAD, 0xBEEF)
} VAR_ADDR_SHIFT;

// Mutate the address as a pointer-width integer, preserving the value type.
#define RET_BY_VAR(value)                                                                                                                                                                                                                                    \
    {                                                                                                                                                                                                                                                        \
        enum { __obfh_ret_mode = RND(0, 2) };                                                                                                                                                                                                                \
        volatile ULONG_PTR __obfh_ret_shift = (ULONG_PTR)OBFH_JUNK_WORD;                                                                                                                                                                                     \
        __obfh_ret_shift ^= (ULONG_PTR)OBFH_JUNK_WORD << (sizeof(ULONG_PTR) == 8 ? 32 : 0);                                                                                                                                                                  \
        ULONG_PTR __obfh_ret_address = __builtin_choose_expr(__obfh_ret_mode == 0, (ULONG_PTR) & (value) ^ __obfh_ret_shift, __builtin_choose_expr(__obfh_ret_mode == 1, (ULONG_PTR) & (value) + __obfh_ret_shift, (ULONG_PTR) & (value)-__obfh_ret_shift)); \
        return *(__typeof__(&(value)))__builtin_choose_expr(__obfh_ret_mode == 0, __obfh_ret_address ^ __obfh_ret_shift, __builtin_choose_expr(__obfh_ret_mode == 1, __obfh_ret_address - __obfh_ret_shift, __obfh_ret_address + __obfh_ret_shift));         \
    }

// Mix separate compile-time draws so the payload is not an affine byte pattern.
#define OBFH_JUNK_BYTE (((RND(0, 65535) * 2246822519u) ^ ((RND(0, 65535) * 3266489917u) >> 13)) & 255u)
#define OBFH_JUNK_WORD ((RND(0, 65535) * 2246822519u) ^ ((unsigned int)RND(0, 65535) << 16) ^ (RND(0, 65535) * 3266489917u))
#define OBFH_MIX_A(value) (((unsigned int)(value) ^ ((unsigned int)(value) >> 16)) * 2246822507u)
#define OBFH_MIX_B(value) (((unsigned int)(value) ^ ((unsigned int)(value) >> 13)) * 3266489909u)

// Select stores at compile time; each use has its own captured parameters.
#define OBFH_CRT_NAME_ORDER(forward, reverse) ({                  \
    enum { __obfh_name_order = OBFH_MIX_B(OBFH_JUNK_WORD) & 1u }; \
    __builtin_choose_expr(__obfh_name_order, forward, reverse);   \
})

#define OBFH_DATA_DRAW(salt) OBFH_MIX_B(OBFH_MIX_A((unsigned int)__LINE__ ^ (unsigned int)OBFH_BUILD_SEED ^ ((unsigned int)(salt)*2654435761u)))

// ============================================================================
// 04. Retained data and optional instruction padding
// ============================================================================

// Interleave retained random data; the original volatile objects keep their types.
#define OBFH_DATA_JUNK(name, section)                                                                                       \
    static volatile unsigned char __obfh_data_##name[8u + (OBFH_DATA_DRAW(0u) & 15u)] section __attribute__((used)) = {     \
        (OBFH_DATA_DRAW(1u) & 255u), (OBFH_DATA_DRAW(2u) & 255u), (OBFH_DATA_DRAW(3u) & 255u), (OBFH_DATA_DRAW(4u) & 255u), \
        (OBFH_DATA_DRAW(5u) & 255u), (OBFH_DATA_DRAW(6u) & 255u), (OBFH_DATA_DRAW(7u) & 255u), (OBFH_DATA_DRAW(8u) & 255u)}
#define OBFH_STRING_CONST(name, value)                          \
    static volatile char name[] OBFH_SECTION_ATTRIBUTE = value; \
    OBFH_DATA_JUNK(name, OBFH_SECTION_ATTRIBUTE)
#define OBFH_CHAR_CONST(name, value, section)  \
    static volatile char name section = value; \
    OBFH_DATA_JUNK(name, section)

OBFH_STRING_CONST(_s_a, "a");
OBFH_STRING_CONST(_s_b, "b");
OBFH_STRING_CONST(_s_c, "c");
OBFH_STRING_CONST(_s_d, "d");
OBFH_STRING_CONST(_s_e, "e");
OBFH_STRING_CONST(_s_f, "f");
OBFH_STRING_CONST(_s_g, "g");
OBFH_STRING_CONST(_s_h, "h");
OBFH_STRING_CONST(_s_i, "i");
OBFH_STRING_CONST(_s_j, "j");
OBFH_STRING_CONST(_s_k, "k");
OBFH_STRING_CONST(_s_l, "l");
OBFH_STRING_CONST(_s_m, "m");
OBFH_STRING_CONST(_s_n, "n");
OBFH_STRING_CONST(_s_o, "o");
OBFH_STRING_CONST(_s_p, "p");
OBFH_STRING_CONST(_s_q, "q");
OBFH_STRING_CONST(_s_r, "r");
OBFH_STRING_CONST(_s_s, "s");
OBFH_STRING_CONST(_s_t, "t");
OBFH_STRING_CONST(_s_u, "u");
OBFH_STRING_CONST(_s_v, "v");
OBFH_STRING_CONST(_s_w, "w");
OBFH_STRING_CONST(_s_x, "x");
OBFH_STRING_CONST(_s_y, "y");
OBFH_STRING_CONST(_s_z, "z");
OBFH_CHAR_CONST(_a, 'a', TEXT_SECTION_ATTRIBUTE);
OBFH_CHAR_CONST(_b, 'b', DATA_SECTION_ATTRIBUTE);
OBFH_CHAR_CONST(_c, 'c', OBFH_SECTION_ATTRIBUTE);
OBFH_CHAR_CONST(_d, 'd', TEXT_SECTION_ATTRIBUTE);
OBFH_CHAR_CONST(_e, 'e', DATA_SECTION_ATTRIBUTE);
OBFH_CHAR_CONST(_f, 'f', OBFH_SECTION_ATTRIBUTE);
OBFH_CHAR_CONST(_g, 'g', TEXT_SECTION_ATTRIBUTE);
OBFH_CHAR_CONST(_h, 'h', DATA_SECTION_ATTRIBUTE);
OBFH_CHAR_CONST(_i, 'i', OBFH_SECTION_ATTRIBUTE);
OBFH_CHAR_CONST(_j, 'j', TEXT_SECTION_ATTRIBUTE);
OBFH_CHAR_CONST(_k, 'k', DATA_SECTION_ATTRIBUTE);
OBFH_CHAR_CONST(_l, 'l', OBFH_SECTION_ATTRIBUTE);
OBFH_CHAR_CONST(_m, 'm', TEXT_SECTION_ATTRIBUTE);
OBFH_CHAR_CONST(_n, 'n', DATA_SECTION_ATTRIBUTE);
OBFH_CHAR_CONST(_o, 'o', OBFH_SECTION_ATTRIBUTE);
OBFH_CHAR_CONST(_p, 'p', TEXT_SECTION_ATTRIBUTE);
OBFH_CHAR_CONST(_q, 'q', DATA_SECTION_ATTRIBUTE);
OBFH_CHAR_CONST(_r, 'r', OBFH_SECTION_ATTRIBUTE);
OBFH_CHAR_CONST(_s, 's', TEXT_SECTION_ATTRIBUTE);
OBFH_CHAR_CONST(_t, 't', DATA_SECTION_ATTRIBUTE);
OBFH_CHAR_CONST(_u, 'u', OBFH_SECTION_ATTRIBUTE);
OBFH_CHAR_CONST(_v, 'v', TEXT_SECTION_ATTRIBUTE);
OBFH_CHAR_CONST(_w, 'w', DATA_SECTION_ATTRIBUTE);
OBFH_CHAR_CONST(_x, 'x', OBFH_SECTION_ATTRIBUTE);
OBFH_CHAR_CONST(_y, 'y', TEXT_SECTION_ATTRIBUTE);
OBFH_CHAR_CONST(_z, 'z', DATA_SECTION_ATTRIBUTE);
OBFH_CHAR_CONST(_S, 'S', OBFH_SECTION_ATTRIBUTE);
OBFH_CHAR_CONST(_L, 'L', TEXT_SECTION_ATTRIBUTE);
OBFH_CHAR_CONST(_A, 'A', DATA_SECTION_ATTRIBUTE);
OBFH_CHAR_CONST(_I, 'I', OBFH_SECTION_ATTRIBUTE);
OBFH_CHAR_CONST(_D, 'D', TEXT_SECTION_ATTRIBUTE);
OBFH_CHAR_CONST(_P, 'P', OBFH_SECTION_ATTRIBUTE);
OBFH_CHAR_CONST(_0, 0, TEXT_SECTION_ATTRIBUTE);
OBFH_CHAR_CONST(_1, 1, OBFH_SECTION_ATTRIBUTE);
OBFH_CHAR_CONST(_2, 2, DATA_SECTION_ATTRIBUTE);
OBFH_CHAR_CONST(_3, 3, TEXT_SECTION_ATTRIBUTE);
OBFH_CHAR_CONST(_4, 4, OBFH_SECTION_ATTRIBUTE);
OBFH_CHAR_CONST(_5, 5, DATA_SECTION_ATTRIBUTE);
OBFH_CHAR_CONST(_6, 6, TEXT_SECTION_ATTRIBUTE);
OBFH_CHAR_CONST(_7, 7, OBFH_SECTION_ATTRIBUTE);
OBFH_CHAR_CONST(_8, 8, DATA_SECTION_ATTRIBUTE);
OBFH_CHAR_CONST(_9, 9, TEXT_SECTION_ATTRIBUTE);

#define __obfh_asm__(...) __asm__ __volatile(__VA_ARGS__)

// Compile-time byte variation only: no runtime branch, register or flag changes.
#define OBFH_PHANTOM_DRAW(site) \
    OBFH_MIX_B(OBFH_MIX_A((unsigned int)(site) ^ (unsigned int)OBFH_BUILD_SEED ^ (unsigned int)__LINE__ * 2654435761u))
#define PHANTOM_NOP                                                       \
    ({                                                                    \
        enum { __obfh_phantom_site = __LINE__ };                          \
        __obfh_asm__(".fill %c0, 1, 0x90;"                                \
                     :                                                    \
                     : "i"(OBFH_PHANTOM_DRAW(__obfh_phantom_site) & 1u)); \
    })

// ============================================================================
// 05. Native decoys with PE unwind metadata
// ============================================================================

// Static PE unwind-backed decoys. TCC alone supplies the RVA relocations.
// Its function-table range begins after the carrier's 11-byte prologue;
// an independent native entry there matches its PUSH_RBP / SET_FPREG info.
#if defined(__TINYC__) && defined(__x86_64__) && defined(_WIN32) && !NO_PDATA_DECOYS
#define OBFH_PD_MIX_A(value) ((((value) ^ ((value) >> 16)) * 2246822519u) & 0xffffffffu)
#define OBFH_PD_MIX_B(value) ((((value) ^ ((value) >> 13)) * 3266489917u) & 0xffffffffu)
#define OBFH_PD_DRAW(site, salt) OBFH_PD_MIX_B(OBFH_PD_MIX_A(((OBFH_BUILD_SEED & 0xffffffffu) ^ (((site) + 1u) * 2654435761u & 0xffffffffu) ^ ((salt)*2246822519u & 0xffffffffu))))
#ifndef OBFH_PDATA_DECOY_COUNT
#define OBFH_PDATA_DECOY_COUNT (86u + (OBFH_PD_DRAW(0u, 97u) % 43u))
#endif
#if OBFH_PDATA_DECOY_COUNT < 86 || OBFH_PDATA_DECOY_COUNT > 128
#error OBFH_PDATA_DECOY_COUNT must be between 86 and 128
#endif
#define OBFH_PD_BODY_0 "xorl %[key], %%eax; imull %[mul], %%eax; addl %[key2], %%eax;"
#define OBFH_PD_BODY_1 "roll %[rotate], %%eax; xorl %[key], %%eax; addl %[key2], %%eax;"
#define OBFH_PD_BODY_2 "addl %[key], %%eax; movl %%eax, %%edx; shrl %[rotate], %%edx; xorl %%edx, %%eax;"
#define OBFH_PD_BODY_3 "bswap %%eax; xorl %[key], %%eax; addl %[key2], %%eax;"
#define OBFH_PD_BODY_4 "notl %%eax; addl %[key], %%eax; xorl %[key2], %%eax;"
#define OBFH_PD_BODY_5 "movl %%eax, %%edx; andl %[key], %%edx; orl %[key2], %%eax; xorl %%edx, %%eax;"
#define OBFH_PD_BODY_6 "movl %%eax, %%edx; shrl $13, %%edx; xorl %%edx, %%eax; imull %[mul], %%eax;"
#define OBFH_PD_BODY_7 "rorl %[rotate], %%eax; addl %[key], %%eax; bswap %%eax;"
#define OBFH_PD_BODY_8 "leal (%%eax,%%eax,2), %%edx; xorl %[key], %%edx; addl %%edx, %%eax;"
#define OBFH_PD_BODY_9 "addl %[key], %%eax; imull %%eax, %%eax; xorl %[key2], %%eax;"
#define OBFH_PD_BODY_10 "subl %[key], %%eax; roll %[rotate], %%eax; xorl %[key2], %%eax;"
#define OBFH_PD_BODY_11 "xorl %[key], %%eax; addl -%c[slot](%%rbp), %%eax; imull %[mul], %%eax;"
#define OBFH_PD_BODY_12 "testl %[key], %%eax; jz 1f; xorl %[key2], %%eax; jmp 2f; 1: addl %[mul], %%eax; 2:"
#define OBFH_PD_BODY_13 "movl %[loops], %%edx; 1: roll %[rotate], %%eax; xorl %[key], %%eax; decl %%edx; jnz 1b;"
#define OBFH_PD_BODY_14 "movl %%eax, %%edx; shll $5, %%eax; shrl $3, %%edx; xorl %%edx, %%eax; addl %[key], %%eax;"
#define OBFH_PD_BODY_15 "movl %%eax, %%edx; negl %%edx; andl %%edx, %%eax; xorl %[key], %%eax; addl %[key2], %%eax;"
#define OBFH_PD_ASM(body)                                                                       \
    __obfh_asm__(                                                                               \
        "pushq %%rbp; movq %%rsp, %%rbp; .byte 0x48, 0x81, 0xec; .long %c[frame]; "             \
        "movl %%ecx, %%eax; movl %%eax, -%c[slot](%%rbp); " body                                \
        ".fill %c[phantom], 1, 0x90; .byte 0x48, 0x81, 0xc4; .long %c[frame]; popq %%rbp; ret;" \
        :                                                                                       \
        : [frame] "i"(__obfh_pd_frame), [slot] "i"(__obfh_pd_slot),                             \
          [key] "i"(__obfh_pd_key), [key2] "i"(__obfh_pd_key2), [mul] "i"(__obfh_pd_mul),       \
          [rotate] "i"(__obfh_pd_rotate), [loops] "i"(__obfh_pd_loops),                         \
          [phantom] "i"(OBFH_PHANTOM_DRAW(__obfh_pd_key) & 1u)                                  \
        : "rax", "rcx", "rdx", "cc", "memory")
#define OBFH_PD_DEFINE(site)                                                                                                                                                                     \
    static void __obfh_pdata_decoy_##site(void) __attribute__((noinline, used));                                                                                                                 \
    static void __obfh_pdata_decoy_##site(void) {                                                                                                                                                \
        enum {                                                                                                                                                                                   \
            __obfh_pd_kind = OBFH_PD_DRAW(site, 1u) & 15u,                                                                                                                                       \
            __obfh_pd_frame = 48u + 16u * (OBFH_PD_DRAW(site, 2u) % 14u),                                                                                                                        \
            __obfh_pd_slot = 8u + 8u * (OBFH_PD_DRAW(site, 3u) % 5u),                                                                                                                            \
            __obfh_pd_key = OBFH_PD_DRAW(site, 4u),                                                                                                                                              \
            __obfh_pd_key2 = OBFH_PD_DRAW(site, 5u),                                                                                                                                             \
            __obfh_pd_mul = OBFH_PD_DRAW(site, 6u) | 1u,                                                                                                                                         \
            __obfh_pd_rotate = 1u + (OBFH_PD_DRAW(site, 7u) % 31u),                                                                                                                              \
            __obfh_pd_loops = 2u + (OBFH_PD_DRAW(site, 8u) % 6u)                                                                                                                                 \
        };                                                                                                                                                                                       \
        __builtin_choose_expr(__obfh_pd_kind < 8u,                                                                                                                                               \
                              __builtin_choose_expr(__obfh_pd_kind < 4u,                                                                                                                         \
                                                    __builtin_choose_expr(__obfh_pd_kind < 2u,                                                                                                   \
                                                                          __builtin_choose_expr(__obfh_pd_kind < 1u, ({ OBFH_PD_ASM(OBFH_PD_BODY_0); }), ({ OBFH_PD_ASM(OBFH_PD_BODY_1); })),    \
                                                                          __builtin_choose_expr(__obfh_pd_kind < 3u, ({ OBFH_PD_ASM(OBFH_PD_BODY_2); }), ({ OBFH_PD_ASM(OBFH_PD_BODY_3); }))),   \
                                                    __builtin_choose_expr(__obfh_pd_kind < 6u,                                                                                                   \
                                                                          __builtin_choose_expr(__obfh_pd_kind < 5u, ({ OBFH_PD_ASM(OBFH_PD_BODY_4); }), ({ OBFH_PD_ASM(OBFH_PD_BODY_5); })),    \
                                                                          __builtin_choose_expr(__obfh_pd_kind < 7u, ({ OBFH_PD_ASM(OBFH_PD_BODY_6); }), ({ OBFH_PD_ASM(OBFH_PD_BODY_7); })))),  \
                              __builtin_choose_expr(__obfh_pd_kind < 12u,                                                                                                                        \
                                                    __builtin_choose_expr(__obfh_pd_kind < 10u,                                                                                                  \
                                                                          __builtin_choose_expr(__obfh_pd_kind < 9u, ({ OBFH_PD_ASM(OBFH_PD_BODY_8); }), ({ OBFH_PD_ASM(OBFH_PD_BODY_9); })),    \
                                                                          __builtin_choose_expr(                                                                                                 \
                                                                              __obfh_pd_kind < 11u, ({ OBFH_PD_ASM(OBFH_PD_BODY_10); }), ({ OBFH_PD_ASM(OBFH_PD_BODY_11); }))),                  \
                                                    __builtin_choose_expr(__obfh_pd_kind < 14u,                                                                                                  \
                                                                          __builtin_choose_expr(__obfh_pd_kind < 13u, ({ OBFH_PD_ASM(OBFH_PD_BODY_12); }), ({ OBFH_PD_ASM(OBFH_PD_BODY_13); })), \
                                                                          __builtin_choose_expr(                                                                                                 \
                                                                              __obfh_pd_kind < 15u, ({ OBFH_PD_ASM(OBFH_PD_BODY_14); }), ({ OBFH_PD_ASM(OBFH_PD_BODY_15); })))));                \
    }
OBFH_PD_DEFINE(0);
OBFH_PD_DEFINE(1);
OBFH_PD_DEFINE(2);
OBFH_PD_DEFINE(3);
OBFH_PD_DEFINE(4);
OBFH_PD_DEFINE(5);
OBFH_PD_DEFINE(6);
OBFH_PD_DEFINE(7);
OBFH_PD_DEFINE(8);
OBFH_PD_DEFINE(9);
OBFH_PD_DEFINE(10);
OBFH_PD_DEFINE(11);
OBFH_PD_DEFINE(12);
OBFH_PD_DEFINE(13);
OBFH_PD_DEFINE(14);
OBFH_PD_DEFINE(15);
OBFH_PD_DEFINE(16);
OBFH_PD_DEFINE(17);
OBFH_PD_DEFINE(18);
OBFH_PD_DEFINE(19);
OBFH_PD_DEFINE(20);
OBFH_PD_DEFINE(21);
OBFH_PD_DEFINE(22);
OBFH_PD_DEFINE(23);
OBFH_PD_DEFINE(24);
OBFH_PD_DEFINE(25);
OBFH_PD_DEFINE(26);
OBFH_PD_DEFINE(27);
OBFH_PD_DEFINE(28);
OBFH_PD_DEFINE(29);
OBFH_PD_DEFINE(30);
OBFH_PD_DEFINE(31);
OBFH_PD_DEFINE(32);
OBFH_PD_DEFINE(33);
OBFH_PD_DEFINE(34);
OBFH_PD_DEFINE(35);
OBFH_PD_DEFINE(36);
OBFH_PD_DEFINE(37);
OBFH_PD_DEFINE(38);
OBFH_PD_DEFINE(39);
OBFH_PD_DEFINE(40);
OBFH_PD_DEFINE(41);
OBFH_PD_DEFINE(42);
OBFH_PD_DEFINE(43);
OBFH_PD_DEFINE(44);
OBFH_PD_DEFINE(45);
OBFH_PD_DEFINE(46);
OBFH_PD_DEFINE(47);
OBFH_PD_DEFINE(48);
OBFH_PD_DEFINE(49);
OBFH_PD_DEFINE(50);
OBFH_PD_DEFINE(51);
OBFH_PD_DEFINE(52);
OBFH_PD_DEFINE(53);
OBFH_PD_DEFINE(54);
OBFH_PD_DEFINE(55);
OBFH_PD_DEFINE(56);
OBFH_PD_DEFINE(57);
OBFH_PD_DEFINE(58);
OBFH_PD_DEFINE(59);
OBFH_PD_DEFINE(60);
OBFH_PD_DEFINE(61);
OBFH_PD_DEFINE(62);
OBFH_PD_DEFINE(63);
OBFH_PD_DEFINE(64);
OBFH_PD_DEFINE(65);
OBFH_PD_DEFINE(66);
OBFH_PD_DEFINE(67);
OBFH_PD_DEFINE(68);
OBFH_PD_DEFINE(69);
OBFH_PD_DEFINE(70);
OBFH_PD_DEFINE(71);
OBFH_PD_DEFINE(72);
OBFH_PD_DEFINE(73);
OBFH_PD_DEFINE(74);
OBFH_PD_DEFINE(75);
OBFH_PD_DEFINE(76);
OBFH_PD_DEFINE(77);
OBFH_PD_DEFINE(78);
OBFH_PD_DEFINE(79);
OBFH_PD_DEFINE(80);
OBFH_PD_DEFINE(81);
OBFH_PD_DEFINE(82);
OBFH_PD_DEFINE(83);
OBFH_PD_DEFINE(84);
OBFH_PD_DEFINE(85);
#if OBFH_PDATA_DECOY_COUNT > 86
OBFH_PD_DEFINE(86);
#endif
#if OBFH_PDATA_DECOY_COUNT > 87
OBFH_PD_DEFINE(87);
#endif
#if OBFH_PDATA_DECOY_COUNT > 88
OBFH_PD_DEFINE(88);
#endif
#if OBFH_PDATA_DECOY_COUNT > 89
OBFH_PD_DEFINE(89);
#endif
#if OBFH_PDATA_DECOY_COUNT > 90
OBFH_PD_DEFINE(90);
#endif
#if OBFH_PDATA_DECOY_COUNT > 91
OBFH_PD_DEFINE(91);
#endif
#if OBFH_PDATA_DECOY_COUNT > 92
OBFH_PD_DEFINE(92);
#endif
#if OBFH_PDATA_DECOY_COUNT > 93
OBFH_PD_DEFINE(93);
#endif
#if OBFH_PDATA_DECOY_COUNT > 94
OBFH_PD_DEFINE(94);
#endif
#if OBFH_PDATA_DECOY_COUNT > 95
OBFH_PD_DEFINE(95);
#endif
#if OBFH_PDATA_DECOY_COUNT > 96
OBFH_PD_DEFINE(96);
#endif
#if OBFH_PDATA_DECOY_COUNT > 97
OBFH_PD_DEFINE(97);
#endif
#if OBFH_PDATA_DECOY_COUNT > 98
OBFH_PD_DEFINE(98);
#endif
#if OBFH_PDATA_DECOY_COUNT > 99
OBFH_PD_DEFINE(99);
#endif
#if OBFH_PDATA_DECOY_COUNT > 100
OBFH_PD_DEFINE(100);
#endif
#if OBFH_PDATA_DECOY_COUNT > 101
OBFH_PD_DEFINE(101);
#endif
#if OBFH_PDATA_DECOY_COUNT > 102
OBFH_PD_DEFINE(102);
#endif
#if OBFH_PDATA_DECOY_COUNT > 103
OBFH_PD_DEFINE(103);
#endif
#if OBFH_PDATA_DECOY_COUNT > 104
OBFH_PD_DEFINE(104);
#endif
#if OBFH_PDATA_DECOY_COUNT > 105
OBFH_PD_DEFINE(105);
#endif
#if OBFH_PDATA_DECOY_COUNT > 106
OBFH_PD_DEFINE(106);
#endif
#if OBFH_PDATA_DECOY_COUNT > 107
OBFH_PD_DEFINE(107);
#endif
#if OBFH_PDATA_DECOY_COUNT > 108
OBFH_PD_DEFINE(108);
#endif
#if OBFH_PDATA_DECOY_COUNT > 109
OBFH_PD_DEFINE(109);
#endif
#if OBFH_PDATA_DECOY_COUNT > 110
OBFH_PD_DEFINE(110);
#endif
#if OBFH_PDATA_DECOY_COUNT > 111
OBFH_PD_DEFINE(111);
#endif
#if OBFH_PDATA_DECOY_COUNT > 112
OBFH_PD_DEFINE(112);
#endif
#if OBFH_PDATA_DECOY_COUNT > 113
OBFH_PD_DEFINE(113);
#endif
#if OBFH_PDATA_DECOY_COUNT > 114
OBFH_PD_DEFINE(114);
#endif
#if OBFH_PDATA_DECOY_COUNT > 115
OBFH_PD_DEFINE(115);
#endif
#if OBFH_PDATA_DECOY_COUNT > 116
OBFH_PD_DEFINE(116);
#endif
#if OBFH_PDATA_DECOY_COUNT > 117
OBFH_PD_DEFINE(117);
#endif
#if OBFH_PDATA_DECOY_COUNT > 118
OBFH_PD_DEFINE(118);
#endif
#if OBFH_PDATA_DECOY_COUNT > 119
OBFH_PD_DEFINE(119);
#endif
#if OBFH_PDATA_DECOY_COUNT > 120
OBFH_PD_DEFINE(120);
#endif
#if OBFH_PDATA_DECOY_COUNT > 121
OBFH_PD_DEFINE(121);
#endif
#if OBFH_PDATA_DECOY_COUNT > 122
OBFH_PD_DEFINE(122);
#endif
#if OBFH_PDATA_DECOY_COUNT > 123
OBFH_PD_DEFINE(123);
#endif
#if OBFH_PDATA_DECOY_COUNT > 124
OBFH_PD_DEFINE(124);
#endif
#if OBFH_PDATA_DECOY_COUNT > 125
OBFH_PD_DEFINE(125);
#endif
#if OBFH_PDATA_DECOY_COUNT > 126
OBFH_PD_DEFINE(126);
#endif
#if OBFH_PDATA_DECOY_COUNT > 127
OBFH_PD_DEFINE(127);
#endif
#endif

// ============================================================================
// 06. Skipped payloads, ASM operands and BREAK_STACK_CFLOW templates
// ============================================================================

#define OBFH_JUNK_RANDOM_INPUTS \
    "i"(OBFH_JUNK_BYTE), "i"(OBFH_JUNK_BYTE), "i"(OBFH_JUNK_BYTE), "i"(OBFH_JUNK_BYTE), "i"(OBFH_JUNK_WORD)

// Operand positions are part of the payload contract. Keep each draw per expansion.
#define OBFH_JUNK_INPUTS "i"(RND(1, 15)), "i"(RND(0, 255)), OBFH_JUNK_RANDOM_INPUTS
#define OBFH_STACK_JUNK_INPUTS "i"(RND(1, 32767)), OBFH_JUNK_INPUTS
#define OBFH_CFLOW_RANDOM_INPUTS "i"(RND(1, 32767)), "i"(RND(1, 15)), "i"(RND(0, 255)), "i"(RND(1, 2147483646u))
#define OBFH_CPUID_CLOBBERS "eax", "ebx", "ecx", "edx", "cc", "memory"
#define OBFH_CFLOW_CLOBBERS "eax", "edx", "ecx", "cc", "memory"
#define OBFH_JUNK_ASM(code) __obfh_asm__(code               \
                                         :                  \
                                         : OBFH_JUNK_INPUTS \
                                         : OBFH_CPUID_CLOBBERS)
#define OBFH_STACK_JUNK_ASM(code, ...) __obfh_asm__(code                     \
                                                    :                        \
                                                    : OBFH_STACK_JUNK_INPUTS \
                                                    : __VA_ARGS__, "cc", "memory")
#define OBFH_CFLOW_ASM(code) __obfh_asm__(code                       \
                                          :                          \
                                          : OBFH_CFLOW_RANDOM_INPUTS \
                                          : OBFH_CFLOW_CLOBBERS)

// These bytes belong only to skipped regions; labels and branch predicates stay at the call site.
#define OBFH_JUNK_PAYLOAD ".byte %c2, %c3, %c4, %c5; .long %c6; .fill %c0, 1, %c1;"
#define OBFH_STACK_JUNK_PAYLOAD ".byte %c3, %c4, %c5, %c6; .long %c7; .fill %c1, 1, %c2;"
#define OBFH_CFLOW_FILL ".fill %c1, 1, %c2;"
#define OBFH_CFLOW_PAYLOAD_CALL ".byte 0xE8; " OBFH_CFLOW_FILL
#define OBFH_CFLOW_PAYLOAD_INDIRECT ".byte 0xFF, 0x25; .long %c3; " OBFH_CFLOW_FILL
#define OBFH_CFLOW_PAYLOAD_MOV ".byte 0x48, 0xB8; .long %c3; .long %c3; " OBFH_CFLOW_FILL
#define OBFH_CFLOW_PAYLOAD_JUMP ".byte 0xE9; .long %c3; " OBFH_CFLOW_FILL
#define OBFH_CFLOW_PAYLOAD_CALL_DATA ".byte 0xE8, %c2; .long %c3; " OBFH_CFLOW_FILL
#define OBFH_CFLOW_PAYLOAD_TRAP_INDIRECT ".byte 0x0F, 0x0B; .byte 0xFF, 0x25; .long %c3; .byte %c2, 0xE9;"

// Every skip identity holds modulo 2^32 for every input, not just aligned SP.
// Unsigned polynomial/bit identities; parameters are asm register strings.
#define OBFH_CFLOW_PRED_ADJACENT_PRODUCT(r0, r1) "leal 1(" r0 "), " r1 "; imull " r1 ", " r0 ";"
#define OBFH_CFLOW_PRED_SQUARE(r0) "imull " r0 ", " r0 ";"
#define OBFH_CFLOW_PRED_SQUARE_XOR(r0, r1) "movl " r0 ", " r1 "; imull " r0 ", " r0 "; xorl " r1 ", " r0 ";"
#define OBFH_CFLOW_PRED_ODD_PRODUCT(r0, r1) "leal 1(" r0 "), " r1 "; imull " r1 ", " r0 "; addl $1, " r0 ";"
#define OBFH_CFLOW_PRED_CUBE_MINUS(r0, r1) "movl " r0 ", " r1 "; imull " r0 ", " r0 "; imull " r1 ", " r0 "; subl " r1 ", " r0 ";"
#define OBFH_CFLOW_PRED_THREE_PRODUCT(r0, r1, r2) "leal 1(" r0 "), " r1 "; leal 2(" r0 "), " r2 "; imull " r1 ", " r0 "; imull " r2 ", " r0 ";"
#define OBFH_CFLOW_PRED_FOUR_PRODUCT(r0, r1, r2) "leal 1(" r0 "), " r1 "; leal 2(" r0 "), " r2 "; imull " r0 ", " r1 "; addl $3, " r0 "; imull " r2 ", " r1 "; imull " r1 ", " r0 ";"
#define OBFH_CFLOW_PRED_SQUARE_PRODUCT(r0, r1) "imull " r0 ", " r0 "; leal -1(" r0 "), " r1 "; imull " r1 ", " r0 ";"
#define OBFH_CFLOW_PRED_FOURTH(r0) "imull " r0 ", " r0 "; imull " r0 ", " r0 ";"
#define OBFH_CFLOW_PRED_FIFTH_MINUS(r0, r1) "movl " r0 ", " r1 "; imull " r0 ", " r0 "; imull " r0 ", " r0 "; imull " r1 ", " r0 "; subl " r1 ", " r0 ";"
#define OBFH_CFLOW_PRED_ADJACENT_OR(r0, r1) "leal 1(" r0 "), " r1 "; orl " r1 ", " r0 ";"
#define OBFH_CFLOW_PRED_ADJACENT_AND(r0, r1) "leal 1(" r0 "), " r1 "; andl " r1 ", " r0 ";"
#define OBFH_CFLOW_PRED_PREVIOUS_PRODUCT(r0, r1) "leal -1(" r0 "), " r1 "; imull " r1 ", " r0 ";"
#define OBFH_CFLOW_PRED_SQUARE_PLUS(r0, r1) "movl " r0 ", " r1 "; imull " r0 ", " r0 "; addl " r1 ", " r0 ";"
#define OBFH_CFLOW_PRED_CUBE_PLUS(r0, r1) "movl " r0 ", " r1 "; imull " r0 ", " r0 "; imull " r1 ", " r0 "; addl " r1 ", " r0 ";"
#define OBFH_CFLOW_PRED_ODD_SQUARE(r0) "imull " r0 ", " r0 "; orl $1, " r0 ";"

// The live path reads SP only. Stack writes, traps and indirect transfers are skipped data.
#define OBFH_CFLOW_NAMED_INPUTS                                        \
    [junk_salt] "i"(RND(1, 32767)), "i"(RND(1, 15)), "i"(RND(0, 255)), \
        [junk_key] "i"(OBFH_JUNK_WORD), OBFH_JUNK_RANDOM_INPUTS, [junk_rotate] "i"(RND(1, 31))
#define OBFH_CFLOW_NAMED_ASM(code) __obfh_asm__(code                      \
                                                :                         \
                                                : OBFH_CFLOW_NAMED_INPUTS \
                                                : OBFH_CFLOW_CLOBBERS)
#define OBFH_CFLOW_INPUT(reg) "movl %%esp, " reg "; xorl %[junk_salt], " reg "; roll %[junk_rotate], " reg ";"
#define OBFH_CFLOW_DATA ".byte %c4, %c5, %c6, %c7; .long %c8;" OBFH_CFLOW_FILL
#define OBFH_CFLOW_DATA_CALL ".byte 0xE8; .long %c3;" OBFH_CFLOW_DATA
#define OBFH_CFLOW_DATA_STACK ".byte 0x48, 0xBC; .long %c8; .long %c3; .byte 0xFF, 0xE4;" OBFH_CFLOW_DATA
#define OBFH_CFLOW_DATA_FRAME ".byte 0xC8, %c4, %c5, %c6, 0xC9, 0xC3;" OBFH_CFLOW_DATA
#define OBFH_CFLOW_DATA_INDIRECT ".byte 0xFF, 0x15; .long %c8; .byte 0xE9; .long %c3;" OBFH_CFLOW_DATA
#define OBFH_CFLOW_DATA_TRAP ".byte 0x0F, 0x0B, 0x8F, 0x04, 0x24;" OBFH_CFLOW_DATA
#define OBFH_CFLOW_DATA_RETURN ".byte 0xC2, %c4, %c5, 0xE8; .long %c8;" OBFH_CFLOW_DATA

#define OBFH_CFLOW_PRED_ADD_CARRY OBFH_CFLOW_INPUT("%%eax") "movl %%eax, %%edx; xorl %[junk_key], %%eax; andl %[junk_key], %%edx; addl %%edx, %%edx; addl %%edx, %%eax; " OBFH_CFLOW_INPUT("%%edx") "addl %[junk_key], %%edx; cmpl %%edx, %%eax;"
#define OBFH_CFLOW_PRED_OR_AND_SUM OBFH_CFLOW_INPUT("%%eax") "movl %%eax, %%edx; orl %[junk_key], %%eax; andl %[junk_key], %%edx; addl %%edx, %%eax; " OBFH_CFLOW_INPUT("%%edx") "addl %[junk_key], %%edx; cmpl %%edx, %%eax;"
#define OBFH_CFLOW_PRED_SUB_BORROW OBFH_CFLOW_INPUT("%%eax") "movl %%eax, %%edx; xorl %[junk_key], %%eax; notl %%edx; andl %[junk_key], %%edx; addl %%edx, %%edx; subl %%edx, %%eax; " OBFH_CFLOW_INPUT("%%edx") "subl %[junk_key], %%edx; cmpl %%edx, %%eax;"
#define OBFH_CFLOW_PRED_DEMORGAN OBFH_CFLOW_INPUT("%%eax") "orl %[junk_key], %%eax; notl %%eax; " OBFH_CFLOW_INPUT("%%edx") "notl %%edx; movl %[junk_key], %%ecx; notl %%ecx; andl %%ecx, %%edx; cmpl %%edx, %%eax;"
#define OBFH_CFLOW_PRED_OR_DISTRIBUTE OBFH_CFLOW_INPUT("%%eax") "movl %%eax, %%edx; orl %[junk_key], %%eax; movl %[junk_key], %%ecx; notl %%ecx; orl %%ecx, %%edx; andl %%edx, %%eax; " OBFH_CFLOW_INPUT("%%edx") " cmpl %%edx, %%eax;"
#define OBFH_CFLOW_PRED_AND_PARTITION OBFH_CFLOW_INPUT("%%eax") "movl %%eax, %%edx; andl %[junk_key], %%eax; movl %[junk_key], %%ecx; notl %%ecx; andl %%ecx, %%edx; orl %%edx, %%eax; " OBFH_CFLOW_INPUT("%%edx") " cmpl %%edx, %%eax;"
#define OBFH_CFLOW_PRED_XOR_CANCEL OBFH_CFLOW_INPUT("%%eax") "movl %%eax, %%edx; xorl %[junk_key], %%eax; xorl %%edx, %%eax; cmpl %[junk_key], %%eax;"
#define OBFH_CFLOW_PRED_COMPLEMENT_CARRY OBFH_CFLOW_INPUT("%%eax") "movl %%eax, %%edx; notl %%edx; addl %%edx, %%eax;"
#define OBFH_CFLOW_PRED_COMPLEMENT_WRAP OBFH_CFLOW_INPUT("%%eax") "movl %%eax, %%edx; notl %%edx; addl %%edx, %%eax; addl $1, %%eax;"
#define OBFH_CFLOW_PRED_MASK_SUBTRACT OBFH_CFLOW_INPUT("%%eax") "movl %%eax, %%edx; andl %[junk_key], %%edx; subl %%edx, %%eax;"
#define OBFH_CFLOW_PRED_MASK_OR_ORDER OBFH_CFLOW_INPUT("%%eax") "movl %%eax, %%edx; orl %[junk_key], %%eax; cmpl %%edx, %%eax;"
#define OBFH_CFLOW_PRED_ROTATE_XOR OBFH_CFLOW_INPUT("%%eax") "xorl %[junk_key], %%eax; roll %[junk_rotate], %%eax; " OBFH_CFLOW_INPUT("%%edx") "roll %[junk_rotate], %%edx; movl %[junk_key], %%ecx; roll %[junk_rotate], %%ecx; xorl %%ecx, %%edx; cmpl %%edx, %%eax;"
#define OBFH_CFLOW_PRED_ROTATE_RESTORE OBFH_CFLOW_INPUT("%%eax") "roll %[junk_rotate], %%eax; rorl %[junk_rotate], %%eax; " OBFH_CFLOW_INPUT("%%edx") " cmpl %%edx, %%eax;"
#define OBFH_CFLOW_PRED_BSWAP_XOR OBFH_CFLOW_INPUT("%%eax") "xorl %[junk_key], %%eax; bswap %%eax; " OBFH_CFLOW_INPUT("%%edx") "bswap %%edx; movl %[junk_key], %%ecx; bswap %%ecx; xorl %%ecx, %%edx; cmpl %%edx, %%eax;"
#define OBFH_CFLOW_PRED_WORD_PARTITION OBFH_CFLOW_INPUT("%%eax") "movl %%eax, %%edx; andl $65535, %%eax; andl $-65536, %%edx; orl %%edx, %%eax; " OBFH_CFLOW_INPUT("%%edx") " cmpl %%edx, %%eax;"
#define OBFH_CFLOW_PRED_BYTE_PARITY OBFH_CFLOW_INPUT("%%eax") "movzbl %%al, %%edx; imull %%eax, %%eax; addl %%edx, %%eax; testb $1, %%al;"
#define OBFH_CFLOW_PRED_BYTE_ROTATE OBFH_CFLOW_INPUT("%%eax") "movl %%eax, %%edx; rolb %[junk_rotate], %%al; rorb %[junk_rotate], %%al; cmpb %%dl, %%al;"
#define OBFH_CFLOW_PRED_WORD_ROTATE OBFH_CFLOW_INPUT("%%eax") "movl %%eax, %%edx; rolw %[junk_rotate], %%ax; rorw %[junk_rotate], %%ax; cmpw %%dx, %%ax;"
#define OBFH_CFLOW_PRED_NEG_COMPLEMENT OBFH_CFLOW_INPUT("%%eax") "notl %%eax; addl $1, %%eax; " OBFH_CFLOW_INPUT("%%edx") "negl %%edx; cmpl %%edx, %%eax;"
#define OBFH_CFLOW_PRED_MUL_DISTRIBUTE OBFH_CFLOW_INPUT("%%eax") "addl %[junk_key], %%eax; imull %[junk_salt], %%eax; " OBFH_CFLOW_INPUT("%%edx") "imull %[junk_salt], %%edx; movl %[junk_key], %%ecx; imull %[junk_salt], %%ecx; addl %%ecx, %%edx; cmpl %%edx, %%eax;"
#define OBFH_CFLOW_PRED_SQUARE_EXPAND OBFH_CFLOW_INPUT("%%eax") "movl %%eax, %%edx; imull %%edx, %%edx; movl %%eax, %%ecx; imull %[junk_key], %%ecx; addl %%ecx, %%ecx; addl %[junk_key], %%eax; imull %%eax, %%eax; subl %%edx, %%eax; subl %%ecx, %%eax; movl %[junk_key], %%edx; imull %%edx, %%edx; cmpl %%edx, %%eax;"
#define OBFH_CFLOW_PRED_NEG_SQUARE OBFH_CFLOW_INPUT("%%eax") "negl %%eax; imull %%eax, %%eax; " OBFH_CFLOW_INPUT("%%edx") "imull %%edx, %%edx; cmpl %%edx, %%eax;"
#define OBFH_CFLOW_PRED_BSWAP_NOT OBFH_CFLOW_INPUT("%%eax") "notl %%eax; bswap %%eax; " OBFH_CFLOW_INPUT("%%edx") "bswap %%edx; notl %%edx; cmpl %%edx, %%eax;"
#define OBFH_CFLOW_PRED_SUB_MASK OBFH_CFLOW_INPUT("%%eax") "movl %%eax, %%edx; andl %[junk_key], %%edx; subl %%edx, %%eax; " OBFH_CFLOW_INPUT("%%edx") "movl %[junk_key], %%ecx; notl %%ecx; andl %%ecx, %%edx; cmpl %%edx, %%eax;"
#define OBFH_CFLOW_PRED_MASK_ORDER OBFH_CFLOW_INPUT("%%eax") "movl %%eax, %%edx; andl %[junk_key], %%eax; orl %[junk_key], %%edx; cmpl %%edx, %%eax;"
#define OBFH_CFLOW_PRED_COMPLEMENT_TRANSLATE OBFH_CFLOW_INPUT("%%eax") "notl %%eax; addl %[junk_key], %%eax; " OBFH_CFLOW_INPUT("%%edx") "negl %%edx; addl %[junk_key], %%edx; subl $1, %%edx; cmpl %%edx, %%eax;"
#define OBFH_CFLOW_PRED_SQUARE_RESIDUE OBFH_CFLOW_INPUT("%%eax") "imull %%eax, %%eax; andl $7, %%eax; leal -1(%%eax), %%edx; leal -4(%%eax), %%ecx; imull %%edx, %%eax; imull %%ecx, %%eax; testl %%eax, %%eax;"
#define OBFH_CFLOW_PRED_FOURTH_RESIDUE OBFH_CFLOW_INPUT("%%eax") "imull %%eax, %%eax; imull %%eax, %%eax; andl $15, %%eax; leal -1(%%eax), %%edx; imull %%edx, %%eax; testl %%eax, %%eax;"
#define OBFH_CFLOW_PRED_ISOLATE_BIT OBFH_CFLOW_INPUT("%%eax") "movl %%eax, %%edx; negl %%edx; andl %%edx, %%eax; leal -1(%%eax), %%edx; andl %%edx, %%eax; testl %%eax, %%eax;"

// One local set of immediate parameters per selection. Each operand retains
// its old range and position; no runtime storage or shared helper is needed.
#define OBFH_CFLOW_CAPTURE                   \
    enum {                                   \
        __obfh_cf_salt = RND(1, 32767),      \
        __obfh_cf_pad = RND(1, 15),          \
        __obfh_cf_byte = RND(0, 255),        \
        __obfh_cf_key = RND(1, 2147483646u), \
        __obfh_cf_word = OBFH_JUNK_WORD,     \
        __obfh_cf_byte0 = OBFH_JUNK_BYTE,    \
        __obfh_cf_byte1 = OBFH_JUNK_BYTE,    \
        __obfh_cf_byte2 = OBFH_JUNK_BYTE,    \
        __obfh_cf_byte3 = OBFH_JUNK_BYTE,    \
        __obfh_cf_payload = OBFH_JUNK_WORD,  \
        __obfh_cf_rotate = RND(1, 31)        \
    }
#define OBFH_CFLOW_LOCAL_RANDOM \
    "i"(__obfh_cf_byte0), "i"(__obfh_cf_byte1), "i"(__obfh_cf_byte2), "i"(__obfh_cf_byte3), "i"(__obfh_cf_payload)
#define OBFH_CFLOW_LOCAL_JUNK_INPUTS "i"(__obfh_cf_pad), "i"(__obfh_cf_byte), OBFH_CFLOW_LOCAL_RANDOM
#define OBFH_CFLOW_LOCAL_ASM(code)                                                                  \
    __obfh_asm__(code                                                                               \
                 :                                                                                  \
                 : "i"(__obfh_cf_salt), "i"(__obfh_cf_pad), "i"(__obfh_cf_byte), "i"(__obfh_cf_key) \
                 : OBFH_CFLOW_CLOBBERS)
#define OBFH_CFLOW_LOCAL_JUNK_ASM(code)         \
    __obfh_asm__(code                           \
                 :                              \
                 : OBFH_CFLOW_LOCAL_JUNK_INPUTS \
                 : OBFH_CPUID_CLOBBERS)
#define OBFH_CFLOW_LOCAL_STACK_ASM(code, ...)                        \
    __obfh_asm__(code                                                \
                 :                                                   \
                 : "i"(__obfh_cf_salt), OBFH_CFLOW_LOCAL_JUNK_INPUTS \
                 : __VA_ARGS__, "cc", "memory")
#define OBFH_CFLOW_LOCAL_NAMED_ASM(code)                                                                        \
    __obfh_asm__(code                                                                                           \
                 :                                                                                              \
                 : [junk_salt] "i"(__obfh_cf_salt), "i"(__obfh_cf_pad), "i"(__obfh_cf_byte),                    \
                   [junk_key] "i"(__obfh_cf_word), OBFH_CFLOW_LOCAL_RANDOM, [junk_rotate] "i"(__obfh_cf_rotate) \
                 : OBFH_CFLOW_CLOBBERS)

// All 128 templates are available; selection emits only the chosen asm.
#define OBFH_CFLOW_VARIANT_COUNT 128u
#define OBFH_CFLOW_GROUP_0(index) \
    __builtin_choose_expr((index) < 4u, __builtin_choose_expr((index) < 2u, __builtin_choose_expr((index) < 1u, ({ OBFH_CFLOW_TEMPLATE_0; }), ({ OBFH_CFLOW_TEMPLATE_1; })), __builtin_choose_expr((index) < 3u, ({ OBFH_CFLOW_TEMPLATE_2; }), ({ OBFH_CFLOW_TEMPLATE_3; }))), __builtin_choose_expr((index) < 6u, __builtin_choose_expr((index) < 5u, ({ OBFH_CFLOW_TEMPLATE_4; }), ({ OBFH_CFLOW_TEMPLATE_5; })), __builtin_choose_expr((index) < 7u, ({ OBFH_CFLOW_TEMPLATE_6; }), ({ OBFH_CFLOW_TEMPLATE_7; }))))
#define OBFH_CFLOW_GROUP_1(index) \
    __builtin_choose_expr((index) < 12u, __builtin_choose_expr((index) < 10u, __builtin_choose_expr((index) < 9u, ({ OBFH_CFLOW_TEMPLATE_8; }), ({ OBFH_CFLOW_TEMPLATE_9; })), __builtin_choose_expr((index) < 11u, ({ OBFH_CFLOW_TEMPLATE_10; }), ({ OBFH_CFLOW_TEMPLATE_11; }))), __builtin_choose_expr((index) < 14u, __builtin_choose_expr((index) < 13u, ({ OBFH_CFLOW_TEMPLATE_12; }), ({ OBFH_CFLOW_TEMPLATE_13; })), __builtin_choose_expr((index) < 15u, ({ OBFH_CFLOW_TEMPLATE_14; }), ({ OBFH_CFLOW_TEMPLATE_15; }))))
#define OBFH_CFLOW_GROUP_2(index) \
    __builtin_choose_expr((index) < 20u, __builtin_choose_expr((index) < 18u, __builtin_choose_expr((index) < 17u, ({ OBFH_CFLOW_TEMPLATE_16; }), ({ OBFH_CFLOW_TEMPLATE_17; })), __builtin_choose_expr((index) < 19u, ({ OBFH_CFLOW_TEMPLATE_18; }), ({ OBFH_CFLOW_TEMPLATE_19; }))), __builtin_choose_expr((index) < 22u, __builtin_choose_expr((index) < 21u, ({ OBFH_CFLOW_TEMPLATE_20; }), ({ OBFH_CFLOW_TEMPLATE_21; })), __builtin_choose_expr((index) < 23u, ({ OBFH_CFLOW_TEMPLATE_22; }), ({ OBFH_CFLOW_TEMPLATE_23; }))))
#define OBFH_CFLOW_GROUP_3(index) \
    __builtin_choose_expr((index) < 28u, __builtin_choose_expr((index) < 26u, __builtin_choose_expr((index) < 25u, ({ OBFH_CFLOW_TEMPLATE_24; }), ({ OBFH_CFLOW_TEMPLATE_25; })), __builtin_choose_expr((index) < 27u, ({ OBFH_CFLOW_TEMPLATE_26; }), ({ OBFH_CFLOW_TEMPLATE_27; }))), __builtin_choose_expr((index) < 30u, __builtin_choose_expr((index) < 29u, ({ OBFH_CFLOW_TEMPLATE_28; }), ({ OBFH_CFLOW_TEMPLATE_29; })), __builtin_choose_expr((index) < 31u, ({ OBFH_CFLOW_TEMPLATE_30; }), ({ OBFH_CFLOW_TEMPLATE_31; }))))
#define OBFH_CFLOW_GROUP_4(index) \
    __builtin_choose_expr((index) < 36u, __builtin_choose_expr((index) < 34u, __builtin_choose_expr((index) < 33u, ({ OBFH_CFLOW_TEMPLATE_32; }), ({ OBFH_CFLOW_TEMPLATE_33; })), __builtin_choose_expr((index) < 35u, ({ OBFH_CFLOW_TEMPLATE_34; }), ({ OBFH_CFLOW_TEMPLATE_35; }))), __builtin_choose_expr((index) < 38u, __builtin_choose_expr((index) < 37u, ({ OBFH_CFLOW_TEMPLATE_36; }), ({ OBFH_CFLOW_TEMPLATE_37; })), __builtin_choose_expr((index) < 39u, ({ OBFH_CFLOW_TEMPLATE_38; }), ({ OBFH_CFLOW_TEMPLATE_39; }))))
#define OBFH_CFLOW_GROUP_5(index) \
    __builtin_choose_expr((index) < 44u, __builtin_choose_expr((index) < 42u, __builtin_choose_expr((index) < 41u, ({ OBFH_CFLOW_TEMPLATE_40; }), ({ OBFH_CFLOW_TEMPLATE_41; })), __builtin_choose_expr((index) < 43u, ({ OBFH_CFLOW_TEMPLATE_42; }), ({ OBFH_CFLOW_TEMPLATE_43; }))), __builtin_choose_expr((index) < 46u, __builtin_choose_expr((index) < 45u, ({ OBFH_CFLOW_TEMPLATE_44; }), ({ OBFH_CFLOW_TEMPLATE_45; })), __builtin_choose_expr((index) < 47u, ({ OBFH_CFLOW_TEMPLATE_46; }), ({ OBFH_CFLOW_TEMPLATE_47; }))))
#define OBFH_CFLOW_GROUP_6(index) \
    __builtin_choose_expr((index) < 52u, __builtin_choose_expr((index) < 50u, __builtin_choose_expr((index) < 49u, ({ OBFH_CFLOW_TEMPLATE_48; }), ({ OBFH_CFLOW_TEMPLATE_49; })), __builtin_choose_expr((index) < 51u, ({ OBFH_CFLOW_TEMPLATE_50; }), ({ OBFH_CFLOW_TEMPLATE_51; }))), __builtin_choose_expr((index) < 54u, __builtin_choose_expr((index) < 53u, ({ OBFH_CFLOW_TEMPLATE_52; }), ({ OBFH_CFLOW_TEMPLATE_53; })), __builtin_choose_expr((index) < 55u, ({ OBFH_CFLOW_TEMPLATE_54; }), ({ OBFH_CFLOW_TEMPLATE_55; }))))
#define OBFH_CFLOW_GROUP_7(index) \
    __builtin_choose_expr((index) < 60u, __builtin_choose_expr((index) < 58u, __builtin_choose_expr((index) < 57u, ({ OBFH_CFLOW_TEMPLATE_56; }), ({ OBFH_CFLOW_TEMPLATE_57; })), __builtin_choose_expr((index) < 59u, ({ OBFH_CFLOW_TEMPLATE_58; }), ({ OBFH_CFLOW_TEMPLATE_59; }))), __builtin_choose_expr((index) < 62u, __builtin_choose_expr((index) < 61u, ({ OBFH_CFLOW_TEMPLATE_60; }), ({ OBFH_CFLOW_TEMPLATE_61; })), __builtin_choose_expr((index) < 63u, ({ OBFH_CFLOW_TEMPLATE_62; }), ({ OBFH_CFLOW_TEMPLATE_63; }))))
#define OBFH_CFLOW_GROUP_8(index) \
    __builtin_choose_expr((index) < 68u, __builtin_choose_expr((index) < 66u, __builtin_choose_expr((index) < 65u, ({ OBFH_CFLOW_TEMPLATE_64; }), ({ OBFH_CFLOW_TEMPLATE_65; })), __builtin_choose_expr((index) < 67u, ({ OBFH_CFLOW_TEMPLATE_66; }), ({ OBFH_CFLOW_TEMPLATE_67; }))), __builtin_choose_expr((index) < 70u, __builtin_choose_expr((index) < 69u, ({ OBFH_CFLOW_TEMPLATE_68; }), ({ OBFH_CFLOW_TEMPLATE_69; })), __builtin_choose_expr((index) < 71u, ({ OBFH_CFLOW_TEMPLATE_70; }), ({ OBFH_CFLOW_TEMPLATE_71; }))))
#define OBFH_CFLOW_GROUP_9(index) \
    __builtin_choose_expr((index) < 76u, __builtin_choose_expr((index) < 74u, __builtin_choose_expr((index) < 73u, ({ OBFH_CFLOW_TEMPLATE_72; }), ({ OBFH_CFLOW_TEMPLATE_73; })), __builtin_choose_expr((index) < 75u, ({ OBFH_CFLOW_TEMPLATE_74; }), ({ OBFH_CFLOW_TEMPLATE_75; }))), __builtin_choose_expr((index) < 78u, __builtin_choose_expr((index) < 77u, ({ OBFH_CFLOW_TEMPLATE_76; }), ({ OBFH_CFLOW_TEMPLATE_77; })), __builtin_choose_expr((index) < 79u, ({ OBFH_CFLOW_TEMPLATE_78; }), ({ OBFH_CFLOW_TEMPLATE_79; }))))
#define OBFH_CFLOW_GROUP_10(index) \
    __builtin_choose_expr((index) < 84u, __builtin_choose_expr((index) < 82u, __builtin_choose_expr((index) < 81u, ({ OBFH_CFLOW_TEMPLATE_80; }), ({ OBFH_CFLOW_TEMPLATE_81; })), __builtin_choose_expr((index) < 83u, ({ OBFH_CFLOW_TEMPLATE_82; }), ({ OBFH_CFLOW_TEMPLATE_83; }))), __builtin_choose_expr((index) < 86u, __builtin_choose_expr((index) < 85u, ({ OBFH_CFLOW_TEMPLATE_84; }), ({ OBFH_CFLOW_TEMPLATE_85; })), __builtin_choose_expr((index) < 87u, ({ OBFH_CFLOW_TEMPLATE_86; }), ({ OBFH_CFLOW_TEMPLATE_87; }))))
#define OBFH_CFLOW_GROUP_11(index) \
    __builtin_choose_expr((index) < 92u, __builtin_choose_expr((index) < 90u, __builtin_choose_expr((index) < 89u, ({ OBFH_CFLOW_TEMPLATE_88; }), ({ OBFH_CFLOW_TEMPLATE_89; })), __builtin_choose_expr((index) < 91u, ({ OBFH_CFLOW_TEMPLATE_90; }), ({ OBFH_CFLOW_TEMPLATE_91; }))), __builtin_choose_expr((index) < 94u, __builtin_choose_expr((index) < 93u, ({ OBFH_CFLOW_TEMPLATE_92; }), ({ OBFH_CFLOW_TEMPLATE_93; })), __builtin_choose_expr((index) < 95u, ({ OBFH_CFLOW_TEMPLATE_94; }), ({ OBFH_CFLOW_TEMPLATE_95; }))))
#define OBFH_CFLOW_GROUP_12(index) \
    __builtin_choose_expr((index) < 100u, __builtin_choose_expr((index) < 98u, __builtin_choose_expr((index) < 97u, ({ OBFH_CFLOW_TEMPLATE_96; }), ({ OBFH_CFLOW_TEMPLATE_97; })), __builtin_choose_expr((index) < 99u, ({ OBFH_CFLOW_TEMPLATE_98; }), ({ OBFH_CFLOW_TEMPLATE_99; }))), __builtin_choose_expr((index) < 102u, __builtin_choose_expr((index) < 101u, ({ OBFH_CFLOW_TEMPLATE_100; }), ({ OBFH_CFLOW_TEMPLATE_101; })), __builtin_choose_expr((index) < 103u, ({ OBFH_CFLOW_TEMPLATE_102; }), ({ OBFH_CFLOW_TEMPLATE_103; }))))
#define OBFH_CFLOW_GROUP_13(index) \
    __builtin_choose_expr((index) < 108u, __builtin_choose_expr((index) < 106u, __builtin_choose_expr((index) < 105u, ({ OBFH_CFLOW_TEMPLATE_104; }), ({ OBFH_CFLOW_TEMPLATE_105; })), __builtin_choose_expr((index) < 107u, ({ OBFH_CFLOW_TEMPLATE_106; }), ({ OBFH_CFLOW_TEMPLATE_107; }))), __builtin_choose_expr((index) < 110u, __builtin_choose_expr((index) < 109u, ({ OBFH_CFLOW_TEMPLATE_108; }), ({ OBFH_CFLOW_TEMPLATE_109; })), __builtin_choose_expr((index) < 111u, ({ OBFH_CFLOW_TEMPLATE_110; }), ({ OBFH_CFLOW_TEMPLATE_111; }))))
#define OBFH_CFLOW_GROUP_14(index) \
    __builtin_choose_expr((index) < 116u, __builtin_choose_expr((index) < 114u, __builtin_choose_expr((index) < 113u, ({ OBFH_CFLOW_TEMPLATE_112; }), ({ OBFH_CFLOW_TEMPLATE_113; })), __builtin_choose_expr((index) < 115u, ({ OBFH_CFLOW_TEMPLATE_114; }), ({ OBFH_CFLOW_TEMPLATE_115; }))), __builtin_choose_expr((index) < 118u, __builtin_choose_expr((index) < 117u, ({ OBFH_CFLOW_TEMPLATE_116; }), ({ OBFH_CFLOW_TEMPLATE_117; })), __builtin_choose_expr((index) < 119u, ({ OBFH_CFLOW_TEMPLATE_118; }), ({ OBFH_CFLOW_TEMPLATE_119; }))))
#define OBFH_CFLOW_GROUP_15(index) \
    __builtin_choose_expr((index) < 124u, __builtin_choose_expr((index) < 122u, __builtin_choose_expr((index) < 121u, ({ OBFH_CFLOW_TEMPLATE_120; }), ({ OBFH_CFLOW_TEMPLATE_121; })), __builtin_choose_expr((index) < 123u, ({ OBFH_CFLOW_TEMPLATE_122; }), ({ OBFH_CFLOW_TEMPLATE_123; }))), __builtin_choose_expr((index) < 126u, __builtin_choose_expr((index) < 125u, ({ OBFH_CFLOW_TEMPLATE_124; }), ({ OBFH_CFLOW_TEMPLATE_125; })), __builtin_choose_expr((index) < 127u, ({ OBFH_CFLOW_TEMPLATE_126; }), ({ OBFH_CFLOW_TEMPLATE_127; }))))
#define OBFH_CFLOW_SELECT(index) ({ OBFH_CFLOW_CAPTURE; \
    __builtin_choose_expr((index) < 64u, __builtin_choose_expr((index) < 32u, __builtin_choose_expr((index) < 16u, __builtin_choose_expr((index) < 8u, OBFH_CFLOW_GROUP_0(index), OBFH_CFLOW_GROUP_1(index)), __builtin_choose_expr((index) < 24u, OBFH_CFLOW_GROUP_2(index), OBFH_CFLOW_GROUP_3(index))), __builtin_choose_expr((index) < 48u, __builtin_choose_expr((index) < 40u, OBFH_CFLOW_GROUP_4(index), OBFH_CFLOW_GROUP_5(index)), __builtin_choose_expr((index) < 56u, OBFH_CFLOW_GROUP_6(index), OBFH_CFLOW_GROUP_7(index)))), __builtin_choose_expr((index) < 96u, __builtin_choose_expr((index) < 80u, __builtin_choose_expr((index) < 72u, OBFH_CFLOW_GROUP_8(index), OBFH_CFLOW_GROUP_9(index)), __builtin_choose_expr((index) < 88u, OBFH_CFLOW_GROUP_10(index), OBFH_CFLOW_GROUP_11(index))), __builtin_choose_expr((index) < 112u, __builtin_choose_expr((index) < 104u, OBFH_CFLOW_GROUP_12(index), OBFH_CFLOW_GROUP_13(index)), __builtin_choose_expr((index) < 120u, OBFH_CFLOW_GROUP_14(index), OBFH_CFLOW_GROUP_15(index))))); (void)0; })

// Avalanche the captured counter and seed before weighted selection.
// Seven CPUID variants occupy 7/256 slots; the other 121 share the remaining slots.
#define OBFH_CFLOW_SLOT(value) (((unsigned int)(value) ^ ((unsigned int)(value) >> 16)) & 255u)
#define OBFH_CFLOW_LIGHT_INDEX(value) ((value) < 86u ? (value) : ((value) < 88u ? (value) + 1u : (value) + 7u))
#define OBFH_CFLOW_WEIGHTED_INDEX(slot) ((slot) < 7u ? ((slot) == 0u ? 86u : (slot) + 88u) : OBFH_CFLOW_LIGHT_INDEX(((slot)-7u) % 121u))
#define OBFH_CFLOW_INDEX(site) OBFH_CFLOW_WEIGHTED_INDEX(OBFH_CFLOW_SLOT(OBFH_MIX_B(OBFH_MIX_A((unsigned int)(site) ^ (unsigned int)OBFH_BUILD_SEED ^ 2654435769u))))
#define OBFH_CFLOW_SINGLE(index) ((void)0)
#define OBFH_CFLOW_LIGHT_ORDINAL(index) ((index) < 86u ? (index) : ((index) < 89u ? (index)-1u : (index)-7u))
#define OBFH_CFLOW_EXTRA_INDEX(index) OBFH_CFLOW_LIGHT_INDEX((OBFH_CFLOW_LIGHT_ORDINAL(index) + 37u) % 121u)
#if CFLOW_V2
// Capture once: the 128-way selector must not duplicate the mapping expression.
#define OBFH_CFLOW_EXTRA(index) ({                               \
    enum { __obfh_extra_index = OBFH_CFLOW_EXTRA_INDEX(index) }; \
    OBFH_CFLOW_SELECT(__obfh_extra_index);                       \
    (void)0;                                                     \
})
#else
#define OBFH_CFLOW_EXTRA(index) OBFH_CFLOW_SINGLE(index)
#endif

#define OBFH_CFLOW_EMIT(site, extra) ({                                                                                  \
    enum { __obfh_break_id = (site),                                                                                     \
           __obfh_break_hash1 = OBFH_MIX_A((unsigned int)__obfh_break_id ^ (unsigned int)OBFH_BUILD_SEED ^ 2654435769u), \
           __obfh_break_hash2 = OBFH_MIX_B(__obfh_break_hash1),                                                          \
           __obfh_break_slot = OBFH_CFLOW_SLOT(__obfh_break_hash2),                                                      \
           __obfh_break_index = OBFH_CFLOW_WEIGHTED_INDEX(__obfh_break_slot) };                                          \
    OBFH_CFLOW_SELECT(__obfh_break_index);                                                                               \
    extra(__obfh_break_index);                                                                                           \
    (void)0;                                                                                                             \
})

// Keep the string compound literal in caller scope and emit exactly one template.
#define OBFH_HIDE_JUNK() OBFH_CFLOW_EMIT(__COUNTER__, OBFH_CFLOW_SINGLE)

// Template pool; only the selected ASM branch is emitted.
#define OBFH_CFLOW_TEMPLATE_0 \
    OBFH_CFLOW_LOCAL_ASM("movl %%esp, %%eax; addl %0, %%eax; " OBFH_CFLOW_PRED_ADJACENT_PRODUCT("%%eax", "%%edx") " testl $1, %%eax; jz 1f; " OBFH_CFLOW_PAYLOAD_CALL " 1:")

#define OBFH_CFLOW_TEMPLATE_1 \
    OBFH_CFLOW_LOCAL_ASM("movl %%esp, %%eax; addl %0, %%eax; " OBFH_CFLOW_PRED_SQUARE("%%eax") " testl $2, %%eax; jz 1f; " OBFH_CFLOW_PAYLOAD_INDIRECT " 1:")

#define OBFH_CFLOW_TEMPLATE_2 \
    OBFH_CFLOW_LOCAL_ASM("movl %%esp, %%eax; addl %0, %%eax; " OBFH_CFLOW_PRED_SQUARE_XOR("%%eax", "%%edx") " testl $1, %%eax; jz 1f; " OBFH_CFLOW_PAYLOAD_MOV " 1:")

#define OBFH_CFLOW_TEMPLATE_3 \
    OBFH_CFLOW_LOCAL_ASM("movl %%esp, %%eax; addl %0, %%eax; " OBFH_CFLOW_PRED_ODD_PRODUCT("%%eax", "%%edx") " testl $1, %%eax; jnz 1f; " OBFH_CFLOW_PAYLOAD_JUMP " 1:")

#define OBFH_CFLOW_TEMPLATE_4 \
    OBFH_CFLOW_LOCAL_ASM("movl %%esp, %%edx; addl %0, %%edx; " OBFH_CFLOW_PRED_CUBE_MINUS("%%edx", "%%ecx") " testl $1, %%edx; jz 1f; " OBFH_CFLOW_PAYLOAD_CALL " 1:")

#define OBFH_CFLOW_TEMPLATE_5 \
    OBFH_CFLOW_LOCAL_ASM("movl %%esp, %%edx; addl %0, %%edx; " OBFH_CFLOW_PRED_THREE_PRODUCT("%%edx", "%%ecx", "%%eax") " testl $1, %%edx; jz 1f; " OBFH_CFLOW_PAYLOAD_INDIRECT " 1:")

#define OBFH_CFLOW_TEMPLATE_6 \
    OBFH_CFLOW_LOCAL_ASM("movl %%esp, %%edx; addl %0, %%edx; " OBFH_CFLOW_PRED_FOUR_PRODUCT("%%edx", "%%ecx", "%%eax") " testl $7, %%edx; jz 1f; " OBFH_CFLOW_PAYLOAD_MOV " 1:")

#define OBFH_CFLOW_TEMPLATE_7 \
    OBFH_CFLOW_LOCAL_ASM("movl %%esp, %%edx; addl %0, %%edx; " OBFH_CFLOW_PRED_SQUARE_PRODUCT("%%edx", "%%ecx") " testl $3, %%edx; jz 1f; " OBFH_CFLOW_PAYLOAD_JUMP " 1:")

#define OBFH_CFLOW_TEMPLATE_8 \
    OBFH_CFLOW_LOCAL_ASM("movl %%esp, %%ecx; addl %0, %%ecx; " OBFH_CFLOW_PRED_FOURTH("%%ecx") " testl $14, %%ecx; jz 1f; " OBFH_CFLOW_PAYLOAD_CALL " 1:")

#define OBFH_CFLOW_TEMPLATE_9 \
    OBFH_CFLOW_LOCAL_ASM("movl %%esp, %%ecx; addl %0, %%ecx; " OBFH_CFLOW_PRED_FIFTH_MINUS("%%ecx", "%%eax") " testl $1, %%ecx; jz 1f; " OBFH_CFLOW_PAYLOAD_INDIRECT " 1:")

#define OBFH_CFLOW_TEMPLATE_10 \
    OBFH_CFLOW_LOCAL_ASM("movl %%esp, %%ecx; addl %0, %%ecx; " OBFH_CFLOW_PRED_ADJACENT_OR("%%ecx", "%%eax") " testl $1, %%ecx; jnz 1f; " OBFH_CFLOW_PAYLOAD_MOV " 1:")

#define OBFH_CFLOW_TEMPLATE_11 \
    OBFH_CFLOW_LOCAL_ASM("movl %%esp, %%ecx; addl %0, %%ecx; " OBFH_CFLOW_PRED_ADJACENT_AND("%%ecx", "%%eax") " testl $1, %%ecx; jz 1f; " OBFH_CFLOW_PAYLOAD_JUMP " 1:")

#define OBFH_CFLOW_TEMPLATE_12 \
    OBFH_CFLOW_LOCAL_ASM("movl %%esp, %%eax; addl %0, %%eax; " OBFH_CFLOW_PRED_PREVIOUS_PRODUCT("%%eax", "%%ecx") " testl $1, %%eax; jz 1f; " OBFH_CFLOW_PAYLOAD_CALL " 1:")

#define OBFH_CFLOW_TEMPLATE_13 \
    OBFH_CFLOW_LOCAL_ASM("movl %%esp, %%eax; addl %0, %%eax; " OBFH_CFLOW_PRED_SQUARE_PLUS("%%eax", "%%ecx") " testl $1, %%eax; jz 1f; " OBFH_CFLOW_PAYLOAD_INDIRECT " 1:")

#define OBFH_CFLOW_TEMPLATE_14 \
    OBFH_CFLOW_LOCAL_ASM("movl %%esp, %%eax; addl %0, %%eax; " OBFH_CFLOW_PRED_CUBE_PLUS("%%eax", "%%ecx") " testl $1, %%eax; jz 1f; " OBFH_CFLOW_PAYLOAD_MOV " 1:")

#define OBFH_CFLOW_TEMPLATE_15 \
    OBFH_CFLOW_LOCAL_ASM("movl %%esp, %%eax; addl %0, %%eax; " OBFH_CFLOW_PRED_ODD_SQUARE("%%eax") " testl $2, %%eax; jz 1f; " OBFH_CFLOW_PAYLOAD_JUMP " 1:")

#define OBFH_CFLOW_TEMPLATE_16 \
    OBFH_CFLOW_LOCAL_ASM("movl %%esp, %%eax; xorl %0, %%eax; " OBFH_CFLOW_PRED_ADJACENT_PRODUCT("%%eax", "%%edx") " testl $1, %%eax; jnz 1f; jmp 2f; 1: " OBFH_CFLOW_PAYLOAD_CALL " 2:")

#define OBFH_CFLOW_TEMPLATE_17 \
    OBFH_CFLOW_LOCAL_ASM("movl %%esp, %%eax; xorl %0, %%eax; " OBFH_CFLOW_PRED_SQUARE("%%eax") " testl $2, %%eax; jnz 1f; jmp 2f; 1: " OBFH_CFLOW_PAYLOAD_INDIRECT " 2:")

#define OBFH_CFLOW_TEMPLATE_18 \
    OBFH_CFLOW_LOCAL_ASM("movl %%esp, %%eax; xorl %0, %%eax; " OBFH_CFLOW_PRED_SQUARE_XOR("%%eax", "%%edx") " testl $1, %%eax; jnz 1f; jmp 2f; 1: " OBFH_CFLOW_PAYLOAD_MOV " 2:")

#define OBFH_CFLOW_TEMPLATE_19 \
    OBFH_CFLOW_LOCAL_ASM("movl %%esp, %%eax; xorl %0, %%eax; " OBFH_CFLOW_PRED_ODD_PRODUCT("%%eax", "%%edx") " testl $1, %%eax; jz 1f; jmp 2f; 1: " OBFH_CFLOW_PAYLOAD_JUMP " 2:")

#define OBFH_CFLOW_TEMPLATE_20 \
    OBFH_CFLOW_LOCAL_ASM("movl %%esp, %%edx; xorl %0, %%edx; " OBFH_CFLOW_PRED_CUBE_MINUS("%%edx", "%%ecx") " testl $1, %%edx; jnz 1f; jmp 2f; 1: " OBFH_CFLOW_PAYLOAD_CALL " 2:")

#define OBFH_CFLOW_TEMPLATE_21 \
    OBFH_CFLOW_LOCAL_ASM("movl %%esp, %%edx; xorl %0, %%edx; " OBFH_CFLOW_PRED_THREE_PRODUCT("%%edx", "%%ecx", "%%eax") " testl $1, %%edx; jnz 1f; jmp 2f; 1: " OBFH_CFLOW_PAYLOAD_INDIRECT " 2:")

#define OBFH_CFLOW_TEMPLATE_22 \
    OBFH_CFLOW_LOCAL_ASM("movl %%esp, %%edx; xorl %0, %%edx; " OBFH_CFLOW_PRED_FOUR_PRODUCT("%%edx", "%%ecx", "%%eax") " testl $7, %%edx; jnz 1f; jmp 2f; 1: " OBFH_CFLOW_PAYLOAD_MOV " 2:")

#define OBFH_CFLOW_TEMPLATE_23 \
    OBFH_CFLOW_LOCAL_ASM("movl %%esp, %%edx; xorl %0, %%edx; " OBFH_CFLOW_PRED_SQUARE_PRODUCT("%%edx", "%%ecx") " testl $3, %%edx; jnz 1f; jmp 2f; 1: " OBFH_CFLOW_PAYLOAD_JUMP " 2:")

#define OBFH_CFLOW_TEMPLATE_24 \
    OBFH_CFLOW_LOCAL_ASM("movl %%esp, %%ecx; xorl %0, %%ecx; " OBFH_CFLOW_PRED_FOURTH("%%ecx") " testl $14, %%ecx; jnz 1f; jmp 2f; 1: " OBFH_CFLOW_PAYLOAD_CALL " 2:")

#define OBFH_CFLOW_TEMPLATE_25 \
    OBFH_CFLOW_LOCAL_ASM("movl %%esp, %%ecx; xorl %0, %%ecx; " OBFH_CFLOW_PRED_FIFTH_MINUS("%%ecx", "%%eax") " testl $1, %%ecx; jnz 1f; jmp 2f; 1: " OBFH_CFLOW_PAYLOAD_INDIRECT " 2:")

#define OBFH_CFLOW_TEMPLATE_26 \
    OBFH_CFLOW_LOCAL_ASM("movl %%esp, %%ecx; xorl %0, %%ecx; " OBFH_CFLOW_PRED_ADJACENT_OR("%%ecx", "%%eax") " testl $1, %%ecx; jz 1f; jmp 2f; 1: " OBFH_CFLOW_PAYLOAD_MOV " 2:")

#define OBFH_CFLOW_TEMPLATE_27 \
    OBFH_CFLOW_LOCAL_ASM("movl %%esp, %%ecx; xorl %0, %%ecx; " OBFH_CFLOW_PRED_ADJACENT_AND("%%ecx", "%%eax") " testl $1, %%ecx; jnz 1f; jmp 2f; 1: " OBFH_CFLOW_PAYLOAD_JUMP " 2:")

#define OBFH_CFLOW_TEMPLATE_28 \
    OBFH_CFLOW_LOCAL_ASM("movl %%esp, %%eax; xorl %0, %%eax; " OBFH_CFLOW_PRED_PREVIOUS_PRODUCT("%%eax", "%%ecx") " testl $1, %%eax; jnz 1f; jmp 2f; 1: " OBFH_CFLOW_PAYLOAD_CALL " 2:")

#define OBFH_CFLOW_TEMPLATE_29 \
    OBFH_CFLOW_LOCAL_ASM("movl %%esp, %%eax; xorl %0, %%eax; " OBFH_CFLOW_PRED_SQUARE_PLUS("%%eax", "%%ecx") " testl $1, %%eax; jnz 1f; jmp 2f; 1: " OBFH_CFLOW_PAYLOAD_INDIRECT " 2:")

#define OBFH_CFLOW_TEMPLATE_30 \
    OBFH_CFLOW_LOCAL_ASM("movl %%esp, %%eax; xorl %0, %%eax; " OBFH_CFLOW_PRED_CUBE_PLUS("%%eax", "%%ecx") " testl $1, %%eax; jnz 1f; jmp 2f; 1: " OBFH_CFLOW_PAYLOAD_MOV " 2:")

#define OBFH_CFLOW_TEMPLATE_31 \
    OBFH_CFLOW_LOCAL_ASM("movl %%esp, %%eax; xorl %0, %%eax; " OBFH_CFLOW_PRED_ODD_SQUARE("%%eax") " testl $2, %%eax; jnz 1f; jmp 2f; 1: " OBFH_CFLOW_PAYLOAD_JUMP " 2:")

#define OBFH_CFLOW_TEMPLATE_32 \
    OBFH_CFLOW_LOCAL_ASM("movl %%esp, %%eax; subl %0, %%eax; " OBFH_CFLOW_PRED_ADJACENT_PRODUCT("%%eax", "%%edx") " testl $1, %%eax; jz 1f; " OBFH_CFLOW_PAYLOAD_CALL " 1: testl $1, %%eax; jz 2f; .byte 0x0F, 0x0B, 0xE8; .fill %c1, 1, %c2; 2:")

#define OBFH_CFLOW_TEMPLATE_33 \
    OBFH_CFLOW_LOCAL_ASM("movl %%esp, %%eax; subl %0, %%eax; " OBFH_CFLOW_PRED_SQUARE("%%eax") " testl $2, %%eax; jz 1f; " OBFH_CFLOW_PAYLOAD_INDIRECT " 1: testl $2, %%eax; jz 2f; .byte 0x0F, 0x0B, 0xE8; .fill %c1, 1, %c2; 2:")

#define OBFH_CFLOW_TEMPLATE_34 \
    OBFH_CFLOW_LOCAL_ASM("movl %%esp, %%eax; subl %0, %%eax; " OBFH_CFLOW_PRED_SQUARE_XOR("%%eax", "%%edx") " testl $1, %%eax; jz 1f; " OBFH_CFLOW_PAYLOAD_MOV " 1: testl $1, %%eax; jz 2f; .byte 0x0F, 0x0B, 0xE8; .fill %c1, 1, %c2; 2:")

#define OBFH_CFLOW_TEMPLATE_35 \
    OBFH_CFLOW_LOCAL_ASM("movl %%esp, %%eax; subl %0, %%eax; " OBFH_CFLOW_PRED_ODD_PRODUCT("%%eax", "%%edx") " testl $1, %%eax; jnz 1f; " OBFH_CFLOW_PAYLOAD_JUMP " 1: testl $1, %%eax; jnz 2f; .byte 0x0F, 0x0B, 0xE8; .fill %c1, 1, %c2; 2:")

#define OBFH_CFLOW_TEMPLATE_36 \
    OBFH_CFLOW_LOCAL_ASM("movl %%esp, %%edx; subl %0, %%edx; " OBFH_CFLOW_PRED_CUBE_MINUS("%%edx", "%%ecx") " testl $1, %%edx; jz 1f; " OBFH_CFLOW_PAYLOAD_CALL " 1: testl $1, %%edx; jz 2f; .byte 0x0F, 0x0B, 0xE8; .fill %c1, 1, %c2; 2:")

#define OBFH_CFLOW_TEMPLATE_37 \
    OBFH_CFLOW_LOCAL_ASM("movl %%esp, %%edx; subl %0, %%edx; " OBFH_CFLOW_PRED_THREE_PRODUCT("%%edx", "%%ecx", "%%eax") " testl $1, %%edx; jz 1f; " OBFH_CFLOW_PAYLOAD_INDIRECT " 1: testl $1, %%edx; jz 2f; .byte 0x0F, 0x0B, 0xE8; .fill %c1, 1, %c2; 2:")

#define OBFH_CFLOW_TEMPLATE_38 \
    OBFH_CFLOW_LOCAL_ASM("movl %%esp, %%edx; subl %0, %%edx; " OBFH_CFLOW_PRED_FOUR_PRODUCT("%%edx", "%%ecx", "%%eax") " testl $7, %%edx; jz 1f; " OBFH_CFLOW_PAYLOAD_MOV " 1: testl $7, %%edx; jz 2f; .byte 0x0F, 0x0B, 0xE8; .fill %c1, 1, %c2; 2:")

#define OBFH_CFLOW_TEMPLATE_39 \
    OBFH_CFLOW_LOCAL_ASM("movl %%esp, %%edx; subl %0, %%edx; " OBFH_CFLOW_PRED_SQUARE_PRODUCT("%%edx", "%%ecx") " testl $3, %%edx; jz 1f; " OBFH_CFLOW_PAYLOAD_JUMP " 1: testl $3, %%edx; jz 2f; .byte 0x0F, 0x0B, 0xE8; .fill %c1, 1, %c2; 2:")

#define OBFH_CFLOW_TEMPLATE_40 \
    OBFH_CFLOW_LOCAL_ASM("movl %%esp, %%ecx; subl %0, %%ecx; " OBFH_CFLOW_PRED_FOURTH("%%ecx") " testl $14, %%ecx; jz 1f; " OBFH_CFLOW_PAYLOAD_CALL " 1: testl $14, %%ecx; jz 2f; .byte 0x0F, 0x0B, 0xE8; .fill %c1, 1, %c2; 2:")

#define OBFH_CFLOW_TEMPLATE_41 \
    OBFH_CFLOW_LOCAL_ASM("movl %%esp, %%ecx; subl %0, %%ecx; " OBFH_CFLOW_PRED_FIFTH_MINUS("%%ecx", "%%eax") " testl $1, %%ecx; jz 1f; " OBFH_CFLOW_PAYLOAD_INDIRECT " 1: testl $1, %%ecx; jz 2f; .byte 0x0F, 0x0B, 0xE8; .fill %c1, 1, %c2; 2:")

#define OBFH_CFLOW_TEMPLATE_42 \
    OBFH_CFLOW_LOCAL_ASM("movl %%esp, %%ecx; subl %0, %%ecx; " OBFH_CFLOW_PRED_ADJACENT_OR("%%ecx", "%%eax") " testl $1, %%ecx; jnz 1f; " OBFH_CFLOW_PAYLOAD_MOV " 1: testl $1, %%ecx; jnz 2f; .byte 0x0F, 0x0B, 0xE8; .fill %c1, 1, %c2; 2:")

#define OBFH_CFLOW_TEMPLATE_43 \
    OBFH_CFLOW_LOCAL_ASM("movl %%esp, %%ecx; subl %0, %%ecx; " OBFH_CFLOW_PRED_ADJACENT_AND("%%ecx", "%%eax") " testl $1, %%ecx; jz 1f; " OBFH_CFLOW_PAYLOAD_JUMP " 1: testl $1, %%ecx; jz 2f; .byte 0x0F, 0x0B, 0xE8; .fill %c1, 1, %c2; 2:")

#define OBFH_CFLOW_TEMPLATE_44 \
    OBFH_CFLOW_LOCAL_ASM("movl %%esp, %%eax; subl %0, %%eax; " OBFH_CFLOW_PRED_PREVIOUS_PRODUCT("%%eax", "%%ecx") " testl $1, %%eax; jz 1f; " OBFH_CFLOW_PAYLOAD_CALL " 1: testl $1, %%eax; jz 2f; .byte 0x0F, 0x0B, 0xE8; .fill %c1, 1, %c2; 2:")

#define OBFH_CFLOW_TEMPLATE_45 \
    OBFH_CFLOW_LOCAL_ASM("movl %%esp, %%eax; subl %0, %%eax; " OBFH_CFLOW_PRED_SQUARE_PLUS("%%eax", "%%ecx") " testl $1, %%eax; jz 1f; " OBFH_CFLOW_PAYLOAD_INDIRECT " 1: testl $1, %%eax; jz 2f; .byte 0x0F, 0x0B, 0xE8; .fill %c1, 1, %c2; 2:")

#define OBFH_CFLOW_TEMPLATE_46 \
    OBFH_CFLOW_LOCAL_ASM("movl %%esp, %%eax; subl %0, %%eax; " OBFH_CFLOW_PRED_CUBE_PLUS("%%eax", "%%ecx") " testl $1, %%eax; jz 1f; " OBFH_CFLOW_PAYLOAD_MOV " 1: testl $1, %%eax; jz 2f; .byte 0x0F, 0x0B, 0xE8; .fill %c1, 1, %c2; 2:")

#define OBFH_CFLOW_TEMPLATE_47 \
    OBFH_CFLOW_LOCAL_ASM("movl %%esp, %%eax; subl %0, %%eax; " OBFH_CFLOW_PRED_ODD_SQUARE("%%eax") " testl $2, %%eax; jz 1f; " OBFH_CFLOW_PAYLOAD_JUMP " 1: testl $2, %%eax; jz 2f; .byte 0x0F, 0x0B, 0xE8; .fill %c1, 1, %c2; 2:")

#define OBFH_CFLOW_TEMPLATE_48 \
    OBFH_CFLOW_LOCAL_ASM("movl %%esp, %%eax; roll $7, %%eax; addl %0, %%eax; " OBFH_CFLOW_PRED_ADJACENT_PRODUCT("%%eax", "%%edx") " testl $1, %%eax; jnz 1f; jmp 2f; 1: .byte 0xC3; " OBFH_CFLOW_PAYLOAD_CALL " 2: testl $1, %%eax; jz 3f; .byte 0xFF, 0x25; .long %c3; 3:")

#define OBFH_CFLOW_TEMPLATE_49 \
    OBFH_CFLOW_LOCAL_ASM("movl %%esp, %%eax; roll $7, %%eax; addl %0, %%eax; " OBFH_CFLOW_PRED_SQUARE("%%eax") " testl $2, %%eax; jnz 1f; jmp 2f; 1: .byte 0xC3; " OBFH_CFLOW_PAYLOAD_INDIRECT " 2: testl $2, %%eax; jz 3f; .byte 0xFF, 0x25; .long %c3; 3:")

#define OBFH_CFLOW_TEMPLATE_50 \
    OBFH_CFLOW_LOCAL_ASM("movl %%esp, %%eax; roll $7, %%eax; addl %0, %%eax; " OBFH_CFLOW_PRED_SQUARE_XOR("%%eax", "%%edx") " testl $1, %%eax; jnz 1f; jmp 2f; 1: .byte 0xC3; " OBFH_CFLOW_PAYLOAD_MOV " 2: testl $1, %%eax; jz 3f; .byte 0xFF, 0x25; .long %c3; 3:")

#define OBFH_CFLOW_TEMPLATE_51 \
    OBFH_CFLOW_LOCAL_ASM("movl %%esp, %%eax; roll $7, %%eax; addl %0, %%eax; " OBFH_CFLOW_PRED_ODD_PRODUCT("%%eax", "%%edx") " testl $1, %%eax; jz 1f; jmp 2f; 1: .byte 0xC3; " OBFH_CFLOW_PAYLOAD_JUMP " 2: testl $1, %%eax; jnz 3f; .byte 0xFF, 0x25; .long %c3; 3:")

#define OBFH_CFLOW_TEMPLATE_52 \
    OBFH_CFLOW_LOCAL_ASM("movl %%esp, %%edx; roll $7, %%edx; addl %0, %%edx; " OBFH_CFLOW_PRED_CUBE_MINUS("%%edx", "%%ecx") " testl $1, %%edx; jnz 1f; jmp 2f; 1: .byte 0xC3; " OBFH_CFLOW_PAYLOAD_CALL " 2: testl $1, %%edx; jz 3f; .byte 0xFF, 0x25; .long %c3; 3:")

#define OBFH_CFLOW_TEMPLATE_53 \
    OBFH_CFLOW_LOCAL_ASM("movl %%esp, %%edx; roll $7, %%edx; addl %0, %%edx; " OBFH_CFLOW_PRED_THREE_PRODUCT("%%edx", "%%ecx", "%%eax") " testl $1, %%edx; jnz 1f; jmp 2f; 1: .byte 0xC3; " OBFH_CFLOW_PAYLOAD_INDIRECT " 2: testl $1, %%edx; jz 3f; .byte 0xFF, 0x25; .long %c3; 3:")

#define OBFH_CFLOW_TEMPLATE_54 \
    OBFH_CFLOW_LOCAL_ASM("movl %%esp, %%edx; roll $7, %%edx; addl %0, %%edx; " OBFH_CFLOW_PRED_FOUR_PRODUCT("%%edx", "%%ecx", "%%eax") " testl $7, %%edx; jnz 1f; jmp 2f; 1: .byte 0xC3; " OBFH_CFLOW_PAYLOAD_MOV " 2: testl $7, %%edx; jz 3f; .byte 0xFF, 0x25; .long %c3; 3:")

#define OBFH_CFLOW_TEMPLATE_55 \
    OBFH_CFLOW_LOCAL_ASM("movl %%esp, %%edx; roll $7, %%edx; addl %0, %%edx; " OBFH_CFLOW_PRED_SQUARE_PRODUCT("%%edx", "%%ecx") " testl $3, %%edx; jnz 1f; jmp 2f; 1: .byte 0xC3; " OBFH_CFLOW_PAYLOAD_JUMP " 2: testl $3, %%edx; jz 3f; .byte 0xFF, 0x25; .long %c3; 3:")

#define OBFH_CFLOW_TEMPLATE_56 \
    OBFH_CFLOW_LOCAL_ASM("movl %%esp, %%ecx; roll $7, %%ecx; addl %0, %%ecx; " OBFH_CFLOW_PRED_FOURTH("%%ecx") " testl $14, %%ecx; jnz 1f; jmp 2f; 1: .byte 0xC3; " OBFH_CFLOW_PAYLOAD_CALL " 2: testl $14, %%ecx; jz 3f; .byte 0xFF, 0x25; .long %c3; 3:")

#define OBFH_CFLOW_TEMPLATE_57 \
    OBFH_CFLOW_LOCAL_ASM("movl %%esp, %%ecx; roll $7, %%ecx; addl %0, %%ecx; " OBFH_CFLOW_PRED_FIFTH_MINUS("%%ecx", "%%eax") " testl $1, %%ecx; jnz 1f; jmp 2f; 1: .byte 0xC3; " OBFH_CFLOW_PAYLOAD_INDIRECT " 2: testl $1, %%ecx; jz 3f; .byte 0xFF, 0x25; .long %c3; 3:")

#define OBFH_CFLOW_TEMPLATE_58 \
    OBFH_CFLOW_LOCAL_ASM("movl %%esp, %%ecx; roll $7, %%ecx; addl %0, %%ecx; " OBFH_CFLOW_PRED_ADJACENT_OR("%%ecx", "%%eax") " testl $1, %%ecx; jz 1f; jmp 2f; 1: .byte 0xC3; " OBFH_CFLOW_PAYLOAD_MOV " 2: testl $1, %%ecx; jnz 3f; .byte 0xFF, 0x25; .long %c3; 3:")

#define OBFH_CFLOW_TEMPLATE_59 \
    OBFH_CFLOW_LOCAL_ASM("movl %%esp, %%ecx; roll $7, %%ecx; addl %0, %%ecx; " OBFH_CFLOW_PRED_ADJACENT_AND("%%ecx", "%%eax") " testl $1, %%ecx; jnz 1f; jmp 2f; 1: .byte 0xC3; " OBFH_CFLOW_PAYLOAD_JUMP " 2: testl $1, %%ecx; jz 3f; .byte 0xFF, 0x25; .long %c3; 3:")

#define OBFH_CFLOW_TEMPLATE_60 \
    OBFH_CFLOW_LOCAL_ASM("movl %%esp, %%eax; roll $7, %%eax; addl %0, %%eax; " OBFH_CFLOW_PRED_PREVIOUS_PRODUCT("%%eax", "%%ecx") " testl $1, %%eax; jnz 1f; jmp 2f; 1: .byte 0xC3; " OBFH_CFLOW_PAYLOAD_CALL " 2: testl $1, %%eax; jz 3f; .byte 0xFF, 0x25; .long %c3; 3:")

#define OBFH_CFLOW_TEMPLATE_61 \
    OBFH_CFLOW_LOCAL_ASM("movl %%esp, %%eax; roll $7, %%eax; addl %0, %%eax; " OBFH_CFLOW_PRED_SQUARE_PLUS("%%eax", "%%ecx") " testl $1, %%eax; jnz 1f; jmp 2f; 1: .byte 0xC3; " OBFH_CFLOW_PAYLOAD_INDIRECT " 2: testl $1, %%eax; jz 3f; .byte 0xFF, 0x25; .long %c3; 3:")

#define OBFH_CFLOW_TEMPLATE_62 \
    OBFH_CFLOW_LOCAL_ASM("movl %%esp, %%eax; roll $7, %%eax; addl %0, %%eax; " OBFH_CFLOW_PRED_CUBE_PLUS("%%eax", "%%ecx") " testl $1, %%eax; jnz 1f; jmp 2f; 1: .byte 0xC3; " OBFH_CFLOW_PAYLOAD_MOV " 2: testl $1, %%eax; jz 3f; .byte 0xFF, 0x25; .long %c3; 3:")

#define OBFH_CFLOW_TEMPLATE_63 \
    OBFH_CFLOW_LOCAL_ASM("movl %%esp, %%eax; roll $7, %%eax; addl %0, %%eax; " OBFH_CFLOW_PRED_ODD_SQUARE("%%eax") " testl $2, %%eax; jnz 1f; jmp 2f; 1: .byte 0xC3; " OBFH_CFLOW_PAYLOAD_JUMP " 2: testl $2, %%eax; jz 3f; .byte 0xFF, 0x25; .long %c3; 3:")

#define OBFH_CFLOW_TEMPLATE_64 \
    OBFH_CFLOW_LOCAL_ASM("movl %%esp, %%eax; addl %0, %%eax; " OBFH_CFLOW_PRED_ADJACENT_PRODUCT("%%eax", "%%edx") "  testl $1, %%eax; jz 1f; " OBFH_CFLOW_PAYLOAD_CALL_DATA " 1: movl %%esp, %%edx; addl %0, %%edx; " OBFH_CFLOW_PRED_SQUARE_PRODUCT("%%edx", "%%ecx") "  testl $3, %%edx; jz 2f; .byte 0xFF, 0x25; .long %c3; .byte %c2, 0xE9; 2:")

#define OBFH_CFLOW_TEMPLATE_65 \
    OBFH_CFLOW_LOCAL_ASM("movl %%esp, %%eax; addl %0, %%eax; " OBFH_CFLOW_PRED_SQUARE("%%eax") "  testl $2, %%eax; jz 1f; " OBFH_CFLOW_PAYLOAD_CALL_DATA " 1: movl %%esp, %%ecx; addl %0, %%ecx; " OBFH_CFLOW_PRED_FOURTH("%%ecx") "  testl $14, %%ecx; jz 2f; .byte 0xFF, 0x25; .long %c3; .byte %c2, 0xE9; 2:")

#define OBFH_CFLOW_TEMPLATE_66 \
    OBFH_CFLOW_LOCAL_ASM("movl %%esp, %%eax; addl %0, %%eax; " OBFH_CFLOW_PRED_SQUARE_XOR("%%eax", "%%edx") "  testl $1, %%eax; jz 1f; " OBFH_CFLOW_PAYLOAD_CALL_DATA " 1: movl %%esp, %%ecx; addl %0, %%ecx; " OBFH_CFLOW_PRED_FIFTH_MINUS("%%ecx", "%%eax") "  testl $1, %%ecx; jz 2f; .byte 0xFF, 0x25; .long %c3; .byte %c2, 0xE9; 2:")

#define OBFH_CFLOW_TEMPLATE_67 \
    OBFH_CFLOW_LOCAL_ASM("movl %%esp, %%eax; addl %0, %%eax; " OBFH_CFLOW_PRED_ODD_PRODUCT("%%eax", "%%edx") "  testl $1, %%eax; jnz 1f; " OBFH_CFLOW_PAYLOAD_CALL_DATA " 1: movl %%esp, %%ecx; addl %0, %%ecx; " OBFH_CFLOW_PRED_ADJACENT_OR("%%ecx", "%%eax") "  testl $1, %%ecx; jnz 2f; .byte 0xFF, 0x25; .long %c3; .byte %c2, 0xE9; 2:")

#define OBFH_CFLOW_TEMPLATE_68 \
    OBFH_CFLOW_LOCAL_ASM("movl %%esp, %%edx; addl %0, %%edx; " OBFH_CFLOW_PRED_CUBE_MINUS("%%edx", "%%ecx") "  testl $1, %%edx; jz 1f; " OBFH_CFLOW_PAYLOAD_CALL_DATA " 1: movl %%esp, %%ecx; addl %0, %%ecx; " OBFH_CFLOW_PRED_ADJACENT_AND("%%ecx", "%%eax") "  testl $1, %%ecx; jz 2f; .byte 0xFF, 0x25; .long %c3; .byte %c2, 0xE9; 2:")

#define OBFH_CFLOW_TEMPLATE_69 \
    OBFH_CFLOW_LOCAL_ASM("movl %%esp, %%edx; addl %0, %%edx; " OBFH_CFLOW_PRED_THREE_PRODUCT("%%edx", "%%ecx", "%%eax") "  testl $1, %%edx; jz 1f; " OBFH_CFLOW_PAYLOAD_CALL_DATA " 1: movl %%esp, %%eax; addl %0, %%eax; " OBFH_CFLOW_PRED_PREVIOUS_PRODUCT("%%eax", "%%ecx") "  testl $1, %%eax; jz 2f; .byte 0xFF, 0x25; .long %c3; .byte %c2, 0xE9; 2:")

#define OBFH_CFLOW_TEMPLATE_70 \
    OBFH_CFLOW_LOCAL_ASM("movl %%esp, %%edx; addl %0, %%edx; " OBFH_CFLOW_PRED_FOUR_PRODUCT("%%edx", "%%ecx", "%%eax") "  testl $7, %%edx; jz 1f; " OBFH_CFLOW_PAYLOAD_CALL_DATA " 1: movl %%esp, %%eax; addl %0, %%eax; " OBFH_CFLOW_PRED_SQUARE_PLUS("%%eax", "%%ecx") "  testl $1, %%eax; jz 2f; .byte 0xFF, 0x25; .long %c3; .byte %c2, 0xE9; 2:")

#define OBFH_CFLOW_TEMPLATE_71 \
    OBFH_CFLOW_LOCAL_ASM("movl %%esp, %%edx; addl %0, %%edx; " OBFH_CFLOW_PRED_SQUARE_PRODUCT("%%edx", "%%ecx") "  testl $3, %%edx; jz 1f; " OBFH_CFLOW_PAYLOAD_CALL_DATA " 1: movl %%esp, %%eax; addl %0, %%eax; " OBFH_CFLOW_PRED_CUBE_PLUS("%%eax", "%%ecx") "  testl $1, %%eax; jz 2f; .byte 0xFF, 0x25; .long %c3; .byte %c2, 0xE9; 2:")

#define OBFH_CFLOW_TEMPLATE_72 \
    OBFH_CFLOW_LOCAL_ASM("movl %%esp, %%ecx; addl %0, %%ecx; " OBFH_CFLOW_PRED_FOURTH("%%ecx") "  testl $14, %%ecx; jz 1f; " OBFH_CFLOW_PAYLOAD_CALL_DATA " 1: movl %%esp, %%eax; addl %0, %%eax; " OBFH_CFLOW_PRED_ODD_SQUARE("%%eax") "  testl $2, %%eax; jz 2f; .byte 0xFF, 0x25; .long %c3; .byte %c2, 0xE9; 2:")

#define OBFH_CFLOW_TEMPLATE_73 \
    OBFH_CFLOW_LOCAL_ASM("movl %%esp, %%ecx; addl %0, %%ecx; " OBFH_CFLOW_PRED_FIFTH_MINUS("%%ecx", "%%eax") "  testl $1, %%ecx; jz 1f; " OBFH_CFLOW_PAYLOAD_CALL_DATA " 1: movl %%esp, %%eax; addl %0, %%eax; " OBFH_CFLOW_PRED_ADJACENT_PRODUCT("%%eax", "%%edx") "  testl $1, %%eax; jz 2f; .byte 0xFF, 0x25; .long %c3; .byte %c2, 0xE9; 2:")

#define OBFH_CFLOW_TEMPLATE_74 \
    OBFH_CFLOW_LOCAL_ASM("movl %%esp, %%ecx; addl %0, %%ecx; " OBFH_CFLOW_PRED_ADJACENT_OR("%%ecx", "%%eax") "  testl $1, %%ecx; jnz 1f; " OBFH_CFLOW_PAYLOAD_CALL_DATA " 1: movl %%esp, %%eax; addl %0, %%eax; " OBFH_CFLOW_PRED_SQUARE("%%eax") "  testl $2, %%eax; jz 2f; .byte 0xFF, 0x25; .long %c3; .byte %c2, 0xE9; 2:")

#define OBFH_CFLOW_TEMPLATE_75 \
    OBFH_CFLOW_LOCAL_ASM("movl %%esp, %%ecx; addl %0, %%ecx; " OBFH_CFLOW_PRED_ADJACENT_AND("%%ecx", "%%eax") "  testl $1, %%ecx; jz 1f; " OBFH_CFLOW_PAYLOAD_CALL_DATA " 1: movl %%esp, %%eax; addl %0, %%eax; " OBFH_CFLOW_PRED_SQUARE_XOR("%%eax", "%%edx") "  testl $1, %%eax; jz 2f; .byte 0xFF, 0x25; .long %c3; .byte %c2, 0xE9; 2:")

#define OBFH_CFLOW_TEMPLATE_76 \
    OBFH_CFLOW_LOCAL_ASM("movl %%esp, %%eax; addl %0, %%eax; " OBFH_CFLOW_PRED_PREVIOUS_PRODUCT("%%eax", "%%ecx") "  testl $1, %%eax; jz 1f; " OBFH_CFLOW_PAYLOAD_CALL_DATA " 1: movl %%esp, %%eax; addl %0, %%eax; " OBFH_CFLOW_PRED_ODD_PRODUCT("%%eax", "%%edx") "  testl $1, %%eax; jnz 2f; .byte 0xFF, 0x25; .long %c3; .byte %c2, 0xE9; 2:")

#define OBFH_CFLOW_TEMPLATE_77 \
    OBFH_CFLOW_LOCAL_ASM("movl %%esp, %%eax; addl %0, %%eax; " OBFH_CFLOW_PRED_SQUARE_PLUS("%%eax", "%%ecx") "  testl $1, %%eax; jz 1f; " OBFH_CFLOW_PAYLOAD_CALL_DATA " 1: movl %%esp, %%edx; addl %0, %%edx; " OBFH_CFLOW_PRED_CUBE_MINUS("%%edx", "%%ecx") "  testl $1, %%edx; jz 2f; .byte 0xFF, 0x25; .long %c3; .byte %c2, 0xE9; 2:")

#define OBFH_CFLOW_TEMPLATE_78 \
    OBFH_CFLOW_LOCAL_ASM("movl %%esp, %%eax; addl %0, %%eax; " OBFH_CFLOW_PRED_CUBE_PLUS("%%eax", "%%ecx") "  testl $1, %%eax; jz 1f; " OBFH_CFLOW_PAYLOAD_CALL_DATA " 1: movl %%esp, %%edx; addl %0, %%edx; " OBFH_CFLOW_PRED_THREE_PRODUCT("%%edx", "%%ecx", "%%eax") "  testl $1, %%edx; jz 2f; .byte 0xFF, 0x25; .long %c3; .byte %c2, 0xE9; 2:")

#define OBFH_CFLOW_TEMPLATE_79 \
    OBFH_CFLOW_LOCAL_ASM("movl %%esp, %%eax; addl %0, %%eax; " OBFH_CFLOW_PRED_ODD_SQUARE("%%eax") "  testl $2, %%eax; jz 1f; " OBFH_CFLOW_PAYLOAD_CALL_DATA " 1: movl %%esp, %%edx; addl %0, %%edx; " OBFH_CFLOW_PRED_FOUR_PRODUCT("%%edx", "%%ecx", "%%eax") "  testl $7, %%edx; jz 2f; .byte 0xFF, 0x25; .long %c3; .byte %c2, 0xE9; 2:")

#define OBFH_CFLOW_TEMPLATE_80 \
    OBFH_CFLOW_LOCAL_ASM("movl %%esp, %%eax; xorl %0, %%eax; " OBFH_CFLOW_PRED_ADJACENT_PRODUCT("%%eax", "%%edx") "  testl $1, %%eax; jnz 1f; jmp 2f; 1: " OBFH_CFLOW_PAYLOAD_CALL_DATA " 2: movl %%esp, %%edx; addl %0, %%edx; " OBFH_CFLOW_PRED_SQUARE_PRODUCT("%%edx", "%%ecx") "  testl $3, %%edx; jz 3f; " OBFH_CFLOW_PAYLOAD_TRAP_INDIRECT " 3:")

#define OBFH_CFLOW_TEMPLATE_81 \
    OBFH_CFLOW_LOCAL_ASM("movl %%esp, %%eax; xorl %0, %%eax; " OBFH_CFLOW_PRED_SQUARE("%%eax") "  testl $2, %%eax; jnz 1f; jmp 2f; 1: " OBFH_CFLOW_PAYLOAD_CALL_DATA " 2: movl %%esp, %%ecx; addl %0, %%ecx; " OBFH_CFLOW_PRED_FOURTH("%%ecx") "  testl $14, %%ecx; jz 3f; " OBFH_CFLOW_PAYLOAD_TRAP_INDIRECT " 3:")

#define OBFH_CFLOW_TEMPLATE_82 \
    OBFH_CFLOW_LOCAL_ASM("movl %%esp, %%eax; xorl %0, %%eax; " OBFH_CFLOW_PRED_SQUARE_XOR("%%eax", "%%edx") "  testl $1, %%eax; jnz 1f; jmp 2f; 1: " OBFH_CFLOW_PAYLOAD_CALL_DATA " 2: movl %%esp, %%ecx; addl %0, %%ecx; " OBFH_CFLOW_PRED_FIFTH_MINUS("%%ecx", "%%eax") "  testl $1, %%ecx; jz 3f; " OBFH_CFLOW_PAYLOAD_TRAP_INDIRECT " 3:")

#define OBFH_CFLOW_TEMPLATE_83 \
    OBFH_CFLOW_LOCAL_ASM("movl %%esp, %%eax; xorl %0, %%eax; " OBFH_CFLOW_PRED_ODD_PRODUCT("%%eax", "%%edx") "  testl $1, %%eax; jz 1f; jmp 2f; 1: " OBFH_CFLOW_PAYLOAD_CALL_DATA " 2: movl %%esp, %%ecx; addl %0, %%ecx; " OBFH_CFLOW_PRED_ADJACENT_OR("%%ecx", "%%eax") "  testl $1, %%ecx; jnz 3f; " OBFH_CFLOW_PAYLOAD_TRAP_INDIRECT " 3:")

#define OBFH_CFLOW_TEMPLATE_84 \
    OBFH_CFLOW_LOCAL_ASM("movl %%esp, %%edx; xorl %0, %%edx; " OBFH_CFLOW_PRED_CUBE_MINUS("%%edx", "%%ecx") "  testl $1, %%edx; jnz 1f; jmp 2f; 1: " OBFH_CFLOW_PAYLOAD_CALL_DATA " 2: movl %%esp, %%ecx; addl %0, %%ecx; " OBFH_CFLOW_PRED_ADJACENT_AND("%%ecx", "%%eax") "  testl $1, %%ecx; jz 3f; " OBFH_CFLOW_PAYLOAD_TRAP_INDIRECT " 3:")

#define OBFH_CFLOW_TEMPLATE_85 \
    OBFH_CFLOW_LOCAL_ASM("movl %%esp, %%edx; xorl %0, %%edx; " OBFH_CFLOW_PRED_THREE_PRODUCT("%%edx", "%%ecx", "%%eax") "  testl $1, %%edx; jnz 1f; jmp 2f; 1: " OBFH_CFLOW_PAYLOAD_CALL_DATA " 2: movl %%esp, %%eax; addl %0, %%eax; " OBFH_CFLOW_PRED_PREVIOUS_PRODUCT("%%eax", "%%ecx") "  testl $1, %%eax; jz 3f; " OBFH_CFLOW_PAYLOAD_TRAP_INDIRECT " 3:")

#define OBFH_CFLOW_TEMPLATE_86 \
    OBFH_CFLOW_LOCAL_JUNK_ASM("xorl %%eax, %%eax; jz 1f; .byte 0xE8; " OBFH_JUNK_PAYLOAD " 1: cpuid;")

#define OBFH_CFLOW_TEMPLATE_87 ({                                                                                           \
    enum { __obfh_break_salt = RND(1, 32767),                                                                               \
           __obfh_break_gap = RND(1, 255) };                                                                                \
    __obfh_asm__("movzbl %7, %%eax; xorl %8, %%eax; cmpl %9, %%eax; jne 1f; .byte 0x00, 0xE8; " OBFH_JUNK_PAYLOAD " 1:"     \
                 :                                                                                                          \
                 : OBFH_CFLOW_LOCAL_JUNK_INPUTS, "m"(_0), "i"(__obfh_break_salt), "i"(__obfh_break_salt + __obfh_break_gap) \
                 : OBFH_CPUID_CLOBBERS);                                                                                    \
})

#define OBFH_CFLOW_TEMPLATE_88 ({                                                                                                                                          \
    enum { __obfh_break_salt = RND(1, 32767),                                                                                                                              \
           __obfh_break_case1 = RND(129, 191),                                                                                                                             \
           __obfh_break_case2 = RND(257, 319) };                                                                                                                           \
    __obfh_asm__("movzbl %7, %%eax; addl %8, %%eax; cmpl %9, %%eax; jne 1f; .byte 0x00, 0x00; " OBFH_JUNK_PAYLOAD                                                          \
                 " 1: cmpl %10, %%eax; jne 2f; .byte 0xFF, 0x25; " OBFH_JUNK_PAYLOAD " 2:"                                                                                 \
                 :                                                                                                                                                         \
                 : OBFH_CFLOW_LOCAL_JUNK_INPUTS, "m"(_0), "i"(__obfh_break_salt), "i"(__obfh_break_salt + __obfh_break_case1), "i"(__obfh_break_salt + __obfh_break_case2) \
                 : OBFH_CPUID_CLOBBERS);                                                                                                                                   \
})

#define OBFH_CFLOW_TEMPLATE_89 \
    OBFH_CFLOW_LOCAL_JUNK_ASM("xorl %%ebx, %%ebx; xorl %%edx, %%edx; xorl %%ebx, %%edx; jz 1f; mov $4, %%eax; .byte 0x00; " OBFH_JUNK_PAYLOAD " 1: cpuid;")

#define OBFH_CFLOW_TEMPLATE_90 \
    OBFH_CFLOW_LOCAL_JUNK_ASM("xorl %%ebx, %%ebx; xorl %%eax, %%eax; mov %%eax, %%ebx; mov %%edx, %%ebx; xorl %%ebx, %%edx; jz 1f; .byte 0x20; " OBFH_JUNK_PAYLOAD " 1: cpuid;")

#define OBFH_CFLOW_TEMPLATE_91 \
    OBFH_CFLOW_LOCAL_JUNK_ASM("xorl %%edx, %%edx; xorl %%eax, %%eax; mov %%eax, %%edx; jz 1f; .byte 0xE8; " OBFH_JUNK_PAYLOAD " 1: cpuid;")

#define OBFH_CFLOW_TEMPLATE_92 \
    OBFH_CFLOW_LOCAL_JUNK_ASM("xorl %%edx, %%edx; jz 1f; .byte 0xE8; " OBFH_JUNK_PAYLOAD " 1: cpuid;")

#define OBFH_CFLOW_TEMPLATE_93 \
    OBFH_CFLOW_LOCAL_JUNK_ASM("xorl %%eax, %%eax; jz 1f; .byte 0x50; " OBFH_JUNK_PAYLOAD " 1: cpuid;")

#define OBFH_CFLOW_TEMPLATE_94 \
    OBFH_CFLOW_LOCAL_JUNK_ASM("xorl %%edx, %%edx; jz 1f; .byte 0x00, 0x00; " OBFH_JUNK_PAYLOAD " 1: cpuid;")

#define OBFH_CFLOW_TEMPLATE_95 \
    OBFH_CFLOW_LOCAL_STACK_ASM("movl %%esp, %%eax; addl %0, %%eax; leal 1(%%eax), %%edx; imull %%edx, %%eax; testl $1, %%eax; jz 1f; .byte 0xE8; " OBFH_STACK_JUNK_PAYLOAD " 1:", "eax", "edx")

#define OBFH_CFLOW_TEMPLATE_96 \
    OBFH_CFLOW_LOCAL_STACK_ASM("movl %%esp, %%edx; addl %0, %%edx; imull %%edx, %%edx; testl $2, %%edx; jz 1f; .byte 0xFF, 0x25; " OBFH_STACK_JUNK_PAYLOAD " 1:", "edx")

#define OBFH_CFLOW_TEMPLATE_97 \
    OBFH_CFLOW_LOCAL_STACK_ASM("movl %%esp, %%eax; addl %0, %%eax; movl %%eax, %%edx; imull %%eax, %%eax; xorl %%edx, %%eax; testl $1, %%eax; jz 1f; .byte 0x0F, 0x0B, 0xE8; " OBFH_STACK_JUNK_PAYLOAD " 1:", "eax", "edx")

#define OBFH_CFLOW_TEMPLATE_98 \
    OBFH_CFLOW_LOCAL_STACK_ASM("movl %%esp, %%eax; addl %0, %%eax; leal 1(%%eax), %%edx; imull %%edx, %%eax; addl $1, %%eax; testl $1, %%eax; jnz 1f; .byte 0xC3, 0xE8; " OBFH_STACK_JUNK_PAYLOAD " 1:", "eax", "edx")

#define OBFH_CFLOW_TEMPLATE_99 \
    OBFH_CFLOW_LOCAL_NAMED_ASM(OBFH_CFLOW_PRED_ADD_CARRY "je 1f;" OBFH_CFLOW_DATA_CALL "1:" OBFH_CFLOW_PRED_ROTATE_XOR "je 2f;" OBFH_CFLOW_DATA_INDIRECT "2:")

#define OBFH_CFLOW_TEMPLATE_100 \
    OBFH_CFLOW_LOCAL_NAMED_ASM(OBFH_CFLOW_PRED_OR_AND_SUM "jne 1f; jmp 2f; 1:" OBFH_CFLOW_DATA_STACK "2:" OBFH_CFLOW_PRED_ROTATE_RESTORE "sete %%cl; movzbl %%cl, %%ecx; testl %%ecx, %%ecx; jnz 3f;" OBFH_CFLOW_DATA_TRAP "3:")

#define OBFH_CFLOW_TEMPLATE_101 \
    OBFH_CFLOW_LOCAL_NAMED_ASM(OBFH_CFLOW_PRED_SUB_BORROW "sete %%cl; movzbl %%cl, %%ecx; negl %%ecx; testl %%ecx, %%ecx; js 1f;" OBFH_CFLOW_DATA_FRAME "1:" OBFH_CFLOW_PRED_BSWAP_XOR "jne 2f; jmp 3f; 2:" OBFH_CFLOW_DATA_RETURN "3:")

#define OBFH_CFLOW_TEMPLATE_102 \
    OBFH_CFLOW_LOCAL_NAMED_ASM(OBFH_CFLOW_PRED_DEMORGAN "je 2f; 1:" OBFH_CFLOW_DATA_INDIRECT "2:" OBFH_CFLOW_PRED_WORD_PARTITION "jne 1b; jmp 3f;" OBFH_CFLOW_DATA_CALL "3:")

#define OBFH_CFLOW_TEMPLATE_103 \
    OBFH_CFLOW_LOCAL_NAMED_ASM(OBFH_CFLOW_PRED_OR_DISTRIBUTE "je 1f;" OBFH_CFLOW_DATA_TRAP "1:" OBFH_CFLOW_PRED_BYTE_PARITY "je 2f;" OBFH_CFLOW_DATA_STACK "2:")

#define OBFH_CFLOW_TEMPLATE_104 \
    OBFH_CFLOW_LOCAL_NAMED_ASM(OBFH_CFLOW_PRED_AND_PARTITION "jne 1f; jmp 2f; 1:" OBFH_CFLOW_DATA_RETURN "2:" OBFH_CFLOW_PRED_BYTE_ROTATE "sete %%cl; movzbl %%cl, %%ecx; testl %%ecx, %%ecx; jnz 3f;" OBFH_CFLOW_DATA_FRAME "3:")

#define OBFH_CFLOW_TEMPLATE_105 \
    OBFH_CFLOW_LOCAL_NAMED_ASM(OBFH_CFLOW_PRED_XOR_CANCEL "sete %%cl; movzbl %%cl, %%ecx; negl %%ecx; testl %%ecx, %%ecx; js 1f;" OBFH_CFLOW_DATA_CALL "1:" OBFH_CFLOW_PRED_WORD_ROTATE "jne 2f; jmp 3f; 2:" OBFH_CFLOW_DATA_INDIRECT "3:")

#define OBFH_CFLOW_TEMPLATE_106 \
    OBFH_CFLOW_LOCAL_NAMED_ASM(OBFH_CFLOW_PRED_COMPLEMENT_CARRY "jnc 2f; 1:" OBFH_CFLOW_DATA_STACK "2:" OBFH_CFLOW_PRED_NEG_COMPLEMENT "jne 1b; jmp 3f;" OBFH_CFLOW_DATA_TRAP "3:")

#define OBFH_CFLOW_TEMPLATE_107 \
    OBFH_CFLOW_LOCAL_NAMED_ASM(OBFH_CFLOW_PRED_COMPLEMENT_WRAP "jc 1f;" OBFH_CFLOW_DATA_FRAME "1:" OBFH_CFLOW_PRED_MUL_DISTRIBUTE "je 2f;" OBFH_CFLOW_DATA_RETURN "2:")

#define OBFH_CFLOW_TEMPLATE_108 \
    OBFH_CFLOW_LOCAL_NAMED_ASM(OBFH_CFLOW_PRED_MASK_SUBTRACT "jc 1f; jmp 2f; 1:" OBFH_CFLOW_DATA_INDIRECT "2:" OBFH_CFLOW_PRED_SQUARE_EXPAND "sete %%cl; movzbl %%cl, %%ecx; testl %%ecx, %%ecx; jnz 3f;" OBFH_CFLOW_DATA_CALL "3:")

#define OBFH_CFLOW_TEMPLATE_109 \
    OBFH_CFLOW_LOCAL_NAMED_ASM(OBFH_CFLOW_PRED_MASK_OR_ORDER "setae %%cl; movzbl %%cl, %%ecx; negl %%ecx; testl %%ecx, %%ecx; js 1f;" OBFH_CFLOW_DATA_TRAP "1:" OBFH_CFLOW_PRED_NEG_SQUARE "jne 2f; jmp 3f; 2:" OBFH_CFLOW_DATA_STACK "3:")

#define OBFH_CFLOW_TEMPLATE_110 \
    OBFH_CFLOW_LOCAL_NAMED_ASM(OBFH_CFLOW_PRED_ROTATE_XOR "je 2f; 1:" OBFH_CFLOW_DATA_RETURN "2:" OBFH_CFLOW_PRED_BSWAP_NOT "jne 1b; jmp 3f;" OBFH_CFLOW_DATA_FRAME "3:")

#define OBFH_CFLOW_TEMPLATE_111 \
    OBFH_CFLOW_LOCAL_NAMED_ASM(OBFH_CFLOW_PRED_ROTATE_RESTORE "je 1f;" OBFH_CFLOW_DATA_CALL "1:" OBFH_CFLOW_PRED_SUB_MASK "je 2f;" OBFH_CFLOW_DATA_INDIRECT "2:")

#define OBFH_CFLOW_TEMPLATE_112 \
    OBFH_CFLOW_LOCAL_NAMED_ASM(OBFH_CFLOW_PRED_BSWAP_XOR "jne 1f; jmp 2f; 1:" OBFH_CFLOW_DATA_STACK "2:" OBFH_CFLOW_PRED_MASK_ORDER "setbe %%cl; movzbl %%cl, %%ecx; testl %%ecx, %%ecx; jnz 3f;" OBFH_CFLOW_DATA_TRAP "3:")

#define OBFH_CFLOW_TEMPLATE_113 \
    OBFH_CFLOW_LOCAL_NAMED_ASM(OBFH_CFLOW_PRED_WORD_PARTITION "sete %%cl; movzbl %%cl, %%ecx; negl %%ecx; testl %%ecx, %%ecx; js 1f;" OBFH_CFLOW_DATA_FRAME "1:" OBFH_CFLOW_PRED_COMPLEMENT_TRANSLATE "jne 2f; jmp 3f; 2:" OBFH_CFLOW_DATA_RETURN "3:")

#define OBFH_CFLOW_TEMPLATE_114 \
    OBFH_CFLOW_LOCAL_NAMED_ASM(OBFH_CFLOW_PRED_BYTE_PARITY "je 2f; 1:" OBFH_CFLOW_DATA_INDIRECT "2:" OBFH_CFLOW_PRED_SQUARE_RESIDUE "jne 1b; jmp 3f;" OBFH_CFLOW_DATA_CALL "3:")

#define OBFH_CFLOW_TEMPLATE_115 \
    OBFH_CFLOW_LOCAL_NAMED_ASM(OBFH_CFLOW_PRED_BYTE_ROTATE "je 1f;" OBFH_CFLOW_DATA_TRAP "1:" OBFH_CFLOW_PRED_FOURTH_RESIDUE "je 2f;" OBFH_CFLOW_DATA_STACK "2:")

#define OBFH_CFLOW_TEMPLATE_116 \
    OBFH_CFLOW_LOCAL_NAMED_ASM(OBFH_CFLOW_PRED_WORD_ROTATE "jne 1f; jmp 2f; 1:" OBFH_CFLOW_DATA_RETURN "2:" OBFH_CFLOW_PRED_ISOLATE_BIT "sete %%cl; movzbl %%cl, %%ecx; testl %%ecx, %%ecx; jnz 3f;" OBFH_CFLOW_DATA_FRAME "3:")

#define OBFH_CFLOW_TEMPLATE_117 \
    OBFH_CFLOW_LOCAL_NAMED_ASM(OBFH_CFLOW_PRED_NEG_COMPLEMENT "sete %%cl; movzbl %%cl, %%ecx; negl %%ecx; testl %%ecx, %%ecx; js 1f;" OBFH_CFLOW_DATA_CALL "1:" OBFH_CFLOW_PRED_ADD_CARRY "jne 2f; jmp 3f; 2:" OBFH_CFLOW_DATA_INDIRECT "3:")

#define OBFH_CFLOW_TEMPLATE_118 \
    OBFH_CFLOW_LOCAL_NAMED_ASM(OBFH_CFLOW_PRED_MUL_DISTRIBUTE "je 2f; 1:" OBFH_CFLOW_DATA_STACK "2:" OBFH_CFLOW_PRED_OR_AND_SUM "jne 1b; jmp 3f;" OBFH_CFLOW_DATA_TRAP "3:")

#define OBFH_CFLOW_TEMPLATE_119 \
    OBFH_CFLOW_LOCAL_NAMED_ASM(OBFH_CFLOW_PRED_SQUARE_EXPAND "je 1f;" OBFH_CFLOW_DATA_FRAME "1:" OBFH_CFLOW_PRED_SUB_BORROW "je 2f;" OBFH_CFLOW_DATA_RETURN "2:")

#define OBFH_CFLOW_TEMPLATE_120 \
    OBFH_CFLOW_LOCAL_NAMED_ASM(OBFH_CFLOW_PRED_NEG_SQUARE "jne 1f; jmp 2f; 1:" OBFH_CFLOW_DATA_INDIRECT "2:" OBFH_CFLOW_PRED_DEMORGAN "sete %%cl; movzbl %%cl, %%ecx; testl %%ecx, %%ecx; jnz 3f;" OBFH_CFLOW_DATA_CALL "3:")

#define OBFH_CFLOW_TEMPLATE_121 \
    OBFH_CFLOW_LOCAL_NAMED_ASM(OBFH_CFLOW_PRED_BSWAP_NOT "sete %%cl; movzbl %%cl, %%ecx; negl %%ecx; testl %%ecx, %%ecx; js 1f;" OBFH_CFLOW_DATA_TRAP "1:" OBFH_CFLOW_PRED_OR_DISTRIBUTE "jne 2f; jmp 3f; 2:" OBFH_CFLOW_DATA_STACK "3:")

#define OBFH_CFLOW_TEMPLATE_122 \
    OBFH_CFLOW_LOCAL_NAMED_ASM(OBFH_CFLOW_PRED_SUB_MASK "je 2f; 1:" OBFH_CFLOW_DATA_RETURN "2:" OBFH_CFLOW_PRED_AND_PARTITION "jne 1b; jmp 3f;" OBFH_CFLOW_DATA_FRAME "3:")

#define OBFH_CFLOW_TEMPLATE_123 \
    OBFH_CFLOW_LOCAL_NAMED_ASM(OBFH_CFLOW_PRED_MASK_ORDER "jbe 1f;" OBFH_CFLOW_DATA_CALL "1:" OBFH_CFLOW_PRED_XOR_CANCEL "je 2f;" OBFH_CFLOW_DATA_INDIRECT "2:")

#define OBFH_CFLOW_TEMPLATE_124 \
    OBFH_CFLOW_LOCAL_NAMED_ASM(OBFH_CFLOW_PRED_COMPLEMENT_TRANSLATE "jne 1f; jmp 2f; 1:" OBFH_CFLOW_DATA_STACK "2:" OBFH_CFLOW_PRED_COMPLEMENT_CARRY "setnc %%cl; movzbl %%cl, %%ecx; testl %%ecx, %%ecx; jnz 3f;" OBFH_CFLOW_DATA_TRAP "3:")

#define OBFH_CFLOW_TEMPLATE_125 \
    OBFH_CFLOW_LOCAL_NAMED_ASM(OBFH_CFLOW_PRED_SQUARE_RESIDUE "sete %%cl; movzbl %%cl, %%ecx; negl %%ecx; testl %%ecx, %%ecx; js 1f;" OBFH_CFLOW_DATA_FRAME "1:" OBFH_CFLOW_PRED_COMPLEMENT_WRAP "jnc 2f; jmp 3f; 2:" OBFH_CFLOW_DATA_RETURN "3:")

#define OBFH_CFLOW_TEMPLATE_126 \
    OBFH_CFLOW_LOCAL_NAMED_ASM(OBFH_CFLOW_PRED_FOURTH_RESIDUE "je 2f; 1:" OBFH_CFLOW_DATA_INDIRECT "2:" OBFH_CFLOW_PRED_MASK_SUBTRACT "jc 1b; jmp 3f;" OBFH_CFLOW_DATA_CALL "3:")

#define OBFH_CFLOW_TEMPLATE_127 \
    OBFH_CFLOW_LOCAL_NAMED_ASM(OBFH_CFLOW_PRED_ISOLATE_BIT "je 1f;" OBFH_CFLOW_DATA_TRAP "1:" OBFH_CFLOW_PRED_MASK_OR_ORDER "jae 2f;" OBFH_CFLOW_DATA_STACK "2:")

// Direct templates share the same parameter capture as automatic selection.
#define BREAK_STACK_CFLOW_0 ({ OBFH_CFLOW_CAPTURE; OBFH_CFLOW_TEMPLATE_0; })
#define BREAK_STACK_CFLOW_1 ({ OBFH_CFLOW_CAPTURE; OBFH_CFLOW_TEMPLATE_1; })
#define BREAK_STACK_CFLOW_2 ({ OBFH_CFLOW_CAPTURE; OBFH_CFLOW_TEMPLATE_2; })
#define BREAK_STACK_CFLOW_3 ({ OBFH_CFLOW_CAPTURE; OBFH_CFLOW_TEMPLATE_3; })
#define BREAK_STACK_CFLOW_4 ({ OBFH_CFLOW_CAPTURE; OBFH_CFLOW_TEMPLATE_4; })
#define BREAK_STACK_CFLOW_5 ({ OBFH_CFLOW_CAPTURE; OBFH_CFLOW_TEMPLATE_5; })
#define BREAK_STACK_CFLOW_6 ({ OBFH_CFLOW_CAPTURE; OBFH_CFLOW_TEMPLATE_6; })
#define BREAK_STACK_CFLOW_7 ({ OBFH_CFLOW_CAPTURE; OBFH_CFLOW_TEMPLATE_7; })
#define BREAK_STACK_CFLOW_8 ({ OBFH_CFLOW_CAPTURE; OBFH_CFLOW_TEMPLATE_8; })
#define BREAK_STACK_CFLOW_9 ({ OBFH_CFLOW_CAPTURE; OBFH_CFLOW_TEMPLATE_9; })
#define BREAK_STACK_CFLOW_10 ({ OBFH_CFLOW_CAPTURE; OBFH_CFLOW_TEMPLATE_10; })
#define BREAK_STACK_CFLOW_11 ({ OBFH_CFLOW_CAPTURE; OBFH_CFLOW_TEMPLATE_11; })
#define BREAK_STACK_CFLOW_12 ({ OBFH_CFLOW_CAPTURE; OBFH_CFLOW_TEMPLATE_12; })
#define BREAK_STACK_CFLOW_13 ({ OBFH_CFLOW_CAPTURE; OBFH_CFLOW_TEMPLATE_13; })
#define BREAK_STACK_CFLOW_14 ({ OBFH_CFLOW_CAPTURE; OBFH_CFLOW_TEMPLATE_14; })
#define BREAK_STACK_CFLOW_15 ({ OBFH_CFLOW_CAPTURE; OBFH_CFLOW_TEMPLATE_15; })
#define BREAK_STACK_CFLOW_16 ({ OBFH_CFLOW_CAPTURE; OBFH_CFLOW_TEMPLATE_16; })
#define BREAK_STACK_CFLOW_17 ({ OBFH_CFLOW_CAPTURE; OBFH_CFLOW_TEMPLATE_17; })
#define BREAK_STACK_CFLOW_18 ({ OBFH_CFLOW_CAPTURE; OBFH_CFLOW_TEMPLATE_18; })
#define BREAK_STACK_CFLOW_19 ({ OBFH_CFLOW_CAPTURE; OBFH_CFLOW_TEMPLATE_19; })
#define BREAK_STACK_CFLOW_20 ({ OBFH_CFLOW_CAPTURE; OBFH_CFLOW_TEMPLATE_20; })
#define BREAK_STACK_CFLOW_21 ({ OBFH_CFLOW_CAPTURE; OBFH_CFLOW_TEMPLATE_21; })
#define BREAK_STACK_CFLOW_22 ({ OBFH_CFLOW_CAPTURE; OBFH_CFLOW_TEMPLATE_22; })
#define BREAK_STACK_CFLOW_23 ({ OBFH_CFLOW_CAPTURE; OBFH_CFLOW_TEMPLATE_23; })
#define BREAK_STACK_CFLOW_24 ({ OBFH_CFLOW_CAPTURE; OBFH_CFLOW_TEMPLATE_24; })
#define BREAK_STACK_CFLOW_25 ({ OBFH_CFLOW_CAPTURE; OBFH_CFLOW_TEMPLATE_25; })
#define BREAK_STACK_CFLOW_26 ({ OBFH_CFLOW_CAPTURE; OBFH_CFLOW_TEMPLATE_26; })
#define BREAK_STACK_CFLOW_27 ({ OBFH_CFLOW_CAPTURE; OBFH_CFLOW_TEMPLATE_27; })
#define BREAK_STACK_CFLOW_28 ({ OBFH_CFLOW_CAPTURE; OBFH_CFLOW_TEMPLATE_28; })
#define BREAK_STACK_CFLOW_29 ({ OBFH_CFLOW_CAPTURE; OBFH_CFLOW_TEMPLATE_29; })
#define BREAK_STACK_CFLOW_30 ({ OBFH_CFLOW_CAPTURE; OBFH_CFLOW_TEMPLATE_30; })
#define BREAK_STACK_CFLOW_31 ({ OBFH_CFLOW_CAPTURE; OBFH_CFLOW_TEMPLATE_31; })
#define BREAK_STACK_CFLOW_32 ({ OBFH_CFLOW_CAPTURE; OBFH_CFLOW_TEMPLATE_32; })
#define BREAK_STACK_CFLOW_33 ({ OBFH_CFLOW_CAPTURE; OBFH_CFLOW_TEMPLATE_33; })
#define BREAK_STACK_CFLOW_34 ({ OBFH_CFLOW_CAPTURE; OBFH_CFLOW_TEMPLATE_34; })
#define BREAK_STACK_CFLOW_35 ({ OBFH_CFLOW_CAPTURE; OBFH_CFLOW_TEMPLATE_35; })
#define BREAK_STACK_CFLOW_36 ({ OBFH_CFLOW_CAPTURE; OBFH_CFLOW_TEMPLATE_36; })
#define BREAK_STACK_CFLOW_37 ({ OBFH_CFLOW_CAPTURE; OBFH_CFLOW_TEMPLATE_37; })
#define BREAK_STACK_CFLOW_38 ({ OBFH_CFLOW_CAPTURE; OBFH_CFLOW_TEMPLATE_38; })
#define BREAK_STACK_CFLOW_39 ({ OBFH_CFLOW_CAPTURE; OBFH_CFLOW_TEMPLATE_39; })
#define BREAK_STACK_CFLOW_40 ({ OBFH_CFLOW_CAPTURE; OBFH_CFLOW_TEMPLATE_40; })
#define BREAK_STACK_CFLOW_41 ({ OBFH_CFLOW_CAPTURE; OBFH_CFLOW_TEMPLATE_41; })
#define BREAK_STACK_CFLOW_42 ({ OBFH_CFLOW_CAPTURE; OBFH_CFLOW_TEMPLATE_42; })
#define BREAK_STACK_CFLOW_43 ({ OBFH_CFLOW_CAPTURE; OBFH_CFLOW_TEMPLATE_43; })
#define BREAK_STACK_CFLOW_44 ({ OBFH_CFLOW_CAPTURE; OBFH_CFLOW_TEMPLATE_44; })
#define BREAK_STACK_CFLOW_45 ({ OBFH_CFLOW_CAPTURE; OBFH_CFLOW_TEMPLATE_45; })
#define BREAK_STACK_CFLOW_46 ({ OBFH_CFLOW_CAPTURE; OBFH_CFLOW_TEMPLATE_46; })
#define BREAK_STACK_CFLOW_47 ({ OBFH_CFLOW_CAPTURE; OBFH_CFLOW_TEMPLATE_47; })
#define BREAK_STACK_CFLOW_48 ({ OBFH_CFLOW_CAPTURE; OBFH_CFLOW_TEMPLATE_48; })
#define BREAK_STACK_CFLOW_49 ({ OBFH_CFLOW_CAPTURE; OBFH_CFLOW_TEMPLATE_49; })
#define BREAK_STACK_CFLOW_50 ({ OBFH_CFLOW_CAPTURE; OBFH_CFLOW_TEMPLATE_50; })
#define BREAK_STACK_CFLOW_51 ({ OBFH_CFLOW_CAPTURE; OBFH_CFLOW_TEMPLATE_51; })
#define BREAK_STACK_CFLOW_52 ({ OBFH_CFLOW_CAPTURE; OBFH_CFLOW_TEMPLATE_52; })
#define BREAK_STACK_CFLOW_53 ({ OBFH_CFLOW_CAPTURE; OBFH_CFLOW_TEMPLATE_53; })
#define BREAK_STACK_CFLOW_54 ({ OBFH_CFLOW_CAPTURE; OBFH_CFLOW_TEMPLATE_54; })
#define BREAK_STACK_CFLOW_55 ({ OBFH_CFLOW_CAPTURE; OBFH_CFLOW_TEMPLATE_55; })
#define BREAK_STACK_CFLOW_56 ({ OBFH_CFLOW_CAPTURE; OBFH_CFLOW_TEMPLATE_56; })
#define BREAK_STACK_CFLOW_57 ({ OBFH_CFLOW_CAPTURE; OBFH_CFLOW_TEMPLATE_57; })
#define BREAK_STACK_CFLOW_58 ({ OBFH_CFLOW_CAPTURE; OBFH_CFLOW_TEMPLATE_58; })
#define BREAK_STACK_CFLOW_59 ({ OBFH_CFLOW_CAPTURE; OBFH_CFLOW_TEMPLATE_59; })
#define BREAK_STACK_CFLOW_60 ({ OBFH_CFLOW_CAPTURE; OBFH_CFLOW_TEMPLATE_60; })
#define BREAK_STACK_CFLOW_61 ({ OBFH_CFLOW_CAPTURE; OBFH_CFLOW_TEMPLATE_61; })
#define BREAK_STACK_CFLOW_62 ({ OBFH_CFLOW_CAPTURE; OBFH_CFLOW_TEMPLATE_62; })
#define BREAK_STACK_CFLOW_63 ({ OBFH_CFLOW_CAPTURE; OBFH_CFLOW_TEMPLATE_63; })
#define BREAK_STACK_CFLOW_64 ({ OBFH_CFLOW_CAPTURE; OBFH_CFLOW_TEMPLATE_64; })
#define BREAK_STACK_CFLOW_65 ({ OBFH_CFLOW_CAPTURE; OBFH_CFLOW_TEMPLATE_65; })
#define BREAK_STACK_CFLOW_66 ({ OBFH_CFLOW_CAPTURE; OBFH_CFLOW_TEMPLATE_66; })
#define BREAK_STACK_CFLOW_67 ({ OBFH_CFLOW_CAPTURE; OBFH_CFLOW_TEMPLATE_67; })
#define BREAK_STACK_CFLOW_68 ({ OBFH_CFLOW_CAPTURE; OBFH_CFLOW_TEMPLATE_68; })
#define BREAK_STACK_CFLOW_69 ({ OBFH_CFLOW_CAPTURE; OBFH_CFLOW_TEMPLATE_69; })
#define BREAK_STACK_CFLOW_70 ({ OBFH_CFLOW_CAPTURE; OBFH_CFLOW_TEMPLATE_70; })
#define BREAK_STACK_CFLOW_71 ({ OBFH_CFLOW_CAPTURE; OBFH_CFLOW_TEMPLATE_71; })
#define BREAK_STACK_CFLOW_72 ({ OBFH_CFLOW_CAPTURE; OBFH_CFLOW_TEMPLATE_72; })
#define BREAK_STACK_CFLOW_73 ({ OBFH_CFLOW_CAPTURE; OBFH_CFLOW_TEMPLATE_73; })
#define BREAK_STACK_CFLOW_74 ({ OBFH_CFLOW_CAPTURE; OBFH_CFLOW_TEMPLATE_74; })
#define BREAK_STACK_CFLOW_75 ({ OBFH_CFLOW_CAPTURE; OBFH_CFLOW_TEMPLATE_75; })
#define BREAK_STACK_CFLOW_76 ({ OBFH_CFLOW_CAPTURE; OBFH_CFLOW_TEMPLATE_76; })
#define BREAK_STACK_CFLOW_77 ({ OBFH_CFLOW_CAPTURE; OBFH_CFLOW_TEMPLATE_77; })
#define BREAK_STACK_CFLOW_78 ({ OBFH_CFLOW_CAPTURE; OBFH_CFLOW_TEMPLATE_78; })
#define BREAK_STACK_CFLOW_79 ({ OBFH_CFLOW_CAPTURE; OBFH_CFLOW_TEMPLATE_79; })
#define BREAK_STACK_CFLOW_80 ({ OBFH_CFLOW_CAPTURE; OBFH_CFLOW_TEMPLATE_80; })
#define BREAK_STACK_CFLOW_81 ({ OBFH_CFLOW_CAPTURE; OBFH_CFLOW_TEMPLATE_81; })
#define BREAK_STACK_CFLOW_82 ({ OBFH_CFLOW_CAPTURE; OBFH_CFLOW_TEMPLATE_82; })
#define BREAK_STACK_CFLOW_83 ({ OBFH_CFLOW_CAPTURE; OBFH_CFLOW_TEMPLATE_83; })
#define BREAK_STACK_CFLOW_84 ({ OBFH_CFLOW_CAPTURE; OBFH_CFLOW_TEMPLATE_84; })
#define BREAK_STACK_CFLOW_85 ({ OBFH_CFLOW_CAPTURE; OBFH_CFLOW_TEMPLATE_85; })
#define BREAK_STACK_CFLOW_86 ({ OBFH_CFLOW_CAPTURE; OBFH_CFLOW_TEMPLATE_86; })
#define BREAK_STACK_CFLOW_87 ({ OBFH_CFLOW_CAPTURE; OBFH_CFLOW_TEMPLATE_87; })
#define BREAK_STACK_CFLOW_88 ({ OBFH_CFLOW_CAPTURE; OBFH_CFLOW_TEMPLATE_88; })
#define BREAK_STACK_CFLOW_89 ({ OBFH_CFLOW_CAPTURE; OBFH_CFLOW_TEMPLATE_89; })
#define BREAK_STACK_CFLOW_90 ({ OBFH_CFLOW_CAPTURE; OBFH_CFLOW_TEMPLATE_90; })
#define BREAK_STACK_CFLOW_91 ({ OBFH_CFLOW_CAPTURE; OBFH_CFLOW_TEMPLATE_91; })
#define BREAK_STACK_CFLOW_92 ({ OBFH_CFLOW_CAPTURE; OBFH_CFLOW_TEMPLATE_92; })
#define BREAK_STACK_CFLOW_93 ({ OBFH_CFLOW_CAPTURE; OBFH_CFLOW_TEMPLATE_93; })
#define BREAK_STACK_CFLOW_94 ({ OBFH_CFLOW_CAPTURE; OBFH_CFLOW_TEMPLATE_94; })
#define BREAK_STACK_CFLOW_95 ({ OBFH_CFLOW_CAPTURE; OBFH_CFLOW_TEMPLATE_95; })
#define BREAK_STACK_CFLOW_96 ({ OBFH_CFLOW_CAPTURE; OBFH_CFLOW_TEMPLATE_96; })
#define BREAK_STACK_CFLOW_97 ({ OBFH_CFLOW_CAPTURE; OBFH_CFLOW_TEMPLATE_97; })
#define BREAK_STACK_CFLOW_98 ({ OBFH_CFLOW_CAPTURE; OBFH_CFLOW_TEMPLATE_98; })
#define BREAK_STACK_CFLOW_99 ({ OBFH_CFLOW_CAPTURE; OBFH_CFLOW_TEMPLATE_99; })
#define BREAK_STACK_CFLOW_100 ({ OBFH_CFLOW_CAPTURE; OBFH_CFLOW_TEMPLATE_100; })
#define BREAK_STACK_CFLOW_101 ({ OBFH_CFLOW_CAPTURE; OBFH_CFLOW_TEMPLATE_101; })
#define BREAK_STACK_CFLOW_102 ({ OBFH_CFLOW_CAPTURE; OBFH_CFLOW_TEMPLATE_102; })
#define BREAK_STACK_CFLOW_103 ({ OBFH_CFLOW_CAPTURE; OBFH_CFLOW_TEMPLATE_103; })
#define BREAK_STACK_CFLOW_104 ({ OBFH_CFLOW_CAPTURE; OBFH_CFLOW_TEMPLATE_104; })
#define BREAK_STACK_CFLOW_105 ({ OBFH_CFLOW_CAPTURE; OBFH_CFLOW_TEMPLATE_105; })
#define BREAK_STACK_CFLOW_106 ({ OBFH_CFLOW_CAPTURE; OBFH_CFLOW_TEMPLATE_106; })
#define BREAK_STACK_CFLOW_107 ({ OBFH_CFLOW_CAPTURE; OBFH_CFLOW_TEMPLATE_107; })
#define BREAK_STACK_CFLOW_108 ({ OBFH_CFLOW_CAPTURE; OBFH_CFLOW_TEMPLATE_108; })
#define BREAK_STACK_CFLOW_109 ({ OBFH_CFLOW_CAPTURE; OBFH_CFLOW_TEMPLATE_109; })
#define BREAK_STACK_CFLOW_110 ({ OBFH_CFLOW_CAPTURE; OBFH_CFLOW_TEMPLATE_110; })
#define BREAK_STACK_CFLOW_111 ({ OBFH_CFLOW_CAPTURE; OBFH_CFLOW_TEMPLATE_111; })
#define BREAK_STACK_CFLOW_112 ({ OBFH_CFLOW_CAPTURE; OBFH_CFLOW_TEMPLATE_112; })
#define BREAK_STACK_CFLOW_113 ({ OBFH_CFLOW_CAPTURE; OBFH_CFLOW_TEMPLATE_113; })
#define BREAK_STACK_CFLOW_114 ({ OBFH_CFLOW_CAPTURE; OBFH_CFLOW_TEMPLATE_114; })
#define BREAK_STACK_CFLOW_115 ({ OBFH_CFLOW_CAPTURE; OBFH_CFLOW_TEMPLATE_115; })
#define BREAK_STACK_CFLOW_116 ({ OBFH_CFLOW_CAPTURE; OBFH_CFLOW_TEMPLATE_116; })
#define BREAK_STACK_CFLOW_117 ({ OBFH_CFLOW_CAPTURE; OBFH_CFLOW_TEMPLATE_117; })
#define BREAK_STACK_CFLOW_118 ({ OBFH_CFLOW_CAPTURE; OBFH_CFLOW_TEMPLATE_118; })
#define BREAK_STACK_CFLOW_119 ({ OBFH_CFLOW_CAPTURE; OBFH_CFLOW_TEMPLATE_119; })
#define BREAK_STACK_CFLOW_120 ({ OBFH_CFLOW_CAPTURE; OBFH_CFLOW_TEMPLATE_120; })
#define BREAK_STACK_CFLOW_121 ({ OBFH_CFLOW_CAPTURE; OBFH_CFLOW_TEMPLATE_121; })
#define BREAK_STACK_CFLOW_122 ({ OBFH_CFLOW_CAPTURE; OBFH_CFLOW_TEMPLATE_122; })
#define BREAK_STACK_CFLOW_123 ({ OBFH_CFLOW_CAPTURE; OBFH_CFLOW_TEMPLATE_123; })
#define BREAK_STACK_CFLOW_124 ({ OBFH_CFLOW_CAPTURE; OBFH_CFLOW_TEMPLATE_124; })
#define BREAK_STACK_CFLOW_125 ({ OBFH_CFLOW_CAPTURE; OBFH_CFLOW_TEMPLATE_125; })
#define BREAK_STACK_CFLOW_126 ({ OBFH_CFLOW_CAPTURE; OBFH_CFLOW_TEMPLATE_126; })
#define BREAK_STACK_CFLOW_127 ({ OBFH_CFLOW_CAPTURE; OBFH_CFLOW_TEMPLATE_127; })

#define BREAK_STACK_CFLOW OBFH_CFLOW_EMIT(__COUNTER__, OBFH_CFLOW_EXTRA)

// ============================================================================
// 07. STACK_PROXY_FUNCTIONS: guards, bodies, layouts and selection
// ============================================================================

// Self-contained fake functions: live guards skip all calls and stack edits.
// Frames and calling conventions follow Windows TCC x86/x64. Numeric labels
// stay inside each ASM expansion; no insertion refers to another call site.
#define OBFH_SF_CAPTURE                                                                  \
    enum {                                                                               \
        __obfh_sf_mask = OBFH_JUNK_WORD,                                                 \
        __obfh_sf_alu_a = RND(0, 4),                                                     \
        __obfh_sf_alu_b = RND(0, 6),                                                     \
        __obfh_sf_shift = RND(0, 4),                                                     \
        __obfh_sf_phantom = OBFH_PHANTOM_DRAW(__LINE__),                                 \
        __obfh_sf_salt = RND(1, 32767),                                                  \
        __obfh_sf_rotate = RND(1, 31),                                                   \
        __obfh_sf_frame_a = RND(2, 12) * 16u,                                            \
        __obfh_sf_frame_b = RND(2, 12) * 16u,                                            \
        __obfh_sf_frame_c = RND(2, 12) * 16u,                                            \
        __obfh_sf_arg_a = RND(1, 65535),                                                 \
        __obfh_sf_arg_b = RND(1, 65535),                                                 \
        __obfh_sf_key_a = OBFH_JUNK_WORD,                                                \
        __obfh_sf_key_b = OBFH_JUNK_WORD,                                                \
        __obfh_sf_factor = RND(1, 32767) * 2u + 1u,                                      \
        __obfh_sf_pad = RND(0, 7),                                                       \
        __obfh_sf_loops = RND(1, 7),                                                     \
        __obfh_sf_not_mask = ~(unsigned int)__obfh_sf_mask,                              \
        __obfh_sf_frame_d = RND(2, 12) * 16u,                                            \
        __obfh_sf_pad_b = RND(0, 15),                                                    \
        __obfh_sf_noise_a = OBFH_JUNK_BYTE,                                              \
        __obfh_sf_noise_b = OBFH_JUNK_BYTE,                                              \
        __obfh_sf_local_a = RND(1, 3) * 4u,                                              \
        __obfh_sf_local_b = RND(4, 7) * 4u,                                              \
        __obfh_sf_leaf_frame = RND(2, 6) * 16u + 8u,                                     \
        __obfh_sf_leaf_slot = RND(0, 6) * 4u,                                            \
        __obfh_sf_leaf_alu_a = __obfh_sf_alu_a == 0 ? 0x05 : __obfh_sf_alu_a == 1 ? 0x0d \
                                                         : __obfh_sf_alu_a == 2   ? 0x25 \
                                                         : __obfh_sf_alu_a == 3   ? 0x2d \
                                                                                  : 0x35,  \
        __obfh_sf_leaf_alu_b = 0xc2 + __obfh_sf_alu_b * 8,                               \
        __obfh_sf_leaf_shift = __obfh_sf_shift == 0 ? 0xc0 : __obfh_sf_shift == 1 ? 0xc8 \
                                                         : __obfh_sf_shift == 2   ? 0xe0 \
                                                         : __obfh_sf_shift == 3   ? 0xe8 \
                                                                                  : 0xf8 \
    }
#define OBFH_SF_INPUTS                             \
    [sf_phantom] "i"(__obfh_sf_phantom),           \
        [sf_salt] "i"(__obfh_sf_salt),             \
        [sf_rotate] "i"(__obfh_sf_rotate),         \
        [sf_frame_a] "i"(__obfh_sf_frame_a),       \
        [sf_frame_b] "i"(__obfh_sf_frame_b),       \
        [sf_frame_c] "i"(__obfh_sf_frame_c),       \
        [sf_arg_a] "i"(__obfh_sf_arg_a),           \
        [sf_arg_b] "i"(__obfh_sf_arg_b),           \
        [sf_key_a] "i"(__obfh_sf_key_a),           \
        [sf_key_b] "i"(__obfh_sf_key_b),           \
        [sf_factor] "i"(__obfh_sf_factor),         \
        [sf_pad] "i"(__obfh_sf_pad),               \
        [sf_loops] "i"(__obfh_sf_loops),           \
        [sf_mask] "i"(__obfh_sf_mask),             \
        [sf_not_mask] "i"(__obfh_sf_not_mask),     \
        [sf_frame_d] "i"(__obfh_sf_frame_d),       \
        [sf_pad_b] "i"(__obfh_sf_pad_b),           \
        [sf_noise_a] "i"(__obfh_sf_noise_a),       \
        [sf_noise_b] "i"(__obfh_sf_noise_b),       \
        [sf_local_a] "i"(__obfh_sf_local_a),       \
        [sf_local_b] "i"(__obfh_sf_local_b),       \
        [sf_leaf_frame] "i"(__obfh_sf_leaf_frame), \
        [sf_leaf_slot] "i"(__obfh_sf_leaf_slot),   \
        [sf_leaf_alu_a] "i"(__obfh_sf_leaf_alu_a), \
        [sf_leaf_alu_b] "i"(__obfh_sf_leaf_alu_b), \
        [sf_leaf_shift] "i"(__obfh_sf_leaf_shift)

// All choices below assemble only inside skipped native functions. No new
// RND draws or live-path instructions; existing captured entropy selects forms.
#define OBFH_SF_BYTES(bit, flip, size, value) \
    ".fill (((%c[sf_phantom] >> " bit ") & 1) ^ " flip "), " size ", " value ";"
#define OBFH_SF_WORD(bit, flip, value) OBFH_SF_BYTES(bit, flip, "4", value)
// Optional edges exist only in skipped bodies. x64 call sequence is 23 bytes;
// x86 uses explicit imm32 pushes so its sequence is always 18 bytes.
#define OBFH_SF_LINK_BYTES(channel, size, value) \
    ".fill (((%c[sf_phantom] ^ %c[sf_key_a]) >> (12 + 3 * " channel ")) & 1), " size ", " value ";"
#define OBFH_SF_LINK_GATE(span, channel)                                                                                   \
    OBFH_SF_LINK_BYTES(channel, "2", "0xc085 ^ ((((%c[sf_phantom] ^ %c[sf_key_a]) >> (14 + 3 * " channel ")) & 1) << 12)") \
    OBFH_SF_LINK_BYTES(channel, "1", "0x74 | (((%c[sf_phantom] ^ %c[sf_key_a]) >> (13 + 3 * " channel ")) & 1)")           \
    OBFH_SF_LINK_BYTES(channel, "1", span)
#if defined(__x86_64__)
#define OBFH_SF_INPUT "movl %%esp, %%eax;"
#define OBFH_SF_FRAME_HEAD(size) \
    "pushq %%rbp;" OBFH_SF_BYTES("(1 + ((%c[" size "] >> 4) & 3))", "0", "3", "0xe58948") OBFH_SF_BYTES("(1 + ((%c[" size "] >> 4) & 3))", "1", "4", "0x242c8d48")
#define OBFH_SF_FRAME(size)                                                  \
    OBFH_SF_FRAME_HEAD(size)                                                 \
    OBFH_SF_BYTES("(4 + ((%c[" size "] >> 4) & 3))", "0", "3", "0xec8148")   \
    OBFH_SF_WORD("(4 + ((%c[" size "] >> 4) & 3))", "0", "%c[" size "]")     \
    OBFH_SF_BYTES("(4 + ((%c[" size "] >> 4) & 3))", "1", "4", "0x24a48d48") \
    OBFH_SF_WORD("(4 + ((%c[" size "] >> 4) & 3))", "1", "-%c[" size "]")
#define OBFH_SF_FRAME_ALT(size)                                                                                                                                \
    "pushq %%rbp;" OBFH_SF_BYTES("(4 + ((%c[" size "] >> 4) & 3))", "0", "3", "0xec8148") OBFH_SF_WORD("(4 + ((%c[" size "] >> 4) & 3))", "0", "%c[" size "]") \
        OBFH_SF_BYTES("(4 + ((%c[" size "] >> 4) & 3))", "1", "4", "0x24a48d48") OBFH_SF_WORD("(4 + ((%c[" size "] >> 4) & 3))", "1", "-%c[" size "]") ".fill 1, 4, 0x24ac8d48; .long %c[" size "];"
#define OBFH_SF_EPILOGUE_ALT                   \
    OBFH_SF_BYTES("7", "0", "3", "0xec8948")   \
    OBFH_SF_BYTES("7", "1", "4", "0x00658d48") \
    "popq %%rbp; ret;"
#define OBFH_SF_EPILOGUE                       \
    OBFH_SF_BYTES("6", "0", "1", "0xc9")       \
    OBFH_SF_BYTES("6", "1", "4", "0x5dec8948") \
    "ret;"
#define OBFH_SF_ARGS                       \
    OBFH_SF_BYTES("9", "0", "2", "0xc889") \
    OBFH_SF_BYTES("9", "1", "3", "0x00418d")
#define OBFH_SF_CALL_ARGS                   \
    OBFH_SF_BYTES("10", "0", "1", "0xb9")   \
    OBFH_SF_WORD("10", "0", "%c[sf_arg_a]") \
    OBFH_SF_BYTES("10", "0", "1", "0xba")   \
    OBFH_SF_WORD("10", "0", "%c[sf_arg_b]") \
    OBFH_SF_BYTES("10", "1", "1", "0xba")   \
    OBFH_SF_WORD("10", "1", "%c[sf_arg_b]") \
    OBFH_SF_BYTES("10", "1", "1", "0xb9")   \
    OBFH_SF_WORD("10", "1", "%c[sf_arg_a]")
#define OBFH_SF_CALL_AT(target, channel) OBFH_SF_LINK_GATE("23", channel) "subq $32, %%rsp;" OBFH_SF_CALL_ARGS "call " target "; addq $32, %%rsp;"
#define OBFH_SF_LOCAL                               \
    OBFH_SF_BYTES("8", "0", "2", "0x4589")          \
    OBFH_SF_BYTES("8", "0", "1", "-%c[sf_local_a]") \
    OBFH_SF_BYTES("8", "0", "2", "0x5589")          \
    OBFH_SF_BYTES("8", "0", "1", "-%c[sf_local_b]") \
    OBFH_SF_BYTES("8", "1", "2", "0x5589")          \
    OBFH_SF_BYTES("8", "1", "1", "-%c[sf_local_b]") \
    OBFH_SF_BYTES("8", "1", "2", "0x4589")          \
    OBFH_SF_BYTES("8", "1", "1", "-%c[sf_local_a]")
#else
#define OBFH_SF_INPUT "movl %%esp, %%eax;"
#define OBFH_SF_FRAME_HEAD(size) \
    "pushl %%ebp;" OBFH_SF_BYTES("(1 + ((%c[" size "] >> 4) & 3))", "0", "2", "0xe589") OBFH_SF_BYTES("(1 + ((%c[" size "] >> 4) & 3))", "1", "3", "0x242c8d")
#define OBFH_SF_FRAME(size)                                                \
    OBFH_SF_FRAME_HEAD(size)                                               \
    OBFH_SF_BYTES("(4 + ((%c[" size "] >> 4) & 3))", "0", "2", "0xec81")   \
    OBFH_SF_WORD("(4 + ((%c[" size "] >> 4) & 3))", "0", "%c[" size "]")   \
    OBFH_SF_BYTES("(4 + ((%c[" size "] >> 4) & 3))", "1", "3", "0x24a48d") \
    OBFH_SF_WORD("(4 + ((%c[" size "] >> 4) & 3))", "1", "-%c[" size "]")
#define OBFH_SF_FRAME_ALT(size)                                                                                                                              \
    "pushl %%ebp;" OBFH_SF_BYTES("(4 + ((%c[" size "] >> 4) & 3))", "0", "2", "0xec81") OBFH_SF_WORD("(4 + ((%c[" size "] >> 4) & 3))", "0", "%c[" size "]") \
        OBFH_SF_BYTES("(4 + ((%c[" size "] >> 4) & 3))", "1", "3", "0x24a48d") OBFH_SF_WORD("(4 + ((%c[" size "] >> 4) & 3))", "1", "-%c[" size "]") ".fill 1, 3, 0x24ac8d; .long %c[" size "];"
#define OBFH_SF_EPILOGUE_ALT                 \
    OBFH_SF_BYTES("7", "0", "2", "0xec89")   \
    OBFH_SF_BYTES("7", "1", "3", "0x00658d") \
    "popl %%ebp; ret;"
#define OBFH_SF_EPILOGUE                     \
    OBFH_SF_BYTES("6", "0", "1", "0xc9")     \
    OBFH_SF_BYTES("6", "1", "3", "0x5dec89") \
    "ret;"
#define OBFH_SF_ARGS                         \
    OBFH_SF_BYTES("9", "0", "3", "0x0c558b") \
    OBFH_SF_BYTES("9", "0", "3", "0x08458b") \
    OBFH_SF_BYTES("9", "1", "3", "0x08458b") \
    OBFH_SF_BYTES("9", "1", "3", "0x0c558b")
#define OBFH_SF_CALL_AT(target, channel) OBFH_SF_LINK_GATE("18", channel) ".byte 0x68; .long %c[sf_arg_b]; .byte 0x68; .long %c[sf_arg_a]; call " target "; addl $8, %%esp;"
#define OBFH_SF_LOCAL                               \
    OBFH_SF_BYTES("8", "0", "2", "0x4589")          \
    OBFH_SF_BYTES("8", "0", "1", "-%c[sf_local_a]") \
    OBFH_SF_BYTES("8", "0", "2", "0x5589")          \
    OBFH_SF_BYTES("8", "0", "1", "-%c[sf_local_b]") \
    OBFH_SF_BYTES("8", "1", "2", "0x5589")          \
    OBFH_SF_BYTES("8", "1", "1", "-%c[sf_local_b]") \
    OBFH_SF_BYTES("8", "1", "2", "0x4589")          \
    OBFH_SF_BYTES("8", "1", "1", "-%c[sf_local_a]")
#endif
#define OBFH_SF_CALL(target) OBFH_SF_CALL_AT(target, "0")
#define OBFH_SF_PADDING ".fill %c[sf_pad], 1, 0x90;"
#define OBFH_SF_GAP ".fill %c[sf_pad_b], 1, 0x90; .byte %c[sf_noise_a], %c[sf_noise_b];"
#define OBFH_SF_BODY_A OBFH_SF_ARGS "xorl $%c[sf_key_a], %%eax; imull $%c[sf_factor], %%eax; addl %%edx, %%eax;" OBFH_SF_LOCAL
#define OBFH_SF_BODY_B OBFH_SF_ARGS "addl $%c[sf_key_b], %%eax; roll $%c[sf_rotate], %%eax; xorl %%edx, %%eax;" OBFH_SF_LOCAL
#define OBFH_SF_BODY_C OBFH_SF_ARGS "leal (%%eax, %%eax, 2), %%eax; xorl %%eax, %%edx; addl $%c[sf_key_a], %%eax;" OBFH_SF_LOCAL
#define OBFH_SF_BODY_D OBFH_SF_ARGS "notl %%eax; addl %%edx, %%eax; imull $%c[sf_factor], %%eax; roll $%c[sf_rotate], %%eax;" OBFH_SF_LOCAL

#define OBFH_SF_BODY_E OBFH_SF_ARGS "bswap %%eax; xorl %%edx, %%eax; roll $%c[sf_rotate], %%eax;" OBFH_SF_LOCAL
#define OBFH_SF_BODY_F OBFH_SF_ARGS "movl $%c[sf_loops], %%ecx; 6: addl %%edx, %%eax; imull $%c[sf_factor], %%eax; xorl $%c[sf_key_b], %%eax; decl %%ecx; jnz 6b;" OBFH_SF_LOCAL
#define OBFH_SF_BODY_G OBFH_SF_ARGS "movzbl %%al, %%ecx; shrl $8, %%eax; xorl %%edx, %%ecx; leal (%%eax, %%ecx, 4), %%eax;" OBFH_SF_LOCAL
#define OBFH_SF_BODY_H OBFH_SF_ARGS "imull %%eax, %%edx; xorl $%c[sf_key_b], %%edx; subl %%edx, %%eax; negl %%eax;" OBFH_SF_LOCAL
#define OBFH_SF_BODY_I OBFH_SF_ARGS "movl %%eax, %%ecx; orl $%c[sf_mask], %%eax; andl $%c[sf_mask], %%ecx; subl %%ecx, %%eax; xorl %%edx, %%eax;" OBFH_SF_LOCAL
#define OBFH_SF_BODY_J OBFH_SF_ARGS "xorl $%c[sf_key_a], %%eax; movl %%edx, %%ecx; shll $%c[sf_rotate], %%ecx; shrl $%c[sf_rotate], %%eax; orl %%ecx, %%eax;" OBFH_SF_LOCAL
#define OBFH_SF_BODY_K OBFH_SF_ARGS "testl %%edx, %%edx; js 6f; addl $%c[sf_key_a], %%eax; jmp 7f; 6: negl %%eax; xorl %%edx, %%eax; 7: roll $%c[sf_rotate], %%eax;" OBFH_SF_LOCAL
#define OBFH_SF_BODY_L OBFH_SF_ARGS "movl %%eax, %%ecx; movl %%edx, %%eax; addl %%ecx, %%eax; imull $%c[sf_factor], %%eax; notl %%edx; xorl %%edx, %%eax;" OBFH_SF_LOCAL

#define OBFH_SF_BODY_M OBFH_SF_ARGS "movl %%eax, %%ecx; shrl $16, %%ecx; xorl %%ecx, %%eax; imull $%c[sf_factor], %%eax; xorl %%edx, %%eax;" OBFH_SF_LOCAL
#define OBFH_SF_BODY_N OBFH_SF_ARGS "addl $%c[sf_key_a], %%eax; adcl $%c[sf_key_b], %%edx; xorl %%edx, %%eax; rorl $%c[sf_rotate], %%eax;" OBFH_SF_LOCAL
#define OBFH_SF_BODY_O OBFH_SF_ARGS "xchgl %%eax, %%edx; subl $%c[sf_key_a], %%eax; bswap %%edx; addl %%edx, %%eax;" OBFH_SF_LOCAL
#define OBFH_SF_BODY_P OBFH_SF_ARGS "movl $%c[sf_factor], %%ecx; xorl %%edx, %%edx; divl %%ecx; xorl %%edx, %%eax;" OBFH_SF_LOCAL
#define OBFH_SF_BODY_Q OBFH_SF_ARGS "movzbl %%al, %%ecx; movzbl %%dl, %%edx; imull %%edx, %%ecx; shrl $8, %%eax; addl %%ecx, %%eax;" OBFH_SF_LOCAL
#define OBFH_SF_BODY_R OBFH_SF_ARGS "movl $%c[sf_loops], %%ecx; 6: xorl %%edx, %%eax; roll $%c[sf_rotate], %%eax; addl $%c[sf_key_a], %%edx; decl %%ecx; jnz 6b;" OBFH_SF_LOCAL
#define OBFH_SF_BODY_S OBFH_SF_ARGS "shldl $%c[sf_rotate], %%edx, %%eax; subl $%c[sf_key_b], %%eax; xorl $%c[sf_mask], %%eax;" OBFH_SF_LOCAL
#define OBFH_SF_BODY_T OBFH_SF_ARGS "movl %%eax, %%ecx; shrl $16, %%eax; shll $16, %%ecx; orl %%ecx, %%eax; xorl %%edx, %%eax; negl %%eax;" OBFH_SF_LOCAL

// All fifteen identities hold for arbitrary uint32 inputs, including wraparound.
#define OBFH_SF_GUARD_0 "imull %%eax, %%eax; testl $2, %%eax; jz 9f;"
#define OBFH_SF_GUARD_1 "leal 1(%%eax), %%edx; imull %%edx, %%eax; testl $1, %%eax; jz 9f;"
#define OBFH_SF_GUARD_2 "movl %%eax, %%edx; imull %%eax, %%eax; xorl %%edx, %%eax; testl $1, %%eax; jz 9f;"
#define OBFH_SF_GUARD_3 "movl %%eax, %%edx; notl %%edx; addl %%eax, %%edx; cmpl $-1, %%edx; je 9f;"

#define OBFH_SF_GUARD_4 "movl %%eax, %%edx; movl %%eax, %%ecx; orl $%c[sf_mask], %%eax; andl $%c[sf_mask], %%edx; addl %%edx, %%eax; addl $%c[sf_mask], %%ecx; cmpl %%ecx, %%eax; je 9f;"
#define OBFH_SF_GUARD_5 "movl %%eax, %%edx; roll $%c[sf_rotate], %%eax; rorl $%c[sf_rotate], %%eax; cmpl %%edx, %%eax; je 9f;"
#define OBFH_SF_GUARD_6 "movl %%eax, %%edx; bswap %%eax; bswap %%eax; cmpl %%edx, %%eax; je 9f;"
#define OBFH_SF_GUARD_7 "leal -1(%%eax), %%edx; imull %%edx, %%eax; testl $1, %%eax; jz 9f;"
#define OBFH_SF_GUARD_8 "movl %%eax, %%edx; notl %%edx; xorl $%c[sf_key_a], %%eax; xorl $%c[sf_key_a], %%edx; xorl %%edx, %%eax; cmpl $-1, %%eax; je 9f;"

#define OBFH_SF_GUARD_9 "movl %%eax, %%ecx; movl %%eax, %%edx; xorl $%c[sf_mask], %%eax; andl $%c[sf_mask], %%edx; leal (%%eax, %%edx, 2), %%eax; addl $%c[sf_mask], %%ecx; cmpl %%ecx, %%eax; je 9f;"
#define OBFH_SF_GUARD_10 "movl %%eax, %%edx; negl %%edx; andl %%edx, %%eax; leal -1(%%eax), %%edx; andl %%edx, %%eax; testl %%eax, %%eax; jz 9f;"
#define OBFH_SF_GUARD_11 "movl %%eax, %%edx; movl %%eax, %%ecx; andl $%c[sf_mask], %%eax; andl $%c[sf_not_mask], %%edx; imull $%c[sf_factor], %%eax; imull $%c[sf_factor], %%edx; addl %%edx, %%eax; imull $%c[sf_factor], %%ecx; cmpl %%ecx, %%eax; je 9f;"
#define OBFH_SF_GUARD_12 "movl %%eax, %%edx; addl $%c[sf_key_a], %%eax; imull $%c[sf_factor], %%eax; imull $%c[sf_factor], %%edx; movl $%c[sf_key_a], %%ecx; imull $%c[sf_factor], %%ecx; addl %%edx, %%ecx; cmpl %%ecx, %%eax; je 9f;"
#define OBFH_SF_GUARD_13 "movl %%eax, %%ecx; movl %%eax, %%edx; addl $%c[sf_key_a], %%eax; imull %%eax, %%eax; imull %%ecx, %%ecx; subl %%ecx, %%eax; imull $%c[sf_key_a], %%edx; addl %%edx, %%edx; subl %%edx, %%eax; movl $%c[sf_key_a], %%edx; imull %%edx, %%edx; cmpl %%edx, %%eax; je 9f;"
#define OBFH_SF_GUARD_14 "movl %%eax, %%edx; roll $16, %%eax; roll $16, %%eax; cmpl %%edx, %%eax; je 9f;"

#define OBFH_SF_ENTRY_ALT(label, frame, body) label ": " OBFH_SF_FRAME_ALT(frame) body ".fill ((%c[sf_phantom] >> " label ") & 1), 1, 0x90;"
#define OBFH_SF_ENTRY(label, frame, body) label ": " OBFH_SF_FRAME(frame) body ".fill ((%c[sf_phantom] >> " label ") & 1), 1, 0x90;"
// Physical order, finite link graphs, and shared/tail continuations vary.
#define OBFH_SF_LAYOUT_0(a, b, c)       \
    OBFH_SF_CALL_AT("1f", "0")          \
    OBFH_SF_EPILOGUE                    \
    OBFH_SF_GAP                         \
    OBFH_SF_ENTRY("2", "sf_frame_b", b) \
    OBFH_SF_EPILOGUE                    \
    OBFH_SF_GAP                         \
    OBFH_SF_ENTRY("1", "sf_frame_a", a) \
    OBFH_SF_CALL_AT("2b", "1")          \
    OBFH_SF_EPILOGUE_ALT
#define OBFH_SF_LAYOUT_1(a, b, c)           \
    OBFH_SF_CALL_AT("1f", "0")              \
    OBFH_SF_EPILOGUE                        \
    OBFH_SF_GAP                             \
    OBFH_SF_ENTRY_ALT("1", "sf_frame_a", a) \
    "leave; jmp 2f;" OBFH_SF_GAP            \
        OBFH_SF_ENTRY("2", "sf_frame_b", b) \
            OBFH_SF_EPILOGUE
#define OBFH_SF_LAYOUT_2(a, b, c)           \
    OBFH_SF_CALL_AT("1f", "0")              \
    OBFH_SF_EPILOGUE                        \
    OBFH_SF_GAP                             \
    OBFH_SF_ENTRY("3", "sf_frame_c", c)     \
    OBFH_SF_EPILOGUE                        \
    OBFH_SF_GAP                             \
    OBFH_SF_ENTRY("1", "sf_frame_a", a)     \
    OBFH_SF_CALL_AT("2f", "1")              \
    OBFH_SF_CALL_AT("3b", "2")              \
    OBFH_SF_EPILOGUE                        \
    OBFH_SF_GAP                             \
    OBFH_SF_ENTRY_ALT("2", "sf_frame_b", b) \
    OBFH_SF_CALL_AT("3b", "3")              \
    OBFH_SF_EPILOGUE_ALT
#define OBFH_SF_LAYOUT_3(a, b, c)           \
    OBFH_SF_CALL_AT("1f", "0")              \
    OBFH_SF_EPILOGUE                        \
    OBFH_SF_GAP                             \
    OBFH_SF_ENTRY("2", "sf_frame_b", b)     \
    "leave; jmp 3f;" OBFH_SF_GAP            \
        OBFH_SF_ENTRY("3", "sf_frame_c", c) \
    OBFH_SF_EPILOGUE                        \
    OBFH_SF_GAP                             \
    OBFH_SF_ENTRY_ALT("1", "sf_frame_a", a) \
    "testl %%eax, %%eax; jz 4f;" OBFH_SF_CALL_AT("2b", "1") "jmp 5f; 4:" OBFH_SF_CALL_AT("3b", "2") "5:" OBFH_SF_EPILOGUE_ALT
#define OBFH_SF_LAYOUT_4(a, b, c)           \
    OBFH_SF_CALL_AT("1f", "0")              \
    OBFH_SF_EPILOGUE                        \
    OBFH_SF_GAP                             \
    OBFH_SF_ENTRY_ALT("2", "sf_frame_b", b) \
    OBFH_SF_CALL_AT("3f", "1")              \
    OBFH_SF_EPILOGUE_ALT                    \
    OBFH_SF_GAP                             \
    OBFH_SF_ENTRY("1", "sf_frame_a", a)     \
    OBFH_SF_CALL_AT("2b", "2")              \
    OBFH_SF_EPILOGUE                        \
    OBFH_SF_GAP                             \
    OBFH_SF_ENTRY("3", "sf_frame_c", c)     \
    OBFH_SF_EPILOGUE
#define OBFH_SF_LAYOUT_5(a, b, c)           \
    OBFH_SF_CALL_AT("1f", "0")              \
    OBFH_SF_EPILOGUE                        \
    OBFH_SF_GAP                             \
    OBFH_SF_ENTRY("3", "sf_frame_c", c)     \
    OBFH_SF_EPILOGUE                        \
    OBFH_SF_GAP                             \
    OBFH_SF_ENTRY("2", "sf_frame_b", b)     \
    OBFH_SF_EPILOGUE                        \
    OBFH_SF_GAP                             \
    OBFH_SF_ENTRY_ALT("1", "sf_frame_a", a) \
    OBFH_SF_CALL_AT("2b", "1")              \
    OBFH_SF_CALL_AT("3b", "2")              \
    OBFH_SF_EPILOGUE_ALT
#define OBFH_SF_LAYOUT_6(a, b, c)                                            \
    OBFH_SF_CALL_AT("1f", "0")                                               \
    OBFH_SF_EPILOGUE                                                         \
    OBFH_SF_GAP                                                              \
    OBFH_SF_ENTRY("1", "sf_frame_a", a)                                      \
    OBFH_SF_CALL_AT("2f", "1")                                               \
    "leave; jmp 3f;" OBFH_SF_GAP                                             \
        OBFH_SF_ENTRY_ALT("2", "sf_frame_b", b) "leave; jmp 3f;" OBFH_SF_GAP \
            OBFH_SF_ENTRY("3", "sf_frame_c", c)                              \
                OBFH_SF_EPILOGUE
#define OBFH_SF_LAYOUT_7(a, b, c)           \
    OBFH_SF_CALL_AT("1f", "0")              \
    OBFH_SF_EPILOGUE                        \
    OBFH_SF_GAP                             \
    OBFH_SF_ENTRY("3", "sf_frame_c", c)     \
    OBFH_SF_EPILOGUE                        \
    OBFH_SF_GAP                             \
    OBFH_SF_ENTRY("2", "sf_frame_b", b)     \
    OBFH_SF_CALL_AT("3b", "1")              \
    OBFH_SF_EPILOGUE                        \
    OBFH_SF_GAP                             \
    OBFH_SF_ENTRY_ALT("1", "sf_frame_a", a) \
    OBFH_SF_CALL_AT("2b", "2")              \
    OBFH_SF_CALL_AT("3b", "3")              \
    OBFH_SF_EPILOGUE_ALT
#define OBFH_SF_LAYOUT_8(a, b, c)           \
    OBFH_SF_CALL_AT("1f", "0")              \
    OBFH_SF_CALL_AT("2f", "1")              \
    OBFH_SF_EPILOGUE                        \
    OBFH_SF_GAP                             \
    OBFH_SF_ENTRY("3", "sf_frame_c", c)     \
    OBFH_SF_EPILOGUE                        \
    OBFH_SF_GAP                             \
    OBFH_SF_ENTRY("1", "sf_frame_a", a)     \
    OBFH_SF_CALL_AT("3b", "2")              \
    OBFH_SF_EPILOGUE                        \
    OBFH_SF_GAP                             \
    OBFH_SF_ENTRY_ALT("2", "sf_frame_b", b) \
    OBFH_SF_CALL_AT("3b", "3")              \
    OBFH_SF_EPILOGUE_ALT
#define OBFH_SF_LAYOUT_9(a, b, c)                                                     \
    OBFH_SF_CALL_AT("1f", "0")                                                        \
    OBFH_SF_EPILOGUE                                                                  \
    OBFH_SF_GAP                                                                       \
    OBFH_SF_ENTRY("3", "sf_frame_c", c)                                               \
    OBFH_SF_EPILOGUE                                                                  \
    OBFH_SF_GAP                                                                       \
    OBFH_SF_ENTRY_ALT("2", "sf_frame_b", b)                                           \
    "testl %%eax, %%eax; jz 4f;" OBFH_SF_CALL_AT("3b", "1") "4:" OBFH_SF_EPILOGUE_ALT \
        OBFH_SF_GAP                                                                   \
            OBFH_SF_ENTRY("1", "sf_frame_a", a) "leave; jmp 2b;"

#define OBFH_SF_LAYOUT_10(a, b, c, d)       \
    OBFH_SF_CALL_AT("1f", "0")              \
    OBFH_SF_EPILOGUE                        \
    OBFH_SF_GAP                             \
    OBFH_SF_ENTRY("3", "sf_frame_c", c)     \
    OBFH_SF_EPILOGUE                        \
    OBFH_SF_GAP                             \
    OBFH_SF_ENTRY_ALT("2", "sf_frame_b", b) \
    OBFH_SF_CALL_AT("3b", "1")              \
    OBFH_SF_EPILOGUE_ALT                    \
    OBFH_SF_GAP                             \
    OBFH_SF_ENTRY("1", "sf_frame_a", a)     \
    OBFH_SF_CALL_AT("2b", "2")              \
    OBFH_SF_EPILOGUE

#define OBFH_SF_LAYOUT_11(a, b, c, d)       \
    OBFH_SF_CALL_AT("1f", "0")              \
    OBFH_SF_EPILOGUE                        \
    OBFH_SF_GAP                             \
    OBFH_SF_ENTRY("2", "sf_frame_b", b)     \
    OBFH_SF_CALL_AT("3f", "1")              \
    OBFH_SF_EPILOGUE                        \
    OBFH_SF_GAP                             \
    OBFH_SF_ENTRY_ALT("1", "sf_frame_a", a) \
    OBFH_SF_CALL_AT("2b", "2")              \
    OBFH_SF_CALL_AT("3f", "3")              \
    OBFH_SF_EPILOGUE_ALT                    \
    OBFH_SF_GAP                             \
    OBFH_SF_ENTRY("3", "sf_frame_c", c)     \
    OBFH_SF_EPILOGUE

#define OBFH_SF_LAYOUT_12(a, b, c, d)       \
    OBFH_SF_CALL_AT("1f", "0")              \
    OBFH_SF_EPILOGUE                        \
    OBFH_SF_GAP                             \
    OBFH_SF_ENTRY("1", "sf_frame_a", a)     \
    OBFH_SF_CALL_AT("2f", "1")              \
    OBFH_SF_CALL_AT("8f", "2")              \
    OBFH_SF_EPILOGUE                        \
    OBFH_SF_GAP                             \
    OBFH_SF_ENTRY("2", "sf_frame_b", b)     \
    OBFH_SF_CALL_AT("3f", "3")              \
    OBFH_SF_EPILOGUE                        \
    OBFH_SF_GAP                             \
    OBFH_SF_ENTRY_ALT("3", "sf_frame_c", c) \
    OBFH_SF_EPILOGUE_ALT                    \
    OBFH_SF_GAP                             \
    OBFH_SF_ENTRY("8", "sf_frame_d", d)     \
    OBFH_SF_EPILOGUE

#define OBFH_SF_LAYOUT_13(a, b, c, d)       \
    OBFH_SF_CALL_AT("1f", "0")              \
    OBFH_SF_EPILOGUE                        \
    OBFH_SF_GAP                             \
    OBFH_SF_ENTRY("1", "sf_frame_a", a)     \
    OBFH_SF_CALL_AT("2f", "1")              \
    OBFH_SF_EPILOGUE                        \
    OBFH_SF_GAP                             \
    OBFH_SF_ENTRY_ALT("2", "sf_frame_b", b) \
    OBFH_SF_CALL_AT("3f", "2")              \
    OBFH_SF_EPILOGUE_ALT                    \
    OBFH_SF_GAP                             \
    OBFH_SF_ENTRY("3", "sf_frame_c", c)     \
    "leave; jmp 8f;" OBFH_SF_GAP            \
        OBFH_SF_ENTRY("8", "sf_frame_d", d) \
            OBFH_SF_EPILOGUE

#define OBFH_SF_LAYOUT_14(a, b, c, d)       \
    OBFH_SF_CALL_AT("1f", "0")              \
    OBFH_SF_EPILOGUE                        \
    OBFH_SF_GAP                             \
    OBFH_SF_ENTRY("8", "sf_frame_d", d)     \
    OBFH_SF_CALL_AT("1f", "1")              \
    OBFH_SF_EPILOGUE                        \
    OBFH_SF_GAP                             \
    OBFH_SF_ENTRY_ALT("3", "sf_frame_c", c) \
    OBFH_SF_EPILOGUE_ALT                    \
    OBFH_SF_GAP                             \
    OBFH_SF_ENTRY("1", "sf_frame_a", a)     \
    OBFH_SF_CALL_AT("2f", "2")              \
    OBFH_SF_EPILOGUE                        \
    OBFH_SF_GAP                             \
    OBFH_SF_ENTRY("2", "sf_frame_b", b)     \
    OBFH_SF_CALL_AT("3b", "3")              \
    OBFH_SF_CALL_AT("8b", "4")              \
    OBFH_SF_EPILOGUE

#define OBFH_SF_LAYOUT_15(a, b, c, d)                                                                                      \
    OBFH_SF_CALL_AT("1f", "0")                                                                                             \
    OBFH_SF_EPILOGUE                                                                                                       \
    OBFH_SF_GAP                                                                                                            \
    OBFH_SF_ENTRY_ALT("1", "sf_frame_a", a)                                                                                \
    "testl $1, %%eax; jz 4f;" OBFH_SF_CALL_AT("2f", "1") "jmp 5f; 4:" OBFH_SF_CALL_AT("8f", "2") "5:" OBFH_SF_EPILOGUE_ALT \
        OBFH_SF_GAP                                                                                                        \
            OBFH_SF_ENTRY("2", "sf_frame_b", b)                                                                            \
                OBFH_SF_CALL_AT("3f", "3")                                                                                 \
                    OBFH_SF_EPILOGUE                                                                                       \
                        OBFH_SF_GAP                                                                                        \
                            OBFH_SF_ENTRY("8", "sf_frame_d", d)                                                            \
                                OBFH_SF_CALL_AT("3f", "4")                                                                 \
                                    OBFH_SF_EPILOGUE                                                                       \
                                        OBFH_SF_GAP                                                                        \
                                            OBFH_SF_ENTRY("3", "sf_frame_c", c)                                            \
                                                OBFH_SF_EPILOGUE

#define OBFH_SF_LAYOUT_16(a, b, c, d)                                                       \
    OBFH_SF_CALL_AT("1f", "0")                                                              \
    OBFH_SF_EPILOGUE                                                                        \
    OBFH_SF_GAP                                                                             \
    OBFH_SF_ENTRY("1", "sf_frame_a", a)                                                     \
    OBFH_SF_CALL_AT("2f", "1")                                                              \
    OBFH_SF_EPILOGUE                                                                        \
    OBFH_SF_GAP                                                                             \
    OBFH_SF_ENTRY("2", "sf_frame_b", b)                                                     \
    "testl %%edx, %%edx; jz 4f;" OBFH_SF_CALL_AT("2b", "2") "4:" OBFH_SF_CALL_AT("3f", "3") \
        OBFH_SF_EPILOGUE                                                                    \
            OBFH_SF_GAP                                                                     \
                OBFH_SF_ENTRY_ALT("3", "sf_frame_c", c)                                     \
                    OBFH_SF_EPILOGUE_ALT

#define OBFH_SF_LAYOUT_17(a, b, c, d)                                                                                    \
    OBFH_SF_CALL_AT("1f", "0")                                                                                           \
    OBFH_SF_EPILOGUE                                                                                                     \
    OBFH_SF_GAP                                                                                                          \
    OBFH_SF_ENTRY("1", "sf_frame_a", a)                                                                                  \
    "cmpl %%edx, %%eax; jb 4f;" OBFH_SF_CALL_AT("2f", "1") "jmp 5f; 4:" OBFH_SF_CALL_AT("3f", "2") "5:" OBFH_SF_EPILOGUE \
        OBFH_SF_GAP                                                                                                      \
            OBFH_SF_ENTRY("2", "sf_frame_b", b) "leave; jmp 3f;" OBFH_SF_GAP                                             \
                OBFH_SF_ENTRY_ALT("3", "sf_frame_c", c)                                                                  \
                    OBFH_SF_CALL_AT("1b", "3")                                                                           \
                        OBFH_SF_EPILOGUE_ALT

#define OBFH_SF_LAYOUT_18(a, b, c, d)       \
    OBFH_SF_CALL_AT("3f", "0")              \
    OBFH_SF_EPILOGUE                        \
    OBFH_SF_GAP                             \
    OBFH_SF_ENTRY("1", "sf_frame_a", a)     \
    OBFH_SF_EPILOGUE                        \
    OBFH_SF_GAP                             \
    OBFH_SF_ENTRY_ALT("2", "sf_frame_b", b) \
    OBFH_SF_CALL_AT("1b", "1")              \
    OBFH_SF_EPILOGUE_ALT                    \
    OBFH_SF_GAP                             \
    OBFH_SF_ENTRY("3", "sf_frame_c", c)     \
    OBFH_SF_CALL_AT("2b", "2")              \
    OBFH_SF_EPILOGUE

#define OBFH_SF_LAYOUT_19(a, b, c, d)                                                                                  \
    "testl $1, %%eax; jz 4f;" OBFH_SF_CALL_AT("1f", "0") "jmp 5f; 4:" OBFH_SF_CALL_AT("3f", "1") "5:" OBFH_SF_EPILOGUE \
        OBFH_SF_GAP                                                                                                    \
            OBFH_SF_ENTRY("1", "sf_frame_a", a)                                                                        \
                OBFH_SF_CALL_AT("2f", "2")                                                                             \
                    OBFH_SF_EPILOGUE                                                                                   \
                        OBFH_SF_GAP                                                                                    \
                            OBFH_SF_ENTRY_ALT("2", "sf_frame_b", b)                                                    \
    OBFH_SF_EPILOGUE_ALT                                                                                               \
    OBFH_SF_GAP                                                                                                        \
    OBFH_SF_ENTRY("3", "sf_frame_c", c)                                                                                \
    OBFH_SF_CALL_AT("2b", "3")                                                                                         \
    OBFH_SF_EPILOGUE

#define OBFH_SF_LAYOUT_20(a, b, c, d)                                                                                  \
    OBFH_SF_CALL_AT("1f", "0")                                                                                         \
    OBFH_SF_EPILOGUE                                                                                                   \
    OBFH_SF_GAP                                                                                                        \
    OBFH_SF_ENTRY("2", "sf_frame_b", b)                                                                                \
    OBFH_SF_CALL_AT("8f", "1")                                                                                         \
    OBFH_SF_EPILOGUE                                                                                                   \
    OBFH_SF_GAP                                                                                                        \
    OBFH_SF_ENTRY_ALT("8", "sf_frame_d", d)                                                                            \
    OBFH_SF_CALL_AT("3f", "2")                                                                                         \
    OBFH_SF_EPILOGUE_ALT                                                                                               \
    OBFH_SF_GAP                                                                                                        \
    OBFH_SF_ENTRY("1", "sf_frame_a", a)                                                                                \
    "testl $1, %%eax; jz 4f;" OBFH_SF_CALL_AT("2b", "3") "jmp 5f; 4:" OBFH_SF_CALL_AT("3f", "4") "5:" OBFH_SF_EPILOGUE \
        OBFH_SF_GAP                                                                                                    \
            OBFH_SF_ENTRY("3", "sf_frame_c", c)                                                                        \
                OBFH_SF_CALL_AT("1b", "5")                                                                             \
                    OBFH_SF_EPILOGUE

#define OBFH_SF_LAYOUT_21(a, b, c, d)       \
    OBFH_SF_CALL_AT("1f", "0")              \
    OBFH_SF_EPILOGUE                        \
    OBFH_SF_GAP                             \
    OBFH_SF_ENTRY("3", "sf_frame_c", c)     \
    OBFH_SF_EPILOGUE                        \
    OBFH_SF_GAP                             \
    OBFH_SF_ENTRY_ALT("8", "sf_frame_d", d) \
    OBFH_SF_CALL_AT("3b", "1")              \
    OBFH_SF_EPILOGUE_ALT                    \
    OBFH_SF_GAP                             \
    OBFH_SF_ENTRY("2", "sf_frame_b", b)     \
    OBFH_SF_CALL_AT("8b", "2")              \
    OBFH_SF_EPILOGUE                        \
    OBFH_SF_GAP                             \
    OBFH_SF_ENTRY("1", "sf_frame_a", a)     \
    OBFH_SF_CALL_AT("2b", "3")              \
    OBFH_SF_CALL_AT("3b", "4")              \
    OBFH_SF_EPILOGUE

// Skipped native helpers: register-only and scratch-stack forms share no RBP frame.
// Their arithmetic/opcodes vary per expansion; all stack edits follow the live guard.
#if defined(__x86_64__)
#define OBFH_SF_LEAF_ARGS OBFH_SF_ARGS ".fill (%c[sf_phantom] & 1), 1, 0x90;"
#define OBFH_SF_LEAF_ENTER "subq $%c[sf_leaf_frame], %%rsp;"
#define OBFH_SF_LEAF_STORE "movl %%eax, %c[sf_leaf_slot](%%rsp);"
#define OBFH_SF_LEAF_LOAD "movl %c[sf_leaf_slot](%%rsp), %%ecx;"
#define OBFH_SF_LEAF_EXIT "addq $%c[sf_leaf_frame], %%rsp; ret;"
#else
#define OBFH_SF_LEAF_ARGS "movl 4(%%esp), %%eax; movl 8(%%esp), %%edx; .fill (%c[sf_phantom] & 1), 1, 0x90;"
#define OBFH_SF_LEAF_ENTER "subl $%c[sf_leaf_frame], %%esp;"
#define OBFH_SF_LEAF_STORE "movl %%eax, %c[sf_leaf_slot](%%esp);"
#define OBFH_SF_LEAF_LOAD "movl %c[sf_leaf_slot](%%esp), %%ecx;"
#define OBFH_SF_LEAF_EXIT "addl $%c[sf_leaf_frame], %%esp; ret;"
#endif
// Only valid register ALU/shift encodings are selected. These are instructions,
// not arbitrary bytes: immediate lengths and register operands remain fixed.
#define OBFH_SF_LEAF_ALU_A ".byte %c[sf_leaf_alu_a]; .long %c[sf_key_a];"
#define OBFH_SF_LEAF_ALU_B ".byte 0x81, %c[sf_leaf_alu_b]; .long %c[sf_key_b];"
#define OBFH_SF_LEAF_SHIFT ".byte 0xc1, %c[sf_leaf_shift], %c[sf_rotate];"
#define OBFH_SF_LEAF_A OBFH_SF_LEAF_ARGS OBFH_SF_LEAF_ALU_A "cmpl %%edx, %%eax; jbe 6f;" OBFH_SF_LEAF_SHIFT "subl %%edx, %%eax; ret; 6:" OBFH_SF_LEAF_ALU_B "leal (%%eax, %%edx, 2), %%eax; ret;"
#define OBFH_SF_LEAF_B OBFH_SF_LEAF_ARGS OBFH_SF_LEAF_ENTER OBFH_SF_LEAF_STORE OBFH_SF_LEAF_ALU_A OBFH_SF_LEAF_SHIFT OBFH_SF_LEAF_LOAD "xorl %%ecx, %%eax; addl %%edx, %%eax;" OBFH_SF_LEAF_EXIT
#define OBFH_SF_LEAF_C OBFH_SF_LEAF_ARGS OBFH_SF_LEAF_ENTER OBFH_SF_LEAF_STORE "movl %%edx, %%ecx; roll $%c[sf_rotate], %%ecx; cmpl %%ecx, %%eax; jge 6f;" OBFH_SF_LEAF_ALU_A "xorl %%ecx, %%eax; jmp 7f; 6:" OBFH_SF_LEAF_SHIFT "addl %%edx, %%eax; 7:" OBFH_SF_LEAF_LOAD "subl %%ecx, %%eax;" OBFH_SF_LEAF_EXIT
#define OBFH_SF_LEAF_D OBFH_SF_LEAF_ARGS "movl $%c[sf_loops], %%ecx; 6:" OBFH_SF_LEAF_ALU_A OBFH_SF_LEAF_SHIFT OBFH_SF_LEAF_ALU_B "addl %%edx, %%eax; decl %%ecx; jnz 6b; ret;"
#define OBFH_SF_LEAF_E OBFH_SF_LEAF_ARGS "cmpl $%c[sf_key_a], %%eax; jb 6f; bswap %%eax;" OBFH_SF_LEAF_ALU_A "ret; 6: movl $%c[sf_factor], %%ecx; xorl %%edx, %%edx; divl %%ecx; xorl %%edx, %%eax;" OBFH_SF_LEAF_SHIFT "ret;"
#define OBFH_SF_LEAF_F OBFH_SF_LEAF_ARGS OBFH_SF_LEAF_ENTER OBFH_SF_LEAF_STORE "subl %%edx, %%eax; negl %%eax;" OBFH_SF_LEAF_ALU_A OBFH_SF_LEAF_LOAD "xorl %%ecx, %%eax;" OBFH_SF_LEAF_ALU_B "adcl %%edx, %%eax;" OBFH_SF_LEAF_EXIT
#define OBFH_SF_LEAF_G OBFH_SF_LEAF_ARGS "movl %%eax, %%ecx;" OBFH_SF_LEAF_SHIFT "testl $%c[sf_mask], %%ecx; jnz 6f;" OBFH_SF_LEAF_ALU_A "ret; 6: imull $%c[sf_factor], %%eax;" OBFH_SF_LEAF_ALU_B "xorl %%edx, %%eax; ret;"
#define OBFH_SF_LEAF_H OBFH_SF_LEAF_ARGS OBFH_SF_LEAF_ENTER OBFH_SF_LEAF_STORE "movl $%c[sf_loops], %%ecx; 6:" OBFH_SF_LEAF_SHIFT "addl %%edx, %%eax; decl %%ecx; jnz 6b;" OBFH_SF_LEAF_LOAD "xorl %%ecx, %%eax;" OBFH_SF_LEAF_EXIT
#define OBFH_SF_LEAF_I OBFH_SF_LEAF_ARGS "bswap %%eax;" OBFH_SF_LEAF_ALU_A "cmpl %%edx, %%eax; jne 6f;" OBFH_SF_LEAF_SHIFT "ret; 6: imull $%c[sf_factor], %%eax; subl %%edx, %%eax; ret;"
#define OBFH_SF_LEAF_J OBFH_SF_LEAF_ARGS OBFH_SF_LEAF_ENTER OBFH_SF_LEAF_STORE OBFH_SF_LEAF_ALU_B "testl %%edx, %%edx; js 6f;" OBFH_SF_LEAF_SHIFT "jmp 7f; 6: negl %%eax; 7:" OBFH_SF_LEAF_LOAD "addl %%ecx, %%eax;" OBFH_SF_LEAF_EXIT
#define OBFH_SF_LEAF_K OBFH_SF_LEAF_ARGS "movzbl %%al, %%ecx; shrl $8, %%eax; imull $%c[sf_factor], %%ecx; xorl %%ecx, %%eax;" OBFH_SF_LEAF_ALU_A OBFH_SF_LEAF_SHIFT "addl %%edx, %%eax; ret;"
#define OBFH_SF_LEAF_L OBFH_SF_LEAF_ARGS OBFH_SF_LEAF_ENTER OBFH_SF_LEAF_STORE "cmpl %%edx, %%eax; jbe 6f;" OBFH_SF_LEAF_ALU_A "jmp 7f; 6:" OBFH_SF_LEAF_ALU_B "xorl %%edx, %%eax; 7:" OBFH_SF_LEAF_LOAD "subl %%ecx, %%eax;" OBFH_SF_LEAF_EXIT
#define OBFH_SF_LEAF_ENTRY(label, body) label ": " body
#define OBFH_SF_LAYOUT_22(a, b, c, d)   \
    OBFH_SF_CALL_AT("1f", "0")          \
    OBFH_SF_EPILOGUE OBFH_SF_GAP        \
        OBFH_SF_LEAF_ENTRY("2", c)      \
    OBFH_SF_GAP                         \
    OBFH_SF_ENTRY("1", "sf_frame_a", a) \
    OBFH_SF_CALL_AT("2b", "1")          \
    OBFH_SF_EPILOGUE_ALT
#define OBFH_SF_LAYOUT_23(a, b, c, d) \
    OBFH_SF_CALL_AT("1f", "0")        \
    OBFH_SF_EPILOGUE_ALT OBFH_SF_GAP  \
        OBFH_SF_LEAF_ENTRY("1", c)    \
    OBFH_SF_GAP OBFH_SF_LEAF_ENTRY("2", d)
#define OBFH_SF_LAYOUT_24(a, b, c, d)                                                                                                  \
    OBFH_SF_CALL_AT("1f", "0")                                                                                                         \
    OBFH_SF_EPILOGUE OBFH_SF_GAP                                                                                                       \
        OBFH_SF_ENTRY_ALT("1", "sf_frame_a", a) OBFH_SF_CALL_AT("3f", "1") OBFH_SF_CALL_AT("2f", "2") OBFH_SF_EPILOGUE_ALT OBFH_SF_GAP \
            OBFH_SF_LEAF_ENTRY("2", c)                                                                                                 \
    OBFH_SF_GAP OBFH_SF_LEAF_ENTRY("3", d)
#define OBFH_SF_LAYOUT_25(a, b, c, d)   \
    OBFH_SF_CALL_AT("1f", "0")          \
    OBFH_SF_EPILOGUE_ALT OBFH_SF_GAP    \
        OBFH_SF_LEAF_ENTRY("3", d)      \
    OBFH_SF_GAP                         \
    OBFH_SF_ENTRY("1", "sf_frame_a", a) \
    OBFH_SF_CALL_AT("2f", "1")          \
    OBFH_SF_EPILOGUE OBFH_SF_GAP        \
        OBFH_SF_ENTRY_ALT("2", "sf_frame_b", b) OBFH_SF_CALL_AT("3b", "2") OBFH_SF_EPILOGUE_ALT
#define OBFH_SF_LAYOUT_26(a, b, c, d)                                                             \
    OBFH_SF_CALL_AT("1f", "0")                                                                    \
    OBFH_SF_EPILOGUE OBFH_SF_GAP                                                                  \
        OBFH_SF_LEAF_ENTRY("2", c)                                                                \
    OBFH_SF_GAP                                                                                   \
    OBFH_SF_ENTRY_ALT("1", "sf_frame_a", a)                                                       \
    "testl %%eax, %%eax; jz 4f;" OBFH_SF_CALL_AT("2b", "1") "4:" OBFH_SF_EPILOGUE_ALT OBFH_SF_GAP \
        OBFH_SF_LEAF_ENTRY("3", d)
#define OBFH_SF_LAYOUT_27(a, b, c, d)      \
    OBFH_SF_CALL_AT("2f", "0")             \
    OBFH_SF_EPILOGUE_ALT OBFH_SF_GAP       \
        OBFH_SF_LEAF_ENTRY("1", c)         \
    OBFH_SF_GAP OBFH_SF_LEAF_ENTRY("3", d) \
    OBFH_SF_GAP                            \
    OBFH_SF_ENTRY("2", "sf_frame_b", b)    \
    OBFH_SF_CALL_AT("1b", "1")             \
    OBFH_SF_CALL_AT("3b", "2")             \
    OBFH_SF_EPILOGUE

// Compact equivalent ASM fragments. Keep the readable layouts above.
// Regenerate and verify with node tests/compact_asm.js.
// BEGIN COMPACT ASM CACHE
#if defined(__x86_64__)
#undef OBFH_SF_BYTES
#define OBFH_SF_BYTES(bit, flip, size, value) ".fill (((%c[sf_phantom] >> " bit ") & 1) ^ " flip "), " size ", " value ";"
#undef OBFH_SF_WORD
#define OBFH_SF_WORD(bit, flip, value) ".fill (((%c[sf_phantom] >> " bit ") & 1) ^ " flip "), 4, " value ";"
#undef OBFH_SF_LINK_BYTES
#define OBFH_SF_LINK_BYTES(channel, size, value) ".fill (((%c[sf_phantom] ^ %c[sf_key_a]) >> (12 + 3 * " channel ")) & 1), " size ", " value ";"
#undef OBFH_SF_LINK_GATE
#define OBFH_SF_LINK_GATE(span, channel) ".fill (((%c[sf_phantom] ^ %c[sf_key_a]) >> (12 + 3 * " channel ")) & 1), 2, 0xc085 ^ ((((%c[sf_phantom] ^ %c[sf_key_a]) >> (14 + 3 * " channel ")) & 1) << 12);.fill (((%c[sf_phantom] ^ %c[sf_key_a]) >> (12 + 3 * " channel ")) & 1), 1, 0x74 | (((%c[sf_phantom] ^ %c[sf_key_a]) >> (13 + 3 * " channel ")) & 1);.fill (((%c[sf_phantom] ^ %c[sf_key_a]) >> (12 + 3 * " channel ")) & 1), 1, " span ";"
#undef OBFH_SF_INPUT
#define OBFH_SF_INPUT "movl %%esp, %%eax;"
#undef OBFH_SF_FRAME_HEAD
#define OBFH_SF_FRAME_HEAD(size) "pushq %%rbp;.fill (((%c[sf_phantom] >> (1 + ((%c[" size "] >> 4) & 3))) & 1) ^ 0), 3, 0xe58948;.fill (((%c[sf_phantom] >> (1 + ((%c[" size "] >> 4) & 3))) & 1) ^ 1), 4, 0x242c8d48;"
#undef OBFH_SF_FRAME
#define OBFH_SF_FRAME(size) "pushq %%rbp;.fill (((%c[sf_phantom] >> (1 + ((%c[" size "] >> 4) & 3))) & 1) ^ 0), 3, 0xe58948;.fill (((%c[sf_phantom] >> (1 + ((%c[" size "] >> 4) & 3))) & 1) ^ 1), 4, 0x242c8d48;.fill (((%c[sf_phantom] >> (4 + ((%c[" size "] >> 4) & 3))) & 1) ^ 0), 3, 0xec8148;.fill (((%c[sf_phantom] >> (4 + ((%c[" size "] >> 4) & 3))) & 1) ^ 0), 4, %c[" size "];.fill (((%c[sf_phantom] >> (4 + ((%c[" size "] >> 4) & 3))) & 1) ^ 1), 4, 0x24a48d48;.fill (((%c[sf_phantom] >> (4 + ((%c[" size "] >> 4) & 3))) & 1) ^ 1), 4, -%c[" size "];"
#undef OBFH_SF_FRAME_ALT
#define OBFH_SF_FRAME_ALT(size) "pushq %%rbp;.fill (((%c[sf_phantom] >> (4 + ((%c[" size "] >> 4) & 3))) & 1) ^ 0), 3, 0xec8148;.fill (((%c[sf_phantom] >> (4 + ((%c[" size "] >> 4) & 3))) & 1) ^ 0), 4, %c[" size "];.fill (((%c[sf_phantom] >> (4 + ((%c[" size "] >> 4) & 3))) & 1) ^ 1), 4, 0x24a48d48;.fill (((%c[sf_phantom] >> (4 + ((%c[" size "] >> 4) & 3))) & 1) ^ 1), 4, -%c[" size "];.fill 1, 4, 0x24ac8d48; .long %c[" size "];"
#undef OBFH_SF_EPILOGUE_ALT
#define OBFH_SF_EPILOGUE_ALT ".fill (((%c[sf_phantom] >> 7) & 1) ^ 0), 3, 0xec8948;.fill (((%c[sf_phantom] >> 7) & 1) ^ 1), 4, 0x00658d48;popq %%rbp; ret;"
#undef OBFH_SF_EPILOGUE
#define OBFH_SF_EPILOGUE ".fill (((%c[sf_phantom] >> 6) & 1) ^ 0), 1, 0xc9;.fill (((%c[sf_phantom] >> 6) & 1) ^ 1), 4, 0x5dec8948;ret;"
#undef OBFH_SF_ARGS
#define OBFH_SF_ARGS ".fill (((%c[sf_phantom] >> 9) & 1) ^ 0), 2, 0xc889;.fill (((%c[sf_phantom] >> 9) & 1) ^ 1), 3, 0x00418d;"
#undef OBFH_SF_CALL_ARGS
#define OBFH_SF_CALL_ARGS ".fill (((%c[sf_phantom] >> 10) & 1) ^ 0), 1, 0xb9;.fill (((%c[sf_phantom] >> 10) & 1) ^ 0), 4, %c[sf_arg_a];.fill (((%c[sf_phantom] >> 10) & 1) ^ 0), 1, 0xba;.fill (((%c[sf_phantom] >> 10) & 1) ^ 0), 4, %c[sf_arg_b];.fill (((%c[sf_phantom] >> 10) & 1) ^ 1), 1, 0xba;.fill (((%c[sf_phantom] >> 10) & 1) ^ 1), 4, %c[sf_arg_b];.fill (((%c[sf_phantom] >> 10) & 1) ^ 1), 1, 0xb9;.fill (((%c[sf_phantom] >> 10) & 1) ^ 1), 4, %c[sf_arg_a];"
#undef OBFH_SF_CALL_AT
#define OBFH_SF_CALL_AT(target, channel) ".fill (((%c[sf_phantom] ^ %c[sf_key_a]) >> (12 + 3 * " channel ")) & 1), 2, 0xc085 ^ ((((%c[sf_phantom] ^ %c[sf_key_a]) >> (14 + 3 * " channel ")) & 1) << 12);.fill (((%c[sf_phantom] ^ %c[sf_key_a]) >> (12 + 3 * " channel ")) & 1), 1, 0x74 | (((%c[sf_phantom] ^ %c[sf_key_a]) >> (13 + 3 * " channel ")) & 1);.fill (((%c[sf_phantom] ^ %c[sf_key_a]) >> (12 + 3 * " channel ")) & 1), 1, 23;subq $32, %%rsp;.fill (((%c[sf_phantom] >> 10) & 1) ^ 0), 1, 0xb9;.fill (((%c[sf_phantom] >> 10) & 1) ^ 0), 4, %c[sf_arg_a];.fill (((%c[sf_phantom] >> 10) & 1) ^ 0), 1, 0xba;.fill (((%c[sf_phantom] >> 10) & 1) ^ 0), 4, %c[sf_arg_b];.fill (((%c[sf_phantom] >> 10) & 1) ^ 1), 1, 0xba;.fill (((%c[sf_phantom] >> 10) & 1) ^ 1), 4, %c[sf_arg_b];.fill (((%c[sf_phantom] >> 10) & 1) ^ 1), 1, 0xb9;.fill (((%c[sf_phantom] >> 10) & 1) ^ 1), 4, %c[sf_arg_a];call " target "; addq $32, %%rsp;"
#undef OBFH_SF_LOCAL
#define OBFH_SF_LOCAL ".fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 2, 0x4589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 1, -%c[sf_local_a];.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 2, 0x5589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 1, -%c[sf_local_b];.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 2, 0x5589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 1, -%c[sf_local_b];.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 2, 0x4589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 1, -%c[sf_local_a];"
#undef OBFH_SF_CALL
#define OBFH_SF_CALL(target) ".fill (((%c[sf_phantom] ^ %c[sf_key_a]) >> (12 + 3 * 0)) & 1), 2, 0xc085 ^ ((((%c[sf_phantom] ^ %c[sf_key_a]) >> (14 + 3 * 0)) & 1) << 12);.fill (((%c[sf_phantom] ^ %c[sf_key_a]) >> (12 + 3 * 0)) & 1), 1, 0x74 | (((%c[sf_phantom] ^ %c[sf_key_a]) >> (13 + 3 * 0)) & 1);.fill (((%c[sf_phantom] ^ %c[sf_key_a]) >> (12 + 3 * 0)) & 1), 1, 23;subq $32, %%rsp;.fill (((%c[sf_phantom] >> 10) & 1) ^ 0), 1, 0xb9;.fill (((%c[sf_phantom] >> 10) & 1) ^ 0), 4, %c[sf_arg_a];.fill (((%c[sf_phantom] >> 10) & 1) ^ 0), 1, 0xba;.fill (((%c[sf_phantom] >> 10) & 1) ^ 0), 4, %c[sf_arg_b];.fill (((%c[sf_phantom] >> 10) & 1) ^ 1), 1, 0xba;.fill (((%c[sf_phantom] >> 10) & 1) ^ 1), 4, %c[sf_arg_b];.fill (((%c[sf_phantom] >> 10) & 1) ^ 1), 1, 0xb9;.fill (((%c[sf_phantom] >> 10) & 1) ^ 1), 4, %c[sf_arg_a];call " target "; addq $32, %%rsp;"
#undef OBFH_SF_PADDING
#define OBFH_SF_PADDING ".fill %c[sf_pad], 1, 0x90;"
#undef OBFH_SF_GAP
#define OBFH_SF_GAP ".fill %c[sf_pad_b], 1, 0x90; .byte %c[sf_noise_a], %c[sf_noise_b];"
#undef OBFH_SF_BODY_A
#define OBFH_SF_BODY_A ".fill (((%c[sf_phantom] >> 9) & 1) ^ 0), 2, 0xc889;.fill (((%c[sf_phantom] >> 9) & 1) ^ 1), 3, 0x00418d;xorl $%c[sf_key_a], %%eax; imull $%c[sf_factor], %%eax; addl %%edx, %%eax;.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 2, 0x4589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 1, -%c[sf_local_a];.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 2, 0x5589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 1, -%c[sf_local_b];.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 2, 0x5589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 1, -%c[sf_local_b];.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 2, 0x4589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 1, -%c[sf_local_a];"
#undef OBFH_SF_BODY_B
#define OBFH_SF_BODY_B ".fill (((%c[sf_phantom] >> 9) & 1) ^ 0), 2, 0xc889;.fill (((%c[sf_phantom] >> 9) & 1) ^ 1), 3, 0x00418d;addl $%c[sf_key_b], %%eax; roll $%c[sf_rotate], %%eax; xorl %%edx, %%eax;.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 2, 0x4589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 1, -%c[sf_local_a];.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 2, 0x5589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 1, -%c[sf_local_b];.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 2, 0x5589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 1, -%c[sf_local_b];.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 2, 0x4589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 1, -%c[sf_local_a];"
#undef OBFH_SF_BODY_C
#define OBFH_SF_BODY_C ".fill (((%c[sf_phantom] >> 9) & 1) ^ 0), 2, 0xc889;.fill (((%c[sf_phantom] >> 9) & 1) ^ 1), 3, 0x00418d;leal (%%eax, %%eax, 2), %%eax; xorl %%eax, %%edx; addl $%c[sf_key_a], %%eax;.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 2, 0x4589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 1, -%c[sf_local_a];.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 2, 0x5589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 1, -%c[sf_local_b];.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 2, 0x5589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 1, -%c[sf_local_b];.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 2, 0x4589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 1, -%c[sf_local_a];"
#undef OBFH_SF_BODY_D
#define OBFH_SF_BODY_D ".fill (((%c[sf_phantom] >> 9) & 1) ^ 0), 2, 0xc889;.fill (((%c[sf_phantom] >> 9) & 1) ^ 1), 3, 0x00418d;notl %%eax; addl %%edx, %%eax; imull $%c[sf_factor], %%eax; roll $%c[sf_rotate], %%eax;.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 2, 0x4589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 1, -%c[sf_local_a];.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 2, 0x5589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 1, -%c[sf_local_b];.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 2, 0x5589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 1, -%c[sf_local_b];.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 2, 0x4589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 1, -%c[sf_local_a];"
#undef OBFH_SF_BODY_E
#define OBFH_SF_BODY_E ".fill (((%c[sf_phantom] >> 9) & 1) ^ 0), 2, 0xc889;.fill (((%c[sf_phantom] >> 9) & 1) ^ 1), 3, 0x00418d;bswap %%eax; xorl %%edx, %%eax; roll $%c[sf_rotate], %%eax;.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 2, 0x4589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 1, -%c[sf_local_a];.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 2, 0x5589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 1, -%c[sf_local_b];.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 2, 0x5589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 1, -%c[sf_local_b];.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 2, 0x4589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 1, -%c[sf_local_a];"
#undef OBFH_SF_BODY_F
#define OBFH_SF_BODY_F ".fill (((%c[sf_phantom] >> 9) & 1) ^ 0), 2, 0xc889;.fill (((%c[sf_phantom] >> 9) & 1) ^ 1), 3, 0x00418d;movl $%c[sf_loops], %%ecx; 6: addl %%edx, %%eax; imull $%c[sf_factor], %%eax; xorl $%c[sf_key_b], %%eax; decl %%ecx; jnz 6b;.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 2, 0x4589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 1, -%c[sf_local_a];.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 2, 0x5589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 1, -%c[sf_local_b];.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 2, 0x5589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 1, -%c[sf_local_b];.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 2, 0x4589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 1, -%c[sf_local_a];"
#undef OBFH_SF_BODY_G
#define OBFH_SF_BODY_G ".fill (((%c[sf_phantom] >> 9) & 1) ^ 0), 2, 0xc889;.fill (((%c[sf_phantom] >> 9) & 1) ^ 1), 3, 0x00418d;movzbl %%al, %%ecx; shrl $8, %%eax; xorl %%edx, %%ecx; leal (%%eax, %%ecx, 4), %%eax;.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 2, 0x4589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 1, -%c[sf_local_a];.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 2, 0x5589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 1, -%c[sf_local_b];.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 2, 0x5589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 1, -%c[sf_local_b];.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 2, 0x4589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 1, -%c[sf_local_a];"
#undef OBFH_SF_BODY_H
#define OBFH_SF_BODY_H ".fill (((%c[sf_phantom] >> 9) & 1) ^ 0), 2, 0xc889;.fill (((%c[sf_phantom] >> 9) & 1) ^ 1), 3, 0x00418d;imull %%eax, %%edx; xorl $%c[sf_key_b], %%edx; subl %%edx, %%eax; negl %%eax;.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 2, 0x4589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 1, -%c[sf_local_a];.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 2, 0x5589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 1, -%c[sf_local_b];.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 2, 0x5589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 1, -%c[sf_local_b];.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 2, 0x4589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 1, -%c[sf_local_a];"
#undef OBFH_SF_BODY_I
#define OBFH_SF_BODY_I ".fill (((%c[sf_phantom] >> 9) & 1) ^ 0), 2, 0xc889;.fill (((%c[sf_phantom] >> 9) & 1) ^ 1), 3, 0x00418d;movl %%eax, %%ecx; orl $%c[sf_mask], %%eax; andl $%c[sf_mask], %%ecx; subl %%ecx, %%eax; xorl %%edx, %%eax;.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 2, 0x4589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 1, -%c[sf_local_a];.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 2, 0x5589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 1, -%c[sf_local_b];.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 2, 0x5589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 1, -%c[sf_local_b];.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 2, 0x4589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 1, -%c[sf_local_a];"
#undef OBFH_SF_BODY_J
#define OBFH_SF_BODY_J ".fill (((%c[sf_phantom] >> 9) & 1) ^ 0), 2, 0xc889;.fill (((%c[sf_phantom] >> 9) & 1) ^ 1), 3, 0x00418d;xorl $%c[sf_key_a], %%eax; movl %%edx, %%ecx; shll $%c[sf_rotate], %%ecx; shrl $%c[sf_rotate], %%eax; orl %%ecx, %%eax;.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 2, 0x4589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 1, -%c[sf_local_a];.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 2, 0x5589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 1, -%c[sf_local_b];.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 2, 0x5589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 1, -%c[sf_local_b];.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 2, 0x4589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 1, -%c[sf_local_a];"
#undef OBFH_SF_BODY_K
#define OBFH_SF_BODY_K ".fill (((%c[sf_phantom] >> 9) & 1) ^ 0), 2, 0xc889;.fill (((%c[sf_phantom] >> 9) & 1) ^ 1), 3, 0x00418d;testl %%edx, %%edx; js 6f; addl $%c[sf_key_a], %%eax; jmp 7f; 6: negl %%eax; xorl %%edx, %%eax; 7: roll $%c[sf_rotate], %%eax;.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 2, 0x4589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 1, -%c[sf_local_a];.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 2, 0x5589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 1, -%c[sf_local_b];.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 2, 0x5589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 1, -%c[sf_local_b];.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 2, 0x4589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 1, -%c[sf_local_a];"
#undef OBFH_SF_BODY_L
#define OBFH_SF_BODY_L ".fill (((%c[sf_phantom] >> 9) & 1) ^ 0), 2, 0xc889;.fill (((%c[sf_phantom] >> 9) & 1) ^ 1), 3, 0x00418d;movl %%eax, %%ecx; movl %%edx, %%eax; addl %%ecx, %%eax; imull $%c[sf_factor], %%eax; notl %%edx; xorl %%edx, %%eax;.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 2, 0x4589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 1, -%c[sf_local_a];.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 2, 0x5589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 1, -%c[sf_local_b];.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 2, 0x5589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 1, -%c[sf_local_b];.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 2, 0x4589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 1, -%c[sf_local_a];"
#undef OBFH_SF_BODY_M
#define OBFH_SF_BODY_M ".fill (((%c[sf_phantom] >> 9) & 1) ^ 0), 2, 0xc889;.fill (((%c[sf_phantom] >> 9) & 1) ^ 1), 3, 0x00418d;movl %%eax, %%ecx; shrl $16, %%ecx; xorl %%ecx, %%eax; imull $%c[sf_factor], %%eax; xorl %%edx, %%eax;.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 2, 0x4589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 1, -%c[sf_local_a];.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 2, 0x5589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 1, -%c[sf_local_b];.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 2, 0x5589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 1, -%c[sf_local_b];.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 2, 0x4589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 1, -%c[sf_local_a];"
#undef OBFH_SF_BODY_N
#define OBFH_SF_BODY_N ".fill (((%c[sf_phantom] >> 9) & 1) ^ 0), 2, 0xc889;.fill (((%c[sf_phantom] >> 9) & 1) ^ 1), 3, 0x00418d;addl $%c[sf_key_a], %%eax; adcl $%c[sf_key_b], %%edx; xorl %%edx, %%eax; rorl $%c[sf_rotate], %%eax;.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 2, 0x4589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 1, -%c[sf_local_a];.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 2, 0x5589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 1, -%c[sf_local_b];.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 2, 0x5589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 1, -%c[sf_local_b];.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 2, 0x4589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 1, -%c[sf_local_a];"
#undef OBFH_SF_BODY_O
#define OBFH_SF_BODY_O ".fill (((%c[sf_phantom] >> 9) & 1) ^ 0), 2, 0xc889;.fill (((%c[sf_phantom] >> 9) & 1) ^ 1), 3, 0x00418d;xchgl %%eax, %%edx; subl $%c[sf_key_a], %%eax; bswap %%edx; addl %%edx, %%eax;.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 2, 0x4589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 1, -%c[sf_local_a];.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 2, 0x5589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 1, -%c[sf_local_b];.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 2, 0x5589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 1, -%c[sf_local_b];.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 2, 0x4589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 1, -%c[sf_local_a];"
#undef OBFH_SF_BODY_P
#define OBFH_SF_BODY_P ".fill (((%c[sf_phantom] >> 9) & 1) ^ 0), 2, 0xc889;.fill (((%c[sf_phantom] >> 9) & 1) ^ 1), 3, 0x00418d;movl $%c[sf_factor], %%ecx; xorl %%edx, %%edx; divl %%ecx; xorl %%edx, %%eax;.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 2, 0x4589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 1, -%c[sf_local_a];.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 2, 0x5589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 1, -%c[sf_local_b];.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 2, 0x5589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 1, -%c[sf_local_b];.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 2, 0x4589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 1, -%c[sf_local_a];"
#undef OBFH_SF_BODY_Q
#define OBFH_SF_BODY_Q ".fill (((%c[sf_phantom] >> 9) & 1) ^ 0), 2, 0xc889;.fill (((%c[sf_phantom] >> 9) & 1) ^ 1), 3, 0x00418d;movzbl %%al, %%ecx; movzbl %%dl, %%edx; imull %%edx, %%ecx; shrl $8, %%eax; addl %%ecx, %%eax;.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 2, 0x4589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 1, -%c[sf_local_a];.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 2, 0x5589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 1, -%c[sf_local_b];.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 2, 0x5589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 1, -%c[sf_local_b];.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 2, 0x4589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 1, -%c[sf_local_a];"
#undef OBFH_SF_BODY_R
#define OBFH_SF_BODY_R ".fill (((%c[sf_phantom] >> 9) & 1) ^ 0), 2, 0xc889;.fill (((%c[sf_phantom] >> 9) & 1) ^ 1), 3, 0x00418d;movl $%c[sf_loops], %%ecx; 6: xorl %%edx, %%eax; roll $%c[sf_rotate], %%eax; addl $%c[sf_key_a], %%edx; decl %%ecx; jnz 6b;.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 2, 0x4589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 1, -%c[sf_local_a];.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 2, 0x5589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 1, -%c[sf_local_b];.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 2, 0x5589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 1, -%c[sf_local_b];.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 2, 0x4589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 1, -%c[sf_local_a];"
#undef OBFH_SF_BODY_S
#define OBFH_SF_BODY_S ".fill (((%c[sf_phantom] >> 9) & 1) ^ 0), 2, 0xc889;.fill (((%c[sf_phantom] >> 9) & 1) ^ 1), 3, 0x00418d;shldl $%c[sf_rotate], %%edx, %%eax; subl $%c[sf_key_b], %%eax; xorl $%c[sf_mask], %%eax;.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 2, 0x4589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 1, -%c[sf_local_a];.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 2, 0x5589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 1, -%c[sf_local_b];.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 2, 0x5589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 1, -%c[sf_local_b];.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 2, 0x4589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 1, -%c[sf_local_a];"
#undef OBFH_SF_BODY_T
#define OBFH_SF_BODY_T ".fill (((%c[sf_phantom] >> 9) & 1) ^ 0), 2, 0xc889;.fill (((%c[sf_phantom] >> 9) & 1) ^ 1), 3, 0x00418d;movl %%eax, %%ecx; shrl $16, %%eax; shll $16, %%ecx; orl %%ecx, %%eax; xorl %%edx, %%eax; negl %%eax;.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 2, 0x4589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 1, -%c[sf_local_a];.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 2, 0x5589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 1, -%c[sf_local_b];.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 2, 0x5589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 1, -%c[sf_local_b];.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 2, 0x4589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 1, -%c[sf_local_a];"
#undef OBFH_SF_GUARD_0
#define OBFH_SF_GUARD_0 "imull %%eax, %%eax; testl $2, %%eax; jz 9f;"
#undef OBFH_SF_GUARD_1
#define OBFH_SF_GUARD_1 "leal 1(%%eax), %%edx; imull %%edx, %%eax; testl $1, %%eax; jz 9f;"
#undef OBFH_SF_GUARD_2
#define OBFH_SF_GUARD_2 "movl %%eax, %%edx; imull %%eax, %%eax; xorl %%edx, %%eax; testl $1, %%eax; jz 9f;"
#undef OBFH_SF_GUARD_3
#define OBFH_SF_GUARD_3 "movl %%eax, %%edx; notl %%edx; addl %%eax, %%edx; cmpl $-1, %%edx; je 9f;"
#undef OBFH_SF_GUARD_4
#define OBFH_SF_GUARD_4 "movl %%eax, %%edx; movl %%eax, %%ecx; orl $%c[sf_mask], %%eax; andl $%c[sf_mask], %%edx; addl %%edx, %%eax; addl $%c[sf_mask], %%ecx; cmpl %%ecx, %%eax; je 9f;"
#undef OBFH_SF_GUARD_5
#define OBFH_SF_GUARD_5 "movl %%eax, %%edx; roll $%c[sf_rotate], %%eax; rorl $%c[sf_rotate], %%eax; cmpl %%edx, %%eax; je 9f;"
#undef OBFH_SF_GUARD_6
#define OBFH_SF_GUARD_6 "movl %%eax, %%edx; bswap %%eax; bswap %%eax; cmpl %%edx, %%eax; je 9f;"
#undef OBFH_SF_GUARD_7
#define OBFH_SF_GUARD_7 "leal -1(%%eax), %%edx; imull %%edx, %%eax; testl $1, %%eax; jz 9f;"
#undef OBFH_SF_GUARD_8
#define OBFH_SF_GUARD_8 "movl %%eax, %%edx; notl %%edx; xorl $%c[sf_key_a], %%eax; xorl $%c[sf_key_a], %%edx; xorl %%edx, %%eax; cmpl $-1, %%eax; je 9f;"
#undef OBFH_SF_GUARD_9
#define OBFH_SF_GUARD_9 "movl %%eax, %%ecx; movl %%eax, %%edx; xorl $%c[sf_mask], %%eax; andl $%c[sf_mask], %%edx; leal (%%eax, %%edx, 2), %%eax; addl $%c[sf_mask], %%ecx; cmpl %%ecx, %%eax; je 9f;"
#undef OBFH_SF_GUARD_10
#define OBFH_SF_GUARD_10 "movl %%eax, %%edx; negl %%edx; andl %%edx, %%eax; leal -1(%%eax), %%edx; andl %%edx, %%eax; testl %%eax, %%eax; jz 9f;"
#undef OBFH_SF_GUARD_11
#define OBFH_SF_GUARD_11 "movl %%eax, %%edx; movl %%eax, %%ecx; andl $%c[sf_mask], %%eax; andl $%c[sf_not_mask], %%edx; imull $%c[sf_factor], %%eax; imull $%c[sf_factor], %%edx; addl %%edx, %%eax; imull $%c[sf_factor], %%ecx; cmpl %%ecx, %%eax; je 9f;"
#undef OBFH_SF_GUARD_12
#define OBFH_SF_GUARD_12 "movl %%eax, %%edx; addl $%c[sf_key_a], %%eax; imull $%c[sf_factor], %%eax; imull $%c[sf_factor], %%edx; movl $%c[sf_key_a], %%ecx; imull $%c[sf_factor], %%ecx; addl %%edx, %%ecx; cmpl %%ecx, %%eax; je 9f;"
#undef OBFH_SF_GUARD_13
#define OBFH_SF_GUARD_13 "movl %%eax, %%ecx; movl %%eax, %%edx; addl $%c[sf_key_a], %%eax; imull %%eax, %%eax; imull %%ecx, %%ecx; subl %%ecx, %%eax; imull $%c[sf_key_a], %%edx; addl %%edx, %%edx; subl %%edx, %%eax; movl $%c[sf_key_a], %%edx; imull %%edx, %%edx; cmpl %%edx, %%eax; je 9f;"
#undef OBFH_SF_GUARD_14
#define OBFH_SF_GUARD_14 "movl %%eax, %%edx; roll $16, %%eax; roll $16, %%eax; cmpl %%edx, %%eax; je 9f;"
#undef OBFH_SF_ENTRY_ALT
#define OBFH_SF_ENTRY_ALT(label, frame, body) label ": pushq %%rbp;.fill (((%c[sf_phantom] >> (4 + ((%c[" frame "] >> 4) & 3))) & 1) ^ 0), 3, 0xec8148;.fill (((%c[sf_phantom] >> (4 + ((%c[" frame "] >> 4) & 3))) & 1) ^ 0), 4, %c[" frame "];.fill (((%c[sf_phantom] >> (4 + ((%c[" frame "] >> 4) & 3))) & 1) ^ 1), 4, 0x24a48d48;.fill (((%c[sf_phantom] >> (4 + ((%c[" frame "] >> 4) & 3))) & 1) ^ 1), 4, -%c[" frame "];.fill 1, 4, 0x24ac8d48; .long %c[" frame "];" body ".fill ((%c[sf_phantom] >> " label ") & 1), 1, 0x90;"
#undef OBFH_SF_ENTRY
#define OBFH_SF_ENTRY(label, frame, body) label ": pushq %%rbp;.fill (((%c[sf_phantom] >> (1 + ((%c[" frame "] >> 4) & 3))) & 1) ^ 0), 3, 0xe58948;.fill (((%c[sf_phantom] >> (1 + ((%c[" frame "] >> 4) & 3))) & 1) ^ 1), 4, 0x242c8d48;.fill (((%c[sf_phantom] >> (4 + ((%c[" frame "] >> 4) & 3))) & 1) ^ 0), 3, 0xec8148;.fill (((%c[sf_phantom] >> (4 + ((%c[" frame "] >> 4) & 3))) & 1) ^ 0), 4, %c[" frame "];.fill (((%c[sf_phantom] >> (4 + ((%c[" frame "] >> 4) & 3))) & 1) ^ 1), 4, 0x24a48d48;.fill (((%c[sf_phantom] >> (4 + ((%c[" frame "] >> 4) & 3))) & 1) ^ 1), 4, -%c[" frame "];" body ".fill ((%c[sf_phantom] >> " label ") & 1), 1, 0x90;"
#undef OBFH_SF_LEAF_ARGS
#define OBFH_SF_LEAF_ARGS ".fill (((%c[sf_phantom] >> 9) & 1) ^ 0), 2, 0xc889;.fill (((%c[sf_phantom] >> 9) & 1) ^ 1), 3, 0x00418d;.fill (%c[sf_phantom] & 1), 1, 0x90;"
#undef OBFH_SF_LEAF_ENTER
#define OBFH_SF_LEAF_ENTER "subq $%c[sf_leaf_frame], %%rsp;"
#undef OBFH_SF_LEAF_STORE
#define OBFH_SF_LEAF_STORE "movl %%eax, %c[sf_leaf_slot](%%rsp);"
#undef OBFH_SF_LEAF_LOAD
#define OBFH_SF_LEAF_LOAD "movl %c[sf_leaf_slot](%%rsp), %%ecx;"
#undef OBFH_SF_LEAF_EXIT
#define OBFH_SF_LEAF_EXIT "addq $%c[sf_leaf_frame], %%rsp; ret;"
#undef OBFH_SF_LEAF_ALU_A
#define OBFH_SF_LEAF_ALU_A ".byte %c[sf_leaf_alu_a]; .long %c[sf_key_a];"
#undef OBFH_SF_LEAF_ALU_B
#define OBFH_SF_LEAF_ALU_B ".byte 0x81, %c[sf_leaf_alu_b]; .long %c[sf_key_b];"
#undef OBFH_SF_LEAF_SHIFT
#define OBFH_SF_LEAF_SHIFT ".byte 0xc1, %c[sf_leaf_shift], %c[sf_rotate];"
#undef OBFH_SF_LEAF_A
#define OBFH_SF_LEAF_A ".fill (((%c[sf_phantom] >> 9) & 1) ^ 0), 2, 0xc889;.fill (((%c[sf_phantom] >> 9) & 1) ^ 1), 3, 0x00418d;.fill (%c[sf_phantom] & 1), 1, 0x90;.byte %c[sf_leaf_alu_a]; .long %c[sf_key_a];cmpl %%edx, %%eax; jbe 6f;.byte 0xc1, %c[sf_leaf_shift], %c[sf_rotate];subl %%edx, %%eax; ret; 6:.byte 0x81, %c[sf_leaf_alu_b]; .long %c[sf_key_b];leal (%%eax, %%edx, 2), %%eax; ret;"
#undef OBFH_SF_LEAF_B
#define OBFH_SF_LEAF_B ".fill (((%c[sf_phantom] >> 9) & 1) ^ 0), 2, 0xc889;.fill (((%c[sf_phantom] >> 9) & 1) ^ 1), 3, 0x00418d;.fill (%c[sf_phantom] & 1), 1, 0x90;subq $%c[sf_leaf_frame], %%rsp;movl %%eax, %c[sf_leaf_slot](%%rsp);.byte %c[sf_leaf_alu_a]; .long %c[sf_key_a];.byte 0xc1, %c[sf_leaf_shift], %c[sf_rotate];movl %c[sf_leaf_slot](%%rsp), %%ecx;xorl %%ecx, %%eax; addl %%edx, %%eax;addq $%c[sf_leaf_frame], %%rsp; ret;"
#undef OBFH_SF_LEAF_C
#define OBFH_SF_LEAF_C ".fill (((%c[sf_phantom] >> 9) & 1) ^ 0), 2, 0xc889;.fill (((%c[sf_phantom] >> 9) & 1) ^ 1), 3, 0x00418d;.fill (%c[sf_phantom] & 1), 1, 0x90;subq $%c[sf_leaf_frame], %%rsp;movl %%eax, %c[sf_leaf_slot](%%rsp);movl %%edx, %%ecx; roll $%c[sf_rotate], %%ecx; cmpl %%ecx, %%eax; jge 6f;.byte %c[sf_leaf_alu_a]; .long %c[sf_key_a];xorl %%ecx, %%eax; jmp 7f; 6:.byte 0xc1, %c[sf_leaf_shift], %c[sf_rotate];addl %%edx, %%eax; 7:movl %c[sf_leaf_slot](%%rsp), %%ecx;subl %%ecx, %%eax;addq $%c[sf_leaf_frame], %%rsp; ret;"
#undef OBFH_SF_LEAF_D
#define OBFH_SF_LEAF_D ".fill (((%c[sf_phantom] >> 9) & 1) ^ 0), 2, 0xc889;.fill (((%c[sf_phantom] >> 9) & 1) ^ 1), 3, 0x00418d;.fill (%c[sf_phantom] & 1), 1, 0x90;movl $%c[sf_loops], %%ecx; 6:.byte %c[sf_leaf_alu_a]; .long %c[sf_key_a];.byte 0xc1, %c[sf_leaf_shift], %c[sf_rotate];.byte 0x81, %c[sf_leaf_alu_b]; .long %c[sf_key_b];addl %%edx, %%eax; decl %%ecx; jnz 6b; ret;"
#undef OBFH_SF_LEAF_E
#define OBFH_SF_LEAF_E ".fill (((%c[sf_phantom] >> 9) & 1) ^ 0), 2, 0xc889;.fill (((%c[sf_phantom] >> 9) & 1) ^ 1), 3, 0x00418d;.fill (%c[sf_phantom] & 1), 1, 0x90;cmpl $%c[sf_key_a], %%eax; jb 6f; bswap %%eax;.byte %c[sf_leaf_alu_a]; .long %c[sf_key_a];ret; 6: movl $%c[sf_factor], %%ecx; xorl %%edx, %%edx; divl %%ecx; xorl %%edx, %%eax;.byte 0xc1, %c[sf_leaf_shift], %c[sf_rotate];ret;"
#undef OBFH_SF_LEAF_F
#define OBFH_SF_LEAF_F ".fill (((%c[sf_phantom] >> 9) & 1) ^ 0), 2, 0xc889;.fill (((%c[sf_phantom] >> 9) & 1) ^ 1), 3, 0x00418d;.fill (%c[sf_phantom] & 1), 1, 0x90;subq $%c[sf_leaf_frame], %%rsp;movl %%eax, %c[sf_leaf_slot](%%rsp);subl %%edx, %%eax; negl %%eax;.byte %c[sf_leaf_alu_a]; .long %c[sf_key_a];movl %c[sf_leaf_slot](%%rsp), %%ecx;xorl %%ecx, %%eax;.byte 0x81, %c[sf_leaf_alu_b]; .long %c[sf_key_b];adcl %%edx, %%eax;addq $%c[sf_leaf_frame], %%rsp; ret;"
#undef OBFH_SF_LEAF_G
#define OBFH_SF_LEAF_G ".fill (((%c[sf_phantom] >> 9) & 1) ^ 0), 2, 0xc889;.fill (((%c[sf_phantom] >> 9) & 1) ^ 1), 3, 0x00418d;.fill (%c[sf_phantom] & 1), 1, 0x90;movl %%eax, %%ecx;.byte 0xc1, %c[sf_leaf_shift], %c[sf_rotate];testl $%c[sf_mask], %%ecx; jnz 6f;.byte %c[sf_leaf_alu_a]; .long %c[sf_key_a];ret; 6: imull $%c[sf_factor], %%eax;.byte 0x81, %c[sf_leaf_alu_b]; .long %c[sf_key_b];xorl %%edx, %%eax; ret;"
#undef OBFH_SF_LEAF_H
#define OBFH_SF_LEAF_H ".fill (((%c[sf_phantom] >> 9) & 1) ^ 0), 2, 0xc889;.fill (((%c[sf_phantom] >> 9) & 1) ^ 1), 3, 0x00418d;.fill (%c[sf_phantom] & 1), 1, 0x90;subq $%c[sf_leaf_frame], %%rsp;movl %%eax, %c[sf_leaf_slot](%%rsp);movl $%c[sf_loops], %%ecx; 6:.byte 0xc1, %c[sf_leaf_shift], %c[sf_rotate];addl %%edx, %%eax; decl %%ecx; jnz 6b;movl %c[sf_leaf_slot](%%rsp), %%ecx;xorl %%ecx, %%eax;addq $%c[sf_leaf_frame], %%rsp; ret;"
#undef OBFH_SF_LEAF_I
#define OBFH_SF_LEAF_I ".fill (((%c[sf_phantom] >> 9) & 1) ^ 0), 2, 0xc889;.fill (((%c[sf_phantom] >> 9) & 1) ^ 1), 3, 0x00418d;.fill (%c[sf_phantom] & 1), 1, 0x90;bswap %%eax;.byte %c[sf_leaf_alu_a]; .long %c[sf_key_a];cmpl %%edx, %%eax; jne 6f;.byte 0xc1, %c[sf_leaf_shift], %c[sf_rotate];ret; 6: imull $%c[sf_factor], %%eax; subl %%edx, %%eax; ret;"
#undef OBFH_SF_LEAF_J
#define OBFH_SF_LEAF_J ".fill (((%c[sf_phantom] >> 9) & 1) ^ 0), 2, 0xc889;.fill (((%c[sf_phantom] >> 9) & 1) ^ 1), 3, 0x00418d;.fill (%c[sf_phantom] & 1), 1, 0x90;subq $%c[sf_leaf_frame], %%rsp;movl %%eax, %c[sf_leaf_slot](%%rsp);.byte 0x81, %c[sf_leaf_alu_b]; .long %c[sf_key_b];testl %%edx, %%edx; js 6f;.byte 0xc1, %c[sf_leaf_shift], %c[sf_rotate];jmp 7f; 6: negl %%eax; 7:movl %c[sf_leaf_slot](%%rsp), %%ecx;addl %%ecx, %%eax;addq $%c[sf_leaf_frame], %%rsp; ret;"
#undef OBFH_SF_LEAF_K
#define OBFH_SF_LEAF_K ".fill (((%c[sf_phantom] >> 9) & 1) ^ 0), 2, 0xc889;.fill (((%c[sf_phantom] >> 9) & 1) ^ 1), 3, 0x00418d;.fill (%c[sf_phantom] & 1), 1, 0x90;movzbl %%al, %%ecx; shrl $8, %%eax; imull $%c[sf_factor], %%ecx; xorl %%ecx, %%eax;.byte %c[sf_leaf_alu_a]; .long %c[sf_key_a];.byte 0xc1, %c[sf_leaf_shift], %c[sf_rotate];addl %%edx, %%eax; ret;"
#undef OBFH_SF_LEAF_L
#define OBFH_SF_LEAF_L ".fill (((%c[sf_phantom] >> 9) & 1) ^ 0), 2, 0xc889;.fill (((%c[sf_phantom] >> 9) & 1) ^ 1), 3, 0x00418d;.fill (%c[sf_phantom] & 1), 1, 0x90;subq $%c[sf_leaf_frame], %%rsp;movl %%eax, %c[sf_leaf_slot](%%rsp);cmpl %%edx, %%eax; jbe 6f;.byte %c[sf_leaf_alu_a]; .long %c[sf_key_a];jmp 7f; 6:.byte 0x81, %c[sf_leaf_alu_b]; .long %c[sf_key_b];xorl %%edx, %%eax; 7:movl %c[sf_leaf_slot](%%rsp), %%ecx;subl %%ecx, %%eax;addq $%c[sf_leaf_frame], %%rsp; ret;"
#undef OBFH_SF_LEAF_ENTRY
#define OBFH_SF_LEAF_ENTRY(label, body) label ": " body

#else
#undef OBFH_SF_BYTES
#define OBFH_SF_BYTES(bit, flip, size, value) ".fill (((%c[sf_phantom] >> " bit ") & 1) ^ " flip "), " size ", " value ";"
#undef OBFH_SF_WORD
#define OBFH_SF_WORD(bit, flip, value) ".fill (((%c[sf_phantom] >> " bit ") & 1) ^ " flip "), 4, " value ";"
#undef OBFH_SF_LINK_BYTES
#define OBFH_SF_LINK_BYTES(channel, size, value) ".fill (((%c[sf_phantom] ^ %c[sf_key_a]) >> (12 + 3 * " channel ")) & 1), " size ", " value ";"
#undef OBFH_SF_LINK_GATE
#define OBFH_SF_LINK_GATE(span, channel) ".fill (((%c[sf_phantom] ^ %c[sf_key_a]) >> (12 + 3 * " channel ")) & 1), 2, 0xc085 ^ ((((%c[sf_phantom] ^ %c[sf_key_a]) >> (14 + 3 * " channel ")) & 1) << 12);.fill (((%c[sf_phantom] ^ %c[sf_key_a]) >> (12 + 3 * " channel ")) & 1), 1, 0x74 | (((%c[sf_phantom] ^ %c[sf_key_a]) >> (13 + 3 * " channel ")) & 1);.fill (((%c[sf_phantom] ^ %c[sf_key_a]) >> (12 + 3 * " channel ")) & 1), 1, " span ";"
#undef OBFH_SF_INPUT
#define OBFH_SF_INPUT "movl %%esp, %%eax;"
#undef OBFH_SF_FRAME_HEAD
#define OBFH_SF_FRAME_HEAD(size) "pushl %%ebp;.fill (((%c[sf_phantom] >> (1 + ((%c[" size "] >> 4) & 3))) & 1) ^ 0), 2, 0xe589;.fill (((%c[sf_phantom] >> (1 + ((%c[" size "] >> 4) & 3))) & 1) ^ 1), 3, 0x242c8d;"
#undef OBFH_SF_FRAME
#define OBFH_SF_FRAME(size) "pushl %%ebp;.fill (((%c[sf_phantom] >> (1 + ((%c[" size "] >> 4) & 3))) & 1) ^ 0), 2, 0xe589;.fill (((%c[sf_phantom] >> (1 + ((%c[" size "] >> 4) & 3))) & 1) ^ 1), 3, 0x242c8d;.fill (((%c[sf_phantom] >> (4 + ((%c[" size "] >> 4) & 3))) & 1) ^ 0), 2, 0xec81;.fill (((%c[sf_phantom] >> (4 + ((%c[" size "] >> 4) & 3))) & 1) ^ 0), 4, %c[" size "];.fill (((%c[sf_phantom] >> (4 + ((%c[" size "] >> 4) & 3))) & 1) ^ 1), 3, 0x24a48d;.fill (((%c[sf_phantom] >> (4 + ((%c[" size "] >> 4) & 3))) & 1) ^ 1), 4, -%c[" size "];"
#undef OBFH_SF_FRAME_ALT
#define OBFH_SF_FRAME_ALT(size) "pushl %%ebp;.fill (((%c[sf_phantom] >> (4 + ((%c[" size "] >> 4) & 3))) & 1) ^ 0), 2, 0xec81;.fill (((%c[sf_phantom] >> (4 + ((%c[" size "] >> 4) & 3))) & 1) ^ 0), 4, %c[" size "];.fill (((%c[sf_phantom] >> (4 + ((%c[" size "] >> 4) & 3))) & 1) ^ 1), 3, 0x24a48d;.fill (((%c[sf_phantom] >> (4 + ((%c[" size "] >> 4) & 3))) & 1) ^ 1), 4, -%c[" size "];.fill 1, 3, 0x24ac8d; .long %c[" size "];"
#undef OBFH_SF_EPILOGUE_ALT
#define OBFH_SF_EPILOGUE_ALT ".fill (((%c[sf_phantom] >> 7) & 1) ^ 0), 2, 0xec89;.fill (((%c[sf_phantom] >> 7) & 1) ^ 1), 3, 0x00658d;popl %%ebp; ret;"
#undef OBFH_SF_EPILOGUE
#define OBFH_SF_EPILOGUE ".fill (((%c[sf_phantom] >> 6) & 1) ^ 0), 1, 0xc9;.fill (((%c[sf_phantom] >> 6) & 1) ^ 1), 3, 0x5dec89;ret;"
#undef OBFH_SF_ARGS
#define OBFH_SF_ARGS ".fill (((%c[sf_phantom] >> 9) & 1) ^ 0), 3, 0x0c558b;.fill (((%c[sf_phantom] >> 9) & 1) ^ 0), 3, 0x08458b;.fill (((%c[sf_phantom] >> 9) & 1) ^ 1), 3, 0x08458b;.fill (((%c[sf_phantom] >> 9) & 1) ^ 1), 3, 0x0c558b;"
#undef OBFH_SF_CALL_AT
#define OBFH_SF_CALL_AT(target, channel) ".fill (((%c[sf_phantom] ^ %c[sf_key_a]) >> (12 + 3 * " channel ")) & 1), 2, 0xc085 ^ ((((%c[sf_phantom] ^ %c[sf_key_a]) >> (14 + 3 * " channel ")) & 1) << 12);.fill (((%c[sf_phantom] ^ %c[sf_key_a]) >> (12 + 3 * " channel ")) & 1), 1, 0x74 | (((%c[sf_phantom] ^ %c[sf_key_a]) >> (13 + 3 * " channel ")) & 1);.fill (((%c[sf_phantom] ^ %c[sf_key_a]) >> (12 + 3 * " channel ")) & 1), 1, 18;.byte 0x68; .long %c[sf_arg_b]; .byte 0x68; .long %c[sf_arg_a]; call " target "; addl $8, %%esp;"
#undef OBFH_SF_LOCAL
#define OBFH_SF_LOCAL ".fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 2, 0x4589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 1, -%c[sf_local_a];.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 2, 0x5589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 1, -%c[sf_local_b];.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 2, 0x5589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 1, -%c[sf_local_b];.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 2, 0x4589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 1, -%c[sf_local_a];"
#undef OBFH_SF_CALL
#define OBFH_SF_CALL(target) ".fill (((%c[sf_phantom] ^ %c[sf_key_a]) >> (12 + 3 * 0)) & 1), 2, 0xc085 ^ ((((%c[sf_phantom] ^ %c[sf_key_a]) >> (14 + 3 * 0)) & 1) << 12);.fill (((%c[sf_phantom] ^ %c[sf_key_a]) >> (12 + 3 * 0)) & 1), 1, 0x74 | (((%c[sf_phantom] ^ %c[sf_key_a]) >> (13 + 3 * 0)) & 1);.fill (((%c[sf_phantom] ^ %c[sf_key_a]) >> (12 + 3 * 0)) & 1), 1, 18;.byte 0x68; .long %c[sf_arg_b]; .byte 0x68; .long %c[sf_arg_a]; call " target "; addl $8, %%esp;"
#undef OBFH_SF_PADDING
#define OBFH_SF_PADDING ".fill %c[sf_pad], 1, 0x90;"
#undef OBFH_SF_GAP
#define OBFH_SF_GAP ".fill %c[sf_pad_b], 1, 0x90; .byte %c[sf_noise_a], %c[sf_noise_b];"
#undef OBFH_SF_BODY_A
#define OBFH_SF_BODY_A ".fill (((%c[sf_phantom] >> 9) & 1) ^ 0), 3, 0x0c558b;.fill (((%c[sf_phantom] >> 9) & 1) ^ 0), 3, 0x08458b;.fill (((%c[sf_phantom] >> 9) & 1) ^ 1), 3, 0x08458b;.fill (((%c[sf_phantom] >> 9) & 1) ^ 1), 3, 0x0c558b;xorl $%c[sf_key_a], %%eax; imull $%c[sf_factor], %%eax; addl %%edx, %%eax;.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 2, 0x4589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 1, -%c[sf_local_a];.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 2, 0x5589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 1, -%c[sf_local_b];.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 2, 0x5589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 1, -%c[sf_local_b];.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 2, 0x4589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 1, -%c[sf_local_a];"
#undef OBFH_SF_BODY_B
#define OBFH_SF_BODY_B ".fill (((%c[sf_phantom] >> 9) & 1) ^ 0), 3, 0x0c558b;.fill (((%c[sf_phantom] >> 9) & 1) ^ 0), 3, 0x08458b;.fill (((%c[sf_phantom] >> 9) & 1) ^ 1), 3, 0x08458b;.fill (((%c[sf_phantom] >> 9) & 1) ^ 1), 3, 0x0c558b;addl $%c[sf_key_b], %%eax; roll $%c[sf_rotate], %%eax; xorl %%edx, %%eax;.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 2, 0x4589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 1, -%c[sf_local_a];.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 2, 0x5589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 1, -%c[sf_local_b];.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 2, 0x5589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 1, -%c[sf_local_b];.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 2, 0x4589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 1, -%c[sf_local_a];"
#undef OBFH_SF_BODY_C
#define OBFH_SF_BODY_C ".fill (((%c[sf_phantom] >> 9) & 1) ^ 0), 3, 0x0c558b;.fill (((%c[sf_phantom] >> 9) & 1) ^ 0), 3, 0x08458b;.fill (((%c[sf_phantom] >> 9) & 1) ^ 1), 3, 0x08458b;.fill (((%c[sf_phantom] >> 9) & 1) ^ 1), 3, 0x0c558b;leal (%%eax, %%eax, 2), %%eax; xorl %%eax, %%edx; addl $%c[sf_key_a], %%eax;.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 2, 0x4589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 1, -%c[sf_local_a];.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 2, 0x5589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 1, -%c[sf_local_b];.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 2, 0x5589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 1, -%c[sf_local_b];.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 2, 0x4589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 1, -%c[sf_local_a];"
#undef OBFH_SF_BODY_D
#define OBFH_SF_BODY_D ".fill (((%c[sf_phantom] >> 9) & 1) ^ 0), 3, 0x0c558b;.fill (((%c[sf_phantom] >> 9) & 1) ^ 0), 3, 0x08458b;.fill (((%c[sf_phantom] >> 9) & 1) ^ 1), 3, 0x08458b;.fill (((%c[sf_phantom] >> 9) & 1) ^ 1), 3, 0x0c558b;notl %%eax; addl %%edx, %%eax; imull $%c[sf_factor], %%eax; roll $%c[sf_rotate], %%eax;.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 2, 0x4589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 1, -%c[sf_local_a];.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 2, 0x5589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 1, -%c[sf_local_b];.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 2, 0x5589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 1, -%c[sf_local_b];.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 2, 0x4589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 1, -%c[sf_local_a];"
#undef OBFH_SF_BODY_E
#define OBFH_SF_BODY_E ".fill (((%c[sf_phantom] >> 9) & 1) ^ 0), 3, 0x0c558b;.fill (((%c[sf_phantom] >> 9) & 1) ^ 0), 3, 0x08458b;.fill (((%c[sf_phantom] >> 9) & 1) ^ 1), 3, 0x08458b;.fill (((%c[sf_phantom] >> 9) & 1) ^ 1), 3, 0x0c558b;bswap %%eax; xorl %%edx, %%eax; roll $%c[sf_rotate], %%eax;.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 2, 0x4589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 1, -%c[sf_local_a];.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 2, 0x5589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 1, -%c[sf_local_b];.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 2, 0x5589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 1, -%c[sf_local_b];.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 2, 0x4589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 1, -%c[sf_local_a];"
#undef OBFH_SF_BODY_F
#define OBFH_SF_BODY_F ".fill (((%c[sf_phantom] >> 9) & 1) ^ 0), 3, 0x0c558b;.fill (((%c[sf_phantom] >> 9) & 1) ^ 0), 3, 0x08458b;.fill (((%c[sf_phantom] >> 9) & 1) ^ 1), 3, 0x08458b;.fill (((%c[sf_phantom] >> 9) & 1) ^ 1), 3, 0x0c558b;movl $%c[sf_loops], %%ecx; 6: addl %%edx, %%eax; imull $%c[sf_factor], %%eax; xorl $%c[sf_key_b], %%eax; decl %%ecx; jnz 6b;.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 2, 0x4589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 1, -%c[sf_local_a];.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 2, 0x5589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 1, -%c[sf_local_b];.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 2, 0x5589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 1, -%c[sf_local_b];.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 2, 0x4589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 1, -%c[sf_local_a];"
#undef OBFH_SF_BODY_G
#define OBFH_SF_BODY_G ".fill (((%c[sf_phantom] >> 9) & 1) ^ 0), 3, 0x0c558b;.fill (((%c[sf_phantom] >> 9) & 1) ^ 0), 3, 0x08458b;.fill (((%c[sf_phantom] >> 9) & 1) ^ 1), 3, 0x08458b;.fill (((%c[sf_phantom] >> 9) & 1) ^ 1), 3, 0x0c558b;movzbl %%al, %%ecx; shrl $8, %%eax; xorl %%edx, %%ecx; leal (%%eax, %%ecx, 4), %%eax;.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 2, 0x4589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 1, -%c[sf_local_a];.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 2, 0x5589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 1, -%c[sf_local_b];.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 2, 0x5589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 1, -%c[sf_local_b];.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 2, 0x4589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 1, -%c[sf_local_a];"
#undef OBFH_SF_BODY_H
#define OBFH_SF_BODY_H ".fill (((%c[sf_phantom] >> 9) & 1) ^ 0), 3, 0x0c558b;.fill (((%c[sf_phantom] >> 9) & 1) ^ 0), 3, 0x08458b;.fill (((%c[sf_phantom] >> 9) & 1) ^ 1), 3, 0x08458b;.fill (((%c[sf_phantom] >> 9) & 1) ^ 1), 3, 0x0c558b;imull %%eax, %%edx; xorl $%c[sf_key_b], %%edx; subl %%edx, %%eax; negl %%eax;.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 2, 0x4589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 1, -%c[sf_local_a];.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 2, 0x5589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 1, -%c[sf_local_b];.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 2, 0x5589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 1, -%c[sf_local_b];.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 2, 0x4589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 1, -%c[sf_local_a];"
#undef OBFH_SF_BODY_I
#define OBFH_SF_BODY_I ".fill (((%c[sf_phantom] >> 9) & 1) ^ 0), 3, 0x0c558b;.fill (((%c[sf_phantom] >> 9) & 1) ^ 0), 3, 0x08458b;.fill (((%c[sf_phantom] >> 9) & 1) ^ 1), 3, 0x08458b;.fill (((%c[sf_phantom] >> 9) & 1) ^ 1), 3, 0x0c558b;movl %%eax, %%ecx; orl $%c[sf_mask], %%eax; andl $%c[sf_mask], %%ecx; subl %%ecx, %%eax; xorl %%edx, %%eax;.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 2, 0x4589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 1, -%c[sf_local_a];.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 2, 0x5589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 1, -%c[sf_local_b];.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 2, 0x5589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 1, -%c[sf_local_b];.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 2, 0x4589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 1, -%c[sf_local_a];"
#undef OBFH_SF_BODY_J
#define OBFH_SF_BODY_J ".fill (((%c[sf_phantom] >> 9) & 1) ^ 0), 3, 0x0c558b;.fill (((%c[sf_phantom] >> 9) & 1) ^ 0), 3, 0x08458b;.fill (((%c[sf_phantom] >> 9) & 1) ^ 1), 3, 0x08458b;.fill (((%c[sf_phantom] >> 9) & 1) ^ 1), 3, 0x0c558b;xorl $%c[sf_key_a], %%eax; movl %%edx, %%ecx; shll $%c[sf_rotate], %%ecx; shrl $%c[sf_rotate], %%eax; orl %%ecx, %%eax;.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 2, 0x4589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 1, -%c[sf_local_a];.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 2, 0x5589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 1, -%c[sf_local_b];.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 2, 0x5589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 1, -%c[sf_local_b];.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 2, 0x4589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 1, -%c[sf_local_a];"
#undef OBFH_SF_BODY_K
#define OBFH_SF_BODY_K ".fill (((%c[sf_phantom] >> 9) & 1) ^ 0), 3, 0x0c558b;.fill (((%c[sf_phantom] >> 9) & 1) ^ 0), 3, 0x08458b;.fill (((%c[sf_phantom] >> 9) & 1) ^ 1), 3, 0x08458b;.fill (((%c[sf_phantom] >> 9) & 1) ^ 1), 3, 0x0c558b;testl %%edx, %%edx; js 6f; addl $%c[sf_key_a], %%eax; jmp 7f; 6: negl %%eax; xorl %%edx, %%eax; 7: roll $%c[sf_rotate], %%eax;.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 2, 0x4589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 1, -%c[sf_local_a];.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 2, 0x5589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 1, -%c[sf_local_b];.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 2, 0x5589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 1, -%c[sf_local_b];.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 2, 0x4589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 1, -%c[sf_local_a];"
#undef OBFH_SF_BODY_L
#define OBFH_SF_BODY_L ".fill (((%c[sf_phantom] >> 9) & 1) ^ 0), 3, 0x0c558b;.fill (((%c[sf_phantom] >> 9) & 1) ^ 0), 3, 0x08458b;.fill (((%c[sf_phantom] >> 9) & 1) ^ 1), 3, 0x08458b;.fill (((%c[sf_phantom] >> 9) & 1) ^ 1), 3, 0x0c558b;movl %%eax, %%ecx; movl %%edx, %%eax; addl %%ecx, %%eax; imull $%c[sf_factor], %%eax; notl %%edx; xorl %%edx, %%eax;.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 2, 0x4589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 1, -%c[sf_local_a];.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 2, 0x5589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 1, -%c[sf_local_b];.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 2, 0x5589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 1, -%c[sf_local_b];.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 2, 0x4589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 1, -%c[sf_local_a];"
#undef OBFH_SF_BODY_M
#define OBFH_SF_BODY_M ".fill (((%c[sf_phantom] >> 9) & 1) ^ 0), 3, 0x0c558b;.fill (((%c[sf_phantom] >> 9) & 1) ^ 0), 3, 0x08458b;.fill (((%c[sf_phantom] >> 9) & 1) ^ 1), 3, 0x08458b;.fill (((%c[sf_phantom] >> 9) & 1) ^ 1), 3, 0x0c558b;movl %%eax, %%ecx; shrl $16, %%ecx; xorl %%ecx, %%eax; imull $%c[sf_factor], %%eax; xorl %%edx, %%eax;.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 2, 0x4589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 1, -%c[sf_local_a];.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 2, 0x5589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 1, -%c[sf_local_b];.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 2, 0x5589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 1, -%c[sf_local_b];.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 2, 0x4589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 1, -%c[sf_local_a];"
#undef OBFH_SF_BODY_N
#define OBFH_SF_BODY_N ".fill (((%c[sf_phantom] >> 9) & 1) ^ 0), 3, 0x0c558b;.fill (((%c[sf_phantom] >> 9) & 1) ^ 0), 3, 0x08458b;.fill (((%c[sf_phantom] >> 9) & 1) ^ 1), 3, 0x08458b;.fill (((%c[sf_phantom] >> 9) & 1) ^ 1), 3, 0x0c558b;addl $%c[sf_key_a], %%eax; adcl $%c[sf_key_b], %%edx; xorl %%edx, %%eax; rorl $%c[sf_rotate], %%eax;.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 2, 0x4589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 1, -%c[sf_local_a];.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 2, 0x5589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 1, -%c[sf_local_b];.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 2, 0x5589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 1, -%c[sf_local_b];.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 2, 0x4589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 1, -%c[sf_local_a];"
#undef OBFH_SF_BODY_O
#define OBFH_SF_BODY_O ".fill (((%c[sf_phantom] >> 9) & 1) ^ 0), 3, 0x0c558b;.fill (((%c[sf_phantom] >> 9) & 1) ^ 0), 3, 0x08458b;.fill (((%c[sf_phantom] >> 9) & 1) ^ 1), 3, 0x08458b;.fill (((%c[sf_phantom] >> 9) & 1) ^ 1), 3, 0x0c558b;xchgl %%eax, %%edx; subl $%c[sf_key_a], %%eax; bswap %%edx; addl %%edx, %%eax;.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 2, 0x4589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 1, -%c[sf_local_a];.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 2, 0x5589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 1, -%c[sf_local_b];.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 2, 0x5589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 1, -%c[sf_local_b];.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 2, 0x4589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 1, -%c[sf_local_a];"
#undef OBFH_SF_BODY_P
#define OBFH_SF_BODY_P ".fill (((%c[sf_phantom] >> 9) & 1) ^ 0), 3, 0x0c558b;.fill (((%c[sf_phantom] >> 9) & 1) ^ 0), 3, 0x08458b;.fill (((%c[sf_phantom] >> 9) & 1) ^ 1), 3, 0x08458b;.fill (((%c[sf_phantom] >> 9) & 1) ^ 1), 3, 0x0c558b;movl $%c[sf_factor], %%ecx; xorl %%edx, %%edx; divl %%ecx; xorl %%edx, %%eax;.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 2, 0x4589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 1, -%c[sf_local_a];.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 2, 0x5589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 1, -%c[sf_local_b];.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 2, 0x5589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 1, -%c[sf_local_b];.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 2, 0x4589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 1, -%c[sf_local_a];"
#undef OBFH_SF_BODY_Q
#define OBFH_SF_BODY_Q ".fill (((%c[sf_phantom] >> 9) & 1) ^ 0), 3, 0x0c558b;.fill (((%c[sf_phantom] >> 9) & 1) ^ 0), 3, 0x08458b;.fill (((%c[sf_phantom] >> 9) & 1) ^ 1), 3, 0x08458b;.fill (((%c[sf_phantom] >> 9) & 1) ^ 1), 3, 0x0c558b;movzbl %%al, %%ecx; movzbl %%dl, %%edx; imull %%edx, %%ecx; shrl $8, %%eax; addl %%ecx, %%eax;.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 2, 0x4589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 1, -%c[sf_local_a];.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 2, 0x5589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 1, -%c[sf_local_b];.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 2, 0x5589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 1, -%c[sf_local_b];.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 2, 0x4589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 1, -%c[sf_local_a];"
#undef OBFH_SF_BODY_R
#define OBFH_SF_BODY_R ".fill (((%c[sf_phantom] >> 9) & 1) ^ 0), 3, 0x0c558b;.fill (((%c[sf_phantom] >> 9) & 1) ^ 0), 3, 0x08458b;.fill (((%c[sf_phantom] >> 9) & 1) ^ 1), 3, 0x08458b;.fill (((%c[sf_phantom] >> 9) & 1) ^ 1), 3, 0x0c558b;movl $%c[sf_loops], %%ecx; 6: xorl %%edx, %%eax; roll $%c[sf_rotate], %%eax; addl $%c[sf_key_a], %%edx; decl %%ecx; jnz 6b;.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 2, 0x4589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 1, -%c[sf_local_a];.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 2, 0x5589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 1, -%c[sf_local_b];.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 2, 0x5589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 1, -%c[sf_local_b];.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 2, 0x4589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 1, -%c[sf_local_a];"
#undef OBFH_SF_BODY_S
#define OBFH_SF_BODY_S ".fill (((%c[sf_phantom] >> 9) & 1) ^ 0), 3, 0x0c558b;.fill (((%c[sf_phantom] >> 9) & 1) ^ 0), 3, 0x08458b;.fill (((%c[sf_phantom] >> 9) & 1) ^ 1), 3, 0x08458b;.fill (((%c[sf_phantom] >> 9) & 1) ^ 1), 3, 0x0c558b;shldl $%c[sf_rotate], %%edx, %%eax; subl $%c[sf_key_b], %%eax; xorl $%c[sf_mask], %%eax;.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 2, 0x4589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 1, -%c[sf_local_a];.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 2, 0x5589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 1, -%c[sf_local_b];.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 2, 0x5589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 1, -%c[sf_local_b];.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 2, 0x4589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 1, -%c[sf_local_a];"
#undef OBFH_SF_BODY_T
#define OBFH_SF_BODY_T ".fill (((%c[sf_phantom] >> 9) & 1) ^ 0), 3, 0x0c558b;.fill (((%c[sf_phantom] >> 9) & 1) ^ 0), 3, 0x08458b;.fill (((%c[sf_phantom] >> 9) & 1) ^ 1), 3, 0x08458b;.fill (((%c[sf_phantom] >> 9) & 1) ^ 1), 3, 0x0c558b;movl %%eax, %%ecx; shrl $16, %%eax; shll $16, %%ecx; orl %%ecx, %%eax; xorl %%edx, %%eax; negl %%eax;.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 2, 0x4589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 1, -%c[sf_local_a];.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 2, 0x5589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 0), 1, -%c[sf_local_b];.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 2, 0x5589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 1, -%c[sf_local_b];.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 2, 0x4589;.fill (((%c[sf_phantom] >> 8) & 1) ^ 1), 1, -%c[sf_local_a];"
#undef OBFH_SF_GUARD_0
#define OBFH_SF_GUARD_0 "imull %%eax, %%eax; testl $2, %%eax; jz 9f;"
#undef OBFH_SF_GUARD_1
#define OBFH_SF_GUARD_1 "leal 1(%%eax), %%edx; imull %%edx, %%eax; testl $1, %%eax; jz 9f;"
#undef OBFH_SF_GUARD_2
#define OBFH_SF_GUARD_2 "movl %%eax, %%edx; imull %%eax, %%eax; xorl %%edx, %%eax; testl $1, %%eax; jz 9f;"
#undef OBFH_SF_GUARD_3
#define OBFH_SF_GUARD_3 "movl %%eax, %%edx; notl %%edx; addl %%eax, %%edx; cmpl $-1, %%edx; je 9f;"
#undef OBFH_SF_GUARD_4
#define OBFH_SF_GUARD_4 "movl %%eax, %%edx; movl %%eax, %%ecx; orl $%c[sf_mask], %%eax; andl $%c[sf_mask], %%edx; addl %%edx, %%eax; addl $%c[sf_mask], %%ecx; cmpl %%ecx, %%eax; je 9f;"
#undef OBFH_SF_GUARD_5
#define OBFH_SF_GUARD_5 "movl %%eax, %%edx; roll $%c[sf_rotate], %%eax; rorl $%c[sf_rotate], %%eax; cmpl %%edx, %%eax; je 9f;"
#undef OBFH_SF_GUARD_6
#define OBFH_SF_GUARD_6 "movl %%eax, %%edx; bswap %%eax; bswap %%eax; cmpl %%edx, %%eax; je 9f;"
#undef OBFH_SF_GUARD_7
#define OBFH_SF_GUARD_7 "leal -1(%%eax), %%edx; imull %%edx, %%eax; testl $1, %%eax; jz 9f;"
#undef OBFH_SF_GUARD_8
#define OBFH_SF_GUARD_8 "movl %%eax, %%edx; notl %%edx; xorl $%c[sf_key_a], %%eax; xorl $%c[sf_key_a], %%edx; xorl %%edx, %%eax; cmpl $-1, %%eax; je 9f;"
#undef OBFH_SF_GUARD_9
#define OBFH_SF_GUARD_9 "movl %%eax, %%ecx; movl %%eax, %%edx; xorl $%c[sf_mask], %%eax; andl $%c[sf_mask], %%edx; leal (%%eax, %%edx, 2), %%eax; addl $%c[sf_mask], %%ecx; cmpl %%ecx, %%eax; je 9f;"
#undef OBFH_SF_GUARD_10
#define OBFH_SF_GUARD_10 "movl %%eax, %%edx; negl %%edx; andl %%edx, %%eax; leal -1(%%eax), %%edx; andl %%edx, %%eax; testl %%eax, %%eax; jz 9f;"
#undef OBFH_SF_GUARD_11
#define OBFH_SF_GUARD_11 "movl %%eax, %%edx; movl %%eax, %%ecx; andl $%c[sf_mask], %%eax; andl $%c[sf_not_mask], %%edx; imull $%c[sf_factor], %%eax; imull $%c[sf_factor], %%edx; addl %%edx, %%eax; imull $%c[sf_factor], %%ecx; cmpl %%ecx, %%eax; je 9f;"
#undef OBFH_SF_GUARD_12
#define OBFH_SF_GUARD_12 "movl %%eax, %%edx; addl $%c[sf_key_a], %%eax; imull $%c[sf_factor], %%eax; imull $%c[sf_factor], %%edx; movl $%c[sf_key_a], %%ecx; imull $%c[sf_factor], %%ecx; addl %%edx, %%ecx; cmpl %%ecx, %%eax; je 9f;"
#undef OBFH_SF_GUARD_13
#define OBFH_SF_GUARD_13 "movl %%eax, %%ecx; movl %%eax, %%edx; addl $%c[sf_key_a], %%eax; imull %%eax, %%eax; imull %%ecx, %%ecx; subl %%ecx, %%eax; imull $%c[sf_key_a], %%edx; addl %%edx, %%edx; subl %%edx, %%eax; movl $%c[sf_key_a], %%edx; imull %%edx, %%edx; cmpl %%edx, %%eax; je 9f;"
#undef OBFH_SF_GUARD_14
#define OBFH_SF_GUARD_14 "movl %%eax, %%edx; roll $16, %%eax; roll $16, %%eax; cmpl %%edx, %%eax; je 9f;"
#undef OBFH_SF_ENTRY_ALT
#define OBFH_SF_ENTRY_ALT(label, frame, body) label ": pushl %%ebp;.fill (((%c[sf_phantom] >> (4 + ((%c[" frame "] >> 4) & 3))) & 1) ^ 0), 2, 0xec81;.fill (((%c[sf_phantom] >> (4 + ((%c[" frame "] >> 4) & 3))) & 1) ^ 0), 4, %c[" frame "];.fill (((%c[sf_phantom] >> (4 + ((%c[" frame "] >> 4) & 3))) & 1) ^ 1), 3, 0x24a48d;.fill (((%c[sf_phantom] >> (4 + ((%c[" frame "] >> 4) & 3))) & 1) ^ 1), 4, -%c[" frame "];.fill 1, 3, 0x24ac8d; .long %c[" frame "];" body ".fill ((%c[sf_phantom] >> " label ") & 1), 1, 0x90;"
#undef OBFH_SF_ENTRY
#define OBFH_SF_ENTRY(label, frame, body) label ": pushl %%ebp;.fill (((%c[sf_phantom] >> (1 + ((%c[" frame "] >> 4) & 3))) & 1) ^ 0), 2, 0xe589;.fill (((%c[sf_phantom] >> (1 + ((%c[" frame "] >> 4) & 3))) & 1) ^ 1), 3, 0x242c8d;.fill (((%c[sf_phantom] >> (4 + ((%c[" frame "] >> 4) & 3))) & 1) ^ 0), 2, 0xec81;.fill (((%c[sf_phantom] >> (4 + ((%c[" frame "] >> 4) & 3))) & 1) ^ 0), 4, %c[" frame "];.fill (((%c[sf_phantom] >> (4 + ((%c[" frame "] >> 4) & 3))) & 1) ^ 1), 3, 0x24a48d;.fill (((%c[sf_phantom] >> (4 + ((%c[" frame "] >> 4) & 3))) & 1) ^ 1), 4, -%c[" frame "];" body ".fill ((%c[sf_phantom] >> " label ") & 1), 1, 0x90;"
#undef OBFH_SF_LEAF_ARGS
#define OBFH_SF_LEAF_ARGS "movl 4(%%esp), %%eax; movl 8(%%esp), %%edx; .fill (%c[sf_phantom] & 1), 1, 0x90;"
#undef OBFH_SF_LEAF_ENTER
#define OBFH_SF_LEAF_ENTER "subl $%c[sf_leaf_frame], %%esp;"
#undef OBFH_SF_LEAF_STORE
#define OBFH_SF_LEAF_STORE "movl %%eax, %c[sf_leaf_slot](%%esp);"
#undef OBFH_SF_LEAF_LOAD
#define OBFH_SF_LEAF_LOAD "movl %c[sf_leaf_slot](%%esp), %%ecx;"
#undef OBFH_SF_LEAF_EXIT
#define OBFH_SF_LEAF_EXIT "addl $%c[sf_leaf_frame], %%esp; ret;"
#undef OBFH_SF_LEAF_ALU_A
#define OBFH_SF_LEAF_ALU_A ".byte %c[sf_leaf_alu_a]; .long %c[sf_key_a];"
#undef OBFH_SF_LEAF_ALU_B
#define OBFH_SF_LEAF_ALU_B ".byte 0x81, %c[sf_leaf_alu_b]; .long %c[sf_key_b];"
#undef OBFH_SF_LEAF_SHIFT
#define OBFH_SF_LEAF_SHIFT ".byte 0xc1, %c[sf_leaf_shift], %c[sf_rotate];"
#undef OBFH_SF_LEAF_A
#define OBFH_SF_LEAF_A "movl 4(%%esp), %%eax; movl 8(%%esp), %%edx; .fill (%c[sf_phantom] & 1), 1, 0x90;.byte %c[sf_leaf_alu_a]; .long %c[sf_key_a];cmpl %%edx, %%eax; jbe 6f;.byte 0xc1, %c[sf_leaf_shift], %c[sf_rotate];subl %%edx, %%eax; ret; 6:.byte 0x81, %c[sf_leaf_alu_b]; .long %c[sf_key_b];leal (%%eax, %%edx, 2), %%eax; ret;"
#undef OBFH_SF_LEAF_B
#define OBFH_SF_LEAF_B "movl 4(%%esp), %%eax; movl 8(%%esp), %%edx; .fill (%c[sf_phantom] & 1), 1, 0x90;subl $%c[sf_leaf_frame], %%esp;movl %%eax, %c[sf_leaf_slot](%%esp);.byte %c[sf_leaf_alu_a]; .long %c[sf_key_a];.byte 0xc1, %c[sf_leaf_shift], %c[sf_rotate];movl %c[sf_leaf_slot](%%esp), %%ecx;xorl %%ecx, %%eax; addl %%edx, %%eax;addl $%c[sf_leaf_frame], %%esp; ret;"
#undef OBFH_SF_LEAF_C
#define OBFH_SF_LEAF_C "movl 4(%%esp), %%eax; movl 8(%%esp), %%edx; .fill (%c[sf_phantom] & 1), 1, 0x90;subl $%c[sf_leaf_frame], %%esp;movl %%eax, %c[sf_leaf_slot](%%esp);movl %%edx, %%ecx; roll $%c[sf_rotate], %%ecx; cmpl %%ecx, %%eax; jge 6f;.byte %c[sf_leaf_alu_a]; .long %c[sf_key_a];xorl %%ecx, %%eax; jmp 7f; 6:.byte 0xc1, %c[sf_leaf_shift], %c[sf_rotate];addl %%edx, %%eax; 7:movl %c[sf_leaf_slot](%%esp), %%ecx;subl %%ecx, %%eax;addl $%c[sf_leaf_frame], %%esp; ret;"
#undef OBFH_SF_LEAF_D
#define OBFH_SF_LEAF_D "movl 4(%%esp), %%eax; movl 8(%%esp), %%edx; .fill (%c[sf_phantom] & 1), 1, 0x90;movl $%c[sf_loops], %%ecx; 6:.byte %c[sf_leaf_alu_a]; .long %c[sf_key_a];.byte 0xc1, %c[sf_leaf_shift], %c[sf_rotate];.byte 0x81, %c[sf_leaf_alu_b]; .long %c[sf_key_b];addl %%edx, %%eax; decl %%ecx; jnz 6b; ret;"
#undef OBFH_SF_LEAF_E
#define OBFH_SF_LEAF_E "movl 4(%%esp), %%eax; movl 8(%%esp), %%edx; .fill (%c[sf_phantom] & 1), 1, 0x90;cmpl $%c[sf_key_a], %%eax; jb 6f; bswap %%eax;.byte %c[sf_leaf_alu_a]; .long %c[sf_key_a];ret; 6: movl $%c[sf_factor], %%ecx; xorl %%edx, %%edx; divl %%ecx; xorl %%edx, %%eax;.byte 0xc1, %c[sf_leaf_shift], %c[sf_rotate];ret;"
#undef OBFH_SF_LEAF_F
#define OBFH_SF_LEAF_F "movl 4(%%esp), %%eax; movl 8(%%esp), %%edx; .fill (%c[sf_phantom] & 1), 1, 0x90;subl $%c[sf_leaf_frame], %%esp;movl %%eax, %c[sf_leaf_slot](%%esp);subl %%edx, %%eax; negl %%eax;.byte %c[sf_leaf_alu_a]; .long %c[sf_key_a];movl %c[sf_leaf_slot](%%esp), %%ecx;xorl %%ecx, %%eax;.byte 0x81, %c[sf_leaf_alu_b]; .long %c[sf_key_b];adcl %%edx, %%eax;addl $%c[sf_leaf_frame], %%esp; ret;"
#undef OBFH_SF_LEAF_G
#define OBFH_SF_LEAF_G "movl 4(%%esp), %%eax; movl 8(%%esp), %%edx; .fill (%c[sf_phantom] & 1), 1, 0x90;movl %%eax, %%ecx;.byte 0xc1, %c[sf_leaf_shift], %c[sf_rotate];testl $%c[sf_mask], %%ecx; jnz 6f;.byte %c[sf_leaf_alu_a]; .long %c[sf_key_a];ret; 6: imull $%c[sf_factor], %%eax;.byte 0x81, %c[sf_leaf_alu_b]; .long %c[sf_key_b];xorl %%edx, %%eax; ret;"
#undef OBFH_SF_LEAF_H
#define OBFH_SF_LEAF_H "movl 4(%%esp), %%eax; movl 8(%%esp), %%edx; .fill (%c[sf_phantom] & 1), 1, 0x90;subl $%c[sf_leaf_frame], %%esp;movl %%eax, %c[sf_leaf_slot](%%esp);movl $%c[sf_loops], %%ecx; 6:.byte 0xc1, %c[sf_leaf_shift], %c[sf_rotate];addl %%edx, %%eax; decl %%ecx; jnz 6b;movl %c[sf_leaf_slot](%%esp), %%ecx;xorl %%ecx, %%eax;addl $%c[sf_leaf_frame], %%esp; ret;"
#undef OBFH_SF_LEAF_I
#define OBFH_SF_LEAF_I "movl 4(%%esp), %%eax; movl 8(%%esp), %%edx; .fill (%c[sf_phantom] & 1), 1, 0x90;bswap %%eax;.byte %c[sf_leaf_alu_a]; .long %c[sf_key_a];cmpl %%edx, %%eax; jne 6f;.byte 0xc1, %c[sf_leaf_shift], %c[sf_rotate];ret; 6: imull $%c[sf_factor], %%eax; subl %%edx, %%eax; ret;"
#undef OBFH_SF_LEAF_J
#define OBFH_SF_LEAF_J "movl 4(%%esp), %%eax; movl 8(%%esp), %%edx; .fill (%c[sf_phantom] & 1), 1, 0x90;subl $%c[sf_leaf_frame], %%esp;movl %%eax, %c[sf_leaf_slot](%%esp);.byte 0x81, %c[sf_leaf_alu_b]; .long %c[sf_key_b];testl %%edx, %%edx; js 6f;.byte 0xc1, %c[sf_leaf_shift], %c[sf_rotate];jmp 7f; 6: negl %%eax; 7:movl %c[sf_leaf_slot](%%esp), %%ecx;addl %%ecx, %%eax;addl $%c[sf_leaf_frame], %%esp; ret;"
#undef OBFH_SF_LEAF_K
#define OBFH_SF_LEAF_K "movl 4(%%esp), %%eax; movl 8(%%esp), %%edx; .fill (%c[sf_phantom] & 1), 1, 0x90;movzbl %%al, %%ecx; shrl $8, %%eax; imull $%c[sf_factor], %%ecx; xorl %%ecx, %%eax;.byte %c[sf_leaf_alu_a]; .long %c[sf_key_a];.byte 0xc1, %c[sf_leaf_shift], %c[sf_rotate];addl %%edx, %%eax; ret;"
#undef OBFH_SF_LEAF_L
#define OBFH_SF_LEAF_L "movl 4(%%esp), %%eax; movl 8(%%esp), %%edx; .fill (%c[sf_phantom] & 1), 1, 0x90;subl $%c[sf_leaf_frame], %%esp;movl %%eax, %c[sf_leaf_slot](%%esp);cmpl %%edx, %%eax; jbe 6f;.byte %c[sf_leaf_alu_a]; .long %c[sf_key_a];jmp 7f; 6:.byte 0x81, %c[sf_leaf_alu_b]; .long %c[sf_key_b];xorl %%edx, %%eax; 7:movl %c[sf_leaf_slot](%%esp), %%ecx;subl %%ecx, %%eax;addl $%c[sf_leaf_frame], %%esp; ret;"
#undef OBFH_SF_LEAF_ENTRY
#define OBFH_SF_LEAF_ENTRY(label, body) label ": " body

#endif
// END COMPACT ASM CACHE

// Shared source parameters stay local to each insertion; all guards/layouts remain.
#define OBFH_SF_EMIT(guard, layout)                                                                      \
    __obfh_asm__(OBFH_SF_INPUT "xorl $%c[sf_salt], %%eax; roll $%c[sf_rotate], %%eax;" guard layout "9:" \
                 :                                                                                       \
                 : OBFH_SF_INPUTS                                                                        \
                 : "eax", "edx", "ecx", "cc", "memory")
#define OBFH_SF_ASM(guard, layout) ({ OBFH_SF_CAPTURE; OBFH_SF_EMIT(guard, layout); })
#define OBFH_SF_VARIANT_COUNT 128u
#define OBFH_SF_GROUP_0(index) __builtin_choose_expr((index) <= 3u, __builtin_choose_expr((index) <= 1u, __builtin_choose_expr((index) <= 0u, ({ OBFH_SF_SPEC_0(OBFH_SF_EMIT); }), ({ OBFH_SF_SPEC_1(OBFH_SF_EMIT); })), __builtin_choose_expr((index) <= 2u, ({ OBFH_SF_SPEC_2(OBFH_SF_EMIT); }), ({ OBFH_SF_SPEC_3(OBFH_SF_EMIT); }))), __builtin_choose_expr((index) <= 5u, __builtin_choose_expr((index) <= 4u, ({ OBFH_SF_SPEC_4(OBFH_SF_EMIT); }), ({ OBFH_SF_SPEC_5(OBFH_SF_EMIT); })), __builtin_choose_expr((index) <= 6u, ({ OBFH_SF_SPEC_6(OBFH_SF_EMIT); }), ({ OBFH_SF_SPEC_7(OBFH_SF_EMIT); }))))
#define OBFH_SF_GROUP_1(index) __builtin_choose_expr((index) <= 11u, __builtin_choose_expr((index) <= 9u, __builtin_choose_expr((index) <= 8u, ({ OBFH_SF_SPEC_8(OBFH_SF_EMIT); }), ({ OBFH_SF_SPEC_9(OBFH_SF_EMIT); })), __builtin_choose_expr((index) <= 10u, ({ OBFH_SF_SPEC_10(OBFH_SF_EMIT); }), ({ OBFH_SF_SPEC_11(OBFH_SF_EMIT); }))), __builtin_choose_expr((index) <= 13u, __builtin_choose_expr((index) <= 12u, ({ OBFH_SF_SPEC_12(OBFH_SF_EMIT); }), ({ OBFH_SF_SPEC_13(OBFH_SF_EMIT); })), __builtin_choose_expr((index) <= 14u, ({ OBFH_SF_SPEC_14(OBFH_SF_EMIT); }), ({ OBFH_SF_SPEC_15(OBFH_SF_EMIT); }))))
#define OBFH_SF_GROUP_2(index) __builtin_choose_expr((index) <= 19u, __builtin_choose_expr((index) <= 17u, __builtin_choose_expr((index) <= 16u, ({ OBFH_SF_SPEC_16(OBFH_SF_EMIT); }), ({ OBFH_SF_SPEC_17(OBFH_SF_EMIT); })), __builtin_choose_expr((index) <= 18u, ({ OBFH_SF_SPEC_18(OBFH_SF_EMIT); }), ({ OBFH_SF_SPEC_19(OBFH_SF_EMIT); }))), __builtin_choose_expr((index) <= 21u, __builtin_choose_expr((index) <= 20u, ({ OBFH_SF_SPEC_20(OBFH_SF_EMIT); }), ({ OBFH_SF_SPEC_21(OBFH_SF_EMIT); })), __builtin_choose_expr((index) <= 22u, ({ OBFH_SF_SPEC_22(OBFH_SF_EMIT); }), ({ OBFH_SF_SPEC_23(OBFH_SF_EMIT); }))))
#define OBFH_SF_GROUP_3(index) __builtin_choose_expr((index) <= 27u, __builtin_choose_expr((index) <= 25u, __builtin_choose_expr((index) <= 24u, ({ OBFH_SF_SPEC_24(OBFH_SF_EMIT); }), ({ OBFH_SF_SPEC_25(OBFH_SF_EMIT); })), __builtin_choose_expr((index) <= 26u, ({ OBFH_SF_SPEC_26(OBFH_SF_EMIT); }), ({ OBFH_SF_SPEC_27(OBFH_SF_EMIT); }))), __builtin_choose_expr((index) <= 29u, __builtin_choose_expr((index) <= 28u, ({ OBFH_SF_SPEC_28(OBFH_SF_EMIT); }), ({ OBFH_SF_SPEC_29(OBFH_SF_EMIT); })), __builtin_choose_expr((index) <= 30u, ({ OBFH_SF_SPEC_30(OBFH_SF_EMIT); }), ({ OBFH_SF_SPEC_31(OBFH_SF_EMIT); }))))
#define OBFH_SF_GROUP_4(index) __builtin_choose_expr((index) <= 35u, __builtin_choose_expr((index) <= 33u, __builtin_choose_expr((index) <= 32u, ({ OBFH_SF_SPEC_32(OBFH_SF_EMIT); }), ({ OBFH_SF_SPEC_33(OBFH_SF_EMIT); })), __builtin_choose_expr((index) <= 34u, ({ OBFH_SF_SPEC_34(OBFH_SF_EMIT); }), ({ OBFH_SF_SPEC_35(OBFH_SF_EMIT); }))), __builtin_choose_expr((index) <= 37u, __builtin_choose_expr((index) <= 36u, ({ OBFH_SF_SPEC_36(OBFH_SF_EMIT); }), ({ OBFH_SF_SPEC_37(OBFH_SF_EMIT); })), __builtin_choose_expr((index) <= 38u, ({ OBFH_SF_SPEC_38(OBFH_SF_EMIT); }), ({ OBFH_SF_SPEC_39(OBFH_SF_EMIT); }))))
#define OBFH_SF_GROUP_5(index) __builtin_choose_expr((index) <= 43u, __builtin_choose_expr((index) <= 41u, __builtin_choose_expr((index) <= 40u, ({ OBFH_SF_SPEC_40(OBFH_SF_EMIT); }), ({ OBFH_SF_SPEC_41(OBFH_SF_EMIT); })), __builtin_choose_expr((index) <= 42u, ({ OBFH_SF_SPEC_42(OBFH_SF_EMIT); }), ({ OBFH_SF_SPEC_43(OBFH_SF_EMIT); }))), __builtin_choose_expr((index) <= 45u, __builtin_choose_expr((index) <= 44u, ({ OBFH_SF_SPEC_44(OBFH_SF_EMIT); }), ({ OBFH_SF_SPEC_45(OBFH_SF_EMIT); })), __builtin_choose_expr((index) <= 46u, ({ OBFH_SF_SPEC_46(OBFH_SF_EMIT); }), ({ OBFH_SF_SPEC_47(OBFH_SF_EMIT); }))))
#define OBFH_SF_GROUP_6(index) __builtin_choose_expr((index) <= 51u, __builtin_choose_expr((index) <= 49u, __builtin_choose_expr((index) <= 48u, ({ OBFH_SF_SPEC_48(OBFH_SF_EMIT); }), ({ OBFH_SF_SPEC_49(OBFH_SF_EMIT); })), __builtin_choose_expr((index) <= 50u, ({ OBFH_SF_SPEC_50(OBFH_SF_EMIT); }), ({ OBFH_SF_SPEC_51(OBFH_SF_EMIT); }))), __builtin_choose_expr((index) <= 53u, __builtin_choose_expr((index) <= 52u, ({ OBFH_SF_SPEC_52(OBFH_SF_EMIT); }), ({ OBFH_SF_SPEC_53(OBFH_SF_EMIT); })), __builtin_choose_expr((index) <= 54u, ({ OBFH_SF_SPEC_54(OBFH_SF_EMIT); }), ({ OBFH_SF_SPEC_55(OBFH_SF_EMIT); }))))
#define OBFH_SF_GROUP_7(index) __builtin_choose_expr((index) <= 59u, __builtin_choose_expr((index) <= 57u, __builtin_choose_expr((index) <= 56u, ({ OBFH_SF_SPEC_56(OBFH_SF_EMIT); }), ({ OBFH_SF_SPEC_57(OBFH_SF_EMIT); })), __builtin_choose_expr((index) <= 58u, ({ OBFH_SF_SPEC_58(OBFH_SF_EMIT); }), ({ OBFH_SF_SPEC_59(OBFH_SF_EMIT); }))), __builtin_choose_expr((index) <= 61u, __builtin_choose_expr((index) <= 60u, ({ OBFH_SF_SPEC_60(OBFH_SF_EMIT); }), ({ OBFH_SF_SPEC_61(OBFH_SF_EMIT); })), __builtin_choose_expr((index) <= 62u, ({ OBFH_SF_SPEC_62(OBFH_SF_EMIT); }), ({ OBFH_SF_SPEC_63(OBFH_SF_EMIT); }))))
#define OBFH_SF_GROUP_8(index) __builtin_choose_expr((index) <= 67u, __builtin_choose_expr((index) <= 65u, __builtin_choose_expr((index) <= 64u, ({ OBFH_SF_SPEC_64(OBFH_SF_EMIT); }), ({ OBFH_SF_SPEC_65(OBFH_SF_EMIT); })), __builtin_choose_expr((index) <= 66u, ({ OBFH_SF_SPEC_66(OBFH_SF_EMIT); }), ({ OBFH_SF_SPEC_67(OBFH_SF_EMIT); }))), __builtin_choose_expr((index) <= 69u, __builtin_choose_expr((index) <= 68u, ({ OBFH_SF_SPEC_68(OBFH_SF_EMIT); }), ({ OBFH_SF_SPEC_69(OBFH_SF_EMIT); })), __builtin_choose_expr((index) <= 70u, ({ OBFH_SF_SPEC_70(OBFH_SF_EMIT); }), ({ OBFH_SF_SPEC_71(OBFH_SF_EMIT); }))))
#define OBFH_SF_GROUP_9(index) __builtin_choose_expr((index) <= 75u, __builtin_choose_expr((index) <= 73u, __builtin_choose_expr((index) <= 72u, ({ OBFH_SF_SPEC_72(OBFH_SF_EMIT); }), ({ OBFH_SF_SPEC_73(OBFH_SF_EMIT); })), __builtin_choose_expr((index) <= 74u, ({ OBFH_SF_SPEC_74(OBFH_SF_EMIT); }), ({ OBFH_SF_SPEC_75(OBFH_SF_EMIT); }))), __builtin_choose_expr((index) <= 77u, __builtin_choose_expr((index) <= 76u, ({ OBFH_SF_SPEC_76(OBFH_SF_EMIT); }), ({ OBFH_SF_SPEC_77(OBFH_SF_EMIT); })), __builtin_choose_expr((index) <= 78u, ({ OBFH_SF_SPEC_78(OBFH_SF_EMIT); }), ({ OBFH_SF_SPEC_79(OBFH_SF_EMIT); }))))
#define OBFH_SF_GROUP_10(index) __builtin_choose_expr((index) <= 83u, __builtin_choose_expr((index) <= 81u, __builtin_choose_expr((index) <= 80u, ({ OBFH_SF_SPEC_80(OBFH_SF_EMIT); }), ({ OBFH_SF_SPEC_81(OBFH_SF_EMIT); })), __builtin_choose_expr((index) <= 82u, ({ OBFH_SF_SPEC_82(OBFH_SF_EMIT); }), ({ OBFH_SF_SPEC_83(OBFH_SF_EMIT); }))), __builtin_choose_expr((index) <= 85u, __builtin_choose_expr((index) <= 84u, ({ OBFH_SF_SPEC_84(OBFH_SF_EMIT); }), ({ OBFH_SF_SPEC_85(OBFH_SF_EMIT); })), __builtin_choose_expr((index) <= 86u, ({ OBFH_SF_SPEC_86(OBFH_SF_EMIT); }), ({ OBFH_SF_SPEC_87(OBFH_SF_EMIT); }))))
#define OBFH_SF_GROUP_11(index) __builtin_choose_expr((index) <= 91u, __builtin_choose_expr((index) <= 89u, __builtin_choose_expr((index) <= 88u, ({ OBFH_SF_SPEC_88(OBFH_SF_EMIT); }), ({ OBFH_SF_SPEC_89(OBFH_SF_EMIT); })), __builtin_choose_expr((index) <= 90u, ({ OBFH_SF_SPEC_90(OBFH_SF_EMIT); }), ({ OBFH_SF_SPEC_91(OBFH_SF_EMIT); }))), __builtin_choose_expr((index) <= 93u, __builtin_choose_expr((index) <= 92u, ({ OBFH_SF_SPEC_92(OBFH_SF_EMIT); }), ({ OBFH_SF_SPEC_93(OBFH_SF_EMIT); })), __builtin_choose_expr((index) <= 94u, ({ OBFH_SF_SPEC_94(OBFH_SF_EMIT); }), ({ OBFH_SF_SPEC_95(OBFH_SF_EMIT); }))))
#define OBFH_SF_GROUP_12(index) __builtin_choose_expr((index) <= 99u, __builtin_choose_expr((index) <= 97u, __builtin_choose_expr((index) <= 96u, ({ OBFH_SF_SPEC_96(OBFH_SF_EMIT); }), ({ OBFH_SF_SPEC_97(OBFH_SF_EMIT); })), __builtin_choose_expr((index) <= 98u, ({ OBFH_SF_SPEC_98(OBFH_SF_EMIT); }), ({ OBFH_SF_SPEC_99(OBFH_SF_EMIT); }))), __builtin_choose_expr((index) <= 101u, __builtin_choose_expr((index) <= 100u, ({ OBFH_SF_SPEC_100(OBFH_SF_EMIT); }), ({ OBFH_SF_SPEC_101(OBFH_SF_EMIT); })), __builtin_choose_expr((index) <= 102u, ({ OBFH_SF_SPEC_102(OBFH_SF_EMIT); }), ({ OBFH_SF_SPEC_103(OBFH_SF_EMIT); }))))
#define OBFH_SF_GROUP_13(index) __builtin_choose_expr((index) <= 107u, __builtin_choose_expr((index) <= 105u, __builtin_choose_expr((index) <= 104u, ({ OBFH_SF_SPEC_104(OBFH_SF_EMIT); }), ({ OBFH_SF_SPEC_105(OBFH_SF_EMIT); })), __builtin_choose_expr((index) <= 106u, ({ OBFH_SF_SPEC_106(OBFH_SF_EMIT); }), ({ OBFH_SF_SPEC_107(OBFH_SF_EMIT); }))), __builtin_choose_expr((index) <= 109u, __builtin_choose_expr((index) <= 108u, ({ OBFH_SF_SPEC_108(OBFH_SF_EMIT); }), ({ OBFH_SF_SPEC_109(OBFH_SF_EMIT); })), __builtin_choose_expr((index) <= 110u, ({ OBFH_SF_SPEC_110(OBFH_SF_EMIT); }), ({ OBFH_SF_SPEC_111(OBFH_SF_EMIT); }))))
#define OBFH_SF_GROUP_14(index) __builtin_choose_expr((index) <= 115u, __builtin_choose_expr((index) <= 113u, __builtin_choose_expr((index) <= 112u, ({ OBFH_SF_SPEC_112(OBFH_SF_EMIT); }), ({ OBFH_SF_SPEC_113(OBFH_SF_EMIT); })), __builtin_choose_expr((index) <= 114u, ({ OBFH_SF_SPEC_114(OBFH_SF_EMIT); }), ({ OBFH_SF_SPEC_115(OBFH_SF_EMIT); }))), __builtin_choose_expr((index) <= 117u, __builtin_choose_expr((index) <= 116u, ({ OBFH_SF_SPEC_116(OBFH_SF_EMIT); }), ({ OBFH_SF_SPEC_117(OBFH_SF_EMIT); })), __builtin_choose_expr((index) <= 118u, ({ OBFH_SF_SPEC_118(OBFH_SF_EMIT); }), ({ OBFH_SF_SPEC_119(OBFH_SF_EMIT); }))))
#define OBFH_SF_GROUP_15(index) __builtin_choose_expr((index) <= 123u, __builtin_choose_expr((index) <= 121u, __builtin_choose_expr((index) <= 120u, ({ OBFH_SF_SPEC_120(OBFH_SF_EMIT); }), ({ OBFH_SF_SPEC_121(OBFH_SF_EMIT); })), __builtin_choose_expr((index) <= 122u, ({ OBFH_SF_SPEC_122(OBFH_SF_EMIT); }), ({ OBFH_SF_SPEC_123(OBFH_SF_EMIT); }))), __builtin_choose_expr((index) <= 125u, __builtin_choose_expr((index) <= 124u, ({ OBFH_SF_SPEC_124(OBFH_SF_EMIT); }), ({ OBFH_SF_SPEC_125(OBFH_SF_EMIT); })), __builtin_choose_expr((index) <= 126u, ({ OBFH_SF_SPEC_126(OBFH_SF_EMIT); }), ({ OBFH_SF_SPEC_127(OBFH_SF_EMIT); }))))
#define OBFH_SF_SELECT(index) ({ OBFH_SF_CAPTURE; __builtin_choose_expr((index) <= 63u, __builtin_choose_expr((index) <= 31u, __builtin_choose_expr((index) <= 15u, __builtin_choose_expr((index) <= 7u, OBFH_SF_GROUP_0(index), OBFH_SF_GROUP_1(index)), __builtin_choose_expr((index) <= 23u, OBFH_SF_GROUP_2(index), OBFH_SF_GROUP_3(index))), __builtin_choose_expr((index) <= 47u, __builtin_choose_expr((index) <= 39u, OBFH_SF_GROUP_4(index), OBFH_SF_GROUP_5(index)), __builtin_choose_expr((index) <= 55u, OBFH_SF_GROUP_6(index), OBFH_SF_GROUP_7(index)))), __builtin_choose_expr((index) <= 95u, __builtin_choose_expr((index) <= 79u, __builtin_choose_expr((index) <= 71u, OBFH_SF_GROUP_8(index), OBFH_SF_GROUP_9(index)), __builtin_choose_expr((index) <= 87u, OBFH_SF_GROUP_10(index), OBFH_SF_GROUP_11(index))), __builtin_choose_expr((index) <= 111u, __builtin_choose_expr((index) <= 103u, OBFH_SF_GROUP_12(index), OBFH_SF_GROUP_13(index)), __builtin_choose_expr((index) <= 119u, OBFH_SF_GROUP_14(index), OBFH_SF_GROUP_15(index))))); (void)0; })

// Fixed variant pool: selection changes the emitted layout, not the pool size.
#define OBFH_SF_SPEC_0(emit) emit(OBFH_SF_GUARD_0, OBFH_SF_LAYOUT_0(OBFH_SF_BODY_A, OBFH_SF_BODY_B, OBFH_SF_BODY_C))
#define OBFH_SF_VARIANT_0 OBFH_SF_SPEC_0(OBFH_SF_ASM)
#define OBFH_SF_SPEC_1(emit) emit(OBFH_SF_GUARD_0, OBFH_SF_LAYOUT_1(OBFH_SF_BODY_B, OBFH_SF_BODY_C, OBFH_SF_BODY_D))
#define OBFH_SF_VARIANT_1 OBFH_SF_SPEC_1(OBFH_SF_ASM)
#define OBFH_SF_SPEC_2(emit) emit(OBFH_SF_GUARD_0, OBFH_SF_LAYOUT_2(OBFH_SF_BODY_C, OBFH_SF_BODY_D, OBFH_SF_BODY_A))
#define OBFH_SF_VARIANT_2 OBFH_SF_SPEC_2(OBFH_SF_ASM)
#define OBFH_SF_SPEC_3(emit) emit(OBFH_SF_GUARD_0, OBFH_SF_LAYOUT_3(OBFH_SF_BODY_D, OBFH_SF_BODY_A, OBFH_SF_BODY_B))
#define OBFH_SF_VARIANT_3 OBFH_SF_SPEC_3(OBFH_SF_ASM)
#define OBFH_SF_SPEC_4(emit) emit(OBFH_SF_GUARD_1, OBFH_SF_LAYOUT_0(OBFH_SF_BODY_A, OBFH_SF_BODY_B, OBFH_SF_BODY_C))
#define OBFH_SF_VARIANT_4 OBFH_SF_SPEC_4(OBFH_SF_ASM)
#define OBFH_SF_SPEC_5(emit) emit(OBFH_SF_GUARD_1, OBFH_SF_LAYOUT_1(OBFH_SF_BODY_B, OBFH_SF_BODY_C, OBFH_SF_BODY_D))
#define OBFH_SF_VARIANT_5 OBFH_SF_SPEC_5(OBFH_SF_ASM)
#define OBFH_SF_SPEC_6(emit) emit(OBFH_SF_GUARD_1, OBFH_SF_LAYOUT_2(OBFH_SF_BODY_C, OBFH_SF_BODY_D, OBFH_SF_BODY_A))
#define OBFH_SF_VARIANT_6 OBFH_SF_SPEC_6(OBFH_SF_ASM)
#define OBFH_SF_SPEC_7(emit) emit(OBFH_SF_GUARD_1, OBFH_SF_LAYOUT_3(OBFH_SF_BODY_D, OBFH_SF_BODY_A, OBFH_SF_BODY_B))
#define OBFH_SF_VARIANT_7 OBFH_SF_SPEC_7(OBFH_SF_ASM)
#define OBFH_SF_SPEC_8(emit) emit(OBFH_SF_GUARD_2, OBFH_SF_LAYOUT_0(OBFH_SF_BODY_A, OBFH_SF_BODY_B, OBFH_SF_BODY_C))
#define OBFH_SF_VARIANT_8 OBFH_SF_SPEC_8(OBFH_SF_ASM)
#define OBFH_SF_SPEC_9(emit) emit(OBFH_SF_GUARD_2, OBFH_SF_LAYOUT_1(OBFH_SF_BODY_B, OBFH_SF_BODY_C, OBFH_SF_BODY_D))
#define OBFH_SF_VARIANT_9 OBFH_SF_SPEC_9(OBFH_SF_ASM)
#define OBFH_SF_SPEC_10(emit) emit(OBFH_SF_GUARD_2, OBFH_SF_LAYOUT_2(OBFH_SF_BODY_C, OBFH_SF_BODY_D, OBFH_SF_BODY_A))
#define OBFH_SF_VARIANT_10 OBFH_SF_SPEC_10(OBFH_SF_ASM)
#define OBFH_SF_SPEC_11(emit) emit(OBFH_SF_GUARD_2, OBFH_SF_LAYOUT_3(OBFH_SF_BODY_D, OBFH_SF_BODY_A, OBFH_SF_BODY_B))
#define OBFH_SF_VARIANT_11 OBFH_SF_SPEC_11(OBFH_SF_ASM)
#define OBFH_SF_SPEC_12(emit) emit(OBFH_SF_GUARD_3, OBFH_SF_LAYOUT_0(OBFH_SF_BODY_A, OBFH_SF_BODY_B, OBFH_SF_BODY_C))
#define OBFH_SF_VARIANT_12 OBFH_SF_SPEC_12(OBFH_SF_ASM)
#define OBFH_SF_SPEC_13(emit) emit(OBFH_SF_GUARD_3, OBFH_SF_LAYOUT_1(OBFH_SF_BODY_B, OBFH_SF_BODY_C, OBFH_SF_BODY_D))
#define OBFH_SF_VARIANT_13 OBFH_SF_SPEC_13(OBFH_SF_ASM)
#define OBFH_SF_SPEC_14(emit) emit(OBFH_SF_GUARD_3, OBFH_SF_LAYOUT_2(OBFH_SF_BODY_C, OBFH_SF_BODY_D, OBFH_SF_BODY_A))
#define OBFH_SF_VARIANT_14 OBFH_SF_SPEC_14(OBFH_SF_ASM)
#define OBFH_SF_SPEC_15(emit) emit(OBFH_SF_GUARD_3, OBFH_SF_LAYOUT_3(OBFH_SF_BODY_D, OBFH_SF_BODY_A, OBFH_SF_BODY_B))
#define OBFH_SF_VARIANT_15 OBFH_SF_SPEC_15(OBFH_SF_ASM)
#define OBFH_SF_SPEC_16(emit) emit(OBFH_SF_GUARD_4, OBFH_SF_LAYOUT_4(OBFH_SF_BODY_E, OBFH_SF_BODY_J, OBFH_SF_BODY_B))
#define OBFH_SF_VARIANT_16 OBFH_SF_SPEC_16(OBFH_SF_ASM)
#define OBFH_SF_SPEC_17(emit) emit(OBFH_SF_GUARD_4, OBFH_SF_LAYOUT_5(OBFH_SF_BODY_F, OBFH_SF_BODY_K, OBFH_SF_BODY_C))
#define OBFH_SF_VARIANT_17 OBFH_SF_SPEC_17(OBFH_SF_ASM)
#define OBFH_SF_SPEC_18(emit) emit(OBFH_SF_GUARD_4, OBFH_SF_LAYOUT_6(OBFH_SF_BODY_G, OBFH_SF_BODY_L, OBFH_SF_BODY_D))
#define OBFH_SF_VARIANT_18 OBFH_SF_SPEC_18(OBFH_SF_ASM)
#define OBFH_SF_SPEC_19(emit) emit(OBFH_SF_GUARD_4, OBFH_SF_LAYOUT_7(OBFH_SF_BODY_H, OBFH_SF_BODY_A, OBFH_SF_BODY_E))
#define OBFH_SF_VARIANT_19 OBFH_SF_SPEC_19(OBFH_SF_ASM)
#define OBFH_SF_SPEC_20(emit) emit(OBFH_SF_GUARD_4, OBFH_SF_LAYOUT_8(OBFH_SF_BODY_I, OBFH_SF_BODY_B, OBFH_SF_BODY_F))
#define OBFH_SF_VARIANT_20 OBFH_SF_SPEC_20(OBFH_SF_ASM)
#define OBFH_SF_SPEC_21(emit) emit(OBFH_SF_GUARD_4, OBFH_SF_LAYOUT_9(OBFH_SF_BODY_J, OBFH_SF_BODY_C, OBFH_SF_BODY_G))
#define OBFH_SF_VARIANT_21 OBFH_SF_SPEC_21(OBFH_SF_ASM)
#define OBFH_SF_SPEC_22(emit) emit(OBFH_SF_GUARD_5, OBFH_SF_LAYOUT_4(OBFH_SF_BODY_K, OBFH_SF_BODY_D, OBFH_SF_BODY_H))
#define OBFH_SF_VARIANT_22 OBFH_SF_SPEC_22(OBFH_SF_ASM)
#define OBFH_SF_SPEC_23(emit) emit(OBFH_SF_GUARD_5, OBFH_SF_LAYOUT_5(OBFH_SF_BODY_L, OBFH_SF_BODY_E, OBFH_SF_BODY_I))
#define OBFH_SF_VARIANT_23 OBFH_SF_SPEC_23(OBFH_SF_ASM)
#define OBFH_SF_SPEC_24(emit) emit(OBFH_SF_GUARD_5, OBFH_SF_LAYOUT_6(OBFH_SF_BODY_A, OBFH_SF_BODY_F, OBFH_SF_BODY_J))
#define OBFH_SF_VARIANT_24 OBFH_SF_SPEC_24(OBFH_SF_ASM)
#define OBFH_SF_SPEC_25(emit) emit(OBFH_SF_GUARD_5, OBFH_SF_LAYOUT_7(OBFH_SF_BODY_B, OBFH_SF_BODY_G, OBFH_SF_BODY_K))
#define OBFH_SF_VARIANT_25 OBFH_SF_SPEC_25(OBFH_SF_ASM)
#define OBFH_SF_SPEC_26(emit) emit(OBFH_SF_GUARD_5, OBFH_SF_LAYOUT_8(OBFH_SF_BODY_C, OBFH_SF_BODY_H, OBFH_SF_BODY_L))
#define OBFH_SF_VARIANT_26 OBFH_SF_SPEC_26(OBFH_SF_ASM)
#define OBFH_SF_SPEC_27(emit) emit(OBFH_SF_GUARD_5, OBFH_SF_LAYOUT_9(OBFH_SF_BODY_D, OBFH_SF_BODY_I, OBFH_SF_BODY_A))
#define OBFH_SF_VARIANT_27 OBFH_SF_SPEC_27(OBFH_SF_ASM)
#define OBFH_SF_SPEC_28(emit) emit(OBFH_SF_GUARD_6, OBFH_SF_LAYOUT_4(OBFH_SF_BODY_E, OBFH_SF_BODY_J, OBFH_SF_BODY_B))
#define OBFH_SF_VARIANT_28 OBFH_SF_SPEC_28(OBFH_SF_ASM)
#define OBFH_SF_SPEC_29(emit) emit(OBFH_SF_GUARD_6, OBFH_SF_LAYOUT_5(OBFH_SF_BODY_F, OBFH_SF_BODY_K, OBFH_SF_BODY_C))
#define OBFH_SF_VARIANT_29 OBFH_SF_SPEC_29(OBFH_SF_ASM)
#define OBFH_SF_SPEC_30(emit) emit(OBFH_SF_GUARD_6, OBFH_SF_LAYOUT_6(OBFH_SF_BODY_G, OBFH_SF_BODY_L, OBFH_SF_BODY_D))
#define OBFH_SF_VARIANT_30 OBFH_SF_SPEC_30(OBFH_SF_ASM)
#define OBFH_SF_SPEC_31(emit) emit(OBFH_SF_GUARD_6, OBFH_SF_LAYOUT_7(OBFH_SF_BODY_H, OBFH_SF_BODY_A, OBFH_SF_BODY_E))
#define OBFH_SF_VARIANT_31 OBFH_SF_SPEC_31(OBFH_SF_ASM)
#define OBFH_SF_SPEC_32(emit) emit(OBFH_SF_GUARD_6, OBFH_SF_LAYOUT_8(OBFH_SF_BODY_I, OBFH_SF_BODY_B, OBFH_SF_BODY_F))
#define OBFH_SF_VARIANT_32 OBFH_SF_SPEC_32(OBFH_SF_ASM)
#define OBFH_SF_SPEC_33(emit) emit(OBFH_SF_GUARD_6, OBFH_SF_LAYOUT_9(OBFH_SF_BODY_J, OBFH_SF_BODY_C, OBFH_SF_BODY_G))
#define OBFH_SF_VARIANT_33 OBFH_SF_SPEC_33(OBFH_SF_ASM)
#define OBFH_SF_SPEC_34(emit) emit(OBFH_SF_GUARD_7, OBFH_SF_LAYOUT_4(OBFH_SF_BODY_K, OBFH_SF_BODY_D, OBFH_SF_BODY_H))
#define OBFH_SF_VARIANT_34 OBFH_SF_SPEC_34(OBFH_SF_ASM)
#define OBFH_SF_SPEC_35(emit) emit(OBFH_SF_GUARD_7, OBFH_SF_LAYOUT_5(OBFH_SF_BODY_L, OBFH_SF_BODY_E, OBFH_SF_BODY_I))
#define OBFH_SF_VARIANT_35 OBFH_SF_SPEC_35(OBFH_SF_ASM)
#define OBFH_SF_SPEC_36(emit) emit(OBFH_SF_GUARD_7, OBFH_SF_LAYOUT_6(OBFH_SF_BODY_A, OBFH_SF_BODY_F, OBFH_SF_BODY_J))
#define OBFH_SF_VARIANT_36 OBFH_SF_SPEC_36(OBFH_SF_ASM)
#define OBFH_SF_SPEC_37(emit) emit(OBFH_SF_GUARD_7, OBFH_SF_LAYOUT_7(OBFH_SF_BODY_B, OBFH_SF_BODY_G, OBFH_SF_BODY_K))
#define OBFH_SF_VARIANT_37 OBFH_SF_SPEC_37(OBFH_SF_ASM)
#define OBFH_SF_SPEC_38(emit) emit(OBFH_SF_GUARD_7, OBFH_SF_LAYOUT_8(OBFH_SF_BODY_C, OBFH_SF_BODY_H, OBFH_SF_BODY_L))
#define OBFH_SF_VARIANT_38 OBFH_SF_SPEC_38(OBFH_SF_ASM)
#define OBFH_SF_SPEC_39(emit) emit(OBFH_SF_GUARD_7, OBFH_SF_LAYOUT_9(OBFH_SF_BODY_D, OBFH_SF_BODY_I, OBFH_SF_BODY_A))
#define OBFH_SF_VARIANT_39 OBFH_SF_SPEC_39(OBFH_SF_ASM)
#define OBFH_SF_SPEC_40(emit) emit(OBFH_SF_GUARD_8, OBFH_SF_LAYOUT_4(OBFH_SF_BODY_E, OBFH_SF_BODY_J, OBFH_SF_BODY_B))
#define OBFH_SF_VARIANT_40 OBFH_SF_SPEC_40(OBFH_SF_ASM)
#define OBFH_SF_SPEC_41(emit) emit(OBFH_SF_GUARD_8, OBFH_SF_LAYOUT_5(OBFH_SF_BODY_F, OBFH_SF_BODY_K, OBFH_SF_BODY_C))
#define OBFH_SF_VARIANT_41 OBFH_SF_SPEC_41(OBFH_SF_ASM)
#define OBFH_SF_SPEC_42(emit) emit(OBFH_SF_GUARD_8, OBFH_SF_LAYOUT_6(OBFH_SF_BODY_G, OBFH_SF_BODY_L, OBFH_SF_BODY_D))
#define OBFH_SF_VARIANT_42 OBFH_SF_SPEC_42(OBFH_SF_ASM)
#define OBFH_SF_SPEC_43(emit) emit(OBFH_SF_GUARD_8, OBFH_SF_LAYOUT_7(OBFH_SF_BODY_H, OBFH_SF_BODY_A, OBFH_SF_BODY_E))
#define OBFH_SF_VARIANT_43 OBFH_SF_SPEC_43(OBFH_SF_ASM)
#define OBFH_SF_SPEC_44(emit) emit(OBFH_SF_GUARD_8, OBFH_SF_LAYOUT_8(OBFH_SF_BODY_I, OBFH_SF_BODY_B, OBFH_SF_BODY_F))
#define OBFH_SF_VARIANT_44 OBFH_SF_SPEC_44(OBFH_SF_ASM)
#define OBFH_SF_SPEC_45(emit) emit(OBFH_SF_GUARD_8, OBFH_SF_LAYOUT_9(OBFH_SF_BODY_J, OBFH_SF_BODY_C, OBFH_SF_BODY_G))
#define OBFH_SF_VARIANT_45 OBFH_SF_SPEC_45(OBFH_SF_ASM)
#define OBFH_SF_SPEC_46(emit) emit(OBFH_SF_GUARD_0, OBFH_SF_LAYOUT_22(OBFH_SF_BODY_A, OBFH_SF_BODY_H, OBFH_SF_LEAF_A, OBFH_SF_LEAF_E))
#define OBFH_SF_VARIANT_46 OBFH_SF_SPEC_46(OBFH_SF_ASM)
#define OBFH_SF_SPEC_47(emit) emit(OBFH_SF_GUARD_0, OBFH_SF_LAYOUT_23(OBFH_SF_BODY_D, OBFH_SF_BODY_K, OBFH_SF_LEAF_F, OBFH_SF_LEAF_L))
#define OBFH_SF_VARIANT_47 OBFH_SF_SPEC_47(OBFH_SF_ASM)
#define OBFH_SF_SPEC_48(emit) emit(OBFH_SF_GUARD_0, OBFH_SF_LAYOUT_24(OBFH_SF_BODY_G, OBFH_SF_BODY_N, OBFH_SF_LEAF_K, OBFH_SF_LEAF_G))
#define OBFH_SF_VARIANT_48 OBFH_SF_SPEC_48(OBFH_SF_ASM)
#define OBFH_SF_SPEC_49(emit) emit(OBFH_SF_GUARD_0, OBFH_SF_LAYOUT_25(OBFH_SF_BODY_J, OBFH_SF_BODY_Q, OBFH_SF_LEAF_D, OBFH_SF_LEAF_C))
#define OBFH_SF_VARIANT_49 OBFH_SF_SPEC_49(OBFH_SF_ASM)
#define OBFH_SF_SPEC_50(emit) emit(OBFH_SF_GUARD_0, OBFH_SF_LAYOUT_26(OBFH_SF_BODY_M, OBFH_SF_BODY_T, OBFH_SF_LEAF_I, OBFH_SF_LEAF_J))
#define OBFH_SF_VARIANT_50 OBFH_SF_SPEC_50(OBFH_SF_ASM)
#define OBFH_SF_SPEC_51(emit) emit(OBFH_SF_GUARD_0, OBFH_SF_LAYOUT_27(OBFH_SF_BODY_P, OBFH_SF_BODY_C, OBFH_SF_LEAF_B, OBFH_SF_LEAF_E))
#define OBFH_SF_VARIANT_51 OBFH_SF_SPEC_51(OBFH_SF_ASM)
#define OBFH_SF_SPEC_52(emit) emit(OBFH_SF_GUARD_1, OBFH_SF_LAYOUT_22(OBFH_SF_BODY_S, OBFH_SF_BODY_F, OBFH_SF_LEAF_H, OBFH_SF_LEAF_A))
#define OBFH_SF_VARIANT_52 OBFH_SF_SPEC_52(OBFH_SF_ASM)
#define OBFH_SF_SPEC_53(emit) emit(OBFH_SF_GUARD_1, OBFH_SF_LAYOUT_23(OBFH_SF_BODY_B, OBFH_SF_BODY_I, OBFH_SF_LEAF_A, OBFH_SF_LEAF_H))
#define OBFH_SF_VARIANT_53 OBFH_SF_SPEC_53(OBFH_SF_ASM)
#define OBFH_SF_SPEC_54(emit) emit(OBFH_SF_GUARD_1, OBFH_SF_LAYOUT_24(OBFH_SF_BODY_E, OBFH_SF_BODY_L, OBFH_SF_LEAF_F, OBFH_SF_LEAF_C))
#define OBFH_SF_VARIANT_54 OBFH_SF_SPEC_54(OBFH_SF_ASM)
#define OBFH_SF_SPEC_55(emit) emit(OBFH_SF_GUARD_1, OBFH_SF_LAYOUT_25(OBFH_SF_BODY_H, OBFH_SF_BODY_O, OBFH_SF_LEAF_K, OBFH_SF_LEAF_K))
#define OBFH_SF_VARIANT_55 OBFH_SF_SPEC_55(OBFH_SF_ASM)
#define OBFH_SF_SPEC_56(emit) emit(OBFH_SF_GUARD_1, OBFH_SF_LAYOUT_26(OBFH_SF_BODY_K, OBFH_SF_BODY_R, OBFH_SF_LEAF_D, OBFH_SF_LEAF_F))
#define OBFH_SF_VARIANT_56 OBFH_SF_SPEC_56(OBFH_SF_ASM)
#define OBFH_SF_SPEC_57(emit) emit(OBFH_SF_GUARD_1, OBFH_SF_LAYOUT_27(OBFH_SF_BODY_N, OBFH_SF_BODY_A, OBFH_SF_LEAF_I, OBFH_SF_LEAF_A))
#define OBFH_SF_VARIANT_57 OBFH_SF_SPEC_57(OBFH_SF_ASM)
#define OBFH_SF_SPEC_58(emit) emit(OBFH_SF_GUARD_2, OBFH_SF_LAYOUT_22(OBFH_SF_BODY_Q, OBFH_SF_BODY_D, OBFH_SF_LEAF_C, OBFH_SF_LEAF_I))
#define OBFH_SF_VARIANT_58 OBFH_SF_SPEC_58(OBFH_SF_ASM)
#define OBFH_SF_SPEC_59(emit) emit(OBFH_SF_GUARD_2, OBFH_SF_LAYOUT_23(OBFH_SF_BODY_T, OBFH_SF_BODY_G, OBFH_SF_LEAF_H, OBFH_SF_LEAF_D))
#define OBFH_SF_VARIANT_59 OBFH_SF_SPEC_59(OBFH_SF_ASM)
#define OBFH_SF_SPEC_60(emit) emit(OBFH_SF_GUARD_2, OBFH_SF_LAYOUT_24(OBFH_SF_BODY_C, OBFH_SF_BODY_J, OBFH_SF_LEAF_A, OBFH_SF_LEAF_K))
#define OBFH_SF_VARIANT_60 OBFH_SF_SPEC_60(OBFH_SF_ASM)
#define OBFH_SF_SPEC_61(emit) emit(OBFH_SF_GUARD_2, OBFH_SF_LAYOUT_25(OBFH_SF_BODY_F, OBFH_SF_BODY_M, OBFH_SF_LEAF_F, OBFH_SF_LEAF_G))
#define OBFH_SF_VARIANT_61 OBFH_SF_SPEC_61(OBFH_SF_ASM)
#define OBFH_SF_SPEC_62(emit) emit(OBFH_SF_GUARD_2, OBFH_SF_LAYOUT_26(OBFH_SF_BODY_I, OBFH_SF_BODY_P, OBFH_SF_LEAF_K, OBFH_SF_LEAF_B))
#define OBFH_SF_VARIANT_62 OBFH_SF_SPEC_62(OBFH_SF_ASM)
#define OBFH_SF_SPEC_63(emit) emit(OBFH_SF_GUARD_2, OBFH_SF_LAYOUT_27(OBFH_SF_BODY_L, OBFH_SF_BODY_S, OBFH_SF_LEAF_D, OBFH_SF_LEAF_I))
#define OBFH_SF_VARIANT_63 OBFH_SF_SPEC_63(OBFH_SF_ASM)
#define OBFH_SF_SPEC_64(emit) emit(OBFH_SF_GUARD_3, OBFH_SF_LAYOUT_22(OBFH_SF_BODY_O, OBFH_SF_BODY_B, OBFH_SF_LEAF_J, OBFH_SF_LEAF_E))
#define OBFH_SF_VARIANT_64 OBFH_SF_SPEC_64(OBFH_SF_ASM)
#define OBFH_SF_SPEC_65(emit) emit(OBFH_SF_GUARD_3, OBFH_SF_LAYOUT_23(OBFH_SF_BODY_R, OBFH_SF_BODY_E, OBFH_SF_LEAF_C, OBFH_SF_LEAF_L))
#define OBFH_SF_VARIANT_65 OBFH_SF_SPEC_65(OBFH_SF_ASM)
#define OBFH_SF_SPEC_66(emit) emit(OBFH_SF_GUARD_3, OBFH_SF_LAYOUT_24(OBFH_SF_BODY_A, OBFH_SF_BODY_H, OBFH_SF_LEAF_H, OBFH_SF_LEAF_G))
#define OBFH_SF_VARIANT_66 OBFH_SF_SPEC_66(OBFH_SF_ASM)
#define OBFH_SF_SPEC_67(emit) emit(OBFH_SF_GUARD_3, OBFH_SF_LAYOUT_25(OBFH_SF_BODY_D, OBFH_SF_BODY_K, OBFH_SF_LEAF_A, OBFH_SF_LEAF_C))
#define OBFH_SF_VARIANT_67 OBFH_SF_SPEC_67(OBFH_SF_ASM)
#define OBFH_SF_SPEC_68(emit) emit(OBFH_SF_GUARD_3, OBFH_SF_LAYOUT_26(OBFH_SF_BODY_G, OBFH_SF_BODY_N, OBFH_SF_LEAF_F, OBFH_SF_LEAF_J))
#define OBFH_SF_VARIANT_68 OBFH_SF_SPEC_68(OBFH_SF_ASM)
#define OBFH_SF_SPEC_69(emit) emit(OBFH_SF_GUARD_3, OBFH_SF_LAYOUT_27(OBFH_SF_BODY_J, OBFH_SF_BODY_Q, OBFH_SF_LEAF_K, OBFH_SF_LEAF_E))
#define OBFH_SF_VARIANT_69 OBFH_SF_SPEC_69(OBFH_SF_ASM)
#define OBFH_SF_SPEC_70(emit) emit(OBFH_SF_GUARD_4, OBFH_SF_LAYOUT_22(OBFH_SF_BODY_M, OBFH_SF_BODY_T, OBFH_SF_LEAF_E, OBFH_SF_LEAF_A))
#define OBFH_SF_VARIANT_70 OBFH_SF_SPEC_70(OBFH_SF_ASM)
#define OBFH_SF_SPEC_71(emit) emit(OBFH_SF_GUARD_4, OBFH_SF_LAYOUT_23(OBFH_SF_BODY_P, OBFH_SF_BODY_C, OBFH_SF_LEAF_J, OBFH_SF_LEAF_H))
#define OBFH_SF_VARIANT_71 OBFH_SF_SPEC_71(OBFH_SF_ASM)
#define OBFH_SF_SPEC_72(emit) emit(OBFH_SF_GUARD_4, OBFH_SF_LAYOUT_24(OBFH_SF_BODY_S, OBFH_SF_BODY_F, OBFH_SF_LEAF_C, OBFH_SF_LEAF_C))
#define OBFH_SF_VARIANT_72 OBFH_SF_SPEC_72(OBFH_SF_ASM)
#define OBFH_SF_SPEC_73(emit) emit(OBFH_SF_GUARD_4, OBFH_SF_LAYOUT_25(OBFH_SF_BODY_B, OBFH_SF_BODY_I, OBFH_SF_LEAF_H, OBFH_SF_LEAF_K))
#define OBFH_SF_VARIANT_73 OBFH_SF_SPEC_73(OBFH_SF_ASM)
#define OBFH_SF_SPEC_74(emit) emit(OBFH_SF_GUARD_4, OBFH_SF_LAYOUT_26(OBFH_SF_BODY_E, OBFH_SF_BODY_L, OBFH_SF_LEAF_A, OBFH_SF_LEAF_F))
#define OBFH_SF_VARIANT_74 OBFH_SF_SPEC_74(OBFH_SF_ASM)
#define OBFH_SF_SPEC_75(emit) emit(OBFH_SF_GUARD_4, OBFH_SF_LAYOUT_27(OBFH_SF_BODY_H, OBFH_SF_BODY_O, OBFH_SF_LEAF_F, OBFH_SF_LEAF_A))
#define OBFH_SF_VARIANT_75 OBFH_SF_SPEC_75(OBFH_SF_ASM)
#define OBFH_SF_SPEC_76(emit) emit(OBFH_SF_GUARD_5, OBFH_SF_LAYOUT_22(OBFH_SF_BODY_K, OBFH_SF_BODY_R, OBFH_SF_LEAF_L, OBFH_SF_LEAF_I))
#define OBFH_SF_VARIANT_76 OBFH_SF_SPEC_76(OBFH_SF_ASM)
#define OBFH_SF_SPEC_77(emit) emit(OBFH_SF_GUARD_5, OBFH_SF_LAYOUT_23(OBFH_SF_BODY_N, OBFH_SF_BODY_A, OBFH_SF_LEAF_E, OBFH_SF_LEAF_D))
#define OBFH_SF_VARIANT_77 OBFH_SF_SPEC_77(OBFH_SF_ASM)
#define OBFH_SF_SPEC_78(emit) emit(OBFH_SF_GUARD_5, OBFH_SF_LAYOUT_24(OBFH_SF_BODY_Q, OBFH_SF_BODY_D, OBFH_SF_LEAF_J, OBFH_SF_LEAF_K))
#define OBFH_SF_VARIANT_78 OBFH_SF_SPEC_78(OBFH_SF_ASM)
#define OBFH_SF_SPEC_79(emit) emit(OBFH_SF_GUARD_5, OBFH_SF_LAYOUT_25(OBFH_SF_BODY_T, OBFH_SF_BODY_G, OBFH_SF_LEAF_C, OBFH_SF_LEAF_G))
#define OBFH_SF_VARIANT_79 OBFH_SF_SPEC_79(OBFH_SF_ASM)
#define OBFH_SF_SPEC_80(emit) emit(OBFH_SF_GUARD_5, OBFH_SF_LAYOUT_26(OBFH_SF_BODY_C, OBFH_SF_BODY_J, OBFH_SF_LEAF_H, OBFH_SF_LEAF_B))
#define OBFH_SF_VARIANT_80 OBFH_SF_SPEC_80(OBFH_SF_ASM)
#define OBFH_SF_SPEC_81(emit) emit(OBFH_SF_GUARD_5, OBFH_SF_LAYOUT_27(OBFH_SF_BODY_F, OBFH_SF_BODY_M, OBFH_SF_LEAF_A, OBFH_SF_LEAF_I))
#define OBFH_SF_VARIANT_81 OBFH_SF_SPEC_81(OBFH_SF_ASM)
#define OBFH_SF_SPEC_82(emit) emit(OBFH_SF_GUARD_6, OBFH_SF_LAYOUT_22(OBFH_SF_BODY_I, OBFH_SF_BODY_P, OBFH_SF_LEAF_G, OBFH_SF_LEAF_E))
#define OBFH_SF_VARIANT_82 OBFH_SF_SPEC_82(OBFH_SF_ASM)
#define OBFH_SF_SPEC_83(emit) emit(OBFH_SF_GUARD_6, OBFH_SF_LAYOUT_23(OBFH_SF_BODY_L, OBFH_SF_BODY_S, OBFH_SF_LEAF_L, OBFH_SF_LEAF_L))
#define OBFH_SF_VARIANT_83 OBFH_SF_SPEC_83(OBFH_SF_ASM)
#define OBFH_SF_SPEC_84(emit) emit(OBFH_SF_GUARD_6, OBFH_SF_LAYOUT_24(OBFH_SF_BODY_O, OBFH_SF_BODY_B, OBFH_SF_LEAF_E, OBFH_SF_LEAF_G))
#define OBFH_SF_VARIANT_84 OBFH_SF_SPEC_84(OBFH_SF_ASM)
#define OBFH_SF_SPEC_85(emit) emit(OBFH_SF_GUARD_6, OBFH_SF_LAYOUT_25(OBFH_SF_BODY_R, OBFH_SF_BODY_E, OBFH_SF_LEAF_J, OBFH_SF_LEAF_C))
#define OBFH_SF_VARIANT_85 OBFH_SF_SPEC_85(OBFH_SF_ASM)
#define OBFH_SF_SPEC_86(emit) emit(OBFH_SF_GUARD_6, OBFH_SF_LAYOUT_26(OBFH_SF_BODY_A, OBFH_SF_BODY_H, OBFH_SF_LEAF_C, OBFH_SF_LEAF_J))
#define OBFH_SF_VARIANT_86 OBFH_SF_SPEC_86(OBFH_SF_ASM)
#define OBFH_SF_SPEC_87(emit) emit(OBFH_SF_GUARD_6, OBFH_SF_LAYOUT_27(OBFH_SF_BODY_D, OBFH_SF_BODY_K, OBFH_SF_LEAF_H, OBFH_SF_LEAF_E))
#define OBFH_SF_VARIANT_87 OBFH_SF_SPEC_87(OBFH_SF_ASM)
#define OBFH_SF_SPEC_88(emit) emit(OBFH_SF_GUARD_7, OBFH_SF_LAYOUT_22(OBFH_SF_BODY_G, OBFH_SF_BODY_N, OBFH_SF_LEAF_B, OBFH_SF_LEAF_A))
#define OBFH_SF_VARIANT_88 OBFH_SF_SPEC_88(OBFH_SF_ASM)
#define OBFH_SF_SPEC_89(emit) emit(OBFH_SF_GUARD_7, OBFH_SF_LAYOUT_23(OBFH_SF_BODY_J, OBFH_SF_BODY_Q, OBFH_SF_LEAF_G, OBFH_SF_LEAF_H))
#define OBFH_SF_VARIANT_89 OBFH_SF_SPEC_89(OBFH_SF_ASM)
#define OBFH_SF_SPEC_90(emit) emit(OBFH_SF_GUARD_7, OBFH_SF_LAYOUT_24(OBFH_SF_BODY_M, OBFH_SF_BODY_T, OBFH_SF_LEAF_L, OBFH_SF_LEAF_C))
#define OBFH_SF_VARIANT_90 OBFH_SF_SPEC_90(OBFH_SF_ASM)
#define OBFH_SF_SPEC_91(emit) emit(OBFH_SF_GUARD_7, OBFH_SF_LAYOUT_25(OBFH_SF_BODY_P, OBFH_SF_BODY_C, OBFH_SF_LEAF_E, OBFH_SF_LEAF_K))
#define OBFH_SF_VARIANT_91 OBFH_SF_SPEC_91(OBFH_SF_ASM)
#define OBFH_SF_SPEC_92(emit) emit(OBFH_SF_GUARD_7, OBFH_SF_LAYOUT_26(OBFH_SF_BODY_S, OBFH_SF_BODY_F, OBFH_SF_LEAF_J, OBFH_SF_LEAF_F))
#define OBFH_SF_VARIANT_92 OBFH_SF_SPEC_92(OBFH_SF_ASM)
#define OBFH_SF_SPEC_93(emit) emit(OBFH_SF_GUARD_7, OBFH_SF_LAYOUT_27(OBFH_SF_BODY_B, OBFH_SF_BODY_I, OBFH_SF_LEAF_C, OBFH_SF_LEAF_A))
#define OBFH_SF_VARIANT_93 OBFH_SF_SPEC_93(OBFH_SF_ASM)
#define OBFH_SF_SPEC_94(emit) emit(OBFH_SF_GUARD_8, OBFH_SF_LAYOUT_22(OBFH_SF_BODY_E, OBFH_SF_BODY_L, OBFH_SF_LEAF_I, OBFH_SF_LEAF_I))
#define OBFH_SF_VARIANT_94 OBFH_SF_SPEC_94(OBFH_SF_ASM)
#define OBFH_SF_SPEC_95(emit) emit(OBFH_SF_GUARD_8, OBFH_SF_LAYOUT_23(OBFH_SF_BODY_H, OBFH_SF_BODY_O, OBFH_SF_LEAF_B, OBFH_SF_LEAF_D))
#define OBFH_SF_VARIANT_95 OBFH_SF_SPEC_95(OBFH_SF_ASM)
#define OBFH_SF_SPEC_96(emit) emit(OBFH_SF_GUARD_8, OBFH_SF_LAYOUT_24(OBFH_SF_BODY_K, OBFH_SF_BODY_R, OBFH_SF_LEAF_G, OBFH_SF_LEAF_K))
#define OBFH_SF_VARIANT_96 OBFH_SF_SPEC_96(OBFH_SF_ASM)
#define OBFH_SF_SPEC_97(emit) emit(OBFH_SF_GUARD_8, OBFH_SF_LAYOUT_25(OBFH_SF_BODY_N, OBFH_SF_BODY_A, OBFH_SF_LEAF_L, OBFH_SF_LEAF_G))
#define OBFH_SF_VARIANT_97 OBFH_SF_SPEC_97(OBFH_SF_ASM)
#define OBFH_SF_SPEC_98(emit) emit(OBFH_SF_GUARD_8, OBFH_SF_LAYOUT_26(OBFH_SF_BODY_Q, OBFH_SF_BODY_D, OBFH_SF_LEAF_E, OBFH_SF_LEAF_B))
#define OBFH_SF_VARIANT_98 OBFH_SF_SPEC_98(OBFH_SF_ASM)
#define OBFH_SF_SPEC_99(emit) emit(OBFH_SF_GUARD_8, OBFH_SF_LAYOUT_27(OBFH_SF_BODY_T, OBFH_SF_BODY_G, OBFH_SF_LEAF_J, OBFH_SF_LEAF_I))
#define OBFH_SF_VARIANT_99 OBFH_SF_SPEC_99(OBFH_SF_ASM)
#define OBFH_SF_SPEC_100(emit) emit(OBFH_SF_GUARD_9, OBFH_SF_LAYOUT_22(OBFH_SF_BODY_C, OBFH_SF_BODY_J, OBFH_SF_LEAF_D, OBFH_SF_LEAF_E))
#define OBFH_SF_VARIANT_100 OBFH_SF_SPEC_100(OBFH_SF_ASM)
#define OBFH_SF_SPEC_101(emit) emit(OBFH_SF_GUARD_9, OBFH_SF_LAYOUT_23(OBFH_SF_BODY_F, OBFH_SF_BODY_M, OBFH_SF_LEAF_I, OBFH_SF_LEAF_L))
#define OBFH_SF_VARIANT_101 OBFH_SF_SPEC_101(OBFH_SF_ASM)
#define OBFH_SF_SPEC_102(emit) emit(OBFH_SF_GUARD_9, OBFH_SF_LAYOUT_24(OBFH_SF_BODY_I, OBFH_SF_BODY_P, OBFH_SF_LEAF_B, OBFH_SF_LEAF_G))
#define OBFH_SF_VARIANT_102 OBFH_SF_SPEC_102(OBFH_SF_ASM)
#define OBFH_SF_SPEC_103(emit) emit(OBFH_SF_GUARD_9, OBFH_SF_LAYOUT_25(OBFH_SF_BODY_L, OBFH_SF_BODY_S, OBFH_SF_LEAF_G, OBFH_SF_LEAF_C))
#define OBFH_SF_VARIANT_103 OBFH_SF_SPEC_103(OBFH_SF_ASM)
#define OBFH_SF_SPEC_104(emit) emit(OBFH_SF_GUARD_9, OBFH_SF_LAYOUT_26(OBFH_SF_BODY_O, OBFH_SF_BODY_B, OBFH_SF_LEAF_L, OBFH_SF_LEAF_J))
#define OBFH_SF_VARIANT_104 OBFH_SF_SPEC_104(OBFH_SF_ASM)
#define OBFH_SF_SPEC_105(emit) emit(OBFH_SF_GUARD_9, OBFH_SF_LAYOUT_27(OBFH_SF_BODY_R, OBFH_SF_BODY_E, OBFH_SF_LEAF_E, OBFH_SF_LEAF_E))
#define OBFH_SF_VARIANT_105 OBFH_SF_SPEC_105(OBFH_SF_ASM)
#define OBFH_SF_SPEC_106(emit) emit(OBFH_SF_GUARD_10, OBFH_SF_LAYOUT_22(OBFH_SF_BODY_A, OBFH_SF_BODY_H, OBFH_SF_LEAF_K, OBFH_SF_LEAF_A))
#define OBFH_SF_VARIANT_106 OBFH_SF_SPEC_106(OBFH_SF_ASM)
#define OBFH_SF_SPEC_107(emit) emit(OBFH_SF_GUARD_10, OBFH_SF_LAYOUT_23(OBFH_SF_BODY_D, OBFH_SF_BODY_K, OBFH_SF_LEAF_D, OBFH_SF_LEAF_H))
#define OBFH_SF_VARIANT_107 OBFH_SF_SPEC_107(OBFH_SF_ASM)
#define OBFH_SF_SPEC_108(emit) emit(OBFH_SF_GUARD_10, OBFH_SF_LAYOUT_24(OBFH_SF_BODY_G, OBFH_SF_BODY_N, OBFH_SF_LEAF_I, OBFH_SF_LEAF_C))
#define OBFH_SF_VARIANT_108 OBFH_SF_SPEC_108(OBFH_SF_ASM)
#define OBFH_SF_SPEC_109(emit) emit(OBFH_SF_GUARD_10, OBFH_SF_LAYOUT_25(OBFH_SF_BODY_J, OBFH_SF_BODY_Q, OBFH_SF_LEAF_B, OBFH_SF_LEAF_K))
#define OBFH_SF_VARIANT_109 OBFH_SF_SPEC_109(OBFH_SF_ASM)
#define OBFH_SF_SPEC_110(emit) emit(OBFH_SF_GUARD_10, OBFH_SF_LAYOUT_26(OBFH_SF_BODY_M, OBFH_SF_BODY_T, OBFH_SF_LEAF_G, OBFH_SF_LEAF_F))
#define OBFH_SF_VARIANT_110 OBFH_SF_SPEC_110(OBFH_SF_ASM)
#define OBFH_SF_SPEC_111(emit) emit(OBFH_SF_GUARD_10, OBFH_SF_LAYOUT_27(OBFH_SF_BODY_P, OBFH_SF_BODY_C, OBFH_SF_LEAF_L, OBFH_SF_LEAF_A))
#define OBFH_SF_VARIANT_111 OBFH_SF_SPEC_111(OBFH_SF_ASM)
#define OBFH_SF_SPEC_112(emit) emit(OBFH_SF_GUARD_11, OBFH_SF_LAYOUT_22(OBFH_SF_BODY_S, OBFH_SF_BODY_F, OBFH_SF_LEAF_F, OBFH_SF_LEAF_I))
#define OBFH_SF_VARIANT_112 OBFH_SF_SPEC_112(OBFH_SF_ASM)
#define OBFH_SF_SPEC_113(emit) emit(OBFH_SF_GUARD_11, OBFH_SF_LAYOUT_23(OBFH_SF_BODY_B, OBFH_SF_BODY_I, OBFH_SF_LEAF_K, OBFH_SF_LEAF_D))
#define OBFH_SF_VARIANT_113 OBFH_SF_SPEC_113(OBFH_SF_ASM)
#define OBFH_SF_SPEC_114(emit) emit(OBFH_SF_GUARD_11, OBFH_SF_LAYOUT_24(OBFH_SF_BODY_E, OBFH_SF_BODY_L, OBFH_SF_LEAF_D, OBFH_SF_LEAF_K))
#define OBFH_SF_VARIANT_114 OBFH_SF_SPEC_114(OBFH_SF_ASM)
#define OBFH_SF_SPEC_115(emit) emit(OBFH_SF_GUARD_11, OBFH_SF_LAYOUT_25(OBFH_SF_BODY_H, OBFH_SF_BODY_O, OBFH_SF_LEAF_I, OBFH_SF_LEAF_G))
#define OBFH_SF_VARIANT_115 OBFH_SF_SPEC_115(OBFH_SF_ASM)
#define OBFH_SF_SPEC_116(emit) emit(OBFH_SF_GUARD_11, OBFH_SF_LAYOUT_26(OBFH_SF_BODY_K, OBFH_SF_BODY_R, OBFH_SF_LEAF_B, OBFH_SF_LEAF_B))
#define OBFH_SF_VARIANT_116 OBFH_SF_SPEC_116(OBFH_SF_ASM)
#define OBFH_SF_SPEC_117(emit) emit(OBFH_SF_GUARD_11, OBFH_SF_LAYOUT_27(OBFH_SF_BODY_N, OBFH_SF_BODY_A, OBFH_SF_LEAF_G, OBFH_SF_LEAF_I))
#define OBFH_SF_VARIANT_117 OBFH_SF_SPEC_117(OBFH_SF_ASM)
#define OBFH_SF_SPEC_118(emit) emit(OBFH_SF_GUARD_12, OBFH_SF_LAYOUT_22(OBFH_SF_BODY_Q, OBFH_SF_BODY_D, OBFH_SF_LEAF_A, OBFH_SF_LEAF_E))
#define OBFH_SF_VARIANT_118 OBFH_SF_SPEC_118(OBFH_SF_ASM)
#define OBFH_SF_SPEC_119(emit) emit(OBFH_SF_GUARD_12, OBFH_SF_LAYOUT_23(OBFH_SF_BODY_T, OBFH_SF_BODY_G, OBFH_SF_LEAF_F, OBFH_SF_LEAF_L))
#define OBFH_SF_VARIANT_119 OBFH_SF_SPEC_119(OBFH_SF_ASM)
#define OBFH_SF_SPEC_120(emit) emit(OBFH_SF_GUARD_12, OBFH_SF_LAYOUT_24(OBFH_SF_BODY_C, OBFH_SF_BODY_J, OBFH_SF_LEAF_K, OBFH_SF_LEAF_G))
#define OBFH_SF_VARIANT_120 OBFH_SF_SPEC_120(OBFH_SF_ASM)
#define OBFH_SF_SPEC_121(emit) emit(OBFH_SF_GUARD_12, OBFH_SF_LAYOUT_25(OBFH_SF_BODY_F, OBFH_SF_BODY_M, OBFH_SF_LEAF_D, OBFH_SF_LEAF_C))
#define OBFH_SF_VARIANT_121 OBFH_SF_SPEC_121(OBFH_SF_ASM)
#define OBFH_SF_SPEC_122(emit) emit(OBFH_SF_GUARD_12, OBFH_SF_LAYOUT_26(OBFH_SF_BODY_I, OBFH_SF_BODY_P, OBFH_SF_LEAF_I, OBFH_SF_LEAF_J))
#define OBFH_SF_VARIANT_122 OBFH_SF_SPEC_122(OBFH_SF_ASM)
#define OBFH_SF_SPEC_123(emit) emit(OBFH_SF_GUARD_12, OBFH_SF_LAYOUT_27(OBFH_SF_BODY_L, OBFH_SF_BODY_S, OBFH_SF_LEAF_B, OBFH_SF_LEAF_E))
#define OBFH_SF_VARIANT_123 OBFH_SF_SPEC_123(OBFH_SF_ASM)
#define OBFH_SF_SPEC_124(emit) emit(OBFH_SF_GUARD_13, OBFH_SF_LAYOUT_22(OBFH_SF_BODY_O, OBFH_SF_BODY_B, OBFH_SF_LEAF_H, OBFH_SF_LEAF_A))
#define OBFH_SF_VARIANT_124 OBFH_SF_SPEC_124(OBFH_SF_ASM)
#define OBFH_SF_SPEC_125(emit) emit(OBFH_SF_GUARD_13, OBFH_SF_LAYOUT_23(OBFH_SF_BODY_R, OBFH_SF_BODY_E, OBFH_SF_LEAF_A, OBFH_SF_LEAF_H))
#define OBFH_SF_VARIANT_125 OBFH_SF_SPEC_125(OBFH_SF_ASM)
#define OBFH_SF_SPEC_126(emit) emit(OBFH_SF_GUARD_13, OBFH_SF_LAYOUT_24(OBFH_SF_BODY_A, OBFH_SF_BODY_H, OBFH_SF_LEAF_F, OBFH_SF_LEAF_C))
#define OBFH_SF_VARIANT_126 OBFH_SF_SPEC_126(OBFH_SF_ASM)
#define OBFH_SF_SPEC_127(emit) emit(OBFH_SF_GUARD_13, OBFH_SF_LAYOUT_25(OBFH_SF_BODY_D, OBFH_SF_BODY_K, OBFH_SF_LEAF_K, OBFH_SF_LEAF_K))
#define OBFH_SF_VARIANT_127 OBFH_SF_SPEC_127(OBFH_SF_ASM)

#define STACK_PROXY_FUNCTIONS ({                                                                                                                                                    \
    enum { __obfh_sf_id = __COUNTER__,                                                                                                                                              \
           __obfh_sf_variant = OBFH_MIX_B(OBFH_MIX_A((unsigned int)__obfh_sf_id ^ (unsigned int)OBFH_BUILD_SEED ^ (unsigned int)__LINE__ ^ 0x53504631u)) % OBFH_SF_VARIANT_COUNT }; \
    OBFH_SF_SELECT(__obfh_sf_variant);                                                                                                                                              \
    (void)0;                                                                                                                                                                        \
})

// ============================================================================
// 08. Junk primitives and scalar/string transport helpers
// ============================================================================

#if defined(__x86_64__)
#define BAD_JMP __obfh_asm__("cpuid; mov %eax, %rax; mov %ebx, %edx; .byte 0xFF, 0x25, 0xF1, 0xF2, 0xF3, 0xF4;")
#else
#define BAD_JMP __obfh_asm__(".byte 0xEB, 0xE1;")
#endif

#define BAD_CALL __obfh_asm__(".byte 0xB8;")

static void obfh_junk_func_args(int z, ...) OBFH_CODE_SECTION_ATTRIBUTE {
    BREAK_STACK_CFLOW;
    PHANTOM_NOP;
    __obfh_asm__("nop;");
    return;
}

static void obfh_junk_func() OBFH_DATA_CODE_SECTION_ATTRIBUTE {
    BREAK_STACK_CFLOW;
    PHANTOM_NOP;
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

static FARPROC obfh_crt_resolve(const char *name);
static void *malloc_proxy(size_t size) {
    STACK_PROXY_FUNCTIONS;
    BREAK_STACK_CFLOW;
    char name[7];
    OBFH_CRT_NAME_ORDER(
        (name[0] = _m, name[1] = _a, name[2] = _l, name[3] = _l, name[4] = _o, name[5] = _c, name[6] = _0),
        (name[6] = _0, name[5] = _c, name[4] = _o, name[3] = _l, name[2] = _l, name[1] = _a, name[0] = _m));
    PHANTOM_NOP;
    void *result = ((void *(*)(size_t))obfh_crt_resolve(name))(size);
    STACK_PROXY_FUNCTIONS;
    RET_BY_VAR(result);
}
#define malloc(...) malloc_proxy(__VA_ARGS__)

static float rndValueToProxy = RND(0, 10);

static int obfh_int_proxy(int value) OBFH_CODE_SECTION_ATTRIBUTE {
    BREAK_STACK_CFLOW;
    PHANTOM_NOP;
    RET_BY_VAR(value);
}

// Preserve pointer and SIZE_T width on both Windows targets.
static ULONG_PTR obfh_uintptr_proxy(ULONG_PTR value) OBFH_CODE_SECTION_ATTRIBUTE {
    BREAK_STACK_CFLOW;
    PHANTOM_NOP;
    RET_BY_VAR(value);
}

#define OBFH_PTR(type, value) ((type)obfh_uintptr_proxy((ULONG_PTR)(value)))

static double obfh_double_proxy(double value) OBFH_CODE_SECTION_ATTRIBUTE {
    BREAK_STACK_CFLOW;
    PHANTOM_NOP;
    RET_BY_VAR(value);
}

static float obfh_condition_true();

// Hidden string access
static char *obfh_process_hidden_string(char *string, ...) OBFH_CODE_SECTION_ATTRIBUTE {
    BREAK_STACK_CFLOW;
    PHANTOM_NOP;

    if (!obfh_condition_true() || _0) {
        BAD_JMP;
    }

    // ['\0', 's', 't', 'r', 'i', 'n', 'g'] => "string"
    // STACK_STRING storage belongs to the HIDE_STRING call site.
    return string + 1;
}

static float obfh_condition_true() OBFH_CODE_SECTION_ATTRIBUTE {
    BREAK_STACK_CFLOW;
    PHANTOM_NOP;
    return _1 && TRUE;
}

static int obfh_condition_proxy(float junk, float condition, ...) OBFH_CODE_SECTION_ATTRIBUTE {
    BREAK_STACK_CFLOW;
    PHANTOM_NOP;
    RET_BY_VAR(condition);
}

static long double __s_rdtsc(float junk, ...) OBFH_CODE_SECTION_ATTRIBUTE {
    BREAK_STACK_CFLOW;
    {
        unsigned int lo, hi;
        __obfh_asm__(".byte 0x0f, 0x31;"  // rdtsc
                     : "=a"(lo), "=d"(hi)
                     :
                     : "memory");

        unsigned long long rdtsc_result = ((unsigned long long)hi << 32) | lo;

        if (rdtsc_result == obfh_int_proxy(0))
            __obfh_asm__(".byte 0xE8;");
    }

    unsigned long long time;
    unsigned int low, high;
    static volatile LONG rdtscpAvailable = -1;
    LONG supported = rdtscpAvailable;
    PHANTOM_NOP;
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

// ============================================================================
// 09. CRT cache and VM value transport
// ============================================================================

// Cache entries are immutable after atomic publication. Module references are
// retained by the resolver so cached pointers cannot outlive their DLL.
typedef struct {
    volatile LONG state;
    unsigned char name[32];
    ULONG_PTR address;
} OBFH_CRT_ENTRY;
static OBFH_CRT_ENTRY obfh_crt_entries[32];

static FARPROC obfh_crt_cached(const char *name) {
    for (unsigned int slot = 0; slot < 32; ++slot) {
        OBFH_CRT_ENTRY *entry = &obfh_crt_entries[slot];
        if (InterlockedCompareExchange(&entry->state, 2, 2) != 2)
            continue;
        unsigned int i = 0;
        for (; i < sizeof(entry->name); ++i) {
            unsigned char byte = entry->name[i] ^ (unsigned char)(SALT_SHIFT + i * 13u);
            if (byte != (unsigned char)name[i])
                break;
            if (!byte)
                return (FARPROC)obfh_uintptr_proxy(entry->address ^ SALT_SHIFT);
        }
    }
    PHANTOM_NOP;
    return NULL;
}

static void obfh_crt_publish(const char *name, FARPROC function) {
    BREAK_STACK_CFLOW;
    if (!function || obfh_crt_cached(name))
        return;
    unsigned int length = 0;
    while (name[length] && length < 31)
        ++length;
    PHANTOM_NOP;
    if (name[length])
        return;
    for (unsigned int slot = 0; slot < 32; ++slot) {
        OBFH_CRT_ENTRY *entry = &obfh_crt_entries[slot];
        if (InterlockedCompareExchange(&entry->state, 1, 0) != 0)
            continue;
        for (unsigned int i = 0; i <= length; ++i)
            entry->name[i] = (unsigned char)name[i] ^ (unsigned char)(SALT_SHIFT + i * 13u);
        entry->address = (ULONG_PTR)function ^ SALT_SHIFT;
        InterlockedExchange(&entry->state, 2);
        return;
    }
}

#if VIRT == 1
// Transport object bytes instead of adding a salt to the numeric value.
// This preserves subnormals, signed zero, infinities and NaN payloads.
typedef struct {
    unsigned char bytes[sizeof(long double)];
    unsigned char floating;
    unsigned int nonce;
} OBFH_VM_VALUE;

static OBFH_VM_VALUE obfh_vm_encode(long double value, int salt, unsigned int floating) OBFH_CODE_SECTION_ATTRIBUTE {
    BREAK_STACK_CFLOW;
    OBFH_VM_VALUE encoded;
    volatile int key = (int)obfh_double_proxy((double)(float)obfh_int_proxy(salt));
    const unsigned char *bytes = (const unsigned char *)&value;
    encoded.nonce = (floating >> 1) ^ (unsigned int)OBFH_BUILD_SEED;
    encoded.floating = floating & 1u;
    unsigned int state = encoded.nonce ^ ((unsigned int)key * 2246822519u) ^ 0x9e3779b9u;
    size_t offset = encoded.nonce % sizeof(value);
    PHANTOM_NOP;
    for (size_t i = 0; i < sizeof(value); ++i) {
        state ^= state << 13;
        state ^= state >> 17;
        state ^= state << 5;
        unsigned int rotate = (state >> 24) & 7u;
        unsigned char rotated = (unsigned char)((bytes[i] << rotate) | (bytes[i] >> (8u - rotate)));
        encoded.bytes[(i + offset) % sizeof(value)] = (unsigned char)(rotated + (unsigned char)(state >> 8)) ^ (unsigned char)(state ^ (state >> 16));
    }
    return encoded;
}

static long double obfh_vm_decode(OBFH_VM_VALUE encoded, int salt) OBFH_CODE_SECTION_ATTRIBUTE {
    BREAK_STACK_CFLOW;
    long double value;
    volatile int key = obfh_condition_proxy((float)salt, (float)obfh_int_proxy(salt));
    unsigned char *bytes = (unsigned char *)&value;
    unsigned int state = encoded.nonce ^ ((unsigned int)key * 2246822519u) ^ 0x9e3779b9u;
    size_t offset = encoded.nonce % sizeof(value);
    PHANTOM_NOP;
    for (size_t i = 0; i < sizeof(value); ++i) {
        state ^= state << 13;
        state ^= state >> 17;
        state ^= state << 5;
        unsigned int rotate = (state >> 24) & 7u;
        unsigned char rotated = (unsigned char)((encoded.bytes[(i + offset) % sizeof(value)] ^ (unsigned char)(state ^ (state >> 16))) - (unsigned char)(state >> 8));
        bytes[i] = (unsigned char)((rotated >> rotate) | (rotated << (8u - rotate)));
    }
    RET_BY_VAR(value);
}
#endif

// ============================================================================
// 10. CFLOW helpers: selectors, permutations and local graphs
// ============================================================================

#if NO_CFLOW != 1

#ifndef CFLOW_V2
#define CFLOW_V2 0
#endif

// Instruction starts and their following bytes belong only to skipped regions.
#define OBFH_FLOW_OPCODE(kind) \
    ((((kind)&4u ? 0xF3660FFFu : 0xE9E8898Bu) >> (((kind)&3u) * 8u)) & 255u)
#define OBFH_FLOW_DEAD_BYTES()                                                              \
    ({                                                                                      \
        enum { __obfh_dead_kind = OBFH_JUNK_BYTE & 7u };                                    \
        __obfh_asm__(".byte %c0, %c1, %c2, %c3; .long %c4; .fill %c5, 1, %c6;"              \
                     :                                                                      \
                     : "i"(OBFH_FLOW_OPCODE(__obfh_dead_kind)),                             \
                       "i"(OBFH_JUNK_BYTE), "i"(OBFH_JUNK_BYTE), "i"(OBFH_JUNK_BYTE),       \
                       "i"(OBFH_JUNK_WORD), "i"(OBFH_JUNK_BYTE & 7u), "i"(OBFH_JUNK_BYTE)); \
    })

// A native false guard runs only on the original else path; no loop or switch wrapper.
#define OBFH_FLOW_ELSE_GUARD()                                                         \
    (({                                                                                \
        enum { __obfh_else_kind = OBFH_JUNK_BYTE & 7u,                                 \
               __obfh_else_gap = OBFH_JUNK_BYTE & 3u };                                \
        __obfh_asm__(".byte 0xEB, %c5; .byte %c0, %c1; .long %c2; .fill %c3, 1, %c4;"  \
                     :                                                                 \
                     : "i"(OBFH_FLOW_OPCODE(__obfh_else_kind)), "i"(OBFH_JUNK_BYTE),   \
                       "i"(OBFH_JUNK_WORD), "i"(__obfh_else_gap), "i"(OBFH_JUNK_BYTE), \
                       "i"(6u + __obfh_else_gap));                                     \
        0;                                                                             \
    }))

// Local condition transport: disjoint input tags and path-specific permutations.
#ifdef OBFH_TEST_FLOW_TRACE
#define OBFH_P_BEFORE unsigned int __obfh_before_state = __obfh_flow_state, __obfh_before_tag = __obfh_flow_tag;
#define OBFH_P_TRACE(s, p)                                                                                            \
    obfh_test_flow_stage(__obfh_flow_layout, s, p, __obfh_before_state, __obfh_before_tag, __obfh_flow_state,         \
                         __obfh_flow_tag, __obfh_k##s##p, __obfh_m##s##p, __obfh_a##s##p, __obfh_r##s##p,             \
                         __obfh_style##s##p);                                                                         \
    __builtin_choose_expr((s) == 0, ({                                                                                \
                              obfh_test_flow_visit();                                                                 \
                              obfh_test_flow_route(__obfh_flow_first);                                                \
                              obfh_test_transport_visit(__obfh_flow_layout, __obfh_flow_exit, __obfh_flow_hash & 1u); \
                          }),                                                                                         \
                          ((void)0))
#else
#define OBFH_P_BEFORE
#define OBFH_P_TRACE(s, p) ((void)0)
#endif
#if defined(__x86_64__)
#define OBFH_P_WORD "q"
#define OBFH_P_CX "%%rcx"
#define OBFH_P_SCALE "8"
#else
#define OBFH_P_WORD "l"
#define OBFH_P_CX "%%ecx"
#define OBFH_P_SCALE "4"
#endif
#define OBFH_P_MASK(shift, label_false, label_true)                                                                \
    ({                                                                                                             \
        ULONG_PTR __obfh_target;                                                                                   \
        ULONG_PTR __obfh_target_zero = (ULONG_PTR) && label_false ^ (ULONG_PTR)__obfh_flow_key;                    \
        ULONG_PTR __obfh_target_one = (ULONG_PTR) && label_true ^ (ULONG_PTR)__obfh_flow_key;                      \
        ULONG_PTR __obfh_cookie_mask = 0 - (ULONG_PTR)((__obfh_cookie >> (shift)) & 1u);                           \
        __obfh_asm__("mov" OBFH_P_WORD " %[zero], %[target]; xor" OBFH_P_WORD                                      \
                     " %[one], %[target]; "                                                                        \
                     "and" OBFH_P_WORD " %[flip], %[target]; mov" OBFH_P_WORD " %[target], " OBFH_P_CX             \
                     "; "                                                                                          \
                     "xor" OBFH_P_WORD " %[zero], %[target]; xor" OBFH_P_WORD " %[one], " OBFH_P_CX                \
                     "; "                                                                                          \
                     "cmpl %[tag], %%eax; cmove" OBFH_P_WORD " " OBFH_P_CX                                         \
                     ", %[target]; "                                                                               \
                     "xor" OBFH_P_WORD " %[key], %[target];"                                                       \
                     : [target] "=&r"(__obfh_target)                                                               \
                     : "a"(__obfh_flow_state),                                                                     \
                       [tag] "m"(__obfh_flow_tag), [flip] "m"(__obfh_cookie_mask), [zero] "m"(__obfh_target_zero), \
                       [one] "m"(__obfh_target_one), [key] "i"(__obfh_flow_key)                                    \
                     : "ecx", "cc", "memory");                                                                     \
        goto *(void *)__obfh_target;                                                                               \
    })
#define OBFH_P_TABLE(shift, label_false, label_true)                                            \
    ({                                                                                          \
        ULONG_PTR __obfh_targets[2] = {(ULONG_PTR) && label_false ^ (ULONG_PTR)__obfh_flow_key, \
                                       (ULONG_PTR) && label_true ^ (ULONG_PTR)__obfh_flow_key}, \
                  __obfh_target;                                                                \
        __obfh_asm__(                                                                           \
            "movl %[cookie], %%ecx; shrl %[offset], %%ecx; andl $1, %%ecx; "                    \
            "mov" OBFH_P_WORD " (%[table], " OBFH_P_CX ", " OBFH_P_SCALE                        \
            "), %[target]; xorl $1, %%ecx; "                                                    \
            "mov" OBFH_P_WORD " (%[table], " OBFH_P_CX ", " OBFH_P_SCALE "), " OBFH_P_CX        \
            "; "                                                                                \
            "cmpl %[tag], %%eax; cmove" OBFH_P_WORD " " OBFH_P_CX                               \
            ", %[target]; "                                                                     \
            "xor" OBFH_P_WORD " %[key], %[target];"                                             \
            : [target] "=&r"(__obfh_target)                                                     \
            : "a"(__obfh_flow_state),                                                           \
              [tag] "m"(__obfh_flow_tag), [cookie] "m"(__obfh_cookie), [offset] "i"(shift),     \
              [table] "r"(__obfh_targets), [key] "i"(__obfh_flow_key)                           \
            : "ecx", "cc", "memory");                                                           \
        goto *(void *)__obfh_target;                                                            \
    })
#define OBFH_P_PERMUTE(s, p, instructions)                                                                      \
    ({                                                                                                          \
        __obfh_asm__(instructions                                                                               \
                     : "+a"(__obfh_flow_state),                                                                 \
                       "+d"(__obfh_flow_tag)                                                                    \
                     : [key] "i"(__obfh_k##s##p), [mul] "i"(__obfh_m##s##p),                                    \
                       [add] "i"(__obfh_a##s##p), [rot] "i"(__obfh_r##s##p), [negadd] "i"(0u - __obfh_a##s##p), \
                       [invkey] "i"(~__obfh_k##s##p), [revrot] "i"(32u - __obfh_r##s##p)                        \
                     : "cc", "memory");                                                                         \
    })
#define OBFH_P_STEP(s, p)                                                                                                 \
    ({                                                                                                                    \
        OBFH_P_BEFORE                                                                                                     \
        __builtin_choose_expr(                                                                                            \
            __obfh_style##s##p == 0,                                                                                      \
            OBFH_P_PERMUTE(s, p,                                                                                          \
                           "xorl %[key], %%eax; imull %[mul], %%eax; addl %[add], %%eax; roll %[rot], %%eax;"             \
                           "xorl %[key], %%edx; imull %[mul], %%edx; subl %[negadd], %%edx; rorl %[revrot], %%edx;"),     \
            __builtin_choose_expr(                                                                                        \
                __obfh_style##s##p == 1,                                                                                  \
                OBFH_P_PERMUTE(s, p,                                                                                      \
                               "addl %[add], %%eax; roll %[rot], %%eax; xorl %[key], %%eax; imull %[mul], %%eax;"         \
                               "subl %[negadd], %%edx; rorl %[revrot], %%edx; notl %%edx; xorl %[invkey], %%edx; imull "  \
                               "%[mul], %%edx;"),                                                                         \
                __builtin_choose_expr(                                                                                    \
                    __obfh_style##s##p == 2,                                                                              \
                    OBFH_P_PERMUTE(s, p,                                                                                  \
                                   "imull %[mul], %%eax; xorl %[key], %%eax; rorl %[rot], %%eax; subl %[add], %%eax;"     \
                                   "imull %[mul], %%edx; notl %%edx; xorl %[invkey], %%edx; roll %[revrot], %%edx; addl " \
                                   "%[negadd], %%edx;"),                                                                  \
                    OBFH_P_PERMUTE(                                                                                       \
                        s, p,                                                                                             \
                        "roll %[rot], %%eax; xorl %[key], %%eax; imull %[mul], %%eax; subl %[add], %%eax;"                \
                        "rorl %[revrot], %%edx; xorl %[key], %%edx; imull %[mul], %%edx; addl %[negadd], %%edx;"))));     \
        OBFH_P_TRACE(s, p);                                                                                               \
    })
#define OBFH_P_FINISH_ASM(instructions)           \
    ({                                            \
        __obfh_flow_result = __obfh_flow_state;   \
        __obfh_asm__(instructions                 \
                     : "+a"(__obfh_flow_result)   \
                     : [tag] "m"(__obfh_flow_tag) \
                     : "ecx", "cc", "memory");    \
    })
#define OBFH_P_EXIT_0() OBFH_P_FINISH_ASM("subl %[tag], %%eax; negl %%eax; sbbl %%eax, %%eax; addl $1, %%eax;")
#define OBFH_P_EXIT_1() OBFH_P_FINISH_ASM("xorl %[tag], %%eax; subl $1, %%eax; sbbl %%eax, %%eax; negl %%eax;")
#define OBFH_P_EXIT_2() OBFH_P_FINISH_ASM("cmpl %[tag], %%eax; movl $0, %%eax; movl $1, %%ecx; cmovel %%ecx, %%eax;")
#define OBFH_P_EXIT_3() OBFH_P_FINISH_ASM("cmpl %[tag], %%eax; sete %%cl; movzbl %%cl, %%eax;")
#define OBFH_P_EXIT_4() \
    OBFH_P_FINISH_ASM("subl %[tag], %%eax; negl %%eax; sbbl %%eax, %%eax; notl %%eax; andl $1, %%eax;")
#define OBFH_P_EXIT_5() OBFH_P_FINISH_ASM("cmpl %[tag], %%eax; setne %%cl; movzbl %%cl, %%eax; xorl $1, %%eax;")
#define OBFH_P_EXIT_6() \
    OBFH_P_FINISH_ASM(  \
        "xorl %[tag], %%eax; movl %%eax, %%ecx; negl %%ecx; orl %%ecx, %%eax; shrl $31, %%eax; xorl $1, %%eax;")
#define OBFH_P_EXIT_7() \
    OBFH_P_FINISH_ASM("xorl %[tag], %%eax; testl %%eax, %%eax; movl $1, %%eax; jz 9f; movl $0, %%eax; 9:")
#define OBFH_P_EXIT_SELECT_6(style) __builtin_choose_expr(((style)&7u) == 6u, OBFH_P_EXIT_6(), OBFH_P_EXIT_7())
#define OBFH_P_EXIT_SELECT_5(style) \
    __builtin_choose_expr(((style)&7u) == 5u, OBFH_P_EXIT_5(), OBFH_P_EXIT_SELECT_6(style))
#define OBFH_P_EXIT_SELECT_4(style) \
    __builtin_choose_expr(((style)&7u) == 4u, OBFH_P_EXIT_4(), OBFH_P_EXIT_SELECT_5(style))
#define OBFH_P_EXIT_SELECT_3(style) \
    __builtin_choose_expr(((style)&7u) == 3u, OBFH_P_EXIT_3(), OBFH_P_EXIT_SELECT_4(style))
#define OBFH_P_EXIT_SELECT_2(style) \
    __builtin_choose_expr(((style)&7u) == 2u, OBFH_P_EXIT_2(), OBFH_P_EXIT_SELECT_3(style))
#define OBFH_P_EXIT_SELECT_1(style) \
    __builtin_choose_expr(((style)&7u) == 1u, OBFH_P_EXIT_1(), OBFH_P_EXIT_SELECT_2(style))
#define OBFH_P_EXIT_SELECT_0(style) \
    __builtin_choose_expr(((style)&7u) == 0u, OBFH_P_EXIT_0(), OBFH_P_EXIT_SELECT_1(style))
#define OBFH_P_FINISH(style) OBFH_P_EXIT_SELECT_0(style)
// Graph families: sequential diamonds, split selectors, crossed paths, split exits.
#define OBFH_P_GRAPH_0(s, t, last)        \
    ({                                    \
        __label__ a, b, c, d, join, done; \
        OBFH_P_MASK(s, a, b);             \
    a:                                    \
        OBFH_P_STEP(s, 0);                \
        goto join;                        \
    b:                                    \
        OBFH_P_STEP(s, 1);                \
    join:                                 \
        OBFH_P_MASK(t, c, d);             \
    c:                                    \
        OBFH_P_STEP(t, 0);                \
        goto done;                        \
    d:                                    \
        OBFH_P_STEP(t, 1);                \
    done:                                 \
        __obfh_flow_state;                \
    })
#define OBFH_P_GRAPH_1(s, t, last)        \
    ({                                    \
        __label__ a, b, c, d, join, done; \
        OBFH_P_TABLE(s, a, b);            \
    a:                                    \
        OBFH_P_STEP(s, 0);                \
        OBFH_P_TABLE(t, c, d);            \
    b:                                    \
        OBFH_P_STEP(s, 1);                \
        OBFH_P_TABLE(t, c, d);            \
    c:                                    \
        OBFH_P_STEP(t, 0);                \
        goto done;                        \
    d:                                    \
        OBFH_P_STEP(t, 1);                \
    done:                                 \
        __obfh_flow_state;                \
    })
#define OBFH_P_GRAPH_2(s, t, last)        \
    ({                                    \
        __label__ a, b, c, d, join, done; \
        OBFH_P_MASK(s, a, b);             \
    b:                                    \
        OBFH_P_STEP(s, 1);                \
        OBFH_P_MASK(t, c, d);             \
    c:                                    \
        OBFH_P_STEP(t, 0);                \
        goto done;                        \
    a:                                    \
        OBFH_P_STEP(s, 0);                \
        OBFH_P_MASK(t, c, d);             \
    d:                                    \
        OBFH_P_STEP(t, 1);                \
    done:                                 \
        __obfh_flow_state;                \
    })
#define OBFH_P_GRAPH_3(s, t, last)                                                           \
    ({                                                                                       \
        __label__ a, b, c, d, join, done;                                                    \
        OBFH_P_TABLE(s, a, b);                                                               \
    a:                                                                                       \
        OBFH_P_STEP(s, 0);                                                                   \
        OBFH_P_TABLE(t, c, d);                                                               \
    d:                                                                                       \
        OBFH_P_STEP(t, 1);                                                                   \
        __builtin_choose_expr(last, ({ OBFH_P_FINISH(__obfh_flow_exit ^ 3u); }), ((void)0)); \
        goto done;                                                                           \
    b:                                                                                       \
        OBFH_P_STEP(s, 1);                                                                   \
        OBFH_P_TABLE(t, c, d);                                                               \
    c:                                                                                       \
        OBFH_P_STEP(t, 0);                                                                   \
        __builtin_choose_expr(last, ({ OBFH_P_FINISH(__obfh_flow_exit); }), ((void)0));      \
    done:                                                                                    \
        __obfh_flow_state;                                                                   \
    })
#define OBFH_P_GRAPH(f, s, t, last)                                 \
    __builtin_choose_expr(                                          \
        (f) == 0, OBFH_P_GRAPH_0(s, t, last),                       \
        __builtin_choose_expr((f) == 1, OBFH_P_GRAPH_1(s, t, last), \
                              __builtin_choose_expr((f) == 2, OBFH_P_GRAPH_2(s, t, last), OBFH_P_GRAPH_3(s, t, last))))
// Capture the condition once; all later stages use only local encoded state.
#define OBFH_FLOW_CONDITION(condition, site_value, ...)                                                               \
    ({                                                                                                                \
        enum {                                                                                                        \
            __obfh_flow_site = (site_value),                                                                          \
            __obfh_flow_hash =                                                                                        \
                OBFH_MIX_B(OBFH_MIX_A((unsigned int)__obfh_flow_site ^ (unsigned int)OBFH_BUILD_SEED ^ 0x43464C57u)), \
            __obfh_flow_layout = __obfh_flow_hash % (CFLOW_V2 ? 8u : 4u),                                             \
            __obfh_flow_key = OBFH_JUNK_WORD & 0x7fffffffu,                                                           \
            __obfh_flow_exit = (__obfh_flow_hash >> 16) & 7u,                                                         \
            __obfh_false_tag = OBFH_MIX_A(__obfh_flow_hash ^ 0x15729f81u),                                            \
            __obfh_true_tag = __obfh_false_tag ^ (OBFH_MIX_B(__obfh_flow_hash ^ 0x74ba953du) | 1u),                   \
            __obfh_flow_first = CFLOW_V2 ? (__obfh_flow_layout == 0 || __obfh_flow_layout == 2   ? 0                  \
                                            : __obfh_flow_layout == 1 || __obfh_flow_layout == 4 ? 1                  \
                                            : __obfh_flow_layout == 5 || __obfh_flow_layout == 6 ? 2                  \
                                                                                                 : 3)                 \
                                         : __obfh_flow_layout,                                                        \
            __obfh_flow_second = __obfh_flow_layout == 0 || __obfh_flow_layout == 5   ? 1                             \
                                 : __obfh_flow_layout == 1 || __obfh_flow_layout == 3 ? 0                             \
                                 : __obfh_flow_layout == 2 || __obfh_flow_layout == 6 ? 3                             \
                                                                                      : 2,                            \
            __obfh_k00 = OBFH_MIX_A(__obfh_flow_hash ^ 2654435769u),                                                  \
            __obfh_m00 = ((__obfh_k00 >> 17) & 65535u) | 1u,                                                          \
            __obfh_a00 = OBFH_MIX_B(__obfh_k00 ^ 0x85ebca6bu),                                                        \
            __obfh_r00 = ((__obfh_k00 >> 27) % 31u) + 1u,                                                             \
            __obfh_style00 = (__obfh_k00 >> 11) & 3u,                                                                 \
            __obfh_k01 = OBFH_MIX_A(__obfh_flow_hash ^ 3532493122u),                                                  \
            __obfh_m01 = ((__obfh_k01 >> 17) & 65535u) | 1u,                                                          \
            __obfh_a01 = OBFH_MIX_B(__obfh_k01 ^ 0x85ebca6bu),                                                        \
            __obfh_r01 = ((__obfh_k01 >> 27) % 31u) + 1u,                                                             \
            __obfh_style01 = (__obfh_k01 >> 11) & 3u,                                                                 \
            __obfh_k10 = OBFH_MIX_A(__obfh_flow_hash ^ 2671344829u),                                                  \
            __obfh_m10 = ((__obfh_k10 >> 17) & 65535u) | 1u,                                                          \
            __obfh_a10 = OBFH_MIX_B(__obfh_k10 ^ 0x85ebca6bu),                                                        \
            __obfh_r10 = ((__obfh_k10 >> 27) % 31u) + 1u,                                                             \
            __obfh_style10 = (__obfh_k10 >> 11) & 3u,                                                                 \
            __obfh_k11 = OBFH_MIX_A(__obfh_flow_hash ^ 3549402182u),                                                  \
            __obfh_m11 = ((__obfh_k11 >> 17) & 65535u) | 1u,                                                          \
            __obfh_a11 = OBFH_MIX_B(__obfh_k11 ^ 0x85ebca6bu),                                                        \
            __obfh_r11 = ((__obfh_k11 >> 27) % 31u) + 1u,                                                             \
            __obfh_style11 = (__obfh_k11 >> 11) & 3u,                                                                 \
            __obfh_k20 = OBFH_MIX_A(__obfh_flow_hash ^ 2688253889u),                                                  \
            __obfh_m20 = ((__obfh_k20 >> 17) & 65535u) | 1u,                                                          \
            __obfh_a20 = OBFH_MIX_B(__obfh_k20 ^ 0x85ebca6bu),                                                        \
            __obfh_r20 = ((__obfh_k20 >> 27) % 31u) + 1u,                                                             \
            __obfh_style20 = (__obfh_k20 >> 11) & 3u,                                                                 \
            __obfh_k21 = OBFH_MIX_A(__obfh_flow_hash ^ 3566311242u),                                                  \
            __obfh_m21 = ((__obfh_k21 >> 17) & 65535u) | 1u,                                                          \
            __obfh_a21 = OBFH_MIX_B(__obfh_k21 ^ 0x85ebca6bu),                                                        \
            __obfh_r21 = ((__obfh_k21 >> 27) % 31u) + 1u,                                                             \
            __obfh_style21 = (__obfh_k21 >> 11) & 3u,                                                                 \
            __obfh_k30 = OBFH_MIX_A(__obfh_flow_hash ^ 2705162949u),                                                  \
            __obfh_m30 = ((__obfh_k30 >> 17) & 65535u) | 1u,                                                          \
            __obfh_a30 = OBFH_MIX_B(__obfh_k30 ^ 0x85ebca6bu),                                                        \
            __obfh_r30 = ((__obfh_k30 >> 27) % 31u) + 1u,                                                             \
            __obfh_style30 = (__obfh_k30 >> 11) & 3u,                                                                 \
            __obfh_k31 = OBFH_MIX_A(__obfh_flow_hash ^ 3583220302u),                                                  \
            __obfh_m31 = ((__obfh_k31 >> 17) & 65535u) | 1u,                                                          \
            __obfh_a31 = OBFH_MIX_B(__obfh_k31 ^ 0x85ebca6bu),                                                        \
            __obfh_r31 = ((__obfh_k31 >> 27) % 31u) + 1u,                                                             \
            __obfh_style31 = (__obfh_k31 >> 11) & 3u                                                                  \
        };                                                                                                            \
        /* Random polarity permutes tags, never negates the user expression. */                                       \
        unsigned int __obfh_flow_state = (condition)                                                                  \
                                             ? (__obfh_flow_hash & 1u ? __obfh_true_tag : __obfh_false_tag)           \
                                             : (__obfh_flow_hash & 1u ? __obfh_false_tag : __obfh_true_tag);          \
        __builtin_choose_expr(__obfh_flow_hash & 1u, (void)0,                                                         \
                              (void)(__obfh_flow_state ^= __obfh_false_tag ^ __obfh_true_tag));                       \
        unsigned int __obfh_flow_tag = __obfh_true_tag, __obfh_flow_result;                                           \
        ULONG_PTR __obfh_cookie = ((ULONG_PTR)&__obfh_flow_state >> 4) ^ (ULONG_PTR)__obfh_flow_hash;                 \
        __obfh_asm__(                                                                                                 \
            "jmp 7f; .byte %c[byte], %c[extra]; .long %c[payload]; .fill %c[gap],1,%c[byte]; 7:"                      \
            : "+a"(                                                                                                   \
                __obfh_flow_state)                                                                                    \
            : [byte] "i"(OBFH_FLOW_OPCODE(OBFH_JUNK_BYTE & 7u)),                                                      \
              [extra] "i"(OBFH_JUNK_BYTE), [payload] "i"(OBFH_JUNK_WORD), [gap] "i"(OBFH_JUNK_BYTE & 7u)              \
            : "memory");                                                                                              \
        __VA_ARGS__;                                                                                                  \
        OBFH_P_GRAPH(__obfh_flow_first, 0, 1, !CFLOW_V2);                                                             \
        __builtin_choose_expr(CFLOW_V2, ({ OBFH_P_GRAPH(__obfh_flow_second, 2, 3, 1); }), ((void)0));                 \
        __builtin_choose_expr(CFLOW_V2 ? __obfh_flow_second != 3 : __obfh_flow_first != 3,                            \
                              ({ OBFH_P_FINISH(__obfh_flow_exit); }), ((void)0));                                     \
        __obfh_flow_result;                                                                                           \
    })

#endif /* CFLOW helpers; kernel precedes keyword interception. */
#if VIRT == 1
typedef enum { SALT_NUM1 = RND(16, 48),
               SALT_NUM2 = RND(16, 48) } VM_SALT;
#endif
// ============================================================================
// 11. VM kernel: registers, flags, decoding and instruction execution
// ============================================================================
#if VIRT == 1
typedef char OBFH_V_WORD_32[(sizeof(unsigned int) == 4 && CHAR_BIT == 8) ? 1 : -1];
enum {
    OBFH_V_LOAD_A = 1,
    OBFH_V_LOAD_B,
    OBFH_V_MOVE,
    OBFH_V_SWAP,
    OBFH_V_ADD,
    OBFH_V_SUB,
    OBFH_V_MUL,
    OBFH_V_DIV,
    OBFH_V_MOD,
    OBFH_V_COMPARE,
    OBFH_V_BOOLEAN,
    OBFH_V_TEST,
    OBFH_V_CADD,
    OBFH_V_CXOR,
    OBFH_V_CROL,
    OBFH_V_JUMP,
    OBFH_V_JFLAG,
    OBFH_V_RETURN
};
enum {
    OBFH_V_EQ = 1,
    OBFH_V_LT = 2,
    OBFH_V_GT = 4,
    OBFH_V_UN = 8,
    OBFH_V_TRUE = 16,
    OBFH_V_CONTROL = 32
};
enum {
    OBFH_VOP_ADD,
    OBFH_VOP_SUB,
    OBFH_VOP_MUL,
    OBFH_VOP_DIV,
    OBFH_VOP_MOD,
    OBFH_VOP_EQ,
    OBFH_VOP_NE,
    OBFH_VOP_LT,
    OBFH_VOP_GT,
    OBFH_VOP_LE,
    OBFH_VOP_GE,
    OBFH_VOP_ID,
    OBFH_VOP_TRUTH
};
typedef struct
{
    unsigned char bytes[sizeof(long double)];
    unsigned int mask;
} OBFH_V_REGISTER;
typedef struct
{
    OBFH_V_REGISTER registers[4];
    unsigned int controls[4];
    unsigned int flags, flag_key, pc, steps, initialized, key, status, flags_ready;
    long double result;
} OBFH_V_CONTEXT;

static unsigned int obfh_v_mix(unsigned int x) {
    x ^= x >> 16;
    x *= 2246822507u;
    PHANTOM_NOP;
    x ^= x >> 13;
    return x;
}
/* Encodings are local to an execution; only decoded uint32 values enter ALU operations. */
static unsigned int obfh_v_control_read(const OBFH_V_CONTEXT *c, unsigned int r) {
    PHANTOM_NOP;
    return c->controls[r] ^ obfh_v_mix(c->key ^ (r * 3266489917u));
}
static void obfh_v_control_write(OBFH_V_CONTEXT *c, unsigned int r, unsigned int value) {
    c->controls[r] = value ^ obfh_v_mix(c->key ^ (r * 3266489917u));
    PHANTOM_NOP;
}
static unsigned int obfh_v_flags(const OBFH_V_CONTEXT *c) {
    PHANTOM_NOP;
    return c->flags ^ c->flag_key;
}
static void obfh_v_set_flags(OBFH_V_CONTEXT *c, unsigned int flags) {
    c->flags = flags ^ c->flag_key;
    PHANTOM_NOP;
}
static void obfh_v_write(OBFH_V_CONTEXT *c, unsigned int r, long double v) {
    unsigned int m = obfh_v_mix(c->key ^ (r * 3266489917u) ^ c->steps);
    const unsigned char *p = (const unsigned char *)&v;
    c->registers[r].mask = m;
    PHANTOM_NOP;
    for (unsigned int i = 0; i < sizeof(v); ++i) {
        m = obfh_v_mix(m + i + 1u);
        c->registers[r].bytes[i] = p[i] ^ (unsigned char)m;
    }
    c->initialized |= 1u << r;
}
static long double obfh_v_read(const OBFH_V_CONTEXT *c, unsigned int r) {
    long double v;
    unsigned char *p = (unsigned char *)&v;
    unsigned int m = c->registers[r].mask;
    PHANTOM_NOP;
    for (unsigned int i = 0; i < sizeof(v); ++i) {
        m = obfh_v_mix(m + i + 1u);
        p[i] = c->registers[r].bytes[i] ^ (unsigned char)m;
    }
    return v;
}
static void obfh_v_copy(OBFH_V_CONTEXT *c, unsigned int d, unsigned int a) {
    /* Encoded copy preserves numeric object bytes without an FP operation. */
    c->registers[d] = c->registers[a];
    PHANTOM_NOP;
    c->initialized |= 1u << d;
}
static void obfh_v_failure(OBFH_V_CONTEXT *c, unsigned int status) {
    c->status = status;
    PHANTOM_NOP;
    ExitProcess(0xE0BF0000u | status);
}
static unsigned int obfh_v_decode(unsigned int word, unsigned int key, unsigned int pc) {
    unsigned int rotate = (key % 31u) + 1u;
    word ^= key + pc * 0x9e3779b9u;
    PHANTOM_NOP;
    return (word << rotate) | (word >> (32u - rotate));
}
static unsigned int obfh_v_opcode(unsigned int word, unsigned int key) {
    unsigned int multiplier = ((key >> 8) & 255u) | 1u, inverse = 1u;
    inverse *= 2u - multiplier * inverse;
    PHANTOM_NOP;
    inverse *= 2u - multiplier * inverse;
    inverse *= 2u - multiplier * inverse;
    return (((word & 255u) - (key & 255u)) * inverse) & 255u;
}

static long double Obfh_VirtualMachine(const unsigned int *program, unsigned int length, unsigned int key, OBFH_VM_VALUE input_a,
                                       OBFH_VM_VALUE input_b) OBFH_CODE_SECTION_ATTRIBUTE {
#if NO_CFLOW != 1
    enum {
        __obfh_vm_dispatch_site = RND(1, 65535),
        __obfh_vm_finish_site = RND(1, 65535)
    };
#endif
    STACK_PROXY_FUNCTIONS;
    OBFH_V_CONTEXT c;
    c.pc = 0;
    c.steps = 0;
    c.initialized = 0;
    c.key = key;
    c.status = 0;
    c.flags_ready = 0;
    c.flag_key = obfh_v_mix(key ^ 0x85ebca6bu);
    STACK_PROXY_FUNCTIONS;
    obfh_v_set_flags(&c, 0);
    for (unsigned int i = 0; i < 4; ++i)
        obfh_v_control_write(&c, i, 0);
    if (!program || !length || length > 32)
        obfh_v_failure(&c, 1);
    long double operands[2] = {obfh_vm_decode(input_a, SALT_NUM1), obfh_vm_decode(input_b, SALT_NUM2)};
    STACK_PROXY_FUNCTIONS;
    PHANTOM_NOP;
    /* OBFH_VM_TRACE_ENTER */
    while (c.steps < 128) {
        if (c.pc >= length)
            obfh_v_failure(&c, 2);
        unsigned int at = c.pc++, word = obfh_v_decode(program[at], key, at);
        unsigned int op = obfh_v_opcode(word, key), d = (word >> 8) & 3u, a = (word >> 10) & 3u, b = (word >> 12) & 3u, imm = word >> 14;
        ++c.steps;
        if (op < OBFH_V_LOAD_A || op > OBFH_V_RETURN)
            obfh_v_failure(&c, 3);
        if (op == OBFH_V_MOVE || op == OBFH_V_SWAP || (op >= OBFH_V_ADD && op <= OBFH_V_COMPARE) || op == OBFH_V_TEST ||
            op == OBFH_V_RETURN)
            if (!(c.initialized & (1u << a)))
                obfh_v_failure(&c, 4);
        if (op == OBFH_V_SWAP || (op >= OBFH_V_ADD && op <= OBFH_V_COMPARE))
            if (!(c.initialized & (1u << b)))
                obfh_v_failure(&c, 4);
        if ((op == OBFH_V_JUMP || op == OBFH_V_JFLAG) && imm >= length)
            obfh_v_failure(&c, 5);
            /* OBFH_VM_TRACE_STEP */
#if NO_CFLOW != 1
        if (!OBFH_FLOW_CONDITION(op != 0u, __obfh_vm_dispatch_site))
            obfh_v_failure(&c, 3);
#endif
        /* Both dispatchers had the same audit coverage; keep the simpler switch. */
        switch (op) {
            case OBFH_V_LOAD_A:
                STACK_PROXY_FUNCTIONS;
                goto load_a;
            case OBFH_V_LOAD_B:
                STACK_PROXY_FUNCTIONS;
                goto load_b;
            case OBFH_V_MOVE:
                STACK_PROXY_FUNCTIONS;
                goto move;
            case OBFH_V_SWAP:
                STACK_PROXY_FUNCTIONS;
                goto swap;
            case OBFH_V_ADD:
                STACK_PROXY_FUNCTIONS;
                goto add;
            case OBFH_V_SUB:
                STACK_PROXY_FUNCTIONS;
                goto sub;
            case OBFH_V_MUL:
                STACK_PROXY_FUNCTIONS;
                goto mul;
            case OBFH_V_DIV:
                STACK_PROXY_FUNCTIONS;
                goto divide;
            case OBFH_V_MOD:
                STACK_PROXY_FUNCTIONS;
                goto modulo;
            case OBFH_V_COMPARE:
                STACK_PROXY_FUNCTIONS;
                goto compare;
            case OBFH_V_BOOLEAN:
                STACK_PROXY_FUNCTIONS;
                goto boolean;
            case OBFH_V_TEST:
                STACK_PROXY_FUNCTIONS;
                goto test;
            case OBFH_V_CADD:
                STACK_PROXY_FUNCTIONS;
                goto cadd;
            case OBFH_V_CXOR:
                STACK_PROXY_FUNCTIONS;
                goto cxor;
            case OBFH_V_CROL:
                STACK_PROXY_FUNCTIONS;
                goto crol;
            case OBFH_V_JUMP:
                STACK_PROXY_FUNCTIONS;
                goto jump;
            case OBFH_V_JFLAG:
                STACK_PROXY_FUNCTIONS;
                goto jflag;
            case OBFH_V_RETURN:
                STACK_PROXY_FUNCTIONS;
                goto finish;
        }
    invalid:
        STACK_PROXY_FUNCTIONS;
        obfh_v_failure(&c, 3);
        continue;
    load_a:
        STACK_PROXY_FUNCTIONS;
        obfh_v_write(&c, d, operands[0]);
        continue;
    load_b:
        STACK_PROXY_FUNCTIONS;
        obfh_v_write(&c, d, operands[1]);
        continue;
    move:
        STACK_PROXY_FUNCTIONS;
        obfh_v_copy(&c, d, a);
        continue;
    swap : {
        STACK_PROXY_FUNCTIONS;
        OBFH_V_REGISTER temporary = c.registers[a];
        c.registers[a] = c.registers[b];
        c.registers[b] = temporary;
        continue;
    }
    add:
        STACK_PROXY_FUNCTIONS;
        obfh_v_write(&c, d, obfh_v_read(&c, a) + obfh_v_read(&c, b));
        continue;
    sub:
        STACK_PROXY_FUNCTIONS;
        obfh_v_write(&c, d, obfh_v_read(&c, a) - obfh_v_read(&c, b));
        continue;
    mul:
        STACK_PROXY_FUNCTIONS;
        obfh_v_write(&c, d, obfh_v_read(&c, a) * obfh_v_read(&c, b));
        continue;
    divide : {
        STACK_PROXY_FUNCTIONS;
        long double x = obfh_v_read(&c, a), y = obfh_v_read(&c, b);
        obfh_v_write(&c, d, input_a.floating || y != 0 ? x / y : 0);
        continue;
    }
    modulo : {
        STACK_PROXY_FUNCTIONS;
        int x = (int)obfh_v_read(&c, a), y = (int)obfh_v_read(&c, b);
        obfh_v_write(&c, d, y != 0 && !(x == INT_MIN && y == -1) ? x % y : 0);
        continue;
    }
    compare : {
        STACK_PROXY_FUNCTIONS;
        long double x = obfh_v_read(&c, a), y = obfh_v_read(&c, b);
        obfh_v_set_flags(&c, x != x || y != y ? OBFH_V_UN : (x == y ? OBFH_V_EQ : (x < y ? OBFH_V_LT : OBFH_V_GT)));
        c.flags_ready = 1;
        continue;
    }
    boolean : {
        STACK_PROXY_FUNCTIONS;
        unsigned int f = obfh_v_flags(&c);
        long double value;
        if (imm > 6 || !c.flags_ready)
            obfh_v_failure(&c, 6);
        switch (imm) {
            case 0:
                STACK_PROXY_FUNCTIONS;
                value = !!(f & OBFH_V_EQ);
                break;
            case 1:
                STACK_PROXY_FUNCTIONS;
                value = !(f & OBFH_V_EQ);
                break;
            case 2:
                STACK_PROXY_FUNCTIONS;
                value = !!(f & OBFH_V_LT);
                break;
            case 3:
                STACK_PROXY_FUNCTIONS;
                value = !!(f & OBFH_V_GT);
                break;
            case 4:
                STACK_PROXY_FUNCTIONS;
                value = !!(f & (OBFH_V_LT | OBFH_V_EQ));
                break;
            case 5:
                STACK_PROXY_FUNCTIONS;
                value = !!(f & (OBFH_V_GT | OBFH_V_EQ));
                break;
            default:
                STACK_PROXY_FUNCTIONS;
                value = !!(f & OBFH_V_TRUE);
                break;
        }
        obfh_v_write(&c, d, value);
        continue;
    }
    test:
        STACK_PROXY_FUNCTIONS;
        obfh_v_set_flags(&c, (obfh_v_flags(&c) & OBFH_V_CONTROL) | (obfh_v_read(&c, a) != 0 ? OBFH_V_TRUE : 0));
        c.flags_ready = 1;
        continue;
    cadd : {
        STACK_PROXY_FUNCTIONS;
        unsigned int signed_imm = (imm & 0x20000u) ? imm | 0xfffc0000u : imm;
        unsigned int value = obfh_v_control_read(&c, d) + signed_imm;
        obfh_v_control_write(&c, d, value);
        obfh_v_set_flags(&c, (obfh_v_flags(&c) & ~OBFH_V_CONTROL) | (value ? OBFH_V_CONTROL : 0));
        c.flags_ready = 1;
        continue;
    }
    cxor:
        STACK_PROXY_FUNCTIONS;
        c.controls[d] ^= imm;
        continue;
    crol : {
        STACK_PROXY_FUNCTIONS;
        unsigned int n = (imm % 31u) + 1u, value = obfh_v_control_read(&c, d);
        obfh_v_control_write(&c, d, (value << n) | (value >> (32u - n)));
        continue;
    }
    jump:
        STACK_PROXY_FUNCTIONS;
        c.pc = imm;
        continue;
    jflag : {
        STACK_PROXY_FUNCTIONS;
        unsigned int flag = d | (a << 2);
        if (flag > 5 || !c.flags_ready)
            obfh_v_failure(&c, 6);
        if (obfh_v_flags(&c) & (1u << flag))
            c.pc = imm;
        continue;
    }
    finish:
#if NO_CFLOW != 1
        if (!OBFH_FLOW_CONDITION(c.initialized != 0, __obfh_vm_finish_site))
            obfh_v_failure(&c, 4);
#endif
        c.result = obfh_v_read(&c, a);
        return c.result;
    }
    obfh_v_failure(&c, 7);
    return 0;
}
#endif

// ============================================================================
// 12. Inline string and memory kernels
// ============================================================================

// Local branches contain no protected keywords and survive nested macro rescans.
// Fall through on false; only the taken exit needs a local jump.
#define OBFH_INLINE_EXIT(condition, label) ((condition) ? ({ goto label; }) : (void)0)

// Inline kernels retain entry protection and native internal control flow.
#define memchr_custom(...) ({                                                                                                       \
    __label__ __obfh_memchr_loop, __obfh_memchr_found, __obfh_memchr_done;                                                          \
    struct {                                                                                                                        \
        const void *__obfh_memchr_memory;                                                                                           \
        int __obfh_memchr_value;                                                                                                    \
        size_t __obfh_memchr_count;                                                                                                 \
    } __obfh_memchr_args = {__VA_ARGS__};                                                                                           \
    unsigned char __obfh_memchr_wanted = (unsigned char)__obfh_memchr_args.__obfh_memchr_value;                                     \
    void *__obfh_memchr_result = NULL;                                                                                              \
    BREAK_STACK_CFLOW;                                                                                                              \
    PHANTOM_NOP;                                                                                                                    \
__obfh_memchr_loop:                                                                                                                 \
    OBFH_INLINE_EXIT(!__obfh_memchr_args.__obfh_memchr_count, __obfh_memchr_done);                                                  \
    OBFH_INLINE_EXIT(*(const unsigned char *)__obfh_memchr_args.__obfh_memchr_memory == __obfh_memchr_wanted, __obfh_memchr_found); \
    __obfh_memchr_args.__obfh_memchr_memory = (const unsigned char *)__obfh_memchr_args.__obfh_memchr_memory + 1;                   \
    --__obfh_memchr_args.__obfh_memchr_count;                                                                                       \
    goto __obfh_memchr_loop;                                                                                                        \
__obfh_memchr_found:                                                                                                                \
    __obfh_memchr_result = (void *)__obfh_memchr_args.__obfh_memchr_memory;                                                         \
__obfh_memchr_done:;                                                                                                                \
    __obfh_memchr_result;                                                                                                           \
})
#define memchr(...) memchr_custom(__VA_ARGS__)

#define strspn_custom(...) ({                                                                                         \
    __label__ __obfh_strspn_loop, __obfh_strspn_match, __obfh_strspn_select, __obfh_strspn_done;                      \
    struct {                                                                                                          \
        const char *__obfh_strspn_text;                                                                               \
        const char *__obfh_strspn_set;                                                                                \
    } __obfh_strspn_args = {__VA_ARGS__};                                                                             \
    const unsigned char *__obfh_strspn_cursor = (const unsigned char *)__obfh_strspn_args.__obfh_strspn_text;         \
    const unsigned char *__obfh_strspn_member;                                                                        \
    BREAK_STACK_CFLOW;                                                                                                \
    PHANTOM_NOP;                                                                                                      \
__obfh_strspn_loop:                                                                                                   \
    OBFH_INLINE_EXIT(!*__obfh_strspn_cursor, __obfh_strspn_done);                                                     \
    __obfh_strspn_member = (const unsigned char *)__obfh_strspn_args.__obfh_strspn_set;                               \
__obfh_strspn_match:                                                                                                  \
    OBFH_INLINE_EXIT(!*__obfh_strspn_member || *__obfh_strspn_member == *__obfh_strspn_cursor, __obfh_strspn_select); \
    ++__obfh_strspn_member;                                                                                           \
    goto __obfh_strspn_match;                                                                                         \
__obfh_strspn_select:                                                                                                 \
    OBFH_INLINE_EXIT(!*__obfh_strspn_member, __obfh_strspn_done);                                                     \
    ++__obfh_strspn_cursor;                                                                                           \
    goto __obfh_strspn_loop;                                                                                          \
__obfh_strspn_done:;                                                                                                  \
    (size_t)(__obfh_strspn_cursor - (const unsigned char *)__obfh_strspn_args.__obfh_strspn_text);                    \
})
#define strspn(...) strspn_custom(__VA_ARGS__)

#define strcspn_custom(...) ({                                                                                            \
    __label__ __obfh_strcspn_loop, __obfh_strcspn_match, __obfh_strcspn_select, __obfh_strcspn_done;                      \
    struct {                                                                                                              \
        const char *__obfh_strcspn_text;                                                                                  \
        const char *__obfh_strcspn_set;                                                                                   \
    } __obfh_strcspn_args = {__VA_ARGS__};                                                                                \
    const unsigned char *__obfh_strcspn_cursor = (const unsigned char *)__obfh_strcspn_args.__obfh_strcspn_text;          \
    const unsigned char *__obfh_strcspn_member;                                                                           \
    BREAK_STACK_CFLOW;                                                                                                    \
    PHANTOM_NOP;                                                                                                          \
__obfh_strcspn_loop:                                                                                                      \
    OBFH_INLINE_EXIT(!*__obfh_strcspn_cursor, __obfh_strcspn_done);                                                       \
    __obfh_strcspn_member = (const unsigned char *)__obfh_strcspn_args.__obfh_strcspn_set;                                \
__obfh_strcspn_match:                                                                                                     \
    OBFH_INLINE_EXIT(!*__obfh_strcspn_member || *__obfh_strcspn_member == *__obfh_strcspn_cursor, __obfh_strcspn_select); \
    ++__obfh_strcspn_member;                                                                                              \
    goto __obfh_strcspn_match;                                                                                            \
__obfh_strcspn_select:                                                                                                    \
    OBFH_INLINE_EXIT(*__obfh_strcspn_member, __obfh_strcspn_done);                                                        \
    ++__obfh_strcspn_cursor;                                                                                              \
    goto __obfh_strcspn_loop;                                                                                             \
__obfh_strcspn_done:;                                                                                                     \
    (size_t)(__obfh_strcspn_cursor - (const unsigned char *)__obfh_strcspn_args.__obfh_strcspn_text);                     \
})
#define strcspn(...) strcspn_custom(__VA_ARGS__)

#define strpbrk_custom(...) ({                                                                                            \
    __label__ __obfh_strpbrk_loop, __obfh_strpbrk_match, __obfh_strpbrk_select, __obfh_strpbrk_done;                      \
    struct {                                                                                                              \
        const char *__obfh_strpbrk_text;                                                                                  \
        const char *__obfh_strpbrk_set;                                                                                   \
    } __obfh_strpbrk_args = {__VA_ARGS__};                                                                                \
    const unsigned char *__obfh_strpbrk_cursor = (const unsigned char *)__obfh_strpbrk_args.__obfh_strpbrk_text;          \
    const unsigned char *__obfh_strpbrk_member;                                                                           \
    BREAK_STACK_CFLOW;                                                                                                    \
    PHANTOM_NOP;                                                                                                          \
__obfh_strpbrk_loop:                                                                                                      \
    OBFH_INLINE_EXIT(!*__obfh_strpbrk_cursor, __obfh_strpbrk_done);                                                       \
    __obfh_strpbrk_member = (const unsigned char *)__obfh_strpbrk_args.__obfh_strpbrk_set;                                \
__obfh_strpbrk_match:                                                                                                     \
    OBFH_INLINE_EXIT(!*__obfh_strpbrk_member || *__obfh_strpbrk_member == *__obfh_strpbrk_cursor, __obfh_strpbrk_select); \
    ++__obfh_strpbrk_member;                                                                                              \
    goto __obfh_strpbrk_match;                                                                                            \
__obfh_strpbrk_select:                                                                                                    \
    OBFH_INLINE_EXIT(*__obfh_strpbrk_member, __obfh_strpbrk_done);                                                        \
    ++__obfh_strpbrk_cursor;                                                                                              \
    goto __obfh_strpbrk_loop;                                                                                             \
__obfh_strpbrk_done:;                                                                                                     \
    (char *)(*__obfh_strpbrk_cursor ? (char *)__obfh_strpbrk_cursor : NULL);                                              \
})
#define strpbrk(...) strpbrk_custom(__VA_ARGS__)

#define memcmp_custom(...) ({                                                                                                                                        \
    __label__ __obfh_memcmp_loop, __obfh_memcmp_done;                                                                                                                \
    struct {                                                                                                                                                         \
        const void *__obfh_memcmp_left;                                                                                                                              \
        const void *__obfh_memcmp_right;                                                                                                                             \
        size_t __obfh_memcmp_count;                                                                                                                                  \
    } __obfh_memcmp_args = {__VA_ARGS__};                                                                                                                            \
    int __obfh_memcmp_result = 0;                                                                                                                                    \
    BREAK_STACK_CFLOW;                                                                                                                                               \
    PHANTOM_NOP;                                                                                                                                                     \
__obfh_memcmp_loop:                                                                                                                                                  \
    OBFH_INLINE_EXIT(!__obfh_memcmp_args.__obfh_memcmp_count, __obfh_memcmp_done);                                                                                   \
    __obfh_memcmp_result = (int)*(const unsigned char *)__obfh_memcmp_args.__obfh_memcmp_left - (int)*(const unsigned char *)__obfh_memcmp_args.__obfh_memcmp_right; \
    OBFH_INLINE_EXIT(__obfh_memcmp_result, __obfh_memcmp_done);                                                                                                      \
    __obfh_memcmp_args.__obfh_memcmp_left = (const unsigned char *)__obfh_memcmp_args.__obfh_memcmp_left + 1;                                                        \
    __obfh_memcmp_args.__obfh_memcmp_right = (const unsigned char *)__obfh_memcmp_args.__obfh_memcmp_right + 1;                                                      \
    --__obfh_memcmp_args.__obfh_memcmp_count;                                                                                                                        \
    goto __obfh_memcmp_loop;                                                                                                                                         \
__obfh_memcmp_done:;                                                                                                                                                 \
    __obfh_memcmp_result;                                                                                                                                            \
})
#define memcmp(...) memcmp_custom(__VA_ARGS__)

#define strncmp_custom(...) ({                                                                                                                                            \
    __label__ __obfh_strncmp_loop, __obfh_strncmp_done;                                                                                                                   \
    struct {                                                                                                                                                              \
        const char *__obfh_strncmp_left;                                                                                                                                  \
        const char *__obfh_strncmp_right;                                                                                                                                 \
        size_t __obfh_strncmp_count;                                                                                                                                      \
    } __obfh_strncmp_args = {__VA_ARGS__};                                                                                                                                \
    int __obfh_strncmp_result = 0;                                                                                                                                        \
    BREAK_STACK_CFLOW;                                                                                                                                                    \
    PHANTOM_NOP;                                                                                                                                                          \
__obfh_strncmp_loop:                                                                                                                                                      \
    OBFH_INLINE_EXIT(!__obfh_strncmp_args.__obfh_strncmp_count, __obfh_strncmp_done);                                                                                     \
    __obfh_strncmp_result = (int)*(const unsigned char *)__obfh_strncmp_args.__obfh_strncmp_left - (int)*(const unsigned char *)__obfh_strncmp_args.__obfh_strncmp_right; \
    OBFH_INLINE_EXIT(__obfh_strncmp_result || !*(const unsigned char *)__obfh_strncmp_args.__obfh_strncmp_left, __obfh_strncmp_done);                                     \
    ++__obfh_strncmp_args.__obfh_strncmp_left;                                                                                                                            \
    ++__obfh_strncmp_args.__obfh_strncmp_right;                                                                                                                           \
    --__obfh_strncmp_args.__obfh_strncmp_count;                                                                                                                           \
    goto __obfh_strncmp_loop;                                                                                                                                             \
__obfh_strncmp_done:;                                                                                                                                                     \
    __obfh_strncmp_result;                                                                                                                                                \
})
#define strncmp(...) strncmp_custom(__VA_ARGS__)

#define strstr_custom(...) ({                                                                                              \
    __label__ __obfh_strstr_loop, __obfh_strstr_match, __obfh_strstr_advance, __obfh_strstr_found, __obfh_strstr_done;     \
    struct {                                                                                                               \
        const char *__obfh_strstr_text;                                                                                    \
        const char *__obfh_strstr_pattern;                                                                                 \
    } __obfh_strstr_args = {__VA_ARGS__};                                                                                  \
    const char *__obfh_strstr_a;                                                                                           \
    const char *__obfh_strstr_b;                                                                                           \
    unsigned char __obfh_strstr_first = (unsigned char)*__obfh_strstr_args.__obfh_strstr_pattern;                          \
    char *__obfh_strstr_result = NULL;                                                                                     \
    BREAK_STACK_CFLOW;                                                                                                     \
    PHANTOM_NOP;                                                                                                           \
    OBFH_INLINE_EXIT(!__obfh_strstr_first, __obfh_strstr_found);                                                           \
__obfh_strstr_loop:                                                                                                        \
    OBFH_INLINE_EXIT(!*__obfh_strstr_args.__obfh_strstr_text, __obfh_strstr_done);                                         \
    OBFH_INLINE_EXIT((unsigned char)*__obfh_strstr_args.__obfh_strstr_text != __obfh_strstr_first, __obfh_strstr_advance); \
    __obfh_strstr_a = __obfh_strstr_args.__obfh_strstr_text + 1;                                                           \
    __obfh_strstr_b = __obfh_strstr_args.__obfh_strstr_pattern + 1;                                                        \
__obfh_strstr_match:                                                                                                       \
    OBFH_INLINE_EXIT(!*__obfh_strstr_b, __obfh_strstr_found);                                                              \
    OBFH_INLINE_EXIT(*__obfh_strstr_a != *__obfh_strstr_b, __obfh_strstr_advance);                                         \
    ++__obfh_strstr_a;                                                                                                     \
    ++__obfh_strstr_b;                                                                                                     \
    goto __obfh_strstr_match;                                                                                              \
__obfh_strstr_advance:                                                                                                     \
    ++__obfh_strstr_args.__obfh_strstr_text;                                                                               \
    goto __obfh_strstr_loop;                                                                                               \
__obfh_strstr_found:                                                                                                       \
    __obfh_strstr_result = (char *)__obfh_strstr_args.__obfh_strstr_text;                                                  \
__obfh_strstr_done:;                                                                                                       \
    __obfh_strstr_result;                                                                                                  \
})
#define strstr(...) strstr_custom(__VA_ARGS__)

// Local copying, character search and wide-string kernels.
// REP copy/fill and backward word/SSE2 loads accept unaligned addresses.
// Backward moves of 64+ bytes require SSE2 on x86/x64; byte tails never change DF.
#define strchr_custom(...) ({                                                                            \
    __label__ __obfh_strchr_loop, __obfh_strchr_found, __obfh_strchr_done;                               \
    struct {                                                                                             \
        const char *text;                                                                                \
        int character;                                                                                   \
    } __obfh_strchr_args = {__VA_ARGS__};                                                                \
    const char *__obfh_strchr_cursor = __obfh_strchr_args.text;                                          \
    unsigned char __obfh_strchr_wanted = (unsigned char)__obfh_strchr_args.character;                    \
    char *__obfh_strchr_result = NULL;                                                                   \
    BREAK_STACK_CFLOW;                                                                                   \
    PHANTOM_NOP;                                                                                         \
__obfh_strchr_loop:                                                                                      \
    OBFH_INLINE_EXIT((unsigned char)*__obfh_strchr_cursor == __obfh_strchr_wanted, __obfh_strchr_found); \
    OBFH_INLINE_EXIT(!*__obfh_strchr_cursor, __obfh_strchr_done);                                        \
    ++__obfh_strchr_cursor;                                                                              \
    goto __obfh_strchr_loop;                                                                             \
__obfh_strchr_found:                                                                                     \
    __obfh_strchr_result = (char *)__obfh_strchr_cursor;                                                 \
__obfh_strchr_done:;                                                                                     \
    __obfh_strchr_result;                                                                                \
})

#define strrchr_custom(...) ({                                                                              \
    __label__ __obfh_strrchr_loop, __obfh_strrchr_found, __obfh_strrchr_done;                               \
    struct {                                                                                                \
        const char *text;                                                                                   \
        int character;                                                                                      \
    } __obfh_strrchr_args = {__VA_ARGS__};                                                                  \
    const char *__obfh_strrchr_cursor = __obfh_strrchr_args.text;                                           \
    unsigned char __obfh_strrchr_wanted = (unsigned char)__obfh_strrchr_args.character;                     \
    char *__obfh_strrchr_result = NULL;                                                                     \
    BREAK_STACK_CFLOW;                                                                                      \
    PHANTOM_NOP;                                                                                            \
__obfh_strrchr_loop:                                                                                        \
    OBFH_INLINE_EXIT((unsigned char)*__obfh_strrchr_cursor == __obfh_strrchr_wanted, __obfh_strrchr_found); \
    OBFH_INLINE_EXIT(!*__obfh_strrchr_cursor, __obfh_strrchr_done);                                         \
    ++__obfh_strrchr_cursor;                                                                                \
    goto __obfh_strrchr_loop;                                                                               \
__obfh_strrchr_found:                                                                                       \
    __obfh_strrchr_result = (char *)__obfh_strrchr_cursor;                                                  \
    OBFH_INLINE_EXIT(!*__obfh_strrchr_cursor, __obfh_strrchr_done);                                         \
    ++__obfh_strrchr_cursor;                                                                                \
    goto __obfh_strrchr_loop;                                                                               \
__obfh_strrchr_done:;                                                                                       \
    __obfh_strrchr_result;                                                                                  \
})

#define wcschr_custom(...) ({                                                             \
    __label__ __obfh_wcschr_loop, __obfh_wcschr_found, __obfh_wcschr_done;                \
    struct {                                                                              \
        const wchar_t *text;                                                              \
        wchar_t character;                                                                \
    } __obfh_wcschr_args = {__VA_ARGS__};                                                 \
    const wchar_t *__obfh_wcschr_cursor = __obfh_wcschr_args.text;                        \
    wchar_t __obfh_wcschr_wanted = (wchar_t)__obfh_wcschr_args.character;                 \
    wchar_t *__obfh_wcschr_result = NULL;                                                 \
    BREAK_STACK_CFLOW;                                                                    \
    PHANTOM_NOP;                                                                          \
__obfh_wcschr_loop:                                                                       \
    OBFH_INLINE_EXIT(*__obfh_wcschr_cursor == __obfh_wcschr_wanted, __obfh_wcschr_found); \
    OBFH_INLINE_EXIT(!*__obfh_wcschr_cursor, __obfh_wcschr_done);                         \
    ++__obfh_wcschr_cursor;                                                               \
    goto __obfh_wcschr_loop;                                                              \
__obfh_wcschr_found:                                                                      \
    __obfh_wcschr_result = (wchar_t *)__obfh_wcschr_cursor;                               \
__obfh_wcschr_done:;                                                                      \
    __obfh_wcschr_result;                                                                 \
})
#define wcschr(...) wcschr_custom(__VA_ARGS__)

#define wcsrchr_custom(...) ({                                                               \
    __label__ __obfh_wcsrchr_loop, __obfh_wcsrchr_found, __obfh_wcsrchr_done;                \
    struct {                                                                                 \
        const wchar_t *text;                                                                 \
        wchar_t character;                                                                   \
    } __obfh_wcsrchr_args = {__VA_ARGS__};                                                   \
    const wchar_t *__obfh_wcsrchr_cursor = __obfh_wcsrchr_args.text;                         \
    wchar_t __obfh_wcsrchr_wanted = (wchar_t)__obfh_wcsrchr_args.character;                  \
    wchar_t *__obfh_wcsrchr_result = NULL;                                                   \
    BREAK_STACK_CFLOW;                                                                       \
    PHANTOM_NOP;                                                                             \
__obfh_wcsrchr_loop:                                                                         \
    OBFH_INLINE_EXIT(*__obfh_wcsrchr_cursor == __obfh_wcsrchr_wanted, __obfh_wcsrchr_found); \
    OBFH_INLINE_EXIT(!*__obfh_wcsrchr_cursor, __obfh_wcsrchr_done);                          \
    ++__obfh_wcsrchr_cursor;                                                                 \
    goto __obfh_wcsrchr_loop;                                                                \
__obfh_wcsrchr_found:                                                                        \
    __obfh_wcsrchr_result = (wchar_t *)__obfh_wcsrchr_cursor;                                \
    OBFH_INLINE_EXIT(!*__obfh_wcsrchr_cursor, __obfh_wcsrchr_done);                          \
    ++__obfh_wcsrchr_cursor;                                                                 \
    goto __obfh_wcsrchr_loop;                                                                \
__obfh_wcsrchr_done:;                                                                        \
    __obfh_wcsrchr_result;                                                                   \
})
#define wcsrchr(...) wcsrchr_custom(__VA_ARGS__)

#define strcpy_custom(...) ({                                    \
    __label__ __obfh_strcpy_copy, __obfh_strcpy_done;            \
    struct {                                                     \
        char *destination;                                       \
        const char *text;                                        \
    } __obfh_strcpy_args = {__VA_ARGS__};                        \
    char *__obfh_strcpy_target = __obfh_strcpy_args.destination; \
    const char *__obfh_strcpy_source = __obfh_strcpy_args.text;  \
    char __obfh_strcpy_value;                                    \
    BREAK_STACK_CFLOW;                                           \
    PHANTOM_NOP;                                                 \
__obfh_strcpy_copy:                                              \
    __obfh_strcpy_value = *__obfh_strcpy_source;                 \
    *__obfh_strcpy_target++ = __obfh_strcpy_value;               \
    OBFH_INLINE_EXIT(!__obfh_strcpy_value, __obfh_strcpy_done);  \
    ++__obfh_strcpy_source;                                      \
    goto __obfh_strcpy_copy;                                     \
__obfh_strcpy_done:;                                             \
    __obfh_strcpy_args.destination;                              \
})

#define strcat_custom(...) ({                                             \
    __label__ __obfh_strcat_scan, __obfh_strcat_copy, __obfh_strcat_done; \
    struct {                                                              \
        char *destination;                                                \
        const char *text;                                                 \
    } __obfh_strcat_args = {__VA_ARGS__};                                 \
    char *__obfh_strcat_target = __obfh_strcat_args.destination;          \
    const char *__obfh_strcat_source = __obfh_strcat_args.text;           \
    char __obfh_strcat_value;                                             \
    BREAK_STACK_CFLOW;                                                    \
    PHANTOM_NOP;                                                          \
__obfh_strcat_scan:                                                       \
    OBFH_INLINE_EXIT(!*__obfh_strcat_target, __obfh_strcat_copy);         \
    ++__obfh_strcat_target;                                               \
    goto __obfh_strcat_scan;                                              \
__obfh_strcat_copy:                                                       \
    __obfh_strcat_value = *__obfh_strcat_source;                          \
    *__obfh_strcat_target++ = __obfh_strcat_value;                        \
    OBFH_INLINE_EXIT(!__obfh_strcat_value, __obfh_strcat_done);           \
    ++__obfh_strcat_source;                                               \
    goto __obfh_strcat_copy;                                              \
__obfh_strcat_done:;                                                      \
    __obfh_strcat_args.destination;                                       \
})

#define strncpy_custom(...) ({                                               \
    __label__ __obfh_strncpy_copy, __obfh_strncpy_fill, __obfh_strncpy_done; \
    struct {                                                                 \
        char *destination;                                                   \
        const char *text;                                                    \
        size_t count;                                                        \
    } __obfh_strncpy_args = {__VA_ARGS__};                                   \
    char *__obfh_strncpy_target = __obfh_strncpy_args.destination;           \
    const char *__obfh_strncpy_source = __obfh_strncpy_args.text;            \
    char __obfh_strncpy_value;                                               \
    BREAK_STACK_CFLOW;                                                       \
    PHANTOM_NOP;                                                             \
__obfh_strncpy_copy:                                                         \
    OBFH_INLINE_EXIT(!__obfh_strncpy_args.count, __obfh_strncpy_done);       \
    __obfh_strncpy_value = *__obfh_strncpy_source;                           \
    *__obfh_strncpy_target++ = __obfh_strncpy_value;                         \
    --__obfh_strncpy_args.count;                                             \
    OBFH_INLINE_EXIT(!__obfh_strncpy_value, __obfh_strncpy_fill);            \
    ++__obfh_strncpy_source;                                                 \
    goto __obfh_strncpy_copy;                                                \
__obfh_strncpy_fill:                                                         \
    OBFH_INLINE_EXIT(!__obfh_strncpy_args.count, __obfh_strncpy_done);       \
    *__obfh_strncpy_target++ = 0;                                            \
    --__obfh_strncpy_args.count;                                             \
    goto __obfh_strncpy_fill;                                                \
__obfh_strncpy_done:;                                                        \
    __obfh_strncpy_args.destination;                                         \
})

#define strncat_custom(...) ({                                                                    \
    __label__ __obfh_strncat_scan, __obfh_strncat_copy, __obfh_strncat_fill, __obfh_strncat_done; \
    struct {                                                                                      \
        char *destination;                                                                        \
        const char *text;                                                                         \
        size_t count;                                                                             \
    } __obfh_strncat_args = {__VA_ARGS__};                                                        \
    char *__obfh_strncat_target = __obfh_strncat_args.destination;                                \
    const char *__obfh_strncat_source = __obfh_strncat_args.text;                                 \
    char __obfh_strncat_value;                                                                    \
    BREAK_STACK_CFLOW;                                                                            \
    PHANTOM_NOP;                                                                                  \
__obfh_strncat_scan:                                                                              \
    OBFH_INLINE_EXIT(!*__obfh_strncat_target, __obfh_strncat_copy);                               \
    ++__obfh_strncat_target;                                                                      \
    goto __obfh_strncat_scan;                                                                     \
__obfh_strncat_copy:                                                                              \
    OBFH_INLINE_EXIT(!__obfh_strncat_args.count, __obfh_strncat_fill);                            \
    __obfh_strncat_value = *__obfh_strncat_source;                                                \
    *__obfh_strncat_target++ = __obfh_strncat_value;                                              \
    --__obfh_strncat_args.count;                                                                  \
    OBFH_INLINE_EXIT(!__obfh_strncat_value, __obfh_strncat_done);                                 \
    ++__obfh_strncat_source;                                                                      \
    goto __obfh_strncat_copy;                                                                     \
__obfh_strncat_fill:                                                                              \
    *__obfh_strncat_target = 0;                                                                   \
__obfh_strncat_done:;                                                                             \
    __obfh_strncat_args.destination;                                                              \
})

#define memcpy_custom(...) ({                                              \
    struct {                                                               \
        void *destination;                                                 \
        const void *text;                                                  \
        size_t count;                                                      \
    } __obfh_memcpy_args = {__VA_ARGS__};                                  \
    void *__obfh_memcpy_target = __obfh_memcpy_args.destination;           \
    const void *__obfh_memcpy_source = __obfh_memcpy_args.text;            \
    size_t __obfh_memcpy_count = __obfh_memcpy_args.count;                 \
    BREAK_STACK_CFLOW;                                                     \
    PHANTOM_NOP;                                                           \
    __obfh_asm__("rep movsb"                                               \
                 : "+D"(__obfh_memcpy_target), "+S"(__obfh_memcpy_source), \
                   "+c"(__obfh_memcpy_count)                               \
                 :                                                         \
                 : "memory");                                              \
    __obfh_memcpy_args.destination;                                        \
})

#define memset_custom(...) ({                                            \
    struct {                                                             \
        void *destination;                                               \
        int character;                                                   \
        size_t count;                                                    \
    } __obfh_memset_args = {__VA_ARGS__};                                \
    void *__obfh_memset_target = __obfh_memset_args.destination;         \
    size_t __obfh_memset_count = __obfh_memset_args.count;               \
    BREAK_STACK_CFLOW;                                                   \
    PHANTOM_NOP;                                                         \
    __obfh_asm__("rep stosb"                                             \
                 : "+D"(__obfh_memset_target), "+c"(__obfh_memset_count) \
                 : "a"((unsigned char)__obfh_memset_args.character)      \
                 : "memory");                                            \
    __obfh_memset_args.destination;                                      \
})

#if defined(__x86_64__)
#define OBFH_MOVE_WIDTH "q"
#define OBFH_MOVE_REGISTER "%%rax"
#define OBFH_MOVE_STEP "8"
#else
#define OBFH_MOVE_WIDTH "l"
#define OBFH_MOVE_REGISTER "%%eax"
#define OBFH_MOVE_STEP "4"
#endif
#define memmove_custom(...) ({                                                                                        \
    __label__ __obfh_memmove_backward, __obfh_memmove_vector, __obfh_memmove_done;                                    \
    struct {                                                                                                          \
        void *destination;                                                                                            \
        const void *text;                                                                                             \
        size_t count;                                                                                                 \
    } __obfh_memmove_args = {__VA_ARGS__};                                                                            \
    void *__obfh_memmove_target = __obfh_memmove_args.destination;                                                    \
    const void *__obfh_memmove_source = __obfh_memmove_args.text;                                                     \
    size_t __obfh_memmove_count = __obfh_memmove_args.count;                                                          \
    BREAK_STACK_CFLOW;                                                                                                \
    PHANTOM_NOP;                                                                                                      \
    OBFH_INLINE_EXIT(!__obfh_memmove_count, __obfh_memmove_done);                                                     \
    OBFH_INLINE_EXIT((ULONG_PTR)__obfh_memmove_target > (ULONG_PTR)__obfh_memmove_source &&                           \
                         (ULONG_PTR)__obfh_memmove_target - (ULONG_PTR)__obfh_memmove_source < __obfh_memmove_count,  \
                     __obfh_memmove_backward);                                                                        \
    __obfh_asm__("rep movsb"                                                                                          \
                 : "+D"(__obfh_memmove_target), "+S"(__obfh_memmove_source),                                          \
                   "+c"(__obfh_memmove_count)                                                                         \
                 :                                                                                                    \
                 : "memory");                                                                                         \
    goto __obfh_memmove_done;                                                                                         \
__obfh_memmove_backward:                                                                                              \
    OBFH_INLINE_EXIT(__obfh_memmove_count >= 64, __obfh_memmove_vector);                                              \
    __obfh_memmove_target = (unsigned char *)__obfh_memmove_target + __obfh_memmove_count - 1;                        \
    __obfh_memmove_source = (const unsigned char *)__obfh_memmove_source + __obfh_memmove_count - 1;                  \
    size_t __obfh_memmove_tail = __obfh_memmove_count % sizeof(ULONG_PTR);                                            \
    __obfh_memmove_count /= sizeof(ULONG_PTR);                                                                        \
    __obfh_asm__("test" OBFH_MOVE_WIDTH                                                                               \
                 " %2, %2; jz 2f; 1: movb (%1), %%al; movb %%al, (%0); "                                              \
                 "dec" OBFH_MOVE_WIDTH " %0; dec" OBFH_MOVE_WIDTH " %1; dec" OBFH_MOVE_WIDTH " %2; jnz 1b; 2:"        \
                 : "+D"(__obfh_memmove_target), "+S"(__obfh_memmove_source), "+c"(__obfh_memmove_tail)                \
                 :                                                                                                    \
                 : "eax", "cc", "memory");                                                                            \
    OBFH_INLINE_EXIT(!__obfh_memmove_count, __obfh_memmove_done);                                                     \
    __obfh_memmove_target = (unsigned char *)__obfh_memmove_target - (sizeof(ULONG_PTR) - 1);                         \
    __obfh_memmove_source = (const unsigned char *)__obfh_memmove_source - (sizeof(ULONG_PTR) - 1);                   \
    __obfh_asm__("1: mov" OBFH_MOVE_WIDTH " (%1), " OBFH_MOVE_REGISTER "; mov" OBFH_MOVE_WIDTH " " OBFH_MOVE_REGISTER \
                 ", (%0); "                                                                                           \
                 "sub" OBFH_MOVE_WIDTH " $" OBFH_MOVE_STEP ", %0; sub" OBFH_MOVE_WIDTH " $" OBFH_MOVE_STEP            \
                 ", %1; "                                                                                             \
                 "dec" OBFH_MOVE_WIDTH " %2; jnz 1b;"                                                                 \
                 : "+D"(__obfh_memmove_target), "+S"(__obfh_memmove_source), "+c"(__obfh_memmove_count)               \
                 :                                                                                                    \
                 : "eax", "cc", "memory");                                                                            \
    goto __obfh_memmove_done;                                                                                         \
__obfh_memmove_vector:                                                                                                \
    __obfh_memmove_target = (unsigned char *)__obfh_memmove_target + __obfh_memmove_count - 1;                        \
    __obfh_memmove_source = (const unsigned char *)__obfh_memmove_source + __obfh_memmove_count - 1;                  \
    size_t __obfh_memmove_vector_tail = __obfh_memmove_count & 15u;                                                   \
    __obfh_memmove_count >>= 4;                                                                                       \
    __obfh_asm__("test" OBFH_MOVE_WIDTH                                                                               \
                 " %2, %2; jz 2f; 1: movb (%1), %%al; movb %%al, (%0); "                                              \
                 "dec" OBFH_MOVE_WIDTH " %0; dec" OBFH_MOVE_WIDTH " %1; dec" OBFH_MOVE_WIDTH " %2; jnz 1b; 2:"        \
                 : "+D"(__obfh_memmove_target), "+S"(__obfh_memmove_source), "+c"(__obfh_memmove_vector_tail)         \
                 :                                                                                                    \
                 : "eax", "cc", "memory");                                                                            \
    __obfh_memmove_target = (unsigned char *)__obfh_memmove_target - 15;                                              \
    __obfh_memmove_source = (const unsigned char *)__obfh_memmove_source - 15;                                        \
    /* TCC lacks MOVDQU syntax and XMM clobbers: fixed D/S/d addresses, save/restore XMM0. */                         \
    unsigned char __obfh_memmove_xmm_save[16];                                                                        \
    __obfh_asm__(                                                                                                     \
        ".byte 0xf3,0x0f,0x7f,0x02; 1: .byte 0xf3,0x0f,0x6f,0x06; .byte 0xf3,0x0f,0x7f,0x07; "                        \
        "sub" OBFH_MOVE_WIDTH " $16, %0; sub" OBFH_MOVE_WIDTH                                                         \
        " $16, %1; "                                                                                                  \
        "dec" OBFH_MOVE_WIDTH " %2; jnz 1b; .byte 0xf3,0x0f,0x6f,0x02;"                                               \
        : "+D"(__obfh_memmove_target), "+S"(__obfh_memmove_source), "+c"(__obfh_memmove_count)                        \
        : "d"(__obfh_memmove_xmm_save)                                                                                \
        : "cc", "memory");                                                                                            \
__obfh_memmove_done:;                                                                                                 \
    __obfh_memmove_args.destination;                                                                                  \
})
#define wcslen_custom(...) ({                                                            \
    __label__ __obfh_wcslen_loop, __obfh_wcslen_done;                                    \
    struct {                                                                             \
        const wchar_t *text;                                                             \
    } __obfh_wcslen_args = {__VA_ARGS__};                                                \
    size_t __obfh_wcslen_index = 0;                                                      \
    BREAK_STACK_CFLOW;                                                                   \
    PHANTOM_NOP;                                                                         \
__obfh_wcslen_loop:                                                                      \
    OBFH_INLINE_EXIT(!__obfh_wcslen_args.text[__obfh_wcslen_index], __obfh_wcslen_done); \
    ++__obfh_wcslen_index;                                                               \
    goto __obfh_wcslen_loop;                                                             \
__obfh_wcslen_done:;                                                                     \
    __obfh_wcslen_index;                                                                 \
})
#define wcslen(...) wcslen_custom(__VA_ARGS__)

#define wcscmp_custom(...) ({                                                                                                               \
    __label__ __obfh_wcscmp_loop, __obfh_wcscmp_done;                                                                                       \
    struct {                                                                                                                                \
        const wchar_t *left;                                                                                                                \
        const wchar_t *right;                                                                                                               \
    } __obfh_wcscmp_args = {__VA_ARGS__};                                                                                                   \
    int __obfh_wcscmp_result = 0;                                                                                                           \
    BREAK_STACK_CFLOW;                                                                                                                      \
    PHANTOM_NOP;                                                                                                                            \
__obfh_wcscmp_loop:                                                                                                                         \
    __obfh_wcscmp_result = (*__obfh_wcscmp_args.left > *__obfh_wcscmp_args.right) - (*__obfh_wcscmp_args.left < *__obfh_wcscmp_args.right); \
    OBFH_INLINE_EXIT(__obfh_wcscmp_result || !*__obfh_wcscmp_args.left, __obfh_wcscmp_done);                                                \
    ++__obfh_wcscmp_args.left;                                                                                                              \
    ++__obfh_wcscmp_args.right;                                                                                                             \
    goto __obfh_wcscmp_loop;                                                                                                                \
__obfh_wcscmp_done:;                                                                                                                        \
    __obfh_wcscmp_result;                                                                                                                   \
})
#define wcscmp(...) wcscmp_custom(__VA_ARGS__)

#define wcsncmp_custom(...) ({                                                                                                                   \
    __label__ __obfh_wcsncmp_loop, __obfh_wcsncmp_done;                                                                                          \
    struct {                                                                                                                                     \
        const wchar_t *left;                                                                                                                     \
        const wchar_t *right;                                                                                                                    \
        size_t count;                                                                                                                            \
    } __obfh_wcsncmp_args = {__VA_ARGS__};                                                                                                       \
    int __obfh_wcsncmp_result = 0;                                                                                                               \
    BREAK_STACK_CFLOW;                                                                                                                           \
    PHANTOM_NOP;                                                                                                                                 \
__obfh_wcsncmp_loop:                                                                                                                             \
    OBFH_INLINE_EXIT(!__obfh_wcsncmp_args.count, __obfh_wcsncmp_done);                                                                           \
    __obfh_wcsncmp_result = (*__obfh_wcsncmp_args.left > *__obfh_wcsncmp_args.right) - (*__obfh_wcsncmp_args.left < *__obfh_wcsncmp_args.right); \
    OBFH_INLINE_EXIT(__obfh_wcsncmp_result || !*__obfh_wcsncmp_args.left, __obfh_wcsncmp_done);                                                  \
    ++__obfh_wcsncmp_args.left;                                                                                                                  \
    ++__obfh_wcsncmp_args.right;                                                                                                                 \
    --__obfh_wcsncmp_args.count;                                                                                                                 \
    goto __obfh_wcsncmp_loop;                                                                                                                    \
__obfh_wcsncmp_done:;                                                                                                                            \
    __obfh_wcsncmp_result;                                                                                                                       \
})
#define wcsncmp(...) wcsncmp_custom(__VA_ARGS__)

// Wide copies/searches and bounded lengths retain native internal branches.
#define wcscpy_custom(...) ({                                       \
    __label__ __obfh_wcscpy_copy, __obfh_wcscpy_done;               \
    struct {                                                        \
        wchar_t *destination;                                       \
        const wchar_t *text;                                        \
    } __obfh_wcscpy_args = {__VA_ARGS__};                           \
    wchar_t *__obfh_wcscpy_target = __obfh_wcscpy_args.destination; \
    const wchar_t *__obfh_wcscpy_source = __obfh_wcscpy_args.text;  \
    wchar_t __obfh_wcscpy_value;                                    \
    BREAK_STACK_CFLOW;                                              \
    PHANTOM_NOP;                                                    \
__obfh_wcscpy_copy:                                                 \
    __obfh_wcscpy_value = *__obfh_wcscpy_source;                    \
    *__obfh_wcscpy_target++ = __obfh_wcscpy_value;                  \
    OBFH_INLINE_EXIT(!__obfh_wcscpy_value, __obfh_wcscpy_done);     \
    ++__obfh_wcscpy_source;                                         \
    goto __obfh_wcscpy_copy;                                        \
__obfh_wcscpy_done:;                                                \
    __obfh_wcscpy_args.destination;                                 \
})
#define wcscpy(...) wcscpy_custom(__VA_ARGS__)

#define wcscat_custom(...) ({                                             \
    __label__ __obfh_wcscat_scan, __obfh_wcscat_copy, __obfh_wcscat_done; \
    struct {                                                              \
        wchar_t *destination;                                             \
        const wchar_t *text;                                              \
    } __obfh_wcscat_args = {__VA_ARGS__};                                 \
    wchar_t *__obfh_wcscat_target = __obfh_wcscat_args.destination;       \
    const wchar_t *__obfh_wcscat_source = __obfh_wcscat_args.text;        \
    wchar_t __obfh_wcscat_value;                                          \
    BREAK_STACK_CFLOW;                                                    \
    PHANTOM_NOP;                                                          \
__obfh_wcscat_scan:                                                       \
    OBFH_INLINE_EXIT(!*__obfh_wcscat_target, __obfh_wcscat_copy);         \
    ++__obfh_wcscat_target;                                               \
    goto __obfh_wcscat_scan;                                              \
__obfh_wcscat_copy:                                                       \
    __obfh_wcscat_value = *__obfh_wcscat_source;                          \
    *__obfh_wcscat_target++ = __obfh_wcscat_value;                        \
    OBFH_INLINE_EXIT(!__obfh_wcscat_value, __obfh_wcscat_done);           \
    ++__obfh_wcscat_source;                                               \
    goto __obfh_wcscat_copy;                                              \
__obfh_wcscat_done:;                                                      \
    __obfh_wcscat_args.destination;                                       \
})
#define wcscat(...) wcscat_custom(__VA_ARGS__)

#define wcsncpy_custom(...) ({                                               \
    __label__ __obfh_wcsncpy_copy, __obfh_wcsncpy_fill, __obfh_wcsncpy_done; \
    struct {                                                                 \
        wchar_t *destination;                                                \
        const wchar_t *text;                                                 \
        size_t count;                                                        \
    } __obfh_wcsncpy_args = {__VA_ARGS__};                                   \
    wchar_t *__obfh_wcsncpy_target = __obfh_wcsncpy_args.destination;        \
    const wchar_t *__obfh_wcsncpy_source = __obfh_wcsncpy_args.text;         \
    wchar_t __obfh_wcsncpy_value;                                            \
    BREAK_STACK_CFLOW;                                                       \
    PHANTOM_NOP;                                                             \
__obfh_wcsncpy_copy:                                                         \
    OBFH_INLINE_EXIT(!__obfh_wcsncpy_args.count, __obfh_wcsncpy_done);       \
    __obfh_wcsncpy_value = *__obfh_wcsncpy_source;                           \
    *__obfh_wcsncpy_target++ = __obfh_wcsncpy_value;                         \
    --__obfh_wcsncpy_args.count;                                             \
    OBFH_INLINE_EXIT(!__obfh_wcsncpy_value, __obfh_wcsncpy_fill);            \
    ++__obfh_wcsncpy_source;                                                 \
    goto __obfh_wcsncpy_copy;                                                \
__obfh_wcsncpy_fill:                                                         \
    OBFH_INLINE_EXIT(!__obfh_wcsncpy_args.count, __obfh_wcsncpy_done);       \
    *__obfh_wcsncpy_target++ = 0;                                            \
    --__obfh_wcsncpy_args.count;                                             \
    goto __obfh_wcsncpy_fill;                                                \
__obfh_wcsncpy_done:;                                                        \
    __obfh_wcsncpy_args.destination;                                         \
})
#define wcsncpy(...) wcsncpy_custom(__VA_ARGS__)

#define wcsncat_custom(...) ({                                                                    \
    __label__ __obfh_wcsncat_scan, __obfh_wcsncat_copy, __obfh_wcsncat_fill, __obfh_wcsncat_done; \
    struct {                                                                                      \
        wchar_t *destination;                                                                     \
        const wchar_t *text;                                                                      \
        size_t count;                                                                             \
    } __obfh_wcsncat_args = {__VA_ARGS__};                                                        \
    wchar_t *__obfh_wcsncat_target = __obfh_wcsncat_args.destination;                             \
    const wchar_t *__obfh_wcsncat_source = __obfh_wcsncat_args.text;                              \
    wchar_t __obfh_wcsncat_value;                                                                 \
    BREAK_STACK_CFLOW;                                                                            \
    PHANTOM_NOP;                                                                                  \
__obfh_wcsncat_scan:                                                                              \
    OBFH_INLINE_EXIT(!*__obfh_wcsncat_target, __obfh_wcsncat_copy);                               \
    ++__obfh_wcsncat_target;                                                                      \
    goto __obfh_wcsncat_scan;                                                                     \
__obfh_wcsncat_copy:                                                                              \
    OBFH_INLINE_EXIT(!__obfh_wcsncat_args.count, __obfh_wcsncat_fill);                            \
    __obfh_wcsncat_value = *__obfh_wcsncat_source;                                                \
    *__obfh_wcsncat_target++ = __obfh_wcsncat_value;                                              \
    --__obfh_wcsncat_args.count;                                                                  \
    OBFH_INLINE_EXIT(!__obfh_wcsncat_value, __obfh_wcsncat_done);                                 \
    ++__obfh_wcsncat_source;                                                                      \
    goto __obfh_wcsncat_copy;                                                                     \
__obfh_wcsncat_fill:                                                                              \
    *__obfh_wcsncat_target = 0;                                                                   \
__obfh_wcsncat_done:;                                                                             \
    __obfh_wcsncat_args.destination;                                                              \
})
#define wcsncat(...) wcsncat_custom(__VA_ARGS__)

#define wcsstr_custom(...) ({                                                                                          \
    __label__ __obfh_wcsstr_loop, __obfh_wcsstr_match, __obfh_wcsstr_advance, __obfh_wcsstr_found, __obfh_wcsstr_done; \
    struct {                                                                                                           \
        const wchar_t *__obfh_wcsstr_text;                                                                             \
        const wchar_t *__obfh_wcsstr_pattern;                                                                          \
    } __obfh_wcsstr_args = {__VA_ARGS__};                                                                              \
    const wchar_t *__obfh_wcsstr_a;                                                                                    \
    const wchar_t *__obfh_wcsstr_b;                                                                                    \
    wchar_t __obfh_wcsstr_first = (wchar_t)*__obfh_wcsstr_args.__obfh_wcsstr_pattern;                                  \
    wchar_t *__obfh_wcsstr_result = NULL;                                                                              \
    BREAK_STACK_CFLOW;                                                                                                 \
    PHANTOM_NOP;                                                                                                       \
    OBFH_INLINE_EXIT(!__obfh_wcsstr_first, __obfh_wcsstr_found);                                                       \
__obfh_wcsstr_loop:                                                                                                    \
    OBFH_INLINE_EXIT(!*__obfh_wcsstr_args.__obfh_wcsstr_text, __obfh_wcsstr_done);                                     \
    OBFH_INLINE_EXIT((wchar_t)*__obfh_wcsstr_args.__obfh_wcsstr_text != __obfh_wcsstr_first, __obfh_wcsstr_advance);   \
    __obfh_wcsstr_a = __obfh_wcsstr_args.__obfh_wcsstr_text + 1;                                                       \
    __obfh_wcsstr_b = __obfh_wcsstr_args.__obfh_wcsstr_pattern + 1;                                                    \
__obfh_wcsstr_match:                                                                                                   \
    OBFH_INLINE_EXIT(!*__obfh_wcsstr_b, __obfh_wcsstr_found);                                                          \
    OBFH_INLINE_EXIT(*__obfh_wcsstr_a != *__obfh_wcsstr_b, __obfh_wcsstr_advance);                                     \
    ++__obfh_wcsstr_a;                                                                                                 \
    ++__obfh_wcsstr_b;                                                                                                 \
    goto __obfh_wcsstr_match;                                                                                          \
__obfh_wcsstr_advance:                                                                                                 \
    ++__obfh_wcsstr_args.__obfh_wcsstr_text;                                                                           \
    goto __obfh_wcsstr_loop;                                                                                           \
__obfh_wcsstr_found:                                                                                                   \
    __obfh_wcsstr_result = (wchar_t *)__obfh_wcsstr_args.__obfh_wcsstr_text;                                           \
__obfh_wcsstr_done:;                                                                                                   \
    __obfh_wcsstr_result;                                                                                              \
})
#define wcsstr(...) wcsstr_custom(__VA_ARGS__)

#define wcsspn_custom(...) ({                                                                                         \
    __label__ __obfh_wcsspn_loop, __obfh_wcsspn_match, __obfh_wcsspn_select, __obfh_wcsspn_done;                      \
    struct {                                                                                                          \
        const wchar_t *__obfh_wcsspn_text;                                                                            \
        const wchar_t *__obfh_wcsspn_set;                                                                             \
    } __obfh_wcsspn_args = {__VA_ARGS__};                                                                             \
    const wchar_t *__obfh_wcsspn_cursor = (const wchar_t *)__obfh_wcsspn_args.__obfh_wcsspn_text;                     \
    const wchar_t *__obfh_wcsspn_member;                                                                              \
    BREAK_STACK_CFLOW;                                                                                                \
    PHANTOM_NOP;                                                                                                      \
__obfh_wcsspn_loop:                                                                                                   \
    OBFH_INLINE_EXIT(!*__obfh_wcsspn_cursor, __obfh_wcsspn_done);                                                     \
    __obfh_wcsspn_member = (const wchar_t *)__obfh_wcsspn_args.__obfh_wcsspn_set;                                     \
__obfh_wcsspn_match:                                                                                                  \
    OBFH_INLINE_EXIT(!*__obfh_wcsspn_member || *__obfh_wcsspn_member == *__obfh_wcsspn_cursor, __obfh_wcsspn_select); \
    ++__obfh_wcsspn_member;                                                                                           \
    goto __obfh_wcsspn_match;                                                                                         \
__obfh_wcsspn_select:                                                                                                 \
    OBFH_INLINE_EXIT(!*__obfh_wcsspn_member, __obfh_wcsspn_done);                                                     \
    ++__obfh_wcsspn_cursor;                                                                                           \
    goto __obfh_wcsspn_loop;                                                                                          \
__obfh_wcsspn_done:;                                                                                                  \
    (size_t)(__obfh_wcsspn_cursor - (const wchar_t *)__obfh_wcsspn_args.__obfh_wcsspn_text);                          \
})
#define wcsspn(...) wcsspn_custom(__VA_ARGS__)

#define wcscspn_custom(...) ({                                                                                            \
    __label__ __obfh_wcscspn_loop, __obfh_wcscspn_match, __obfh_wcscspn_select, __obfh_wcscspn_done;                      \
    struct {                                                                                                              \
        const wchar_t *__obfh_wcscspn_text;                                                                               \
        const wchar_t *__obfh_wcscspn_set;                                                                                \
    } __obfh_wcscspn_args = {__VA_ARGS__};                                                                                \
    const wchar_t *__obfh_wcscspn_cursor = (const wchar_t *)__obfh_wcscspn_args.__obfh_wcscspn_text;                      \
    const wchar_t *__obfh_wcscspn_member;                                                                                 \
    BREAK_STACK_CFLOW;                                                                                                    \
    PHANTOM_NOP;                                                                                                          \
__obfh_wcscspn_loop:                                                                                                      \
    OBFH_INLINE_EXIT(!*__obfh_wcscspn_cursor, __obfh_wcscspn_done);                                                       \
    __obfh_wcscspn_member = (const wchar_t *)__obfh_wcscspn_args.__obfh_wcscspn_set;                                      \
__obfh_wcscspn_match:                                                                                                     \
    OBFH_INLINE_EXIT(!*__obfh_wcscspn_member || *__obfh_wcscspn_member == *__obfh_wcscspn_cursor, __obfh_wcscspn_select); \
    ++__obfh_wcscspn_member;                                                                                              \
    goto __obfh_wcscspn_match;                                                                                            \
__obfh_wcscspn_select:                                                                                                    \
    OBFH_INLINE_EXIT(*__obfh_wcscspn_member, __obfh_wcscspn_done);                                                        \
    ++__obfh_wcscspn_cursor;                                                                                              \
    goto __obfh_wcscspn_loop;                                                                                             \
__obfh_wcscspn_done:;                                                                                                     \
    (size_t)(__obfh_wcscspn_cursor - (const wchar_t *)__obfh_wcscspn_args.__obfh_wcscspn_text);                           \
})
#define wcscspn(...) wcscspn_custom(__VA_ARGS__)

#define wcspbrk_custom(...) ({                                                                                            \
    __label__ __obfh_wcspbrk_loop, __obfh_wcspbrk_match, __obfh_wcspbrk_select, __obfh_wcspbrk_done;                      \
    struct {                                                                                                              \
        const wchar_t *__obfh_wcspbrk_text;                                                                               \
        const wchar_t *__obfh_wcspbrk_set;                                                                                \
    } __obfh_wcspbrk_args = {__VA_ARGS__};                                                                                \
    const wchar_t *__obfh_wcspbrk_cursor = (const wchar_t *)__obfh_wcspbrk_args.__obfh_wcspbrk_text;                      \
    const wchar_t *__obfh_wcspbrk_member;                                                                                 \
    BREAK_STACK_CFLOW;                                                                                                    \
    PHANTOM_NOP;                                                                                                          \
__obfh_wcspbrk_loop:                                                                                                      \
    OBFH_INLINE_EXIT(!*__obfh_wcspbrk_cursor, __obfh_wcspbrk_done);                                                       \
    __obfh_wcspbrk_member = (const wchar_t *)__obfh_wcspbrk_args.__obfh_wcspbrk_set;                                      \
__obfh_wcspbrk_match:                                                                                                     \
    OBFH_INLINE_EXIT(!*__obfh_wcspbrk_member || *__obfh_wcspbrk_member == *__obfh_wcspbrk_cursor, __obfh_wcspbrk_select); \
    ++__obfh_wcspbrk_member;                                                                                              \
    goto __obfh_wcspbrk_match;                                                                                            \
__obfh_wcspbrk_select:                                                                                                    \
    OBFH_INLINE_EXIT(*__obfh_wcspbrk_member, __obfh_wcspbrk_done);                                                        \
    ++__obfh_wcspbrk_cursor;                                                                                              \
    goto __obfh_wcspbrk_loop;                                                                                             \
__obfh_wcspbrk_done:;                                                                                                     \
    (wchar_t *)(*__obfh_wcspbrk_cursor ? (wchar_t *)__obfh_wcspbrk_cursor : NULL);                                        \
})
#define wcspbrk(...) wcspbrk_custom(__VA_ARGS__)

#define strnlen_custom(...) ({                                                                                                                     \
    __label__ __obfh_strnlen_loop, __obfh_strnlen_done;                                                                                            \
    struct {                                                                                                                                       \
        const char *text;                                                                                                                          \
        size_t count;                                                                                                                              \
    } __obfh_strnlen_args = {__VA_ARGS__};                                                                                                         \
    size_t __obfh_strnlen_length = 0;                                                                                                              \
    BREAK_STACK_CFLOW;                                                                                                                             \
    PHANTOM_NOP;                                                                                                                                   \
__obfh_strnlen_loop:                                                                                                                               \
    OBFH_INLINE_EXIT(__obfh_strnlen_length == __obfh_strnlen_args.count || !__obfh_strnlen_args.text[__obfh_strnlen_length], __obfh_strnlen_done); \
    ++__obfh_strnlen_length;                                                                                                                       \
    goto __obfh_strnlen_loop;                                                                                                                      \
__obfh_strnlen_done:;                                                                                                                              \
    __obfh_strnlen_length;                                                                                                                         \
})
#define strnlen(...) strnlen_custom(__VA_ARGS__)

#define wcsnlen_custom(...) ({                                                                                                                     \
    __label__ __obfh_wcsnlen_loop, __obfh_wcsnlen_done;                                                                                            \
    struct {                                                                                                                                       \
        const wchar_t *text;                                                                                                                       \
        size_t count;                                                                                                                              \
    } __obfh_wcsnlen_args = {__VA_ARGS__};                                                                                                         \
    size_t __obfh_wcsnlen_length = 0;                                                                                                              \
    BREAK_STACK_CFLOW;                                                                                                                             \
    PHANTOM_NOP;                                                                                                                                   \
__obfh_wcsnlen_loop:                                                                                                                               \
    OBFH_INLINE_EXIT(__obfh_wcsnlen_length == __obfh_wcsnlen_args.count || !__obfh_wcsnlen_args.text[__obfh_wcsnlen_length], __obfh_wcsnlen_done); \
    ++__obfh_wcsnlen_length;                                                                                                                       \
    goto __obfh_wcsnlen_loop;                                                                                                                      \
__obfh_wcsnlen_done:;                                                                                                                              \
    __obfh_wcsnlen_length;                                                                                                                         \
})
#define wcsnlen(...) wcsnlen_custom(__VA_ARGS__)

#define wmemcpy_custom(...) ({                                                                                                        \
    struct {                                                                                                                          \
        wchar_t *destination;                                                                                                         \
        const wchar_t *text;                                                                                                          \
        size_t count;                                                                                                                 \
    } __obfh_wmemcpy_args = {__VA_ARGS__};                                                                                            \
    (wchar_t *)memcpy_custom(__obfh_wmemcpy_args.destination, __obfh_wmemcpy_args.text, __obfh_wmemcpy_args.count * sizeof(wchar_t)); \
})
#define wmemcpy(...) wmemcpy_custom(__VA_ARGS__)

#define wmemmove_custom(...) ({                                                                                                           \
    struct {                                                                                                                              \
        wchar_t *destination;                                                                                                             \
        const wchar_t *text;                                                                                                              \
        size_t count;                                                                                                                     \
    } __obfh_wmemmove_args = {__VA_ARGS__};                                                                                               \
    (wchar_t *)memmove_custom(__obfh_wmemmove_args.destination, __obfh_wmemmove_args.text, __obfh_wmemmove_args.count * sizeof(wchar_t)); \
})
#define wmemmove(...) wmemmove_custom(__VA_ARGS__)

#define wmemset_custom(...) ({                                             \
    struct {                                                               \
        wchar_t *destination;                                              \
        wchar_t value;                                                     \
        size_t count;                                                      \
    } __obfh_wmemset_args = {__VA_ARGS__};                                 \
    wchar_t *__obfh_wmemset_target = __obfh_wmemset_args.destination;      \
    size_t __obfh_wmemset_count = __obfh_wmemset_args.count;               \
    BREAK_STACK_CFLOW;                                                     \
    PHANTOM_NOP;                                                           \
    __obfh_asm__("rep stosw"                                               \
                 : "+D"(__obfh_wmemset_target), "+c"(__obfh_wmemset_count) \
                 : "a"(__obfh_wmemset_args.value)                          \
                 : "memory");                                              \
    __obfh_wmemset_args.destination;                                       \
})
#define wmemset(...) wmemset_custom(__VA_ARGS__)

#define wmemcmp_custom(...) ({                                                                                                                   \
    __label__ __obfh_wmemcmp_loop, __obfh_wmemcmp_done;                                                                                          \
    struct {                                                                                                                                     \
        const wchar_t *left;                                                                                                                     \
        const wchar_t *right;                                                                                                                    \
        size_t count;                                                                                                                            \
    } __obfh_wmemcmp_args = {__VA_ARGS__};                                                                                                       \
    int __obfh_wmemcmp_result = 0;                                                                                                               \
    BREAK_STACK_CFLOW;                                                                                                                           \
    PHANTOM_NOP;                                                                                                                                 \
__obfh_wmemcmp_loop:                                                                                                                             \
    OBFH_INLINE_EXIT(!__obfh_wmemcmp_args.count, __obfh_wmemcmp_done);                                                                           \
    __obfh_wmemcmp_result = (*__obfh_wmemcmp_args.left > *__obfh_wmemcmp_args.right) - (*__obfh_wmemcmp_args.left < *__obfh_wmemcmp_args.right); \
    OBFH_INLINE_EXIT(__obfh_wmemcmp_result, __obfh_wmemcmp_done);                                                                                \
    ++__obfh_wmemcmp_args.left;                                                                                                                  \
    ++__obfh_wmemcmp_args.right;                                                                                                                 \
    --__obfh_wmemcmp_args.count;                                                                                                                 \
    goto __obfh_wmemcmp_loop;                                                                                                                    \
__obfh_wmemcmp_done:;                                                                                                                            \
    __obfh_wmemcmp_result;                                                                                                                       \
})
#define wmemcmp(...) wmemcmp_custom(__VA_ARGS__)

#define wmemchr_custom(...) ({                                                                      \
    __label__ __obfh_wmemchr_loop, __obfh_wmemchr_found, __obfh_wmemchr_done;                       \
    struct {                                                                                        \
        const wchar_t *text;                                                                        \
        wchar_t value;                                                                              \
        size_t count;                                                                               \
    } __obfh_wmemchr_args = {__VA_ARGS__};                                                          \
    wchar_t *__obfh_wmemchr_result = NULL;                                                          \
    BREAK_STACK_CFLOW;                                                                              \
    PHANTOM_NOP;                                                                                    \
__obfh_wmemchr_loop:                                                                                \
    OBFH_INLINE_EXIT(!__obfh_wmemchr_args.count, __obfh_wmemchr_done);                              \
    OBFH_INLINE_EXIT(*__obfh_wmemchr_args.text == __obfh_wmemchr_args.value, __obfh_wmemchr_found); \
    ++__obfh_wmemchr_args.text;                                                                     \
    --__obfh_wmemchr_args.count;                                                                    \
    goto __obfh_wmemchr_loop;                                                                       \
__obfh_wmemchr_found:                                                                               \
    __obfh_wmemchr_result = (wchar_t *)__obfh_wmemchr_args.text;                                    \
__obfh_wmemchr_done:;                                                                               \
    __obfh_wmemchr_result;                                                                          \
})
#define wmemchr(...) wmemchr_custom(__VA_ARGS__)

#define bsearch_custom(...) ({                                                                                                        \
    __label__ __obfh_bsearch_loop, __obfh_bsearch_found, __obfh_bsearch_done;                                                         \
    struct {                                                                                                                          \
        const void *key;                                                                                                              \
        const void *base;                                                                                                             \
        size_t count;                                                                                                                 \
        size_t width;                                                                                                                 \
        int(__cdecl * compare)(const void *, const void *);                                                                           \
    } __obfh_bsearch_args = {__VA_ARGS__};                                                                                            \
    const unsigned char *__obfh_bsearch_base = (const unsigned char *)__obfh_bsearch_args.base;                                       \
    const unsigned char *__obfh_bsearch_middle;                                                                                       \
    size_t __obfh_bsearch_half;                                                                                                       \
    int __obfh_bsearch_order;                                                                                                         \
    void *__obfh_bsearch_result = NULL;                                                                                               \
    BREAK_STACK_CFLOW;                                                                                                                \
    PHANTOM_NOP;                                                                                                                      \
__obfh_bsearch_loop:                                                                                                                  \
    OBFH_INLINE_EXIT(!__obfh_bsearch_args.count, __obfh_bsearch_done);                                                                \
    __obfh_bsearch_half = __obfh_bsearch_args.count / 2;                                                                              \
    __obfh_bsearch_middle = __obfh_bsearch_base + __obfh_bsearch_half * __obfh_bsearch_args.width;                                    \
    __obfh_bsearch_order = __obfh_bsearch_args.compare(__obfh_bsearch_args.key, __obfh_bsearch_middle);                               \
    OBFH_INLINE_EXIT(!__obfh_bsearch_order, __obfh_bsearch_found);                                                                    \
    __obfh_bsearch_base = __obfh_bsearch_order < 0 ? __obfh_bsearch_base : __obfh_bsearch_middle + __obfh_bsearch_args.width;         \
    __obfh_bsearch_args.count = __obfh_bsearch_order < 0 ? __obfh_bsearch_half : __obfh_bsearch_args.count - __obfh_bsearch_half - 1; \
    goto __obfh_bsearch_loop;                                                                                                         \
__obfh_bsearch_found:                                                                                                                 \
    __obfh_bsearch_result = (void *)__obfh_bsearch_middle;                                                                            \
__obfh_bsearch_done:;                                                                                                                 \
    __obfh_bsearch_result;                                                                                                            \
})

// GUI publication precedes invocation, allowing callback reentry.
typedef struct {
    PVOID volatile encoded;
    LONG volatile ready;
} OBFH_GUI_SLOT;
static OBFH_GUI_SLOT obfh_gui_slots[136];
static PVOID volatile obfh_gui_modules[4];
static HMODULE LoadLibraryA_proxy(LPCSTR name);
static FARPROC obfh_find_export(HMODULE module, LPCSTR name, unsigned depth);
#define OBFH_GUI_COUNT 136
#define OBFH_GUI_DRAW(index, salt) OBFH_MIX_B(OBFH_MIX_A((unsigned int)(index) ^ (unsigned int)OBFH_BUILD_SEED ^ (salt)))
#if defined(__x86_64__)
#define OBFH_GUI_WIDTH "q"
#define OBFH_GUI_KEY(index) ((ULONG_PTR)OBFH_GUI_DRAW(index, 0x47554931u) | ((ULONG_PTR)OBFH_GUI_DRAW(index, 0x47554932u) << 32))
#else
#define OBFH_GUI_WIDTH "l"
#define OBFH_GUI_KEY(index) ((ULONG_PTR)OBFH_GUI_DRAW(index, 0x47554931u))
#endif
#define OBFH_GUI_BIAS(index) ((ULONG_PTR)OBFH_GUI_DRAW(index, 0x47554933u))
#define OBFH_GUI_ROTATE(index) (OBFH_GUI_DRAW(index, 0x47554934u) % (sizeof(ULONG_PTR) * 8 - 1) + 1)
#define OBFH_GUI_NAME_KEY(index) ((OBFH_GUI_DRAW(index, 0x47554935u) & 255u) | 1u)

// Native cold-path control avoids expanding CFLOW at every cache check.
static ULONG_PTR obfh_gui_cold(unsigned int module_id, unsigned int index,
                               const unsigned char *encoded_name, size_t length) {
    DWORD saved_error = GetLastError();
    BREAK_STACK_CFLOW;
    PHANTOM_NOP;
    if (module_id >= 4 || index >= OBFH_GUI_COUNT || !length || length > 64)
        ExitProcess(0xe0bf4701u);
    HMODULE module = (HMODULE)InterlockedCompareExchangePointer(&obfh_gui_modules[module_id], NULL, NULL);
    if (!module) {
        char library[9];
        const char *hidden = module_id == 3 ? HIDE_STRING("advapi32") : (module_id == 2 ? HIDE_STRING("kernel32") : (module_id ? HIDE_STRING("gdi32") : HIDE_STRING("user32")));
        size_t n = module_id >= 2 ? 9 : (module_id ? 6 : 7);
        for (size_t i = 0; i < n; ++i)
            library[i] = hidden[i];
        HMODULE loaded = LoadLibraryA_proxy(library);
        if (!loaded)
            ExitProcess(0xe0bf4701u);
        HMODULE previous = (HMODULE)InterlockedCompareExchangePointer(&obfh_gui_modules[module_id], loaded, NULL);
        if (previous) {
            FreeLibrary(loaded);
            module = previous;
        } else
            module = loaded;
    }
    char name[64];
    unsigned char key = (unsigned char)OBFH_GUI_NAME_KEY(index);
    for (size_t i = 0; i < length; ++i)
        name[i] = (char)(encoded_name[i] ^ key);
    if (name[length - 1])
        ExitProcess(0xe0bf4701u);
    FARPROC address = obfh_find_export(module, name, 0);
    if (!address)
        ExitProcess(0xe0bf4701u);
    ULONG_PTR value = (ULONG_PTR)address ^ OBFH_GUI_KEY(index);
    unsigned int rotate = OBFH_GUI_ROTATE(index);
    value = ((value << rotate) | (value >> (sizeof(ULONG_PTR) * 8 - rotate))) + OBFH_GUI_BIAS(index);
    // Zero is a valid encoding; ready is published after the atomic pointer write.
    InterlockedCompareExchangePointer(&obfh_gui_slots[index].encoded, (PVOID)value, NULL);
    InterlockedExchange(&obfh_gui_slots[index].ready, 1);
    SetLastError(saved_error);
    return value;
}
// ============================================================================
// 13. Keyword interception: if / else / break / switch / while / for
// ============================================================================

#if NO_CFLOW != 1
// Each intercepted if inserts its selected template between condition transport stages.
#define if(...) if (({                                                                         \
                        enum { __obfh_if_site = RND(1, 65535) };                               \
                        STACK_PROXY_FUNCTIONS;                                                 \
                        OBFH_FLOW_CONDITION((__VA_ARGS__), __obfh_if_site, BREAK_STACK_CFLOW); \
                    }))

// Keep if separate from its parentheses during macro rescan to avoid another CFLOW layer.
#define else                          \
    else if OBFH_FLOW_ELSE_GUARD () { \
    }                                 \
    else

#define OBFUS_CONDITION_BLOCK(...) OBFH_FLOW_CONDITION((__VA_ARGS__), RND(1, 65535))

// Keep the native break outside the expression so it retains its original target.
#define OBFH_FLOW_BREAK_GUARD()                 \
    (({                                         \
        if (OBFUS_CONDITION_BLOCK(RND(1, 255))) \
            BREAK_STACK_CFLOW;                  \
        1;                                      \
    }))

// A matched conditional consumes the caller's semicolon and preserves dangling else.
#define break                   \
    if OBFH_FLOW_BREAK_GUARD () \
        break;                  \
    else                        \
        (void)0

#define switch(...)                                                                                                  \
    switch (({                                                                                                       \
        __typeof__((__VA_ARGS__)) __obfh_switch_value = (__VA_ARGS__);                                               \
        volatile ULONG_PTR __obfh_switch_shift = (ULONG_PTR)OBFUS_CONDITION_BLOCK(RND(1, 255)) * SALT_SHIFT;         \
        ULONG_PTR __obfh_switch_address = obfh_uintptr_proxy((ULONG_PTR)&__obfh_switch_value ^ __obfh_switch_shift); \
        *(__typeof__(&__obfh_switch_value))(__obfh_switch_address ^ __obfh_switch_shift);                            \
    }))

#define while(...) while (OBFUS_CONDITION_BLOCK((__VA_ARGS__)))

// Protect every iteration and retain randomized bytes in the never-taken arm.
#define for(...)                    \
    for (__VA_ARGS__)               \
        if (0) {                    \
            OBFH_FLOW_DEAD_BYTES(); \
        } else

#endif

// ============================================================================
// 14. Public VM interface and instruction-program templates
// ============================================================================

// Virtualization (instruction programs)
#if VIRT == 1

/* Each expression owns an immutable encoded instruction stream. */
#define OBFH_V_INSTRUCTION(op, d, a, b, imm)                                                                                  \
    (((((unsigned int)(op) * (((__obfh_vkey >> 8) & 255u) | 1u) + (__obfh_vkey & 255u)) & 255u)) | ((unsigned int)(d) << 8) | \
     ((unsigned int)(a) << 10) | ((unsigned int)(b) << 12) | (((unsigned int)(imm)&0x3ffffu) << 14))
#define OBFH_V_OPERATION_0(d, a, b)                                                                                                      \
    OBFH_V_INSTRUCTION(__obfh_voperation < 5                                                                                             \
                           ? OBFH_V_ADD + __obfh_voperation                                                                              \
                           : (__obfh_voperation < 11 ? OBFH_V_COMPARE : (__obfh_voperation == OBFH_VOP_ID ? OBFH_V_MOVE : OBFH_V_TEST)), \
                       d, a, b, 0)
#define OBFH_V_OPERATION_1(d, a)                                                                                             \
    OBFH_V_INSTRUCTION((__obfh_voperation >= 5 && __obfh_voperation != OBFH_VOP_ID) ? OBFH_V_BOOLEAN : OBFH_V_CXOR, d, a, 0, \
                       __obfh_voperation == OBFH_VOP_TRUTH ? 6 : (__obfh_voperation >= 5 ? __obfh_voperation - 5 : 0))
#define OBFH_V_ENCODE(word, pc)                                                                                                \
    (((((unsigned int)(word) >> ((__obfh_vkey % 31u) + 1u)) | ((unsigned int)(word) << (32u - ((__obfh_vkey % 31u) + 1u))))) ^ \
     (__obfh_vkey + (unsigned int)(pc)*0x9e3779b9u))
#define OBFH_V_PROGRAM_0                                                             \
    (const unsigned int[]) {                                                         \
        OBFH_V_ENCODE(OBFH_V_INSTRUCTION(OBFH_V_LOAD_A, __obfh_va, 0, 0, 0), 0),     \
            OBFH_V_ENCODE(OBFH_V_INSTRUCTION(OBFH_V_LOAD_B, __obfh_vb, 0, 0, 0), 1), \
            OBFH_V_ENCODE(OBFH_V_OPERATION_0(__obfh_vc, __obfh_va, __obfh_vb), 2),   \
            OBFH_V_ENCODE(OBFH_V_OPERATION_1(__obfh_vc, __obfh_va), 3),              \
            OBFH_V_ENCODE(OBFH_V_INSTRUCTION(OBFH_V_RETURN, 0, __obfh_vc, 0, 0), 4)  \
    }
#define OBFH_V_PROGRAM_1                                                                   \
    (const unsigned int[]) {                                                               \
        OBFH_V_ENCODE(OBFH_V_INSTRUCTION(OBFH_V_LOAD_B, __obfh_vb, 0, 0, 0), 0),           \
            OBFH_V_ENCODE(OBFH_V_INSTRUCTION(OBFH_V_LOAD_A, __obfh_va, 0, 0, 0), 1),       \
            OBFH_V_ENCODE(OBFH_V_INSTRUCTION(OBFH_V_MOVE, __obfh_vd, __obfh_va, 0, 0), 2), \
            OBFH_V_ENCODE(OBFH_V_OPERATION_0(__obfh_vc, __obfh_vd, __obfh_vb), 3),         \
            OBFH_V_ENCODE(OBFH_V_OPERATION_1(__obfh_vc, __obfh_vd), 4),                    \
            OBFH_V_ENCODE(OBFH_V_INSTRUCTION(OBFH_V_MOVE, __obfh_va, __obfh_vc, 0, 0), 5), \
            OBFH_V_ENCODE(OBFH_V_INSTRUCTION(OBFH_V_RETURN, 0, __obfh_va, 0, 0), 6)        \
    }
#define OBFH_V_PROGRAM_2                                                                                                                \
    (const unsigned int[]) {                                                                                                            \
        OBFH_V_ENCODE(OBFH_V_INSTRUCTION(OBFH_V_LOAD_A, __obfh_va, 0, 0, 0), 0),                                                        \
            OBFH_V_ENCODE(OBFH_V_INSTRUCTION(OBFH_V_LOAD_B, __obfh_vb, 0, 0, 0), 1),                                                    \
            OBFH_V_ENCODE(OBFH_V_INSTRUCTION(OBFH_V_TEST, 0, __obfh_va, 0, 0), 2),                                                      \
            OBFH_V_ENCODE(OBFH_V_INSTRUCTION(OBFH_V_JFLAG, 0, 1, 0, 8), 3),                                                             \
            OBFH_V_ENCODE(OBFH_V_OPERATION_0(__obfh_vc, __obfh_va, __obfh_vb), 4),                                                      \
            OBFH_V_ENCODE(OBFH_V_OPERATION_1(__obfh_vc, __obfh_va), 5), OBFH_V_ENCODE(OBFH_V_INSTRUCTION(OBFH_V_JUMP, 0, 0, 0, 11), 6), \
            OBFH_V_ENCODE(OBFH_V_INSTRUCTION(OBFH_V_CROL, 0, 0, 0, 3), 7),                                                              \
            OBFH_V_ENCODE(OBFH_V_OPERATION_0(__obfh_vd, __obfh_va, __obfh_vb), 8),                                                      \
            OBFH_V_ENCODE(OBFH_V_OPERATION_1(__obfh_vd, __obfh_va), 9),                                                                 \
            OBFH_V_ENCODE(OBFH_V_INSTRUCTION(OBFH_V_MOVE, __obfh_vc, __obfh_vd, 0, 0), 10),                                             \
            OBFH_V_ENCODE(OBFH_V_INSTRUCTION(OBFH_V_RETURN, 0, __obfh_vc, 0, 0), 11)                                                    \
    }
#define OBFH_V_PROGRAM_3                                                             \
    (const unsigned int[]) {                                                         \
        OBFH_V_ENCODE(OBFH_V_INSTRUCTION(OBFH_V_LOAD_A, __obfh_va, 0, 0, 0), 0),     \
            OBFH_V_ENCODE(OBFH_V_INSTRUCTION(OBFH_V_LOAD_B, __obfh_vb, 0, 0, 0), 1), \
            OBFH_V_ENCODE(OBFH_V_INSTRUCTION(OBFH_V_TEST, 0, __obfh_va, 0, 0), 2),   \
            OBFH_V_ENCODE(OBFH_V_INSTRUCTION(OBFH_V_JFLAG, 0, 1, 0, 8), 3),          \
            OBFH_V_ENCODE(OBFH_V_OPERATION_0(__obfh_vc, __obfh_va, __obfh_vb), 4),   \
            OBFH_V_ENCODE(OBFH_V_OPERATION_1(__obfh_vc, __obfh_va), 5),              \
            OBFH_V_ENCODE(OBFH_V_INSTRUCTION(OBFH_V_RETURN, 0, __obfh_vc, 0, 0), 6), \
            OBFH_V_ENCODE(OBFH_V_INSTRUCTION(OBFH_V_CXOR, 0, 0, 0, 5), 7),           \
            OBFH_V_ENCODE(OBFH_V_OPERATION_0(__obfh_vd, __obfh_va, __obfh_vb), 8),   \
            OBFH_V_ENCODE(OBFH_V_OPERATION_1(__obfh_vd, __obfh_va), 9),              \
            OBFH_V_ENCODE(OBFH_V_INSTRUCTION(OBFH_V_RETURN, 0, __obfh_vd, 0, 0), 10) \
    }
#define OBFH_V_PROGRAM_4                                                                    \
    (const unsigned int[]) {                                                                \
        OBFH_V_ENCODE(OBFH_V_INSTRUCTION(OBFH_V_JUMP, 0, 0, 0, 4), 0),                      \
            OBFH_V_ENCODE(OBFH_V_OPERATION_0(__obfh_vc, __obfh_va, __obfh_vb), 1),          \
            OBFH_V_ENCODE(OBFH_V_OPERATION_1(__obfh_vc, __obfh_va), 2),                     \
            OBFH_V_ENCODE(OBFH_V_INSTRUCTION(OBFH_V_RETURN, 0, __obfh_vc, 0, 0), 3),        \
            OBFH_V_ENCODE(OBFH_V_INSTRUCTION(OBFH_V_LOAD_A, __obfh_va, 0, 0, 0), 4),        \
            OBFH_V_ENCODE(OBFH_V_INSTRUCTION(OBFH_V_LOAD_B, __obfh_vb, 0, 0, 0), 5),        \
            OBFH_V_ENCODE(OBFH_V_INSTRUCTION(OBFH_V_TEST, 0, __obfh_va, 0, 0), 6),          \
            OBFH_V_ENCODE(OBFH_V_INSTRUCTION(OBFH_V_JFLAG, 0, 1, 0, 10), 7),                \
            OBFH_V_ENCODE(OBFH_V_INSTRUCTION(OBFH_V_MOVE, __obfh_vd, __obfh_va, 0, 0), 8),  \
            OBFH_V_ENCODE(OBFH_V_INSTRUCTION(OBFH_V_JUMP, 0, 0, 0, 1), 9),                  \
            OBFH_V_ENCODE(OBFH_V_INSTRUCTION(OBFH_V_SWAP, 0, __obfh_va, __obfh_vb, 0), 10), \
            OBFH_V_ENCODE(OBFH_V_INSTRUCTION(OBFH_V_SWAP, 0, __obfh_va, __obfh_vb, 0), 11), \
            OBFH_V_ENCODE(OBFH_V_INSTRUCTION(OBFH_V_JUMP, 0, 0, 0, 1), 12)                  \
    }
#define OBFH_V_PROGRAM_5                                                                                                                \
    (const unsigned int[]) {                                                                                                            \
        OBFH_V_ENCODE(OBFH_V_INSTRUCTION(OBFH_V_LOAD_A, __obfh_va, 0, 0, 0), 0),                                                        \
            OBFH_V_ENCODE(OBFH_V_INSTRUCTION(OBFH_V_LOAD_B, __obfh_vb, 0, 0, 0), 1),                                                    \
            OBFH_V_ENCODE(OBFH_V_INSTRUCTION(OBFH_V_SWAP, 0, __obfh_va, __obfh_vb, 0), 2),                                              \
            OBFH_V_ENCODE(OBFH_V_INSTRUCTION(OBFH_V_TEST, 0, __obfh_vb, 0, 0), 3),                                                      \
            OBFH_V_ENCODE(OBFH_V_INSTRUCTION(OBFH_V_JFLAG, 0, 1, 0, 8), 4),                                                             \
            OBFH_V_ENCODE(OBFH_V_OPERATION_0(__obfh_vc, __obfh_vb, __obfh_va), 5),                                                      \
            OBFH_V_ENCODE(OBFH_V_OPERATION_1(__obfh_vc, __obfh_vb), 6), OBFH_V_ENCODE(OBFH_V_INSTRUCTION(OBFH_V_JUMP, 0, 0, 0, 11), 7), \
            OBFH_V_ENCODE(OBFH_V_OPERATION_0(__obfh_vd, __obfh_vb, __obfh_va), 8),                                                      \
            OBFH_V_ENCODE(OBFH_V_OPERATION_1(__obfh_vd, __obfh_vb), 9),                                                                 \
            OBFH_V_ENCODE(OBFH_V_INSTRUCTION(OBFH_V_MOVE, __obfh_vc, __obfh_vd, 0, 0), 10),                                             \
            OBFH_V_ENCODE(OBFH_V_INSTRUCTION(OBFH_V_RETURN, 0, __obfh_vc, 0, 0), 11)                                                    \
    }
#define OBFH_V_PROGRAM_6                                                                   \
    (const unsigned int[]) {                                                               \
        OBFH_V_ENCODE(OBFH_V_INSTRUCTION(OBFH_V_LOAD_A, __obfh_va, 0, 0, 0), 0),           \
            OBFH_V_ENCODE(OBFH_V_INSTRUCTION(OBFH_V_LOAD_B, __obfh_vb, 0, 0, 0), 1),       \
            OBFH_V_ENCODE(OBFH_V_INSTRUCTION(OBFH_V_CADD, 0, 0, 0, 2), 2),                 \
            OBFH_V_ENCODE(OBFH_V_INSTRUCTION(OBFH_V_CADD, 0, 0, 0, 262143), 3),            \
            OBFH_V_ENCODE(OBFH_V_INSTRUCTION(OBFH_V_JFLAG, 1, 1, 0, 3), 4),                \
            OBFH_V_ENCODE(OBFH_V_INSTRUCTION(OBFH_V_MOVE, __obfh_vd, __obfh_va, 0, 0), 5), \
            OBFH_V_ENCODE(OBFH_V_OPERATION_0(__obfh_vc, __obfh_vd, __obfh_vb), 6),         \
            OBFH_V_ENCODE(OBFH_V_OPERATION_1(__obfh_vc, __obfh_vd), 7),                    \
            OBFH_V_ENCODE(OBFH_V_INSTRUCTION(OBFH_V_RETURN, 0, __obfh_vc, 0, 0), 8)        \
    }
#define OBFH_V_PROGRAM_7                                                                                                                  \
    (const unsigned int[]) {                                                                                                              \
        OBFH_V_ENCODE(OBFH_V_INSTRUCTION(OBFH_V_LOAD_A, __obfh_va, 0, 0, 0), 0),                                                          \
            OBFH_V_ENCODE(OBFH_V_INSTRUCTION(OBFH_V_LOAD_B, __obfh_vb, 0, 0, 0), 1),                                                      \
            OBFH_V_ENCODE(OBFH_V_INSTRUCTION(OBFH_V_TEST, 0, __obfh_va, 0, 0), 2),                                                        \
            OBFH_V_ENCODE(OBFH_V_INSTRUCTION(OBFH_V_JFLAG, 0, 1, 0, 7), 3),                                                               \
            OBFH_V_ENCODE(OBFH_V_INSTRUCTION(OBFH_V_MOVE, __obfh_vd, __obfh_va, 0, 0), 4),                                                \
            OBFH_V_ENCODE(OBFH_V_INSTRUCTION(OBFH_V_CROL, 1, 0, 0, 7), 5), OBFH_V_ENCODE(OBFH_V_INSTRUCTION(OBFH_V_JUMP, 0, 0, 0, 9), 6), \
            OBFH_V_ENCODE(OBFH_V_INSTRUCTION(OBFH_V_MOVE, __obfh_vd, __obfh_va, 0, 0), 7),                                                \
            OBFH_V_ENCODE(OBFH_V_INSTRUCTION(OBFH_V_CXOR, 1, 0, 0, 57), 8),                                                               \
            OBFH_V_ENCODE(OBFH_V_OPERATION_0(__obfh_vc, __obfh_vd, __obfh_vb), 9),                                                        \
            OBFH_V_ENCODE(OBFH_V_OPERATION_1(__obfh_vc, __obfh_vd), 10),                                                                  \
            OBFH_V_ENCODE(OBFH_V_INSTRUCTION(OBFH_V_TEST, 0, __obfh_vc, 0, 0), 11),                                                       \
            OBFH_V_ENCODE(OBFH_V_INSTRUCTION(OBFH_V_JFLAG, 0, 1, 0, 15), 12),                                                             \
            OBFH_V_ENCODE(OBFH_V_INSTRUCTION(OBFH_V_MOVE, __obfh_va, __obfh_vc, 0, 0), 13),                                               \
            OBFH_V_ENCODE(OBFH_V_INSTRUCTION(OBFH_V_RETURN, 0, __obfh_va, 0, 0), 14),                                                     \
            OBFH_V_ENCODE(OBFH_V_INSTRUCTION(OBFH_V_RETURN, 0, __obfh_vc, 0, 0), 15)                                                      \
    }
#define OBFH_V_PROGRAM                                                                \
    __builtin_choose_expr(                                                            \
        __obfh_vvariant == 0, OBFH_V_PROGRAM_0,                                       \
        __builtin_choose_expr(                                                        \
            __obfh_vvariant == 1, OBFH_V_PROGRAM_1,                                   \
            __builtin_choose_expr(                                                    \
                __obfh_vvariant == 2, OBFH_V_PROGRAM_2,                               \
                __builtin_choose_expr(                                                \
                    __obfh_vvariant == 3, OBFH_V_PROGRAM_3,                           \
                    __builtin_choose_expr(                                            \
                        __obfh_vvariant == 4, OBFH_V_PROGRAM_4,                       \
                        __builtin_choose_expr(__obfh_vvariant == 5, OBFH_V_PROGRAM_5, \
                                              __builtin_choose_expr(__obfh_vvariant == 6, OBFH_V_PROGRAM_6, OBFH_V_PROGRAM_7)))))))
#define OBFH_VM_EXEC(executor, operation, value_a, value_b, floating)                                                                     \
    ({                                                                                                                                    \
        enum {                                                                                                                            \
            __obfh_vsite = __COUNTER__,                                                                                                   \
            __obfh_voperation = (operation),                                                                                              \
            __obfh_vkey = OBFH_MIX_A(OBFH_JUNK_WORD ^ __obfh_vsite ^ (unsigned int)OBFH_BUILD_SEED),                                      \
            __obfh_vvariant = RND(0, 7),                                                                                                  \
            __obfh_va = __obfh_vkey & 3u,                                                                                                 \
            __obfh_vb = (__obfh_va + ((__obfh_vkey & 4u) ? 3u : 1u)) & 3u,                                                                \
            __obfh_vc = (__obfh_va + 2u) & 3u,                                                                                            \
            __obfh_vd = (__obfh_va + ((__obfh_vkey & 4u) ? 1u : 3u)) & 3u                                                                 \
        };                                                                                                                                \
        const unsigned int *__obfh_vprogram = OBFH_V_PROGRAM;                                                                             \
        enum {                                                                                                                            \
            __obfh_vlength =                                                                                                              \
                __obfh_vvariant == 0                                                                                                      \
                    ? 5                                                                                                                   \
                    : (__obfh_vvariant == 1                                                                                               \
                           ? 7                                                                                                            \
                           : (__obfh_vvariant == 2                                                                                        \
                                  ? 12                                                                                                    \
                                  : (__obfh_vvariant == 3                                                                                 \
                                         ? 11                                                                                             \
                                         : (__obfh_vvariant == 4 ? 13 : (__obfh_vvariant == 5 ? 12 : (__obfh_vvariant == 6 ? 9 : 16)))))) \
        };                                                                                                                                \
        executor(__obfh_vprogram, __obfh_vlength, __obfh_vkey, OBFH_VM_OPERAND(value_a, SALT_NUM1, floating),                             \
                 OBFH_VM_OPERAND(value_b, SALT_NUM2, floating));                                                                          \
    })
#define OBFH_VM_OPERAND(value, salt, floating) obfh_vm_encode(value, salt, (floating) | (RND(1, 2147483647u) << 1))
#define VM_ADD(num1, num2) (long)OBFH_VM_EXEC(Obfh_VirtualMachine, OBFH_VOP_ADD, (long double)(num1), (long double)(num2), 0u)
#define VM_SUB(num1, num2) (long)OBFH_VM_EXEC(Obfh_VirtualMachine, OBFH_VOP_SUB, (long double)(num1), (long double)(num2), 0u)
#define VM_MUL(num1, num2) (long)OBFH_VM_EXEC(Obfh_VirtualMachine, OBFH_VOP_MUL, (long double)(num1), (long double)(num2), 0u)
#define VM_DIV(num1, num2) (long)OBFH_VM_EXEC(Obfh_VirtualMachine, OBFH_VOP_DIV, (long double)(num1), (long double)(num2), 0u)
#define VM_MOD(num1, num2) (long)OBFH_VM_EXEC(Obfh_VirtualMachine, OBFH_VOP_MOD, (long double)(num1), (long double)(num2), 0u)
#define VM_EQU(num1, num2) (long)OBFH_VM_EXEC(Obfh_VirtualMachine, OBFH_VOP_EQ, (long double)(num1), (long double)(num2), 0u)
#define VM_NEQ(num1, num2) (long)OBFH_VM_EXEC(Obfh_VirtualMachine, OBFH_VOP_NE, (long double)(num1), (long double)(num2), 0u)
#define VM_LSS(num1, num2) (long)OBFH_VM_EXEC(Obfh_VirtualMachine, OBFH_VOP_LT, (long double)(num1), (long double)(num2), 0u)
#define VM_GTR(num1, num2) (long)OBFH_VM_EXEC(Obfh_VirtualMachine, OBFH_VOP_GT, (long double)(num1), (long double)(num2), 0u)
#define VM_LEQ(num1, num2) (long)OBFH_VM_EXEC(Obfh_VirtualMachine, OBFH_VOP_LE, (long double)(num1), (long double)(num2), 0u)
#define VM_GEQ(num1, num2) (long)OBFH_VM_EXEC(Obfh_VirtualMachine, OBFH_VOP_GE, (long double)(num1), (long double)(num2), 0u)
#define VM_ADD_DBL(num1, num2) OBFH_VM_EXEC(Obfh_VirtualMachine, OBFH_VOP_ADD, (double)(num1), (double)(num2), 1u)
#define VM_SUB_DBL(num1, num2) OBFH_VM_EXEC(Obfh_VirtualMachine, OBFH_VOP_SUB, (double)(num1), (double)(num2), 1u)
#define VM_MUL_DBL(num1, num2) OBFH_VM_EXEC(Obfh_VirtualMachine, OBFH_VOP_MUL, (double)(num1), (double)(num2), 1u)
#define VM_DIV_DBL(num1, num2) OBFH_VM_EXEC(Obfh_VirtualMachine, OBFH_VOP_DIV, (double)(num1), (double)(num2), 1u)
#define VM_LSS_DBL(num1, num2) (long)OBFH_VM_EXEC(Obfh_VirtualMachine, OBFH_VOP_LT, (double)(num1), (double)(num2), 1u)
#define VM_GTR_DBL(num1, num2) (long)OBFH_VM_EXEC(Obfh_VirtualMachine, OBFH_VOP_GT, (double)(num1), (double)(num2), 1u)
#define VM_OBF_INT(num1) (long)OBFH_VM_EXEC(Obfh_VirtualMachine, OBFH_VOP_ID, (long double)(num1), (long double)0, 0u)
#define VM_OBF_DBL(num1) OBFH_VM_EXEC(Obfh_VirtualMachine, OBFH_VOP_ID, (long double)(num1), (long double)0, 1u)
#define VM_IF(condition) if ((int)OBFH_VM_EXEC(Obfh_VirtualMachine, OBFH_VOP_TRUTH, (long double)!!(condition), (long double)0, 1u))
#define VM_ELSE_IF(condition) \
    else if ((int)OBFH_VM_EXEC(Obfh_VirtualMachine, OBFH_VOP_TRUTH, (long double)!!(condition), (long double)0, 1u))
#define VM_ELSE else if ((int)OBFH_VM_EXEC(Obfh_VirtualMachine, OBFH_VOP_TRUTH, (long double)!!obfh_condition_true(), (long double)0, 1u))
#endif

// ============================================================================
// 15. Console proxies, inline comparisons and PE export lookup
// ============================================================================

// Caller-owned storage keeps the mask valid and avoids shared-buffer races.
static char *getCharMask(int count, char *mask, size_t capacity) OBFH_CODE_SECTION_ATTRIBUTE {
    BREAK_STACK_CFLOW;
    if (!mask || !capacity || count < 0 || (size_t)count > (capacity - 1) / 2)
        return NULL;
    int i = (((_1 * _5) - _4) + _1) - _2;
    BREAK_STACK_CFLOW;
    PHANTOM_NOP;
    char *ptr = mask;
    for (i = _0; i < count; ++i) {
        *ptr++ = '%';
        *ptr++ = _c;
    }
    *ptr = _0;
    BREAK_STACK_CFLOW;
    return mask;
}

// WriteConsoleA
static BOOL WriteConsoleA_proxy(HANDLE hConsoleOutput, const void *lpBuffer, DWORD nNumberOfCharsToWrite, LPDWORD lpNumberOfCharsWritten, LPVOID lpReserved) OBFH_CODE_SECTION_ATTRIBUTE {
    BREAK_STACK_CFLOW;
    PHANTOM_NOP;
    return WriteConsoleA(hConsoleOutput, lpBuffer, nNumberOfCharsToWrite, lpNumberOfCharsWritten, lpReserved);
}
#define WriteConsoleA(...) WriteConsoleA_proxy(__VA_ARGS__)

static HANDLE GetStdHandle_proxy(DWORD nStdHandle) OBFH_CODE_SECTION_ATTRIBUTE {
    BREAK_STACK_CFLOW;
    PHANTOM_NOP;
    return GetStdHandle(obfh_int_proxy(nStdHandle));
}
#define GetStdHandle(...) GetStdHandle_proxy(__VA_ARGS__)

static HMODULE GetModuleHandleA_proxy(LPCSTR lpModuleName) OBFH_CODE_SECTION_ATTRIBUTE {
    BREAK_STACK_CFLOW;
    PHANTOM_NOP;
    return GetModuleHandleA(lpModuleName);
}
#define GetModuleHandleA(...) GetModuleHandleA_proxy(__VA_ARGS__)

#define strcmp_custom(...) ({                                                                                         \
    __label__ __obfh_strcmp_loop, __obfh_strcmp_advance, __obfh_strcmp_equal, __obfh_strcmp_done;                     \
    struct {                                                                                                          \
        const char *__obfh_strcmp_str1;                                                                               \
        const char *__obfh_strcmp_str2;                                                                               \
    } __obfh_strcmp_args = {__VA_ARGS__};                                                                             \
    int __obfh_strcmp_result;                                                                                         \
    int __obfh_strcmp_a;                                                                                              \
    int __obfh_strcmp_b;                                                                                              \
    BREAK_STACK_CFLOW;                                                                                                \
    PHANTOM_NOP;                                                                                                      \
__obfh_strcmp_loop:                                                                                                   \
    __obfh_strcmp_a = (unsigned char)*__obfh_strcmp_args.__obfh_strcmp_str1;                                          \
    __obfh_strcmp_b = (unsigned char)*__obfh_strcmp_args.__obfh_strcmp_str2;                                          \
    OBFH_INLINE_EXIT(!(__obfh_strcmp_a || __obfh_strcmp_b), __obfh_strcmp_equal);                                     \
    __obfh_strcmp_a = obfh_int_proxy(__obfh_strcmp_a);                                                                \
    __obfh_strcmp_b = obfh_int_proxy(__obfh_strcmp_b);                                                                \
    OBFH_INLINE_EXIT(__obfh_strcmp_a == __obfh_strcmp_b, __obfh_strcmp_advance);                                      \
    __obfh_strcmp_result = obfh_int_proxy((__obfh_strcmp_a > __obfh_strcmp_b) - (__obfh_strcmp_a < __obfh_strcmp_b)); \
    goto __obfh_strcmp_done;                                                                                          \
__obfh_strcmp_advance:                                                                                                \
    __obfh_strcmp_args.__obfh_strcmp_str1 += obfh_int_proxy(_1);                                                      \
    __obfh_strcmp_args.__obfh_strcmp_str2 += obfh_int_proxy(_2 - _1);                                                 \
    goto __obfh_strcmp_loop;                                                                                          \
__obfh_strcmp_equal:                                                                                                  \
    __obfh_strcmp_result = _0;                                                                                        \
__obfh_strcmp_done:;                                                                                                  \
    __obfh_strcmp_result;                                                                                             \
})
#define strcmp(...) strcmp_custom(__VA_ARGS__)

#define strlen_custom(...) ({                                                \
    __label__ __obfh_strlen_loop, __obfh_strlen_done;                        \
    struct {                                                                 \
        const char *str;                                                     \
    } __obfh_strlen_args = {__VA_ARGS__};                                    \
    int __obfh_strlen_step;                                                  \
    size_t __obfh_strlen_length = _0;                                        \
    BREAK_STACK_CFLOW;                                                       \
    PHANTOM_NOP;                                                             \
__obfh_strlen_loop:                                                          \
    OBFH_INLINE_EXIT(!*__obfh_strlen_args.str, __obfh_strlen_done);          \
    __obfh_strlen_step = obfh_int_proxy(_1);                                 \
    __obfh_strlen_length += __obfh_strlen_step;                              \
    __obfh_strlen_args.str += __obfh_strlen_step;                            \
    goto __obfh_strlen_loop;                                                 \
__obfh_strlen_done:                                                          \
    (size_t) obfh_uintptr_proxy(__obfh_strlen_length + (RND(0, 1000) * _0)); \
})
#define strlen(...) strlen_custom(__VA_ARGS__)

// Forward declaration for forwarded-export module loading.
static HMODULE LoadLibraryA_proxy(LPCSTR lpLibFileName);

// Bounded string scan used by the export parser.
static const char *obfh_find_zero(const void *buffer, size_t count) {
    BREAK_STACK_CFLOW;
    const char *bytes = buffer;
    PHANTOM_NOP;
    for (size_t i = _0; i < count; ++i)
        if (bytes[i] == _0)
            return bytes + i;
    BREAK_STACK_CFLOW;
    return NULL;
}
// Check an RVA range against the loaded image size.
static int obfh_image_range(DWORD size, DWORD rva, size_t length) {
    PHANTOM_NOP;
    return rva <= size && length <= (size_t)(size - rva);
}
// GetProcAddress: custom PE export lookup, including ordinals and forwarders.
// Windows may bind a forwarded EAT entry to code outside the source image.
// Accept it only from a loader-owned module and an executable image page.
static FARPROC obfh_bound_export(HMODULE source, DWORD rva) {
    HMODULE owner = NULL;
    if (!GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                            (LPCSTR)source, &owner) ||
        owner != source)
        return NULL;
    ULONG_PTR target = (ULONG_PTR)source + (ULONG_PTR)rva;
    MEMORY_BASIC_INFORMATION page;
    if (!VirtualQuery((LPCVOID)target, &page, sizeof(page)) || page.Type != MEM_IMAGE ||
        (page.Protect & (PAGE_GUARD | PAGE_NOACCESS)) ||
        !(page.Protect & (PAGE_EXECUTE | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY)))
        return NULL;
    return (FARPROC)target;
}

static FARPROC obfh_find_export(HMODULE hModule, LPCSTR lpProcName, unsigned int depth) OBFH_CODE_SECTION_ATTRIBUTE {
    BREAK_STACK_CFLOW;
    BREAK_STACK_CFLOW;
    obfh_junk_func_args(RND(0, 885));
    BREAK_STACK_CFLOW;
    if (!hModule || !lpProcName || depth >= 32 || ((ULONG_PTR)hModule & 3))
        return NULL;
    // Module handles must designate OS-loaded modules, as required by WinAPI.
    BYTE *base = (BYTE *)hModule;
    PIMAGE_DOS_HEADER dos = (PIMAGE_DOS_HEADER)base;
    if (dos->e_magic != IMAGE_DOS_SIGNATURE || dos->e_lfanew < sizeof(*dos))
        return NULL;
    PIMAGE_NT_HEADERS nt = (PIMAGE_NT_HEADERS)(base + dos->e_lfanew);
    if (nt->Signature != IMAGE_NT_SIGNATURE || nt->OptionalHeader.NumberOfRvaAndSizes <= IMAGE_DIRECTORY_ENTRY_EXPORT)
        return NULL;
    DWORD size = nt->OptionalHeader.SizeOfImage;
    IMAGE_DATA_DIRECTORY exports = nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];
    if (!exports.VirtualAddress || !obfh_image_range(size, exports.VirtualAddress, exports.Size) ||
        exports.Size < sizeof(IMAGE_EXPORT_DIRECTORY))
        return NULL;
    PIMAGE_EXPORT_DIRECTORY table = (PIMAGE_EXPORT_DIRECTORY)(base + exports.VirtualAddress);
    if (table->NumberOfNames > size / sizeof(DWORD) || table->NumberOfNames > size / sizeof(WORD) ||
        table->NumberOfFunctions > size / sizeof(DWORD))
        return NULL;
    if (!obfh_image_range(size, table->AddressOfNames, table->NumberOfNames * sizeof(DWORD)) ||
        !obfh_image_range(size, table->AddressOfNameOrdinals, table->NumberOfNames * sizeof(WORD)) ||
        !obfh_image_range(size, table->AddressOfFunctions, table->NumberOfFunctions * sizeof(DWORD)))
        return NULL;
    DWORD *names = (DWORD *)(base + table->AddressOfNames);
    WORD *ordinals = (WORD *)(base + table->AddressOfNameOrdinals);
    DWORD *functions = (DWORD *)(base + table->AddressOfFunctions);
    DWORD index = table->NumberOfFunctions;
    PHANTOM_NOP;
    if ((ULONG_PTR)lpProcName <= 0xffff) {
        DWORD ordinal = (DWORD)(ULONG_PTR)lpProcName;
        if (ordinal < table->Base || ordinal - table->Base >= table->NumberOfFunctions)
            return NULL;
        index = ordinal - table->Base;
    } else {
        for (DWORD i = 0; i < table->NumberOfNames; ++i) {
            if (!obfh_image_range(size, names[i], 1) || !obfh_find_zero(base + names[i], size - names[i]))
                return NULL;
            if (strcmp(lpProcName, (const char *)(base + names[i])) == 0) {
                index = ordinals[i];
                break;
            }
        }
    }
    BREAK_STACK_CFLOW;
    if (index >= table->NumberOfFunctions)
        return NULL;
    DWORD rva = functions[index];
    if (!rva)
        return NULL;
    if (!obfh_image_range(size, rva, 1))
        return obfh_bound_export(hModule, rva);
    if (rva >= exports.VirtualAddress && rva - exports.VirtualAddress < exports.Size) {
        // Forwarded export: MODULE.symbol or MODULE.#ordinal.
        const char *forward = (const char *)(base + rva);
        size_t limit = exports.Size - (rva - exports.VirtualAddress);
        const char *end = obfh_find_zero(forward, limit);
        if (!end)
            return NULL;
        const char *dot = NULL;
        for (const char *cursor = forward; cursor < end; ++cursor)
            if (*cursor == '.')
                dot = cursor;
        if (!dot || dot == forward || dot + 1 == end)
            return NULL;
        char moduleName[MAX_PATH];
        size_t length = dot - forward;
        if (length >= sizeof(moduleName))
            return NULL;
        for (size_t i = _0; i < length; ++i)
            moduleName[i] = forward[i];
        moduleName[length] = 0;
        HMODULE target = GetModuleHandleA(moduleName);
        if (!target)
            target = LoadLibraryA_proxy(moduleName);
        if (!target)
            return NULL;
        const char *symbol = dot + 1;
        if (*symbol == '#') {
            unsigned int ordinal = 0;
            if (++symbol == end)
                return NULL;
            for (; symbol < end; ++symbol) {
                if (*symbol < '0' || *symbol > '9' || ordinal > (65535u - (*symbol - '0')) / 10)
                    return NULL;
                ordinal = ordinal * 10 + (*symbol - '0');
            }
            if (!ordinal)
                return NULL;
            return obfh_find_export(target, (LPCSTR)(ULONG_PTR)ordinal, depth + 1);
        }
        return obfh_find_export(target, symbol, depth + 1);
    }
    return (FARPROC)(base + rva);
}
#define GetProcAddress_custom(...) ({                                                                                                      \
    __label__ __obfh_getprocaddress_done;                                                                                                  \
    struct {                                                                                                                               \
        HMODULE hModule;                                                                                                                   \
        LPCSTR lpProcName;                                                                                                                 \
    } __obfh_getprocaddress_args = {__VA_ARGS__};                                                                                          \
    FARPROC __obfh_getprocaddress_output;                                                                                                  \
    FARPROC __obfh_getprocaddress_result = obfh_find_export(__obfh_getprocaddress_args.hModule, __obfh_getprocaddress_args.lpProcName, 0); \
    PHANTOM_NOP;                                                                                                                           \
    BREAK_STACK_CFLOW;                                                                                                                     \
    {                                                                                                                                      \
        __obfh_getprocaddress_output = (__obfh_getprocaddress_result);                                                                     \
        goto __obfh_getprocaddress_done;                                                                                                   \
    }                                                                                                                                      \
__obfh_getprocaddress_done:                                                                                                                \
    __obfh_getprocaddress_output;                                                                                                          \
})
#define GetProcAddress(...) GetProcAddress_custom(__VA_ARGS__)

// ============================================================================
// 16. Module loading and loader proxy chain
// ============================================================================

// LoadLibraryA: dynamic loader resolution and proxy chain.
static HMODULE LoadLibraryA_0(LPCSTR lpLibFileName) OBFH_CODE_SECTION_ATTRIBUTE {
    BREAK_STACK_CFLOW;
    PHANTOM_NOP;
    switch (_0) {
        case 1:
            __obfh_asm__(".byte 0x74;");
            break;
        case 0: {
            BREAK_STACK_CFLOW;
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
                if (!kernel)
                    return NULL;
                char charL = _L;
                obfh_junk_func_args(_0 + RND(1, 5));
                BREAK_STACK_CFLOW;
                format = getCharMask(_4, mask, sizeof mask);
                sprintf(funcName, format, obfh_int_proxy(charL), obfh_int_proxy(_o), obfh_int_proxy(_a), obfh_int_proxy(_d));
                char tail[] = {_L, _i, _b, _r, _a, _r, _y, _A, _0};
                for (int i = _0; i <= _8; ++i)
                    funcName[_4 + i] = tail[i];
                loader = (LoadLibraryAFunc)GetProcAddress(kernel, funcName);
                if (loader)
                    InterlockedCompareExchangePointer(&cachedLoader, (PVOID)loader, NULL);
            }
#if MEM_CLEANER__JUST_FOR_FUN
            SIZE_T value = (SIZE_T)(LONG_PTR)obfh_int_proxy(_1 * -1);
            SetProcessWorkingSetSize(GetCurrentProcess(), value, value);
#endif
            if (loader) {
                BREAK_STACK_CFLOW;
                return loader(lpLibFileName);
            }
            return NULL;
        }
    }
    return NULL;
}

static HMODULE LoadLibraryA_1(LPCSTR lpLibFileName) OBFH_CODE_SECTION_ATTRIBUTE {
    BREAK_STACK_CFLOW;
    PHANTOM_NOP;
    return LoadLibraryA_0((LPCSTR)lpLibFileName);
}

static HMODULE LoadLibraryA_2(LPCSTR lpLibFileName) {
    BREAK_STACK_CFLOW;
    PHANTOM_NOP;
    return LoadLibraryA_1((LPCSTR)lpLibFileName);
}

static HMODULE LoadLibraryA_3(LPCSTR lpLibFileName) OBFH_CODE_SECTION_ATTRIBUTE {
    BREAK_STACK_CFLOW;
    PHANTOM_NOP;
    return LoadLibraryA_2((LPCSTR)lpLibFileName);
}

static HMODULE LoadLibraryA_4(LPCSTR lpLibFileName) {
    BREAK_STACK_CFLOW;
    PHANTOM_NOP;
    return LoadLibraryA_3((LPCSTR)lpLibFileName);
}

static HMODULE LoadLibraryA_5(LPCSTR lpLibFileName) OBFH_CODE_SECTION_ATTRIBUTE {
    BREAK_STACK_CFLOW;
    PHANTOM_NOP;
    return LoadLibraryA_4((LPCSTR)lpLibFileName);
}

static HMODULE LoadLibraryA_proxy(LPCSTR lpLibFileName) {
    BREAK_STACK_CFLOW;
    PHANTOM_NOP;
    return LoadLibraryA_5((LPCSTR)lpLibFileName);
}
#define LoadLibraryA(...) LoadLibraryA_proxy(__VA_ARGS__)

// ============================================================================
// 17. Anti-debug probes and response routing
// ============================================================================
// Probes collect evidence; response routing is separate from the call site.
#if NO_ANTIDEBUG != 1

#if ANTIDEBUG_V2 == 1
// The worker owns the duplicated handle, including after a failed/timed-out wait.
static DWORD WINAPI obfh_ad_register_worker(void *argument) {
    HANDLE thread = (HANDLE)argument;
    DWORD detected = 0;
    BREAK_STACK_CFLOW;
    PHANTOM_NOP;
    if (SuspendThread(thread) != (DWORD)-1) {
        CONTEXT context = {0};
        context.ContextFlags = CONTEXT_DEBUG_REGISTERS;
        if (GetThreadContext(thread, &context)) {
            // Only enabled local/global breakpoints count. DR6 and reserved DR7
            // bits are not evidence; reading context does not erase breakpoints.
            detected = (context.Dr7 & 0xffu) != 0;
        }
        ResumeThread(thread);
    }
    CloseHandle(thread);
    return detected;
}

static int obfh_ad_register_probe(void) {
    HANDLE target = NULL;
    BREAK_STACK_CFLOW;
    if (!DuplicateHandle(GetCurrentProcess(), GetCurrentThread(), GetCurrentProcess(),
                         &target, 0, FALSE, DUPLICATE_SAME_ACCESS))
        return 0;
    HANDLE worker = CreateThread(NULL, 0, obfh_ad_register_worker, target, 0, NULL);
    PHANTOM_NOP;
    if (!worker) {
        CloseHandle(target);
        return 0;
    }
    DWORD detected = 0;
    DWORD wait = WaitForSingleObject(worker, 5000);
    int result = wait == WAIT_OBJECT_0 && GetExitCodeThread(worker, &detected) && detected != 0;
    CloseHandle(worker);
    return result;
}
#endif

static int obfh_ad_process_probe(void) {
    typedef BOOL(WINAPI * ObfhDebuggerCheck)(void);
    BREAK_STACK_CFLOW;
    // Caller-owned hidden strings remain alive through the export lookup.
    HMODULE kernel = GetModuleHandleA(HIDE_STRING("kernel32"));
    BREAK_STACK_CFLOW;
    ObfhDebuggerCheck check = kernel ? (ObfhDebuggerCheck)GetProcAddress(kernel, HIDE_STRING("IsDebuggerPresent")) : NULL;
    PHANTOM_NOP;
    if (check)
        return check() != FALSE;
    // A resolver failure must not silently disable the base check.
    ULONG_PTR peb;
#if defined(__x86_64__)
    __obfh_asm__("movq %%gs:0x60, %0"
                 : "=r"(peb));
#else
    __obfh_asm__("movl %%fs:0x30, %0"
                 : "=r"(peb));
#endif
    return peb && *((volatile unsigned char *)peb + 2) != 0;
}

static int IsDebuggerPresent_proxy(void) OBFH_CODE_SECTION_ATTRIBUTE {
    int detected = obfh_ad_process_probe();
    BREAK_STACK_CFLOW;
    PHANTOM_NOP;
#if ANTIDEBUG_V2 == 1
    if (!detected)
        detected = obfh_ad_register_probe();
#endif
    return detected;
}

// Live paths use defined unsigned arithmetic and leave SP untouched. Random
// instructions come from the shared pool and reside on its skipped paths.
static void obfh_ad_sink_a(unsigned int initial) OBFH_CODE_SECTION_ATTRIBUTE {
    volatile unsigned int state = initial | 1u;
    PHANTOM_NOP;
    for (;;) {
        BREAK_STACK_CFLOW;
        unsigned int next = state ^ RND(1, 2147483646);
        state = ((next << 7) | (next >> 25)) + RND(1, 65535);
    }
}

static void obfh_ad_sink_b(unsigned int initial) OBFH_CODE_SECTION_ATTRIBUTE {
    volatile unsigned int state = initial;
    PHANTOM_NOP;
    for (;;) {
        state = state * (RND(1, 32767) * 2u + 1u) + RND(1, 65535);
        BREAK_STACK_CFLOW;
        state ^= state >> 13;
    }
}

static void obfh_ad_react(unsigned int nonce, unsigned int route) OBFH_CODE_SECTION_ATTRIBUTE {
    typedef void (*ObfhResponse)(unsigned int);
    ObfhResponse volatile response = (route & 1u) ? obfh_ad_sink_a : obfh_ad_sink_b;
    PHANTOM_NOP;
    BREAK_STACK_CFLOW;
    response(nonce ^ RND(1, 2147483646));
}

#define ANTI_DEBUG                                        \
    do {                                                  \
        BREAK_STACK_CFLOW;                                \
        if (IsDebuggerPresent_proxy())                    \
            obfh_ad_react(RND(1, 2147483646), RND(0, 1)); \
    } while (0)
#else
#define ANTI_DEBUG \
    do {           \
    } while (0)
#endif

// ============================================================================
// 18. CRT resolution, formatted output and compact call adapters
// ============================================================================

// CRT module name, copied within the hidden string's lifetime.
static char *getStdLibName_proxy(char *name, size_t capacity) {
    BREAK_STACK_CFLOW;
    if (!name || capacity < sizeof("msvcrt"))
        return NULL;
    const char *hidden = HIDE_STRING("msvcrt");
    PHANTOM_NOP;
    OBFH_CRT_NAME_ORDER(
        (name[0] = hidden[0], name[1] = hidden[1], name[2] = hidden[2], name[3] = hidden[3], name[4] = hidden[4], name[5] = hidden[5], name[6] = hidden[6]),
        (name[6] = hidden[6], name[5] = hidden[5], name[4] = hidden[4], name[3] = hidden[3], name[2] = hidden[2], name[1] = hidden[1], name[0] = hidden[0]));
    return name;
}

// Resolve through the custom loader/export chain once, then keep its DLL alive.
static FARPROC obfh_crt_resolve(const char *name) {
    BREAK_STACK_CFLOW;
    FARPROC cached = obfh_crt_cached(name);
    if (cached)
        return cached;
    static PVOID volatile cachedModule;
    HMODULE module = (HMODULE)InterlockedCompareExchangePointer(&cachedModule, NULL, NULL);
    char moduleName[11];
    PHANTOM_NOP;
    if (!module) {
        HMODULE loaded = LoadLibraryA_proxy(getStdLibName_proxy(moduleName, sizeof moduleName));
        if (!loaded)
            return NULL;
        HMODULE previous = (HMODULE)InterlockedCompareExchangePointer(&cachedModule, loaded, NULL);
        if (previous) {
            FreeLibrary(loaded);
            module = previous;
        } else
            module = loaded;
    }
    FARPROC function = GetProcAddress(module, name);
    obfh_crt_publish(name, function);
    return function;
}

// A count conversion writes to user memory and must not run in a sizing pass.
static int obfh_format_has_count(const char *format) OBFH_CODE_SECTION_ATTRIBUTE {
    BREAK_STACK_CFLOW;
    PHANTOM_NOP;
    for (const char *cursor = format; *cursor; ++cursor) {
        if (*cursor != '%')
            continue;
        ++cursor;
        if (*cursor == '%')
            continue;
        for (; *cursor; ++cursor) {
            if (*cursor == _n)
                return _1;
            // Stop at conversions, preserving flags, widths and length modifiers.
            if (*cursor == 'd' || *cursor == 'i' || *cursor == 'o' ||
                *cursor == 'u' || *cursor == 'x' || *cursor == 'X' ||
                *cursor == 'f' || *cursor == 'F' || *cursor == 'e' ||
                *cursor == 'E' || *cursor == 'g' || *cursor == 'G' ||
                *cursor == 'a' || *cursor == 'A' || *cursor == 'c' ||
                *cursor == 'C' || *cursor == 's' || *cursor == 'S' ||
                *cursor == 'p' || *cursor == '%')
                break;
        }
        if (!*cursor)
            break;
    }
    return _0;
}

// printf
static int obfh_printf_variadic(int junk, const char *format, ...) {
    BREAK_STACK_CFLOW;
    va_list args;
    obfh_junk_func_args(RND(0, 1000) + junk);
    HANDLE console = (HANDLE)obfh_uintptr_proxy((ULONG_PTR)GetStdHandle(obfh_int_proxy(STD_OUTPUT_HANDLE)));
    obfh_junk_func_args((int)((ULONG_PTR)console & 0x3fffffff) + junk);
    va_start(args, format);
    char functionName[8];
    OBFH_CRT_NAME_ORDER(
        (functionName[0] = _v, functionName[1] = _p, functionName[2] = _r, functionName[3] = _i, functionName[4] = _n, functionName[5] = _t, functionName[6] = _f, functionName[7] = _0),
        (functionName[7] = _0, functionName[6] = _f, functionName[5] = _t, functionName[4] = _n, functionName[3] = _i, functionName[2] = _r, functionName[1] = _p, functionName[0] = _v));
    int result;
    DWORD mode;
    PHANTOM_NOP;
    if (GetConsoleMode(console, &mode) && !obfh_format_has_count(format)) {
        char countName[11];
        OBFH_CRT_NAME_ORDER(
            (countName[0] = '_', countName[1] = _v, countName[2] = _s, countName[3] = _c, countName[4] = _p, countName[5] = _r, countName[6] = _i, countName[7] = _n, countName[8] = _t, countName[9] = _f, countName[10] = _0),
            (countName[10] = _0, countName[9] = _f, countName[8] = _t, countName[7] = _n, countName[6] = _i, countName[5] = _r, countName[4] = _p, countName[3] = _c, countName[2] = _s, countName[1] = _v, countName[0] = '_'));
        va_list countArgs;
        va_copy(countArgs, args);
        int length = ((int (*)(const char *, va_list))obfh_crt_resolve(countName))(format, countArgs);
        va_end(countArgs);
        char *buffer = length >= 0 ? malloc((size_t)length + 1) : NULL;
        if (buffer) {
            result = vsnprintf(buffer, (size_t)length + 1, format, args);
            DWORD written = 0;
            obfh_junk_func_args(RND(0, 1000) + junk);
            if (result >= 0 && (!WriteConsoleA(console, buffer, (DWORD)obfh_uintptr_proxy((ULONG_PTR)result), &written, NULL) || written != (DWORD)result))
                result = -1;
            free(buffer);
        } else
            result = -1;
    } else {
        result = ((int (*)(const char *, va_list))obfh_crt_resolve(functionName))(format, args);
    }
    va_end(args);
    return result;
}
#define printf_custom(...) obfh_printf_variadic(__VA_ARGS__)
#define printf(...)                                                                         \
    ({                                                                                      \
        int __obfh_printf_result;                                                           \
        do {                                                                                \
            BREAK_STACK_CFLOW;                                                              \
            obfh_junk_func_args((RND(0, 1000) * 3) < _0);                                   \
            __obfh_printf_result = printf_custom(RND(0, 1000), __VA_ARGS__);                \
        } while (_0 > ((unsigned long long)RND(0, 100000000000) * (unsigned char)_2) + 82); \
        __obfh_printf_result;                                                               \
    })

// Reuse the CRT resolver without a formatting pass or an extra proxy function.
#define puts(string) ({                                                                                                                      \
    const char *__obfh_puts_string = (string);                                                                                               \
    char __obfh_puts_name[5];                                                                                                                \
    OBFH_CRT_NAME_ORDER(                                                                                                                     \
        (__obfh_puts_name[0] = _p, __obfh_puts_name[1] = _u, __obfh_puts_name[2] = _t, __obfh_puts_name[3] = _s, __obfh_puts_name[4] = _0),  \
        (__obfh_puts_name[4] = _0, __obfh_puts_name[3] = _s, __obfh_puts_name[2] = _t, __obfh_puts_name[1] = _u, __obfh_puts_name[0] = _p)); \
    ((int (*)(const char *))obfh_crt_resolve(__obfh_puts_name))(__obfh_puts_string);                                                         \
})

// Build the name in caller-owned storage, then invoke the resolved typed function.
#define OBFH_CRT_CALL(name_builder, function_type, ...) ({                         \
    BREAK_STACK_CFLOW;                                                             \
    char __obfh_crt_name[32];                                                      \
    STACK_PROXY_FUNCTIONS;                                                         \
    ((function_type)obfh_crt_resolve(name_builder(__obfh_crt_name)))(__VA_ARGS__); \
})

static char *getScanfName_proxy(char *name) {
    BREAK_STACK_CFLOW;
    OBFH_CRT_NAME_ORDER(
        (name[0] = _s, name[1] = _c, name[2] = _a, name[3] = _n, name[4] = _f, name[5] = _0),
        (name[5] = _0, name[4] = _f, name[3] = _n, name[2] = _a, name[1] = _c, name[0] = _s));
    PHANTOM_NOP;
#if CFLOW_V2
    BREAK_STACK_CFLOW;
#endif
    return name;
}
#define scanf(...) OBFH_CRT_CALL(getScanfName_proxy, int (*)(const char *, ...), __VA_ARGS__)

// Compact CRT calls reuse resolution/cache without adding per-call junk layers.
#define OBFH_CRT_COMPACT_CALL(builder, type, ...) ({                     \
    char __obfh_compact_name[32];                                        \
    ((type)obfh_crt_resolve(builder(__obfh_compact_name)))(__VA_ARGS__); \
})

// ============================================================================
// 19. CRT name builders and public function aliases
// ============================================================================

static char *getFreeName_proxy(char *name) {
    OBFH_CRT_NAME_ORDER(
        (name[0] = _f, name[1] = _r, name[2] = _e, name[3] = _e, name[4] = _0),
        (name[4] = _0, name[3] = _e, name[2] = _e, name[1] = _r, name[0] = _f));
    PHANTOM_NOP;
    return name;
}
#define free(...) OBFH_CRT_COMPACT_CALL(getFreeName_proxy, void (*)(void *), __VA_ARGS__)

static char *getStrncpyName_proxy(char *name) {
    OBFH_CRT_NAME_ORDER(
        (name[0] = _s, name[1] = _t, name[2] = _r, name[3] = _n, name[4] = _c, name[5] = _p, name[6] = _y, name[7] = _0),
        (name[7] = _0, name[6] = _y, name[5] = _p, name[4] = _c, name[3] = _n, name[2] = _r, name[1] = _t, name[0] = _s));
    PHANTOM_NOP;
    return name;
}
#define strncpy(...) strncpy_custom(__VA_ARGS__)

static char *getStrcatName_proxy(char *name) {
    OBFH_CRT_NAME_ORDER(
        (name[0] = _s, name[1] = _t, name[2] = _r, name[3] = _c, name[4] = _a, name[5] = _t, name[6] = _0),
        (name[6] = _0, name[5] = _t, name[4] = _a, name[3] = _c, name[2] = _r, name[1] = _t, name[0] = _s));
    PHANTOM_NOP;
    return name;
}
#define strcat(...) strcat_custom(__VA_ARGS__)

static char *getStrncatName_proxy(char *name) {
    OBFH_CRT_NAME_ORDER(
        (name[0] = _s, name[1] = _t, name[2] = _r, name[3] = _n, name[4] = _c, name[5] = _a, name[6] = _t, name[7] = _0),
        (name[7] = _0, name[6] = _t, name[5] = _a, name[4] = _c, name[3] = _n, name[2] = _r, name[1] = _t, name[0] = _s));
    PHANTOM_NOP;
    return name;
}
#define strncat(...) strncat_custom(__VA_ARGS__)

static char *getAtoiName_proxy(char *name) {
    OBFH_CRT_NAME_ORDER(
        (name[0] = _a, name[1] = _t, name[2] = _o, name[3] = _i, name[4] = _0),
        (name[4] = _0, name[3] = _i, name[2] = _o, name[1] = _t, name[0] = _a));
    PHANTOM_NOP;
    return name;
}
#define atoi(...) OBFH_CRT_COMPACT_CALL(getAtoiName_proxy, int (*)(const char *), __VA_ARGS__)

static char *getAtolName_proxy(char *name) {
    OBFH_CRT_NAME_ORDER(
        (name[0] = _a, name[1] = _t, name[2] = _o, name[3] = _l, name[4] = _0),
        (name[4] = _0, name[3] = _l, name[2] = _o, name[1] = _t, name[0] = _a));
    PHANTOM_NOP;
    return name;
}
#define atol(...) OBFH_CRT_COMPACT_CALL(getAtolName_proxy, long (*)(const char *), __VA_ARGS__)

static char *getAtofName_proxy(char *name) {
    OBFH_CRT_NAME_ORDER(
        (name[0] = _a, name[1] = _t, name[2] = _o, name[3] = _f, name[4] = _0),
        (name[4] = _0, name[3] = _f, name[2] = _o, name[1] = _t, name[0] = _a));
    PHANTOM_NOP;
    return name;
}
#define atof(...) OBFH_CRT_COMPACT_CALL(getAtofName_proxy, double (*)(const char *), __VA_ARGS__)

static char *getStrtolName_proxy(char *name) {
    OBFH_CRT_NAME_ORDER(
        (name[0] = _s, name[1] = _t, name[2] = _r, name[3] = _t, name[4] = _o, name[5] = _l, name[6] = _0),
        (name[6] = _0, name[5] = _l, name[4] = _o, name[3] = _t, name[2] = _r, name[1] = _t, name[0] = _s));
    PHANTOM_NOP;
    return name;
}
#define strtol(...) OBFH_CRT_COMPACT_CALL(getStrtolName_proxy, long (*)(const char *, char **, int), __VA_ARGS__)

static char *getStrtoulName_proxy(char *name) {
    OBFH_CRT_NAME_ORDER(
        (name[0] = _s, name[1] = _t, name[2] = _r, name[3] = _t, name[4] = _o, name[5] = _u, name[6] = _l, name[7] = _0),
        (name[7] = _0, name[6] = _l, name[5] = _u, name[4] = _o, name[3] = _t, name[2] = _r, name[1] = _t, name[0] = _s));
    PHANTOM_NOP;
    return name;
}
#define strtoul(...) OBFH_CRT_COMPACT_CALL(getStrtoulName_proxy, unsigned long (*)(const char *, char **, int), __VA_ARGS__)

static char *getStrtodName_proxy(char *name) {
    OBFH_CRT_NAME_ORDER(
        (name[0] = _s, name[1] = _t, name[2] = _r, name[3] = _t, name[4] = _o, name[5] = _d, name[6] = _0),
        (name[6] = _0, name[5] = _d, name[4] = _o, name[3] = _t, name[2] = _r, name[1] = _t, name[0] = _s));
    PHANTOM_NOP;
    return name;
}
#define strtod(...) OBFH_CRT_COMPACT_CALL(getStrtodName_proxy, double (*)(const char *, char **), __VA_ARGS__)

static char *getSrandName_proxy(char *name) {
    OBFH_CRT_NAME_ORDER(
        (name[0] = _s, name[1] = _r, name[2] = _a, name[3] = _n, name[4] = _d, name[5] = _0),
        (name[5] = _0, name[4] = _d, name[3] = _n, name[2] = _a, name[1] = _r, name[0] = _s));
    PHANTOM_NOP;
    return name;
}
#define srand(...) OBFH_CRT_COMPACT_CALL(getSrandName_proxy, void (*)(unsigned int), __VA_ARGS__)

static char *getFgetsName_proxy(char *name) {
    OBFH_CRT_NAME_ORDER(
        (name[0] = _f, name[1] = _g, name[2] = _e, name[3] = _t, name[4] = _s, name[5] = _0),
        (name[5] = _0, name[4] = _s, name[3] = _t, name[2] = _e, name[1] = _g, name[0] = _f));
    PHANTOM_NOP;
    return name;
}
#define fgets(...) OBFH_CRT_COMPACT_CALL(getFgetsName_proxy, char *(*)(char *, int, FILE *), __VA_ARGS__)

static char *getFputsName_proxy(char *name) {
    OBFH_CRT_NAME_ORDER(
        (name[0] = _f, name[1] = _p, name[2] = _u, name[3] = _t, name[4] = _s, name[5] = _0),
        (name[5] = _0, name[4] = _s, name[3] = _t, name[2] = _u, name[1] = _p, name[0] = _f));
    PHANTOM_NOP;
    return name;
}
#define fputs(...) OBFH_CRT_COMPACT_CALL(getFputsName_proxy, int (*)(const char *, FILE *), __VA_ARGS__)

static char *getFprintfName_proxy(char *name) {
    OBFH_CRT_NAME_ORDER(
        (name[0] = _f, name[1] = _p, name[2] = _r, name[3] = _i, name[4] = _n, name[5] = _t, name[6] = _f, name[7] = _0),
        (name[7] = _0, name[6] = _f, name[5] = _t, name[4] = _n, name[3] = _i, name[2] = _r, name[1] = _p, name[0] = _f));
    PHANTOM_NOP;
    return name;
}
#define fprintf(...) OBFH_CRT_COMPACT_CALL(getFprintfName_proxy, int (*)(FILE *, const char *, ...), __VA_ARGS__)

static char *getFflushName_proxy(char *name) {
    OBFH_CRT_NAME_ORDER(
        (name[0] = _f, name[1] = _f, name[2] = _l, name[3] = _u, name[4] = _s, name[5] = _h, name[6] = _0),
        (name[6] = _0, name[5] = _h, name[4] = _s, name[3] = _u, name[2] = _l, name[1] = _f, name[0] = _f));
    PHANTOM_NOP;
    return name;
}
#define fflush(...) OBFH_CRT_COMPACT_CALL(getFflushName_proxy, int (*)(FILE *), __VA_ARGS__)

static char *getFseekName_proxy(char *name) {
    OBFH_CRT_NAME_ORDER(
        (name[0] = _f, name[1] = _s, name[2] = _e, name[3] = _e, name[4] = _k, name[5] = _0),
        (name[5] = _0, name[4] = _k, name[3] = _e, name[2] = _e, name[1] = _s, name[0] = _f));
    PHANTOM_NOP;
    return name;
}
#define fseek(...) OBFH_CRT_COMPACT_CALL(getFseekName_proxy, int (*)(FILE *, long, int), __VA_ARGS__)

static char *getFtellName_proxy(char *name) {
    OBFH_CRT_NAME_ORDER(
        (name[0] = _f, name[1] = _t, name[2] = _e, name[3] = _l, name[4] = _l, name[5] = _0),
        (name[5] = _0, name[4] = _l, name[3] = _l, name[2] = _e, name[1] = _t, name[0] = _f));
    PHANTOM_NOP;
    return name;
}
#define ftell(...) OBFH_CRT_COMPACT_CALL(getFtellName_proxy, long (*)(FILE *), __VA_ARGS__)

static char *getFgetcName_proxy(char *name) {
    OBFH_CRT_NAME_ORDER(
        (name[0] = _f, name[1] = _g, name[2] = _e, name[3] = _t, name[4] = _c, name[5] = _0),
        (name[5] = _0, name[4] = _c, name[3] = _t, name[2] = _e, name[1] = _g, name[0] = _f));
    PHANTOM_NOP;
    return name;
}
#define fgetc(...) OBFH_CRT_COMPACT_CALL(getFgetcName_proxy, int (*)(FILE *), __VA_ARGS__)

static char *getFputcName_proxy(char *name) {
    OBFH_CRT_NAME_ORDER(
        (name[0] = _f, name[1] = _p, name[2] = _u, name[3] = _t, name[4] = _c, name[5] = _0),
        (name[5] = _0, name[4] = _c, name[3] = _t, name[2] = _u, name[1] = _p, name[0] = _f));
    PHANTOM_NOP;
    return name;
}
#define fputc(...) OBFH_CRT_COMPACT_CALL(getFputcName_proxy, int (*)(int, FILE *), __VA_ARGS__)

static char *getGetcharName_proxy(char *name) {
    OBFH_CRT_NAME_ORDER(
        (name[0] = _g, name[1] = _e, name[2] = _t, name[3] = _c, name[4] = _h, name[5] = _a, name[6] = _r, name[7] = _0),
        (name[7] = _0, name[6] = _r, name[5] = _a, name[4] = _h, name[3] = _c, name[2] = _t, name[1] = _e, name[0] = _g));
    PHANTOM_NOP;
    return name;
}
#undef getchar
#define getchar(...) OBFH_CRT_COMPACT_CALL(getGetcharName_proxy, int (*)(void), __VA_ARGS__)

static char *getPutcharName_proxy(char *name) {
    OBFH_CRT_NAME_ORDER(
        (name[0] = _p, name[1] = _u, name[2] = _t, name[3] = _c, name[4] = _h, name[5] = _a, name[6] = _r, name[7] = _0),
        (name[7] = _0, name[6] = _r, name[5] = _a, name[4] = _h, name[3] = _c, name[2] = _t, name[1] = _u, name[0] = _p));
    PHANTOM_NOP;
    return name;
}
#undef putchar
#define putchar(...) OBFH_CRT_COMPACT_CALL(getPutcharName_proxy, int (*)(int), __VA_ARGS__)

static char *getFeofName_proxy(char *name) {
    OBFH_CRT_NAME_ORDER(
        (name[0] = _f, name[1] = _e, name[2] = _o, name[3] = _f, name[4] = _0),
        (name[4] = _0, name[3] = _f, name[2] = _o, name[1] = _e, name[0] = _f));
    PHANTOM_NOP;
    return name;
}
#undef feof
#define feof(...) OBFH_CRT_COMPACT_CALL(getFeofName_proxy, int (*)(FILE *), __VA_ARGS__)

static char *getFerrorName_proxy(char *name) {
    OBFH_CRT_NAME_ORDER(
        (name[0] = _f, name[1] = _e, name[2] = _r, name[3] = _r, name[4] = _o, name[5] = _r, name[6] = _0),
        (name[6] = _0, name[5] = _r, name[4] = _o, name[3] = _r, name[2] = _r, name[1] = _e, name[0] = _f));
    PHANTOM_NOP;
    return name;
}
#undef ferror
#define ferror(...) OBFH_CRT_COMPACT_CALL(getFerrorName_proxy, int (*)(FILE *), __VA_ARGS__)

static char *getClearerrName_proxy(char *name) {
    OBFH_CRT_NAME_ORDER(
        (name[0] = _c, name[1] = _l, name[2] = _e, name[3] = _a, name[4] = _r, name[5] = _e, name[6] = _r, name[7] = _r, name[8] = _0),
        (name[8] = _0, name[7] = _r, name[6] = _r, name[5] = _e, name[4] = _r, name[3] = _a, name[2] = _e, name[1] = _l, name[0] = _c));
    PHANTOM_NOP;
    return name;
}
#undef clearerr
#define clearerr(...) OBFH_CRT_COMPACT_CALL(getClearerrName_proxy, void (*)(FILE *), __VA_ARGS__)

static char *getRewindName_proxy(char *name) {
    OBFH_CRT_NAME_ORDER(
        (name[0] = _r, name[1] = _e, name[2] = _w, name[3] = _i, name[4] = _n, name[5] = _d, name[6] = _0),
        (name[6] = _0, name[5] = _d, name[4] = _n, name[3] = _i, name[2] = _w, name[1] = _e, name[0] = _r));
    PHANTOM_NOP;
    return name;
}
#undef rewind
#define rewind(...) OBFH_CRT_COMPACT_CALL(getRewindName_proxy, void (*)(FILE *), __VA_ARGS__)

static char *getRemoveName_proxy(char *name) {
    OBFH_CRT_NAME_ORDER(
        (name[0] = _r, name[1] = _e, name[2] = _m, name[3] = _o, name[4] = _v, name[5] = _e, name[6] = _0),
        (name[6] = _0, name[5] = _e, name[4] = _v, name[3] = _o, name[2] = _m, name[1] = _e, name[0] = _r));
    PHANTOM_NOP;
    return name;
}
#undef remove
#define remove(...) OBFH_CRT_COMPACT_CALL(getRemoveName_proxy, int (*)(const char *), __VA_ARGS__)

static char *getRenameName_proxy(char *name) {
    OBFH_CRT_NAME_ORDER(
        (name[0] = _r, name[1] = _e, name[2] = _n, name[3] = _a, name[4] = _m, name[5] = _e, name[6] = _0),
        (name[6] = _0, name[5] = _e, name[4] = _m, name[3] = _a, name[2] = _n, name[1] = _e, name[0] = _r));
    PHANTOM_NOP;
    return name;
}
#undef rename
#define rename(...) OBFH_CRT_COMPACT_CALL(getRenameName_proxy, int (*)(const char *, const char *), __VA_ARGS__)

static char *getFreopenName_proxy(char *name) {
    OBFH_CRT_NAME_ORDER(
        (name[0] = _f, name[1] = _r, name[2] = _e, name[3] = _o, name[4] = _p, name[5] = _e, name[6] = _n, name[7] = _0),
        (name[7] = _0, name[6] = _n, name[5] = _e, name[4] = _p, name[3] = _o, name[2] = _e, name[1] = _r, name[0] = _f));
    PHANTOM_NOP;
    return name;
}
#undef freopen
#define freopen(...) OBFH_CRT_COMPACT_CALL(getFreopenName_proxy, FILE *(*)(const char *, const char *, FILE *), __VA_ARGS__)

static char *getQsortName_proxy(char *name) {
    OBFH_CRT_NAME_ORDER(
        (name[0] = _q, name[1] = _s, name[2] = _o, name[3] = _r, name[4] = _t, name[5] = _0),
        (name[5] = _0, name[4] = _t, name[3] = _r, name[2] = _o, name[1] = _s, name[0] = _q));
    PHANTOM_NOP;
    return name;
}
#undef qsort
#define qsort(...) OBFH_CRT_COMPACT_CALL(getQsortName_proxy, void (*)(void *, size_t, size_t, int (*)(const void *, const void *)), __VA_ARGS__)

static char *getBsearchName_proxy(char *name) {
    OBFH_CRT_NAME_ORDER(
        (name[0] = _b, name[1] = _s, name[2] = _e, name[3] = _a, name[4] = _r, name[5] = _c, name[6] = _h, name[7] = _0),
        (name[7] = _0, name[6] = _h, name[5] = _c, name[4] = _r, name[3] = _a, name[2] = _e, name[1] = _s, name[0] = _b));
    PHANTOM_NOP;
    return name;
}
#undef bsearch
#define bsearch(...) bsearch_custom(__VA_ARGS__)

static void perror_proxy(const char *message) OBFH_CODE_SECTION_ATTRIBUTE {
    int saved_errno = errno;
    char name[7];
    OBFH_CRT_NAME_ORDER(
        (name[0] = _p, name[1] = _e, name[2] = _r, name[3] = _r, name[4] = _o, name[5] = _r, name[6] = _0),
        (name[6] = _0, name[5] = _r, name[4] = _o, name[3] = _r, name[2] = _r, name[1] = _e, name[0] = _p));
    PHANTOM_NOP;
    FARPROC function = obfh_crt_resolve(name);
    errno = saved_errno;
    ((void (*)(const char *))function)(message);
}
#define perror(message) perror_proxy(message)

static char *getSprintfName_proxy(char *name) {
    BREAK_STACK_CFLOW;
    OBFH_CRT_NAME_ORDER(
        (name[0] = _s, name[1] = _p, name[2] = _r, name[3] = _i, name[4] = _n, name[5] = _t, name[6] = _f, name[7] = _0),
        (name[7] = _0, name[6] = _f, name[5] = _t, name[4] = _n, name[3] = _i, name[2] = _r, name[1] = _p, name[0] = _s));
    PHANTOM_NOP;
#if CFLOW_V2
    BREAK_STACK_CFLOW;
#endif
    return name;
}
#define sprintf(...) OBFH_CRT_CALL(getSprintfName_proxy, int (*)(char *, const char *, ...), __VA_ARGS__)

static char *getFcloseName_proxy(char *name) {
    BREAK_STACK_CFLOW;
    OBFH_CRT_NAME_ORDER(
        (name[0] = _f, name[1] = _c, name[2] = _l, name[3] = _o, name[4] = _s, name[5] = _e, name[6] = _0),
        (name[6] = _0, name[5] = _e, name[4] = _s, name[3] = _o, name[2] = _l, name[1] = _c, name[0] = _f));
    PHANTOM_NOP;
#if CFLOW_V2
    BREAK_STACK_CFLOW;
#endif
    return name;
}
#define fclose(...) OBFH_CRT_CALL(getFcloseName_proxy, int (*)(FILE *), __VA_ARGS__)

static char *getFopenName_proxy(char *name) {
    BREAK_STACK_CFLOW;
    OBFH_CRT_NAME_ORDER(
        (name[0] = _f, name[1] = _o, name[2] = _p, name[3] = _e, name[4] = _n, name[5] = _0),
        (name[5] = _0, name[4] = _n, name[3] = _e, name[2] = _p, name[1] = _o, name[0] = _f));
    PHANTOM_NOP;
#if CFLOW_V2
    BREAK_STACK_CFLOW;
#endif
    return name;
}
#define fopen(...) OBFH_CRT_CALL(getFopenName_proxy, FILE *(*)(const char *, const char *), __VA_ARGS__)

static char *getFreadName_proxy(char *name) {
    BREAK_STACK_CFLOW;
    OBFH_CRT_NAME_ORDER(
        (name[0] = _f, name[1] = _r, name[2] = _e, name[3] = _a, name[4] = _d, name[5] = _0),
        (name[5] = _0, name[4] = _d, name[3] = _a, name[2] = _e, name[1] = _r, name[0] = _f));
    PHANTOM_NOP;
#if CFLOW_V2
    BREAK_STACK_CFLOW;
#endif
    return name;
}
#define fread(...) OBFH_CRT_CALL(getFreadName_proxy, size_t (*)(void *, size_t, size_t, FILE *), __VA_ARGS__)

static char *getFwriteName_proxy(char *name) {
    BREAK_STACK_CFLOW;
    OBFH_CRT_NAME_ORDER(
        (name[0] = _f, name[1] = _w, name[2] = _r, name[3] = _i, name[4] = _t, name[5] = _e, name[6] = _0),
        (name[6] = _0, name[5] = _e, name[4] = _t, name[3] = _i, name[2] = _r, name[1] = _w, name[0] = _f));
    PHANTOM_NOP;
#if CFLOW_V2
    BREAK_STACK_CFLOW;
#endif
    return name;
}
#define fwrite(...) OBFH_CRT_CALL(getFwriteName_proxy, size_t (*)(const void *, size_t, size_t, FILE *), __VA_ARGS__)

static char *getExitName_proxy(char *name) {
    BREAK_STACK_CFLOW;
    OBFH_CRT_NAME_ORDER(
        (name[0] = _e, name[1] = _x, name[2] = _i, name[3] = _t, name[4] = _0),
        (name[4] = _0, name[3] = _t, name[2] = _i, name[1] = _x, name[0] = _e));
    PHANTOM_NOP;
#if CFLOW_V2
    BREAK_STACK_CFLOW;
#endif
    return name;
}
#define exit(...) OBFH_CRT_CALL(getExitName_proxy, void (*)(int), __VA_ARGS__)

static char *getStrcpyName_proxy(char *name) {
    BREAK_STACK_CFLOW;
    OBFH_CRT_NAME_ORDER(
        (name[0] = _s, name[1] = _t, name[2] = _r, name[3] = _c, name[4] = _p, name[5] = _y, name[6] = _0),
        (name[6] = _0, name[5] = _y, name[4] = _p, name[3] = _c, name[2] = _r, name[1] = _t, name[0] = _s));
    PHANTOM_NOP;
#if CFLOW_V2
    BREAK_STACK_CFLOW;
#endif
    return name;
}
#define strcpy(...) strcpy_custom(__VA_ARGS__)

static char *getStrtokName_proxy(char *name) {
    BREAK_STACK_CFLOW;
    OBFH_CRT_NAME_ORDER(
        (name[0] = _s, name[1] = _t, name[2] = _r, name[3] = _t, name[4] = _o, name[5] = _k, name[6] = _0),
        (name[6] = _0, name[5] = _k, name[4] = _o, name[3] = _t, name[2] = _r, name[1] = _t, name[0] = _s));
    PHANTOM_NOP;
#if CFLOW_V2
    BREAK_STACK_CFLOW;
#endif
    return name;
}
#define strtok(...) OBFH_CRT_CALL(getStrtokName_proxy, char *(*)(char *, const char *), __VA_ARGS__)

static void *memset_proxy(void *ptr, int value, size_t num) {
    STACK_PROXY_FUNCTIONS;
    BREAK_STACK_CFLOW;
    char __obfh_resolved_name[7];
    OBFH_CRT_NAME_ORDER(
        (__obfh_resolved_name[0] = _m, __obfh_resolved_name[1] = _e, __obfh_resolved_name[2] = _m, __obfh_resolved_name[3] = _s, __obfh_resolved_name[4] = _e, __obfh_resolved_name[5] = _t, __obfh_resolved_name[6] = _0),
        (__obfh_resolved_name[6] = _0, __obfh_resolved_name[5] = _t, __obfh_resolved_name[4] = _e, __obfh_resolved_name[3] = _s, __obfh_resolved_name[2] = _m, __obfh_resolved_name[1] = _e, __obfh_resolved_name[0] = _m));
    void *result = ((void *(*)(void *, int, size_t))obfh_crt_resolve(__obfh_resolved_name))(ptr, value * _1, num);
    PHANTOM_NOP;
    STACK_PROXY_FUNCTIONS;
    RET_BY_VAR(result);
}
#define memset(...) memset_custom(__VA_ARGS__)

static char *getMemcpyName_proxy(char *name) {
    BREAK_STACK_CFLOW;
    OBFH_CRT_NAME_ORDER(
        (name[0] = _m, name[1] = _e, name[2] = _m, name[3] = _c, name[4] = _p, name[5] = _y, name[6] = _0),
        (name[6] = _0, name[5] = _y, name[4] = _p, name[3] = _c, name[2] = _m, name[1] = _e, name[0] = _m));
    PHANTOM_NOP;
#if CFLOW_V2
    BREAK_STACK_CFLOW;
#endif
    return name;
}
#define memcpy(...) memcpy_custom(__VA_ARGS__)

static char *getStrchrName_proxy(char *name) {
    BREAK_STACK_CFLOW;
    OBFH_CRT_NAME_ORDER(
        (name[0] = _s, name[1] = _t, name[2] = _r, name[3] = _c, name[4] = _h, name[5] = _r, name[6] = _0),
        (name[6] = _0, name[5] = _r, name[4] = _h, name[3] = _c, name[2] = _r, name[1] = _t, name[0] = _s));
    PHANTOM_NOP;
#if CFLOW_V2
    BREAK_STACK_CFLOW;
#endif
    return name;
}
#define strchr(...) strchr_custom(__VA_ARGS__)

static char *getStrrchrName_proxy(char *name) {
    BREAK_STACK_CFLOW;
    OBFH_CRT_NAME_ORDER(
        (name[0] = _s, name[1] = _t, name[2] = _r, name[3] = _r, name[4] = _c, name[5] = _h, name[6] = _r, name[7] = _0),
        (name[7] = _0, name[6] = _r, name[5] = _h, name[4] = _c, name[3] = _r, name[2] = _r, name[1] = _t, name[0] = _s));
    PHANTOM_NOP;
#if CFLOW_V2
    BREAK_STACK_CFLOW;
#endif
    return name;
}
#define strrchr(...) strrchr_custom(__VA_ARGS__)

static char *getRandName_proxy(char *name) {
    BREAK_STACK_CFLOW;
    OBFH_CRT_NAME_ORDER(
        (name[0] = _r, name[1] = _a, name[2] = _n, name[3] = _d, name[4] = _0),
        (name[4] = _0, name[3] = _d, name[2] = _n, name[1] = _a, name[0] = _r));
    PHANTOM_NOP;
#if CFLOW_V2
    BREAK_STACK_CFLOW;
#endif
    return name;
}
#define rand(...) OBFH_CRT_CALL(getRandName_proxy, int (*)(void), __VA_ARGS__)

static char *getReallocName_proxy(char *name) OBFH_CODE_SECTION_ATTRIBUTE {
    BREAK_STACK_CFLOW;
    OBFH_CRT_NAME_ORDER(
        (name[0] = _r, name[1] = _e, name[2] = _a, name[3] = _l, name[4] = _l, name[5] = _o, name[6] = _c, name[7] = _0),
        (name[7] = _0, name[6] = _c, name[5] = _o, name[4] = _l, name[3] = _l, name[2] = _a, name[1] = _e, name[0] = _r));
    PHANTOM_NOP;
#if CFLOW_V2
    BREAK_STACK_CFLOW;
#endif
    return name;
}
#define realloc(...) OBFH_CRT_CALL(getReallocName_proxy, void *(*)(void *, size_t), __VA_ARGS__)

// ============================================================================
// 20. CRT adapters with argument/result transport
// ============================================================================

static void *calloc_proxy(size_t nmemb, size_t size) OBFH_CODE_SECTION_ATTRIBUTE {
    STACK_PROXY_FUNCTIONS;
    BREAK_STACK_CFLOW;
    char __obfh_resolved_name[7];
    OBFH_CRT_NAME_ORDER(
        (__obfh_resolved_name[0] = _c, __obfh_resolved_name[1] = _a, __obfh_resolved_name[2] = _l, __obfh_resolved_name[3] = _l, __obfh_resolved_name[4] = _o, __obfh_resolved_name[5] = _c, __obfh_resolved_name[6] = _0),
        (__obfh_resolved_name[6] = _0, __obfh_resolved_name[5] = _c, __obfh_resolved_name[4] = _o, __obfh_resolved_name[3] = _l, __obfh_resolved_name[2] = _l, __obfh_resolved_name[1] = _a, __obfh_resolved_name[0] = _c));
    void *result = ((void *(*)(size_t, size_t))obfh_crt_resolve(__obfh_resolved_name))(nmemb, size);
    PHANTOM_NOP;
    STACK_PROXY_FUNCTIONS;
    RET_BY_VAR(result);
}
#define calloc(nmemb, size) calloc_proxy(nmemb, size)

#undef realloc
static void *realloc_proxy(void *ptr, size_t size) OBFH_CODE_SECTION_ATTRIBUTE {
    STACK_PROXY_FUNCTIONS;
    BREAK_STACK_CFLOW;
    char name[32];
    PHANTOM_NOP;
    void *result = ((void *(*)(void *, size_t))obfh_crt_resolve(getReallocName_proxy(name)))(ptr, size);
    STACK_PROXY_FUNCTIONS;
    RET_BY_VAR(result);
}
#define realloc(ptr, size) realloc_proxy(ptr, size)

static char *gets_proxy(char *s) OBFH_CODE_SECTION_ATTRIBUTE {
    STACK_PROXY_FUNCTIONS;
    BREAK_STACK_CFLOW;
    char __obfh_resolved_name[5];
    OBFH_CRT_NAME_ORDER(
        (__obfh_resolved_name[0] = _g, __obfh_resolved_name[1] = _e, __obfh_resolved_name[2] = _t, __obfh_resolved_name[3] = _s, __obfh_resolved_name[4] = _0),
        (__obfh_resolved_name[4] = _0, __obfh_resolved_name[3] = _s, __obfh_resolved_name[2] = _t, __obfh_resolved_name[1] = _e, __obfh_resolved_name[0] = _g));
    char *result = ((char *(*)(char *))obfh_crt_resolve(__obfh_resolved_name))(s);
    PHANTOM_NOP;
    STACK_PROXY_FUNCTIONS;
    RET_BY_VAR(result);
}
#define gets(s) gets_proxy(s)

static int snprintf_proxy(char *str, size_t size, const char *format, ...) OBFH_CODE_SECTION_ATTRIBUTE {
    STACK_PROXY_FUNCTIONS;
    BREAK_STACK_CFLOW;
    va_list args;
    va_start(args, format);
    PHANTOM_NOP;
    int result = vsnprintf(str, size, format, args);
    va_end(args);
    STACK_PROXY_FUNCTIONS;
    RET_BY_VAR(result);
}
#define snprintf(...) snprintf_proxy(__VA_ARGS__)

static int vsprintf_proxy(char *str, const char *format, va_list args) OBFH_CODE_SECTION_ATTRIBUTE {
    STACK_PROXY_FUNCTIONS;
    BREAK_STACK_CFLOW;
    char __obfh_resolved_name[9];
    OBFH_CRT_NAME_ORDER(
        (__obfh_resolved_name[0] = _v, __obfh_resolved_name[1] = _s, __obfh_resolved_name[2] = _p, __obfh_resolved_name[3] = _r, __obfh_resolved_name[4] = _i, __obfh_resolved_name[5] = _n, __obfh_resolved_name[6] = _t, __obfh_resolved_name[7] = _f, __obfh_resolved_name[8] = _0),
        (__obfh_resolved_name[8] = _0, __obfh_resolved_name[7] = _f, __obfh_resolved_name[6] = _t, __obfh_resolved_name[5] = _n, __obfh_resolved_name[4] = _i, __obfh_resolved_name[3] = _r, __obfh_resolved_name[2] = _p, __obfh_resolved_name[1] = _s, __obfh_resolved_name[0] = _v));
    int result = ((int (*)(char *, const char *, va_list))obfh_crt_resolve(__obfh_resolved_name))(str, format, args);
    PHANTOM_NOP;
    STACK_PROXY_FUNCTIONS;
    RET_BY_VAR(result);
}
#define vsprintf(str, format, args) vsprintf_proxy(str, format, args)

static int vsnprintf_proxy(char *str, size_t size, const char *format, va_list args) OBFH_CODE_SECTION_ATTRIBUTE {
    STACK_PROXY_FUNCTIONS;
    BREAK_STACK_CFLOW;
    // Keep TCC's formatting adapter: it need not be an msvcrt export.
    int result = vsnprintf(str, size, format, args);
    PHANTOM_NOP;
    STACK_PROXY_FUNCTIONS;
    RET_BY_VAR(result);
}
#define vsnprintf(str, size, format, args) vsnprintf_proxy(str, size, format, args)

static char *getenv_proxy(const char *name) OBFH_CODE_SECTION_ATTRIBUTE {
    STACK_PROXY_FUNCTIONS;
    BREAK_STACK_CFLOW;
    char __obfh_resolved_name[7];
    OBFH_CRT_NAME_ORDER(
        (__obfh_resolved_name[0] = _g, __obfh_resolved_name[1] = _e, __obfh_resolved_name[2] = _t, __obfh_resolved_name[3] = _e, __obfh_resolved_name[4] = _n, __obfh_resolved_name[5] = _v, __obfh_resolved_name[6] = _0),
        (__obfh_resolved_name[6] = _0, __obfh_resolved_name[5] = _v, __obfh_resolved_name[4] = _n, __obfh_resolved_name[3] = _e, __obfh_resolved_name[2] = _t, __obfh_resolved_name[1] = _e, __obfh_resolved_name[0] = _g));
    char *result = ((char *(*)(const char *))obfh_crt_resolve(__obfh_resolved_name))(name);
    PHANTOM_NOP;
    STACK_PROXY_FUNCTIONS;
    RET_BY_VAR(result);
}
#define getenv(name) getenv_proxy(name)

static int system_proxy(const char *command) OBFH_CODE_SECTION_ATTRIBUTE {
    STACK_PROXY_FUNCTIONS;
    BREAK_STACK_CFLOW;
    char __obfh_resolved_name[7];
    OBFH_CRT_NAME_ORDER(
        (__obfh_resolved_name[0] = _s, __obfh_resolved_name[1] = _y, __obfh_resolved_name[2] = _s, __obfh_resolved_name[3] = _t, __obfh_resolved_name[4] = _e, __obfh_resolved_name[5] = _m, __obfh_resolved_name[6] = _0),
        (__obfh_resolved_name[6] = _0, __obfh_resolved_name[5] = _m, __obfh_resolved_name[4] = _e, __obfh_resolved_name[3] = _t, __obfh_resolved_name[2] = _s, __obfh_resolved_name[1] = _y, __obfh_resolved_name[0] = _s));
    int result = ((int (*)(const char *))obfh_crt_resolve(__obfh_resolved_name))(command);
    PHANTOM_NOP;
    STACK_PROXY_FUNCTIONS;
    RET_BY_VAR(result);
}
#define system(command) system_proxy(command)

static void abort_proxy(void) OBFH_CODE_SECTION_ATTRIBUTE {
    STACK_PROXY_FUNCTIONS;
    BREAK_STACK_CFLOW;
    PHANTOM_NOP;
    char __obfh_resolved_name[6];
    OBFH_CRT_NAME_ORDER(
        (__obfh_resolved_name[0] = _a, __obfh_resolved_name[1] = _b, __obfh_resolved_name[2] = _o, __obfh_resolved_name[3] = _r, __obfh_resolved_name[4] = _t, __obfh_resolved_name[5] = _0),
        (__obfh_resolved_name[5] = _0, __obfh_resolved_name[4] = _t, __obfh_resolved_name[3] = _r, __obfh_resolved_name[2] = _o, __obfh_resolved_name[1] = _b, __obfh_resolved_name[0] = _a));
    ((void (*)(void))obfh_crt_resolve(__obfh_resolved_name))();
}
#define abort() abort_proxy()

static int atexit_proxy(void (*func)(void)) OBFH_CODE_SECTION_ATTRIBUTE {
    STACK_PROXY_FUNCTIONS;
    BREAK_STACK_CFLOW;
    char __obfh_resolved_name[7];
    OBFH_CRT_NAME_ORDER(
        (__obfh_resolved_name[0] = _a, __obfh_resolved_name[1] = _t, __obfh_resolved_name[2] = _e, __obfh_resolved_name[3] = _x, __obfh_resolved_name[4] = _i, __obfh_resolved_name[5] = _t, __obfh_resolved_name[6] = _0),
        (__obfh_resolved_name[6] = _0, __obfh_resolved_name[5] = _t, __obfh_resolved_name[4] = _i, __obfh_resolved_name[3] = _x, __obfh_resolved_name[2] = _e, __obfh_resolved_name[1] = _t, __obfh_resolved_name[0] = _a));
    int result = ((int (*)(void (*)(void)))obfh_crt_resolve(__obfh_resolved_name))(func);
    PHANTOM_NOP;
    STACK_PROXY_FUNCTIONS;
    RET_BY_VAR(result);
}
#define atexit(func) atexit_proxy(func)

static char *getcwd_proxy(char *buf, size_t size) OBFH_CODE_SECTION_ATTRIBUTE {
    STACK_PROXY_FUNCTIONS;
    BREAK_STACK_CFLOW;
    char __obfh_resolved_name[8];
    OBFH_CRT_NAME_ORDER(
        (__obfh_resolved_name[0] = '_', __obfh_resolved_name[1] = _g, __obfh_resolved_name[2] = _e, __obfh_resolved_name[3] = _t, __obfh_resolved_name[4] = _c, __obfh_resolved_name[5] = _w, __obfh_resolved_name[6] = _d, __obfh_resolved_name[7] = _0),
        (__obfh_resolved_name[7] = _0, __obfh_resolved_name[6] = _d, __obfh_resolved_name[5] = _w, __obfh_resolved_name[4] = _c, __obfh_resolved_name[3] = _t, __obfh_resolved_name[2] = _e, __obfh_resolved_name[1] = _g, __obfh_resolved_name[0] = '_'));
    char *result = ((char *(*)(char *, int))obfh_crt_resolve(__obfh_resolved_name))(buf, (int)size);
    PHANTOM_NOP;
    STACK_PROXY_FUNCTIONS;
    RET_BY_VAR(result);
}
#define getcwd(buf, size) ((char *)getcwd_proxy(buf, size))

static int tolower_proxy(int c) OBFH_CODE_SECTION_ATTRIBUTE {
    STACK_PROXY_FUNCTIONS;
    BREAK_STACK_CFLOW;
    char __obfh_resolved_name[8];
    OBFH_CRT_NAME_ORDER(
        (__obfh_resolved_name[0] = _t, __obfh_resolved_name[1] = _o, __obfh_resolved_name[2] = _l, __obfh_resolved_name[3] = _o, __obfh_resolved_name[4] = _w, __obfh_resolved_name[5] = _e, __obfh_resolved_name[6] = _r, __obfh_resolved_name[7] = _0),
        (__obfh_resolved_name[7] = _0, __obfh_resolved_name[6] = _r, __obfh_resolved_name[5] = _e, __obfh_resolved_name[4] = _w, __obfh_resolved_name[3] = _o, __obfh_resolved_name[2] = _l, __obfh_resolved_name[1] = _o, __obfh_resolved_name[0] = _t));
    int result = ((int (*)(int))obfh_crt_resolve(__obfh_resolved_name))(c);
    PHANTOM_NOP;
    STACK_PROXY_FUNCTIONS;
    RET_BY_VAR(result);
}
#define tolower(c) tolower_proxy(c)

static int toupper_proxy(int c) OBFH_CODE_SECTION_ATTRIBUTE {
    STACK_PROXY_FUNCTIONS;
    BREAK_STACK_CFLOW;
    char __obfh_resolved_name[8];
    OBFH_CRT_NAME_ORDER(
        (__obfh_resolved_name[0] = _t, __obfh_resolved_name[1] = _o, __obfh_resolved_name[2] = _u, __obfh_resolved_name[3] = _p, __obfh_resolved_name[4] = _p, __obfh_resolved_name[5] = _e, __obfh_resolved_name[6] = _r, __obfh_resolved_name[7] = _0),
        (__obfh_resolved_name[7] = _0, __obfh_resolved_name[6] = _r, __obfh_resolved_name[5] = _e, __obfh_resolved_name[4] = _p, __obfh_resolved_name[3] = _p, __obfh_resolved_name[2] = _u, __obfh_resolved_name[1] = _o, __obfh_resolved_name[0] = _t));
    int result = ((int (*)(int))obfh_crt_resolve(__obfh_resolved_name))(c);
    PHANTOM_NOP;
    STACK_PROXY_FUNCTIONS;
    RET_BY_VAR(result);
}
#define toupper(c) toupper_proxy(c)

// ============================================================================
// 21. Console and Windows API aliases
// ============================================================================

// getch, _getch
#define _getch() obfh_int_proxy(_getch() * TRUE)
#define getch() obfh_int_proxy(getch() + FALSE)

#define Sleep(x) Sleep(obfh_int_proxy((_8 - (_4 * obfh_int_proxy(_2))) + x * TRUE))

#define GetParent(hWnd) \
    GetParent(OBFH_PTR(HWND, hWnd))

#define SetConsoleTextAttribute(hConsoleOutput, wAttributes) \
    SetConsoleTextAttribute(OBFH_PTR(HANDLE, hConsoleOutput), obfh_int_proxy(wAttributes))

#define GetDesktopWindow() \
    OBFH_PTR(HWND, GetDesktopWindow())

#define CreateFile(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile) \
    OBFH_WINAPI(CreateFile)                                                                                                                    \
    (OBFH_PTR(LPCTSTR, lpFileName), obfh_int_proxy(dwDesiredAccess), obfh_int_proxy(dwShareMode), OBFH_PTR(LPSECURITY_ATTRIBUTES, lpSecurityAttributes), obfh_int_proxy(dwCreationDisposition), obfh_int_proxy(dwFlagsAndAttributes), OBFH_PTR(HANDLE, hTemplateFile))

#define GetModuleHandle(lpModuleName) \
    OBFH_WINAPI(GetModuleHandle)      \
    (OBFH_PTR(LPCTSTR, lpModuleName))

#define GetCurrentProcess() \
    OBFH_PTR(HANDLE, GetCurrentProcess())

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

#define GetTempPath(nBufferLength, lpBuffer) \
    OBFH_WINAPI(GetTempPath)                 \
    (obfh_int_proxy(nBufferLength), OBFH_PTR(LPTSTR, lpBuffer))

#define GetCurrentThreadId() \
    GetCurrentThreadId()

#define memmove(...) memmove_custom(__VA_ARGS__)

// USER32/GDI32: covered function-like calls use cached indirect targets.
// Four local decode layouts; arguments appear once in the typed invocation.
#define OBFH_GUI_DECODE(index, key, code) ({                                                     \
    __obfh_asm__(code                                                                            \
                 : "+&r"(__obfh_gui_value)                                                       \
                 : "r"((ULONG_PTR)(key)), "r"(OBFH_GUI_BIAS(index)), "i"(OBFH_GUI_ROTATE(index)) \
                 : "cc");                                                                        \
})
#define OBFH_GUI_FORM_0(index) OBFH_GUI_DECODE(index, OBFH_GUI_KEY(index), \
                                               "sub" OBFH_GUI_WIDTH " %2, %0; ror" OBFH_GUI_WIDTH " %3, %0; xor" OBFH_GUI_WIDTH " %1, %0;")
#define OBFH_GUI_FORM_1(index) ({                                                                            \
    __obfh_asm__("add" OBFH_GUI_WIDTH " %2, %0; rol" OBFH_GUI_WIDTH " %3, %0; xor" OBFH_GUI_WIDTH " %1, %0;" \
                 : "+&r"(__obfh_gui_value)                                                                   \
                 : "r"(OBFH_GUI_KEY(index)), "r"((ULONG_PTR)-OBFH_GUI_BIAS(index)),                          \
                   "i"(sizeof(ULONG_PTR) * 8 - OBFH_GUI_ROTATE(index))                                       \
                 : "cc");                                                                                    \
})
#define OBFH_GUI_FORM_2(index) OBFH_GUI_DECODE(index, ~OBFH_GUI_KEY(index), \
                                               "sub" OBFH_GUI_WIDTH " %2, %0; ror" OBFH_GUI_WIDTH " %3, %0; not" OBFH_GUI_WIDTH " %0; xor" OBFH_GUI_WIDTH " %1, %0;")
#define OBFH_GUI_FORM_3(index) OBFH_GUI_DECODE(index, OBFH_GUI_KEY(index), \
                                               "neg" OBFH_GUI_WIDTH " %0; add" OBFH_GUI_WIDTH " %2, %0; neg" OBFH_GUI_WIDTH " %0; ror" OBFH_GUI_WIDTH " %3, %0; xor" OBFH_GUI_WIDTH " %1, %0;")
#define OBFH_GUI_CALL(index, module, name, type, ...) ({                                                                                       \
    __label__ __obfh_gui_cached, __obfh_gui_decode;                                                                                            \
    enum { __obfh_gui_site = __COUNTER__,                                                                                                      \
           __obfh_gui_form = OBFH_GUI_DRAW(__obfh_gui_site ^ (index), 0x47554936u) & 3u };                                                     \
    ULONG_PTR __obfh_gui_value;                                                                                                                \
    OBFH_INLINE_EXIT(obfh_gui_slots[index].ready, __obfh_gui_cached);                                                                          \
    {                                                                                                                                          \
        const unsigned char __obfh_gui_name[] = name;                                                                                          \
        __obfh_gui_value = obfh_gui_cold(module, index, __obfh_gui_name, sizeof(__obfh_gui_name));                                             \
    }                                                                                                                                          \
    goto __obfh_gui_decode;                                                                                                                    \
__obfh_gui_cached:                                                                                                                             \
    __obfh_asm__(""                                                                                                                            \
                 :                                                                                                                             \
                 :                                                                                                                             \
                 : "memory");                                                                                                                  \
    __obfh_gui_value = (ULONG_PTR)obfh_gui_slots[index].encoded;                                                                               \
__obfh_gui_decode:                                                                                                                             \
    __builtin_choose_expr(__obfh_gui_form == 0, OBFH_GUI_FORM_0(index),                                                                        \
                          __builtin_choose_expr(__obfh_gui_form == 1, OBFH_GUI_FORM_1(index),                                                  \
                                                __builtin_choose_expr(__obfh_gui_form == 2, OBFH_GUI_FORM_2(index), OBFH_GUI_FORM_3(index)))); \
    ((type)__obfh_gui_value)(__VA_ARGS__);                                                                                                     \
})

// One typed front end for every cached API; names remain compile-time data.
#define OBFH_API_CALL(module, name, ...) \
    OBFH_GUI_CALL(OBFH_GUI_ID_##name, module, OBFH_GUI_NAME_##name, __typeof__(&name), __VA_ARGS__)

#define OBFH_GUI_ID_AppendMenuA 0
#define OBFH_GUI_NAME_AppendMenuA \
    { (65u ^ OBFH_GUI_NAME_KEY(0)), (112u ^ OBFH_GUI_NAME_KEY(0)), (112u ^ OBFH_GUI_NAME_KEY(0)), (101u ^ OBFH_GUI_NAME_KEY(0)), (110u ^ OBFH_GUI_NAME_KEY(0)), (100u ^ OBFH_GUI_NAME_KEY(0)), (77u ^ OBFH_GUI_NAME_KEY(0)), (101u ^ OBFH_GUI_NAME_KEY(0)), (110u ^ OBFH_GUI_NAME_KEY(0)), (117u ^ OBFH_GUI_NAME_KEY(0)), (65u ^ OBFH_GUI_NAME_KEY(0)), (0u ^ OBFH_GUI_NAME_KEY(0)) }
#define AppendMenuA(...) OBFH_API_CALL(0, AppendMenuA, __VA_ARGS__)

#define OBFH_GUI_ID_AppendMenuW 1
#define OBFH_GUI_NAME_AppendMenuW \
    { (65u ^ OBFH_GUI_NAME_KEY(1)), (112u ^ OBFH_GUI_NAME_KEY(1)), (112u ^ OBFH_GUI_NAME_KEY(1)), (101u ^ OBFH_GUI_NAME_KEY(1)), (110u ^ OBFH_GUI_NAME_KEY(1)), (100u ^ OBFH_GUI_NAME_KEY(1)), (77u ^ OBFH_GUI_NAME_KEY(1)), (101u ^ OBFH_GUI_NAME_KEY(1)), (110u ^ OBFH_GUI_NAME_KEY(1)), (117u ^ OBFH_GUI_NAME_KEY(1)), (87u ^ OBFH_GUI_NAME_KEY(1)), (0u ^ OBFH_GUI_NAME_KEY(1)) }
#define AppendMenuW(...) OBFH_API_CALL(0, AppendMenuW, __VA_ARGS__)

#define OBFH_GUI_ID_CheckMenuItem 2
#define OBFH_GUI_NAME_CheckMenuItem \
    { (67u ^ OBFH_GUI_NAME_KEY(2)), (104u ^ OBFH_GUI_NAME_KEY(2)), (101u ^ OBFH_GUI_NAME_KEY(2)), (99u ^ OBFH_GUI_NAME_KEY(2)), (107u ^ OBFH_GUI_NAME_KEY(2)), (77u ^ OBFH_GUI_NAME_KEY(2)), (101u ^ OBFH_GUI_NAME_KEY(2)), (110u ^ OBFH_GUI_NAME_KEY(2)), (117u ^ OBFH_GUI_NAME_KEY(2)), (73u ^ OBFH_GUI_NAME_KEY(2)), (116u ^ OBFH_GUI_NAME_KEY(2)), (101u ^ OBFH_GUI_NAME_KEY(2)), (109u ^ OBFH_GUI_NAME_KEY(2)), (0u ^ OBFH_GUI_NAME_KEY(2)) }
#define CheckMenuItem(...) OBFH_API_CALL(0, CheckMenuItem, __VA_ARGS__)

#define OBFH_GUI_ID_CloseClipboard 3
#define OBFH_GUI_NAME_CloseClipboard \
    { (67u ^ OBFH_GUI_NAME_KEY(3)), (108u ^ OBFH_GUI_NAME_KEY(3)), (111u ^ OBFH_GUI_NAME_KEY(3)), (115u ^ OBFH_GUI_NAME_KEY(3)), (101u ^ OBFH_GUI_NAME_KEY(3)), (67u ^ OBFH_GUI_NAME_KEY(3)), (108u ^ OBFH_GUI_NAME_KEY(3)), (105u ^ OBFH_GUI_NAME_KEY(3)), (112u ^ OBFH_GUI_NAME_KEY(3)), (98u ^ OBFH_GUI_NAME_KEY(3)), (111u ^ OBFH_GUI_NAME_KEY(3)), (97u ^ OBFH_GUI_NAME_KEY(3)), (114u ^ OBFH_GUI_NAME_KEY(3)), (100u ^ OBFH_GUI_NAME_KEY(3)), (0u ^ OBFH_GUI_NAME_KEY(3)) }
#define CloseClipboard(...) OBFH_API_CALL(0, CloseClipboard, __VA_ARGS__)

#define OBFH_GUI_ID_CreateMenu 4
#define OBFH_GUI_NAME_CreateMenu \
    { (67u ^ OBFH_GUI_NAME_KEY(4)), (114u ^ OBFH_GUI_NAME_KEY(4)), (101u ^ OBFH_GUI_NAME_KEY(4)), (97u ^ OBFH_GUI_NAME_KEY(4)), (116u ^ OBFH_GUI_NAME_KEY(4)), (101u ^ OBFH_GUI_NAME_KEY(4)), (77u ^ OBFH_GUI_NAME_KEY(4)), (101u ^ OBFH_GUI_NAME_KEY(4)), (110u ^ OBFH_GUI_NAME_KEY(4)), (117u ^ OBFH_GUI_NAME_KEY(4)), (0u ^ OBFH_GUI_NAME_KEY(4)) }
#define CreateMenu(...) OBFH_API_CALL(0, CreateMenu, __VA_ARGS__)

#define OBFH_GUI_ID_CreatePopupMenu 5
#define OBFH_GUI_NAME_CreatePopupMenu \
    { (67u ^ OBFH_GUI_NAME_KEY(5)), (114u ^ OBFH_GUI_NAME_KEY(5)), (101u ^ OBFH_GUI_NAME_KEY(5)), (97u ^ OBFH_GUI_NAME_KEY(5)), (116u ^ OBFH_GUI_NAME_KEY(5)), (101u ^ OBFH_GUI_NAME_KEY(5)), (80u ^ OBFH_GUI_NAME_KEY(5)), (111u ^ OBFH_GUI_NAME_KEY(5)), (112u ^ OBFH_GUI_NAME_KEY(5)), (117u ^ OBFH_GUI_NAME_KEY(5)), (112u ^ OBFH_GUI_NAME_KEY(5)), (77u ^ OBFH_GUI_NAME_KEY(5)), (101u ^ OBFH_GUI_NAME_KEY(5)), (110u ^ OBFH_GUI_NAME_KEY(5)), (117u ^ OBFH_GUI_NAME_KEY(5)), (0u ^ OBFH_GUI_NAME_KEY(5)) }
#define CreatePopupMenu(...) OBFH_API_CALL(0, CreatePopupMenu, __VA_ARGS__)

#define OBFH_GUI_ID_CreateWindowExA 6
#define OBFH_GUI_NAME_CreateWindowExA \
    { (67u ^ OBFH_GUI_NAME_KEY(6)), (114u ^ OBFH_GUI_NAME_KEY(6)), (101u ^ OBFH_GUI_NAME_KEY(6)), (97u ^ OBFH_GUI_NAME_KEY(6)), (116u ^ OBFH_GUI_NAME_KEY(6)), (101u ^ OBFH_GUI_NAME_KEY(6)), (87u ^ OBFH_GUI_NAME_KEY(6)), (105u ^ OBFH_GUI_NAME_KEY(6)), (110u ^ OBFH_GUI_NAME_KEY(6)), (100u ^ OBFH_GUI_NAME_KEY(6)), (111u ^ OBFH_GUI_NAME_KEY(6)), (119u ^ OBFH_GUI_NAME_KEY(6)), (69u ^ OBFH_GUI_NAME_KEY(6)), (120u ^ OBFH_GUI_NAME_KEY(6)), (65u ^ OBFH_GUI_NAME_KEY(6)), (0u ^ OBFH_GUI_NAME_KEY(6)) }
#define CreateWindowExA(...) OBFH_API_CALL(0, CreateWindowExA, __VA_ARGS__)

#define OBFH_GUI_ID_CreateWindowExW 7
#define OBFH_GUI_NAME_CreateWindowExW \
    { (67u ^ OBFH_GUI_NAME_KEY(7)), (114u ^ OBFH_GUI_NAME_KEY(7)), (101u ^ OBFH_GUI_NAME_KEY(7)), (97u ^ OBFH_GUI_NAME_KEY(7)), (116u ^ OBFH_GUI_NAME_KEY(7)), (101u ^ OBFH_GUI_NAME_KEY(7)), (87u ^ OBFH_GUI_NAME_KEY(7)), (105u ^ OBFH_GUI_NAME_KEY(7)), (110u ^ OBFH_GUI_NAME_KEY(7)), (100u ^ OBFH_GUI_NAME_KEY(7)), (111u ^ OBFH_GUI_NAME_KEY(7)), (119u ^ OBFH_GUI_NAME_KEY(7)), (69u ^ OBFH_GUI_NAME_KEY(7)), (120u ^ OBFH_GUI_NAME_KEY(7)), (87u ^ OBFH_GUI_NAME_KEY(7)), (0u ^ OBFH_GUI_NAME_KEY(7)) }
#define CreateWindowExW(...) OBFH_API_CALL(0, CreateWindowExW, __VA_ARGS__)

#define OBFH_GUI_ID_DefWindowProcA 8
#define OBFH_GUI_NAME_DefWindowProcA \
    { (68u ^ OBFH_GUI_NAME_KEY(8)), (101u ^ OBFH_GUI_NAME_KEY(8)), (102u ^ OBFH_GUI_NAME_KEY(8)), (87u ^ OBFH_GUI_NAME_KEY(8)), (105u ^ OBFH_GUI_NAME_KEY(8)), (110u ^ OBFH_GUI_NAME_KEY(8)), (100u ^ OBFH_GUI_NAME_KEY(8)), (111u ^ OBFH_GUI_NAME_KEY(8)), (119u ^ OBFH_GUI_NAME_KEY(8)), (80u ^ OBFH_GUI_NAME_KEY(8)), (114u ^ OBFH_GUI_NAME_KEY(8)), (111u ^ OBFH_GUI_NAME_KEY(8)), (99u ^ OBFH_GUI_NAME_KEY(8)), (65u ^ OBFH_GUI_NAME_KEY(8)), (0u ^ OBFH_GUI_NAME_KEY(8)) }
#define DefWindowProcA(...) OBFH_API_CALL(0, DefWindowProcA, __VA_ARGS__)

#define OBFH_GUI_ID_DefWindowProcW 9
#define OBFH_GUI_NAME_DefWindowProcW \
    { (68u ^ OBFH_GUI_NAME_KEY(9)), (101u ^ OBFH_GUI_NAME_KEY(9)), (102u ^ OBFH_GUI_NAME_KEY(9)), (87u ^ OBFH_GUI_NAME_KEY(9)), (105u ^ OBFH_GUI_NAME_KEY(9)), (110u ^ OBFH_GUI_NAME_KEY(9)), (100u ^ OBFH_GUI_NAME_KEY(9)), (111u ^ OBFH_GUI_NAME_KEY(9)), (119u ^ OBFH_GUI_NAME_KEY(9)), (80u ^ OBFH_GUI_NAME_KEY(9)), (114u ^ OBFH_GUI_NAME_KEY(9)), (111u ^ OBFH_GUI_NAME_KEY(9)), (99u ^ OBFH_GUI_NAME_KEY(9)), (87u ^ OBFH_GUI_NAME_KEY(9)), (0u ^ OBFH_GUI_NAME_KEY(9)) }
#define DefWindowProcW(...) OBFH_API_CALL(0, DefWindowProcW, __VA_ARGS__)

#define OBFH_GUI_ID_DestroyWindow 10
#define OBFH_GUI_NAME_DestroyWindow \
    { (68u ^ OBFH_GUI_NAME_KEY(10)), (101u ^ OBFH_GUI_NAME_KEY(10)), (115u ^ OBFH_GUI_NAME_KEY(10)), (116u ^ OBFH_GUI_NAME_KEY(10)), (114u ^ OBFH_GUI_NAME_KEY(10)), (111u ^ OBFH_GUI_NAME_KEY(10)), (121u ^ OBFH_GUI_NAME_KEY(10)), (87u ^ OBFH_GUI_NAME_KEY(10)), (105u ^ OBFH_GUI_NAME_KEY(10)), (110u ^ OBFH_GUI_NAME_KEY(10)), (100u ^ OBFH_GUI_NAME_KEY(10)), (111u ^ OBFH_GUI_NAME_KEY(10)), (119u ^ OBFH_GUI_NAME_KEY(10)), (0u ^ OBFH_GUI_NAME_KEY(10)) }
#define DestroyWindow(...) OBFH_API_CALL(0, DestroyWindow, __VA_ARGS__)

#define OBFH_GUI_ID_DispatchMessageA 11
#define OBFH_GUI_NAME_DispatchMessageA \
    { (68u ^ OBFH_GUI_NAME_KEY(11)), (105u ^ OBFH_GUI_NAME_KEY(11)), (115u ^ OBFH_GUI_NAME_KEY(11)), (112u ^ OBFH_GUI_NAME_KEY(11)), (97u ^ OBFH_GUI_NAME_KEY(11)), (116u ^ OBFH_GUI_NAME_KEY(11)), (99u ^ OBFH_GUI_NAME_KEY(11)), (104u ^ OBFH_GUI_NAME_KEY(11)), (77u ^ OBFH_GUI_NAME_KEY(11)), (101u ^ OBFH_GUI_NAME_KEY(11)), (115u ^ OBFH_GUI_NAME_KEY(11)), (115u ^ OBFH_GUI_NAME_KEY(11)), (97u ^ OBFH_GUI_NAME_KEY(11)), (103u ^ OBFH_GUI_NAME_KEY(11)), (101u ^ OBFH_GUI_NAME_KEY(11)), (65u ^ OBFH_GUI_NAME_KEY(11)), (0u ^ OBFH_GUI_NAME_KEY(11)) }
#define DispatchMessageA(...) OBFH_API_CALL(0, DispatchMessageA, __VA_ARGS__)

#define OBFH_GUI_ID_DispatchMessageW 12
#define OBFH_GUI_NAME_DispatchMessageW \
    { (68u ^ OBFH_GUI_NAME_KEY(12)), (105u ^ OBFH_GUI_NAME_KEY(12)), (115u ^ OBFH_GUI_NAME_KEY(12)), (112u ^ OBFH_GUI_NAME_KEY(12)), (97u ^ OBFH_GUI_NAME_KEY(12)), (116u ^ OBFH_GUI_NAME_KEY(12)), (99u ^ OBFH_GUI_NAME_KEY(12)), (104u ^ OBFH_GUI_NAME_KEY(12)), (77u ^ OBFH_GUI_NAME_KEY(12)), (101u ^ OBFH_GUI_NAME_KEY(12)), (115u ^ OBFH_GUI_NAME_KEY(12)), (115u ^ OBFH_GUI_NAME_KEY(12)), (97u ^ OBFH_GUI_NAME_KEY(12)), (103u ^ OBFH_GUI_NAME_KEY(12)), (101u ^ OBFH_GUI_NAME_KEY(12)), (87u ^ OBFH_GUI_NAME_KEY(12)), (0u ^ OBFH_GUI_NAME_KEY(12)) }
#define DispatchMessageW(...) OBFH_API_CALL(0, DispatchMessageW, __VA_ARGS__)

#define OBFH_GUI_ID_EmptyClipboard 13
#define OBFH_GUI_NAME_EmptyClipboard \
    { (69u ^ OBFH_GUI_NAME_KEY(13)), (109u ^ OBFH_GUI_NAME_KEY(13)), (112u ^ OBFH_GUI_NAME_KEY(13)), (116u ^ OBFH_GUI_NAME_KEY(13)), (121u ^ OBFH_GUI_NAME_KEY(13)), (67u ^ OBFH_GUI_NAME_KEY(13)), (108u ^ OBFH_GUI_NAME_KEY(13)), (105u ^ OBFH_GUI_NAME_KEY(13)), (112u ^ OBFH_GUI_NAME_KEY(13)), (98u ^ OBFH_GUI_NAME_KEY(13)), (111u ^ OBFH_GUI_NAME_KEY(13)), (97u ^ OBFH_GUI_NAME_KEY(13)), (114u ^ OBFH_GUI_NAME_KEY(13)), (100u ^ OBFH_GUI_NAME_KEY(13)), (0u ^ OBFH_GUI_NAME_KEY(13)) }
#define EmptyClipboard(...) OBFH_API_CALL(0, EmptyClipboard, __VA_ARGS__)

#define OBFH_GUI_ID_GetClientRect 14
#define OBFH_GUI_NAME_GetClientRect \
    { (71u ^ OBFH_GUI_NAME_KEY(14)), (101u ^ OBFH_GUI_NAME_KEY(14)), (116u ^ OBFH_GUI_NAME_KEY(14)), (67u ^ OBFH_GUI_NAME_KEY(14)), (108u ^ OBFH_GUI_NAME_KEY(14)), (105u ^ OBFH_GUI_NAME_KEY(14)), (101u ^ OBFH_GUI_NAME_KEY(14)), (110u ^ OBFH_GUI_NAME_KEY(14)), (116u ^ OBFH_GUI_NAME_KEY(14)), (82u ^ OBFH_GUI_NAME_KEY(14)), (101u ^ OBFH_GUI_NAME_KEY(14)), (99u ^ OBFH_GUI_NAME_KEY(14)), (116u ^ OBFH_GUI_NAME_KEY(14)), (0u ^ OBFH_GUI_NAME_KEY(14)) }
#define GetClientRect(...) OBFH_API_CALL(0, GetClientRect, __VA_ARGS__)

#define OBFH_GUI_ID_GetDC 15
#define OBFH_GUI_NAME_GetDC \
    { (71u ^ OBFH_GUI_NAME_KEY(15)), (101u ^ OBFH_GUI_NAME_KEY(15)), (116u ^ OBFH_GUI_NAME_KEY(15)), (68u ^ OBFH_GUI_NAME_KEY(15)), (67u ^ OBFH_GUI_NAME_KEY(15)), (0u ^ OBFH_GUI_NAME_KEY(15)) }
#define GetDC(...) OBFH_API_CALL(0, GetDC, __VA_ARGS__)

#define OBFH_GUI_ID_GetDlgItem 16
#define OBFH_GUI_NAME_GetDlgItem \
    { (71u ^ OBFH_GUI_NAME_KEY(16)), (101u ^ OBFH_GUI_NAME_KEY(16)), (116u ^ OBFH_GUI_NAME_KEY(16)), (68u ^ OBFH_GUI_NAME_KEY(16)), (108u ^ OBFH_GUI_NAME_KEY(16)), (103u ^ OBFH_GUI_NAME_KEY(16)), (73u ^ OBFH_GUI_NAME_KEY(16)), (116u ^ OBFH_GUI_NAME_KEY(16)), (101u ^ OBFH_GUI_NAME_KEY(16)), (109u ^ OBFH_GUI_NAME_KEY(16)), (0u ^ OBFH_GUI_NAME_KEY(16)) }
#define GetDlgItem(...) OBFH_API_CALL(0, GetDlgItem, __VA_ARGS__)

#define OBFH_GUI_ID_GetKeyState 17
#define OBFH_GUI_NAME_GetKeyState \
    { (71u ^ OBFH_GUI_NAME_KEY(17)), (101u ^ OBFH_GUI_NAME_KEY(17)), (116u ^ OBFH_GUI_NAME_KEY(17)), (75u ^ OBFH_GUI_NAME_KEY(17)), (101u ^ OBFH_GUI_NAME_KEY(17)), (121u ^ OBFH_GUI_NAME_KEY(17)), (83u ^ OBFH_GUI_NAME_KEY(17)), (116u ^ OBFH_GUI_NAME_KEY(17)), (97u ^ OBFH_GUI_NAME_KEY(17)), (116u ^ OBFH_GUI_NAME_KEY(17)), (101u ^ OBFH_GUI_NAME_KEY(17)), (0u ^ OBFH_GUI_NAME_KEY(17)) }
#define GetKeyState(...) OBFH_API_CALL(0, GetKeyState, __VA_ARGS__)

#define OBFH_GUI_ID_GetMenu 18
#define OBFH_GUI_NAME_GetMenu \
    { (71u ^ OBFH_GUI_NAME_KEY(18)), (101u ^ OBFH_GUI_NAME_KEY(18)), (116u ^ OBFH_GUI_NAME_KEY(18)), (77u ^ OBFH_GUI_NAME_KEY(18)), (101u ^ OBFH_GUI_NAME_KEY(18)), (110u ^ OBFH_GUI_NAME_KEY(18)), (117u ^ OBFH_GUI_NAME_KEY(18)), (0u ^ OBFH_GUI_NAME_KEY(18)) }
#define GetMenu(...) OBFH_API_CALL(0, GetMenu, __VA_ARGS__)

#define OBFH_GUI_ID_GetMessageA 19
#define OBFH_GUI_NAME_GetMessageA \
    { (71u ^ OBFH_GUI_NAME_KEY(19)), (101u ^ OBFH_GUI_NAME_KEY(19)), (116u ^ OBFH_GUI_NAME_KEY(19)), (77u ^ OBFH_GUI_NAME_KEY(19)), (101u ^ OBFH_GUI_NAME_KEY(19)), (115u ^ OBFH_GUI_NAME_KEY(19)), (115u ^ OBFH_GUI_NAME_KEY(19)), (97u ^ OBFH_GUI_NAME_KEY(19)), (103u ^ OBFH_GUI_NAME_KEY(19)), (101u ^ OBFH_GUI_NAME_KEY(19)), (65u ^ OBFH_GUI_NAME_KEY(19)), (0u ^ OBFH_GUI_NAME_KEY(19)) }
#define GetMessageA(...) OBFH_API_CALL(0, GetMessageA, __VA_ARGS__)

#define OBFH_GUI_ID_GetMessageW 20
#define OBFH_GUI_NAME_GetMessageW \
    { (71u ^ OBFH_GUI_NAME_KEY(20)), (101u ^ OBFH_GUI_NAME_KEY(20)), (116u ^ OBFH_GUI_NAME_KEY(20)), (77u ^ OBFH_GUI_NAME_KEY(20)), (101u ^ OBFH_GUI_NAME_KEY(20)), (115u ^ OBFH_GUI_NAME_KEY(20)), (115u ^ OBFH_GUI_NAME_KEY(20)), (97u ^ OBFH_GUI_NAME_KEY(20)), (103u ^ OBFH_GUI_NAME_KEY(20)), (101u ^ OBFH_GUI_NAME_KEY(20)), (87u ^ OBFH_GUI_NAME_KEY(20)), (0u ^ OBFH_GUI_NAME_KEY(20)) }
#define GetMessageW(...) OBFH_API_CALL(0, GetMessageW, __VA_ARGS__)

#define OBFH_GUI_ID_GetSysColor 21
#define OBFH_GUI_NAME_GetSysColor \
    { (71u ^ OBFH_GUI_NAME_KEY(21)), (101u ^ OBFH_GUI_NAME_KEY(21)), (116u ^ OBFH_GUI_NAME_KEY(21)), (83u ^ OBFH_GUI_NAME_KEY(21)), (121u ^ OBFH_GUI_NAME_KEY(21)), (115u ^ OBFH_GUI_NAME_KEY(21)), (67u ^ OBFH_GUI_NAME_KEY(21)), (111u ^ OBFH_GUI_NAME_KEY(21)), (108u ^ OBFH_GUI_NAME_KEY(21)), (111u ^ OBFH_GUI_NAME_KEY(21)), (114u ^ OBFH_GUI_NAME_KEY(21)), (0u ^ OBFH_GUI_NAME_KEY(21)) }
#define GetSysColor(...) OBFH_API_CALL(0, GetSysColor, __VA_ARGS__)

#define OBFH_GUI_ID_GetSysColorBrush 22
#define OBFH_GUI_NAME_GetSysColorBrush \
    { (71u ^ OBFH_GUI_NAME_KEY(22)), (101u ^ OBFH_GUI_NAME_KEY(22)), (116u ^ OBFH_GUI_NAME_KEY(22)), (83u ^ OBFH_GUI_NAME_KEY(22)), (121u ^ OBFH_GUI_NAME_KEY(22)), (115u ^ OBFH_GUI_NAME_KEY(22)), (67u ^ OBFH_GUI_NAME_KEY(22)), (111u ^ OBFH_GUI_NAME_KEY(22)), (108u ^ OBFH_GUI_NAME_KEY(22)), (111u ^ OBFH_GUI_NAME_KEY(22)), (114u ^ OBFH_GUI_NAME_KEY(22)), (66u ^ OBFH_GUI_NAME_KEY(22)), (114u ^ OBFH_GUI_NAME_KEY(22)), (117u ^ OBFH_GUI_NAME_KEY(22)), (115u ^ OBFH_GUI_NAME_KEY(22)), (104u ^ OBFH_GUI_NAME_KEY(22)), (0u ^ OBFH_GUI_NAME_KEY(22)) }
#define GetSysColorBrush(...) OBFH_API_CALL(0, GetSysColorBrush, __VA_ARGS__)

#define OBFH_GUI_ID_GetSystemMetrics 23
#define OBFH_GUI_NAME_GetSystemMetrics \
    { (71u ^ OBFH_GUI_NAME_KEY(23)), (101u ^ OBFH_GUI_NAME_KEY(23)), (116u ^ OBFH_GUI_NAME_KEY(23)), (83u ^ OBFH_GUI_NAME_KEY(23)), (121u ^ OBFH_GUI_NAME_KEY(23)), (115u ^ OBFH_GUI_NAME_KEY(23)), (116u ^ OBFH_GUI_NAME_KEY(23)), (101u ^ OBFH_GUI_NAME_KEY(23)), (109u ^ OBFH_GUI_NAME_KEY(23)), (77u ^ OBFH_GUI_NAME_KEY(23)), (101u ^ OBFH_GUI_NAME_KEY(23)), (116u ^ OBFH_GUI_NAME_KEY(23)), (114u ^ OBFH_GUI_NAME_KEY(23)), (105u ^ OBFH_GUI_NAME_KEY(23)), (99u ^ OBFH_GUI_NAME_KEY(23)), (115u ^ OBFH_GUI_NAME_KEY(23)), (0u ^ OBFH_GUI_NAME_KEY(23)) }
#define GetSystemMetrics(...) OBFH_API_CALL(0, GetSystemMetrics, __VA_ARGS__)

#define OBFH_GUI_ID_GetWindowRect 24
#define OBFH_GUI_NAME_GetWindowRect \
    { (71u ^ OBFH_GUI_NAME_KEY(24)), (101u ^ OBFH_GUI_NAME_KEY(24)), (116u ^ OBFH_GUI_NAME_KEY(24)), (87u ^ OBFH_GUI_NAME_KEY(24)), (105u ^ OBFH_GUI_NAME_KEY(24)), (110u ^ OBFH_GUI_NAME_KEY(24)), (100u ^ OBFH_GUI_NAME_KEY(24)), (111u ^ OBFH_GUI_NAME_KEY(24)), (119u ^ OBFH_GUI_NAME_KEY(24)), (82u ^ OBFH_GUI_NAME_KEY(24)), (101u ^ OBFH_GUI_NAME_KEY(24)), (99u ^ OBFH_GUI_NAME_KEY(24)), (116u ^ OBFH_GUI_NAME_KEY(24)), (0u ^ OBFH_GUI_NAME_KEY(24)) }
#define GetWindowRect(...) OBFH_API_CALL(0, GetWindowRect, __VA_ARGS__)

#define OBFH_GUI_ID_GetWindowTextA 25
#define OBFH_GUI_NAME_GetWindowTextA \
    { (71u ^ OBFH_GUI_NAME_KEY(25)), (101u ^ OBFH_GUI_NAME_KEY(25)), (116u ^ OBFH_GUI_NAME_KEY(25)), (87u ^ OBFH_GUI_NAME_KEY(25)), (105u ^ OBFH_GUI_NAME_KEY(25)), (110u ^ OBFH_GUI_NAME_KEY(25)), (100u ^ OBFH_GUI_NAME_KEY(25)), (111u ^ OBFH_GUI_NAME_KEY(25)), (119u ^ OBFH_GUI_NAME_KEY(25)), (84u ^ OBFH_GUI_NAME_KEY(25)), (101u ^ OBFH_GUI_NAME_KEY(25)), (120u ^ OBFH_GUI_NAME_KEY(25)), (116u ^ OBFH_GUI_NAME_KEY(25)), (65u ^ OBFH_GUI_NAME_KEY(25)), (0u ^ OBFH_GUI_NAME_KEY(25)) }
#define GetWindowTextA(...) OBFH_API_CALL(0, GetWindowTextA, __VA_ARGS__)

#define OBFH_GUI_ID_GetWindowTextW 26
#define OBFH_GUI_NAME_GetWindowTextW \
    { (71u ^ OBFH_GUI_NAME_KEY(26)), (101u ^ OBFH_GUI_NAME_KEY(26)), (116u ^ OBFH_GUI_NAME_KEY(26)), (87u ^ OBFH_GUI_NAME_KEY(26)), (105u ^ OBFH_GUI_NAME_KEY(26)), (110u ^ OBFH_GUI_NAME_KEY(26)), (100u ^ OBFH_GUI_NAME_KEY(26)), (111u ^ OBFH_GUI_NAME_KEY(26)), (119u ^ OBFH_GUI_NAME_KEY(26)), (84u ^ OBFH_GUI_NAME_KEY(26)), (101u ^ OBFH_GUI_NAME_KEY(26)), (120u ^ OBFH_GUI_NAME_KEY(26)), (116u ^ OBFH_GUI_NAME_KEY(26)), (87u ^ OBFH_GUI_NAME_KEY(26)), (0u ^ OBFH_GUI_NAME_KEY(26)) }
#define GetWindowTextW(...) OBFH_API_CALL(0, GetWindowTextW, __VA_ARGS__)

#define OBFH_GUI_ID_GetWindowTextLengthA 27
#define OBFH_GUI_NAME_GetWindowTextLengthA \
    { (71u ^ OBFH_GUI_NAME_KEY(27)), (101u ^ OBFH_GUI_NAME_KEY(27)), (116u ^ OBFH_GUI_NAME_KEY(27)), (87u ^ OBFH_GUI_NAME_KEY(27)), (105u ^ OBFH_GUI_NAME_KEY(27)), (110u ^ OBFH_GUI_NAME_KEY(27)), (100u ^ OBFH_GUI_NAME_KEY(27)), (111u ^ OBFH_GUI_NAME_KEY(27)), (119u ^ OBFH_GUI_NAME_KEY(27)), (84u ^ OBFH_GUI_NAME_KEY(27)), (101u ^ OBFH_GUI_NAME_KEY(27)), (120u ^ OBFH_GUI_NAME_KEY(27)), (116u ^ OBFH_GUI_NAME_KEY(27)), (76u ^ OBFH_GUI_NAME_KEY(27)), (101u ^ OBFH_GUI_NAME_KEY(27)), (110u ^ OBFH_GUI_NAME_KEY(27)), (103u ^ OBFH_GUI_NAME_KEY(27)), (116u ^ OBFH_GUI_NAME_KEY(27)), (104u ^ OBFH_GUI_NAME_KEY(27)), (65u ^ OBFH_GUI_NAME_KEY(27)), (0u ^ OBFH_GUI_NAME_KEY(27)) }
#define GetWindowTextLengthA(...) OBFH_API_CALL(0, GetWindowTextLengthA, __VA_ARGS__)

#define OBFH_GUI_ID_GetWindowTextLengthW 28
#define OBFH_GUI_NAME_GetWindowTextLengthW \
    { (71u ^ OBFH_GUI_NAME_KEY(28)), (101u ^ OBFH_GUI_NAME_KEY(28)), (116u ^ OBFH_GUI_NAME_KEY(28)), (87u ^ OBFH_GUI_NAME_KEY(28)), (105u ^ OBFH_GUI_NAME_KEY(28)), (110u ^ OBFH_GUI_NAME_KEY(28)), (100u ^ OBFH_GUI_NAME_KEY(28)), (111u ^ OBFH_GUI_NAME_KEY(28)), (119u ^ OBFH_GUI_NAME_KEY(28)), (84u ^ OBFH_GUI_NAME_KEY(28)), (101u ^ OBFH_GUI_NAME_KEY(28)), (120u ^ OBFH_GUI_NAME_KEY(28)), (116u ^ OBFH_GUI_NAME_KEY(28)), (76u ^ OBFH_GUI_NAME_KEY(28)), (101u ^ OBFH_GUI_NAME_KEY(28)), (110u ^ OBFH_GUI_NAME_KEY(28)), (103u ^ OBFH_GUI_NAME_KEY(28)), (116u ^ OBFH_GUI_NAME_KEY(28)), (104u ^ OBFH_GUI_NAME_KEY(28)), (87u ^ OBFH_GUI_NAME_KEY(28)), (0u ^ OBFH_GUI_NAME_KEY(28)) }
#define GetWindowTextLengthW(...) OBFH_API_CALL(0, GetWindowTextLengthW, __VA_ARGS__)

#define OBFH_GUI_ID_IsDialogMessageA 29
#define OBFH_GUI_NAME_IsDialogMessageA \
    { (73u ^ OBFH_GUI_NAME_KEY(29)), (115u ^ OBFH_GUI_NAME_KEY(29)), (68u ^ OBFH_GUI_NAME_KEY(29)), (105u ^ OBFH_GUI_NAME_KEY(29)), (97u ^ OBFH_GUI_NAME_KEY(29)), (108u ^ OBFH_GUI_NAME_KEY(29)), (111u ^ OBFH_GUI_NAME_KEY(29)), (103u ^ OBFH_GUI_NAME_KEY(29)), (77u ^ OBFH_GUI_NAME_KEY(29)), (101u ^ OBFH_GUI_NAME_KEY(29)), (115u ^ OBFH_GUI_NAME_KEY(29)), (115u ^ OBFH_GUI_NAME_KEY(29)), (97u ^ OBFH_GUI_NAME_KEY(29)), (103u ^ OBFH_GUI_NAME_KEY(29)), (101u ^ OBFH_GUI_NAME_KEY(29)), (65u ^ OBFH_GUI_NAME_KEY(29)), (0u ^ OBFH_GUI_NAME_KEY(29)) }
#define IsDialogMessageA(...) OBFH_API_CALL(0, IsDialogMessageA, __VA_ARGS__)

#define OBFH_GUI_ID_IsDialogMessageW 30
#define OBFH_GUI_NAME_IsDialogMessageW \
    { (73u ^ OBFH_GUI_NAME_KEY(30)), (115u ^ OBFH_GUI_NAME_KEY(30)), (68u ^ OBFH_GUI_NAME_KEY(30)), (105u ^ OBFH_GUI_NAME_KEY(30)), (97u ^ OBFH_GUI_NAME_KEY(30)), (108u ^ OBFH_GUI_NAME_KEY(30)), (111u ^ OBFH_GUI_NAME_KEY(30)), (103u ^ OBFH_GUI_NAME_KEY(30)), (77u ^ OBFH_GUI_NAME_KEY(30)), (101u ^ OBFH_GUI_NAME_KEY(30)), (115u ^ OBFH_GUI_NAME_KEY(30)), (115u ^ OBFH_GUI_NAME_KEY(30)), (97u ^ OBFH_GUI_NAME_KEY(30)), (103u ^ OBFH_GUI_NAME_KEY(30)), (101u ^ OBFH_GUI_NAME_KEY(30)), (87u ^ OBFH_GUI_NAME_KEY(30)), (0u ^ OBFH_GUI_NAME_KEY(30)) }
#define IsDialogMessageW(...) OBFH_API_CALL(0, IsDialogMessageW, __VA_ARGS__)

#define OBFH_GUI_ID_LoadCursorA 31
#define OBFH_GUI_NAME_LoadCursorA \
    { (76u ^ OBFH_GUI_NAME_KEY(31)), (111u ^ OBFH_GUI_NAME_KEY(31)), (97u ^ OBFH_GUI_NAME_KEY(31)), (100u ^ OBFH_GUI_NAME_KEY(31)), (67u ^ OBFH_GUI_NAME_KEY(31)), (117u ^ OBFH_GUI_NAME_KEY(31)), (114u ^ OBFH_GUI_NAME_KEY(31)), (115u ^ OBFH_GUI_NAME_KEY(31)), (111u ^ OBFH_GUI_NAME_KEY(31)), (114u ^ OBFH_GUI_NAME_KEY(31)), (65u ^ OBFH_GUI_NAME_KEY(31)), (0u ^ OBFH_GUI_NAME_KEY(31)) }
#define LoadCursorA(...) OBFH_API_CALL(0, LoadCursorA, __VA_ARGS__)

#define OBFH_GUI_ID_LoadCursorW 32
#define OBFH_GUI_NAME_LoadCursorW \
    { (76u ^ OBFH_GUI_NAME_KEY(32)), (111u ^ OBFH_GUI_NAME_KEY(32)), (97u ^ OBFH_GUI_NAME_KEY(32)), (100u ^ OBFH_GUI_NAME_KEY(32)), (67u ^ OBFH_GUI_NAME_KEY(32)), (117u ^ OBFH_GUI_NAME_KEY(32)), (114u ^ OBFH_GUI_NAME_KEY(32)), (115u ^ OBFH_GUI_NAME_KEY(32)), (111u ^ OBFH_GUI_NAME_KEY(32)), (114u ^ OBFH_GUI_NAME_KEY(32)), (87u ^ OBFH_GUI_NAME_KEY(32)), (0u ^ OBFH_GUI_NAME_KEY(32)) }
#define LoadCursorW(...) OBFH_API_CALL(0, LoadCursorW, __VA_ARGS__)

#define OBFH_GUI_ID_LoadIconA 33
#define OBFH_GUI_NAME_LoadIconA \
    { (76u ^ OBFH_GUI_NAME_KEY(33)), (111u ^ OBFH_GUI_NAME_KEY(33)), (97u ^ OBFH_GUI_NAME_KEY(33)), (100u ^ OBFH_GUI_NAME_KEY(33)), (73u ^ OBFH_GUI_NAME_KEY(33)), (99u ^ OBFH_GUI_NAME_KEY(33)), (111u ^ OBFH_GUI_NAME_KEY(33)), (110u ^ OBFH_GUI_NAME_KEY(33)), (65u ^ OBFH_GUI_NAME_KEY(33)), (0u ^ OBFH_GUI_NAME_KEY(33)) }
#define LoadIconA(...) OBFH_API_CALL(0, LoadIconA, __VA_ARGS__)

#define OBFH_GUI_ID_LoadIconW 34
#define OBFH_GUI_NAME_LoadIconW \
    { (76u ^ OBFH_GUI_NAME_KEY(34)), (111u ^ OBFH_GUI_NAME_KEY(34)), (97u ^ OBFH_GUI_NAME_KEY(34)), (100u ^ OBFH_GUI_NAME_KEY(34)), (73u ^ OBFH_GUI_NAME_KEY(34)), (99u ^ OBFH_GUI_NAME_KEY(34)), (111u ^ OBFH_GUI_NAME_KEY(34)), (110u ^ OBFH_GUI_NAME_KEY(34)), (87u ^ OBFH_GUI_NAME_KEY(34)), (0u ^ OBFH_GUI_NAME_KEY(34)) }
#define LoadIconW(...) OBFH_API_CALL(0, LoadIconW, __VA_ARGS__)

#define OBFH_GUI_ID_MessageBeep 35
#define OBFH_GUI_NAME_MessageBeep \
    { (77u ^ OBFH_GUI_NAME_KEY(35)), (101u ^ OBFH_GUI_NAME_KEY(35)), (115u ^ OBFH_GUI_NAME_KEY(35)), (115u ^ OBFH_GUI_NAME_KEY(35)), (97u ^ OBFH_GUI_NAME_KEY(35)), (103u ^ OBFH_GUI_NAME_KEY(35)), (101u ^ OBFH_GUI_NAME_KEY(35)), (66u ^ OBFH_GUI_NAME_KEY(35)), (101u ^ OBFH_GUI_NAME_KEY(35)), (101u ^ OBFH_GUI_NAME_KEY(35)), (112u ^ OBFH_GUI_NAME_KEY(35)), (0u ^ OBFH_GUI_NAME_KEY(35)) }
#define MessageBeep(...) OBFH_API_CALL(0, MessageBeep, __VA_ARGS__)

#define OBFH_GUI_ID_MessageBoxA 36
#define OBFH_GUI_NAME_MessageBoxA \
    { (77u ^ OBFH_GUI_NAME_KEY(36)), (101u ^ OBFH_GUI_NAME_KEY(36)), (115u ^ OBFH_GUI_NAME_KEY(36)), (115u ^ OBFH_GUI_NAME_KEY(36)), (97u ^ OBFH_GUI_NAME_KEY(36)), (103u ^ OBFH_GUI_NAME_KEY(36)), (101u ^ OBFH_GUI_NAME_KEY(36)), (66u ^ OBFH_GUI_NAME_KEY(36)), (111u ^ OBFH_GUI_NAME_KEY(36)), (120u ^ OBFH_GUI_NAME_KEY(36)), (65u ^ OBFH_GUI_NAME_KEY(36)), (0u ^ OBFH_GUI_NAME_KEY(36)) }
#define MessageBoxA(...) OBFH_API_CALL(0, MessageBoxA, __VA_ARGS__)

#define OBFH_GUI_ID_MessageBoxW 37
#define OBFH_GUI_NAME_MessageBoxW \
    { (77u ^ OBFH_GUI_NAME_KEY(37)), (101u ^ OBFH_GUI_NAME_KEY(37)), (115u ^ OBFH_GUI_NAME_KEY(37)), (115u ^ OBFH_GUI_NAME_KEY(37)), (97u ^ OBFH_GUI_NAME_KEY(37)), (103u ^ OBFH_GUI_NAME_KEY(37)), (101u ^ OBFH_GUI_NAME_KEY(37)), (66u ^ OBFH_GUI_NAME_KEY(37)), (111u ^ OBFH_GUI_NAME_KEY(37)), (120u ^ OBFH_GUI_NAME_KEY(37)), (87u ^ OBFH_GUI_NAME_KEY(37)), (0u ^ OBFH_GUI_NAME_KEY(37)) }
#define MessageBoxW(...) OBFH_API_CALL(0, MessageBoxW, __VA_ARGS__)

#define OBFH_GUI_ID_MoveWindow 38
#define OBFH_GUI_NAME_MoveWindow \
    { (77u ^ OBFH_GUI_NAME_KEY(38)), (111u ^ OBFH_GUI_NAME_KEY(38)), (118u ^ OBFH_GUI_NAME_KEY(38)), (101u ^ OBFH_GUI_NAME_KEY(38)), (87u ^ OBFH_GUI_NAME_KEY(38)), (105u ^ OBFH_GUI_NAME_KEY(38)), (110u ^ OBFH_GUI_NAME_KEY(38)), (100u ^ OBFH_GUI_NAME_KEY(38)), (111u ^ OBFH_GUI_NAME_KEY(38)), (119u ^ OBFH_GUI_NAME_KEY(38)), (0u ^ OBFH_GUI_NAME_KEY(38)) }
#define MoveWindow(...) OBFH_API_CALL(0, MoveWindow, __VA_ARGS__)

#define OBFH_GUI_ID_OpenClipboard 39
#define OBFH_GUI_NAME_OpenClipboard \
    { (79u ^ OBFH_GUI_NAME_KEY(39)), (112u ^ OBFH_GUI_NAME_KEY(39)), (101u ^ OBFH_GUI_NAME_KEY(39)), (110u ^ OBFH_GUI_NAME_KEY(39)), (67u ^ OBFH_GUI_NAME_KEY(39)), (108u ^ OBFH_GUI_NAME_KEY(39)), (105u ^ OBFH_GUI_NAME_KEY(39)), (112u ^ OBFH_GUI_NAME_KEY(39)), (98u ^ OBFH_GUI_NAME_KEY(39)), (111u ^ OBFH_GUI_NAME_KEY(39)), (97u ^ OBFH_GUI_NAME_KEY(39)), (114u ^ OBFH_GUI_NAME_KEY(39)), (100u ^ OBFH_GUI_NAME_KEY(39)), (0u ^ OBFH_GUI_NAME_KEY(39)) }
#define OpenClipboard(...) OBFH_API_CALL(0, OpenClipboard, __VA_ARGS__)

#define OBFH_GUI_ID_PostQuitMessage 40
#define OBFH_GUI_NAME_PostQuitMessage \
    { (80u ^ OBFH_GUI_NAME_KEY(40)), (111u ^ OBFH_GUI_NAME_KEY(40)), (115u ^ OBFH_GUI_NAME_KEY(40)), (116u ^ OBFH_GUI_NAME_KEY(40)), (81u ^ OBFH_GUI_NAME_KEY(40)), (117u ^ OBFH_GUI_NAME_KEY(40)), (105u ^ OBFH_GUI_NAME_KEY(40)), (116u ^ OBFH_GUI_NAME_KEY(40)), (77u ^ OBFH_GUI_NAME_KEY(40)), (101u ^ OBFH_GUI_NAME_KEY(40)), (115u ^ OBFH_GUI_NAME_KEY(40)), (115u ^ OBFH_GUI_NAME_KEY(40)), (97u ^ OBFH_GUI_NAME_KEY(40)), (103u ^ OBFH_GUI_NAME_KEY(40)), (101u ^ OBFH_GUI_NAME_KEY(40)), (0u ^ OBFH_GUI_NAME_KEY(40)) }
#define PostQuitMessage(...) OBFH_API_CALL(0, PostQuitMessage, __VA_ARGS__)

#define OBFH_GUI_ID_RegisterClassExA 41
#define OBFH_GUI_NAME_RegisterClassExA \
    { (82u ^ OBFH_GUI_NAME_KEY(41)), (101u ^ OBFH_GUI_NAME_KEY(41)), (103u ^ OBFH_GUI_NAME_KEY(41)), (105u ^ OBFH_GUI_NAME_KEY(41)), (115u ^ OBFH_GUI_NAME_KEY(41)), (116u ^ OBFH_GUI_NAME_KEY(41)), (101u ^ OBFH_GUI_NAME_KEY(41)), (114u ^ OBFH_GUI_NAME_KEY(41)), (67u ^ OBFH_GUI_NAME_KEY(41)), (108u ^ OBFH_GUI_NAME_KEY(41)), (97u ^ OBFH_GUI_NAME_KEY(41)), (115u ^ OBFH_GUI_NAME_KEY(41)), (115u ^ OBFH_GUI_NAME_KEY(41)), (69u ^ OBFH_GUI_NAME_KEY(41)), (120u ^ OBFH_GUI_NAME_KEY(41)), (65u ^ OBFH_GUI_NAME_KEY(41)), (0u ^ OBFH_GUI_NAME_KEY(41)) }
#define RegisterClassExA(...) OBFH_API_CALL(0, RegisterClassExA, __VA_ARGS__)

#define OBFH_GUI_ID_RegisterClassExW 42
#define OBFH_GUI_NAME_RegisterClassExW \
    { (82u ^ OBFH_GUI_NAME_KEY(42)), (101u ^ OBFH_GUI_NAME_KEY(42)), (103u ^ OBFH_GUI_NAME_KEY(42)), (105u ^ OBFH_GUI_NAME_KEY(42)), (115u ^ OBFH_GUI_NAME_KEY(42)), (116u ^ OBFH_GUI_NAME_KEY(42)), (101u ^ OBFH_GUI_NAME_KEY(42)), (114u ^ OBFH_GUI_NAME_KEY(42)), (67u ^ OBFH_GUI_NAME_KEY(42)), (108u ^ OBFH_GUI_NAME_KEY(42)), (97u ^ OBFH_GUI_NAME_KEY(42)), (115u ^ OBFH_GUI_NAME_KEY(42)), (115u ^ OBFH_GUI_NAME_KEY(42)), (69u ^ OBFH_GUI_NAME_KEY(42)), (120u ^ OBFH_GUI_NAME_KEY(42)), (87u ^ OBFH_GUI_NAME_KEY(42)), (0u ^ OBFH_GUI_NAME_KEY(42)) }
#define RegisterClassExW(...) OBFH_API_CALL(0, RegisterClassExW, __VA_ARGS__)

#define OBFH_GUI_ID_ReleaseDC 43
#define OBFH_GUI_NAME_ReleaseDC \
    { (82u ^ OBFH_GUI_NAME_KEY(43)), (101u ^ OBFH_GUI_NAME_KEY(43)), (108u ^ OBFH_GUI_NAME_KEY(43)), (101u ^ OBFH_GUI_NAME_KEY(43)), (97u ^ OBFH_GUI_NAME_KEY(43)), (115u ^ OBFH_GUI_NAME_KEY(43)), (101u ^ OBFH_GUI_NAME_KEY(43)), (68u ^ OBFH_GUI_NAME_KEY(43)), (67u ^ OBFH_GUI_NAME_KEY(43)), (0u ^ OBFH_GUI_NAME_KEY(43)) }
#define ReleaseDC(...) OBFH_API_CALL(0, ReleaseDC, __VA_ARGS__)

#define OBFH_GUI_ID_SendMessageA 44
#define OBFH_GUI_NAME_SendMessageA \
    { (83u ^ OBFH_GUI_NAME_KEY(44)), (101u ^ OBFH_GUI_NAME_KEY(44)), (110u ^ OBFH_GUI_NAME_KEY(44)), (100u ^ OBFH_GUI_NAME_KEY(44)), (77u ^ OBFH_GUI_NAME_KEY(44)), (101u ^ OBFH_GUI_NAME_KEY(44)), (115u ^ OBFH_GUI_NAME_KEY(44)), (115u ^ OBFH_GUI_NAME_KEY(44)), (97u ^ OBFH_GUI_NAME_KEY(44)), (103u ^ OBFH_GUI_NAME_KEY(44)), (101u ^ OBFH_GUI_NAME_KEY(44)), (65u ^ OBFH_GUI_NAME_KEY(44)), (0u ^ OBFH_GUI_NAME_KEY(44)) }
#define SendMessageA(...) OBFH_API_CALL(0, SendMessageA, __VA_ARGS__)

#define OBFH_GUI_ID_SendMessageW 45
#define OBFH_GUI_NAME_SendMessageW \
    { (83u ^ OBFH_GUI_NAME_KEY(45)), (101u ^ OBFH_GUI_NAME_KEY(45)), (110u ^ OBFH_GUI_NAME_KEY(45)), (100u ^ OBFH_GUI_NAME_KEY(45)), (77u ^ OBFH_GUI_NAME_KEY(45)), (101u ^ OBFH_GUI_NAME_KEY(45)), (115u ^ OBFH_GUI_NAME_KEY(45)), (115u ^ OBFH_GUI_NAME_KEY(45)), (97u ^ OBFH_GUI_NAME_KEY(45)), (103u ^ OBFH_GUI_NAME_KEY(45)), (101u ^ OBFH_GUI_NAME_KEY(45)), (87u ^ OBFH_GUI_NAME_KEY(45)), (0u ^ OBFH_GUI_NAME_KEY(45)) }
#define SendMessageW(...) OBFH_API_CALL(0, SendMessageW, __VA_ARGS__)

#define OBFH_GUI_ID_SetClipboardData 46
#define OBFH_GUI_NAME_SetClipboardData \
    { (83u ^ OBFH_GUI_NAME_KEY(46)), (101u ^ OBFH_GUI_NAME_KEY(46)), (116u ^ OBFH_GUI_NAME_KEY(46)), (67u ^ OBFH_GUI_NAME_KEY(46)), (108u ^ OBFH_GUI_NAME_KEY(46)), (105u ^ OBFH_GUI_NAME_KEY(46)), (112u ^ OBFH_GUI_NAME_KEY(46)), (98u ^ OBFH_GUI_NAME_KEY(46)), (111u ^ OBFH_GUI_NAME_KEY(46)), (97u ^ OBFH_GUI_NAME_KEY(46)), (114u ^ OBFH_GUI_NAME_KEY(46)), (100u ^ OBFH_GUI_NAME_KEY(46)), (68u ^ OBFH_GUI_NAME_KEY(46)), (97u ^ OBFH_GUI_NAME_KEY(46)), (116u ^ OBFH_GUI_NAME_KEY(46)), (97u ^ OBFH_GUI_NAME_KEY(46)), (0u ^ OBFH_GUI_NAME_KEY(46)) }
#define SetClipboardData(...) OBFH_API_CALL(0, SetClipboardData, __VA_ARGS__)

#define OBFH_GUI_ID_SetFocus 47
#define OBFH_GUI_NAME_SetFocus \
    { (83u ^ OBFH_GUI_NAME_KEY(47)), (101u ^ OBFH_GUI_NAME_KEY(47)), (116u ^ OBFH_GUI_NAME_KEY(47)), (70u ^ OBFH_GUI_NAME_KEY(47)), (111u ^ OBFH_GUI_NAME_KEY(47)), (99u ^ OBFH_GUI_NAME_KEY(47)), (117u ^ OBFH_GUI_NAME_KEY(47)), (115u ^ OBFH_GUI_NAME_KEY(47)), (0u ^ OBFH_GUI_NAME_KEY(47)) }
#define SetFocus(...) OBFH_API_CALL(0, SetFocus, __VA_ARGS__)

#define OBFH_GUI_ID_SetWindowPos 48
#define OBFH_GUI_NAME_SetWindowPos \
    { (83u ^ OBFH_GUI_NAME_KEY(48)), (101u ^ OBFH_GUI_NAME_KEY(48)), (116u ^ OBFH_GUI_NAME_KEY(48)), (87u ^ OBFH_GUI_NAME_KEY(48)), (105u ^ OBFH_GUI_NAME_KEY(48)), (110u ^ OBFH_GUI_NAME_KEY(48)), (100u ^ OBFH_GUI_NAME_KEY(48)), (111u ^ OBFH_GUI_NAME_KEY(48)), (119u ^ OBFH_GUI_NAME_KEY(48)), (80u ^ OBFH_GUI_NAME_KEY(48)), (111u ^ OBFH_GUI_NAME_KEY(48)), (115u ^ OBFH_GUI_NAME_KEY(48)), (0u ^ OBFH_GUI_NAME_KEY(48)) }
#define SetWindowPos(...) OBFH_API_CALL(0, SetWindowPos, __VA_ARGS__)

#define OBFH_GUI_ID_SetWindowTextA 49
#define OBFH_GUI_NAME_SetWindowTextA \
    { (83u ^ OBFH_GUI_NAME_KEY(49)), (101u ^ OBFH_GUI_NAME_KEY(49)), (116u ^ OBFH_GUI_NAME_KEY(49)), (87u ^ OBFH_GUI_NAME_KEY(49)), (105u ^ OBFH_GUI_NAME_KEY(49)), (110u ^ OBFH_GUI_NAME_KEY(49)), (100u ^ OBFH_GUI_NAME_KEY(49)), (111u ^ OBFH_GUI_NAME_KEY(49)), (119u ^ OBFH_GUI_NAME_KEY(49)), (84u ^ OBFH_GUI_NAME_KEY(49)), (101u ^ OBFH_GUI_NAME_KEY(49)), (120u ^ OBFH_GUI_NAME_KEY(49)), (116u ^ OBFH_GUI_NAME_KEY(49)), (65u ^ OBFH_GUI_NAME_KEY(49)), (0u ^ OBFH_GUI_NAME_KEY(49)) }
#define SetWindowTextA(...) OBFH_API_CALL(0, SetWindowTextA, __VA_ARGS__)

#define OBFH_GUI_ID_SetWindowTextW 50
#define OBFH_GUI_NAME_SetWindowTextW \
    { (83u ^ OBFH_GUI_NAME_KEY(50)), (101u ^ OBFH_GUI_NAME_KEY(50)), (116u ^ OBFH_GUI_NAME_KEY(50)), (87u ^ OBFH_GUI_NAME_KEY(50)), (105u ^ OBFH_GUI_NAME_KEY(50)), (110u ^ OBFH_GUI_NAME_KEY(50)), (100u ^ OBFH_GUI_NAME_KEY(50)), (111u ^ OBFH_GUI_NAME_KEY(50)), (119u ^ OBFH_GUI_NAME_KEY(50)), (84u ^ OBFH_GUI_NAME_KEY(50)), (101u ^ OBFH_GUI_NAME_KEY(50)), (120u ^ OBFH_GUI_NAME_KEY(50)), (116u ^ OBFH_GUI_NAME_KEY(50)), (87u ^ OBFH_GUI_NAME_KEY(50)), (0u ^ OBFH_GUI_NAME_KEY(50)) }
#define SetWindowTextW(...) OBFH_API_CALL(0, SetWindowTextW, __VA_ARGS__)

#define OBFH_GUI_ID_ShowWindow 51
#define OBFH_GUI_NAME_ShowWindow \
    { (83u ^ OBFH_GUI_NAME_KEY(51)), (104u ^ OBFH_GUI_NAME_KEY(51)), (111u ^ OBFH_GUI_NAME_KEY(51)), (119u ^ OBFH_GUI_NAME_KEY(51)), (87u ^ OBFH_GUI_NAME_KEY(51)), (105u ^ OBFH_GUI_NAME_KEY(51)), (110u ^ OBFH_GUI_NAME_KEY(51)), (100u ^ OBFH_GUI_NAME_KEY(51)), (111u ^ OBFH_GUI_NAME_KEY(51)), (119u ^ OBFH_GUI_NAME_KEY(51)), (0u ^ OBFH_GUI_NAME_KEY(51)) }
#define ShowWindow(...) OBFH_API_CALL(0, ShowWindow, __VA_ARGS__)

#define OBFH_GUI_ID_TranslateMessage 52
#define OBFH_GUI_NAME_TranslateMessage \
    { (84u ^ OBFH_GUI_NAME_KEY(52)), (114u ^ OBFH_GUI_NAME_KEY(52)), (97u ^ OBFH_GUI_NAME_KEY(52)), (110u ^ OBFH_GUI_NAME_KEY(52)), (115u ^ OBFH_GUI_NAME_KEY(52)), (108u ^ OBFH_GUI_NAME_KEY(52)), (97u ^ OBFH_GUI_NAME_KEY(52)), (116u ^ OBFH_GUI_NAME_KEY(52)), (101u ^ OBFH_GUI_NAME_KEY(52)), (77u ^ OBFH_GUI_NAME_KEY(52)), (101u ^ OBFH_GUI_NAME_KEY(52)), (115u ^ OBFH_GUI_NAME_KEY(52)), (115u ^ OBFH_GUI_NAME_KEY(52)), (97u ^ OBFH_GUI_NAME_KEY(52)), (103u ^ OBFH_GUI_NAME_KEY(52)), (101u ^ OBFH_GUI_NAME_KEY(52)), (0u ^ OBFH_GUI_NAME_KEY(52)) }
#define TranslateMessage(...) OBFH_API_CALL(0, TranslateMessage, __VA_ARGS__)

#define OBFH_GUI_ID_UnregisterClassA 53
#define OBFH_GUI_NAME_UnregisterClassA \
    { (85u ^ OBFH_GUI_NAME_KEY(53)), (110u ^ OBFH_GUI_NAME_KEY(53)), (114u ^ OBFH_GUI_NAME_KEY(53)), (101u ^ OBFH_GUI_NAME_KEY(53)), (103u ^ OBFH_GUI_NAME_KEY(53)), (105u ^ OBFH_GUI_NAME_KEY(53)), (115u ^ OBFH_GUI_NAME_KEY(53)), (116u ^ OBFH_GUI_NAME_KEY(53)), (101u ^ OBFH_GUI_NAME_KEY(53)), (114u ^ OBFH_GUI_NAME_KEY(53)), (67u ^ OBFH_GUI_NAME_KEY(53)), (108u ^ OBFH_GUI_NAME_KEY(53)), (97u ^ OBFH_GUI_NAME_KEY(53)), (115u ^ OBFH_GUI_NAME_KEY(53)), (115u ^ OBFH_GUI_NAME_KEY(53)), (65u ^ OBFH_GUI_NAME_KEY(53)), (0u ^ OBFH_GUI_NAME_KEY(53)) }
#define UnregisterClassA(...) OBFH_API_CALL(0, UnregisterClassA, __VA_ARGS__)

#define OBFH_GUI_ID_UnregisterClassW 54
#define OBFH_GUI_NAME_UnregisterClassW \
    { (85u ^ OBFH_GUI_NAME_KEY(54)), (110u ^ OBFH_GUI_NAME_KEY(54)), (114u ^ OBFH_GUI_NAME_KEY(54)), (101u ^ OBFH_GUI_NAME_KEY(54)), (103u ^ OBFH_GUI_NAME_KEY(54)), (105u ^ OBFH_GUI_NAME_KEY(54)), (115u ^ OBFH_GUI_NAME_KEY(54)), (116u ^ OBFH_GUI_NAME_KEY(54)), (101u ^ OBFH_GUI_NAME_KEY(54)), (114u ^ OBFH_GUI_NAME_KEY(54)), (67u ^ OBFH_GUI_NAME_KEY(54)), (108u ^ OBFH_GUI_NAME_KEY(54)), (97u ^ OBFH_GUI_NAME_KEY(54)), (115u ^ OBFH_GUI_NAME_KEY(54)), (115u ^ OBFH_GUI_NAME_KEY(54)), (87u ^ OBFH_GUI_NAME_KEY(54)), (0u ^ OBFH_GUI_NAME_KEY(54)) }
#define UnregisterClassW(...) OBFH_API_CALL(0, UnregisterClassW, __VA_ARGS__)

#define OBFH_GUI_ID_UpdateWindow 55
#define OBFH_GUI_NAME_UpdateWindow \
    { (85u ^ OBFH_GUI_NAME_KEY(55)), (112u ^ OBFH_GUI_NAME_KEY(55)), (100u ^ OBFH_GUI_NAME_KEY(55)), (97u ^ OBFH_GUI_NAME_KEY(55)), (116u ^ OBFH_GUI_NAME_KEY(55)), (101u ^ OBFH_GUI_NAME_KEY(55)), (87u ^ OBFH_GUI_NAME_KEY(55)), (105u ^ OBFH_GUI_NAME_KEY(55)), (110u ^ OBFH_GUI_NAME_KEY(55)), (100u ^ OBFH_GUI_NAME_KEY(55)), (111u ^ OBFH_GUI_NAME_KEY(55)), (119u ^ OBFH_GUI_NAME_KEY(55)), (0u ^ OBFH_GUI_NAME_KEY(55)) }
#define UpdateWindow(...) OBFH_API_CALL(0, UpdateWindow, __VA_ARGS__)

#define OBFH_GUI_ID_CreateFontA 56
#define OBFH_GUI_NAME_CreateFontA \
    { (67u ^ OBFH_GUI_NAME_KEY(56)), (114u ^ OBFH_GUI_NAME_KEY(56)), (101u ^ OBFH_GUI_NAME_KEY(56)), (97u ^ OBFH_GUI_NAME_KEY(56)), (116u ^ OBFH_GUI_NAME_KEY(56)), (101u ^ OBFH_GUI_NAME_KEY(56)), (70u ^ OBFH_GUI_NAME_KEY(56)), (111u ^ OBFH_GUI_NAME_KEY(56)), (110u ^ OBFH_GUI_NAME_KEY(56)), (116u ^ OBFH_GUI_NAME_KEY(56)), (65u ^ OBFH_GUI_NAME_KEY(56)), (0u ^ OBFH_GUI_NAME_KEY(56)) }
#define CreateFontA(...) OBFH_API_CALL(1, CreateFontA, __VA_ARGS__)

#define OBFH_GUI_ID_CreateFontW 57
#define OBFH_GUI_NAME_CreateFontW \
    { (67u ^ OBFH_GUI_NAME_KEY(57)), (114u ^ OBFH_GUI_NAME_KEY(57)), (101u ^ OBFH_GUI_NAME_KEY(57)), (97u ^ OBFH_GUI_NAME_KEY(57)), (116u ^ OBFH_GUI_NAME_KEY(57)), (101u ^ OBFH_GUI_NAME_KEY(57)), (70u ^ OBFH_GUI_NAME_KEY(57)), (111u ^ OBFH_GUI_NAME_KEY(57)), (110u ^ OBFH_GUI_NAME_KEY(57)), (116u ^ OBFH_GUI_NAME_KEY(57)), (87u ^ OBFH_GUI_NAME_KEY(57)), (0u ^ OBFH_GUI_NAME_KEY(57)) }
#define CreateFontW(...) OBFH_API_CALL(1, CreateFontW, __VA_ARGS__)

#define OBFH_GUI_ID_CreateFontIndirectA 58
#define OBFH_GUI_NAME_CreateFontIndirectA \
    { (67u ^ OBFH_GUI_NAME_KEY(58)), (114u ^ OBFH_GUI_NAME_KEY(58)), (101u ^ OBFH_GUI_NAME_KEY(58)), (97u ^ OBFH_GUI_NAME_KEY(58)), (116u ^ OBFH_GUI_NAME_KEY(58)), (101u ^ OBFH_GUI_NAME_KEY(58)), (70u ^ OBFH_GUI_NAME_KEY(58)), (111u ^ OBFH_GUI_NAME_KEY(58)), (110u ^ OBFH_GUI_NAME_KEY(58)), (116u ^ OBFH_GUI_NAME_KEY(58)), (73u ^ OBFH_GUI_NAME_KEY(58)), (110u ^ OBFH_GUI_NAME_KEY(58)), (100u ^ OBFH_GUI_NAME_KEY(58)), (105u ^ OBFH_GUI_NAME_KEY(58)), (114u ^ OBFH_GUI_NAME_KEY(58)), (101u ^ OBFH_GUI_NAME_KEY(58)), (99u ^ OBFH_GUI_NAME_KEY(58)), (116u ^ OBFH_GUI_NAME_KEY(58)), (65u ^ OBFH_GUI_NAME_KEY(58)), (0u ^ OBFH_GUI_NAME_KEY(58)) }
#define CreateFontIndirectA(...) OBFH_API_CALL(1, CreateFontIndirectA, __VA_ARGS__)

#define OBFH_GUI_ID_CreateFontIndirectW 59
#define OBFH_GUI_NAME_CreateFontIndirectW \
    { (67u ^ OBFH_GUI_NAME_KEY(59)), (114u ^ OBFH_GUI_NAME_KEY(59)), (101u ^ OBFH_GUI_NAME_KEY(59)), (97u ^ OBFH_GUI_NAME_KEY(59)), (116u ^ OBFH_GUI_NAME_KEY(59)), (101u ^ OBFH_GUI_NAME_KEY(59)), (70u ^ OBFH_GUI_NAME_KEY(59)), (111u ^ OBFH_GUI_NAME_KEY(59)), (110u ^ OBFH_GUI_NAME_KEY(59)), (116u ^ OBFH_GUI_NAME_KEY(59)), (73u ^ OBFH_GUI_NAME_KEY(59)), (110u ^ OBFH_GUI_NAME_KEY(59)), (100u ^ OBFH_GUI_NAME_KEY(59)), (105u ^ OBFH_GUI_NAME_KEY(59)), (114u ^ OBFH_GUI_NAME_KEY(59)), (101u ^ OBFH_GUI_NAME_KEY(59)), (99u ^ OBFH_GUI_NAME_KEY(59)), (116u ^ OBFH_GUI_NAME_KEY(59)), (87u ^ OBFH_GUI_NAME_KEY(59)), (0u ^ OBFH_GUI_NAME_KEY(59)) }
#define CreateFontIndirectW(...) OBFH_API_CALL(1, CreateFontIndirectW, __VA_ARGS__)

#define OBFH_GUI_ID_DeleteObject 60
#define OBFH_GUI_NAME_DeleteObject \
    { (68u ^ OBFH_GUI_NAME_KEY(60)), (101u ^ OBFH_GUI_NAME_KEY(60)), (108u ^ OBFH_GUI_NAME_KEY(60)), (101u ^ OBFH_GUI_NAME_KEY(60)), (116u ^ OBFH_GUI_NAME_KEY(60)), (101u ^ OBFH_GUI_NAME_KEY(60)), (79u ^ OBFH_GUI_NAME_KEY(60)), (98u ^ OBFH_GUI_NAME_KEY(60)), (106u ^ OBFH_GUI_NAME_KEY(60)), (101u ^ OBFH_GUI_NAME_KEY(60)), (99u ^ OBFH_GUI_NAME_KEY(60)), (116u ^ OBFH_GUI_NAME_KEY(60)), (0u ^ OBFH_GUI_NAME_KEY(60)) }
#define DeleteObject(...) OBFH_API_CALL(1, DeleteObject, __VA_ARGS__)

#define OBFH_GUI_ID_GetDeviceCaps 61
#define OBFH_GUI_NAME_GetDeviceCaps \
    { (71u ^ OBFH_GUI_NAME_KEY(61)), (101u ^ OBFH_GUI_NAME_KEY(61)), (116u ^ OBFH_GUI_NAME_KEY(61)), (68u ^ OBFH_GUI_NAME_KEY(61)), (101u ^ OBFH_GUI_NAME_KEY(61)), (118u ^ OBFH_GUI_NAME_KEY(61)), (105u ^ OBFH_GUI_NAME_KEY(61)), (99u ^ OBFH_GUI_NAME_KEY(61)), (101u ^ OBFH_GUI_NAME_KEY(61)), (67u ^ OBFH_GUI_NAME_KEY(61)), (97u ^ OBFH_GUI_NAME_KEY(61)), (112u ^ OBFH_GUI_NAME_KEY(61)), (115u ^ OBFH_GUI_NAME_KEY(61)), (0u ^ OBFH_GUI_NAME_KEY(61)) }
#define GetDeviceCaps(...) OBFH_API_CALL(1, GetDeviceCaps, __VA_ARGS__)

#define OBFH_GUI_ID_SelectObject 62
#define OBFH_GUI_NAME_SelectObject \
    { (83u ^ OBFH_GUI_NAME_KEY(62)), (101u ^ OBFH_GUI_NAME_KEY(62)), (108u ^ OBFH_GUI_NAME_KEY(62)), (101u ^ OBFH_GUI_NAME_KEY(62)), (99u ^ OBFH_GUI_NAME_KEY(62)), (116u ^ OBFH_GUI_NAME_KEY(62)), (79u ^ OBFH_GUI_NAME_KEY(62)), (98u ^ OBFH_GUI_NAME_KEY(62)), (106u ^ OBFH_GUI_NAME_KEY(62)), (101u ^ OBFH_GUI_NAME_KEY(62)), (99u ^ OBFH_GUI_NAME_KEY(62)), (116u ^ OBFH_GUI_NAME_KEY(62)), (0u ^ OBFH_GUI_NAME_KEY(62)) }
#define SelectObject(...) OBFH_API_CALL(1, SelectObject, __VA_ARGS__)

#define OBFH_GUI_ID_SetBkColor 63
#define OBFH_GUI_NAME_SetBkColor \
    { (83u ^ OBFH_GUI_NAME_KEY(63)), (101u ^ OBFH_GUI_NAME_KEY(63)), (116u ^ OBFH_GUI_NAME_KEY(63)), (66u ^ OBFH_GUI_NAME_KEY(63)), (107u ^ OBFH_GUI_NAME_KEY(63)), (67u ^ OBFH_GUI_NAME_KEY(63)), (111u ^ OBFH_GUI_NAME_KEY(63)), (108u ^ OBFH_GUI_NAME_KEY(63)), (111u ^ OBFH_GUI_NAME_KEY(63)), (114u ^ OBFH_GUI_NAME_KEY(63)), (0u ^ OBFH_GUI_NAME_KEY(63)) }
#define SetBkColor(...) OBFH_API_CALL(1, SetBkColor, __VA_ARGS__)

#define OBFH_GUI_ID_SetTextColor 64
#define OBFH_GUI_NAME_SetTextColor \
    { (83u ^ OBFH_GUI_NAME_KEY(64)), (101u ^ OBFH_GUI_NAME_KEY(64)), (116u ^ OBFH_GUI_NAME_KEY(64)), (84u ^ OBFH_GUI_NAME_KEY(64)), (101u ^ OBFH_GUI_NAME_KEY(64)), (120u ^ OBFH_GUI_NAME_KEY(64)), (116u ^ OBFH_GUI_NAME_KEY(64)), (67u ^ OBFH_GUI_NAME_KEY(64)), (111u ^ OBFH_GUI_NAME_KEY(64)), (108u ^ OBFH_GUI_NAME_KEY(64)), (111u ^ OBFH_GUI_NAME_KEY(64)), (114u ^ OBFH_GUI_NAME_KEY(64)), (0u ^ OBFH_GUI_NAME_KEY(64)) }
#define SetTextColor(...) OBFH_API_CALL(1, SetTextColor, __VA_ARGS__)
// KERNEL32 user call sites share the cache; resolver definitions above use
// native bootstrap calls and cannot recurse through these late intercepts.
#define OBFH_GUI_ID_ExitProcess 65
#define OBFH_GUI_NAME_ExitProcess \
    { (69u ^ OBFH_GUI_NAME_KEY(65)), (120u ^ OBFH_GUI_NAME_KEY(65)), (105u ^ OBFH_GUI_NAME_KEY(65)), (116u ^ OBFH_GUI_NAME_KEY(65)), (80u ^ OBFH_GUI_NAME_KEY(65)), (114u ^ OBFH_GUI_NAME_KEY(65)), (111u ^ OBFH_GUI_NAME_KEY(65)), (99u ^ OBFH_GUI_NAME_KEY(65)), (101u ^ OBFH_GUI_NAME_KEY(65)), (115u ^ OBFH_GUI_NAME_KEY(65)), (115u ^ OBFH_GUI_NAME_KEY(65)), (0u ^ OBFH_GUI_NAME_KEY(65)) }
#undef ExitProcess
#define ExitProcess(...) OBFH_API_CALL(2, ExitProcess, __VA_ARGS__)

#define OBFH_GUI_ID_GetLastError 66
#define OBFH_GUI_NAME_GetLastError \
    { (71u ^ OBFH_GUI_NAME_KEY(66)), (101u ^ OBFH_GUI_NAME_KEY(66)), (116u ^ OBFH_GUI_NAME_KEY(66)), (76u ^ OBFH_GUI_NAME_KEY(66)), (97u ^ OBFH_GUI_NAME_KEY(66)), (115u ^ OBFH_GUI_NAME_KEY(66)), (116u ^ OBFH_GUI_NAME_KEY(66)), (69u ^ OBFH_GUI_NAME_KEY(66)), (114u ^ OBFH_GUI_NAME_KEY(66)), (114u ^ OBFH_GUI_NAME_KEY(66)), (111u ^ OBFH_GUI_NAME_KEY(66)), (114u ^ OBFH_GUI_NAME_KEY(66)), (0u ^ OBFH_GUI_NAME_KEY(66)) }
#undef GetLastError
#define GetLastError(...) OBFH_API_CALL(2, GetLastError, __VA_ARGS__)

#define OBFH_GUI_ID_FreeLibrary 67
#define OBFH_GUI_NAME_FreeLibrary \
    { (70u ^ OBFH_GUI_NAME_KEY(67)), (114u ^ OBFH_GUI_NAME_KEY(67)), (101u ^ OBFH_GUI_NAME_KEY(67)), (101u ^ OBFH_GUI_NAME_KEY(67)), (76u ^ OBFH_GUI_NAME_KEY(67)), (105u ^ OBFH_GUI_NAME_KEY(67)), (98u ^ OBFH_GUI_NAME_KEY(67)), (114u ^ OBFH_GUI_NAME_KEY(67)), (97u ^ OBFH_GUI_NAME_KEY(67)), (114u ^ OBFH_GUI_NAME_KEY(67)), (121u ^ OBFH_GUI_NAME_KEY(67)), (0u ^ OBFH_GUI_NAME_KEY(67)) }
#undef FreeLibrary
#define FreeLibrary(...) OBFH_API_CALL(2, FreeLibrary, __VA_ARGS__)

#define OBFH_GUI_ID_SetLastError 68
#define OBFH_GUI_NAME_SetLastError \
    { (83u ^ OBFH_GUI_NAME_KEY(68)), (101u ^ OBFH_GUI_NAME_KEY(68)), (116u ^ OBFH_GUI_NAME_KEY(68)), (76u ^ OBFH_GUI_NAME_KEY(68)), (97u ^ OBFH_GUI_NAME_KEY(68)), (115u ^ OBFH_GUI_NAME_KEY(68)), (116u ^ OBFH_GUI_NAME_KEY(68)), (69u ^ OBFH_GUI_NAME_KEY(68)), (114u ^ OBFH_GUI_NAME_KEY(68)), (114u ^ OBFH_GUI_NAME_KEY(68)), (111u ^ OBFH_GUI_NAME_KEY(68)), (114u ^ OBFH_GUI_NAME_KEY(68)), (0u ^ OBFH_GUI_NAME_KEY(68)) }
#undef SetLastError
#define SetLastError(...) OBFH_API_CALL(2, SetLastError, __VA_ARGS__)

#define OBFH_GUI_ID_WriteConsoleA 69
#define OBFH_GUI_NAME_WriteConsoleA \
    { (87u ^ OBFH_GUI_NAME_KEY(69)), (114u ^ OBFH_GUI_NAME_KEY(69)), (105u ^ OBFH_GUI_NAME_KEY(69)), (116u ^ OBFH_GUI_NAME_KEY(69)), (101u ^ OBFH_GUI_NAME_KEY(69)), (67u ^ OBFH_GUI_NAME_KEY(69)), (111u ^ OBFH_GUI_NAME_KEY(69)), (110u ^ OBFH_GUI_NAME_KEY(69)), (115u ^ OBFH_GUI_NAME_KEY(69)), (111u ^ OBFH_GUI_NAME_KEY(69)), (108u ^ OBFH_GUI_NAME_KEY(69)), (101u ^ OBFH_GUI_NAME_KEY(69)), (65u ^ OBFH_GUI_NAME_KEY(69)), (0u ^ OBFH_GUI_NAME_KEY(69)) }
#undef WriteConsoleA
#define WriteConsoleA(...) OBFH_API_CALL(2, WriteConsoleA, __VA_ARGS__)

#define OBFH_GUI_ID_GetStdHandle 70
#define OBFH_GUI_NAME_GetStdHandle \
    { (71u ^ OBFH_GUI_NAME_KEY(70)), (101u ^ OBFH_GUI_NAME_KEY(70)), (116u ^ OBFH_GUI_NAME_KEY(70)), (83u ^ OBFH_GUI_NAME_KEY(70)), (116u ^ OBFH_GUI_NAME_KEY(70)), (100u ^ OBFH_GUI_NAME_KEY(70)), (72u ^ OBFH_GUI_NAME_KEY(70)), (97u ^ OBFH_GUI_NAME_KEY(70)), (110u ^ OBFH_GUI_NAME_KEY(70)), (100u ^ OBFH_GUI_NAME_KEY(70)), (108u ^ OBFH_GUI_NAME_KEY(70)), (101u ^ OBFH_GUI_NAME_KEY(70)), (0u ^ OBFH_GUI_NAME_KEY(70)) }
#undef GetStdHandle
#define GetStdHandle(...) OBFH_API_CALL(2, GetStdHandle, __VA_ARGS__)

#define OBFH_GUI_ID_GetModuleHandleA 71
#define OBFH_GUI_NAME_GetModuleHandleA \
    { (71u ^ OBFH_GUI_NAME_KEY(71)), (101u ^ OBFH_GUI_NAME_KEY(71)), (116u ^ OBFH_GUI_NAME_KEY(71)), (77u ^ OBFH_GUI_NAME_KEY(71)), (111u ^ OBFH_GUI_NAME_KEY(71)), (100u ^ OBFH_GUI_NAME_KEY(71)), (117u ^ OBFH_GUI_NAME_KEY(71)), (108u ^ OBFH_GUI_NAME_KEY(71)), (101u ^ OBFH_GUI_NAME_KEY(71)), (72u ^ OBFH_GUI_NAME_KEY(71)), (97u ^ OBFH_GUI_NAME_KEY(71)), (110u ^ OBFH_GUI_NAME_KEY(71)), (100u ^ OBFH_GUI_NAME_KEY(71)), (108u ^ OBFH_GUI_NAME_KEY(71)), (101u ^ OBFH_GUI_NAME_KEY(71)), (65u ^ OBFH_GUI_NAME_KEY(71)), (0u ^ OBFH_GUI_NAME_KEY(71)) }
#undef GetModuleHandleA
#define GetModuleHandleA(...) OBFH_API_CALL(2, GetModuleHandleA, __VA_ARGS__)

#define OBFH_GUI_ID_GetModuleHandleExA 72
#define OBFH_GUI_NAME_GetModuleHandleExA \
    { (71u ^ OBFH_GUI_NAME_KEY(72)), (101u ^ OBFH_GUI_NAME_KEY(72)), (116u ^ OBFH_GUI_NAME_KEY(72)), (77u ^ OBFH_GUI_NAME_KEY(72)), (111u ^ OBFH_GUI_NAME_KEY(72)), (100u ^ OBFH_GUI_NAME_KEY(72)), (117u ^ OBFH_GUI_NAME_KEY(72)), (108u ^ OBFH_GUI_NAME_KEY(72)), (101u ^ OBFH_GUI_NAME_KEY(72)), (72u ^ OBFH_GUI_NAME_KEY(72)), (97u ^ OBFH_GUI_NAME_KEY(72)), (110u ^ OBFH_GUI_NAME_KEY(72)), (100u ^ OBFH_GUI_NAME_KEY(72)), (108u ^ OBFH_GUI_NAME_KEY(72)), (101u ^ OBFH_GUI_NAME_KEY(72)), (69u ^ OBFH_GUI_NAME_KEY(72)), (120u ^ OBFH_GUI_NAME_KEY(72)), (65u ^ OBFH_GUI_NAME_KEY(72)), (0u ^ OBFH_GUI_NAME_KEY(72)) }
#undef GetModuleHandleExA
#define GetModuleHandleExA(...) OBFH_API_CALL(2, GetModuleHandleExA, __VA_ARGS__)

#define OBFH_GUI_ID_VirtualQuery 73
#define OBFH_GUI_NAME_VirtualQuery \
    { (86u ^ OBFH_GUI_NAME_KEY(73)), (105u ^ OBFH_GUI_NAME_KEY(73)), (114u ^ OBFH_GUI_NAME_KEY(73)), (116u ^ OBFH_GUI_NAME_KEY(73)), (117u ^ OBFH_GUI_NAME_KEY(73)), (97u ^ OBFH_GUI_NAME_KEY(73)), (108u ^ OBFH_GUI_NAME_KEY(73)), (81u ^ OBFH_GUI_NAME_KEY(73)), (117u ^ OBFH_GUI_NAME_KEY(73)), (101u ^ OBFH_GUI_NAME_KEY(73)), (114u ^ OBFH_GUI_NAME_KEY(73)), (121u ^ OBFH_GUI_NAME_KEY(73)), (0u ^ OBFH_GUI_NAME_KEY(73)) }
#undef VirtualQuery
#define VirtualQuery(...) OBFH_API_CALL(2, VirtualQuery, __VA_ARGS__)

#define OBFH_GUI_ID_GetConsoleMode 74
#define OBFH_GUI_NAME_GetConsoleMode \
    { (71u ^ OBFH_GUI_NAME_KEY(74)), (101u ^ OBFH_GUI_NAME_KEY(74)), (116u ^ OBFH_GUI_NAME_KEY(74)), (67u ^ OBFH_GUI_NAME_KEY(74)), (111u ^ OBFH_GUI_NAME_KEY(74)), (110u ^ OBFH_GUI_NAME_KEY(74)), (115u ^ OBFH_GUI_NAME_KEY(74)), (111u ^ OBFH_GUI_NAME_KEY(74)), (108u ^ OBFH_GUI_NAME_KEY(74)), (101u ^ OBFH_GUI_NAME_KEY(74)), (77u ^ OBFH_GUI_NAME_KEY(74)), (111u ^ OBFH_GUI_NAME_KEY(74)), (100u ^ OBFH_GUI_NAME_KEY(74)), (101u ^ OBFH_GUI_NAME_KEY(74)), (0u ^ OBFH_GUI_NAME_KEY(74)) }
#undef GetConsoleMode
#define GetConsoleMode(...) OBFH_API_CALL(2, GetConsoleMode, __VA_ARGS__)

#define OBFH_GUI_ID_MulDiv 75
#define OBFH_GUI_NAME_MulDiv \
    { (77u ^ OBFH_GUI_NAME_KEY(75)), (117u ^ OBFH_GUI_NAME_KEY(75)), (108u ^ OBFH_GUI_NAME_KEY(75)), (68u ^ OBFH_GUI_NAME_KEY(75)), (105u ^ OBFH_GUI_NAME_KEY(75)), (118u ^ OBFH_GUI_NAME_KEY(75)), (0u ^ OBFH_GUI_NAME_KEY(75)) }
#undef MulDiv
#define MulDiv(...) OBFH_API_CALL(2, MulDiv, __VA_ARGS__)

#define OBFH_GUI_ID_GlobalAlloc 76
#define OBFH_GUI_NAME_GlobalAlloc \
    { (71u ^ OBFH_GUI_NAME_KEY(76)), (108u ^ OBFH_GUI_NAME_KEY(76)), (111u ^ OBFH_GUI_NAME_KEY(76)), (98u ^ OBFH_GUI_NAME_KEY(76)), (97u ^ OBFH_GUI_NAME_KEY(76)), (108u ^ OBFH_GUI_NAME_KEY(76)), (65u ^ OBFH_GUI_NAME_KEY(76)), (108u ^ OBFH_GUI_NAME_KEY(76)), (108u ^ OBFH_GUI_NAME_KEY(76)), (111u ^ OBFH_GUI_NAME_KEY(76)), (99u ^ OBFH_GUI_NAME_KEY(76)), (0u ^ OBFH_GUI_NAME_KEY(76)) }
#undef GlobalAlloc
#define GlobalAlloc(...) OBFH_API_CALL(2, GlobalAlloc, __VA_ARGS__)

#define OBFH_GUI_ID_GlobalLock 77
#define OBFH_GUI_NAME_GlobalLock \
    { (71u ^ OBFH_GUI_NAME_KEY(77)), (108u ^ OBFH_GUI_NAME_KEY(77)), (111u ^ OBFH_GUI_NAME_KEY(77)), (98u ^ OBFH_GUI_NAME_KEY(77)), (97u ^ OBFH_GUI_NAME_KEY(77)), (108u ^ OBFH_GUI_NAME_KEY(77)), (76u ^ OBFH_GUI_NAME_KEY(77)), (111u ^ OBFH_GUI_NAME_KEY(77)), (99u ^ OBFH_GUI_NAME_KEY(77)), (107u ^ OBFH_GUI_NAME_KEY(77)), (0u ^ OBFH_GUI_NAME_KEY(77)) }
#undef GlobalLock
#define GlobalLock(...) OBFH_API_CALL(2, GlobalLock, __VA_ARGS__)

#define OBFH_GUI_ID_GlobalFree 78
#define OBFH_GUI_NAME_GlobalFree \
    { (71u ^ OBFH_GUI_NAME_KEY(78)), (108u ^ OBFH_GUI_NAME_KEY(78)), (111u ^ OBFH_GUI_NAME_KEY(78)), (98u ^ OBFH_GUI_NAME_KEY(78)), (97u ^ OBFH_GUI_NAME_KEY(78)), (108u ^ OBFH_GUI_NAME_KEY(78)), (70u ^ OBFH_GUI_NAME_KEY(78)), (114u ^ OBFH_GUI_NAME_KEY(78)), (101u ^ OBFH_GUI_NAME_KEY(78)), (101u ^ OBFH_GUI_NAME_KEY(78)), (0u ^ OBFH_GUI_NAME_KEY(78)) }
#undef GlobalFree
#define GlobalFree(...) OBFH_API_CALL(2, GlobalFree, __VA_ARGS__)

#define OBFH_GUI_ID_GlobalUnlock 79
#define OBFH_GUI_NAME_GlobalUnlock \
    { (71u ^ OBFH_GUI_NAME_KEY(79)), (108u ^ OBFH_GUI_NAME_KEY(79)), (111u ^ OBFH_GUI_NAME_KEY(79)), (98u ^ OBFH_GUI_NAME_KEY(79)), (97u ^ OBFH_GUI_NAME_KEY(79)), (108u ^ OBFH_GUI_NAME_KEY(79)), (85u ^ OBFH_GUI_NAME_KEY(79)), (110u ^ OBFH_GUI_NAME_KEY(79)), (108u ^ OBFH_GUI_NAME_KEY(79)), (111u ^ OBFH_GUI_NAME_KEY(79)), (99u ^ OBFH_GUI_NAME_KEY(79)), (107u ^ OBFH_GUI_NAME_KEY(79)), (0u ^ OBFH_GUI_NAME_KEY(79)) }
#undef GlobalUnlock
#define GlobalUnlock(...) OBFH_API_CALL(2, GlobalUnlock, __VA_ARGS__)

#define OBFH_GUI_ID_GetStartupInfoA 80
#define OBFH_GUI_NAME_GetStartupInfoA \
    { (71u ^ OBFH_GUI_NAME_KEY(80)), (101u ^ OBFH_GUI_NAME_KEY(80)), (116u ^ OBFH_GUI_NAME_KEY(80)), (83u ^ OBFH_GUI_NAME_KEY(80)), (116u ^ OBFH_GUI_NAME_KEY(80)), (97u ^ OBFH_GUI_NAME_KEY(80)), (114u ^ OBFH_GUI_NAME_KEY(80)), (116u ^ OBFH_GUI_NAME_KEY(80)), (117u ^ OBFH_GUI_NAME_KEY(80)), (112u ^ OBFH_GUI_NAME_KEY(80)), (73u ^ OBFH_GUI_NAME_KEY(80)), (110u ^ OBFH_GUI_NAME_KEY(80)), (102u ^ OBFH_GUI_NAME_KEY(80)), (111u ^ OBFH_GUI_NAME_KEY(80)), (65u ^ OBFH_GUI_NAME_KEY(80)), (0u ^ OBFH_GUI_NAME_KEY(80)) }
#undef GetStartupInfoA
#define GetStartupInfoA(...) OBFH_API_CALL(2, GetStartupInfoA, __VA_ARGS__)

#define OBFH_GUI_ID_GetCommandLineA 81
#define OBFH_GUI_NAME_GetCommandLineA \
    { (71u ^ OBFH_GUI_NAME_KEY(81)), (101u ^ OBFH_GUI_NAME_KEY(81)), (116u ^ OBFH_GUI_NAME_KEY(81)), (67u ^ OBFH_GUI_NAME_KEY(81)), (111u ^ OBFH_GUI_NAME_KEY(81)), (109u ^ OBFH_GUI_NAME_KEY(81)), (109u ^ OBFH_GUI_NAME_KEY(81)), (97u ^ OBFH_GUI_NAME_KEY(81)), (110u ^ OBFH_GUI_NAME_KEY(81)), (100u ^ OBFH_GUI_NAME_KEY(81)), (76u ^ OBFH_GUI_NAME_KEY(81)), (105u ^ OBFH_GUI_NAME_KEY(81)), (110u ^ OBFH_GUI_NAME_KEY(81)), (101u ^ OBFH_GUI_NAME_KEY(81)), (65u ^ OBFH_GUI_NAME_KEY(81)), (0u ^ OBFH_GUI_NAME_KEY(81)) }
#undef GetCommandLineA
#define GetCommandLineA(...) OBFH_API_CALL(2, GetCommandLineA, __VA_ARGS__)

#define OBFH_GUI_ID_WriteConsoleW 82
#define OBFH_GUI_NAME_WriteConsoleW \
    { (87u ^ OBFH_GUI_NAME_KEY(82)), (114u ^ OBFH_GUI_NAME_KEY(82)), (105u ^ OBFH_GUI_NAME_KEY(82)), (116u ^ OBFH_GUI_NAME_KEY(82)), (101u ^ OBFH_GUI_NAME_KEY(82)), (67u ^ OBFH_GUI_NAME_KEY(82)), (111u ^ OBFH_GUI_NAME_KEY(82)), (110u ^ OBFH_GUI_NAME_KEY(82)), (115u ^ OBFH_GUI_NAME_KEY(82)), (111u ^ OBFH_GUI_NAME_KEY(82)), (108u ^ OBFH_GUI_NAME_KEY(82)), (101u ^ OBFH_GUI_NAME_KEY(82)), (87u ^ OBFH_GUI_NAME_KEY(82)), (0u ^ OBFH_GUI_NAME_KEY(82)) }
#undef WriteConsoleW
#define WriteConsoleW(...) OBFH_API_CALL(2, WriteConsoleW, __VA_ARGS__)

#define OBFH_GUI_ID_GetModuleHandleW 83
#define OBFH_GUI_NAME_GetModuleHandleW \
    { (71u ^ OBFH_GUI_NAME_KEY(83)), (101u ^ OBFH_GUI_NAME_KEY(83)), (116u ^ OBFH_GUI_NAME_KEY(83)), (77u ^ OBFH_GUI_NAME_KEY(83)), (111u ^ OBFH_GUI_NAME_KEY(83)), (100u ^ OBFH_GUI_NAME_KEY(83)), (117u ^ OBFH_GUI_NAME_KEY(83)), (108u ^ OBFH_GUI_NAME_KEY(83)), (101u ^ OBFH_GUI_NAME_KEY(83)), (72u ^ OBFH_GUI_NAME_KEY(83)), (97u ^ OBFH_GUI_NAME_KEY(83)), (110u ^ OBFH_GUI_NAME_KEY(83)), (100u ^ OBFH_GUI_NAME_KEY(83)), (108u ^ OBFH_GUI_NAME_KEY(83)), (101u ^ OBFH_GUI_NAME_KEY(83)), (87u ^ OBFH_GUI_NAME_KEY(83)), (0u ^ OBFH_GUI_NAME_KEY(83)) }
#undef GetModuleHandleW
#define GetModuleHandleW(...) OBFH_API_CALL(2, GetModuleHandleW, __VA_ARGS__)

#define OBFH_GUI_ID_GetModuleHandleExW 84
#define OBFH_GUI_NAME_GetModuleHandleExW \
    { (71u ^ OBFH_GUI_NAME_KEY(84)), (101u ^ OBFH_GUI_NAME_KEY(84)), (116u ^ OBFH_GUI_NAME_KEY(84)), (77u ^ OBFH_GUI_NAME_KEY(84)), (111u ^ OBFH_GUI_NAME_KEY(84)), (100u ^ OBFH_GUI_NAME_KEY(84)), (117u ^ OBFH_GUI_NAME_KEY(84)), (108u ^ OBFH_GUI_NAME_KEY(84)), (101u ^ OBFH_GUI_NAME_KEY(84)), (72u ^ OBFH_GUI_NAME_KEY(84)), (97u ^ OBFH_GUI_NAME_KEY(84)), (110u ^ OBFH_GUI_NAME_KEY(84)), (100u ^ OBFH_GUI_NAME_KEY(84)), (108u ^ OBFH_GUI_NAME_KEY(84)), (101u ^ OBFH_GUI_NAME_KEY(84)), (69u ^ OBFH_GUI_NAME_KEY(84)), (120u ^ OBFH_GUI_NAME_KEY(84)), (87u ^ OBFH_GUI_NAME_KEY(84)), (0u ^ OBFH_GUI_NAME_KEY(84)) }
#undef GetModuleHandleExW
#define GetModuleHandleExW(...) OBFH_API_CALL(2, GetModuleHandleExW, __VA_ARGS__)

#define OBFH_GUI_ID_GetStartupInfoW 85
#define OBFH_GUI_NAME_GetStartupInfoW \
    { (71u ^ OBFH_GUI_NAME_KEY(85)), (101u ^ OBFH_GUI_NAME_KEY(85)), (116u ^ OBFH_GUI_NAME_KEY(85)), (83u ^ OBFH_GUI_NAME_KEY(85)), (116u ^ OBFH_GUI_NAME_KEY(85)), (97u ^ OBFH_GUI_NAME_KEY(85)), (114u ^ OBFH_GUI_NAME_KEY(85)), (116u ^ OBFH_GUI_NAME_KEY(85)), (117u ^ OBFH_GUI_NAME_KEY(85)), (112u ^ OBFH_GUI_NAME_KEY(85)), (73u ^ OBFH_GUI_NAME_KEY(85)), (110u ^ OBFH_GUI_NAME_KEY(85)), (102u ^ OBFH_GUI_NAME_KEY(85)), (111u ^ OBFH_GUI_NAME_KEY(85)), (87u ^ OBFH_GUI_NAME_KEY(85)), (0u ^ OBFH_GUI_NAME_KEY(85)) }
#undef GetStartupInfoW
#define GetStartupInfoW(...) OBFH_API_CALL(2, GetStartupInfoW, __VA_ARGS__)

#define OBFH_GUI_ID_GetCommandLineW 86
#define OBFH_GUI_NAME_GetCommandLineW \
    { (71u ^ OBFH_GUI_NAME_KEY(86)), (101u ^ OBFH_GUI_NAME_KEY(86)), (116u ^ OBFH_GUI_NAME_KEY(86)), (67u ^ OBFH_GUI_NAME_KEY(86)), (111u ^ OBFH_GUI_NAME_KEY(86)), (109u ^ OBFH_GUI_NAME_KEY(86)), (109u ^ OBFH_GUI_NAME_KEY(86)), (97u ^ OBFH_GUI_NAME_KEY(86)), (110u ^ OBFH_GUI_NAME_KEY(86)), (100u ^ OBFH_GUI_NAME_KEY(86)), (76u ^ OBFH_GUI_NAME_KEY(86)), (105u ^ OBFH_GUI_NAME_KEY(86)), (110u ^ OBFH_GUI_NAME_KEY(86)), (101u ^ OBFH_GUI_NAME_KEY(86)), (87u ^ OBFH_GUI_NAME_KEY(86)), (0u ^ OBFH_GUI_NAME_KEY(86)) }
#undef GetCommandLineW
#define GetCommandLineW(...) OBFH_API_CALL(2, GetCommandLineW, __VA_ARGS__)

// Files.

#define OBFH_GUI_ID_CreateFileA 87
#define OBFH_GUI_NAME_CreateFileA \
    { (67u ^ OBFH_GUI_NAME_KEY(87)), (114u ^ OBFH_GUI_NAME_KEY(87)), (101u ^ OBFH_GUI_NAME_KEY(87)), (97u ^ OBFH_GUI_NAME_KEY(87)), (116u ^ OBFH_GUI_NAME_KEY(87)), (101u ^ OBFH_GUI_NAME_KEY(87)), (70u ^ OBFH_GUI_NAME_KEY(87)), (105u ^ OBFH_GUI_NAME_KEY(87)), (108u ^ OBFH_GUI_NAME_KEY(87)), (101u ^ OBFH_GUI_NAME_KEY(87)), (65u ^ OBFH_GUI_NAME_KEY(87)), (0u ^ OBFH_GUI_NAME_KEY(87)) }
#undef CreateFileA
#define CreateFileA(...) OBFH_API_CALL(2, CreateFileA, __VA_ARGS__)

#define OBFH_GUI_ID_CreateFileW 88
#define OBFH_GUI_NAME_CreateFileW \
    { (67u ^ OBFH_GUI_NAME_KEY(88)), (114u ^ OBFH_GUI_NAME_KEY(88)), (101u ^ OBFH_GUI_NAME_KEY(88)), (97u ^ OBFH_GUI_NAME_KEY(88)), (116u ^ OBFH_GUI_NAME_KEY(88)), (101u ^ OBFH_GUI_NAME_KEY(88)), (70u ^ OBFH_GUI_NAME_KEY(88)), (105u ^ OBFH_GUI_NAME_KEY(88)), (108u ^ OBFH_GUI_NAME_KEY(88)), (101u ^ OBFH_GUI_NAME_KEY(88)), (87u ^ OBFH_GUI_NAME_KEY(88)), (0u ^ OBFH_GUI_NAME_KEY(88)) }
#undef CreateFileW
#define CreateFileW(...) OBFH_API_CALL(2, CreateFileW, __VA_ARGS__)

#define OBFH_GUI_ID_ReadFile 89
#define OBFH_GUI_NAME_ReadFile \
    { (82u ^ OBFH_GUI_NAME_KEY(89)), (101u ^ OBFH_GUI_NAME_KEY(89)), (97u ^ OBFH_GUI_NAME_KEY(89)), (100u ^ OBFH_GUI_NAME_KEY(89)), (70u ^ OBFH_GUI_NAME_KEY(89)), (105u ^ OBFH_GUI_NAME_KEY(89)), (108u ^ OBFH_GUI_NAME_KEY(89)), (101u ^ OBFH_GUI_NAME_KEY(89)), (0u ^ OBFH_GUI_NAME_KEY(89)) }
#undef ReadFile
#define ReadFile(...) OBFH_API_CALL(2, ReadFile, __VA_ARGS__)

#define OBFH_GUI_ID_WriteFile 90
#define OBFH_GUI_NAME_WriteFile \
    { (87u ^ OBFH_GUI_NAME_KEY(90)), (114u ^ OBFH_GUI_NAME_KEY(90)), (105u ^ OBFH_GUI_NAME_KEY(90)), (116u ^ OBFH_GUI_NAME_KEY(90)), (101u ^ OBFH_GUI_NAME_KEY(90)), (70u ^ OBFH_GUI_NAME_KEY(90)), (105u ^ OBFH_GUI_NAME_KEY(90)), (108u ^ OBFH_GUI_NAME_KEY(90)), (101u ^ OBFH_GUI_NAME_KEY(90)), (0u ^ OBFH_GUI_NAME_KEY(90)) }
#undef WriteFile
#define WriteFile(...) OBFH_API_CALL(2, WriteFile, __VA_ARGS__)

#define OBFH_GUI_ID_CloseHandle 91
#define OBFH_GUI_NAME_CloseHandle \
    { (67u ^ OBFH_GUI_NAME_KEY(91)), (108u ^ OBFH_GUI_NAME_KEY(91)), (111u ^ OBFH_GUI_NAME_KEY(91)), (115u ^ OBFH_GUI_NAME_KEY(91)), (101u ^ OBFH_GUI_NAME_KEY(91)), (72u ^ OBFH_GUI_NAME_KEY(91)), (97u ^ OBFH_GUI_NAME_KEY(91)), (110u ^ OBFH_GUI_NAME_KEY(91)), (100u ^ OBFH_GUI_NAME_KEY(91)), (108u ^ OBFH_GUI_NAME_KEY(91)), (101u ^ OBFH_GUI_NAME_KEY(91)), (0u ^ OBFH_GUI_NAME_KEY(91)) }
#undef CloseHandle
#define CloseHandle(...) OBFH_API_CALL(2, CloseHandle, __VA_ARGS__)

#define OBFH_GUI_ID_GetFileSizeEx 92
#define OBFH_GUI_NAME_GetFileSizeEx \
    { (71u ^ OBFH_GUI_NAME_KEY(92)), (101u ^ OBFH_GUI_NAME_KEY(92)), (116u ^ OBFH_GUI_NAME_KEY(92)), (70u ^ OBFH_GUI_NAME_KEY(92)), (105u ^ OBFH_GUI_NAME_KEY(92)), (108u ^ OBFH_GUI_NAME_KEY(92)), (101u ^ OBFH_GUI_NAME_KEY(92)), (83u ^ OBFH_GUI_NAME_KEY(92)), (105u ^ OBFH_GUI_NAME_KEY(92)), (122u ^ OBFH_GUI_NAME_KEY(92)), (101u ^ OBFH_GUI_NAME_KEY(92)), (69u ^ OBFH_GUI_NAME_KEY(92)), (120u ^ OBFH_GUI_NAME_KEY(92)), (0u ^ OBFH_GUI_NAME_KEY(92)) }
#undef GetFileSizeEx
#define GetFileSizeEx(...) OBFH_API_CALL(2, GetFileSizeEx, __VA_ARGS__)

#define OBFH_GUI_ID_SetFilePointerEx 93
#define OBFH_GUI_NAME_SetFilePointerEx \
    { (83u ^ OBFH_GUI_NAME_KEY(93)), (101u ^ OBFH_GUI_NAME_KEY(93)), (116u ^ OBFH_GUI_NAME_KEY(93)), (70u ^ OBFH_GUI_NAME_KEY(93)), (105u ^ OBFH_GUI_NAME_KEY(93)), (108u ^ OBFH_GUI_NAME_KEY(93)), (101u ^ OBFH_GUI_NAME_KEY(93)), (80u ^ OBFH_GUI_NAME_KEY(93)), (111u ^ OBFH_GUI_NAME_KEY(93)), (105u ^ OBFH_GUI_NAME_KEY(93)), (110u ^ OBFH_GUI_NAME_KEY(93)), (116u ^ OBFH_GUI_NAME_KEY(93)), (101u ^ OBFH_GUI_NAME_KEY(93)), (114u ^ OBFH_GUI_NAME_KEY(93)), (69u ^ OBFH_GUI_NAME_KEY(93)), (120u ^ OBFH_GUI_NAME_KEY(93)), (0u ^ OBFH_GUI_NAME_KEY(93)) }
#undef SetFilePointerEx
#define SetFilePointerEx(...) OBFH_API_CALL(2, SetFilePointerEx, __VA_ARGS__)

// Memory.

#define OBFH_GUI_ID_VirtualAlloc 94
#define OBFH_GUI_NAME_VirtualAlloc \
    { (86u ^ OBFH_GUI_NAME_KEY(94)), (105u ^ OBFH_GUI_NAME_KEY(94)), (114u ^ OBFH_GUI_NAME_KEY(94)), (116u ^ OBFH_GUI_NAME_KEY(94)), (117u ^ OBFH_GUI_NAME_KEY(94)), (97u ^ OBFH_GUI_NAME_KEY(94)), (108u ^ OBFH_GUI_NAME_KEY(94)), (65u ^ OBFH_GUI_NAME_KEY(94)), (108u ^ OBFH_GUI_NAME_KEY(94)), (108u ^ OBFH_GUI_NAME_KEY(94)), (111u ^ OBFH_GUI_NAME_KEY(94)), (99u ^ OBFH_GUI_NAME_KEY(94)), (0u ^ OBFH_GUI_NAME_KEY(94)) }
#undef VirtualAlloc
#define VirtualAlloc(...) OBFH_API_CALL(2, VirtualAlloc, __VA_ARGS__)

#define OBFH_GUI_ID_VirtualProtect 95
#define OBFH_GUI_NAME_VirtualProtect \
    { (86u ^ OBFH_GUI_NAME_KEY(95)), (105u ^ OBFH_GUI_NAME_KEY(95)), (114u ^ OBFH_GUI_NAME_KEY(95)), (116u ^ OBFH_GUI_NAME_KEY(95)), (117u ^ OBFH_GUI_NAME_KEY(95)), (97u ^ OBFH_GUI_NAME_KEY(95)), (108u ^ OBFH_GUI_NAME_KEY(95)), (80u ^ OBFH_GUI_NAME_KEY(95)), (114u ^ OBFH_GUI_NAME_KEY(95)), (111u ^ OBFH_GUI_NAME_KEY(95)), (116u ^ OBFH_GUI_NAME_KEY(95)), (101u ^ OBFH_GUI_NAME_KEY(95)), (99u ^ OBFH_GUI_NAME_KEY(95)), (116u ^ OBFH_GUI_NAME_KEY(95)), (0u ^ OBFH_GUI_NAME_KEY(95)) }
#undef VirtualProtect
#define VirtualProtect(...) OBFH_API_CALL(2, VirtualProtect, __VA_ARGS__)

#define OBFH_GUI_ID_VirtualFree 96
#define OBFH_GUI_NAME_VirtualFree \
    { (86u ^ OBFH_GUI_NAME_KEY(96)), (105u ^ OBFH_GUI_NAME_KEY(96)), (114u ^ OBFH_GUI_NAME_KEY(96)), (116u ^ OBFH_GUI_NAME_KEY(96)), (117u ^ OBFH_GUI_NAME_KEY(96)), (97u ^ OBFH_GUI_NAME_KEY(96)), (108u ^ OBFH_GUI_NAME_KEY(96)), (70u ^ OBFH_GUI_NAME_KEY(96)), (114u ^ OBFH_GUI_NAME_KEY(96)), (101u ^ OBFH_GUI_NAME_KEY(96)), (101u ^ OBFH_GUI_NAME_KEY(96)), (0u ^ OBFH_GUI_NAME_KEY(96)) }
#undef VirtualFree
#define VirtualFree(...) OBFH_API_CALL(2, VirtualFree, __VA_ARGS__)

#define OBFH_GUI_ID_GetProcessHeap 97
#define OBFH_GUI_NAME_GetProcessHeap \
    { (71u ^ OBFH_GUI_NAME_KEY(97)), (101u ^ OBFH_GUI_NAME_KEY(97)), (116u ^ OBFH_GUI_NAME_KEY(97)), (80u ^ OBFH_GUI_NAME_KEY(97)), (114u ^ OBFH_GUI_NAME_KEY(97)), (111u ^ OBFH_GUI_NAME_KEY(97)), (99u ^ OBFH_GUI_NAME_KEY(97)), (101u ^ OBFH_GUI_NAME_KEY(97)), (115u ^ OBFH_GUI_NAME_KEY(97)), (115u ^ OBFH_GUI_NAME_KEY(97)), (72u ^ OBFH_GUI_NAME_KEY(97)), (101u ^ OBFH_GUI_NAME_KEY(97)), (97u ^ OBFH_GUI_NAME_KEY(97)), (112u ^ OBFH_GUI_NAME_KEY(97)), (0u ^ OBFH_GUI_NAME_KEY(97)) }
#undef GetProcessHeap
#define GetProcessHeap(...) OBFH_API_CALL(2, GetProcessHeap, __VA_ARGS__)

#define OBFH_GUI_ID_HeapAlloc 98
#define OBFH_GUI_NAME_HeapAlloc \
    { (72u ^ OBFH_GUI_NAME_KEY(98)), (101u ^ OBFH_GUI_NAME_KEY(98)), (97u ^ OBFH_GUI_NAME_KEY(98)), (112u ^ OBFH_GUI_NAME_KEY(98)), (65u ^ OBFH_GUI_NAME_KEY(98)), (108u ^ OBFH_GUI_NAME_KEY(98)), (108u ^ OBFH_GUI_NAME_KEY(98)), (111u ^ OBFH_GUI_NAME_KEY(98)), (99u ^ OBFH_GUI_NAME_KEY(98)), (0u ^ OBFH_GUI_NAME_KEY(98)) }
#undef HeapAlloc
#define HeapAlloc(...) OBFH_API_CALL(2, HeapAlloc, __VA_ARGS__)

#define OBFH_GUI_ID_HeapReAlloc 99
#define OBFH_GUI_NAME_HeapReAlloc \
    { (72u ^ OBFH_GUI_NAME_KEY(99)), (101u ^ OBFH_GUI_NAME_KEY(99)), (97u ^ OBFH_GUI_NAME_KEY(99)), (112u ^ OBFH_GUI_NAME_KEY(99)), (82u ^ OBFH_GUI_NAME_KEY(99)), (101u ^ OBFH_GUI_NAME_KEY(99)), (65u ^ OBFH_GUI_NAME_KEY(99)), (108u ^ OBFH_GUI_NAME_KEY(99)), (108u ^ OBFH_GUI_NAME_KEY(99)), (111u ^ OBFH_GUI_NAME_KEY(99)), (99u ^ OBFH_GUI_NAME_KEY(99)), (0u ^ OBFH_GUI_NAME_KEY(99)) }
#undef HeapReAlloc
#define HeapReAlloc(...) OBFH_API_CALL(2, HeapReAlloc, __VA_ARGS__)

#define OBFH_GUI_ID_HeapFree 100
#define OBFH_GUI_NAME_HeapFree \
    { (72u ^ OBFH_GUI_NAME_KEY(100)), (101u ^ OBFH_GUI_NAME_KEY(100)), (97u ^ OBFH_GUI_NAME_KEY(100)), (112u ^ OBFH_GUI_NAME_KEY(100)), (70u ^ OBFH_GUI_NAME_KEY(100)), (114u ^ OBFH_GUI_NAME_KEY(100)), (101u ^ OBFH_GUI_NAME_KEY(100)), (101u ^ OBFH_GUI_NAME_KEY(100)), (0u ^ OBFH_GUI_NAME_KEY(100)) }
#undef HeapFree
#define HeapFree(...) OBFH_API_CALL(2, HeapFree, __VA_ARGS__)

// Paths and environment.

#define OBFH_GUI_ID_GetModuleFileNameA 101
#define OBFH_GUI_NAME_GetModuleFileNameA \
    { (71u ^ OBFH_GUI_NAME_KEY(101)), (101u ^ OBFH_GUI_NAME_KEY(101)), (116u ^ OBFH_GUI_NAME_KEY(101)), (77u ^ OBFH_GUI_NAME_KEY(101)), (111u ^ OBFH_GUI_NAME_KEY(101)), (100u ^ OBFH_GUI_NAME_KEY(101)), (117u ^ OBFH_GUI_NAME_KEY(101)), (108u ^ OBFH_GUI_NAME_KEY(101)), (101u ^ OBFH_GUI_NAME_KEY(101)), (70u ^ OBFH_GUI_NAME_KEY(101)), (105u ^ OBFH_GUI_NAME_KEY(101)), (108u ^ OBFH_GUI_NAME_KEY(101)), (101u ^ OBFH_GUI_NAME_KEY(101)), (78u ^ OBFH_GUI_NAME_KEY(101)), (97u ^ OBFH_GUI_NAME_KEY(101)), (109u ^ OBFH_GUI_NAME_KEY(101)), (101u ^ OBFH_GUI_NAME_KEY(101)), (65u ^ OBFH_GUI_NAME_KEY(101)), (0u ^ OBFH_GUI_NAME_KEY(101)) }
#undef GetModuleFileNameA
#define GetModuleFileNameA(...) OBFH_API_CALL(2, GetModuleFileNameA, __VA_ARGS__)

#define OBFH_GUI_ID_GetModuleFileNameW 102
#define OBFH_GUI_NAME_GetModuleFileNameW \
    { (71u ^ OBFH_GUI_NAME_KEY(102)), (101u ^ OBFH_GUI_NAME_KEY(102)), (116u ^ OBFH_GUI_NAME_KEY(102)), (77u ^ OBFH_GUI_NAME_KEY(102)), (111u ^ OBFH_GUI_NAME_KEY(102)), (100u ^ OBFH_GUI_NAME_KEY(102)), (117u ^ OBFH_GUI_NAME_KEY(102)), (108u ^ OBFH_GUI_NAME_KEY(102)), (101u ^ OBFH_GUI_NAME_KEY(102)), (70u ^ OBFH_GUI_NAME_KEY(102)), (105u ^ OBFH_GUI_NAME_KEY(102)), (108u ^ OBFH_GUI_NAME_KEY(102)), (101u ^ OBFH_GUI_NAME_KEY(102)), (78u ^ OBFH_GUI_NAME_KEY(102)), (97u ^ OBFH_GUI_NAME_KEY(102)), (109u ^ OBFH_GUI_NAME_KEY(102)), (101u ^ OBFH_GUI_NAME_KEY(102)), (87u ^ OBFH_GUI_NAME_KEY(102)), (0u ^ OBFH_GUI_NAME_KEY(102)) }
#undef GetModuleFileNameW
#define GetModuleFileNameW(...) OBFH_API_CALL(2, GetModuleFileNameW, __VA_ARGS__)

#define OBFH_GUI_ID_GetEnvironmentVariableA 103
#define OBFH_GUI_NAME_GetEnvironmentVariableA \
    { (71u ^ OBFH_GUI_NAME_KEY(103)), (101u ^ OBFH_GUI_NAME_KEY(103)), (116u ^ OBFH_GUI_NAME_KEY(103)), (69u ^ OBFH_GUI_NAME_KEY(103)), (110u ^ OBFH_GUI_NAME_KEY(103)), (118u ^ OBFH_GUI_NAME_KEY(103)), (105u ^ OBFH_GUI_NAME_KEY(103)), (114u ^ OBFH_GUI_NAME_KEY(103)), (111u ^ OBFH_GUI_NAME_KEY(103)), (110u ^ OBFH_GUI_NAME_KEY(103)), (109u ^ OBFH_GUI_NAME_KEY(103)), (101u ^ OBFH_GUI_NAME_KEY(103)), (110u ^ OBFH_GUI_NAME_KEY(103)), (116u ^ OBFH_GUI_NAME_KEY(103)), (86u ^ OBFH_GUI_NAME_KEY(103)), (97u ^ OBFH_GUI_NAME_KEY(103)), (114u ^ OBFH_GUI_NAME_KEY(103)), (105u ^ OBFH_GUI_NAME_KEY(103)), (97u ^ OBFH_GUI_NAME_KEY(103)), (98u ^ OBFH_GUI_NAME_KEY(103)), (108u ^ OBFH_GUI_NAME_KEY(103)), (101u ^ OBFH_GUI_NAME_KEY(103)), (65u ^ OBFH_GUI_NAME_KEY(103)), (0u ^ OBFH_GUI_NAME_KEY(103)) }
#undef GetEnvironmentVariableA
#define GetEnvironmentVariableA(...) OBFH_API_CALL(2, GetEnvironmentVariableA, __VA_ARGS__)

#define OBFH_GUI_ID_GetEnvironmentVariableW 104
#define OBFH_GUI_NAME_GetEnvironmentVariableW \
    { (71u ^ OBFH_GUI_NAME_KEY(104)), (101u ^ OBFH_GUI_NAME_KEY(104)), (116u ^ OBFH_GUI_NAME_KEY(104)), (69u ^ OBFH_GUI_NAME_KEY(104)), (110u ^ OBFH_GUI_NAME_KEY(104)), (118u ^ OBFH_GUI_NAME_KEY(104)), (105u ^ OBFH_GUI_NAME_KEY(104)), (114u ^ OBFH_GUI_NAME_KEY(104)), (111u ^ OBFH_GUI_NAME_KEY(104)), (110u ^ OBFH_GUI_NAME_KEY(104)), (109u ^ OBFH_GUI_NAME_KEY(104)), (101u ^ OBFH_GUI_NAME_KEY(104)), (110u ^ OBFH_GUI_NAME_KEY(104)), (116u ^ OBFH_GUI_NAME_KEY(104)), (86u ^ OBFH_GUI_NAME_KEY(104)), (97u ^ OBFH_GUI_NAME_KEY(104)), (114u ^ OBFH_GUI_NAME_KEY(104)), (105u ^ OBFH_GUI_NAME_KEY(104)), (97u ^ OBFH_GUI_NAME_KEY(104)), (98u ^ OBFH_GUI_NAME_KEY(104)), (108u ^ OBFH_GUI_NAME_KEY(104)), (101u ^ OBFH_GUI_NAME_KEY(104)), (87u ^ OBFH_GUI_NAME_KEY(104)), (0u ^ OBFH_GUI_NAME_KEY(104)) }
#undef GetEnvironmentVariableW
#define GetEnvironmentVariableW(...) OBFH_API_CALL(2, GetEnvironmentVariableW, __VA_ARGS__)

#define OBFH_GUI_ID_GetCurrentDirectoryA 105
#define OBFH_GUI_NAME_GetCurrentDirectoryA \
    { (71u ^ OBFH_GUI_NAME_KEY(105)), (101u ^ OBFH_GUI_NAME_KEY(105)), (116u ^ OBFH_GUI_NAME_KEY(105)), (67u ^ OBFH_GUI_NAME_KEY(105)), (117u ^ OBFH_GUI_NAME_KEY(105)), (114u ^ OBFH_GUI_NAME_KEY(105)), (114u ^ OBFH_GUI_NAME_KEY(105)), (101u ^ OBFH_GUI_NAME_KEY(105)), (110u ^ OBFH_GUI_NAME_KEY(105)), (116u ^ OBFH_GUI_NAME_KEY(105)), (68u ^ OBFH_GUI_NAME_KEY(105)), (105u ^ OBFH_GUI_NAME_KEY(105)), (114u ^ OBFH_GUI_NAME_KEY(105)), (101u ^ OBFH_GUI_NAME_KEY(105)), (99u ^ OBFH_GUI_NAME_KEY(105)), (116u ^ OBFH_GUI_NAME_KEY(105)), (111u ^ OBFH_GUI_NAME_KEY(105)), (114u ^ OBFH_GUI_NAME_KEY(105)), (121u ^ OBFH_GUI_NAME_KEY(105)), (65u ^ OBFH_GUI_NAME_KEY(105)), (0u ^ OBFH_GUI_NAME_KEY(105)) }
#undef GetCurrentDirectoryA
#define GetCurrentDirectoryA(...) OBFH_API_CALL(2, GetCurrentDirectoryA, __VA_ARGS__)

#define OBFH_GUI_ID_GetCurrentDirectoryW 106
#define OBFH_GUI_NAME_GetCurrentDirectoryW \
    { (71u ^ OBFH_GUI_NAME_KEY(106)), (101u ^ OBFH_GUI_NAME_KEY(106)), (116u ^ OBFH_GUI_NAME_KEY(106)), (67u ^ OBFH_GUI_NAME_KEY(106)), (117u ^ OBFH_GUI_NAME_KEY(106)), (114u ^ OBFH_GUI_NAME_KEY(106)), (114u ^ OBFH_GUI_NAME_KEY(106)), (101u ^ OBFH_GUI_NAME_KEY(106)), (110u ^ OBFH_GUI_NAME_KEY(106)), (116u ^ OBFH_GUI_NAME_KEY(106)), (68u ^ OBFH_GUI_NAME_KEY(106)), (105u ^ OBFH_GUI_NAME_KEY(106)), (114u ^ OBFH_GUI_NAME_KEY(106)), (101u ^ OBFH_GUI_NAME_KEY(106)), (99u ^ OBFH_GUI_NAME_KEY(106)), (116u ^ OBFH_GUI_NAME_KEY(106)), (111u ^ OBFH_GUI_NAME_KEY(106)), (114u ^ OBFH_GUI_NAME_KEY(106)), (121u ^ OBFH_GUI_NAME_KEY(106)), (87u ^ OBFH_GUI_NAME_KEY(106)), (0u ^ OBFH_GUI_NAME_KEY(106)) }
#undef GetCurrentDirectoryW
#define GetCurrentDirectoryW(...) OBFH_API_CALL(2, GetCurrentDirectoryW, __VA_ARGS__)

#define OBFH_GUI_ID_GetTempPathA 107
#define OBFH_GUI_NAME_GetTempPathA \
    { (71u ^ OBFH_GUI_NAME_KEY(107)), (101u ^ OBFH_GUI_NAME_KEY(107)), (116u ^ OBFH_GUI_NAME_KEY(107)), (84u ^ OBFH_GUI_NAME_KEY(107)), (101u ^ OBFH_GUI_NAME_KEY(107)), (109u ^ OBFH_GUI_NAME_KEY(107)), (112u ^ OBFH_GUI_NAME_KEY(107)), (80u ^ OBFH_GUI_NAME_KEY(107)), (97u ^ OBFH_GUI_NAME_KEY(107)), (116u ^ OBFH_GUI_NAME_KEY(107)), (104u ^ OBFH_GUI_NAME_KEY(107)), (65u ^ OBFH_GUI_NAME_KEY(107)), (0u ^ OBFH_GUI_NAME_KEY(107)) }
#undef GetTempPathA
#define GetTempPathA(...) OBFH_API_CALL(2, GetTempPathA, __VA_ARGS__)

#define OBFH_GUI_ID_GetTempPathW 108
#define OBFH_GUI_NAME_GetTempPathW \
    { (71u ^ OBFH_GUI_NAME_KEY(108)), (101u ^ OBFH_GUI_NAME_KEY(108)), (116u ^ OBFH_GUI_NAME_KEY(108)), (84u ^ OBFH_GUI_NAME_KEY(108)), (101u ^ OBFH_GUI_NAME_KEY(108)), (109u ^ OBFH_GUI_NAME_KEY(108)), (112u ^ OBFH_GUI_NAME_KEY(108)), (80u ^ OBFH_GUI_NAME_KEY(108)), (97u ^ OBFH_GUI_NAME_KEY(108)), (116u ^ OBFH_GUI_NAME_KEY(108)), (104u ^ OBFH_GUI_NAME_KEY(108)), (87u ^ OBFH_GUI_NAME_KEY(108)), (0u ^ OBFH_GUI_NAME_KEY(108)) }
#undef GetTempPathW
#define GetTempPathW(...) OBFH_API_CALL(2, GetTempPathW, __VA_ARGS__)

// Threads and events.

#define OBFH_GUI_ID_CreateThread 109
#define OBFH_GUI_NAME_CreateThread \
    { (67u ^ OBFH_GUI_NAME_KEY(109)), (114u ^ OBFH_GUI_NAME_KEY(109)), (101u ^ OBFH_GUI_NAME_KEY(109)), (97u ^ OBFH_GUI_NAME_KEY(109)), (116u ^ OBFH_GUI_NAME_KEY(109)), (101u ^ OBFH_GUI_NAME_KEY(109)), (84u ^ OBFH_GUI_NAME_KEY(109)), (104u ^ OBFH_GUI_NAME_KEY(109)), (114u ^ OBFH_GUI_NAME_KEY(109)), (101u ^ OBFH_GUI_NAME_KEY(109)), (97u ^ OBFH_GUI_NAME_KEY(109)), (100u ^ OBFH_GUI_NAME_KEY(109)), (0u ^ OBFH_GUI_NAME_KEY(109)) }
#undef CreateThread
#define CreateThread(...) OBFH_API_CALL(2, CreateThread, __VA_ARGS__)

#define OBFH_GUI_ID_WaitForSingleObject 110
#define OBFH_GUI_NAME_WaitForSingleObject \
    { (87u ^ OBFH_GUI_NAME_KEY(110)), (97u ^ OBFH_GUI_NAME_KEY(110)), (105u ^ OBFH_GUI_NAME_KEY(110)), (116u ^ OBFH_GUI_NAME_KEY(110)), (70u ^ OBFH_GUI_NAME_KEY(110)), (111u ^ OBFH_GUI_NAME_KEY(110)), (114u ^ OBFH_GUI_NAME_KEY(110)), (83u ^ OBFH_GUI_NAME_KEY(110)), (105u ^ OBFH_GUI_NAME_KEY(110)), (110u ^ OBFH_GUI_NAME_KEY(110)), (103u ^ OBFH_GUI_NAME_KEY(110)), (108u ^ OBFH_GUI_NAME_KEY(110)), (101u ^ OBFH_GUI_NAME_KEY(110)), (79u ^ OBFH_GUI_NAME_KEY(110)), (98u ^ OBFH_GUI_NAME_KEY(110)), (106u ^ OBFH_GUI_NAME_KEY(110)), (101u ^ OBFH_GUI_NAME_KEY(110)), (99u ^ OBFH_GUI_NAME_KEY(110)), (116u ^ OBFH_GUI_NAME_KEY(110)), (0u ^ OBFH_GUI_NAME_KEY(110)) }
#undef WaitForSingleObject
#define WaitForSingleObject(...) OBFH_API_CALL(2, WaitForSingleObject, __VA_ARGS__)

#define OBFH_GUI_ID_WaitForMultipleObjects 111
#define OBFH_GUI_NAME_WaitForMultipleObjects \
    { (87u ^ OBFH_GUI_NAME_KEY(111)), (97u ^ OBFH_GUI_NAME_KEY(111)), (105u ^ OBFH_GUI_NAME_KEY(111)), (116u ^ OBFH_GUI_NAME_KEY(111)), (70u ^ OBFH_GUI_NAME_KEY(111)), (111u ^ OBFH_GUI_NAME_KEY(111)), (114u ^ OBFH_GUI_NAME_KEY(111)), (77u ^ OBFH_GUI_NAME_KEY(111)), (117u ^ OBFH_GUI_NAME_KEY(111)), (108u ^ OBFH_GUI_NAME_KEY(111)), (116u ^ OBFH_GUI_NAME_KEY(111)), (105u ^ OBFH_GUI_NAME_KEY(111)), (112u ^ OBFH_GUI_NAME_KEY(111)), (108u ^ OBFH_GUI_NAME_KEY(111)), (101u ^ OBFH_GUI_NAME_KEY(111)), (79u ^ OBFH_GUI_NAME_KEY(111)), (98u ^ OBFH_GUI_NAME_KEY(111)), (106u ^ OBFH_GUI_NAME_KEY(111)), (101u ^ OBFH_GUI_NAME_KEY(111)), (99u ^ OBFH_GUI_NAME_KEY(111)), (116u ^ OBFH_GUI_NAME_KEY(111)), (115u ^ OBFH_GUI_NAME_KEY(111)), (0u ^ OBFH_GUI_NAME_KEY(111)) }
#undef WaitForMultipleObjects
#define WaitForMultipleObjects(...) OBFH_API_CALL(2, WaitForMultipleObjects, __VA_ARGS__)

#define OBFH_GUI_ID_CreateEventA 112
#define OBFH_GUI_NAME_CreateEventA \
    { (67u ^ OBFH_GUI_NAME_KEY(112)), (114u ^ OBFH_GUI_NAME_KEY(112)), (101u ^ OBFH_GUI_NAME_KEY(112)), (97u ^ OBFH_GUI_NAME_KEY(112)), (116u ^ OBFH_GUI_NAME_KEY(112)), (101u ^ OBFH_GUI_NAME_KEY(112)), (69u ^ OBFH_GUI_NAME_KEY(112)), (118u ^ OBFH_GUI_NAME_KEY(112)), (101u ^ OBFH_GUI_NAME_KEY(112)), (110u ^ OBFH_GUI_NAME_KEY(112)), (116u ^ OBFH_GUI_NAME_KEY(112)), (65u ^ OBFH_GUI_NAME_KEY(112)), (0u ^ OBFH_GUI_NAME_KEY(112)) }
#undef CreateEventA
#define CreateEventA(...) OBFH_API_CALL(2, CreateEventA, __VA_ARGS__)

#define OBFH_GUI_ID_CreateEventW 113
#define OBFH_GUI_NAME_CreateEventW \
    { (67u ^ OBFH_GUI_NAME_KEY(113)), (114u ^ OBFH_GUI_NAME_KEY(113)), (101u ^ OBFH_GUI_NAME_KEY(113)), (97u ^ OBFH_GUI_NAME_KEY(113)), (116u ^ OBFH_GUI_NAME_KEY(113)), (101u ^ OBFH_GUI_NAME_KEY(113)), (69u ^ OBFH_GUI_NAME_KEY(113)), (118u ^ OBFH_GUI_NAME_KEY(113)), (101u ^ OBFH_GUI_NAME_KEY(113)), (110u ^ OBFH_GUI_NAME_KEY(113)), (116u ^ OBFH_GUI_NAME_KEY(113)), (87u ^ OBFH_GUI_NAME_KEY(113)), (0u ^ OBFH_GUI_NAME_KEY(113)) }
#undef CreateEventW
#define CreateEventW(...) OBFH_API_CALL(2, CreateEventW, __VA_ARGS__)

#define OBFH_GUI_ID_SetEvent 114
#define OBFH_GUI_NAME_SetEvent \
    { (83u ^ OBFH_GUI_NAME_KEY(114)), (101u ^ OBFH_GUI_NAME_KEY(114)), (116u ^ OBFH_GUI_NAME_KEY(114)), (69u ^ OBFH_GUI_NAME_KEY(114)), (118u ^ OBFH_GUI_NAME_KEY(114)), (101u ^ OBFH_GUI_NAME_KEY(114)), (110u ^ OBFH_GUI_NAME_KEY(114)), (116u ^ OBFH_GUI_NAME_KEY(114)), (0u ^ OBFH_GUI_NAME_KEY(114)) }
#undef SetEvent
#define SetEvent(...) OBFH_API_CALL(2, SetEvent, __VA_ARGS__)

#define OBFH_GUI_ID_ResetEvent 115
#define OBFH_GUI_NAME_ResetEvent \
    { (82u ^ OBFH_GUI_NAME_KEY(115)), (101u ^ OBFH_GUI_NAME_KEY(115)), (115u ^ OBFH_GUI_NAME_KEY(115)), (101u ^ OBFH_GUI_NAME_KEY(115)), (116u ^ OBFH_GUI_NAME_KEY(115)), (69u ^ OBFH_GUI_NAME_KEY(115)), (118u ^ OBFH_GUI_NAME_KEY(115)), (101u ^ OBFH_GUI_NAME_KEY(115)), (110u ^ OBFH_GUI_NAME_KEY(115)), (116u ^ OBFH_GUI_NAME_KEY(115)), (0u ^ OBFH_GUI_NAME_KEY(115)) }
#undef ResetEvent
#define ResetEvent(...) OBFH_API_CALL(2, ResetEvent, __VA_ARGS__)

// Painting.

#define OBFH_GUI_ID_BeginPaint 116
#define OBFH_GUI_NAME_BeginPaint \
    { (66u ^ OBFH_GUI_NAME_KEY(116)), (101u ^ OBFH_GUI_NAME_KEY(116)), (103u ^ OBFH_GUI_NAME_KEY(116)), (105u ^ OBFH_GUI_NAME_KEY(116)), (110u ^ OBFH_GUI_NAME_KEY(116)), (80u ^ OBFH_GUI_NAME_KEY(116)), (97u ^ OBFH_GUI_NAME_KEY(116)), (105u ^ OBFH_GUI_NAME_KEY(116)), (110u ^ OBFH_GUI_NAME_KEY(116)), (116u ^ OBFH_GUI_NAME_KEY(116)), (0u ^ OBFH_GUI_NAME_KEY(116)) }
#undef BeginPaint
#define BeginPaint(...) OBFH_API_CALL(0, BeginPaint, __VA_ARGS__)

#define OBFH_GUI_ID_EndPaint 117
#define OBFH_GUI_NAME_EndPaint \
    { (69u ^ OBFH_GUI_NAME_KEY(117)), (110u ^ OBFH_GUI_NAME_KEY(117)), (100u ^ OBFH_GUI_NAME_KEY(117)), (80u ^ OBFH_GUI_NAME_KEY(117)), (97u ^ OBFH_GUI_NAME_KEY(117)), (105u ^ OBFH_GUI_NAME_KEY(117)), (110u ^ OBFH_GUI_NAME_KEY(117)), (116u ^ OBFH_GUI_NAME_KEY(117)), (0u ^ OBFH_GUI_NAME_KEY(117)) }
#undef EndPaint
#define EndPaint(...) OBFH_API_CALL(0, EndPaint, __VA_ARGS__)

#define OBFH_GUI_ID_DrawTextA 118
#define OBFH_GUI_NAME_DrawTextA \
    { (68u ^ OBFH_GUI_NAME_KEY(118)), (114u ^ OBFH_GUI_NAME_KEY(118)), (97u ^ OBFH_GUI_NAME_KEY(118)), (119u ^ OBFH_GUI_NAME_KEY(118)), (84u ^ OBFH_GUI_NAME_KEY(118)), (101u ^ OBFH_GUI_NAME_KEY(118)), (120u ^ OBFH_GUI_NAME_KEY(118)), (116u ^ OBFH_GUI_NAME_KEY(118)), (65u ^ OBFH_GUI_NAME_KEY(118)), (0u ^ OBFH_GUI_NAME_KEY(118)) }
#undef DrawTextA
#define DrawTextA(...) OBFH_API_CALL(0, DrawTextA, __VA_ARGS__)

#define OBFH_GUI_ID_DrawTextW 119
#define OBFH_GUI_NAME_DrawTextW \
    { (68u ^ OBFH_GUI_NAME_KEY(119)), (114u ^ OBFH_GUI_NAME_KEY(119)), (97u ^ OBFH_GUI_NAME_KEY(119)), (119u ^ OBFH_GUI_NAME_KEY(119)), (84u ^ OBFH_GUI_NAME_KEY(119)), (101u ^ OBFH_GUI_NAME_KEY(119)), (120u ^ OBFH_GUI_NAME_KEY(119)), (116u ^ OBFH_GUI_NAME_KEY(119)), (87u ^ OBFH_GUI_NAME_KEY(119)), (0u ^ OBFH_GUI_NAME_KEY(119)) }
#undef DrawTextW
#define DrawTextW(...) OBFH_API_CALL(0, DrawTextW, __VA_ARGS__)

#define OBFH_GUI_ID_TextOutA 120
#define OBFH_GUI_NAME_TextOutA \
    { (84u ^ OBFH_GUI_NAME_KEY(120)), (101u ^ OBFH_GUI_NAME_KEY(120)), (120u ^ OBFH_GUI_NAME_KEY(120)), (116u ^ OBFH_GUI_NAME_KEY(120)), (79u ^ OBFH_GUI_NAME_KEY(120)), (117u ^ OBFH_GUI_NAME_KEY(120)), (116u ^ OBFH_GUI_NAME_KEY(120)), (65u ^ OBFH_GUI_NAME_KEY(120)), (0u ^ OBFH_GUI_NAME_KEY(120)) }
#undef TextOutA
#define TextOutA(...) OBFH_API_CALL(1, TextOutA, __VA_ARGS__)

#define OBFH_GUI_ID_TextOutW 121
#define OBFH_GUI_NAME_TextOutW \
    { (84u ^ OBFH_GUI_NAME_KEY(121)), (101u ^ OBFH_GUI_NAME_KEY(121)), (120u ^ OBFH_GUI_NAME_KEY(121)), (116u ^ OBFH_GUI_NAME_KEY(121)), (79u ^ OBFH_GUI_NAME_KEY(121)), (117u ^ OBFH_GUI_NAME_KEY(121)), (116u ^ OBFH_GUI_NAME_KEY(121)), (87u ^ OBFH_GUI_NAME_KEY(121)), (0u ^ OBFH_GUI_NAME_KEY(121)) }
#undef TextOutW
#define TextOutW(...) OBFH_API_CALL(1, TextOutW, __VA_ARGS__)

#define OBFH_GUI_ID_BitBlt 122
#define OBFH_GUI_NAME_BitBlt \
    { (66u ^ OBFH_GUI_NAME_KEY(122)), (105u ^ OBFH_GUI_NAME_KEY(122)), (116u ^ OBFH_GUI_NAME_KEY(122)), (66u ^ OBFH_GUI_NAME_KEY(122)), (108u ^ OBFH_GUI_NAME_KEY(122)), (116u ^ OBFH_GUI_NAME_KEY(122)), (0u ^ OBFH_GUI_NAME_KEY(122)) }
#undef BitBlt
#define BitBlt(...) OBFH_API_CALL(1, BitBlt, __VA_ARGS__)

#define OBFH_GUI_ID_CreateCompatibleDC 123
#define OBFH_GUI_NAME_CreateCompatibleDC \
    { (67u ^ OBFH_GUI_NAME_KEY(123)), (114u ^ OBFH_GUI_NAME_KEY(123)), (101u ^ OBFH_GUI_NAME_KEY(123)), (97u ^ OBFH_GUI_NAME_KEY(123)), (116u ^ OBFH_GUI_NAME_KEY(123)), (101u ^ OBFH_GUI_NAME_KEY(123)), (67u ^ OBFH_GUI_NAME_KEY(123)), (111u ^ OBFH_GUI_NAME_KEY(123)), (109u ^ OBFH_GUI_NAME_KEY(123)), (112u ^ OBFH_GUI_NAME_KEY(123)), (97u ^ OBFH_GUI_NAME_KEY(123)), (116u ^ OBFH_GUI_NAME_KEY(123)), (105u ^ OBFH_GUI_NAME_KEY(123)), (98u ^ OBFH_GUI_NAME_KEY(123)), (108u ^ OBFH_GUI_NAME_KEY(123)), (101u ^ OBFH_GUI_NAME_KEY(123)), (68u ^ OBFH_GUI_NAME_KEY(123)), (67u ^ OBFH_GUI_NAME_KEY(123)), (0u ^ OBFH_GUI_NAME_KEY(123)) }
#undef CreateCompatibleDC
#define CreateCompatibleDC(...) OBFH_API_CALL(1, CreateCompatibleDC, __VA_ARGS__)

#define OBFH_GUI_ID_CreateCompatibleBitmap 124
#define OBFH_GUI_NAME_CreateCompatibleBitmap \
    { (67u ^ OBFH_GUI_NAME_KEY(124)), (114u ^ OBFH_GUI_NAME_KEY(124)), (101u ^ OBFH_GUI_NAME_KEY(124)), (97u ^ OBFH_GUI_NAME_KEY(124)), (116u ^ OBFH_GUI_NAME_KEY(124)), (101u ^ OBFH_GUI_NAME_KEY(124)), (67u ^ OBFH_GUI_NAME_KEY(124)), (111u ^ OBFH_GUI_NAME_KEY(124)), (109u ^ OBFH_GUI_NAME_KEY(124)), (112u ^ OBFH_GUI_NAME_KEY(124)), (97u ^ OBFH_GUI_NAME_KEY(124)), (116u ^ OBFH_GUI_NAME_KEY(124)), (105u ^ OBFH_GUI_NAME_KEY(124)), (98u ^ OBFH_GUI_NAME_KEY(124)), (108u ^ OBFH_GUI_NAME_KEY(124)), (101u ^ OBFH_GUI_NAME_KEY(124)), (66u ^ OBFH_GUI_NAME_KEY(124)), (105u ^ OBFH_GUI_NAME_KEY(124)), (116u ^ OBFH_GUI_NAME_KEY(124)), (109u ^ OBFH_GUI_NAME_KEY(124)), (97u ^ OBFH_GUI_NAME_KEY(124)), (112u ^ OBFH_GUI_NAME_KEY(124)), (0u ^ OBFH_GUI_NAME_KEY(124)) }
#undef CreateCompatibleBitmap
#define CreateCompatibleBitmap(...) OBFH_API_CALL(1, CreateCompatibleBitmap, __VA_ARGS__)

#define OBFH_GUI_ID_GetStockObject 125
#define OBFH_GUI_NAME_GetStockObject \
    { (71u ^ OBFH_GUI_NAME_KEY(125)), (101u ^ OBFH_GUI_NAME_KEY(125)), (116u ^ OBFH_GUI_NAME_KEY(125)), (83u ^ OBFH_GUI_NAME_KEY(125)), (116u ^ OBFH_GUI_NAME_KEY(125)), (111u ^ OBFH_GUI_NAME_KEY(125)), (99u ^ OBFH_GUI_NAME_KEY(125)), (107u ^ OBFH_GUI_NAME_KEY(125)), (79u ^ OBFH_GUI_NAME_KEY(125)), (98u ^ OBFH_GUI_NAME_KEY(125)), (106u ^ OBFH_GUI_NAME_KEY(125)), (101u ^ OBFH_GUI_NAME_KEY(125)), (99u ^ OBFH_GUI_NAME_KEY(125)), (116u ^ OBFH_GUI_NAME_KEY(125)), (0u ^ OBFH_GUI_NAME_KEY(125)) }
#undef GetStockObject
#define GetStockObject(...) OBFH_API_CALL(1, GetStockObject, __VA_ARGS__)

// Registry.

#define OBFH_GUI_ID_RegOpenKeyExA 126
#define OBFH_GUI_NAME_RegOpenKeyExA \
    { (82u ^ OBFH_GUI_NAME_KEY(126)), (101u ^ OBFH_GUI_NAME_KEY(126)), (103u ^ OBFH_GUI_NAME_KEY(126)), (79u ^ OBFH_GUI_NAME_KEY(126)), (112u ^ OBFH_GUI_NAME_KEY(126)), (101u ^ OBFH_GUI_NAME_KEY(126)), (110u ^ OBFH_GUI_NAME_KEY(126)), (75u ^ OBFH_GUI_NAME_KEY(126)), (101u ^ OBFH_GUI_NAME_KEY(126)), (121u ^ OBFH_GUI_NAME_KEY(126)), (69u ^ OBFH_GUI_NAME_KEY(126)), (120u ^ OBFH_GUI_NAME_KEY(126)), (65u ^ OBFH_GUI_NAME_KEY(126)), (0u ^ OBFH_GUI_NAME_KEY(126)) }
#undef RegOpenKeyExA
#define RegOpenKeyExA(...) OBFH_API_CALL(3, RegOpenKeyExA, __VA_ARGS__)

#define OBFH_GUI_ID_RegOpenKeyExW 127
#define OBFH_GUI_NAME_RegOpenKeyExW \
    { (82u ^ OBFH_GUI_NAME_KEY(127)), (101u ^ OBFH_GUI_NAME_KEY(127)), (103u ^ OBFH_GUI_NAME_KEY(127)), (79u ^ OBFH_GUI_NAME_KEY(127)), (112u ^ OBFH_GUI_NAME_KEY(127)), (101u ^ OBFH_GUI_NAME_KEY(127)), (110u ^ OBFH_GUI_NAME_KEY(127)), (75u ^ OBFH_GUI_NAME_KEY(127)), (101u ^ OBFH_GUI_NAME_KEY(127)), (121u ^ OBFH_GUI_NAME_KEY(127)), (69u ^ OBFH_GUI_NAME_KEY(127)), (120u ^ OBFH_GUI_NAME_KEY(127)), (87u ^ OBFH_GUI_NAME_KEY(127)), (0u ^ OBFH_GUI_NAME_KEY(127)) }
#undef RegOpenKeyExW
#define RegOpenKeyExW(...) OBFH_API_CALL(3, RegOpenKeyExW, __VA_ARGS__)

#define OBFH_GUI_ID_RegQueryValueExA 128
#define OBFH_GUI_NAME_RegQueryValueExA \
    { (82u ^ OBFH_GUI_NAME_KEY(128)), (101u ^ OBFH_GUI_NAME_KEY(128)), (103u ^ OBFH_GUI_NAME_KEY(128)), (81u ^ OBFH_GUI_NAME_KEY(128)), (117u ^ OBFH_GUI_NAME_KEY(128)), (101u ^ OBFH_GUI_NAME_KEY(128)), (114u ^ OBFH_GUI_NAME_KEY(128)), (121u ^ OBFH_GUI_NAME_KEY(128)), (86u ^ OBFH_GUI_NAME_KEY(128)), (97u ^ OBFH_GUI_NAME_KEY(128)), (108u ^ OBFH_GUI_NAME_KEY(128)), (117u ^ OBFH_GUI_NAME_KEY(128)), (101u ^ OBFH_GUI_NAME_KEY(128)), (69u ^ OBFH_GUI_NAME_KEY(128)), (120u ^ OBFH_GUI_NAME_KEY(128)), (65u ^ OBFH_GUI_NAME_KEY(128)), (0u ^ OBFH_GUI_NAME_KEY(128)) }
#undef RegQueryValueExA
#define RegQueryValueExA(...) OBFH_API_CALL(3, RegQueryValueExA, __VA_ARGS__)

#define OBFH_GUI_ID_RegQueryValueExW 129
#define OBFH_GUI_NAME_RegQueryValueExW \
    { (82u ^ OBFH_GUI_NAME_KEY(129)), (101u ^ OBFH_GUI_NAME_KEY(129)), (103u ^ OBFH_GUI_NAME_KEY(129)), (81u ^ OBFH_GUI_NAME_KEY(129)), (117u ^ OBFH_GUI_NAME_KEY(129)), (101u ^ OBFH_GUI_NAME_KEY(129)), (114u ^ OBFH_GUI_NAME_KEY(129)), (121u ^ OBFH_GUI_NAME_KEY(129)), (86u ^ OBFH_GUI_NAME_KEY(129)), (97u ^ OBFH_GUI_NAME_KEY(129)), (108u ^ OBFH_GUI_NAME_KEY(129)), (117u ^ OBFH_GUI_NAME_KEY(129)), (101u ^ OBFH_GUI_NAME_KEY(129)), (69u ^ OBFH_GUI_NAME_KEY(129)), (120u ^ OBFH_GUI_NAME_KEY(129)), (87u ^ OBFH_GUI_NAME_KEY(129)), (0u ^ OBFH_GUI_NAME_KEY(129)) }
#undef RegQueryValueExW
#define RegQueryValueExW(...) OBFH_API_CALL(3, RegQueryValueExW, __VA_ARGS__)

#define OBFH_GUI_ID_RegSetValueExA 130
#define OBFH_GUI_NAME_RegSetValueExA \
    { (82u ^ OBFH_GUI_NAME_KEY(130)), (101u ^ OBFH_GUI_NAME_KEY(130)), (103u ^ OBFH_GUI_NAME_KEY(130)), (83u ^ OBFH_GUI_NAME_KEY(130)), (101u ^ OBFH_GUI_NAME_KEY(130)), (116u ^ OBFH_GUI_NAME_KEY(130)), (86u ^ OBFH_GUI_NAME_KEY(130)), (97u ^ OBFH_GUI_NAME_KEY(130)), (108u ^ OBFH_GUI_NAME_KEY(130)), (117u ^ OBFH_GUI_NAME_KEY(130)), (101u ^ OBFH_GUI_NAME_KEY(130)), (69u ^ OBFH_GUI_NAME_KEY(130)), (120u ^ OBFH_GUI_NAME_KEY(130)), (65u ^ OBFH_GUI_NAME_KEY(130)), (0u ^ OBFH_GUI_NAME_KEY(130)) }
#undef RegSetValueExA
#define RegSetValueExA(...) OBFH_API_CALL(3, RegSetValueExA, __VA_ARGS__)

#define OBFH_GUI_ID_RegSetValueExW 131
#define OBFH_GUI_NAME_RegSetValueExW \
    { (82u ^ OBFH_GUI_NAME_KEY(131)), (101u ^ OBFH_GUI_NAME_KEY(131)), (103u ^ OBFH_GUI_NAME_KEY(131)), (83u ^ OBFH_GUI_NAME_KEY(131)), (101u ^ OBFH_GUI_NAME_KEY(131)), (116u ^ OBFH_GUI_NAME_KEY(131)), (86u ^ OBFH_GUI_NAME_KEY(131)), (97u ^ OBFH_GUI_NAME_KEY(131)), (108u ^ OBFH_GUI_NAME_KEY(131)), (117u ^ OBFH_GUI_NAME_KEY(131)), (101u ^ OBFH_GUI_NAME_KEY(131)), (69u ^ OBFH_GUI_NAME_KEY(131)), (120u ^ OBFH_GUI_NAME_KEY(131)), (87u ^ OBFH_GUI_NAME_KEY(131)), (0u ^ OBFH_GUI_NAME_KEY(131)) }
#undef RegSetValueExW
#define RegSetValueExW(...) OBFH_API_CALL(3, RegSetValueExW, __VA_ARGS__)

#define OBFH_GUI_ID_RegCloseKey 132
#define OBFH_GUI_NAME_RegCloseKey \
    { (82u ^ OBFH_GUI_NAME_KEY(132)), (101u ^ OBFH_GUI_NAME_KEY(132)), (103u ^ OBFH_GUI_NAME_KEY(132)), (67u ^ OBFH_GUI_NAME_KEY(132)), (108u ^ OBFH_GUI_NAME_KEY(132)), (111u ^ OBFH_GUI_NAME_KEY(132)), (115u ^ OBFH_GUI_NAME_KEY(132)), (101u ^ OBFH_GUI_NAME_KEY(132)), (75u ^ OBFH_GUI_NAME_KEY(132)), (101u ^ OBFH_GUI_NAME_KEY(132)), (121u ^ OBFH_GUI_NAME_KEY(132)), (0u ^ OBFH_GUI_NAME_KEY(132)) }
#undef RegCloseKey
#define RegCloseKey(...) OBFH_API_CALL(3, RegCloseKey, __VA_ARGS__)

// Loading.

#define OBFH_GUI_ID_LoadLibraryW 133
#define OBFH_GUI_NAME_LoadLibraryW \
    { (76u ^ OBFH_GUI_NAME_KEY(133)), (111u ^ OBFH_GUI_NAME_KEY(133)), (97u ^ OBFH_GUI_NAME_KEY(133)), (100u ^ OBFH_GUI_NAME_KEY(133)), (76u ^ OBFH_GUI_NAME_KEY(133)), (105u ^ OBFH_GUI_NAME_KEY(133)), (98u ^ OBFH_GUI_NAME_KEY(133)), (114u ^ OBFH_GUI_NAME_KEY(133)), (97u ^ OBFH_GUI_NAME_KEY(133)), (114u ^ OBFH_GUI_NAME_KEY(133)), (121u ^ OBFH_GUI_NAME_KEY(133)), (87u ^ OBFH_GUI_NAME_KEY(133)), (0u ^ OBFH_GUI_NAME_KEY(133)) }
#undef LoadLibraryW
#define LoadLibraryW(...) OBFH_API_CALL(2, LoadLibraryW, __VA_ARGS__)

#define OBFH_GUI_ID_LoadLibraryExA 134
#define OBFH_GUI_NAME_LoadLibraryExA \
    { (76u ^ OBFH_GUI_NAME_KEY(134)), (111u ^ OBFH_GUI_NAME_KEY(134)), (97u ^ OBFH_GUI_NAME_KEY(134)), (100u ^ OBFH_GUI_NAME_KEY(134)), (76u ^ OBFH_GUI_NAME_KEY(134)), (105u ^ OBFH_GUI_NAME_KEY(134)), (98u ^ OBFH_GUI_NAME_KEY(134)), (114u ^ OBFH_GUI_NAME_KEY(134)), (97u ^ OBFH_GUI_NAME_KEY(134)), (114u ^ OBFH_GUI_NAME_KEY(134)), (121u ^ OBFH_GUI_NAME_KEY(134)), (69u ^ OBFH_GUI_NAME_KEY(134)), (120u ^ OBFH_GUI_NAME_KEY(134)), (65u ^ OBFH_GUI_NAME_KEY(134)), (0u ^ OBFH_GUI_NAME_KEY(134)) }
#undef LoadLibraryExA
#define LoadLibraryExA(...) OBFH_API_CALL(2, LoadLibraryExA, __VA_ARGS__)

#define OBFH_GUI_ID_LoadLibraryExW 135
#define OBFH_GUI_NAME_LoadLibraryExW \
    { (76u ^ OBFH_GUI_NAME_KEY(135)), (111u ^ OBFH_GUI_NAME_KEY(135)), (97u ^ OBFH_GUI_NAME_KEY(135)), (100u ^ OBFH_GUI_NAME_KEY(135)), (76u ^ OBFH_GUI_NAME_KEY(135)), (105u ^ OBFH_GUI_NAME_KEY(135)), (98u ^ OBFH_GUI_NAME_KEY(135)), (114u ^ OBFH_GUI_NAME_KEY(135)), (97u ^ OBFH_GUI_NAME_KEY(135)), (114u ^ OBFH_GUI_NAME_KEY(135)), (121u ^ OBFH_GUI_NAME_KEY(135)), (69u ^ OBFH_GUI_NAME_KEY(135)), (120u ^ OBFH_GUI_NAME_KEY(135)), (87u ^ OBFH_GUI_NAME_KEY(135)), (0u ^ OBFH_GUI_NAME_KEY(135)) }
#undef LoadLibraryExW
#define LoadLibraryExW(...) OBFH_API_CALL(2, LoadLibraryExW, __VA_ARGS__)

// ============================================================================
// 22. Math aliases and typed operand transport
// ============================================================================

static int obfh_abs_proxy(int value) {
    BREAK_STACK_CFLOW;
    PHANTOM_NOP;
    return value < (int)FALSE ? -value : value;
}
#define abs(x) obfh_abs_proxy(x)

#if virt_std == 1
#define OBFH_MATH_KEY() ((ULONG_PTR)VM_OBF_INT(SALT_SHIFT))
#else
#define OBFH_MATH_KEY() ((ULONG_PTR)obfh_condition_proxy((float)_1, (float)obfh_int_proxy(SALT_SHIFT)))
#endif
// Mutate the typed value's address: arithmetic identities can change -0,
// rounding, NaN payloads or wide integer exponents before the math call.
#define _MUTATE_MATH(value) ({                                                                           \
    BREAK_STACK_CFLOW;                                                                                   \
    __typeof__((value)) volatile __obfh_math_value = (value);                                            \
    volatile ULONG_PTR __obfh_math_key = OBFH_MATH_KEY();                                                \
    ULONG_PTR __obfh_math_address = obfh_uintptr_proxy((ULONG_PTR)&__obfh_math_value ^ __obfh_math_key); \
    *(__typeof__(&__obfh_math_value))(__obfh_math_address ^ __obfh_math_key);                            \
})

// Native math calls with typed operand transport.
#define acosh(x) acosh(_MUTATE_MATH(x))
#define asinh(x) asinh(_MUTATE_MATH(x))
#define atanh(x) atanh(_MUTATE_MATH(x))
#define cbrt(x) cbrt(_MUTATE_MATH(x))
#define copysign(x, y) copysign(_MUTATE_MATH(x), _MUTATE_MATH(y))
#define erf(x) erf(_MUTATE_MATH(x))
#define erfc(x) erfc(_MUTATE_MATH(x))
#define exp2(x) exp2(_MUTATE_MATH(x))
#define expm1(x) expm1(_MUTATE_MATH(x))
#define fdim(x, y) fdim(_MUTATE_MATH(x), _MUTATE_MATH(y))
#define fma(x, y, z) fma(_MUTATE_MATH(x), _MUTATE_MATH(y), _MUTATE_MATH(z))
#define fmax(x, y) fmax(_MUTATE_MATH(x), _MUTATE_MATH(y))
#define fmin(x, y) fmin(_MUTATE_MATH(x), _MUTATE_MATH(y))
#define hypot(x, y) hypot(_MUTATE_MATH(x), _MUTATE_MATH(y))
#define ilogb(x) ilogb(_MUTATE_MATH(x))
#define lgamma(x) lgamma(_MUTATE_MATH(x))
#define log1p(x) log1p(_MUTATE_MATH(x))
#define log2(x) log2(_MUTATE_MATH(x))
#define logb(x) logb(_MUTATE_MATH(x))
#define nearbyint(x) nearbyint(_MUTATE_MATH(x))
#define nextafter(x, y) nextafter(_MUTATE_MATH(x), _MUTATE_MATH(y))
#define nexttoward(x, y) nexttoward(_MUTATE_MATH(x), _MUTATE_MATH(y))
#define remainder(x, y) remainder(_MUTATE_MATH(x), _MUTATE_MATH(y))
#define remquo(x, y, z) remquo(_MUTATE_MATH(x), _MUTATE_MATH(y), (z))
#define rint(x) rint(_MUTATE_MATH(x))
#define round(x) round(_MUTATE_MATH(x))
#define scalbln(x, y) scalbln(_MUTATE_MATH(x), _MUTATE_MATH(y))
#define scalbn(x, y) scalbn(_MUTATE_MATH(x), _MUTATE_MATH(y))
#define tgamma(x) tgamma(_MUTATE_MATH(x))
#define trunc(x) trunc(_MUTATE_MATH(x))

// CRT exports resolved through the protected loader.
#define acos(x) ({                                                                                                                           \
    char __obfh_math_name[5];                                                                                                                \
    OBFH_CRT_NAME_ORDER(                                                                                                                     \
        (__obfh_math_name[0] = _a, __obfh_math_name[1] = _c, __obfh_math_name[2] = _o, __obfh_math_name[3] = _s, __obfh_math_name[4] = _0),  \
        (__obfh_math_name[4] = _0, __obfh_math_name[3] = _s, __obfh_math_name[2] = _o, __obfh_math_name[1] = _c, __obfh_math_name[0] = _a)); \
    ((double (*)(double))obfh_crt_resolve(__obfh_math_name))(x);                                                                             \
})

#define asin(x) ({                                                                                                                           \
    char __obfh_math_name[5];                                                                                                                \
    OBFH_CRT_NAME_ORDER(                                                                                                                     \
        (__obfh_math_name[0] = _a, __obfh_math_name[1] = _s, __obfh_math_name[2] = _i, __obfh_math_name[3] = _n, __obfh_math_name[4] = _0),  \
        (__obfh_math_name[4] = _0, __obfh_math_name[3] = _n, __obfh_math_name[2] = _i, __obfh_math_name[1] = _s, __obfh_math_name[0] = _a)); \
    ((double (*)(double))obfh_crt_resolve(__obfh_math_name))(x);                                                                             \
})

#define atan(x) ({                                                                                                                           \
    char __obfh_math_name[5];                                                                                                                \
    OBFH_CRT_NAME_ORDER(                                                                                                                     \
        (__obfh_math_name[0] = _a, __obfh_math_name[1] = _t, __obfh_math_name[2] = _a, __obfh_math_name[3] = _n, __obfh_math_name[4] = _0),  \
        (__obfh_math_name[4] = _0, __obfh_math_name[3] = _n, __obfh_math_name[2] = _a, __obfh_math_name[1] = _t, __obfh_math_name[0] = _a)); \
    ((double (*)(double))obfh_crt_resolve(__obfh_math_name))(x);                                                                             \
})

#define atan2(y, x) ({                                                                                                                                                         \
    char __obfh_math_name[6];                                                                                                                                                  \
    OBFH_CRT_NAME_ORDER(                                                                                                                                                       \
        (__obfh_math_name[0] = _a, __obfh_math_name[1] = _t, __obfh_math_name[2] = _a, __obfh_math_name[3] = _n, __obfh_math_name[4] = (_2 + '0'), __obfh_math_name[5] = _0),  \
        (__obfh_math_name[5] = _0, __obfh_math_name[4] = (_2 + '0'), __obfh_math_name[3] = _n, __obfh_math_name[2] = _a, __obfh_math_name[1] = _t, __obfh_math_name[0] = _a)); \
    ((double (*)(double, double))obfh_crt_resolve(__obfh_math_name))(y, x);                                                                                                    \
})

#define ceil(x) ({                                                                                                                           \
    char __obfh_math_name[5];                                                                                                                \
    OBFH_CRT_NAME_ORDER(                                                                                                                     \
        (__obfh_math_name[0] = _c, __obfh_math_name[1] = _e, __obfh_math_name[2] = _i, __obfh_math_name[3] = _l, __obfh_math_name[4] = _0),  \
        (__obfh_math_name[4] = _0, __obfh_math_name[3] = _l, __obfh_math_name[2] = _i, __obfh_math_name[1] = _e, __obfh_math_name[0] = _c)); \
    ((double (*)(double))obfh_crt_resolve(__obfh_math_name))(x);                                                                             \
})

#define cos(x) ({                                                                                                  \
    char __obfh_math_name[4];                                                                                      \
    OBFH_CRT_NAME_ORDER(                                                                                           \
        (__obfh_math_name[0] = _c, __obfh_math_name[1] = _o, __obfh_math_name[2] = _s, __obfh_math_name[3] = _0),  \
        (__obfh_math_name[3] = _0, __obfh_math_name[2] = _s, __obfh_math_name[1] = _o, __obfh_math_name[0] = _c)); \
    ((double (*)(double))obfh_crt_resolve(__obfh_math_name))(x);                                                   \
})

#define cosh(x) ({                                                                                                                           \
    char __obfh_math_name[5];                                                                                                                \
    OBFH_CRT_NAME_ORDER(                                                                                                                     \
        (__obfh_math_name[0] = _c, __obfh_math_name[1] = _o, __obfh_math_name[2] = _s, __obfh_math_name[3] = _h, __obfh_math_name[4] = _0),  \
        (__obfh_math_name[4] = _0, __obfh_math_name[3] = _h, __obfh_math_name[2] = _s, __obfh_math_name[1] = _o, __obfh_math_name[0] = _c)); \
    ((double (*)(double))obfh_crt_resolve(__obfh_math_name))(x);                                                                             \
})

#define exp(x) ({                                                                                                  \
    char __obfh_math_name[4];                                                                                      \
    OBFH_CRT_NAME_ORDER(                                                                                           \
        (__obfh_math_name[0] = _e, __obfh_math_name[1] = _x, __obfh_math_name[2] = _p, __obfh_math_name[3] = _0),  \
        (__obfh_math_name[3] = _0, __obfh_math_name[2] = _p, __obfh_math_name[1] = _x, __obfh_math_name[0] = _e)); \
    ((double (*)(double))obfh_crt_resolve(__obfh_math_name))(x);                                                   \
})

#define fabs(x) ({                                                                                                                           \
    char __obfh_math_name[5];                                                                                                                \
    OBFH_CRT_NAME_ORDER(                                                                                                                     \
        (__obfh_math_name[0] = _f, __obfh_math_name[1] = _a, __obfh_math_name[2] = _b, __obfh_math_name[3] = _s, __obfh_math_name[4] = _0),  \
        (__obfh_math_name[4] = _0, __obfh_math_name[3] = _s, __obfh_math_name[2] = _b, __obfh_math_name[1] = _a, __obfh_math_name[0] = _f)); \
    ((double (*)(double))obfh_crt_resolve(__obfh_math_name))(x);                                                                             \
})

#define floor(x) ({                                                                                                                                                    \
    char __obfh_math_name[6];                                                                                                                                          \
    OBFH_CRT_NAME_ORDER(                                                                                                                                               \
        (__obfh_math_name[0] = _f, __obfh_math_name[1] = _l, __obfh_math_name[2] = _o, __obfh_math_name[3] = _o, __obfh_math_name[4] = _r, __obfh_math_name[5] = _0),  \
        (__obfh_math_name[5] = _0, __obfh_math_name[4] = _r, __obfh_math_name[3] = _o, __obfh_math_name[2] = _o, __obfh_math_name[1] = _l, __obfh_math_name[0] = _f)); \
    ((double (*)(double))obfh_crt_resolve(__obfh_math_name))(x);                                                                                                       \
})

#define fmod(x, y) ({                                                                                                                        \
    char __obfh_math_name[5];                                                                                                                \
    OBFH_CRT_NAME_ORDER(                                                                                                                     \
        (__obfh_math_name[0] = _f, __obfh_math_name[1] = _m, __obfh_math_name[2] = _o, __obfh_math_name[3] = _d, __obfh_math_name[4] = _0),  \
        (__obfh_math_name[4] = _0, __obfh_math_name[3] = _d, __obfh_math_name[2] = _o, __obfh_math_name[1] = _m, __obfh_math_name[0] = _f)); \
    ((double (*)(double, double))obfh_crt_resolve(__obfh_math_name))(x, y);                                                                  \
})

#define frexp(x, y) ({                                                                                                                                                 \
    char __obfh_math_name[6];                                                                                                                                          \
    OBFH_CRT_NAME_ORDER(                                                                                                                                               \
        (__obfh_math_name[0] = _f, __obfh_math_name[1] = _r, __obfh_math_name[2] = _e, __obfh_math_name[3] = _x, __obfh_math_name[4] = _p, __obfh_math_name[5] = _0),  \
        (__obfh_math_name[5] = _0, __obfh_math_name[4] = _p, __obfh_math_name[3] = _x, __obfh_math_name[2] = _e, __obfh_math_name[1] = _r, __obfh_math_name[0] = _f)); \
    ((double (*)(double, int *))obfh_crt_resolve(__obfh_math_name))(x, y);                                                                                             \
})

#define ldexp(x, y) ({                                                                                                                                                 \
    char __obfh_math_name[6];                                                                                                                                          \
    OBFH_CRT_NAME_ORDER(                                                                                                                                               \
        (__obfh_math_name[0] = _l, __obfh_math_name[1] = _d, __obfh_math_name[2] = _e, __obfh_math_name[3] = _x, __obfh_math_name[4] = _p, __obfh_math_name[5] = _0),  \
        (__obfh_math_name[5] = _0, __obfh_math_name[4] = _p, __obfh_math_name[3] = _x, __obfh_math_name[2] = _e, __obfh_math_name[1] = _d, __obfh_math_name[0] = _l)); \
    ((double (*)(double, int))obfh_crt_resolve(__obfh_math_name))(x, y);                                                                                               \
})

#define log(x) ({                                                                                                  \
    char __obfh_math_name[4];                                                                                      \
    OBFH_CRT_NAME_ORDER(                                                                                           \
        (__obfh_math_name[0] = _l, __obfh_math_name[1] = _o, __obfh_math_name[2] = _g, __obfh_math_name[3] = _0),  \
        (__obfh_math_name[3] = _0, __obfh_math_name[2] = _g, __obfh_math_name[1] = _o, __obfh_math_name[0] = _l)); \
    ((double (*)(double))obfh_crt_resolve(__obfh_math_name))(x);                                                   \
})

#define log10(x) ({                                                                                                                                                                    \
    char __obfh_math_name[6];                                                                                                                                                          \
    OBFH_CRT_NAME_ORDER(                                                                                                                                                               \
        (__obfh_math_name[0] = _l, __obfh_math_name[1] = _o, __obfh_math_name[2] = _g, __obfh_math_name[3] = (_1 + '0'), __obfh_math_name[4] = (_0 + '0'), __obfh_math_name[5] = _0),  \
        (__obfh_math_name[5] = _0, __obfh_math_name[4] = (_0 + '0'), __obfh_math_name[3] = (_1 + '0'), __obfh_math_name[2] = _g, __obfh_math_name[1] = _o, __obfh_math_name[0] = _l)); \
    ((double (*)(double))obfh_crt_resolve(__obfh_math_name))(x);                                                                                                                       \
})

#define modf(x, y) ({                                                                                                                        \
    char __obfh_math_name[5];                                                                                                                \
    OBFH_CRT_NAME_ORDER(                                                                                                                     \
        (__obfh_math_name[0] = _m, __obfh_math_name[1] = _o, __obfh_math_name[2] = _d, __obfh_math_name[3] = _f, __obfh_math_name[4] = _0),  \
        (__obfh_math_name[4] = _0, __obfh_math_name[3] = _f, __obfh_math_name[2] = _d, __obfh_math_name[1] = _o, __obfh_math_name[0] = _m)); \
    ((double (*)(double, double *))obfh_crt_resolve(__obfh_math_name))(x, y);                                                                \
})

#define pow(x, y) ({                                                                                               \
    char __obfh_math_name[4];                                                                                      \
    OBFH_CRT_NAME_ORDER(                                                                                           \
        (__obfh_math_name[0] = _p, __obfh_math_name[1] = _o, __obfh_math_name[2] = _w, __obfh_math_name[3] = _0),  \
        (__obfh_math_name[3] = _0, __obfh_math_name[2] = _w, __obfh_math_name[1] = _o, __obfh_math_name[0] = _p)); \
    ((double (*)(double, double))obfh_crt_resolve(__obfh_math_name))(x, y);                                        \
})

#define sin(x) ({                                                                                                  \
    char __obfh_math_name[4];                                                                                      \
    OBFH_CRT_NAME_ORDER(                                                                                           \
        (__obfh_math_name[0] = _s, __obfh_math_name[1] = _i, __obfh_math_name[2] = _n, __obfh_math_name[3] = _0),  \
        (__obfh_math_name[3] = _0, __obfh_math_name[2] = _n, __obfh_math_name[1] = _i, __obfh_math_name[0] = _s)); \
    ((double (*)(double))obfh_crt_resolve(__obfh_math_name))(x);                                                   \
})

#define sinh(x) ({                                                                                                                           \
    char __obfh_math_name[5];                                                                                                                \
    OBFH_CRT_NAME_ORDER(                                                                                                                     \
        (__obfh_math_name[0] = _s, __obfh_math_name[1] = _i, __obfh_math_name[2] = _n, __obfh_math_name[3] = _h, __obfh_math_name[4] = _0),  \
        (__obfh_math_name[4] = _0, __obfh_math_name[3] = _h, __obfh_math_name[2] = _n, __obfh_math_name[1] = _i, __obfh_math_name[0] = _s)); \
    ((double (*)(double))obfh_crt_resolve(__obfh_math_name))(x);                                                                             \
})

#define sqrt(x) ({                                                                                                                           \
    char __obfh_math_name[5];                                                                                                                \
    OBFH_CRT_NAME_ORDER(                                                                                                                     \
        (__obfh_math_name[0] = _s, __obfh_math_name[1] = _q, __obfh_math_name[2] = _r, __obfh_math_name[3] = _t, __obfh_math_name[4] = _0),  \
        (__obfh_math_name[4] = _0, __obfh_math_name[3] = _t, __obfh_math_name[2] = _r, __obfh_math_name[1] = _q, __obfh_math_name[0] = _s)); \
    ((double (*)(double))obfh_crt_resolve(__obfh_math_name))(x);                                                                             \
})

#define tan(x) ({                                                                                                  \
    char __obfh_math_name[4];                                                                                      \
    OBFH_CRT_NAME_ORDER(                                                                                           \
        (__obfh_math_name[0] = _t, __obfh_math_name[1] = _a, __obfh_math_name[2] = _n, __obfh_math_name[3] = _0),  \
        (__obfh_math_name[3] = _0, __obfh_math_name[2] = _n, __obfh_math_name[1] = _a, __obfh_math_name[0] = _t)); \
    ((double (*)(double))obfh_crt_resolve(__obfh_math_name))(x);                                                   \
})

#define tanh(x) ({                                                                                                                           \
    char __obfh_math_name[5];                                                                                                                \
    OBFH_CRT_NAME_ORDER(                                                                                                                     \
        (__obfh_math_name[0] = _t, __obfh_math_name[1] = _a, __obfh_math_name[2] = _n, __obfh_math_name[3] = _h, __obfh_math_name[4] = _0),  \
        (__obfh_math_name[4] = _0, __obfh_math_name[3] = _h, __obfh_math_name[2] = _n, __obfh_math_name[1] = _a, __obfh_math_name[0] = _t)); \
    ((double (*)(double))obfh_crt_resolve(__obfh_math_name))(x);                                                                             \
})

// ============================================================================
// 23. Library marker and disabled-mode diagnostic
// ============================================================================

__declspec(dllexport) __attribute__((weak)) char *WhatSoundDoesACowMake() OBFH_CODE_SECTION_ATTRIBUTE {
    PHANTOM_NOP;
    return HIDE_STRING("Moo");
}

#elif !OBFH_EDITOR_VIEW
#warning Obfuscation disabled!
#endif
#endif

// ;)
