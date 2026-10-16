/*
         ██████╗ ██████╗ ███████╗██╗   ██╗███████╗     ██╗
        ██╔═══██╗██╔══██╗██╔════╝██║   ██║██╔════╝     ██║
        ██║   ██║██████╔╝█████╗  ██║   ██║███████╗     ███████║
        ██║   ██║██╔══██╗██╔══╝  ██║   ██║╚════██║     ██╔══██║
        ╚██████╔╝██████╔╝██║     ╚██████╔╝███████║ ██╗ ██║  ██║
         ╚═════╝ ╚═════╝ ╚═╝      ╚═════╝ ╚══════╝ ╚═╝ ╚═╝  ╚═╝
              Very reliable armor for your C programs!
                    Coded by (C) DosX, 2026

 [Build settings]
 ~ OBFH_SEED                     = unsigned 32-bit build seed (default: 0u)

 [Additional options]
 ~ OBFH_BETTER_CONTROL_FLOW      = more powerful Control Flow obfuscation (slowly!)
 ~ OBFH_BETTER_ANTIDEBUG         = use better dynamic anti-debugging protection
 ~ OBFH_FAKE_SIGNATURES          = adds fake signatures of various protectors or packers

 [Disabling default features]
 ~ OBFH_DISABLE_CONTROL_FLOW     = disable control flow obfuscation
 ~ OBFH_DISABLE_ANTIDEBUG        = disable antidebug protection

 ~ OBFH_DISABLE_ANY_OBFUSCATION  = disable obfuscation

 GitHub:
 -> https://github.com/DosX-dev/obfus.h

 (Full documentation and examples are available on the GitHub page)
*/

#ifndef OBFH
#define OBFH

/*
 * Reading guide (fold the numbered regions in VS Code to navigate):
 * - Public options and disabled-mode behavior: section 01.
 * - Random values and seed handling: section 03.
 * - Fake functions and skipped instruction bodies: sections 05-07.
 * - Control-flow protection: sections 10 and 13.
 * - Virtual machine: sections 09, 11 and 14.
 * - Platform calls, anti-debugging and library wrappers: sections 15-22.
 *
 * Layout follows preprocessing dependencies, not alphabetical order.
 * 01-09: configuration, entropy, native decoys and shared value helpers.
 * 10-12: CFLOW/VM internals and inline kernels using native control flow.
 * 13:    keyword interception boundary; later function bodies use protection.
 * 14-23: public VM interface, platform/CRT adapters and math aliases.
 *
 * Seed selectors use separate, mutually exclusive #if blocks on purpose.
 * Long #elif chains multiply clang-format's work across nesting levels.
 * Keep fallback conditions complementary and selector expressions immutable.
 *
 * Keep helpers that need native if/for/while above section 13. Macro definitions
 * expand at their use sites: moving a function across that boundary changes it.
 * Keep each proxy and its public alias together. ASM operand order, local labels
 * and captured random draws are contracts, even in skipped code.
 */

// #region 01. Compiler setup, editor interface and public fallbacks
// ============================================================================
// 01. Compiler setup, editor interface and public fallbacks
// ============================================================================

#if !__TINYC__ && !__GNUC__ && !__MINGW32__
#define __attribute__(...)
#endif

#if defined(__TINYC__) && __TINYC__ < 928
#error obfus.h requires TinyCC 0.9.28rc or newer from TinyCC-builder.
#endif

// Editor detection never substitutes a real TCC build.
#if defined(__INTELLISENSE__) && !defined(__TINYC__)
#define OBFH_EDITOR_VIEW 1
#else
#define OBFH_EDITOR_VIEW 0
#endif

// Legacy option names: canonical OBFH_* options take precedence.
#ifdef NO_OBF
#warning NO_OBF is deprecated; use OBFH_DISABLE_ANY_OBFUSCATION.
#ifndef OBFH_DISABLE_ANY_OBFUSCATION
#define OBFH_DISABLE_ANY_OBFUSCATION NO_OBF
#endif
#endif

#ifdef NO_CFLOW
#warning NO_CFLOW is deprecated; use OBFH_DISABLE_CONTROL_FLOW.
#ifndef OBFH_DISABLE_CONTROL_FLOW
#define OBFH_DISABLE_CONTROL_FLOW NO_CFLOW
#endif
#endif

#ifdef NO_ANTIDEBUG
#warning NO_ANTIDEBUG is deprecated; use OBFH_DISABLE_ANTIDEBUG.
#ifndef OBFH_DISABLE_ANTIDEBUG
#define OBFH_DISABLE_ANTIDEBUG NO_ANTIDEBUG
#endif
#endif

#ifdef NO_PDATA_DECOYS
#warning NO_PDATA_DECOYS is deprecated; use OBFH_DISABLE_PDATA_DECOYS.
#ifndef OBFH_DISABLE_PDATA_DECOYS
#define OBFH_DISABLE_PDATA_DECOYS NO_PDATA_DECOYS
#endif
#endif

#ifdef CFLOW_V2
#warning CFLOW_V2 is deprecated; use OBFH_BETTER_CONTROL_FLOW.
#ifndef OBFH_BETTER_CONTROL_FLOW
#define OBFH_BETTER_CONTROL_FLOW CFLOW_V2
#endif
#endif

#ifdef ANTIDEBUG_V2
#warning ANTIDEBUG_V2 is deprecated; use OBFH_BETTER_ANTIDEBUG.
#ifndef OBFH_BETTER_ANTIDEBUG
#define OBFH_BETTER_ANTIDEBUG ANTIDEBUG_V2
#endif
#endif

#ifdef FAKE_SIGNS
#warning FAKE_SIGNS is deprecated; use OBFH_FAKE_SIGNATURES.
#ifndef OBFH_FAKE_SIGNATURES
#define OBFH_FAKE_SIGNATURES FAKE_SIGNS
#endif
#endif

#ifdef MEM_CLEANER__JUST_FOR_FUN
#warning MEM_CLEANER__JUST_FOR_FUN is deprecated; use OBFH_MEMORY_CLEANER.
#ifndef OBFH_MEMORY_CLEANER
#define OBFH_MEMORY_CLEANER MEM_CLEANER__JUST_FOR_FUN
#endif
#endif

#ifdef OBFH_BUILD_SEED
#warning OBFH_BUILD_SEED is deprecated; use OBFH_SEED.
#ifndef OBFH_SEED
#define OBFH_SEED OBFH_BUILD_SEED
#endif
#endif

// Shared entropy settings for compilation and editor expressions.
#if !OBFH_DISABLE_ANY_OBFUSCATION || OBFH_EDITOR_VIEW
#ifndef OBFH_SEED
#define OBFH_SEED 0u
#endif
#endif

// Platform declarations used by the implementation and editor aliases.
#if !OBFH_DISABLE_ANY_OBFUSCATION || OBFH_EDITOR_VIEW
#include <conio.h>
#include <ctype.h>
#include <direct.h>
#include <errno.h>
#include <limits.h>
#include <locale.h>
#include <math.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <windows.h>
#endif

#if OBFH_DISABLE_ANY_OBFUSCATION || OBFH_EDITOR_VIEW
#define OBFH_LOADER ((void)0)
#endif

// Lightweight editor interface; runtime protection is defined below.
#if OBFH_EDITOR_VIEW
#define RND(min, max) (min)

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

#undef strtoll
#undef strtoull
#define strtoll(...) _strtoi64(__VA_ARGS__)
#define strtoull(...) _strtoui64(__VA_ARGS__)

#define OBFH_EDITOR_BOUNDED_LENGTH(char_type, text, count) \
    ({ \
        struct { \
            const char_type *p; \
            size_t n; \
        } __obfh_editor_args = {(text), (count)}; \
        size_t __obfh_editor_length = 0; \
        if (__obfh_editor_args.p) \
            while (__obfh_editor_length < __obfh_editor_args.n && __obfh_editor_args.p[__obfh_editor_length]) \
                ++__obfh_editor_length; \
        __obfh_editor_length; \
    })
#define strnlen_s(text, count) OBFH_EDITOR_BOUNDED_LENGTH(char, text, count)
#define wcsnlen_s(text, count) OBFH_EDITOR_BOUNDED_LENGTH(wchar_t, text, count)

// The bundled import library omits these tokenizers and lldiv. Keep editor
// results typed without adding OBFH_DISABLE_ANY_OBFUSCATION exports.
#define strtok_s(text, delimiters, context) \
    ((char *(__cdecl *)(char *, const char *, char **)) \
         GetProcAddress(GetModuleHandleA("msvcrt"), "strtok_s"))((text), (delimiters), (context))
#define wcstok_s(text, delimiters, context) \
    ((wchar_t * (__cdecl *)(wchar_t *, const wchar_t *, wchar_t **)) \
         GetProcAddress(GetModuleHandleA("msvcrt"), "wcstok_s"))((text), (delimiters), (context))
#define lldiv(numerator, denominator) \
    ({ \
        struct { \
            long long n, d; \
        } __obfh_editor_args = {(numerator), (denominator)}; \
        lldiv_t __obfh_editor_result; \
        __obfh_editor_result.quot = __obfh_editor_args.n / __obfh_editor_args.d; \
        __obfh_editor_result.rem = __obfh_editor_args.n - __obfh_editor_result.quot * __obfh_editor_args.d; \
        __obfh_editor_result; \
    })

#endif  // OBFH_EDITOR_VIEW

// VM editor expressions preserve protected result types and single evaluation.
// These expressions describe the interface; they are not runtime VM handlers.
#if OBFH_EDITOR_VIEW && OBFH_DISABLE_ANY_OBFUSCATION != 1
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
#define VM_AND(a, b) ((unsigned int)(a) & (unsigned int)(b))
#define VM_OR(a, b) ((unsigned int)(a) | (unsigned int)(b))
#define VM_XOR(a, b) ((unsigned int)(a) ^ (unsigned int)(b))
#define VM_NOT(a) (~(unsigned int)(a))
#define VM_SHL(a, b) ((unsigned int)(a) << ((unsigned int)(b)&31u))
#define VM_SHR(a, b) ((unsigned int)(a) >> ((unsigned int)(b)&31u))
#define VM_IF(condition) if (condition)
#define VM_ELSE_IF(condition) else if (condition)
#define VM_ELSE else
// Native expressions when all obfuscation is disabled.
#elif OBFH_DISABLE_ANY_OBFUSCATION == 1 || OBFH_EDITOR_VIEW
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
#define VM_AND(a, b) ((unsigned int)(a) & (unsigned int)(b))
#define VM_OR(a, b) ((unsigned int)(a) | (unsigned int)(b))
#define VM_XOR(a, b) ((unsigned int)(a) ^ (unsigned int)(b))
#define VM_NOT(a) (~(unsigned int)(a))
#define VM_SHL(a, b) ((unsigned int)(a) << ((unsigned int)(b)&31u))
#define VM_SHR(a, b) ((unsigned int)(a) >> ((unsigned int)(b)&31u))
#define VM_IF(condition) if (condition)
#define VM_ELSE_IF(condition) else if (condition)
#define VM_ELSE else
#define VM_OBF_INT(num) (num)
#define VM_OBF_DBL(num) (num)
#endif

// Disabled protection and editor-only placeholders.
#if OBFH_DISABLE_ANY_OBFUSCATION == 1 || OBFH_EDITOR_VIEW
#define HIDE_STRING(str) str
#define BREAK_STACK_CFLOW ((void)0)
#define STACK_PROXY_FUNCTIONS ((void)0)
#define PHANTOM_NOP ((void)0)
#define ANTI_DEBUG 0
#endif

#if !OBFH_DISABLE_ANY_OBFUSCATION && !OBFH_EDITOR_VIEW
#define RND(min, max) \
    ((min) + ((__COUNTER__ + __LINE__ + (unsigned int)OBFH_SEED) * 2654435761u % ((max) - (min) + 1)))

// #endregion

// #region 02. Platform compatibility and section placement
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
#if OBFH_FAKE_SIGNATURES == 1

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

// TCC unwind RVAs are relative to .text. Code also needs executable PE flags.
// Keep functions in .text on both architectures; protected data stays separate.
#if defined(__TINYC__) && defined(_WIN32)
#define OBFH_CODE_SECTION_ATTRIBUTE TEXT_SECTION_ATTRIBUTE
#define OBFH_DATA_CODE_SECTION_ATTRIBUTE TEXT_SECTION_ATTRIBUTE
#else
#define OBFH_CODE_SECTION_ATTRIBUTE OBFH_SECTION_ATTRIBUTE
#define OBFH_DATA_CODE_SECTION_ATTRIBUTE DATA_SECTION_ATTRIBUTE
#endif

// #endregion

// #region 03. Compile-time entropy, strings and typed address transport
// ============================================================================
// 03. Compile-time entropy, strings and typed address transport
// ============================================================================

#define STACK_STRING(str) ((char[]){str})

#define HIDE_STRING(str) \
    (OBFH_HIDE_JUNK, (_0 < RND(1, 255) ? obfh_process_hidden_string(STACK_STRING("\0" str "\0"), (float)__s_rdtsc(RND(0, 255)) != 0.1) : (char *)(ULONG_PTR)((float)__s_rdtsc(RND(0, 255)) == RND(0, 255))))

typedef enum {
    SALT_SHIFT = RND(0xBAD, 0xBEEF)
} VAR_ADDR_SHIFT;

// Pointer-width transport: independently selected reversible stages, separated
// by volatile storage. Only the recovered address is dereferenced; the value's
// type, bits and ABI remain unchanged, including aggregate and floating returns.
#if defined(__x86_64__)
#define OBFH_RET_WIDTH "q"
#else
#define OBFH_RET_WIDTH "l"
#endif

#define OBFH_RET_MIX(address, key, form, inverse) \
    __builtin_choose_expr((form) == 0, ({ \
                              __obfh_asm__("xor" OBFH_RET_WIDTH " %1, %0" \
                                           : "+&r"(address) \
                                           : "r"(key) \
                                           : "cc"); \
                          }), \
                          __builtin_choose_expr((form) == 1, ({ \
                                                    __builtin_choose_expr(inverse, ({ \
                                                                              __obfh_asm__("sub" OBFH_RET_WIDTH " %1, %0" \
                                                                                           : "+&r"(address) \
                                                                                           : "r"(key) \
                                                                                           : "cc"); \
                                                                          }), \
                                                                          ({ \
                                                                              __obfh_asm__("add" OBFH_RET_WIDTH " %1, %0" \
                                                                                           : "+&r"(address) \
                                                                                           : "r"(key) \
                                                                                           : "cc"); \
                                                                          })); \
                                                }), \
                                                __builtin_choose_expr((form) == 2, ({ \
                                                                          __builtin_choose_expr(inverse, ({ \
                                                                                                    __obfh_asm__("add" OBFH_RET_WIDTH " %1, %0" \
                                                                                                                 : "+&r"(address) \
                                                                                                                 : "r"(key) \
                                                                                                                 : "cc"); \
                                                                                                }), \
                                                                                                ({ \
                                                                                                    __obfh_asm__("sub" OBFH_RET_WIDTH " %1, %0" \
                                                                                                                 : "+&r"(address) \
                                                                                                                 : "r"(key) \
                                                                                                                 : "cc"); \
                                                                                                })); \
                                                                      }), \
                                                                      ({ \
                                                                          __builtin_choose_expr(inverse, ({ \
                                                                                                    __obfh_asm__("xor" OBFH_RET_WIDTH " %1, %0; neg" OBFH_RET_WIDTH " %0" \
                                                                                                                 : "+&r"(address) \
                                                                                                                 : "r"(key) \
                                                                                                                 : "cc"); \
                                                                                                }), \
                                                                                                ({ \
                                                                                                    __obfh_asm__("neg" OBFH_RET_WIDTH " %0; xor" OBFH_RET_WIDTH " %1, %0" \
                                                                                                                 : "+&r"(address) \
                                                                                                                 : "r"(key) \
                                                                                                                 : "cc"); \
                                                                                                })); \
                                                                      }))))

#define OBFH_RET_ROTATE(address, count, right) \
    __builtin_choose_expr(right, ({ \
                              __obfh_asm__("ror" OBFH_RET_WIDTH " $%c1, %0" \
                                           : "+r"(address) \
                                           : "i"(count) \
                                           : "cc"); \
                          }), \
                          ({ \
                              __obfh_asm__("rol" OBFH_RET_WIDTH " $%c1, %0" \
                                           : "+r"(address) \
                                           : "i"(count) \
                                           : "cc"); \
                          }))

// Each stage also chooses whether rotation precedes or follows its key mix.
#define OBFH_RET_STAGE(address, key, draw, inverse) \
    ({ \
        enum { __obfh_ret_rotate_first = (((draw) >> 3) & 1u) ^ (inverse), \
               __obfh_ret_rotate_right = (((draw) >> 2) & 1u) ^ (inverse), \
               __obfh_ret_rotate_count = 1u + (((draw) >> 4) % (sizeof(ULONG_PTR) * 8u - 1u)) }; \
        __builtin_choose_expr(__obfh_ret_rotate_first, ({ \
                                  OBFH_RET_ROTATE(address, __obfh_ret_rotate_count, __obfh_ret_rotate_right); \
                                  OBFH_RET_MIX(address, key, (draw)&3u, inverse); \
                              }), \
                              ({ \
                                  OBFH_RET_MIX(address, key, (draw)&3u, inverse); \
                                  OBFH_RET_ROTATE(address, __obfh_ret_rotate_count, __obfh_ret_rotate_right); \
                              })); \
    })

#define RET_BY_VAR(value) \
    { \
        enum { OBFH_DATA_FIELDS(__obfh_ret_first, __COUNTER__), \
               OBFH_DATA_FIELDS(__obfh_ret_second_draw, __COUNTER__), \
               __obfh_ret_second = (__obfh_ret_second_draw & ~3u) | (((__obfh_ret_first & 3u) + 1u + ((__obfh_ret_second_draw >> 8) % 3u)) & 3u) }; \
        volatile ULONG_PTR __obfh_ret_address_slot; \
        ULONG_PTR __obfh_ret_key = (ULONG_PTR)&__obfh_ret_address_slot ^ (ULONG_PTR)__obfh_ret_first; \
        ULONG_PTR __obfh_ret_address = (ULONG_PTR) & (value); \
        OBFH_RET_STAGE(__obfh_ret_address, __obfh_ret_key, __obfh_ret_first, 0); \
        OBFH_RET_STAGE(__obfh_ret_address, __obfh_ret_key ^ (ULONG_PTR)(__obfh_ret_second | 1u), __obfh_ret_second, 0); \
        __obfh_ret_address_slot = __obfh_ret_address; \
        __obfh_ret_address = __obfh_ret_address_slot; \
        OBFH_RET_STAGE(__obfh_ret_address, __obfh_ret_key ^ (ULONG_PTR)(__obfh_ret_second | 1u), __obfh_ret_second, 1); \
        OBFH_RET_STAGE(__obfh_ret_address, __obfh_ret_key, __obfh_ret_first, 1); \
        return *(__typeof__(&(value)))__obfh_ret_address; \
    }
// Independent 16-bit draws keep the payload mix; masking is modulo 65536.
#define OBFH_DRAW16 (((__COUNTER__ + __LINE__ + (unsigned int)OBFH_SEED) * 2654435761u) & 65535u)
#define OBFH_JUNK_BYTE (((OBFH_DRAW16 * 2246822519u) ^ ((OBFH_DRAW16 * 3266489917u) >> 13)) & 255u)
#define OBFH_JUNK_WORD ((OBFH_DRAW16 * 2246822519u) ^ ((unsigned int)OBFH_DRAW16 << 16) ^ (OBFH_DRAW16 * 3266489917u))
#define OBFH_MIX_A(value) (((unsigned int)(value) ^ ((unsigned int)(value) >> 16)) * 2246822507u)
#define OBFH_MIX_B(value) (((unsigned int)(value) ^ ((unsigned int)(value) >> 13)) * 3266489909u)
// Enum fields only: preserve the original uint32 mix without runtime storage.
#define OBFH_MIX_FIELDS(name, value) \
    name##_mix = OBFH_MIX_A(value), name = OBFH_MIX_B(name##_mix)
// Compile-time index forms: 4/6 literal, 1/6 scalar byte, 1/6 protected call.
// Digits are ready before any name builder; no runtime selection branch.
static inline int obfh_int_proxy(int value) OBFH_CODE_SECTION_ATTRIBUTE;
#define OBFH_NAME_INDEX_PICK(index, draw) \
    __builtin_choose_expr((draw) % 6u < 4u, (index), __builtin_choose_expr((draw) % 6u == 4u, OBFH_NAME_BYTE_##index + ((index) / 10) * 10, obfh_int_proxy(index)))
#define OBFH_NAME_INDEX(index) OBFH_NAME_INDEX_PICK(index, OBFH_DATA_DRAW(__COUNTER__))
#define OBFH_NAME_BYTE_0 _0
#define OBFH_NAME_BYTE_1 _1
#define OBFH_NAME_BYTE_2 _2
#define OBFH_NAME_BYTE_3 _3
#define OBFH_NAME_BYTE_4 _4
#define OBFH_NAME_BYTE_5 _5
#define OBFH_NAME_BYTE_6 _6
#define OBFH_NAME_BYTE_7 _7
#define OBFH_NAME_BYTE_8 _8
#define OBFH_NAME_BYTE_9 _9
#define OBFH_NAME_BYTE_10 _0
#define OBFH_NAME_BYTE_11 _1
#define OBFH_NAME_BYTE_12 _2
#define OBFH_NAME_BYTE_13 _3
#define OBFH_NAME_BYTE_14 _4
#define OBFH_NAME_BYTE_15 _5
#define OBFH_NAME_BYTE_16 _6
#define OBFH_NAME_BYTE_17 _7

// Select stores at compile time; each use has its own captured parameters.
#define OBFH_NAME_ORDER_FOR(groups, forward, reverse) ({ \
    OBFH_LOADER_FOR(groups); \
    enum { __obfh_name_order = OBFH_MIX_B(OBFH_JUNK_WORD) }; \
    __builtin_choose_expr(__obfh_name_order & 1u, forward, reverse); \
})

// Only distinct caller-owned byte stores may be paired; neither reads the other.
#define OBFH_NAME_PAIR(bit, first, second) \
    __builtin_choose_expr((__obfh_name_order >> (bit)) & 1u, ((first), (second)), ((second), (first)))
#define OBFH_NAME_ORDER(forward, reverse) OBFH_NAME_ORDER_FOR(OBFH_ALPHA_ALL, forward, reverse)
#define OBFH_CRT_NAME_ORDER_FOR(groups, forward, reverse) OBFH_NAME_ORDER_FOR(groups, forward, reverse)
#define OBFH_CRT_NAME_ORDER(forward, reverse) OBFH_NAME_ORDER(forward, reverse)

#define OBFH_DATA_DRAW(salt) OBFH_MIX_B(OBFH_MIX_A((unsigned int)__LINE__ ^ (unsigned int)OBFH_SEED ^ ((unsigned int)(salt)*2654435761u)))
#define OBFH_DATA_FIELDS(name, salt) \
    OBFH_MIX_FIELDS(name, (unsigned int)__LINE__ ^ (unsigned int)OBFH_SEED ^ ((unsigned int)(salt)*2654435761u))

// #endregion

// #region 04. Retained data and optional instruction padding
// ============================================================================
// 04. Retained data and optional instruction padding
// ============================================================================

// Interleave retained random data; the original volatile objects keep their types.
#define OBFH_DATA_JUNK(name, section) \
    static volatile unsigned char __obfh_data_##name[8u + (OBFH_DATA_DRAW(0u) & 15u)] section __attribute__((used)) = { \
        (OBFH_DATA_DRAW(1u) & 255u), (OBFH_DATA_DRAW(2u) & 255u), (OBFH_DATA_DRAW(3u) & 255u), (OBFH_DATA_DRAW(4u) & 255u), \
        (OBFH_DATA_DRAW(5u) & 255u), (OBFH_DATA_DRAW(6u) & 255u), (OBFH_DATA_DRAW(7u) & 255u), (OBFH_DATA_DRAW(8u) & 255u)}
#define OBFH_STRING_CONST(name, value) \
    static volatile char name[] OBFH_SECTION_ATTRIBUTE = value; \
    OBFH_DATA_JUNK(name, OBFH_SECTION_ATTRIBUTE)
#define OBFH_CHAR_CONST(name, value, section) \
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
// Only name-building letters are lazy. Scalar digits remain available to CFLOW.
// Each group has one native initializer, independent of the protected call chain.
#define OBFH_ALPHA_ROL(value, shift) ((((unsigned)(value) << (shift)) | ((unsigned)(value) >> (8u - (shift)))) & 255u)
#define OBFH_ALPHA_ROR(value, shift) OBFH_ALPHA_ROL(value, 8u - (shift))
#define OBFH_ALPHA_CONST(name, value, section) \
    enum { __obfh_alpha_key_##name = (OBFH_DATA_DRAW(value) & 255u) | 1u, \
           __obfh_alpha_form_##name = (OBFH_DATA_DRAW((value) + 257u) >> 8) % 7u, \
           __obfh_alpha_shift_##name = 1u + OBFH_DATA_DRAW((value) + 521u) % 7u, \
           __obfh_alpha_rotate_##name = OBFH_ALPHA_ROL(value, __obfh_alpha_shift_##name) == (value) ? 1u : __obfh_alpha_shift_##name }; \
    static volatile char name DATA_SECTION_ATTRIBUTE = (char)(unsigned char)(__obfh_alpha_form_##name == 0 ? (value) ^ __obfh_alpha_key_##name : __obfh_alpha_form_##name == 1 ? (value) + __obfh_alpha_key_##name \
                                                                                                                                             : __obfh_alpha_form_##name == 2   ? (value)-__obfh_alpha_key_##name \
                                                                                                                                             : __obfh_alpha_form_##name == 3   ? ~(value) \
                                                                                                                                             : __obfh_alpha_form_##name == 4   ? -(value) \
                                                                                                                                             : __obfh_alpha_form_##name == 5   ? OBFH_ALPHA_ROR(value, __obfh_alpha_rotate_##name) \
                                                                                                                                                                               : OBFH_ALPHA_ROL(value, __obfh_alpha_rotate_##name)); \
    OBFH_DATA_JUNK(name, section)
// Byte-sized in-place operations keep the one-time loader compact.
#define OBFH_ALPHA_BYTE(op, name) ({ \
    __asm__ __volatile__(op " $%c1, %0" \
                         : "+m"(name) \
                         : "i"(__obfh_alpha_key_##name) \
                         : "cc"); \
})
#define OBFH_ALPHA_UNARY(op, name) ({ \
    __asm__ __volatile__(op " %0" \
                         : "+m"(name)::"cc"); \
})
#define OBFH_ALPHA_ROTATE(op, name) ({ \
    __asm__ __volatile__(op " $%c1, %0" \
                         : "+m"(name) \
                         : "i"(__obfh_alpha_rotate_##name) \
                         : "cc"); \
})
// Independent pairs change order without a runtime branch or additional operation.
#define OBFH_ALPHA_PAIR(first, second) \
    __builtin_choose_expr(OBFH_DATA_DRAW(__obfh_alpha_key_##first + __obfh_alpha_key_##second) & 1u, \
                          ({ OBFH_ALPHA_DECODE(first); OBFH_ALPHA_DECODE(second); }), \
                          ({ OBFH_ALPHA_DECODE(second); OBFH_ALPHA_DECODE(first); }))
#define OBFH_ALPHA_DECODE(name) \
    __builtin_choose_expr(__obfh_alpha_form_##name == 0, OBFH_ALPHA_BYTE("xorb", name), \
                          __builtin_choose_expr(__obfh_alpha_form_##name == 1, OBFH_ALPHA_BYTE("subb", name), \
                                                __builtin_choose_expr(__obfh_alpha_form_##name == 2, OBFH_ALPHA_BYTE("addb", name), \
                                                                      __builtin_choose_expr(__obfh_alpha_form_##name == 3, OBFH_ALPHA_UNARY("notb", name), \
                                                                                            __builtin_choose_expr(__obfh_alpha_form_##name == 4, OBFH_ALPHA_UNARY("negb", name), \
                                                                                                                  __builtin_choose_expr(__obfh_alpha_form_##name == 5, OBFH_ALPHA_ROTATE("rolb", name), OBFH_ALPHA_ROTATE("rorb", name)))))))

OBFH_ALPHA_CONST(_a, 'a', TEXT_SECTION_ATTRIBUTE);
OBFH_ALPHA_CONST(_b, 'b', DATA_SECTION_ATTRIBUTE);
OBFH_ALPHA_CONST(_c, 'c', OBFH_SECTION_ATTRIBUTE);
OBFH_ALPHA_CONST(_d, 'd', TEXT_SECTION_ATTRIBUTE);
OBFH_ALPHA_CONST(_e, 'e', DATA_SECTION_ATTRIBUTE);
OBFH_ALPHA_CONST(_f, 'f', OBFH_SECTION_ATTRIBUTE);
OBFH_ALPHA_CONST(_g, 'g', TEXT_SECTION_ATTRIBUTE);
OBFH_ALPHA_CONST(_h, 'h', DATA_SECTION_ATTRIBUTE);
OBFH_ALPHA_CONST(_i, 'i', OBFH_SECTION_ATTRIBUTE);
OBFH_ALPHA_CONST(_j, 'j', TEXT_SECTION_ATTRIBUTE);
OBFH_ALPHA_CONST(_k, 'k', DATA_SECTION_ATTRIBUTE);
OBFH_ALPHA_CONST(_l, 'l', OBFH_SECTION_ATTRIBUTE);
OBFH_ALPHA_CONST(_m, 'm', TEXT_SECTION_ATTRIBUTE);
OBFH_ALPHA_CONST(_n, 'n', DATA_SECTION_ATTRIBUTE);
OBFH_ALPHA_CONST(_o, 'o', OBFH_SECTION_ATTRIBUTE);
OBFH_ALPHA_CONST(_p, 'p', TEXT_SECTION_ATTRIBUTE);
OBFH_ALPHA_CONST(_q, 'q', DATA_SECTION_ATTRIBUTE);
OBFH_ALPHA_CONST(_r, 'r', OBFH_SECTION_ATTRIBUTE);
OBFH_ALPHA_CONST(_s, 's', TEXT_SECTION_ATTRIBUTE);
OBFH_ALPHA_CONST(_t, 't', DATA_SECTION_ATTRIBUTE);
OBFH_ALPHA_CONST(_u, 'u', OBFH_SECTION_ATTRIBUTE);
OBFH_ALPHA_CONST(_v, 'v', TEXT_SECTION_ATTRIBUTE);
OBFH_ALPHA_CONST(_w, 'w', DATA_SECTION_ATTRIBUTE);
OBFH_ALPHA_CONST(_x, 'x', OBFH_SECTION_ATTRIBUTE);
OBFH_ALPHA_CONST(_y, 'y', TEXT_SECTION_ATTRIBUTE);
OBFH_ALPHA_CONST(_z, 'z', DATA_SECTION_ATTRIBUTE);
OBFH_ALPHA_CONST(_S, 'S', OBFH_SECTION_ATTRIBUTE);
OBFH_ALPHA_CONST(_L, 'L', TEXT_SECTION_ATTRIBUTE);
OBFH_ALPHA_CONST(_A, 'A', DATA_SECTION_ATTRIBUTE);
OBFH_ALPHA_CONST(_I, 'I', OBFH_SECTION_ATTRIBUTE);
OBFH_ALPHA_CONST(_D, 'D', TEXT_SECTION_ATTRIBUTE);
OBFH_ALPHA_CONST(_P, 'P', OBFH_SECTION_ATTRIBUTE);
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

// Aligned x86/x64 loads acquire the published bytes without a locked warm-path read.
// The memory clobber keeps subsequent letter reads after readiness observation.
#define OBFH_LOADER_READ(state) ({ \
    LONG __obfh_alpha_ready; \
    __asm__ __volatile__("movl %1, %0" \
                         : "=r"(__obfh_alpha_ready) \
                         : "m"(state) \
                         : "memory"); \
    __obfh_alpha_ready; \
})

static volatile LONG obfh_alpha_low_state;
static inline void obfh_alpha_low_load(void) {
    if (InterlockedCompareExchange(&obfh_alpha_low_state, 1, 0) == 0) {
        OBFH_ALPHA_PAIR(_a, _h);
        OBFH_ALPHA_PAIR(_b, _i);
        OBFH_ALPHA_PAIR(_c, _j);
        OBFH_ALPHA_PAIR(_d, _k);
        OBFH_ALPHA_PAIR(_e, _l);
        OBFH_ALPHA_PAIR(_f, _m);
        OBFH_ALPHA_DECODE(_g);
        InterlockedExchange(&obfh_alpha_low_state, 2);
    } else {
        while (OBFH_LOADER_READ(obfh_alpha_low_state) != 2)
            __asm__ __volatile__("pause" ::
                                     : "memory");
    }
}

static volatile LONG obfh_alpha_high_state;
static inline void obfh_alpha_high_load(void) {
    if (InterlockedCompareExchange(&obfh_alpha_high_state, 1, 0) == 0) {
        OBFH_ALPHA_PAIR(_n, _u);
        OBFH_ALPHA_PAIR(_o, _v);
        OBFH_ALPHA_PAIR(_p, _w);
        OBFH_ALPHA_PAIR(_q, _x);
        OBFH_ALPHA_PAIR(_r, _y);
        OBFH_ALPHA_PAIR(_s, _z);
        OBFH_ALPHA_DECODE(_t);
        InterlockedExchange(&obfh_alpha_high_state, 2);
    } else {
        while (OBFH_LOADER_READ(obfh_alpha_high_state) != 2)
            __asm__ __volatile__("pause" ::
                                     : "memory");
    }
}

static volatile LONG obfh_alpha_upper_state;
static inline void obfh_alpha_upper_load(void) {
    if (InterlockedCompareExchange(&obfh_alpha_upper_state, 1, 0) == 0) {
        OBFH_ALPHA_PAIR(_S, _I);
        OBFH_ALPHA_PAIR(_L, _D);
        OBFH_ALPHA_PAIR(_A, _P);
        InterlockedExchange(&obfh_alpha_upper_state, 2);
    } else {
        while (OBFH_LOADER_READ(obfh_alpha_upper_state) != 2)
            __asm__ __volatile__("pause" ::
                                     : "memory");
    }
}

enum { OBFH_ALPHA_LOW = 1u,
       OBFH_ALPHA_HIGH = 2u,
       OBFH_ALPHA_UPPER = 4u,
       OBFH_ALPHA_ALL = 7u };
#define OBFH_LOADER_GROUP(state, load) (OBFH_LOADER_READ(state) == 2 ? (void)0 : (load)())
#define OBFH_LOADER_FOR(groups) ({ \
    __builtin_choose_expr(!!((groups)&OBFH_ALPHA_LOW), OBFH_LOADER_GROUP(obfh_alpha_low_state, obfh_alpha_low_load), (void)0); \
    __builtin_choose_expr(!!((groups)&OBFH_ALPHA_HIGH), OBFH_LOADER_GROUP(obfh_alpha_high_state, obfh_alpha_high_load), (void)0); \
    __builtin_choose_expr(!!((groups)&OBFH_ALPHA_UPPER), OBFH_LOADER_GROUP(obfh_alpha_upper_state, obfh_alpha_upper_load), (void)0); \
    (void)0; \
})
#define OBFH_LOADER OBFH_LOADER_FOR(OBFH_ALPHA_ALL)

#if defined(__TINYC__)
#define __obfh_asm__(...) asm(__VA_ARGS__)
#else
#define __obfh_asm__(...) __asm__ __volatile(__VA_ARGS__)
#endif

// Compile-time byte variation only: no runtime branch, register or flag changes.
#define OBFH_PHANTOM_DRAW(site) \
    OBFH_MIX_B(OBFH_MIX_A((unsigned int)(site) ^ (unsigned int)OBFH_SEED ^ (unsigned int)__LINE__ * 2654435761u))
// Capture the first mix before expanding the second; its argument is repeated.
#define OBFH_PHANTOM_CAPTURE(site_name, draw_name, site) \
    enum { site_name = (site), \
           site_name##_mix = OBFH_MIX_A((unsigned int)site_name ^ (unsigned int)OBFH_SEED ^ (unsigned int)__LINE__ * 2654435761u), \
           draw_name = OBFH_MIX_B(site_name##_mix) }
#define OBFH_PHANTOM_NOP_AT(site) \
    ({ \
        OBFH_PHANTOM_CAPTURE(__obfh_phantom_site, __obfh_phantom_draw, site); \
        __obfh_asm__(".fill %c0, 1, 0x90;" \
                     : \
                     : "i"(__obfh_phantom_draw & 1u)); \
    })
#define PHANTOM_NOP OBFH_PHANTOM_NOP_AT(__LINE__)

// #endregion

// #region 05. Native decoys with PE unwind metadata
// ============================================================================
// 05. Native decoys with PE unwind metadata
// ============================================================================

// Static PE unwind-backed decoys. TCC alone supplies the RVA relocations.
// TCC 0.9.28rc records the actual C entry. Let it allocate the native frame;
// the ASM body uses a Windows-recognized epilogue matching that unwind info.
#if defined(__TINYC__) && defined(__x86_64__) && defined(_WIN32) && !OBFH_DISABLE_PDATA_DECOYS
#define OBFH_PD_MIX_A(value) ((((value) ^ ((value) >> 16)) * 2246822519u) & 0xffffffffu)
#define OBFH_PD_MIX_B(value) ((((value) ^ ((value) >> 13)) * 3266489917u) & 0xffffffffu)
#define OBFH_PD_DRAW(site, salt) OBFH_PD_MIX_B(OBFH_PD_MIX_A(((OBFH_SEED & 0xffffffffu) ^ (((site) + 1u) * 2654435761u & 0xffffffffu) ^ ((salt)*2246822519u & 0xffffffffu))))
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
#define OBFH_PD_ASM(body) \
    __obfh_asm__( \
        "movl %%ecx, %%eax; movl %%eax, -%c[slot](%%rbp); " body \
        ".fill %c[phantom], 1,0x90; .byte 0x48, 0x8d, 0x65, 0, 0x5d, 0xc3;" \
        : \
        : [scratch] "m"(__obfh_pd_scratch), [frame] "i"(__obfh_pd_frame), [slot] "i"(__obfh_pd_slot), \
          [key] "i"(__obfh_pd_key), [key2] "i"(__obfh_pd_key2), [mul] "i"(__obfh_pd_mul), \
          [rotate] "i"(__obfh_pd_rotate), [loops] "i"(__obfh_pd_loops), \
          [phantom] "i"(OBFH_PHANTOM_DRAW(__obfh_pd_key) & 1u) \
        : "rax", "rcx", "rdx", "cc", "memory")

#if OBFH_DISABLE_CONTROL_FLOW != 1
#define OBFH_PD_LIVE_ENABLED 1
#else
#define OBFH_PD_LIVE_ENABLED 0
#endif
// Some existing native carriers also transport live CFLOW state.
// Their prologue/epilogue stays identical; RDX/R8 supply the remaining ladder.
// Scatter the live nodes over the existing minimum-sized pool reproducibly.
enum {
    __obfh_pd_live_stride = 1u + 2u * (OBFH_PD_DRAW(0u, 101u) & 7u),
    __obfh_pd_live_shift = OBFH_PD_DRAW(0u, 102u) % 86u,
    __obfh_pd_live_inverse = __obfh_pd_live_stride == 1u ? 1u : __obfh_pd_live_stride == 3u ? 29u
                                                            : __obfh_pd_live_stride == 5u   ? 69u
                                                            : __obfh_pd_live_stride == 7u   ? 37u
                                                            : __obfh_pd_live_stride == 9u   ? 67u
                                                            : __obfh_pd_live_stride == 11u  ? 47u
                                                            : __obfh_pd_live_stride == 13u  ? 53u
                                                                                            : 23u
};
#define OBFH_PD_LIVE_STRIDE __obfh_pd_live_stride
#define OBFH_PD_LIVE_SHIFT __obfh_pd_live_shift
#define OBFH_PD_LIVE_INVERSE __obfh_pd_live_inverse
#define OBFH_PD_LIVE_SITE(node) (((unsigned int)(node)*OBFH_PD_LIVE_STRIDE + OBFH_PD_LIVE_SHIFT) % 86u)
#define OBFH_PD_LIVE_INDEX(site) ((((unsigned int)(site) + 86u - OBFH_PD_LIVE_SHIFT) * OBFH_PD_LIVE_INVERSE) % 86u)
#define OBFH_PD_LIVE_PRE_0 OBFH_PD_BODY_0
#define OBFH_PD_LIVE_PRE_1 OBFH_PD_BODY_1
#define OBFH_PD_LIVE_PRE_2 OBFH_PD_BODY_2
#define OBFH_PD_LIVE_PRE_3 OBFH_PD_BODY_3
#define OBFH_PD_LIVE_PRE_4 OBFH_PD_BODY_4
#define OBFH_PD_LIVE_PRE_5 OBFH_PD_BODY_6
#define OBFH_PD_LIVE_PRE_6 OBFH_PD_BODY_7
#define OBFH_PD_LIVE_PRE_7 OBFH_PD_BODY_10
#define OBFH_PD_LIVE_ASM(pre) \
    __obfh_asm__( \
        "movq %%rdx, -8(%%rbp); movq %%r8, -16(%%rbp); movl %%ecx, %%eax;" pre \
        "cmpq $0, -8(%%rbp); je 1f; movl %%eax, %%ecx; movq -16(%%rbp), %%rdx;xorl %%r8d, %%r8d; call *-8(%%rbp); 1:testb $1, %%al; jz 2f; xorl %[even_key], %%eax; imull %[mul], %%eax; addl %[even_add], %%eax; jmp 3f;2: imull %[mul], %%eax; addl %[even_add], %%eax; xorl %[even_key], %%eax; 3:.fill %c[phantom], 1,0x90; .byte 0x48, 0x8d, 0x65, 0, 0x5d, 0xc3;" \
        : \
        : [scratch] "m"(__obfh_pd_scratch), [frame] "i"(__obfh_pd_frame), [key] "i"(__obfh_pd_key), [key2] "i"(__obfh_pd_key2), \
          [mul] "i"(__obfh_pd_mul), [rotate] "i"(__obfh_pd_rotate), \
          [even_key] "i"(__obfh_pd_key2 & ~1u), [even_add] "i"(__obfh_pd_key & ~1u), \
          [phantom] "i"(OBFH_PHANTOM_DRAW(__obfh_pd_key) & 1u) \
        : "rax", "rcx", "rdx", "r8", "cc", "memory")

#define OBFH_PD_LIVE_SELECT(kind) \
    __builtin_choose_expr((kind) < 4u, \
                          __builtin_choose_expr((kind) < 2u, \
                                                __builtin_choose_expr((kind) == 0u, ({ OBFH_PD_LIVE_ASM(OBFH_PD_LIVE_PRE_0); }), ({ OBFH_PD_LIVE_ASM(OBFH_PD_LIVE_PRE_1); })), \
                                                __builtin_choose_expr((kind) == 2u, ({ OBFH_PD_LIVE_ASM(OBFH_PD_LIVE_PRE_2); }), ({ OBFH_PD_LIVE_ASM(OBFH_PD_LIVE_PRE_3); }))), \
                          __builtin_choose_expr((kind) < 6u, \
                                                __builtin_choose_expr((kind) == 4u, ({ OBFH_PD_LIVE_ASM(OBFH_PD_LIVE_PRE_4); }), ({ OBFH_PD_LIVE_ASM(OBFH_PD_LIVE_PRE_5); })), \
                                                __builtin_choose_expr((kind) == 6u, ({ OBFH_PD_LIVE_ASM(OBFH_PD_LIVE_PRE_6); }), ({ OBFH_PD_LIVE_ASM(OBFH_PD_LIVE_PRE_7); }))))
#define OBFH_PD_DEFINE(site) \
    static unsigned int __obfh_pdata_decoy_##site(unsigned int __input, void *__child, void *__context) __attribute__((noinline, used)); \
    static unsigned int __obfh_pdata_decoy_##site(unsigned int __input, void *__child, void *__context) { \
        enum { \
            __obfh_pd_kind = OBFH_PD_DRAW(site, 1u) & 15u, \
            __obfh_pd_frame = 48u + 16u * (OBFH_PD_DRAW(site, 2u) % 14u), \
            __obfh_pd_slot = 8u + 8u * (OBFH_PD_DRAW(site, 3u) % 5u), \
            __obfh_pd_key = OBFH_PD_DRAW(site, 4u), \
            __obfh_pd_key2 = OBFH_PD_DRAW(site, 5u), \
            __obfh_pd_mul = OBFH_PD_DRAW(site, 6u) | 1u, \
            __obfh_pd_rotate = 1u + (OBFH_PD_DRAW(site, 7u) % 31u), \
            __obfh_pd_loops = 2u + (OBFH_PD_DRAW(site, 8u) % 6u) \
        }; \
        unsigned char __obfh_pd_scratch[__obfh_pd_frame]; \
        __builtin_choose_expr(OBFH_PD_LIVE_INDEX(site) < 16u && OBFH_PD_LIVE_ENABLED, OBFH_PD_LIVE_SELECT(__obfh_pd_kind & 7u), \
                              __builtin_choose_expr(__obfh_pd_kind < 8u, \
                                                    __builtin_choose_expr(__obfh_pd_kind < 4u, \
                                                                          __builtin_choose_expr(__obfh_pd_kind < 2u, \
                                                                                                __builtin_choose_expr(__obfh_pd_kind < 1u, ({ OBFH_PD_ASM(OBFH_PD_BODY_0); }), ({ OBFH_PD_ASM(OBFH_PD_BODY_1); })), \
                                                                                                __builtin_choose_expr(__obfh_pd_kind < 3u, ({ OBFH_PD_ASM(OBFH_PD_BODY_2); }), ({ OBFH_PD_ASM(OBFH_PD_BODY_3); }))), \
                                                                          __builtin_choose_expr(__obfh_pd_kind < 6u, \
                                                                                                __builtin_choose_expr(__obfh_pd_kind < 5u, ({ OBFH_PD_ASM(OBFH_PD_BODY_4); }), ({ OBFH_PD_ASM(OBFH_PD_BODY_5); })), \
                                                                                                __builtin_choose_expr(__obfh_pd_kind < 7u, ({ OBFH_PD_ASM(OBFH_PD_BODY_6); }), ({ OBFH_PD_ASM(OBFH_PD_BODY_7); })))), \
                                                    __builtin_choose_expr(__obfh_pd_kind < 12u, \
                                                                          __builtin_choose_expr(__obfh_pd_kind < 10u, \
                                                                                                __builtin_choose_expr(__obfh_pd_kind < 9u, ({ OBFH_PD_ASM(OBFH_PD_BODY_8); }), ({ OBFH_PD_ASM(OBFH_PD_BODY_9); })), \
                                                                                                __builtin_choose_expr( \
                                                                                                    __obfh_pd_kind < 11u, ({ OBFH_PD_ASM(OBFH_PD_BODY_10); }), ({ OBFH_PD_ASM(OBFH_PD_BODY_11); }))), \
                                                                          __builtin_choose_expr(__obfh_pd_kind < 14u, \
                                                                                                __builtin_choose_expr(__obfh_pd_kind < 13u, ({ OBFH_PD_ASM(OBFH_PD_BODY_12); }), ({ OBFH_PD_ASM(OBFH_PD_BODY_13); })), \
                                                                                                __builtin_choose_expr( \
                                                                                                    __obfh_pd_kind < 15u, ({ OBFH_PD_ASM(OBFH_PD_BODY_14); }), ({ OBFH_PD_ASM(OBFH_PD_BODY_15); })))))); \
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

// #endregion

// #region 06. Skipped payloads, ASM operands and BREAK_STACK_CFLOW templates
// ============================================================================
// 06. Skipped payloads, ASM operands and BREAK_STACK_CFLOW templates
// ============================================================================

#define OBFH_JUNK_RANDOM_INPUTS \
    "i"(OBFH_JUNK_BYTE), "i"(OBFH_JUNK_BYTE), "i"(OBFH_JUNK_BYTE), "i"(OBFH_JUNK_BYTE), "i"(OBFH_JUNK_WORD)

// Operand positions are part of the payload contract. Keep each draw per expansion.
#define OBFH_JUNK_INPUTS "i"(RND(1, 15)), "i"(RND(0, 255)), OBFH_JUNK_RANDOM_INPUTS
#define OBFH_STACK_JUNK_INPUTS "i"(RND(1, 32767)), OBFH_JUNK_INPUTS
#define OBFH_CFLOW_RANDOM_INPUTS "i"(RND(1, 32767)), "i"(RND(1, 15)), "i"(RND(0, 255)), "i"(RND(1, 2147483646u))
#define OBFH_JUNK_CLOBBERS "eax", "ebx", "ecx", "edx", "cc", "memory"
#define OBFH_CFLOW_CLOBBERS "eax", "edx", "ecx", "cc", "memory"
#define OBFH_JUNK_ASM(code) __obfh_asm__(code \
                                         : \
                                         : OBFH_JUNK_INPUTS \
                                         : OBFH_JUNK_CLOBBERS)
#define OBFH_STACK_JUNK_ASM(code, ...) __obfh_asm__(code \
                                                    : \
                                                    : OBFH_STACK_JUNK_INPUTS \
                                                    : __VA_ARGS__, "cc", "memory")
#define OBFH_CFLOW_ASM(code) __obfh_asm__(code \
                                          : \
                                          : OBFH_CFLOW_RANDOM_INPUTS \
                                          : OBFH_CFLOW_CLOBBERS)

// Payload lengths stay fixed; word-sized fill and opcode choices reuse existing immediate draws.
#define OBFH_JUNK_PAYLOAD ".byte %c2, %c3, %c4, %c5; .long %c6; .fill %c0, 1,((( %c1)&0xffffffff)^0x80000000)-0x80000000;"
#define OBFH_STACK_JUNK_PAYLOAD ".byte %c3, %c4, %c5, %c6; .long %c7; .fill %c1, 1,((( %c2)&0xffffffff)^0x80000000)-0x80000000;"
#define OBFH_CFLOW_FILL ".fill (.Lcfpad >> 2), 4,.Lcfbasic; .fill (.Lcfpad & 3), 1,.Lcfbyte;"
#define OBFH_CFLOW_PAYLOAD_CALL ".byte (0xE8 + (.Lcfbasic & 1)); " OBFH_CFLOW_FILL
#define OBFH_CFLOW_PAYLOAD_INDIRECT ".byte 0xFF, (0x15 + ((.Lcfbasic >> 4) & 1) * 16); .long .Lcfbasic; " OBFH_CFLOW_FILL
#define OBFH_CFLOW_PAYLOAD_MOV ".fill ((.Lcfbasic >> 8) & 1),1,0x66; .byte (0xB8 + ((.Lcfbasic >> 9) & 7)); .long .Lcfbasic; " OBFH_CFLOW_FILL
#define OBFH_CFLOW_PAYLOAD_JUMP ".byte (0xB8 + ((.Lcfbasic >> 12) & 7)); .long .Lcfbasic; " OBFH_CFLOW_FILL
#define OBFH_CFLOW_PAYLOAD_CALL_DATA ".byte 0xE8, .Lcfbyte; .long .Lcfbasic; " OBFH_CFLOW_FILL
#define OBFH_CFLOW_PAYLOAD_TRAP_INDIRECT ".byte 0x0F, 0x0B; .byte 0xFF, 0x25; .long .Lcfbasic; .byte .Lcfbyte, 0xE9;"

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

// MOV direction varies at fixed width; only skipped data varies in payloads. Live code reads SP only.
#define OBFH_CFLOW_NAMED_INPUTS \
    [junk_salt] "i"(RND(1, 32767)), "i"(RND(1, 15)), "i"(RND(0, 255)), \
        [junk_key] "i"(OBFH_JUNK_WORD), OBFH_JUNK_RANDOM_INPUTS, [junk_rotate] "i"(RND(1, 31))
#define OBFH_CFLOW_NAMED_ASM(code) __obfh_asm__(code \
                                                : \
                                                : OBFH_CFLOW_NAMED_INPUTS \
                                                : OBFH_CFLOW_CLOBBERS)
// BEGIN SHORT ASM FORMS
// Pack the input spelling once, outside the template pool. All arithmetic is uint32.
#define OBFH_INPUT_CORE(p, salt, rotate, key) \
    enum { p##_f = ((unsigned int)(key) >> 17) & 3u, \
           p##_o = (p##_f == 0 || p##_f == 3) && (((unsigned int)(key) >> 12) & 1u), \
           p##_n = 8u - (p##_f == 3), \
           p##_a = p##_f == 1 ? 0x05u : p##_f == 2 ? 0x2du \
                                                   : 0x35u, \
           p##_m = p##_f == 1 ? 0xc0u : p##_f == 2 ? 0xe8u \
                                                   : 0xf0u, \
           p##_r = p##_f == 3 ? 0xc80fu : 0xc1u | ((0xc0u + 8u * (((unsigned int)(key) >> 13) & 1u)) << 8) | (((rotate) + (((unsigned int)(key) >> 13) & 1u) * (32u - 2u * (rotate))) << 16), \
           p##_x = p##_f == 3 ? OBFH_INPUT_BSWAP(salt) : ((unsigned int)(salt) << (rotate)) | ((unsigned int)(salt) >> (32u - (rotate))) }
#define OBFH_INPUT_BSWAP(x) (((unsigned int)(x) << 24) | (((unsigned int)(x)&0xff00u) << 8) | (((unsigned int)(x) >> 8) & 0xff00u) | ((unsigned int)(x) >> 24))
#define OBFH_INPUT_EAX(p, salt) \
    enum { p##_0a = p##_o ? (unsigned int)p##_r | ((unsigned int)p##_a << (8u * (p##_n - 5u))) | ((p##_n == 7 ? (unsigned int)p##_x : 0u) << 24) : p##_a | ((unsigned int)(salt) << 8), \
           p##_0b = p##_o ? (p##_n == 7 ? (unsigned int)p##_x >> 8 : (unsigned int)p##_x) : ((unsigned int)(salt) >> 24) | ((unsigned int)p##_r << 8) }
#define OBFH_INPUT_OTHER(p, salt, index) \
    enum { p##_##index##s = (unsigned int)p##_r + ((index) << 8), \
           p##_##index##a = p##_o ? p##_##index##s | (0x81u << (8u * (p##_n - 5u))) | ((p##_n == 7 ? p##_m + (index) : 0u) << 24) : 0x81u | ((p##_m + (index)) << 8) | ((unsigned int)(salt) << 16), \
           p##_##index##b = p##_o ? (p##_n == 7 ? (unsigned int)p##_x : p##_m + (index) | ((unsigned int)p##_x << 8)) : ((unsigned int)(salt) >> 16) | (p##_##index##s << 16) }
// Fixed EAX/EDX input encodings; MOV preserves flags, zeroing XOR/SUB requires dead flags.
#define OBFH_CFLOW_INPUT_AX ".short 0xe089-7166*((.Lcfk>>5)&1);.long .Lcfi0a;.fill 1,.Lcfn-4,(((.Lcfi0b)&0xffffffff)^0x80000000)-0x80000000;"
#define OBFH_CFLOW_INPUT_DX ".short 0xe289-3582*((.Lcfk>>5)&1);.long .Lcfi2a,.Lcfi2b;.fill .Lcfn-7,1,.Lcft;"
#define OBFH_ASM_ZERO32(index, form) \
    ".fill ((" form "!=2)&1),2,(0xc031+2304*" index "-8*((" form "==1)&1));.fill ((" form "==2)&1),1,(0xb8+" index ");.fill ((" form "==2)&1),4,0;"
#define OBFH_ASM_MOV32(dst, src, key, bit) \
    ".short 0x89+256*(0xc0+8*" src "+" dst ")+((" key ">>" bit ")&1)*(2+1792*(" dst "-" src "));"
// Alternate opcode direction, not arithmetic operand order. Flags are identical.
// Only low register-register operands are supported; each form is two bytes.
#define OBFH_ASM_BINARY32(opcode, dst, src, key, bit) \
    ".short " opcode "+256*(0xc0+8*" src "+" dst ")+((" key ">>" bit ")&1)*(2+1792*(" dst "-" src "));"
// Only equality consumers may reverse the operands: other CMP flags differ.
#define OBFH_ASM_EQ32(dst, src, key, bit) \
    ".short 0x39+256*(0xc0+8*" src "+" dst ")+((" key ">>" bit ")&1)*1792*(" dst "-" src ");"
#define OBFH_ASM_XOR32(index, value, enabled) \
    ".fill " enabled ",1,(0x35+76*((" index "!=0)&1));.fill (" enabled ")*((" index "!=0)&1),1,(0xf0+" index ");.fill " enabled ",4,(((" value ")&0xffffffff)^0x80000000)-0x80000000;"
// Keep the two-byte rotate-by-one spelling; all other input rotates stay three bytes.
#define OBFH_ASM_INPUT_ROTATE(index, rotate, key) \
    ".fill ((" rotate "==1)&1),2,(0xc0d1+256*" index ");.fill ((" rotate "!=1)&1),3,(0xc1+256*(0xc0+" index "+8*((" key ">>13)&1))+65536*(" rotate "+((" key ">>13)&1)*(32-2*" rotate ")));"
#define OBFH_ASM_XOR_ROTATE(index, salt, rotate, key) \
    OBFH_ASM_XOR32(index, salt, "((" key ">>12)&1)^1") \
    OBFH_ASM_INPUT_ROTATE(index, rotate, key) \
    OBFH_ASM_XOR32(index, "((" salt "<<" rotate ")|(" salt ">>(32-" rotate ")))", "((" key ">>12)&1)")
#define OBFH_ASM_ROTATE_PAIR(index, rotate, key, flip) \
    ".fill ((" rotate "==1)&1),2,(0xd1+256*(0xc0+" index "+8*(((" key ">>14)&1)^" flip ")));.fill ((" rotate "!=1)&1),3,(0xc1+256*(0xc0+" index "+8*(((" key ">>14)&1)^" flip "))+65536*" rotate ");"
#define OBFH_CFLOW_BINARY_AX_DX(opcode, bit) ".short " opcode "+0xd000-3582*((.Lcfk>>" bit ")&1);"
#define OBFH_CFLOW_BINARY_DX_CX(opcode, bit) ".short " opcode "+0xca00+1794*((.Lcfk>>" bit ")&1);"
#define OBFH_CFLOW_BINARY_AX_CX(opcode, bit) ".short " opcode "+0xc800-1790*((.Lcfk>>" bit ")&1);"
#define OBFH_CFLOW_EQ_AX_DX(bit) ".short 0xd039-3584*((.Lcfk>>" bit ")&1);"
// END SHORT ASM FORMS: fixed CFLOW pairs above retain both direction encodings.
#define OBFH_CFLOW_INPUT(reg, index) ".short 0xe089+256*" index "+((.Lcfk>>5)&1)*(1792*" index "-7166);.long .Lcfi" index "a;.fill 1,4-((" index "==0)&1)*(8-.Lcfn),(((.Lcfi" index "b)&0xffffffff)^0x80000000)-0x80000000;.fill ((" index "!=0)&1)*(.Lcfn-7),1,.Lcft;"
#define OBFH_CFLOW_DATA ".byte .Lcfb0,.Lcfb1,.Lcfb2,.Lcfb3;.long .Lcfdata;.fill (.Lcfpad>>2),4,((.Lcfk&0xffffffff)^0x80000000)-0x80000000;.fill (.Lcfpad&3),1,.Lcfbyte;"
#define OBFH_CFLOW_DATA_CALL ".byte (0xE8 + ((.Lcfk >> 15) & 1)); .long .Lcfk;" OBFH_CFLOW_DATA
#define OBFH_CFLOW_DATA_STACK ".byte 0xBC; .long .Lcfdata; .byte 0xFF, 0xE4;" OBFH_CFLOW_DATA
#define OBFH_CFLOW_DATA_FRAME ".byte 0xC8, .Lcfb0, .Lcfb1, .Lcfb2, 0xC9, 0xC3;" OBFH_CFLOW_DATA
#define OBFH_CFLOW_DATA_INDIRECT ".byte 0xFF, 0x15; .long .Lcfdata; .byte 0xE9; .long .Lcfk;" OBFH_CFLOW_DATA
#define OBFH_CFLOW_DATA_TRAP ".byte 0x0F, 0x0B, 0x8F, 0x04, 0x24;" OBFH_CFLOW_DATA
#define OBFH_CFLOW_DATA_RETURN ".byte 0xC2, .Lcfb0, .Lcfb1, 0xE8; .long .Lcfdata;" OBFH_CFLOW_DATA
#define OBFH_CFLOW_PRED_ADD_CARRY OBFH_CFLOW_INPUT_AX OBFH_CFLOW_TAIL_ADD_CARRY
#define OBFH_CFLOW_COPY_DX(bit) ".short 0xc289+0xe02*((.Lcfk>>" bit ")&1);"
#define OBFH_CFLOW_COPY_CX(bit) ".short 0xc189+0x702*((.Lcfk>>" bit ")&1);"
// Source-only spelling choices reuse captured keys. No extra instructions.
#define OBFH_CFLOW_DOUBLE(reg, direction, shift) ".short (0xc001+2304*" reg "+2*((.Lcfk>>" direction ")&1))*(1-((.Lcfk>>" shift ")&1))+(0xe0d1+256*" reg ")*((.Lcfk>>" shift ")&1);"
// BSWAP EDX is independent of the ECX chain; MOV and BSWAP preserve flags.
#define OBFH_CFLOW_BSWAP_PAIR ".fill ((.Lcfk>>8)&1)^1,2,0xca0f;movl $.Lcfk,%%ecx;bswap %%ecx;.fill (.Lcfk>>8)&1,2,0xca0f;"
// Move only the ECX constant load around the EDX multiply, not across its consumer.
#define OBFH_CFLOW_MULTIPLY_PAIR ".fill (.Lcfk>>9)&1,1,0xb9;.fill (.Lcfk>>9)&1,4,((.Lcfk&0xffffffff)^0x80000000)-0x80000000;imull $.Lcfs,%%edx;.fill ((.Lcfk>>9)&1)^1,1,0xb9;.fill ((.Lcfk>>9)&1)^1,4,((.Lcfk&0xffffffff)^0x80000000)-0x80000000;imull $.Lcfs,%%ecx;"
#define OBFH_CFLOW_HEAD_ADD_CARRY OBFH_CFLOW_COPY_DX("6") " xorl $.Lcfk, %%eax; andl $.Lcfk, %%edx; " OBFH_CFLOW_DOUBLE("2", "19", "8") OBFH_CFLOW_BINARY_AX_DX("0x01", "20")
#define OBFH_CFLOW_SECOND_ADD_CARRY "addl $.Lcfk, %%edx; " OBFH_CFLOW_BINARY_AX_DX("0x39", "21")
#define OBFH_CFLOW_TAIL_ADD_CARRY OBFH_CFLOW_HEAD_ADD_CARRY OBFH_CFLOW_INPUT_DX OBFH_CFLOW_SECOND_ADD_CARRY
#define OBFH_CFLOW_PRED_OR_AND_SUM OBFH_CFLOW_INPUT_AX OBFH_CFLOW_TAIL_OR_AND_SUM
#define OBFH_CFLOW_HEAD_OR_AND_SUM OBFH_CFLOW_COPY_DX("7") " orl $.Lcfk, %%eax; andl $.Lcfk, %%edx; " OBFH_CFLOW_BINARY_AX_DX("0x01", "22")
#define OBFH_CFLOW_SECOND_OR_AND_SUM "addl $.Lcfk, %%edx; " OBFH_CFLOW_BINARY_AX_DX("0x39", "23")
#define OBFH_CFLOW_TAIL_OR_AND_SUM OBFH_CFLOW_HEAD_OR_AND_SUM OBFH_CFLOW_INPUT_DX OBFH_CFLOW_SECOND_OR_AND_SUM
#define OBFH_CFLOW_PRED_SUB_BORROW OBFH_CFLOW_INPUT_AX OBFH_CFLOW_TAIL_SUB_BORROW
#define OBFH_CFLOW_HEAD_SUB_BORROW OBFH_CFLOW_COPY_DX("8") " xorl $.Lcfk, %%eax; notl %%edx; andl $.Lcfk, %%edx; " OBFH_CFLOW_DOUBLE("2", "24", "9") OBFH_CFLOW_BINARY_AX_DX("0x29", "25")
#define OBFH_CFLOW_SECOND_SUB_BORROW "subl $.Lcfk, %%edx; " OBFH_CFLOW_BINARY_AX_DX("0x39", "26")
#define OBFH_CFLOW_TAIL_SUB_BORROW OBFH_CFLOW_HEAD_SUB_BORROW OBFH_CFLOW_INPUT_DX OBFH_CFLOW_SECOND_SUB_BORROW
#define OBFH_CFLOW_PRED_DEMORGAN OBFH_CFLOW_INPUT_AX OBFH_CFLOW_TAIL_DEMORGAN
#define OBFH_CFLOW_HEAD_DEMORGAN "orl $.Lcfk, %%eax; notl %%eax; "
#define OBFH_CFLOW_SECOND_DEMORGAN "notl %%edx; movl $.Lcfk, %%ecx; notl %%ecx; " OBFH_CFLOW_BINARY_DX_CX("0x21", "27") OBFH_CFLOW_BINARY_AX_DX("0x39", "28")
#define OBFH_CFLOW_TAIL_DEMORGAN OBFH_CFLOW_HEAD_DEMORGAN OBFH_CFLOW_INPUT_DX OBFH_CFLOW_SECOND_DEMORGAN
#define OBFH_CFLOW_PRED_OR_DISTRIBUTE OBFH_CFLOW_INPUT_AX OBFH_CFLOW_TAIL_OR_DISTRIBUTE
#define OBFH_CFLOW_HEAD_OR_DISTRIBUTE OBFH_CFLOW_COPY_DX("9") " orl $.Lcfk, %%eax; movl $.Lcfk, %%ecx; notl %%ecx; " OBFH_CFLOW_BINARY_DX_CX("0x09", "29") OBFH_CFLOW_BINARY_AX_DX("0x21", "18")
#define OBFH_CFLOW_SECOND_OR_DISTRIBUTE OBFH_CFLOW_BINARY_AX_DX("0x39", "19")
#define OBFH_CFLOW_TAIL_OR_DISTRIBUTE OBFH_CFLOW_HEAD_OR_DISTRIBUTE OBFH_CFLOW_INPUT_DX OBFH_CFLOW_SECOND_OR_DISTRIBUTE
#define OBFH_CFLOW_PRED_AND_PARTITION OBFH_CFLOW_INPUT_AX OBFH_CFLOW_TAIL_AND_PARTITION
#define OBFH_CFLOW_HEAD_AND_PARTITION OBFH_CFLOW_COPY_DX("10") " andl $.Lcfk, %%eax; movl $.Lcfk, %%ecx; notl %%ecx; " OBFH_CFLOW_BINARY_DX_CX("0x21", "20") OBFH_CFLOW_BINARY_AX_DX("0x09", "21")
#define OBFH_CFLOW_SECOND_AND_PARTITION OBFH_CFLOW_BINARY_AX_DX("0x39", "22")
#define OBFH_CFLOW_TAIL_AND_PARTITION OBFH_CFLOW_HEAD_AND_PARTITION OBFH_CFLOW_INPUT_DX OBFH_CFLOW_SECOND_AND_PARTITION
#define OBFH_CFLOW_PRED_XOR_CANCEL OBFH_CFLOW_INPUT_AX OBFH_CFLOW_TAIL_XOR_CANCEL
#define OBFH_CFLOW_TAIL_XOR_CANCEL "" OBFH_CFLOW_COPY_DX("11") " xorl $.Lcfk, %%eax; " OBFH_CFLOW_BINARY_AX_DX("0x31", "23") " cmpl $.Lcfk, %%eax;"
#define OBFH_CFLOW_PRED_COMPLEMENT_CARRY OBFH_CFLOW_INPUT_AX OBFH_CFLOW_TAIL_COMPLEMENT_CARRY
#define OBFH_CFLOW_TAIL_COMPLEMENT_CARRY "" OBFH_CFLOW_COPY_DX("12") " notl %%edx; " OBFH_CFLOW_BINARY_AX_DX("0x01", "24") ""
#define OBFH_CFLOW_PRED_COMPLEMENT_WRAP OBFH_CFLOW_INPUT_AX OBFH_CFLOW_TAIL_COMPLEMENT_WRAP
#define OBFH_CFLOW_TAIL_COMPLEMENT_WRAP "" OBFH_CFLOW_COPY_DX("13") " notl %%edx; " OBFH_CFLOW_BINARY_AX_DX("0x01", "25") " addl $1, %%eax;"
#define OBFH_CFLOW_PRED_MASK_SUBTRACT OBFH_CFLOW_INPUT_AX OBFH_CFLOW_TAIL_MASK_SUBTRACT
#define OBFH_CFLOW_TAIL_MASK_SUBTRACT "" OBFH_CFLOW_COPY_DX("14") " andl $.Lcfk, %%edx; " OBFH_CFLOW_BINARY_AX_DX("0x29", "26") ""
#define OBFH_CFLOW_PRED_MASK_OR_ORDER OBFH_CFLOW_INPUT_AX OBFH_CFLOW_TAIL_MASK_OR_ORDER
#define OBFH_CFLOW_TAIL_MASK_OR_ORDER "" OBFH_CFLOW_COPY_DX("15") " orl $.Lcfk, %%eax; " OBFH_CFLOW_BINARY_AX_DX("0x39", "27") ""
#define OBFH_CFLOW_PRED_ROTATE_XOR OBFH_CFLOW_INPUT_AX OBFH_CFLOW_TAIL_ROTATE_XOR
#define OBFH_CFLOW_HEAD_ROTATE_XOR "xorl $.Lcfk, %%eax; roll $.Lcfr, %%eax; "
#define OBFH_CFLOW_SECOND_ROTATE_XOR "roll $.Lcfr, %%edx; movl $.Lcfk, %%ecx; roll $.Lcfr, %%ecx; " OBFH_CFLOW_BINARY_DX_CX("0x31", "28") OBFH_CFLOW_BINARY_AX_DX("0x39", "29")
#define OBFH_CFLOW_TAIL_ROTATE_XOR OBFH_CFLOW_HEAD_ROTATE_XOR OBFH_CFLOW_INPUT_DX OBFH_CFLOW_SECOND_ROTATE_XOR
#define OBFH_CFLOW_PRED_ROTATE_RESTORE OBFH_CFLOW_INPUT_AX OBFH_CFLOW_TAIL_ROTATE_RESTORE
#define OBFH_CFLOW_HEAD_ROTATE_RESTORE OBFH_ASM_ROTATE_PAIR("0", ".Lcfr", ".Lcfk", "0") OBFH_ASM_ROTATE_PAIR("0", ".Lcfr", ".Lcfk", "1")
#define OBFH_CFLOW_SECOND_ROTATE_RESTORE OBFH_CFLOW_EQ_AX_DX("16")
#define OBFH_CFLOW_TAIL_ROTATE_RESTORE OBFH_CFLOW_HEAD_ROTATE_RESTORE OBFH_CFLOW_INPUT_DX OBFH_CFLOW_SECOND_ROTATE_RESTORE
#define OBFH_CFLOW_PRED_BSWAP_XOR OBFH_CFLOW_INPUT_AX OBFH_CFLOW_TAIL_BSWAP_XOR
#define OBFH_CFLOW_HEAD_BSWAP_XOR "xorl $.Lcfk, %%eax; bswap %%eax; "
#define OBFH_CFLOW_SECOND_BSWAP_XOR OBFH_CFLOW_BSWAP_PAIR OBFH_CFLOW_BINARY_DX_CX("0x31", "18") OBFH_CFLOW_EQ_AX_DX("16")
#define OBFH_CFLOW_TAIL_BSWAP_XOR OBFH_CFLOW_HEAD_BSWAP_XOR OBFH_CFLOW_INPUT_DX OBFH_CFLOW_SECOND_BSWAP_XOR
#define OBFH_CFLOW_PRED_WORD_PARTITION OBFH_CFLOW_INPUT_AX OBFH_CFLOW_TAIL_WORD_PARTITION
#define OBFH_CFLOW_HEAD_WORD_PARTITION OBFH_CFLOW_COPY_DX("16") " andl $65535, %%eax; andl $-65536, %%edx; " OBFH_CFLOW_BINARY_AX_DX("0x09", "19")
#define OBFH_CFLOW_SECOND_WORD_PARTITION OBFH_CFLOW_BINARY_AX_DX("0x39", "20")
#define OBFH_CFLOW_TAIL_WORD_PARTITION OBFH_CFLOW_HEAD_WORD_PARTITION OBFH_CFLOW_INPUT_DX OBFH_CFLOW_SECOND_WORD_PARTITION
#define OBFH_CFLOW_PRED_BYTE_PARITY OBFH_CFLOW_INPUT_AX OBFH_CFLOW_TAIL_BYTE_PARITY
#define OBFH_CFLOW_TAIL_BYTE_PARITY "movzbl %%al, %%edx; imull %%eax, %%eax; " OBFH_CFLOW_BINARY_AX_DX("0x01", "21") " testb $1, %%al;"
#define OBFH_CFLOW_PRED_BYTE_ROTATE OBFH_CFLOW_INPUT_AX OBFH_CFLOW_TAIL_BYTE_ROTATE
#define OBFH_CFLOW_TAIL_BYTE_ROTATE "" OBFH_CFLOW_COPY_DX("17") " rolb $.Lcfr, %%al; rorb $.Lcfr, %%al; cmpb %%dl, %%al;"
#define OBFH_CFLOW_PRED_WORD_ROTATE OBFH_CFLOW_INPUT_AX OBFH_CFLOW_TAIL_WORD_ROTATE
#define OBFH_CFLOW_TAIL_WORD_ROTATE "" OBFH_CFLOW_COPY_DX("6") " rolw $.Lcfr, %%ax; rorw $.Lcfr, %%ax; cmpw %%dx, %%ax;"
#define OBFH_CFLOW_PRED_NEG_COMPLEMENT OBFH_CFLOW_INPUT_AX OBFH_CFLOW_TAIL_NEG_COMPLEMENT
#define OBFH_CFLOW_HEAD_NEG_COMPLEMENT "notl %%eax; addl $1, %%eax; "
#define OBFH_CFLOW_SECOND_NEG_COMPLEMENT "negl %%edx; " OBFH_CFLOW_EQ_AX_DX("16")
#define OBFH_CFLOW_TAIL_NEG_COMPLEMENT OBFH_CFLOW_HEAD_NEG_COMPLEMENT OBFH_CFLOW_INPUT_DX OBFH_CFLOW_SECOND_NEG_COMPLEMENT
#define OBFH_CFLOW_PRED_MUL_DISTRIBUTE OBFH_CFLOW_INPUT_AX OBFH_CFLOW_TAIL_MUL_DISTRIBUTE
#define OBFH_CFLOW_HEAD_MUL_DISTRIBUTE "addl $.Lcfk, %%eax; imull $.Lcfs, %%eax; "
#define OBFH_CFLOW_SECOND_MUL_DISTRIBUTE OBFH_CFLOW_MULTIPLY_PAIR OBFH_CFLOW_BINARY_DX_CX("0x01", "22") OBFH_CFLOW_EQ_AX_DX("16")
#define OBFH_CFLOW_TAIL_MUL_DISTRIBUTE OBFH_CFLOW_HEAD_MUL_DISTRIBUTE OBFH_CFLOW_INPUT_DX OBFH_CFLOW_SECOND_MUL_DISTRIBUTE
#define OBFH_CFLOW_PRED_SQUARE_EXPAND OBFH_CFLOW_INPUT_AX OBFH_CFLOW_TAIL_SQUARE_EXPAND
#define OBFH_CFLOW_TAIL_SQUARE_EXPAND "" OBFH_CFLOW_COPY_DX("7") " imull %%edx, %%edx; " OBFH_CFLOW_COPY_CX("8") " imull $.Lcfk, %%ecx; " OBFH_CFLOW_DOUBLE("1", "23", "10") " addl $.Lcfk, %%eax; imull %%eax, %%eax; " OBFH_CFLOW_BINARY_AX_DX("0x29", "24") OBFH_CFLOW_BINARY_AX_CX("0x29", "25") " movl $.Lcfk, %%edx; imull %%edx, %%edx; " OBFH_CFLOW_BINARY_AX_DX("0x39", "26") ""
#define OBFH_CFLOW_PRED_NEG_SQUARE OBFH_CFLOW_INPUT_AX OBFH_CFLOW_TAIL_NEG_SQUARE
#define OBFH_CFLOW_HEAD_NEG_SQUARE "negl %%eax; imull %%eax, %%eax; "
#define OBFH_CFLOW_SECOND_NEG_SQUARE "imull %%edx, %%edx; " OBFH_CFLOW_BINARY_AX_DX("0x39", "27")
#define OBFH_CFLOW_TAIL_NEG_SQUARE OBFH_CFLOW_HEAD_NEG_SQUARE OBFH_CFLOW_INPUT_DX OBFH_CFLOW_SECOND_NEG_SQUARE
#define OBFH_CFLOW_PRED_BSWAP_NOT OBFH_CFLOW_INPUT_AX OBFH_CFLOW_TAIL_BSWAP_NOT
#define OBFH_CFLOW_HEAD_BSWAP_NOT "notl %%eax; bswap %%eax; "
#define OBFH_CFLOW_SECOND_BSWAP_NOT "bswap %%edx; notl %%edx; " OBFH_CFLOW_EQ_AX_DX("16")
#define OBFH_CFLOW_TAIL_BSWAP_NOT OBFH_CFLOW_HEAD_BSWAP_NOT OBFH_CFLOW_INPUT_DX OBFH_CFLOW_SECOND_BSWAP_NOT
#define OBFH_CFLOW_PRED_SUB_MASK OBFH_CFLOW_INPUT_AX OBFH_CFLOW_TAIL_SUB_MASK
#define OBFH_CFLOW_HEAD_SUB_MASK OBFH_CFLOW_COPY_DX("9") " andl $.Lcfk, %%edx; " OBFH_CFLOW_BINARY_AX_DX("0x29", "28")
#define OBFH_CFLOW_SECOND_SUB_MASK "movl $.Lcfk, %%ecx; notl %%ecx; " OBFH_CFLOW_BINARY_DX_CX("0x21", "29") OBFH_CFLOW_BINARY_AX_DX("0x39", "18")
#define OBFH_CFLOW_TAIL_SUB_MASK OBFH_CFLOW_HEAD_SUB_MASK OBFH_CFLOW_INPUT_DX OBFH_CFLOW_SECOND_SUB_MASK
#define OBFH_CFLOW_PRED_MASK_ORDER OBFH_CFLOW_INPUT_AX OBFH_CFLOW_TAIL_MASK_ORDER
#define OBFH_CFLOW_TAIL_MASK_ORDER "" OBFH_CFLOW_COPY_DX("10") " andl $.Lcfk, %%eax; orl $.Lcfk, %%edx; " OBFH_CFLOW_BINARY_AX_DX("0x39", "19") ""
#define OBFH_CFLOW_PRED_COMPLEMENT_TRANSLATE OBFH_CFLOW_INPUT_AX OBFH_CFLOW_TAIL_COMPLEMENT_TRANSLATE
#define OBFH_CFLOW_HEAD_COMPLEMENT_TRANSLATE "notl %%eax; addl $.Lcfk, %%eax; "
#define OBFH_CFLOW_SECOND_COMPLEMENT_TRANSLATE "negl %%edx; addl $.Lcfk, %%edx; subl $1, %%edx; " OBFH_CFLOW_BINARY_AX_DX("0x39", "20")
#define OBFH_CFLOW_TAIL_COMPLEMENT_TRANSLATE OBFH_CFLOW_HEAD_COMPLEMENT_TRANSLATE OBFH_CFLOW_INPUT_DX OBFH_CFLOW_SECOND_COMPLEMENT_TRANSLATE
#define OBFH_CFLOW_PRED_SQUARE_RESIDUE OBFH_CFLOW_INPUT_AX OBFH_CFLOW_TAIL_SQUARE_RESIDUE
#define OBFH_CFLOW_TAIL_SQUARE_RESIDUE "imull %%eax, %%eax; andl $7, %%eax; leal -1(%%eax), %%edx; leal -4(%%eax), %%ecx; imull %%edx, %%eax; imull %%ecx, %%eax; testl %%eax, %%eax;"
#define OBFH_CFLOW_PRED_FOURTH_RESIDUE OBFH_CFLOW_INPUT_AX OBFH_CFLOW_TAIL_FOURTH_RESIDUE
#define OBFH_CFLOW_TAIL_FOURTH_RESIDUE "imull %%eax, %%eax; imull %%eax, %%eax; andl $15, %%eax; leal -1(%%eax), %%edx; imull %%edx, %%eax; testl %%eax, %%eax;"
#define OBFH_CFLOW_PRED_ISOLATE_BIT OBFH_CFLOW_INPUT_AX OBFH_CFLOW_TAIL_ISOLATE_BIT
#define OBFH_CFLOW_TAIL_ISOLATE_BIT "" OBFH_CFLOW_COPY_DX("11") " negl %%edx; " OBFH_CFLOW_BINARY_AX_DX("0x21", "21") " leal -1(%%eax), %%edx; " OBFH_CFLOW_BINARY_AX_DX("0x21", "22") " testl %%eax, %%eax;"

#define OBFH_CFLOW_NATIVE_PAYLOAD ".byte .Lcfb0,.Lcfb1,.Lcfb2,.Lcfb3;.long .Lcfdata;.fill .Lcfpad,1,.Lcfbyte;"
#define OBFH_CFLOW_PRED_CONSTANTS ".set .Lcfk,%c[junk_key];.set .Lcfr,%c[junk_rotate];.set .Lcfs,%c[junk_salt];"
#define OBFH_CFLOW_INPUT_CONSTANTS ".set .Lcfn,%c[il];.set .Lcft,%c[it];.set .Lcfi0a,%c[i0a];.set .Lcfi0b,%c[i0b];.set .Lcfi2a,%c[i2a];.set .Lcfi2b,%c[i2b];"
#define OBFH_CFLOW_DATA_CONSTANTS ".set .Lcfpad,%c1;.set .Lcfbyte,%c2;.set .Lcfb0,%c4;.set .Lcfb1,%c5;.set .Lcfb2,%c6;.set .Lcfb3,%c7;.set .Lcfdata,%c8;"
// One local set of immediate parameters per selection. Each operand retains
// its old range and position; no runtime storage or shared helper is needed.
#define OBFH_CFLOW_CAPTURE \
    enum { \
        __obfh_cf_salt = RND(1, 32767), \
        __obfh_cf_pad = RND(1, 7), \
        __obfh_cf_byte = RND(0, 255), \
        __obfh_cf_key = RND(1, 2147483646u), \
        __obfh_cf_word = OBFH_JUNK_WORD, \
        __obfh_cf_byte0 = OBFH_JUNK_BYTE, \
        __obfh_cf_byte1 = OBFH_JUNK_BYTE, \
        __obfh_cf_byte2 = OBFH_JUNK_BYTE, \
        __obfh_cf_byte3 = OBFH_JUNK_BYTE, \
        __obfh_cf_payload = OBFH_JUNK_WORD, \
        __obfh_cf_rotate = RND(1, 31) \
    }; \
    OBFH_INPUT_CORE(__ci, __obfh_cf_salt, __obfh_cf_rotate, __obfh_cf_word); \
    OBFH_INPUT_EAX(__ci, __obfh_cf_salt); \
    /* Named predicates load EAX and EDX only. */ \
    OBFH_INPUT_OTHER(__ci, __obfh_cf_salt, 2); \
    enum { __ct = __ci_o ? (unsigned int)__ci_x >> 24 : (unsigned int)__ci_r >> 16 }; \
    __obfh_asm__(OBFH_CFLOW_PRED_CONSTANTS OBFH_CFLOW_INPUT_CONSTANTS OBFH_CFLOW_DATA_CONSTANTS ".set .Lcfbasic,%c[basic_key];" \
                 : \
                 : OBFH_CFLOW_LOCAL_NAMED_INPUTS, [basic_key] "i"(__obfh_cf_key))
#define OBFH_CFLOW_LOCAL_RANDOM \
    "i"(__obfh_cf_byte0), "i"(__obfh_cf_byte1), "i"(__obfh_cf_byte2), "i"(__obfh_cf_byte3), "i"(__obfh_cf_payload)
#define OBFH_CFLOW_LOCAL_JUNK_INPUTS "i"(__obfh_cf_pad), "i"(__obfh_cf_byte), OBFH_CFLOW_LOCAL_RANDOM
#define OBFH_CFLOW_LOCAL_ASM(code) __obfh_asm__(code \
                                                : \
                                                : \
                                                : OBFH_CFLOW_CLOBBERS)
#define OBFH_CFLOW_LOCAL_JUNK_ASM(code) \
    __obfh_asm__(code \
                 : \
                 : [tail_key] "i"(__obfh_cf_payload), [tail_pad] "i"(__obfh_cf_pad) \
                 : OBFH_JUNK_CLOBBERS)
#define OBFH_CFLOW_LOCAL_STACK_ASM(code, ...) \
    __obfh_asm__(code \
                 : \
                 : "i"(__obfh_cf_salt) \
                 : __VA_ARGS__, "cc", "memory")
// No ECX input encoding is consumed by the named predicates.
#define OBFH_CFLOW_LOCAL_NAMED_INPUTS [junk_salt] "i"(__obfh_cf_salt), "i"(__obfh_cf_pad), "i"(__obfh_cf_byte), \
                                      [junk_key] "i"(__obfh_cf_word), OBFH_CFLOW_LOCAL_RANDOM, [junk_rotate] "i"(__obfh_cf_rotate), \
                                      [i0a] "i"(__ci_0a), [i0b] "i"(__ci_0b), \
                                      [i2a] "i"(__ci_2a), [i2b] "i"(__ci_2b), [it] "i"(__ct), [il] "i"(__ci_n)
#define OBFH_CFLOW_LOCAL_NAMED_ASM(code) __obfh_asm__(code \
                                                      : \
                                                      : \
                                                      : OBFH_CFLOW_CLOBBERS)
// Native fragments are adjacent immediate-only ASM blocks. No C evaluation may
// be inserted between them: predicates, flags and numeric labels span blocks.
#define OBFH_CFLOW_BASIC_ASM(text) ({ OBFH_CFLOW_LOCAL_ASM(text); })
#define OBFH_CFLOW_BASIC_REGISTER(family) ((family) >= 4u && (family) < 8u ? 2u : (family) >= 8u && (family) < 12u ? 1u \
                                                                                                                   : 0u)
#define OBFH_CFLOW_BASIC_MASK(family) ((family) == 1u || (family) == 15u ? 2u : (family) == 6u ? 7u \
                                                                            : (family) == 7u   ? 3u \
                                                                            : (family) == 8u   ? 14u \
                                                                                               : 1u)
#define OBFH_CFLOW_BASIC_NONZERO(family) ((family) == 3u || (family) == 10u)
#define OBFH_CFLOW_BASIC_LOAD_REG(reg, style) ({ \
    __obfh_asm__(".short 0xe089+256*%c[native_reg];.fill (%c[load_style]==3)&1,3,0x07c0c1+256*%c[native_reg];.set .Lcl_short,(%c[native_reg]!=0)&(.Lcfs>0)&(.Lcfs<=127)&1;.set .Lcl_group,((%c[native_reg]!=0)|.Lcl_short)&1;.byte 5+8*%c[alu]+.Lcl_group*(124+2*.Lcl_short-8*%c[alu]);.fill .Lcl_group,1,0xc0+%c[native_reg]+8*%c[alu];.fill 1,4-3*.Lcl_short,.Lcfs;" \
                 : \
                 : [native_reg] "i"(reg), [load_style] "i"(style), [alu] "i"(6u * ((style) == 1u || (style) == 5u) + 5u * ((style) == 2u)) \
                 : OBFH_CFLOW_CLOBBERS); \
})
// EAX keeps imm32; ECX/EDX use imm8 for salts 1..127. ADD/SUB/XOR group fields are 0/5/6.
#define OBFH_CFLOW_BASIC_LOAD(family, style) OBFH_CFLOW_BASIC_LOAD_REG(OBFH_CFLOW_BASIC_REGISTER(family), style)
// TEST uses A8 for AL and F6 /0 for CL/DL, retaining the original native widths.
#define OBFH_CFLOW_BASIC_TEST_REG(reg, mask) ({ \
    __obfh_asm__(".byte 0xa8+78*((%c[native_reg]!=0)&1);.fill (%c[native_reg]!=0)&1,1,0xc0+%c[native_reg];.byte %c[test_mask];" \
                 : \
                 : [native_reg] "i"(reg), [test_mask] "i"(mask) \
                 : OBFH_CFLOW_CLOBBERS); \
})
#define OBFH_CFLOW_BASIC_TEST(family, mask) OBFH_CFLOW_BASIC_TEST_REG(OBFH_CFLOW_BASIC_REGISTER(family), mask)
#define OBFH_CFLOW_BASIC_BRANCH(nonzero, target) \
    OBFH_CFLOW_BASIC_PARAMETER_ASM(".byte 0x0f,%c[branch];.long " target "-.-4;", [branch] "i"((nonzero) ? 0x85u : 0x84u))
#if defined(__x86_64__)
#define OBFH_CFLOW_BASIC_ADDRESS ".byte 0x67;"
#else
#define OBFH_CFLOW_BASIC_ADDRESS ""
#endif
#define OBFH_CFLOW_BASIC_PARAMETER_ASM(text, ...) ({ __obfh_asm__(text \
                                                                  : \
                                                                  : __VA_ARGS__ \
                                                                  : OBFH_CFLOW_CLOBBERS); })
// Adjacent/previous products: LEA, IMUL, optional accumulator ADD imm32.
#define OBFH_CFLOW_BASIC_PRODUCT(f) OBFH_CFLOW_BASIC_PARAMETER_ASM(OBFH_CFLOW_BASIC_ADDRESS ".fill 1,3,0x01508d-2048*((%c[family]==12)&1)+0xfe0000*((%c[family]==12)&1);.fill 1,3,0xc2af0f-65536*((%c[family]==12)&1);.fill (%c[family]==3)&1,1,0x05;.fill (%c[family]==3)&1,4,1;", [family] "i"(f))
// EAX square; ECX fourth power; EAX odd square. OR EAX retains imm32.
#define OBFH_CFLOW_BASIC_POWER(f) OBFH_CFLOW_BASIC_PARAMETER_ASM(".fill 1,3,0xc0af0f+0x90000*((%c[family]==8)&1);.fill (%c[family]==8)&1,3,0xc9af0f;.fill (%c[family]==15)&1,1,0x0d;.fill (%c[family]==15)&1,4,1;", [family] "i"(f))
// Copy/square chains share operands while retaining their multiply count and final ALU.
#define OBFH_CFLOW_BASIC_CHAIN(f) OBFH_CFLOW_BASIC_PARAMETER_ASM(".short 0xc089+256*(8*%c[r]+%c[t]);.fill 1,3,0xc0af0f+0x90000*%c[r];.fill (%c[family]==9)&1,3,0xc9af0f;.fill %c[mul],3,0xc0af0f+65536*(8*%c[r]+%c[t]);.short %c[op]+256*(0xc0+8*%c[t]+%c[r]);", [family] "i"(f), [r] "i"((f) == 4 ? 2u : (f) == 9 ? 1u \
                                                                                                                                                                                                                                                                                                                   : 0u), \
                                                                 [t] "i"((f) == 2 ? 2u : (f) == 9 ? 0u \
                                                                                                  : 1u), \
                                                                 [mul] "i"((f) == 4 || (f) == 9 || (f) == 14), [op] "i"((f) == 2 ? 0x31u : (f) == 4 || (f) == 9 ? 0x29u \
                                                                                                                                                                : 0x01u))
// The adjacent OR/AND pair differs only in its final opcode.
#define OBFH_CFLOW_BASIC_BITS(f) OBFH_CFLOW_BASIC_PARAMETER_ASM(OBFH_CFLOW_BASIC_ADDRESS ".fill 1,3,0x01418d;.short 0xc109+24*((%c[family]==11)&1);", [family] "i"(f))
#define OBFH_CFLOW_BASIC_PREDICATE(f) ({ __builtin_choose_expr((f) == 0u || (f) == 3u || (f) == 12u, ({ OBFH_CFLOW_BASIC_PRODUCT(f); }), __builtin_choose_expr((f) == 1u || (f) == 8u || (f) == 15u, ({ OBFH_CFLOW_BASIC_POWER(f); }), __builtin_choose_expr((f) == 2u || (f) == 4u || (f) == 9u || (f) == 13u || (f) == 14u, ({ OBFH_CFLOW_BASIC_CHAIN(f); }), __builtin_choose_expr((f) == 10u || (f) == 11u, ({ OBFH_CFLOW_BASIC_BITS(f); }), __builtin_choose_expr((f) == 5u, OBFH_CFLOW_BASIC_ASM(OBFH_CFLOW_PRED_THREE_PRODUCT("%%edx", "%%ecx", "%%eax")), __builtin_choose_expr((f) == 6u, OBFH_CFLOW_BASIC_ASM(OBFH_CFLOW_PRED_FOUR_PRODUCT("%%edx", "%%ecx", "%%eax")), OBFH_CFLOW_BASIC_ASM(OBFH_CFLOW_PRED_SQUARE_PRODUCT("%%edx", "%%ecx")))))))); })
#define OBFH_CFLOW_BASIC_PAYLOAD(index) ({ \
    __builtin_choose_expr(((index)&3u) == 0u, OBFH_CFLOW_BASIC_ASM(".byte 0xe8+(.Lcfbasic&1);"), __builtin_choose_expr(((index)&3u) == 1u, OBFH_CFLOW_BASIC_ASM(".byte 0xff,0x15+16*((.Lcfbasic>>4)&1);.long .Lcfbasic;"), __builtin_choose_expr(((index)&3u) == 2u, OBFH_CFLOW_BASIC_ASM(".fill (.Lcfbasic>>8)&1,1,0x66;.byte 0xb8+((.Lcfbasic>>9)&7);.long .Lcfbasic;"), OBFH_CFLOW_BASIC_ASM(".byte 0xb8+((.Lcfbasic>>12)&7);.long .Lcfbasic;")))); \
    OBFH_CFLOW_BASIC_ASM(OBFH_CFLOW_FILL); \
})
#define OBFH_CFLOW_BASIC_EMIT(index) ({ \
    enum { __obfh_basic_family = (index)&15u, \
           __obfh_basic_style = (index) >> 4, \
           __obfh_basic_second = (__obfh_basic_family + 7u) & 15u, \
           __obfh_basic_mask = OBFH_CFLOW_BASIC_MASK(__obfh_basic_family), \
           __obfh_basic_mask_second = OBFH_CFLOW_BASIC_MASK(__obfh_basic_second) }; \
    OBFH_CFLOW_BASIC_LOAD(__obfh_basic_family, __obfh_basic_style); \
    OBFH_CFLOW_BASIC_PREDICATE(__obfh_basic_family); \
    OBFH_CFLOW_BASIC_TEST(__obfh_basic_family, __obfh_basic_mask); \
    __builtin_choose_expr(__obfh_basic_style & 1u, ({ \
                              OBFH_CFLOW_BASIC_BRANCH(!OBFH_CFLOW_BASIC_NONZERO(__obfh_basic_family), "1f"); \
                              OBFH_CFLOW_BASIC_ASM("jmp 2f;1:"); \
                          }), \
                          OBFH_CFLOW_BASIC_BRANCH(OBFH_CFLOW_BASIC_NONZERO(__obfh_basic_family), "1f")); \
    __builtin_choose_expr(__obfh_basic_style == 3u, OBFH_CFLOW_BASIC_ASM(".byte 0xc3;"), ((void)0)); \
    __builtin_choose_expr(__obfh_basic_style >= 4u, OBFH_CFLOW_BASIC_ASM(OBFH_CFLOW_PAYLOAD_CALL_DATA), OBFH_CFLOW_BASIC_PAYLOAD(index)); \
    __builtin_choose_expr(__obfh_basic_style & 1u, OBFH_CFLOW_BASIC_ASM("2:"), OBFH_CFLOW_BASIC_ASM("1:")); \
    __builtin_choose_expr(__obfh_basic_style >= 4u, ({ \
                              OBFH_CFLOW_BASIC_LOAD(__obfh_basic_second, 0u); \
                              OBFH_CFLOW_BASIC_PREDICATE(__obfh_basic_second); \
                              OBFH_CFLOW_BASIC_TEST(__obfh_basic_second, __obfh_basic_mask_second); \
                              __builtin_choose_expr(__obfh_basic_style == 4u, ({ \
                                                        OBFH_CFLOW_BASIC_BRANCH(OBFH_CFLOW_BASIC_NONZERO(__obfh_basic_second), "2f"); \
                                                        OBFH_CFLOW_BASIC_ASM(".byte 0xff,0x25;.long .Lcfbasic;.byte .Lcfbyte,0xe9;2:"); \
                                                    }), \
                                                    ({ \
                                                        OBFH_CFLOW_BASIC_BRANCH(OBFH_CFLOW_BASIC_NONZERO(__obfh_basic_second), "3f"); \
                                                        OBFH_CFLOW_BASIC_ASM(OBFH_CFLOW_PAYLOAD_TRAP_INDIRECT "3:"); \
                                                    })); \
                          }), \
                          ({ \
                              __builtin_choose_expr(__obfh_basic_style == 2u || __obfh_basic_style == 3u, ({ \
                                                        OBFH_CFLOW_BASIC_TEST(__obfh_basic_family, __obfh_basic_mask); \
                                                        __builtin_choose_expr(__obfh_basic_style == 2u, ({ \
                                                                                  OBFH_CFLOW_BASIC_BRANCH(OBFH_CFLOW_BASIC_NONZERO(__obfh_basic_family), "2f"); \
                                                                                  OBFH_CFLOW_BASIC_ASM(".byte 0x0f,0x0b,0xe8;.fill .Lcfpad,1,.Lcfbyte;2:"); \
                                                                              }), \
                                                                              ({ \
                                                                                  OBFH_CFLOW_BASIC_BRANCH(OBFH_CFLOW_BASIC_NONZERO(__obfh_basic_family), "3f"); \
                                                                                  OBFH_CFLOW_BASIC_ASM(".byte 0xff,0x25;.long .Lcfbasic;3:"); \
                                                                              })); \
                                                    }), \
                                                    ((void)0)); \
                          })); \
    (void)0; \
})
// Named forms share source fragments only: every predicate, branch and payload
// remains adjacent native ASM with no runtime C evaluation between fragments.
#define OBFH_CFLOW_NAMED_FRAGMENT(text) ({ OBFH_CFLOW_LOCAL_NAMED_ASM(text); })
#define OBFH_CFLOW_NAMED_SECOND(family) __builtin_choose_expr((family) <= 12u, __builtin_choose_expr((family) <= 3u, __builtin_choose_expr((family) <= 1u, __builtin_choose_expr((family) <= 0u, OBFH_CFLOW_NAMED_FRAGMENT(OBFH_CFLOW_SECOND_ADD_CARRY), OBFH_CFLOW_NAMED_FRAGMENT(OBFH_CFLOW_SECOND_OR_AND_SUM)), __builtin_choose_expr((family) <= 2u, OBFH_CFLOW_NAMED_FRAGMENT(OBFH_CFLOW_SECOND_SUB_BORROW), OBFH_CFLOW_NAMED_FRAGMENT(OBFH_CFLOW_SECOND_DEMORGAN))), __builtin_choose_expr((family) <= 5u, __builtin_choose_expr((family) <= 4u, OBFH_CFLOW_NAMED_FRAGMENT(OBFH_CFLOW_SECOND_OR_DISTRIBUTE), OBFH_CFLOW_NAMED_FRAGMENT(OBFH_CFLOW_SECOND_AND_PARTITION)), __builtin_choose_expr((family) <= 11u, OBFH_CFLOW_NAMED_FRAGMENT(OBFH_CFLOW_SECOND_ROTATE_XOR), OBFH_CFLOW_NAMED_FRAGMENT(OBFH_CFLOW_SECOND_ROTATE_RESTORE)))), __builtin_choose_expr((family) <= 19u, __builtin_choose_expr((family) <= 14u, __builtin_choose_expr((family) <= 13u, OBFH_CFLOW_NAMED_FRAGMENT(OBFH_CFLOW_SECOND_BSWAP_XOR), OBFH_CFLOW_NAMED_FRAGMENT(OBFH_CFLOW_SECOND_WORD_PARTITION)), __builtin_choose_expr((family) <= 18u, OBFH_CFLOW_NAMED_FRAGMENT(OBFH_CFLOW_SECOND_NEG_COMPLEMENT), OBFH_CFLOW_NAMED_FRAGMENT(OBFH_CFLOW_SECOND_MUL_DISTRIBUTE))), __builtin_choose_expr((family) <= 22u, __builtin_choose_expr((family) <= 21u, OBFH_CFLOW_NAMED_FRAGMENT(OBFH_CFLOW_SECOND_NEG_SQUARE), OBFH_CFLOW_NAMED_FRAGMENT(OBFH_CFLOW_SECOND_BSWAP_NOT)), __builtin_choose_expr((family) <= 23u, OBFH_CFLOW_NAMED_FRAGMENT(OBFH_CFLOW_SECOND_SUB_MASK), OBFH_CFLOW_NAMED_FRAGMENT(OBFH_CFLOW_SECOND_COMPLEMENT_TRANSLATE)))))
#define OBFH_CFLOW_NAMED_PREDICATE(family) ({ \
    OBFH_CFLOW_NAMED_FRAGMENT(OBFH_CFLOW_INPUT_AX); \
    __builtin_choose_expr((family) < 14u, __builtin_choose_expr((family) < 7u, __builtin_choose_expr((family) < 3u, __builtin_choose_expr((family) < 1u, OBFH_CFLOW_NAMED_FRAGMENT(OBFH_CFLOW_HEAD_ADD_CARRY), __builtin_choose_expr((family) < 2u, OBFH_CFLOW_NAMED_FRAGMENT(OBFH_CFLOW_HEAD_OR_AND_SUM), OBFH_CFLOW_NAMED_FRAGMENT(OBFH_CFLOW_HEAD_SUB_BORROW))), __builtin_choose_expr((family) < 5u, __builtin_choose_expr((family) < 4u, OBFH_CFLOW_NAMED_FRAGMENT(OBFH_CFLOW_HEAD_DEMORGAN), OBFH_CFLOW_NAMED_FRAGMENT(OBFH_CFLOW_HEAD_OR_DISTRIBUTE)), __builtin_choose_expr((family) < 6u, OBFH_CFLOW_NAMED_FRAGMENT(OBFH_CFLOW_HEAD_AND_PARTITION), OBFH_CFLOW_NAMED_FRAGMENT(OBFH_CFLOW_TAIL_XOR_CANCEL)))), __builtin_choose_expr((family) < 10u, __builtin_choose_expr((family) < 8u, OBFH_CFLOW_NAMED_FRAGMENT(OBFH_CFLOW_TAIL_COMPLEMENT_CARRY), __builtin_choose_expr((family) < 9u, OBFH_CFLOW_NAMED_FRAGMENT(OBFH_CFLOW_TAIL_COMPLEMENT_WRAP), OBFH_CFLOW_NAMED_FRAGMENT(OBFH_CFLOW_TAIL_MASK_SUBTRACT))), __builtin_choose_expr((family) < 12u, __builtin_choose_expr((family) < 11u, OBFH_CFLOW_NAMED_FRAGMENT(OBFH_CFLOW_TAIL_MASK_OR_ORDER), OBFH_CFLOW_NAMED_FRAGMENT(OBFH_CFLOW_HEAD_ROTATE_XOR)), __builtin_choose_expr((family) < 13u, OBFH_CFLOW_NAMED_FRAGMENT(OBFH_CFLOW_HEAD_ROTATE_RESTORE), OBFH_CFLOW_NAMED_FRAGMENT(OBFH_CFLOW_HEAD_BSWAP_XOR))))), __builtin_choose_expr((family) < 21u, __builtin_choose_expr((family) < 17u, __builtin_choose_expr((family) < 15u, OBFH_CFLOW_NAMED_FRAGMENT(OBFH_CFLOW_HEAD_WORD_PARTITION), __builtin_choose_expr((family) < 16u, OBFH_CFLOW_NAMED_FRAGMENT(OBFH_CFLOW_TAIL_BYTE_PARITY), OBFH_CFLOW_NAMED_FRAGMENT(OBFH_CFLOW_TAIL_BYTE_ROTATE))), __builtin_choose_expr((family) < 19u, __builtin_choose_expr((family) < 18u, OBFH_CFLOW_NAMED_FRAGMENT(OBFH_CFLOW_TAIL_WORD_ROTATE), OBFH_CFLOW_NAMED_FRAGMENT(OBFH_CFLOW_HEAD_NEG_COMPLEMENT)), __builtin_choose_expr((family) < 20u, OBFH_CFLOW_NAMED_FRAGMENT(OBFH_CFLOW_HEAD_MUL_DISTRIBUTE), OBFH_CFLOW_NAMED_FRAGMENT(OBFH_CFLOW_TAIL_SQUARE_EXPAND)))), __builtin_choose_expr((family) < 25u, __builtin_choose_expr((family) < 23u, __builtin_choose_expr((family) < 22u, OBFH_CFLOW_NAMED_FRAGMENT(OBFH_CFLOW_HEAD_NEG_SQUARE), OBFH_CFLOW_NAMED_FRAGMENT(OBFH_CFLOW_HEAD_BSWAP_NOT)), __builtin_choose_expr((family) < 24u, OBFH_CFLOW_NAMED_FRAGMENT(OBFH_CFLOW_HEAD_SUB_MASK), OBFH_CFLOW_NAMED_FRAGMENT(OBFH_CFLOW_TAIL_MASK_ORDER))), __builtin_choose_expr((family) < 27u, __builtin_choose_expr((family) < 26u, OBFH_CFLOW_NAMED_FRAGMENT(OBFH_CFLOW_HEAD_COMPLEMENT_TRANSLATE), OBFH_CFLOW_NAMED_FRAGMENT(OBFH_CFLOW_TAIL_SQUARE_RESIDUE)), __builtin_choose_expr((family) < 28u, OBFH_CFLOW_NAMED_FRAGMENT(OBFH_CFLOW_TAIL_FOURTH_RESIDUE), OBFH_CFLOW_NAMED_FRAGMENT(OBFH_CFLOW_TAIL_ISOLATE_BIT)))))); \
    __builtin_choose_expr(((family) <= 5u || ((family) >= 11u && (family) <= 14u) || ((family) >= 18u && (family) <= 19u) || ((family) >= 21u && (family) <= 23u) || (family) == 25u), ({ OBFH_CFLOW_NAMED_FRAGMENT(OBFH_CFLOW_INPUT_DX); OBFH_CFLOW_NAMED_SECOND(family); }), ((void)0)); \
})
// Native condition codes keep the original equality/carry/order tests.
#define OBFH_CFLOW_NAMED_CC(family) ((family) == 7u || (family) == 9u || (family) == 10u ? 0x83u : (family) == 8u ? 0x82u \
                                                                                               : (family) == 24u  ? 0x86u \
                                                                                                                  : 0x84u)
#define OBFH_CFLOW_NAMED_BRANCH(cc, inverse, target) ({ \
    __obfh_asm__(".byte 0x0f,%c[condition];.long " target "-.-4;" \
                 : \
                 : [condition] "i"((cc) ^ (inverse)) \
                 : OBFH_CFLOW_CLOBBERS); \
})
// The backward edges in the four layouts consume only ZF or clear CF.
// Let the assembler retain its normal short/near branch selection.
#define OBFH_CFLOW_NAMED_BACK_BRANCH(cc, target) \
    __builtin_choose_expr((cc) == 0x84u, OBFH_CFLOW_NAMED_FRAGMENT("jne " target ";"), OBFH_CFLOW_NAMED_FRAGMENT("jc " target ";"))
// Invert SETcc and its final consumer together. ECX is dead after the branch.
#define OBFH_CFLOW_NAMED_BOOLEAN(cc, flip, negative, target) ({ \
    __obfh_asm__(".byte 0x0f,%c[set],0xc1;movzbl %%cl,%%ecx;.fill %c[negate],2,0xd9f7;testl %%ecx,%%ecx;.byte 0x0f,%c[branch];.long " target "-.-4;" \
                 : \
                 : [set] "i"(((cc) ^ (flip)) + 0x10u), [negate] "i"(negative), [branch] "i"(((negative) ? 0x88u : 0x85u) ^ (flip)) \
                 : OBFH_CFLOW_CLOBBERS); \
})
#define OBFH_CFLOW_NAMED_PAYLOAD(kind) ({ \
    __builtin_choose_expr((kind) < 3u, __builtin_choose_expr((kind) < 1u, OBFH_CFLOW_NAMED_FRAGMENT(".byte (0xE8 + ((.Lcfk >> 15) & 1)); .long .Lcfk;"), __builtin_choose_expr((kind) < 2u, OBFH_CFLOW_NAMED_FRAGMENT(".byte 0xBC; .long .Lcfdata; .byte 0xFF, 0xE4;"), OBFH_CFLOW_NAMED_FRAGMENT(".byte 0xC8, .Lcfb0, .Lcfb1, .Lcfb2, 0xC9, 0xC3;"))), __builtin_choose_expr((kind) < 4u, OBFH_CFLOW_NAMED_FRAGMENT(".byte 0xFF, 0x15; .long .Lcfdata; .byte 0xE9; .long .Lcfk;"), __builtin_choose_expr((kind) < 5u, OBFH_CFLOW_NAMED_FRAGMENT(".byte 0x0F, 0x0B, 0x8F, 0x04, 0x24;"), OBFH_CFLOW_NAMED_FRAGMENT(".byte 0xC2, .Lcfb0, .Lcfb1, 0xE8; .long .Lcfdata;")))); \
    OBFH_CFLOW_NAMED_FRAGMENT(OBFH_CFLOW_DATA); \
})
#define OBFH_CFLOW_NAMED_EMIT(index) ({ \
    enum { __obfh_named_first = (index)-99u, \
           __obfh_named_second = (__obfh_named_first + 11u) % 29u, \
           __obfh_named_layout = __obfh_named_first & 3u, \
           __obfh_named_payload = __obfh_named_first % 6u, \
           __obfh_named_payload_second = (__obfh_named_payload + 3u) % 6u, \
           __obfh_named_cc_first = OBFH_CFLOW_NAMED_CC(__obfh_named_first), \
           __obfh_named_cc_second = OBFH_CFLOW_NAMED_CC(__obfh_named_second) }; \
    OBFH_CFLOW_NAMED_PREDICATE(__obfh_named_first); \
    __builtin_choose_expr(__obfh_named_layout == 2u, ({ \
                              OBFH_CFLOW_NAMED_BOOLEAN(__obfh_named_cc_first, (__obfh_cf_word >> 6) & 1u, 1, "52f"); \
                          }), \
                          __builtin_choose_expr(__obfh_named_layout == 1u, OBFH_CFLOW_NAMED_BRANCH(__obfh_named_cc_first, 1, "51f"), OBFH_CFLOW_NAMED_BRANCH(__obfh_named_cc_first, 0, "52f"))); \
    __builtin_choose_expr(__obfh_named_layout == 1u, OBFH_CFLOW_NAMED_FRAGMENT("jmp 52f;"), ((void)0)); \
    OBFH_CFLOW_NAMED_FRAGMENT("51:"); \
    OBFH_CFLOW_NAMED_PAYLOAD(__obfh_named_payload); \
    OBFH_CFLOW_NAMED_FRAGMENT("52:"); \
    OBFH_CFLOW_NAMED_PREDICATE(__obfh_named_second); \
    __builtin_choose_expr(__obfh_named_layout == 1u, ({ \
                              OBFH_CFLOW_NAMED_BOOLEAN(__obfh_named_cc_second, (__obfh_cf_word >> 7) & 1u, 0, "54f"); \
                          }), \
                          __builtin_choose_expr(__obfh_named_layout == 0u, OBFH_CFLOW_NAMED_BRANCH(__obfh_named_cc_second, 0, "54f"), \
                                                __builtin_choose_expr(__obfh_named_layout == 2u, OBFH_CFLOW_NAMED_BRANCH(__obfh_named_cc_second, 1, "53f"), OBFH_CFLOW_NAMED_BACK_BRANCH(__obfh_named_cc_second, "51b")))); \
    __builtin_choose_expr(__obfh_named_layout >= 2u, OBFH_CFLOW_NAMED_FRAGMENT("jmp 54f;"), ((void)0)); \
    OBFH_CFLOW_NAMED_FRAGMENT("53:"); \
    OBFH_CFLOW_NAMED_PAYLOAD(__obfh_named_payload_second); \
    OBFH_CFLOW_NAMED_FRAGMENT("54:"); \
    (void)0; \
})
// All 128 native forms retain their original selection indices.
#define OBFH_CFLOW_VARIANT_COUNT 128u
#define OBFH_CFLOW_OTHER(index) __builtin_choose_expr((index) < 92u, __builtin_choose_expr((index) < 89u, __builtin_choose_expr((index) < 87u, ({ OBFH_CFLOW_TEMPLATE_86; }), __builtin_choose_expr((index) < 88u, ({ OBFH_CFLOW_TEMPLATE_87; }), ({ OBFH_CFLOW_TEMPLATE_88; }))), __builtin_choose_expr((index) < 90u, ({ OBFH_CFLOW_TEMPLATE_89; }), __builtin_choose_expr((index) < 91u, ({ OBFH_CFLOW_TEMPLATE_90; }), ({ OBFH_CFLOW_TEMPLATE_91; })))), __builtin_choose_expr((index) < 95u, __builtin_choose_expr((index) < 93u, ({ OBFH_CFLOW_TEMPLATE_92; }), __builtin_choose_expr((index) < 94u, ({ OBFH_CFLOW_TEMPLATE_93; }), ({ OBFH_CFLOW_TEMPLATE_94; }))), OBFH_CFLOW_STACK_EMIT(index)))
#define OBFH_CFLOW_SELECT(index) ({ OBFH_CFLOW_CAPTURE; __builtin_choose_expr((index)<86u, OBFH_CFLOW_BASIC_EMIT(index), __builtin_choose_expr((index)<99u, OBFH_CFLOW_OTHER(index), OBFH_CFLOW_NAMED_EMIT(index))); (void)0; })
// All 128 families use nonserializing arithmetic; keep selection uniform.
#define OBFH_CFLOW_SLOT(value) (((unsigned int)(value) ^ ((unsigned int)(value) >> 16)) & 127u)
#define OBFH_CFLOW_INDEX(site) OBFH_CFLOW_SLOT(OBFH_MIX_B(OBFH_MIX_A((unsigned int)(site) ^ (unsigned int)OBFH_SEED ^ 2654435769u)))
#define OBFH_CFLOW_SINGLE(index) ((void)0)
#define OBFH_CFLOW_EXTRA_OFFSET(index, seed) (1u | ((OBFH_MIX_B((unsigned int)(seed) ^ 0xC6EF3720u) >> ((unsigned int)(index)&15u)) & 126u))
#define OBFH_CFLOW_EXTRA_INDEX(index) (((index) + OBFH_CFLOW_EXTRA_OFFSET(index, OBFH_SEED)) & 127u)
#if OBFH_BETTER_CONTROL_FLOW
// Capture once: the 128-way selector must not duplicate the mapping expression.
#define OBFH_CFLOW_EXTRA(index) ({ \
    enum { __obfh_extra_index = OBFH_CFLOW_EXTRA_INDEX(index) }; \
    OBFH_CFLOW_SELECT(__obfh_extra_index); \
    (void)0; \
})
#else
#define OBFH_CFLOW_EXTRA(index) OBFH_CFLOW_SINGLE(index)
#endif
#define OBFH_CFLOW_EMIT(site, extra) ({ \
    enum { __obfh_break_id = (site), \
           __obfh_break_hash1 = OBFH_MIX_A((unsigned int)__obfh_break_id ^ (unsigned int)OBFH_SEED ^ 2654435769u), \
           __obfh_break_hash2 = OBFH_MIX_B(__obfh_break_hash1), \
           __obfh_break_slot = OBFH_CFLOW_SLOT(__obfh_break_hash2), \
           __obfh_break_index = __obfh_break_slot }; \
    OBFH_CFLOW_SELECT(__obfh_break_index); \
    extra(__obfh_break_index); \
    (void)0; \
})
// Keep the string compound literal in caller scope and emit exactly one template.
#define OBFH_HIDE_JUNK OBFH_CFLOW_EMIT(__COUNTER__, OBFH_CFLOW_SINGLE)

// Template pool; only the selected ASM branch is emitted.
#define OBFH_CFLOW_TEMPLATE_0 OBFH_CFLOW_BASIC_EMIT(0u)
#define OBFH_CFLOW_TEMPLATE_1 OBFH_CFLOW_BASIC_EMIT(1u)
#define OBFH_CFLOW_TEMPLATE_2 OBFH_CFLOW_BASIC_EMIT(2u)
#define OBFH_CFLOW_TEMPLATE_3 OBFH_CFLOW_BASIC_EMIT(3u)
#define OBFH_CFLOW_TEMPLATE_4 OBFH_CFLOW_BASIC_EMIT(4u)
#define OBFH_CFLOW_TEMPLATE_5 OBFH_CFLOW_BASIC_EMIT(5u)
#define OBFH_CFLOW_TEMPLATE_6 OBFH_CFLOW_BASIC_EMIT(6u)
#define OBFH_CFLOW_TEMPLATE_7 OBFH_CFLOW_BASIC_EMIT(7u)
#define OBFH_CFLOW_TEMPLATE_8 OBFH_CFLOW_BASIC_EMIT(8u)
#define OBFH_CFLOW_TEMPLATE_9 OBFH_CFLOW_BASIC_EMIT(9u)
#define OBFH_CFLOW_TEMPLATE_10 OBFH_CFLOW_BASIC_EMIT(10u)
#define OBFH_CFLOW_TEMPLATE_11 OBFH_CFLOW_BASIC_EMIT(11u)
#define OBFH_CFLOW_TEMPLATE_12 OBFH_CFLOW_BASIC_EMIT(12u)
#define OBFH_CFLOW_TEMPLATE_13 OBFH_CFLOW_BASIC_EMIT(13u)
#define OBFH_CFLOW_TEMPLATE_14 OBFH_CFLOW_BASIC_EMIT(14u)
#define OBFH_CFLOW_TEMPLATE_15 OBFH_CFLOW_BASIC_EMIT(15u)
#define OBFH_CFLOW_TEMPLATE_16 OBFH_CFLOW_BASIC_EMIT(16u)
#define OBFH_CFLOW_TEMPLATE_17 OBFH_CFLOW_BASIC_EMIT(17u)
#define OBFH_CFLOW_TEMPLATE_18 OBFH_CFLOW_BASIC_EMIT(18u)
#define OBFH_CFLOW_TEMPLATE_19 OBFH_CFLOW_BASIC_EMIT(19u)
#define OBFH_CFLOW_TEMPLATE_20 OBFH_CFLOW_BASIC_EMIT(20u)
#define OBFH_CFLOW_TEMPLATE_21 OBFH_CFLOW_BASIC_EMIT(21u)
#define OBFH_CFLOW_TEMPLATE_22 OBFH_CFLOW_BASIC_EMIT(22u)
#define OBFH_CFLOW_TEMPLATE_23 OBFH_CFLOW_BASIC_EMIT(23u)
#define OBFH_CFLOW_TEMPLATE_24 OBFH_CFLOW_BASIC_EMIT(24u)
#define OBFH_CFLOW_TEMPLATE_25 OBFH_CFLOW_BASIC_EMIT(25u)
#define OBFH_CFLOW_TEMPLATE_26 OBFH_CFLOW_BASIC_EMIT(26u)
#define OBFH_CFLOW_TEMPLATE_27 OBFH_CFLOW_BASIC_EMIT(27u)
#define OBFH_CFLOW_TEMPLATE_28 OBFH_CFLOW_BASIC_EMIT(28u)
#define OBFH_CFLOW_TEMPLATE_29 OBFH_CFLOW_BASIC_EMIT(29u)
#define OBFH_CFLOW_TEMPLATE_30 OBFH_CFLOW_BASIC_EMIT(30u)
#define OBFH_CFLOW_TEMPLATE_31 OBFH_CFLOW_BASIC_EMIT(31u)
#define OBFH_CFLOW_TEMPLATE_32 OBFH_CFLOW_BASIC_EMIT(32u)
#define OBFH_CFLOW_TEMPLATE_33 OBFH_CFLOW_BASIC_EMIT(33u)
#define OBFH_CFLOW_TEMPLATE_34 OBFH_CFLOW_BASIC_EMIT(34u)
#define OBFH_CFLOW_TEMPLATE_35 OBFH_CFLOW_BASIC_EMIT(35u)
#define OBFH_CFLOW_TEMPLATE_36 OBFH_CFLOW_BASIC_EMIT(36u)
#define OBFH_CFLOW_TEMPLATE_37 OBFH_CFLOW_BASIC_EMIT(37u)
#define OBFH_CFLOW_TEMPLATE_38 OBFH_CFLOW_BASIC_EMIT(38u)
#define OBFH_CFLOW_TEMPLATE_39 OBFH_CFLOW_BASIC_EMIT(39u)
#define OBFH_CFLOW_TEMPLATE_40 OBFH_CFLOW_BASIC_EMIT(40u)
#define OBFH_CFLOW_TEMPLATE_41 OBFH_CFLOW_BASIC_EMIT(41u)
#define OBFH_CFLOW_TEMPLATE_42 OBFH_CFLOW_BASIC_EMIT(42u)
#define OBFH_CFLOW_TEMPLATE_43 OBFH_CFLOW_BASIC_EMIT(43u)
#define OBFH_CFLOW_TEMPLATE_44 OBFH_CFLOW_BASIC_EMIT(44u)
#define OBFH_CFLOW_TEMPLATE_45 OBFH_CFLOW_BASIC_EMIT(45u)
#define OBFH_CFLOW_TEMPLATE_46 OBFH_CFLOW_BASIC_EMIT(46u)
#define OBFH_CFLOW_TEMPLATE_47 OBFH_CFLOW_BASIC_EMIT(47u)
#define OBFH_CFLOW_TEMPLATE_48 OBFH_CFLOW_BASIC_EMIT(48u)
#define OBFH_CFLOW_TEMPLATE_49 OBFH_CFLOW_BASIC_EMIT(49u)
#define OBFH_CFLOW_TEMPLATE_50 OBFH_CFLOW_BASIC_EMIT(50u)
#define OBFH_CFLOW_TEMPLATE_51 OBFH_CFLOW_BASIC_EMIT(51u)
#define OBFH_CFLOW_TEMPLATE_52 OBFH_CFLOW_BASIC_EMIT(52u)
#define OBFH_CFLOW_TEMPLATE_53 OBFH_CFLOW_BASIC_EMIT(53u)
#define OBFH_CFLOW_TEMPLATE_54 OBFH_CFLOW_BASIC_EMIT(54u)
#define OBFH_CFLOW_TEMPLATE_55 OBFH_CFLOW_BASIC_EMIT(55u)
#define OBFH_CFLOW_TEMPLATE_56 OBFH_CFLOW_BASIC_EMIT(56u)
#define OBFH_CFLOW_TEMPLATE_57 OBFH_CFLOW_BASIC_EMIT(57u)
#define OBFH_CFLOW_TEMPLATE_58 OBFH_CFLOW_BASIC_EMIT(58u)
#define OBFH_CFLOW_TEMPLATE_59 OBFH_CFLOW_BASIC_EMIT(59u)
#define OBFH_CFLOW_TEMPLATE_60 OBFH_CFLOW_BASIC_EMIT(60u)
#define OBFH_CFLOW_TEMPLATE_61 OBFH_CFLOW_BASIC_EMIT(61u)
#define OBFH_CFLOW_TEMPLATE_62 OBFH_CFLOW_BASIC_EMIT(62u)
#define OBFH_CFLOW_TEMPLATE_63 OBFH_CFLOW_BASIC_EMIT(63u)
#define OBFH_CFLOW_TEMPLATE_64 OBFH_CFLOW_BASIC_EMIT(64u)
#define OBFH_CFLOW_TEMPLATE_65 OBFH_CFLOW_BASIC_EMIT(65u)
#define OBFH_CFLOW_TEMPLATE_66 OBFH_CFLOW_BASIC_EMIT(66u)
#define OBFH_CFLOW_TEMPLATE_67 OBFH_CFLOW_BASIC_EMIT(67u)
#define OBFH_CFLOW_TEMPLATE_68 OBFH_CFLOW_BASIC_EMIT(68u)
#define OBFH_CFLOW_TEMPLATE_69 OBFH_CFLOW_BASIC_EMIT(69u)
#define OBFH_CFLOW_TEMPLATE_70 OBFH_CFLOW_BASIC_EMIT(70u)
#define OBFH_CFLOW_TEMPLATE_71 OBFH_CFLOW_BASIC_EMIT(71u)
#define OBFH_CFLOW_TEMPLATE_72 OBFH_CFLOW_BASIC_EMIT(72u)
#define OBFH_CFLOW_TEMPLATE_73 OBFH_CFLOW_BASIC_EMIT(73u)
#define OBFH_CFLOW_TEMPLATE_74 OBFH_CFLOW_BASIC_EMIT(74u)
#define OBFH_CFLOW_TEMPLATE_75 OBFH_CFLOW_BASIC_EMIT(75u)
#define OBFH_CFLOW_TEMPLATE_76 OBFH_CFLOW_BASIC_EMIT(76u)
#define OBFH_CFLOW_TEMPLATE_77 OBFH_CFLOW_BASIC_EMIT(77u)
#define OBFH_CFLOW_TEMPLATE_78 OBFH_CFLOW_BASIC_EMIT(78u)
#define OBFH_CFLOW_TEMPLATE_79 OBFH_CFLOW_BASIC_EMIT(79u)
#define OBFH_CFLOW_TEMPLATE_80 OBFH_CFLOW_BASIC_EMIT(80u)
#define OBFH_CFLOW_TEMPLATE_81 OBFH_CFLOW_BASIC_EMIT(81u)
#define OBFH_CFLOW_TEMPLATE_82 OBFH_CFLOW_BASIC_EMIT(82u)
#define OBFH_CFLOW_TEMPLATE_83 OBFH_CFLOW_BASIC_EMIT(83u)
#define OBFH_CFLOW_TEMPLATE_84 OBFH_CFLOW_BASIC_EMIT(84u)
#define OBFH_CFLOW_TEMPLATE_85 OBFH_CFLOW_BASIC_EMIT(85u)

#define OBFH_CFLOW_TEMPLATE_86 \
    OBFH_CFLOW_LOCAL_JUNK_ASM("xorl %%eax, %%eax; jz 1f; .byte 0xE8; " OBFH_CFLOW_NATIVE_PAYLOAD "1: movl $%c[tail_key], %%eax; roll $%c[tail_pad], %%eax; xorl %%eax, %%edx;")

#define OBFH_CFLOW_TEMPLATE_87 ({ \
    enum { __obfh_break_salt = RND(1, 32767), \
           __obfh_break_gap = RND(1, 255) }; \
    __obfh_asm__("movzbl %[byte_zero], %%eax; xorl %[salt], %%eax; cmpl %[limit], %%eax; jne 1f; .byte 0x00, 0xE8; " OBFH_CFLOW_NATIVE_PAYLOAD "1:" \
                 : \
                 : [byte_zero] "m"(_0), [salt] "i"(__obfh_break_salt), [limit] "i"(__obfh_break_salt + __obfh_break_gap) \
                 : OBFH_JUNK_CLOBBERS); \
})

#define OBFH_CFLOW_TEMPLATE_88 ({ \
    enum { __obfh_break_salt = RND(1, 32767), \
           __obfh_break_case1 = RND(129, 191), \
           __obfh_break_case2 = RND(257, 319) }; \
    __obfh_asm__("movzbl %[byte_zero], %%eax; addl %[salt], %%eax; cmpl %[limit], %%eax; jne 1f; .byte 0x00, 0x00; " OBFH_CFLOW_NATIVE_PAYLOAD \
                 "1: cmpl %[limit2], %%eax; jne 2f; .byte 0xFF, 0x25; " OBFH_CFLOW_NATIVE_PAYLOAD "2:" \
                 : \
                 : [byte_zero] "m"(_0), [salt] "i"(__obfh_break_salt), [limit] "i"(__obfh_break_salt + __obfh_break_case1), [limit2] "i"(__obfh_break_salt + __obfh_break_case2) \
                 : OBFH_JUNK_CLOBBERS); \
})

#define OBFH_CFLOW_TEMPLATE_89 \
    OBFH_CFLOW_LOCAL_JUNK_ASM("xorl %%ebx, %%ebx; xorl %%edx, %%edx; xorl %%ebx, %%edx; jz 1f; mov $4, %%eax; .byte 0x00; " OBFH_CFLOW_NATIVE_PAYLOAD "1: leal %c[tail_key](%%ebx), %%ecx; rorl $%c[tail_pad], %%ecx;")

#define OBFH_CFLOW_TEMPLATE_90 \
    OBFH_CFLOW_LOCAL_JUNK_ASM("xorl %%ebx, %%ebx; xorl %%eax, %%eax; mov %%eax, %%ebx; mov %%edx, %%ebx; xorl %%ebx, %%edx; jz 1f; .byte 0x20; " OBFH_CFLOW_NATIVE_PAYLOAD "1: movl $%c[tail_key], %%edx; bswap %%edx; xorl %%edx, %%eax;")

#define OBFH_CFLOW_TEMPLATE_91 \
    OBFH_CFLOW_LOCAL_JUNK_ASM("xorl %%edx, %%edx; xorl %%eax, %%eax; mov %%eax, %%edx; jz 1f; .byte 0xE8; " OBFH_CFLOW_NATIVE_PAYLOAD "1: movl $%c[tail_key], %%ecx; negl %%ecx; roll $%c[tail_pad], %%ecx;")

#define OBFH_CFLOW_TEMPLATE_92 \
    OBFH_CFLOW_LOCAL_JUNK_ASM("xorl %%edx, %%edx; jz 1f; .byte 0xE8; " OBFH_CFLOW_NATIVE_PAYLOAD "1: movl $%c[tail_key], %%eax; notl %%eax; leal (%%eax,%%eax,2), %%edx;")

#define OBFH_CFLOW_TEMPLATE_93 \
    OBFH_CFLOW_LOCAL_JUNK_ASM("xorl %%eax, %%eax; jz 1f; .byte 0x50; " OBFH_CFLOW_NATIVE_PAYLOAD "1: movl $%c[tail_key], %%ecx; rorl $%c[tail_pad], %%ecx; xorl %%ecx, %%eax;")

#define OBFH_CFLOW_TEMPLATE_94 \
    OBFH_CFLOW_LOCAL_JUNK_ASM("xorl %%edx, %%edx; jz 1f; .byte 0x00, 0x00; " OBFH_CFLOW_NATIVE_PAYLOAD "1: movl $%c[tail_key], %%edx; roll $%c[tail_pad], %%edx; leal (%%edx,%%edx,4), %%eax;")

#define OBFH_CFLOW_STACK_EMIT(index) ({ __builtin_choose_expr((index)<=96u, __builtin_choose_expr((index)<=95u, ({ OBFH_CFLOW_LOCAL_STACK_ASM("movl %%esp, %%eax; addl %0, %%eax; leal 1(%%eax), %%edx; imull %%edx, %%eax; testb $1, %%al; jz 1f; .byte 0xE8; ", "eax", "edx"); }), ({ OBFH_CFLOW_LOCAL_STACK_ASM("movl %%esp, %%edx; addl %0, %%edx; imull %%edx, %%edx; testb $2, %%dl; jz 1f; .byte 0xFF, 0x25; ", "edx"); })), __builtin_choose_expr((index)<=97u, ({ OBFH_CFLOW_LOCAL_STACK_ASM("movl %%esp, %%eax; addl %0, %%eax; movl %%eax, %%edx; imull %%eax, %%eax; xorl %%edx, %%eax; testb $1, %%al; jz 1f; .byte 0x0F, 0x0B, 0xE8; ", "eax", "edx"); }), ({ OBFH_CFLOW_LOCAL_STACK_ASM("movl %%esp, %%eax; addl %0, %%eax; leal 1(%%eax), %%edx; imull %%edx, %%eax; addl $1, %%eax; testb $1, %%al; jnz 1f; .byte 0xC3, 0xE8; ", "eax", "edx"); }))); OBFH_CFLOW_LOCAL_ASM(OBFH_CFLOW_NATIVE_PAYLOAD "1:"); })
#define OBFH_CFLOW_TEMPLATE_95 \
    OBFH_CFLOW_LOCAL_STACK_ASM("movl %%esp, %%eax; addl %0, %%eax; leal 1(%%eax), %%edx; imull %%edx, %%eax; testb $1, %%al; jz 1f; .byte 0xE8; " OBFH_CFLOW_NATIVE_PAYLOAD "1:", "eax", "edx")

#define OBFH_CFLOW_TEMPLATE_96 \
    OBFH_CFLOW_LOCAL_STACK_ASM("movl %%esp, %%edx; addl %0, %%edx; imull %%edx, %%edx; testb $2, %%dl; jz 1f; .byte 0xFF, 0x25; " OBFH_CFLOW_NATIVE_PAYLOAD "1:", "edx")

#define OBFH_CFLOW_TEMPLATE_97 \
    OBFH_CFLOW_LOCAL_STACK_ASM("movl %%esp, %%eax; addl %0, %%eax; movl %%eax, %%edx; imull %%eax, %%eax; xorl %%edx, %%eax; testb $1, %%al; jz 1f; .byte 0x0F, 0x0B, 0xE8; " OBFH_CFLOW_NATIVE_PAYLOAD "1:", "eax", "edx")

#define OBFH_CFLOW_TEMPLATE_98 \
    OBFH_CFLOW_LOCAL_STACK_ASM("movl %%esp, %%eax; addl %0, %%eax; leal 1(%%eax), %%edx; imull %%edx, %%eax; addl $1, %%eax; testb $1, %%al; jnz 1f; .byte 0xC3, 0xE8; " OBFH_CFLOW_NATIVE_PAYLOAD "1:", "eax", "edx")

#define OBFH_CFLOW_TEMPLATE_99 OBFH_CFLOW_NAMED_EMIT(99u)
#define OBFH_CFLOW_TEMPLATE_100 OBFH_CFLOW_NAMED_EMIT(100u)
#define OBFH_CFLOW_TEMPLATE_101 OBFH_CFLOW_NAMED_EMIT(101u)
#define OBFH_CFLOW_TEMPLATE_102 OBFH_CFLOW_NAMED_EMIT(102u)
#define OBFH_CFLOW_TEMPLATE_103 OBFH_CFLOW_NAMED_EMIT(103u)
#define OBFH_CFLOW_TEMPLATE_104 OBFH_CFLOW_NAMED_EMIT(104u)
#define OBFH_CFLOW_TEMPLATE_105 OBFH_CFLOW_NAMED_EMIT(105u)
#define OBFH_CFLOW_TEMPLATE_106 OBFH_CFLOW_NAMED_EMIT(106u)
#define OBFH_CFLOW_TEMPLATE_107 OBFH_CFLOW_NAMED_EMIT(107u)
#define OBFH_CFLOW_TEMPLATE_108 OBFH_CFLOW_NAMED_EMIT(108u)
#define OBFH_CFLOW_TEMPLATE_109 OBFH_CFLOW_NAMED_EMIT(109u)
#define OBFH_CFLOW_TEMPLATE_110 OBFH_CFLOW_NAMED_EMIT(110u)
#define OBFH_CFLOW_TEMPLATE_111 OBFH_CFLOW_NAMED_EMIT(111u)
#define OBFH_CFLOW_TEMPLATE_112 OBFH_CFLOW_NAMED_EMIT(112u)
#define OBFH_CFLOW_TEMPLATE_113 OBFH_CFLOW_NAMED_EMIT(113u)
#define OBFH_CFLOW_TEMPLATE_114 OBFH_CFLOW_NAMED_EMIT(114u)
#define OBFH_CFLOW_TEMPLATE_115 OBFH_CFLOW_NAMED_EMIT(115u)
#define OBFH_CFLOW_TEMPLATE_116 OBFH_CFLOW_NAMED_EMIT(116u)
#define OBFH_CFLOW_TEMPLATE_117 OBFH_CFLOW_NAMED_EMIT(117u)
#define OBFH_CFLOW_TEMPLATE_118 OBFH_CFLOW_NAMED_EMIT(118u)
#define OBFH_CFLOW_TEMPLATE_119 OBFH_CFLOW_NAMED_EMIT(119u)
#define OBFH_CFLOW_TEMPLATE_120 OBFH_CFLOW_NAMED_EMIT(120u)
#define OBFH_CFLOW_TEMPLATE_121 OBFH_CFLOW_NAMED_EMIT(121u)
#define OBFH_CFLOW_TEMPLATE_122 OBFH_CFLOW_NAMED_EMIT(122u)
#define OBFH_CFLOW_TEMPLATE_123 OBFH_CFLOW_NAMED_EMIT(123u)
#define OBFH_CFLOW_TEMPLATE_124 OBFH_CFLOW_NAMED_EMIT(124u)
#define OBFH_CFLOW_TEMPLATE_125 OBFH_CFLOW_NAMED_EMIT(125u)
#define OBFH_CFLOW_TEMPLATE_126 OBFH_CFLOW_NAMED_EMIT(126u)
#define OBFH_CFLOW_TEMPLATE_127 OBFH_CFLOW_NAMED_EMIT(127u)

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

// #endregion

// #region 07. STACK_PROXY_FUNCTIONS: guards, bodies, layouts and selection
// ============================================================================
// 07. STACK_PROXY_FUNCTIONS: guards, bodies, layouts and selection
// ============================================================================

/*
 * A proxy insertion has four steps:
 * 1. CAPTURE draws the local constants once (including unused draws).
 * 2. SPEC describes one of 128 guard/layout/body combinations.
 * 3. SELECT captures that recipe as a compile-time value.
 * 4. The guard, layout and bodies are selected independently, avoiding expansion
 *    of the entire body combination for each possible recipe.
 *
 * Edit these source definitions directly; there is no generated cache.
 * Do not remove captured draws: their order affects later random choices.
 */
// Self-contained fake functions: live guards skip all calls and stack edits.
// Frames and calling conventions follow Windows TCC x86/x64. Bodies stay at
// their original physical positions; the selectors add no runtime transfers.
// Skeleton labels include the layout number, so discarded choices cannot
// capture another layout's branches. Repeated numeric labels support nested macros.
#define OBFH_SF_CAPTURE \
    enum { \
        __obfh_sf_mask = OBFH_JUNK_WORD, \
        __obfh_sf_alu_a = RND(0, 4), \
        __obfh_sf_alu_b = RND(0, 6), \
        __obfh_sf_shift = RND(0, 4), \
        OBFH_MIX_FIELDS(__obfh_sf_phantom, (unsigned int)__LINE__ ^ (unsigned int)OBFH_SEED ^ (unsigned int)__LINE__ * 2654435761u), \
        __obfh_sf_salt = RND(1, 32767), \
        __obfh_sf_rotate = RND(1, 31), \
        __obfh_sf_frame_a = RND(2, 12) * 16u, \
        __obfh_sf_frame_b = RND(2, 12) * 16u, \
        __obfh_sf_frame_c = RND(2, 12) * 16u, \
        __obfh_sf_arg_a = RND(1, 65535), \
        __obfh_sf_arg_b = RND(1, 65535), \
        __obfh_sf_key_a = OBFH_JUNK_WORD, \
        __obfh_sf_key_b = OBFH_JUNK_WORD, \
        __obfh_sf_factor = RND(1, 32767) * 2u + 1u, \
        __obfh_sf_pad = RND(0, 7), \
        __obfh_sf_loops = RND(1, 7), \
        __obfh_sf_not_mask = ~(unsigned int)__obfh_sf_mask, \
        __obfh_sf_frame_d = RND(2, 12) * 16u, \
        __obfh_sf_pad_b = RND(0, 15), \
        __obfh_sf_noise_a = OBFH_JUNK_BYTE, \
        __obfh_sf_noise_b = OBFH_JUNK_BYTE, \
        __obfh_sf_local_a = RND(1, 3) * 4u, \
        __obfh_sf_local_b = RND(4, 7) * 4u, \
        __obfh_sf_leaf_frame = RND(2, 6) * 16u + 8u, \
        __obfh_sf_leaf_slot = RND(0, 6) * 4u, \
        __obfh_sf_leaf_alu_a = __obfh_sf_alu_a == 0 ? 0x05 : __obfh_sf_alu_a == 1 ? 0x0d \
                                                         : __obfh_sf_alu_a == 2   ? 0x25 \
                                                         : __obfh_sf_alu_a == 3   ? 0x2d \
                                                                                  : 0x35, \
        __obfh_sf_leaf_alu_b = 0xc2 + __obfh_sf_alu_b * 8, \
        __obfh_sf_leaf_shift = __obfh_sf_shift == 0 ? 0xc0 : __obfh_sf_shift == 1 ? 0xc8 \
                                                         : __obfh_sf_shift == 2   ? 0xe0 \
                                                         : __obfh_sf_shift == 3   ? 0xe8 \
                                                                                  : 0xf8 \
    }; \
    OBFH_INPUT_CORE(__si, __obfh_sf_salt, __obfh_sf_rotate, __obfh_sf_phantom); \
    OBFH_INPUT_EAX(__si, __obfh_sf_salt); \
    enum { __s0 = __si_0a, \
           __s1 = __si_0b }
#define OBFH_SF_NATIVE_INPUTS \
    [phantom] "i"(__obfh_sf_phantom), \
        [rotate] "i"(__obfh_sf_rotate), \
        [frame_a] "i"(__obfh_sf_frame_a), \
        [frame_b] "i"(__obfh_sf_frame_b), \
        [frame_c] "i"(__obfh_sf_frame_c), \
        [arg_a] "i"(__obfh_sf_arg_a), \
        [arg_b] "i"(__obfh_sf_arg_b), \
        [key_a] "i"(__obfh_sf_key_a), \
        [key_b] "i"(__obfh_sf_key_b), \
        [factor] "i"(__obfh_sf_factor), \
        [pad] "i"(__obfh_sf_pad), \
        [loops] "i"(__obfh_sf_loops), \
        [mask] "i"(__obfh_sf_mask), \
        [not_mask] "i"(__obfh_sf_not_mask), \
        [frame_d] "i"(__obfh_sf_frame_d), \
        [pad_b] "i"(__obfh_sf_pad_b), \
        [noise_a] "i"(__obfh_sf_noise_a), \
        [noise_b] "i"(__obfh_sf_noise_b), \
        [local_a] "i"(__obfh_sf_local_a), \
        [local_b] "i"(__obfh_sf_local_b), \
        [leaf_frame] "i"(__obfh_sf_leaf_frame), \
        [leaf_slot] "i"(__obfh_sf_leaf_slot), \
        [leaf_alu_a] "i"(__obfh_sf_leaf_alu_a), \
        [leaf_alu_b] "i"(__obfh_sf_leaf_alu_b), \
        [leaf_shift] "i"(__obfh_sf_leaf_shift)
// Input spelling precedes the selected guard; immediate-only ASM fragments must stay adjacent.
#define OBFH_SF_GUARD_INPUTS OBFH_SF_OPERAND(phantom), OBFH_SF_OPERAND(rotate), OBFH_SF_OPERAND(key_a), OBFH_SF_OPERAND(factor), OBFH_SF_OPERAND(mask), OBFH_SF_OPERAND(not_mask)
#define OBFH_SF_INPUTS \
    [il] "i"(__si_n), [i0] "i"(__s0), [i1] "i"(__s1), OBFH_SF_NATIVE_INPUTS
#define OBFH_SF_START_PREFIX __obfh_asm__(OBFH_SF_INPUT ".long %c[i0];.fill 1,%c[il]-4,(((%c[i1])&0xffffffff)^0x80000000)-0x80000000;" \
                                          : \
                                          : [il] "i"(__si_n), [i0] "i"(__s0), [i1] "i"(__s1) \
                                          : "eax", "edx", "ecx", "cc", "memory")
#define OBFH_SF_FLOW_START_PREFIX __obfh_asm__(".long %c[i0];.fill 1,%c[il]-4,(((%c[i1])&0xffffffff)^0x80000000)-0x80000000;" \
                                               : \
                                               : "a"(__obfh_live_actual), [il] "i"(__si_n), [i0] "i"(__s0), [i1] "i"(__s1) \
                                               : "edx", "ecx", "cc", "memory")
// All choices below assemble only inside skipped native functions. No new
// RND draws or live-path instructions; existing captured entropy selects forms.
#define OBFH_SF_BYTES(bit, flip, size, value) \
    ".fill (((.Lsp >> " bit ") & 1) ^ " flip ")," size "," value ";"
#define OBFH_SF_BIT_BYTES(bit, flip, size, value) \
    ".fill (.Lsp" bit " ^ " flip ")," size "," value ";"
#define OBFH_SF_WORD(bit, flip, value) OBFH_SF_BYTES(bit, flip, "4", value)
// Optional edges exist only in skipped bodies. x64 call sequence is 23 bytes;
// x86 uses explicit imm32 pushes so its sequence is always 18 bytes.
#define OBFH_SF_LINK_BYTES(channel, size, value) \
    ".fill .Lsle" channel "," size "," value ";"
// Capture TEST and Jcc opcode bytes; the gate adds its original rel8 displacement.
#define OBFH_SF_LINK_CONSTANT(channel) ".set .Lslhead" channel ",(0xc085^(.Lsln" channel "<<12))|((0x74|.Lslz" channel ")<<16);"
#define OBFH_SF_LINK_GATE(span, channel) ".fill .Lsle" channel ",4,.Lslhead" channel "+((" span ")<<24);"
// Frame sizes are 32..192 bytes. Bit 7 selects imm8/disp8 or imm32/disp32;
// SUB/LEA selection, frame sizes, native boundaries and links stay unchanged.
// Capture each frame encoding once per pool; emission only reads its fields.
// hw/aw/dw are head/allocation/displacement widths; h/a/d hold their native values.
#if defined(__x86_64__)
#define OBFH_SF_INPUT "movl %%esp, %%eax;"
#define OBFH_SF_FRAME_CONSTANTS(size) ".set .Lfh,1^((.Lsp>>(1+((.Lss_" size ">>4)&3)))&1);.set .Lfw,.Lss_" size ">>7;.set .Lfa,1^((.Lsp>>(4+((.Lss_" size ">>4)&3)))&1);.set .Lsf_hw_" size ",3+.Lfh;.set .Lsf_h_" size ",0xe58948+.Lfh*(0x242c8d48-0xe58948);.set .Lsf_aw_" size ",3+.Lfa;.set .Lsf_a_" size ",0xec8348-.Lfw*0x200+.Lfa*(0x24648d48-0xec8348+.Lfw*(0x400000+0x200));.set .Lsf_dw_" size ",1+3*.Lfw;.set .Lsf_d_" size ",.Lss_" size "*(1-2*.Lfa);"
#define OBFH_SF_FRAME_HEAD(size) "pushq %%rbp;.fill 1,.Lsf_hw_" size ",.Lsf_h_" size ";"
#define OBFH_SF_FRAME_ALLOC(size) ".fill 1,.Lsf_aw_" size ",.Lsf_a_" size ";.fill 1,.Lsf_dw_" size ",((.Lsf_d_" size "&0xffffffff)^0x80000000)-0x80000000;"
#define OBFH_SF_FRAME(size) OBFH_SF_FRAME_HEAD(size) OBFH_SF_FRAME_ALLOC(size)

#define OBFH_SF_FRAME_ALT(size) "pushq %%rbp;" OBFH_SF_FRAME_ALLOC(size) "leaq .Lss_" size "(%%rsp), %%rbp;"

#define OBFH_SF_EPILOGUE_ALT \
    ".fill 1,4+(-1)*.Lsp7," \
    "0x658d48+(0xec8948-0x658d48)*.Lsp7;" \
    "popq %%rbp; ret;"
#define OBFH_SF_EPILOGUE \
    ".fill 1,4+(-3)*.Lsp6," \
    "0x5dec8948+(0xc9-0x5dec8948)*.Lsp6;" \
    "ret;"
#define OBFH_SF_ARGS \
    ".fill 1,3+(-1)*.Lsp9," \
    "0x418d+(0xc889-0x418d)*.Lsp9;"
// Select argument order once; each call retains its two MOV imm32 instructions.
#define OBFH_SF_CALL_CONSTANTS ".set .Lsc_op0,0xba-.Lsp10;.set .Lsc_op1,0xb9+.Lsp10;.set .Lsc_arg0,.Lss_arg_b+(.Lss_arg_a-.Lss_arg_b)*.Lsp10;.set .Lsc_arg1,.Lss_arg_a+(.Lss_arg_b-.Lss_arg_a)*.Lsp10;"
#define OBFH_SF_CALL_ARGS ".byte .Lsc_op0;.long .Lsc_arg0;.byte .Lsc_op1;.long .Lsc_arg1;"
#define OBFH_SF_CALL_AT(target, channel) OBFH_SF_LINK_GATE("23", channel) "subq $32, %%rsp;" OBFH_SF_CALL_ARGS "call " target "; addq $32, %%rsp;"
#else
#define OBFH_SF_CALL_CONSTANTS ""
#define OBFH_SF_INPUT "movl %%esp, %%eax;"
#define OBFH_SF_FRAME_CONSTANTS(size) ".set .Lfh,1^((.Lsp>>(1+((.Lss_" size ">>4)&3)))&1);.set .Lfw,.Lss_" size ">>7;.set .Lfa,1^((.Lsp>>(4+((.Lss_" size ">>4)&3)))&1);.set .Lsf_hw_" size ",2+.Lfh;.set .Lsf_h_" size ",0xe589+.Lfh*(0x242c8d-0xe589);.set .Lsf_aw_" size ",2+.Lfa;.set .Lsf_a_" size ",0xec83-.Lfw*2+.Lfa*(0x24648d-0xec83+.Lfw*(0x4000+2));.set .Lsf_dw_" size ",1+3*.Lfw;.set .Lsf_d_" size ",.Lss_" size "*(1-2*.Lfa);"
#define OBFH_SF_FRAME_HEAD(size) "pushl %%ebp;.fill 1,.Lsf_hw_" size ",.Lsf_h_" size ";"

#define OBFH_SF_FRAME_ALLOC(size) ".fill 1,.Lsf_aw_" size ",.Lsf_a_" size ";.fill 1,.Lsf_dw_" size ",((.Lsf_d_" size "&0xffffffff)^0x80000000)-0x80000000;"
#define OBFH_SF_FRAME(size) OBFH_SF_FRAME_HEAD(size) OBFH_SF_FRAME_ALLOC(size)

#define OBFH_SF_FRAME_ALT(size) "pushl %%ebp;" OBFH_SF_FRAME_ALLOC(size) "leal .Lss_" size "(%%esp), %%ebp;"

#define OBFH_SF_EPILOGUE_ALT \
    ".fill 1,3+(-1)*.Lsp7," \
    "0x658d+(0xec89-0x658d)*.Lsp7;" \
    "popl %%ebp; ret;"
#define OBFH_SF_EPILOGUE \
    ".fill 1,3+(-2)*.Lsp6," \
    "0x5dec89+(0xc9-0x5dec89)*.Lsp6;" \
    "ret;"
#define OBFH_SF_ARGS \
    ".fill 1,3,0x08458b+" \
    "(0x0c558b-0x08458b)*.Lsp9;" \
    ".fill 1,3,0x0c558b+" \
    "(0x08458b-0x0c558b)*.Lsp9;"
#define OBFH_SF_CALL_AT(target, channel) OBFH_SF_LINK_GATE("18", channel) ".byte 0x68; .long .Lss_arg_b; .byte 0x68; .long .Lss_arg_a; call " target "; addl $8, %%esp;"
#endif
// Same 32-bit local stores: pack each opcode with its signed disp8.
#define OBFH_SF_LOCAL \
    ".set .LstA,0x4589+" \
    "(((-%c[local_a])&255)<<16);" \
    ".set .LstB,0x5589+" \
    "(((-%c[local_b])&255)<<16);" \
    ".fill 1,3,.LstB+" \
    "(.LstA-.LstB)*.Lsp8;" \
    ".fill 1,3,.LstA+" \
    "(.LstB-.LstA)*.Lsp8;"
#define OBFH_SF_CALL(target) OBFH_SF_CALL_AT(target, "0")
#define OBFH_SF_PADDING ".fill .Lss_pad, 1, 0x90;"
#define OBFH_SF_GAP ".fill .Lss_pad_b, 1, 0x90; .byte .Lss_noise_a, .Lss_noise_b;"
// Immediate-only aliases are established once by SF_ASM_CONSTANTS. They emit no bytes.
// Fixed literal fragments avoid repeated string-token concatenation in body selectors.
#define OBFH_SF_ROTATE_B ".fill 1,.Lsr_len,.LsrB;"
#define OBFH_SF_ROTATE_D ".fill 1,.Lsr_len,.LsrD;"
#define OBFH_SF_ROTATE_E ".fill 1,.Lsr_len,.LsrE;"
#define OBFH_SF_ROTATE_K ".fill 1,.Lsr_len,.LsrK;"
#define OBFH_SF_ROTATE_N ".fill 1,.Lsr_len,.LsrN;"
#define OBFH_SF_ROTATE_R ".fill 1,.Lsr_len,.LsrR;"
#define OBFH_SF_ADD_A ".fill 1,1+.Lsas,.Lsaadd;.fill 1,4-3*.Lsas,((.Lsav&0xffffffff)^0x80000000)-0x80000000;"
#define OBFH_SF_SUB_A ".fill 1,1+.Lsas,.Lsasub;.fill 1,4-3*.Lsas,((.Lsav&0xffffffff)^0x80000000)-0x80000000;"
#define OBFH_SF_ADD_B ".fill 1,1+.Lsbs,.Lsbadd;.fill 1,4-3*.Lsbs,((.Lsbv&0xffffffff)^0x80000000)-0x80000000;"
#define OBFH_SF_SUB_B ".fill 1,1+.Lsbs,.Lsbsub;.fill 1,4-3*.Lsbs,((.Lsbv&0xffffffff)^0x80000000)-0x80000000;"
#define OBFH_SF_SPELLING_CONSTANTS \
    ".set .Lsa,%c[key_a];.set .Lsb,%c[key_b];.set .Lsr,%c[rotate];.set .Lsr_len,3-((.Lsr==1)&1);.set .Lsr_op,0xc0c1+16*((.Lsr==1)&1)+65536*.Lsr;.set .Lsr_delta,2048*((.Lsr!=1)&1)+65536*(32-2*.Lsr);" \
    ".set .LsrB,.Lsr_op+.Lsp6*.Lsr_delta;" \
    ".set .LsrD,.Lsr_op+.Lsp7*.Lsr_delta;" \
    ".set .LsrE,.Lsr_op+.Lsp8*.Lsr_delta;" \
    ".set .LsrK,.Lsr_op+.Lsp9*.Lsr_delta;" \
    ".set .LsrN,.Lsr_op+2048+.Lsp10*(.Lsr_delta-4096*((.Lsr!=1)&1));" \
    ".set .LsrR,.Lsr_op+.Lsp11*.Lsr_delta;" \
    ".set .Lsaf,.Lsp12&(((%c[key_a]&0xffffffff)!=0xffffff80)&1);.set .Lsas,((((%c[key_a]+128)&0xffffff00)==0)&1);.set .Lsav,(%c[key_a]*(1-2*.Lsaf))&0xffffffff;" \
    ".set .Lsaadd,5+40*.Lsaf+.Lsas*(0xc07e+0x27d8*.Lsaf);.set .Lsasub,.Lsaadd+(1-2*.Lsaf)*(40+0x27d8*.Lsas);" \
    ".set .Lsbf,.Lsp12&(((%c[key_b]&0xffffffff)!=0xffffff80)&1);.set .Lsbs,((((%c[key_b]+128)&0xffffff00)==0)&1);.set .Lsbv,(%c[key_b]*(1-2*.Lsbf))&0xffffffff;" \
    ".set .Lsbadd,5+40*.Lsbf+.Lsbs*(0xc07e+0x27d8*.Lsbf);.set .Lsbsub,.Lsbadd+(1-2*.Lsbf)*(40+0x27d8*.Lsbs);"
#define OBFH_SF_CORE_A "xorl $%c[key_a], %%eax; imull $%c[factor], %%eax; addl %%edx, %%eax;"
#define OBFH_SF_BODY_A OBFH_SF_ARGS OBFH_SF_CORE_A OBFH_SF_LOCAL
#define OBFH_SF_CORE_B OBFH_SF_ADD_B OBFH_SF_ROTATE_B " xorl %%edx, %%eax;"
#define OBFH_SF_BODY_B OBFH_SF_ARGS OBFH_SF_CORE_B OBFH_SF_LOCAL
#define OBFH_SF_CORE_C "leal (%%eax, %%eax, 2), %%eax; xorl %%eax, %%edx; " OBFH_SF_ADD_A
#define OBFH_SF_BODY_C OBFH_SF_ARGS OBFH_SF_CORE_C OBFH_SF_LOCAL

#define OBFH_SF_CORE_D "notl %%eax; addl %%edx, %%eax; imull $%c[factor], %%eax; " OBFH_SF_ROTATE_D
#define OBFH_SF_BODY_D OBFH_SF_ARGS OBFH_SF_CORE_D OBFH_SF_LOCAL

#define OBFH_SF_CORE_E "bswap %%eax; xorl %%edx, %%eax; " OBFH_SF_ROTATE_E
#define OBFH_SF_BODY_E OBFH_SF_ARGS OBFH_SF_CORE_E OBFH_SF_LOCAL

#define OBFH_SF_CORE_F "movl $%c[loops], %%ecx; 6: addl %%edx, %%eax; imull $%c[factor], %%eax; xorl $%c[key_b], %%eax; decl %%ecx; jnz 6b;"
#define OBFH_SF_BODY_F OBFH_SF_ARGS OBFH_SF_CORE_F OBFH_SF_LOCAL

#define OBFH_SF_CORE_G "movzbl %%al, %%ecx; shrl $8, %%eax; xorl %%edx, %%ecx; leal (%%eax, %%ecx, 4), %%eax;"
#define OBFH_SF_BODY_G OBFH_SF_ARGS OBFH_SF_CORE_G OBFH_SF_LOCAL
#define OBFH_SF_CORE_H "imull %%eax, %%edx; xorl $%c[key_b], %%edx; subl %%edx, %%eax; negl %%eax;"
#define OBFH_SF_BODY_H OBFH_SF_ARGS OBFH_SF_CORE_H OBFH_SF_LOCAL

#define OBFH_SF_CORE_I "movl %%eax, %%ecx; orl $%c[mask], %%eax; andl $%c[mask], %%ecx; subl %%ecx, %%eax; xorl %%edx, %%eax;"
#define OBFH_SF_BODY_I OBFH_SF_ARGS OBFH_SF_CORE_I OBFH_SF_LOCAL

#define OBFH_SF_CORE_J "xorl $%c[key_a], %%eax; movl %%edx, %%ecx; shll $%c[rotate], %%ecx; shrl $%c[rotate], %%eax; orl %%ecx, %%eax;"
#define OBFH_SF_BODY_J OBFH_SF_ARGS OBFH_SF_CORE_J OBFH_SF_LOCAL

#define OBFH_SF_CORE_K "testl %%edx, %%edx; js 6f; " OBFH_SF_ADD_A " .byte 0xeb,4; 6: negl %%eax; xorl %%edx, %%eax; 7: " OBFH_SF_ROTATE_K
#define OBFH_SF_BODY_K OBFH_SF_ARGS OBFH_SF_CORE_K OBFH_SF_LOCAL

#define OBFH_SF_CORE_L "movl %%eax, %%ecx; movl %%edx, %%eax; addl %%ecx, %%eax; imull $%c[factor], %%eax; notl %%edx; xorl %%edx, %%eax;"
#define OBFH_SF_BODY_L OBFH_SF_ARGS OBFH_SF_CORE_L OBFH_SF_LOCAL

#define OBFH_SF_CORE_M "movl %%eax, %%ecx; shrl $16, %%ecx; xorl %%ecx, %%eax; imull $%c[factor], %%eax; xorl %%edx, %%eax;"
#define OBFH_SF_BODY_M OBFH_SF_ARGS OBFH_SF_CORE_M OBFH_SF_LOCAL

#define OBFH_SF_CORE_N "addl $%c[key_a], %%eax; adcl $%c[key_b], %%edx; xorl %%edx, %%eax; " OBFH_SF_ROTATE_N
#define OBFH_SF_BODY_N OBFH_SF_ARGS OBFH_SF_CORE_N OBFH_SF_LOCAL

#define OBFH_SF_CORE_O "xchgl %%eax, %%edx; " OBFH_SF_SUB_A " bswap %%edx; addl %%edx, %%eax;"
#define OBFH_SF_BODY_O OBFH_SF_ARGS OBFH_SF_CORE_O OBFH_SF_LOCAL

#define OBFH_SF_CORE_P "movl $%c[factor], %%ecx; xorl %%edx, %%edx; divl %%ecx; xorl %%edx, %%eax;"
#define OBFH_SF_BODY_P OBFH_SF_ARGS OBFH_SF_CORE_P OBFH_SF_LOCAL

#define OBFH_SF_CORE_Q "movzbl %%al, %%ecx; movzbl %%dl, %%edx; imull %%edx, %%ecx; shrl $8, %%eax; addl %%ecx, %%eax;"
#define OBFH_SF_BODY_Q OBFH_SF_ARGS OBFH_SF_CORE_Q OBFH_SF_LOCAL
#define OBFH_SF_CORE_R "movl $%c[loops], %%ecx; 6: xorl %%edx, %%eax; " OBFH_SF_ROTATE_R " addl $%c[key_a], %%edx; decl %%ecx; jnz 6b;"
#define OBFH_SF_BODY_R OBFH_SF_ARGS OBFH_SF_CORE_R OBFH_SF_LOCAL

#define OBFH_SF_CORE_S "shldl $%c[rotate], %%edx, %%eax; " OBFH_SF_SUB_B " xorl $%c[mask], %%eax;"
#define OBFH_SF_BODY_S OBFH_SF_ARGS OBFH_SF_CORE_S OBFH_SF_LOCAL

#define OBFH_SF_CORE_T "movl %%eax, %%ecx; shrl $16, %%eax; shll $16, %%ecx; orl %%ecx, %%eax; xorl %%edx, %%eax; negl %%eax;"
#define OBFH_SF_BODY_T OBFH_SF_ARGS OBFH_SF_CORE_T OBFH_SF_LOCAL

// All fifteen identities hold for arbitrary uint32 inputs, including wraparound.
#define OBFH_SF_GUARD_0 "imull %%eax, %%eax; testb $2, %%al; jz 9f;"
#define OBFH_SF_GUARD_1 "leal 1(%%eax), %%edx; imull %%edx, %%eax; testb $1, %%al; jz 9f;"
#define OBFH_SF_GUARD_2 OBFH_ASM_MOV32("2", "0", "%c[phantom]", "15") "imull %%eax, %%eax; xorl %%edx, %%eax; testb $1, %%al; jz 9f;"
#define OBFH_SF_GUARD_3 OBFH_ASM_MOV32("2", "0", "%c[phantom]", "15") "notl %%edx; addl %%eax, %%edx; cmpl $-1, %%edx; je 9f;"

#define OBFH_SF_GUARD_4 OBFH_ASM_MOV32("2", "0", "%c[phantom]", "15") OBFH_ASM_MOV32("1", "0", "%c[phantom]", "15") "orl $%c[mask], %%eax; andl $%c[mask], %%edx; addl %%edx, %%eax; addl $%c[mask], %%ecx; " OBFH_ASM_EQ32("0", "1", "%c[phantom]", "16") "je 9f;"

#define OBFH_SF_GUARD_5 OBFH_ASM_MOV32("2", "0", "%c[phantom]", "15") OBFH_ASM_ROTATE_PAIR("0", "%c[rotate]", "%c[phantom]", "0") OBFH_ASM_ROTATE_PAIR("0", "%c[rotate]", "%c[phantom]", "1") OBFH_ASM_EQ32("0", "2", "%c[phantom]", "16") "je 9f;"
#define OBFH_SF_GUARD_6 OBFH_ASM_MOV32("2", "0", "%c[phantom]", "15") "bswap %%eax; bswap %%eax; " OBFH_ASM_EQ32("0", "2", "%c[phantom]", "16") "je 9f;"
#define OBFH_SF_GUARD_7 "leal -1(%%eax), %%edx; imull %%edx, %%eax; testb $1, %%al; jz 9f;"
// These three operations commute; the final comparison supplies the flags.
#if ((OBFH_SEED >> 16) & 7u) % 6u == 0u
#define OBFH_SF_GUARD_8_ORDER "notl %%edx;xorl $%c[key_a],%%eax;xorl $%c[key_a],%%edx;"
#endif
#if ((OBFH_SEED >> 16) & 7u) % 6u == 1u
#define OBFH_SF_GUARD_8_ORDER "notl %%edx;xorl $%c[key_a],%%edx;xorl $%c[key_a],%%eax;"
#endif
#if ((OBFH_SEED >> 16) & 7u) % 6u == 2u
#define OBFH_SF_GUARD_8_ORDER "xorl $%c[key_a],%%eax;notl %%edx;xorl $%c[key_a],%%edx;"
#endif
#if ((OBFH_SEED >> 16) & 7u) % 6u == 3u
#define OBFH_SF_GUARD_8_ORDER "xorl $%c[key_a],%%eax;xorl $%c[key_a],%%edx;notl %%edx;"
#endif
#if ((OBFH_SEED >> 16) & 7u) % 6u == 4u
#define OBFH_SF_GUARD_8_ORDER "xorl $%c[key_a],%%edx;notl %%edx;xorl $%c[key_a],%%eax;"
#endif
#if ((OBFH_SEED >> 16) & 7u) % 6u == 5u
#define OBFH_SF_GUARD_8_ORDER "xorl $%c[key_a],%%edx;xorl $%c[key_a],%%eax;notl %%edx;"
#endif
#define OBFH_SF_GUARD_8 OBFH_ASM_MOV32("2", "0", "%c[phantom]", "15") OBFH_SF_GUARD_8_ORDER "notl %%edx;cmpl %%edx,%%eax;je 9f;"
#define OBFH_SF_GUARD_9 "movl %%eax, %%ecx;" OBFH_ASM_MOV32("2", "0", "%c[phantom]", "15") "xorl $%c[mask], %%eax; andl $%c[mask], %%edx; leal (%%eax, %%edx, 2), %%eax; addl $%c[mask], %%ecx; " OBFH_ASM_EQ32("0", "1", "%c[phantom]", "16") "je 9f;"

#define OBFH_SF_GUARD_10 OBFH_ASM_MOV32("2", "0", "%c[phantom]", "15") "negl %%edx; andl %%edx, %%eax; leal -1(%%eax), %%edx; testl %%edx, %%eax; jz 9f;"
#define OBFH_SF_GUARD_11 OBFH_ASM_MOV32("2", "0", "%c[phantom]", "15") OBFH_ASM_MOV32("1", "0", "%c[phantom]", "15") "andl $%c[mask], %%eax; andl $%c[not_mask], %%edx; imull $%c[factor], %%eax; imull $%c[factor], %%edx; addl %%edx, %%eax; imull $%c[factor], %%ecx; " OBFH_ASM_EQ32("0", "1", "%c[phantom]", "16") "je 9f;"

#define OBFH_SF_GUARD_12 OBFH_ASM_MOV32("2", "0", "%c[phantom]", "15") "addl $%c[key_a], %%eax; imull $%c[factor], %%eax; imull $%c[factor], %%edx; movl $%c[key_a], %%ecx; imull $%c[factor], %%ecx; addl %%edx, %%ecx; " OBFH_ASM_EQ32("0", "1", "%c[phantom]", "16") "je 9f;"

#define OBFH_SF_GUARD_13 "movl %%eax, %%ecx;" OBFH_ASM_MOV32("2", "0", "%c[phantom]", "15") "addl $%c[key_a], %%eax; imull %%eax, %%eax; imull %%ecx, %%ecx; subl %%ecx, %%eax;imull $%c[key_a], %%edx; addl %%edx, %%edx; .byte 0x81,0xc2; .long (((%c[key_a]*%c[key_a])&0xffffffff)^0x80000000)-0x80000000;" OBFH_ASM_EQ32("0", "2", "%c[phantom]", "16") "je 9f;"
#define OBFH_SF_GUARD_14 OBFH_ASM_MOV32("2", "0", "%c[phantom]", "15") "roll $16, %%eax; roll $16, %%eax; " OBFH_ASM_EQ32("0", "2", "%c[phantom]", "16") "je 9f;"
#define OBFH_SF_ENTRY_ALT(label, frame, body) label ": " OBFH_SF_FRAME_ALT(frame) body ".fill ((%c[phantom] >> " label ") & 1), 1, 0x90;"
#define OBFH_SF_ENTRY(label, frame, body) label ": " OBFH_SF_FRAME(frame) body ".fill ((%c[phantom] >> " label ") & 1), 1, 0x90;"
// One source per layout: skeleton fragments alternate with native body roles.
// Callbacks select a physical slot without expanding the other slots' bodies.
#define OBFH_SF_ENTRY_PADDING(label) ".fill ((.Lsp >> " label ") & 1),1,0x90;"
#define OBFH_SF_SLOT_DROP(context, text, role)
#define OBFH_SF_SLOT_TEXT(context, text, role) text
#define OBFH_SF_SLOT_ROLE(context, text, role) +role
#define OBFH_SF_REFERENCE_0(a, b, c, d, e)
#define OBFH_SF_REFERENCE_1(a, b, c, d, e) a
#define OBFH_SF_REFERENCE_2(a, b, c, d, e) b
#define OBFH_SF_REFERENCE_3(a, b, c, d, e) c
#define OBFH_SF_REFERENCE_4(a, b, c, d, e) d
#define OBFH_SF_REFERENCE_5(a, b, c, d, e) e
#define OBFH_SF_SLOT_REFERENCE(context, text, role) text OBFH_SF_REFERENCE_##role context
#define OBFH_SF_SLOT_0(layout, emit, context) layout(emit, OBFH_SF_SLOT_DROP, OBFH_SF_SLOT_DROP, OBFH_SF_SLOT_DROP, OBFH_SF_SLOT_DROP, OBFH_SF_SLOT_DROP, OBFH_SF_SLOT_DROP, OBFH_SF_SLOT_DROP, OBFH_SF_SLOT_DROP, context)
#define OBFH_SF_SLOT_1(layout, emit, context) layout(OBFH_SF_SLOT_DROP, emit, OBFH_SF_SLOT_DROP, OBFH_SF_SLOT_DROP, OBFH_SF_SLOT_DROP, OBFH_SF_SLOT_DROP, OBFH_SF_SLOT_DROP, OBFH_SF_SLOT_DROP, OBFH_SF_SLOT_DROP, context)
#define OBFH_SF_SLOT_2(layout, emit, context) layout(OBFH_SF_SLOT_DROP, OBFH_SF_SLOT_DROP, emit, OBFH_SF_SLOT_DROP, OBFH_SF_SLOT_DROP, OBFH_SF_SLOT_DROP, OBFH_SF_SLOT_DROP, OBFH_SF_SLOT_DROP, OBFH_SF_SLOT_DROP, context)
#define OBFH_SF_SLOT_3(layout, emit, context) layout(OBFH_SF_SLOT_DROP, OBFH_SF_SLOT_DROP, OBFH_SF_SLOT_DROP, emit, OBFH_SF_SLOT_DROP, OBFH_SF_SLOT_DROP, OBFH_SF_SLOT_DROP, OBFH_SF_SLOT_DROP, OBFH_SF_SLOT_DROP, context)
#define OBFH_SF_SLOT_4(layout, emit, context) layout(OBFH_SF_SLOT_DROP, OBFH_SF_SLOT_DROP, OBFH_SF_SLOT_DROP, OBFH_SF_SLOT_DROP, emit, OBFH_SF_SLOT_DROP, OBFH_SF_SLOT_DROP, OBFH_SF_SLOT_DROP, OBFH_SF_SLOT_DROP, context)
#define OBFH_SF_SLOT_5(layout, emit, context) layout(OBFH_SF_SLOT_DROP, OBFH_SF_SLOT_DROP, OBFH_SF_SLOT_DROP, OBFH_SF_SLOT_DROP, OBFH_SF_SLOT_DROP, emit, OBFH_SF_SLOT_DROP, OBFH_SF_SLOT_DROP, OBFH_SF_SLOT_DROP, context)
#define OBFH_SF_SLOT_6(layout, emit, context) layout(OBFH_SF_SLOT_DROP, OBFH_SF_SLOT_DROP, OBFH_SF_SLOT_DROP, OBFH_SF_SLOT_DROP, OBFH_SF_SLOT_DROP, OBFH_SF_SLOT_DROP, emit, OBFH_SF_SLOT_DROP, OBFH_SF_SLOT_DROP, context)
#define OBFH_SF_SLOT_7(layout, emit, context) layout(OBFH_SF_SLOT_DROP, OBFH_SF_SLOT_DROP, OBFH_SF_SLOT_DROP, OBFH_SF_SLOT_DROP, OBFH_SF_SLOT_DROP, OBFH_SF_SLOT_DROP, OBFH_SF_SLOT_DROP, emit, OBFH_SF_SLOT_DROP, context)
#define OBFH_SF_SLOT_8(layout, emit, context) layout(OBFH_SF_SLOT_DROP, OBFH_SF_SLOT_DROP, OBFH_SF_SLOT_DROP, OBFH_SF_SLOT_DROP, OBFH_SF_SLOT_DROP, OBFH_SF_SLOT_DROP, OBFH_SF_SLOT_DROP, OBFH_SF_SLOT_DROP, emit, context)
// Physical order, finite link graphs, and shared/tail continuations vary.
#define OBFH_SF_LAYOUT_SOURCE_0(A, B, C, D, E, F, G, H, I, context) \
    A(context, OBFH_SF_CALL_AT("301f", "0") OBFH_SF_EPILOGUE OBFH_SF_GAP "302:;" OBFH_SF_FRAME("frame_b"), 2) \
    B(context, OBFH_SF_ENTRY_PADDING("2") OBFH_SF_EPILOGUE OBFH_SF_GAP "200:;301:;" OBFH_SF_FRAME("frame_a"), \
      1) \
    C(context, OBFH_SF_ENTRY_PADDING("1") OBFH_SF_CALL_AT("302b", "1") OBFH_SF_EPILOGUE_ALT, 0)
#define OBFH_SF_LAYOUT_0(a, b, c) OBFH_SF_LAYOUT_SOURCE_0(OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, (a, b, c, "", ""))
#define OBFH_SF_LAYOUT_SOURCE_1(A, B, C, D, E, F, G, H, I, context) \
    A(context, OBFH_SF_CALL_AT("311f", "0") OBFH_SF_EPILOGUE OBFH_SF_GAP "210:;311:;" OBFH_SF_FRAME_ALT("frame_a"), \
      1) \
    B(context, OBFH_SF_ENTRY_PADDING("1") "leave; jmp 312f;" OBFH_SF_GAP "312:;" OBFH_SF_FRAME("frame_b"), 2) \
    C(context, OBFH_SF_ENTRY_PADDING("2") OBFH_SF_EPILOGUE, 0)
#define OBFH_SF_LAYOUT_1(a, b, c) OBFH_SF_LAYOUT_SOURCE_1(OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, (a, b, c, "", ""))
#define OBFH_SF_LAYOUT_SOURCE_2(A, B, C, D, E, F, G, H, I, context) \
    A(context, OBFH_SF_CALL_AT("321f", "0") OBFH_SF_EPILOGUE OBFH_SF_GAP "323:;" OBFH_SF_FRAME("frame_c"), 3) \
    B(context, OBFH_SF_ENTRY_PADDING("3") OBFH_SF_EPILOGUE OBFH_SF_GAP "220:;321:;" OBFH_SF_FRAME("frame_a"), \
      1) \
    C(context, OBFH_SF_ENTRY_PADDING("1") OBFH_SF_CALL_AT("322f", "1") OBFH_SF_CALL_AT("323b", "2") OBFH_SF_EPILOGUE OBFH_SF_GAP "322:;" OBFH_SF_FRAME_ALT("frame_b"), 2) \
    D(context, OBFH_SF_ENTRY_PADDING("2") OBFH_SF_CALL_AT("323b", "3") OBFH_SF_EPILOGUE_ALT, 0)
#define OBFH_SF_LAYOUT_2(a, b, c) OBFH_SF_LAYOUT_SOURCE_2(OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, (a, b, c, "", ""))
#define OBFH_SF_LAYOUT_SOURCE_3(A, B, C, D, E, F, G, H, I, context) \
    A(context, OBFH_SF_CALL_AT("331f", "0") OBFH_SF_EPILOGUE OBFH_SF_GAP "332:;" OBFH_SF_FRAME("frame_b"), 2) \
    B(context, OBFH_SF_ENTRY_PADDING("2") "leave; jmp 333f;" OBFH_SF_GAP "333:;" OBFH_SF_FRAME("frame_c"), 3) \
    C(context, OBFH_SF_ENTRY_PADDING("3") OBFH_SF_EPILOGUE OBFH_SF_GAP "230:;331:;" OBFH_SF_FRAME_ALT("frame_a"), \
      1) \
    D(context, OBFH_SF_ENTRY_PADDING("1") "testl %%eax, %%eax; jz 334f;" OBFH_SF_CALL_AT("332b", "1") "jmp 335f; 334:" OBFH_SF_CALL_AT("333b", "2") "335:" OBFH_SF_EPILOGUE_ALT, 0)
#define OBFH_SF_LAYOUT_3(a, b, c) OBFH_SF_LAYOUT_SOURCE_3(OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, (a, b, c, "", ""))
#define OBFH_SF_LAYOUT_SOURCE_4(A, B, C, D, E, F, G, H, I, context) \
    A(context, OBFH_SF_CALL_AT("341f", "0") OBFH_SF_EPILOGUE OBFH_SF_GAP "342:;" OBFH_SF_FRAME_ALT("frame_b"), 2) \
    B(context, OBFH_SF_ENTRY_PADDING("2") OBFH_SF_CALL_AT("343f", "1") OBFH_SF_EPILOGUE_ALT OBFH_SF_GAP "240:;341:;" OBFH_SF_FRAME("frame_a"), \
      1) \
    C(context, OBFH_SF_ENTRY_PADDING("1") OBFH_SF_CALL_AT("342b", "2") OBFH_SF_EPILOGUE OBFH_SF_GAP "343:;" OBFH_SF_FRAME("frame_c"), 3) \
    D(context, OBFH_SF_ENTRY_PADDING("3") OBFH_SF_EPILOGUE, 0)
#define OBFH_SF_LAYOUT_4(a, b, c) OBFH_SF_LAYOUT_SOURCE_4(OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, (a, b, c, "", ""))
#define OBFH_SF_LAYOUT_SOURCE_5(A, B, C, D, E, F, G, H, I, context) \
    A(context, OBFH_SF_CALL_AT("351f", "0") OBFH_SF_EPILOGUE OBFH_SF_GAP "353:;" OBFH_SF_FRAME("frame_c"), 3) \
    B(context, OBFH_SF_ENTRY_PADDING("3") OBFH_SF_EPILOGUE OBFH_SF_GAP "352:;" OBFH_SF_FRAME("frame_b"), 2) \
    C(context, OBFH_SF_ENTRY_PADDING("2") OBFH_SF_EPILOGUE OBFH_SF_GAP "250:;351:;" OBFH_SF_FRAME_ALT("frame_a"), \
      1) \
    D(context, OBFH_SF_ENTRY_PADDING("1") OBFH_SF_CALL_AT("352b", "1") OBFH_SF_CALL_AT("353b", "2") OBFH_SF_EPILOGUE_ALT, 0)
#define OBFH_SF_LAYOUT_5(a, b, c) OBFH_SF_LAYOUT_SOURCE_5(OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, (a, b, c, "", ""))
#define OBFH_SF_LAYOUT_SOURCE_6(A, B, C, D, E, F, G, H, I, context) \
    A(context, OBFH_SF_CALL_AT("361f", "0") OBFH_SF_EPILOGUE OBFH_SF_GAP "260:;361:;" OBFH_SF_FRAME("frame_a"), \
      1) \
    B(context, OBFH_SF_ENTRY_PADDING("1") OBFH_SF_CALL_AT("362f", "1") "leave; jmp 363f;" OBFH_SF_GAP "362:;" OBFH_SF_FRAME_ALT("frame_b"), 2) \
    C(context, OBFH_SF_ENTRY_PADDING("2") "leave; jmp 363f;" OBFH_SF_GAP "363:;" OBFH_SF_FRAME("frame_c"), 3) \
    D(context, OBFH_SF_ENTRY_PADDING("3") OBFH_SF_EPILOGUE, 0)
#define OBFH_SF_LAYOUT_6(a, b, c) OBFH_SF_LAYOUT_SOURCE_6(OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, (a, b, c, "", ""))
#define OBFH_SF_LAYOUT_SOURCE_7(A, B, C, D, E, F, G, H, I, context) \
    A(context, OBFH_SF_CALL_AT("371f", "0") OBFH_SF_EPILOGUE OBFH_SF_GAP "373:;" OBFH_SF_FRAME("frame_c"), 3) \
    B(context, OBFH_SF_ENTRY_PADDING("3") OBFH_SF_EPILOGUE OBFH_SF_GAP "372:;" OBFH_SF_FRAME("frame_b"), 2) \
    C(context, OBFH_SF_ENTRY_PADDING("2") OBFH_SF_CALL_AT("373b", "1") OBFH_SF_EPILOGUE OBFH_SF_GAP "270:;371:;" OBFH_SF_FRAME_ALT("frame_a"), \
      1) \
    D(context, OBFH_SF_ENTRY_PADDING("1") OBFH_SF_CALL_AT("372b", "2") OBFH_SF_CALL_AT("373b", "3") OBFH_SF_EPILOGUE_ALT, 0)
#define OBFH_SF_LAYOUT_7(a, b, c) OBFH_SF_LAYOUT_SOURCE_7(OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, (a, b, c, "", ""))
#define OBFH_SF_LAYOUT_SOURCE_8(A, B, C, D, E, F, G, H, I, context) \
    A(context, OBFH_SF_CALL_AT("381f", "0") OBFH_SF_CALL_AT("382f", "1") OBFH_SF_EPILOGUE OBFH_SF_GAP "383:;" OBFH_SF_FRAME("frame_c"), 3) \
    B(context, OBFH_SF_ENTRY_PADDING("3") OBFH_SF_EPILOGUE OBFH_SF_GAP "280:;381:;" OBFH_SF_FRAME("frame_a"), \
      1) \
    C(context, OBFH_SF_ENTRY_PADDING("1") OBFH_SF_CALL_AT("383b", "2") OBFH_SF_EPILOGUE OBFH_SF_GAP "382:;" OBFH_SF_FRAME_ALT("frame_b"), 2) \
    D(context, OBFH_SF_ENTRY_PADDING("2") OBFH_SF_CALL_AT("383b", "3") OBFH_SF_EPILOGUE_ALT, 0)
#define OBFH_SF_LAYOUT_8(a, b, c) OBFH_SF_LAYOUT_SOURCE_8(OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, (a, b, c, "", ""))
#define OBFH_SF_LAYOUT_SOURCE_9(A, B, C, D, E, F, G, H, I, context) \
    A(context, OBFH_SF_CALL_AT("391f", "0") OBFH_SF_EPILOGUE OBFH_SF_GAP "393:;" OBFH_SF_FRAME("frame_c"), 3) \
    B(context, OBFH_SF_ENTRY_PADDING("3") OBFH_SF_EPILOGUE OBFH_SF_GAP "392:;" OBFH_SF_FRAME_ALT("frame_b"), 2) \
    C(context, OBFH_SF_ENTRY_PADDING("2") "testl %%eax, %%eax; jz 394f;" OBFH_SF_CALL_AT("393b", "1") "394:" OBFH_SF_EPILOGUE_ALT OBFH_SF_GAP "290:;391:;" OBFH_SF_FRAME("frame_a"), \
      1) \
    D(context, OBFH_SF_ENTRY_PADDING("1") "leave; jmp 392b;", 0)
#define OBFH_SF_LAYOUT_9(a, b, c) OBFH_SF_LAYOUT_SOURCE_9(OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, (a, b, c, "", ""))
#define OBFH_SF_LAYOUT_SOURCE_10(A, B, C, D, E, F, G, H, I, context) \
    A(context, OBFH_SF_CALL_AT("3101f", "0") OBFH_SF_EPILOGUE OBFH_SF_GAP "3103:;" OBFH_SF_FRAME("frame_c"), 3) \
    B(context, OBFH_SF_ENTRY_PADDING("3") OBFH_SF_EPILOGUE OBFH_SF_GAP "3102:;" OBFH_SF_FRAME_ALT("frame_b"), 2) \
    C(context, OBFH_SF_ENTRY_PADDING("2") OBFH_SF_CALL_AT("3103b", "1") OBFH_SF_EPILOGUE_ALT OBFH_SF_GAP "3101:;" OBFH_SF_FRAME("frame_a"), 1) \
    D(context, OBFH_SF_ENTRY_PADDING("1") OBFH_SF_CALL_AT("3102b", "2") OBFH_SF_EPILOGUE, 0)
#define OBFH_SF_LAYOUT_10(a, b, c, d) OBFH_SF_LAYOUT_SOURCE_10(OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, (a, b, c, d, ""))
#define OBFH_SF_LAYOUT_SOURCE_11(A, B, C, D, E, F, G, H, I, context) \
    A(context, OBFH_SF_CALL_AT("3111f", "0") OBFH_SF_EPILOGUE OBFH_SF_GAP "3112:;" OBFH_SF_FRAME("frame_b"), 2) \
    B(context, OBFH_SF_ENTRY_PADDING("2") OBFH_SF_CALL_AT("3113f", "1") OBFH_SF_EPILOGUE OBFH_SF_GAP "3111:;" OBFH_SF_FRAME_ALT("frame_a"), 1) \
    C(context, OBFH_SF_ENTRY_PADDING("1") OBFH_SF_CALL_AT("3112b", "2") OBFH_SF_CALL_AT("3113f", "3") OBFH_SF_EPILOGUE_ALT OBFH_SF_GAP "3113:;" OBFH_SF_FRAME("frame_c"), 3) \
    D(context, OBFH_SF_ENTRY_PADDING("3") OBFH_SF_EPILOGUE, 0)
#define OBFH_SF_LAYOUT_11(a, b, c, d) OBFH_SF_LAYOUT_SOURCE_11(OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, (a, b, c, d, ""))
#define OBFH_SF_LAYOUT_SOURCE_12(A, B, C, D, E, F, G, H, I, context) \
    A(context, OBFH_SF_CALL_AT("3121f", "0") OBFH_SF_EPILOGUE OBFH_SF_GAP "3121:;" OBFH_SF_FRAME("frame_a"), 1) \
    B(context, OBFH_SF_ENTRY_PADDING("1") OBFH_SF_CALL_AT("3122f", "1") OBFH_SF_CALL_AT("3128f", "2") OBFH_SF_EPILOGUE OBFH_SF_GAP "3122:;" OBFH_SF_FRAME("frame_b"), 2) \
    C(context, OBFH_SF_ENTRY_PADDING("2") OBFH_SF_CALL_AT("3123f", "3") OBFH_SF_EPILOGUE OBFH_SF_GAP "3123:;" OBFH_SF_FRAME_ALT("frame_c"), 3) \
    D(context, OBFH_SF_ENTRY_PADDING("3") OBFH_SF_EPILOGUE_ALT OBFH_SF_GAP "3128:;" OBFH_SF_FRAME("frame_d"), 4) \
    E(context, OBFH_SF_ENTRY_PADDING("8") OBFH_SF_EPILOGUE, 0)
#define OBFH_SF_LAYOUT_12(a, b, c, d) OBFH_SF_LAYOUT_SOURCE_12(OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, (a, b, c, d, ""))
#define OBFH_SF_LAYOUT_SOURCE_13(A, B, C, D, E, F, G, H, I, context) \
    A(context, OBFH_SF_CALL_AT("3131f", "0") OBFH_SF_EPILOGUE OBFH_SF_GAP "3131:;" OBFH_SF_FRAME("frame_a"), 1) \
    B(context, OBFH_SF_ENTRY_PADDING("1") OBFH_SF_CALL_AT("3132f", "1") OBFH_SF_EPILOGUE OBFH_SF_GAP "3132:;" OBFH_SF_FRAME_ALT("frame_b"), 2) \
    C(context, OBFH_SF_ENTRY_PADDING("2") OBFH_SF_CALL_AT("3133f", "2") OBFH_SF_EPILOGUE_ALT OBFH_SF_GAP "3133:;" OBFH_SF_FRAME("frame_c"), 3) \
    D(context, OBFH_SF_ENTRY_PADDING("3") "leave; jmp 3138f;" OBFH_SF_GAP "3138:;" OBFH_SF_FRAME("frame_d"), 4) \
    E(context, OBFH_SF_ENTRY_PADDING("8") OBFH_SF_EPILOGUE, 0)
#define OBFH_SF_LAYOUT_13(a, b, c, d) OBFH_SF_LAYOUT_SOURCE_13(OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, (a, b, c, d, ""))
#define OBFH_SF_LAYOUT_SOURCE_14(A, B, C, D, E, F, G, H, I, context) \
    A(context, OBFH_SF_CALL_AT("3141f", "0") OBFH_SF_EPILOGUE OBFH_SF_GAP "3148:;" OBFH_SF_FRAME("frame_d"), 4) \
    B(context, OBFH_SF_ENTRY_PADDING("8") OBFH_SF_CALL_AT("3141f", "1") OBFH_SF_EPILOGUE OBFH_SF_GAP "3143:;" OBFH_SF_FRAME_ALT("frame_c"), 3) \
    C(context, OBFH_SF_ENTRY_PADDING("3") OBFH_SF_EPILOGUE_ALT OBFH_SF_GAP "3141:;" OBFH_SF_FRAME("frame_a"), 1) \
    D(context, OBFH_SF_ENTRY_PADDING("1") OBFH_SF_CALL_AT("3142f", "2") OBFH_SF_EPILOGUE OBFH_SF_GAP "3142:;" OBFH_SF_FRAME("frame_b"), 2) \
    E(context, OBFH_SF_ENTRY_PADDING("2") OBFH_SF_CALL_AT("3143b", "3") OBFH_SF_CALL_AT("3148b", "4") OBFH_SF_EPILOGUE, 0)
#define OBFH_SF_LAYOUT_14(a, b, c, d) OBFH_SF_LAYOUT_SOURCE_14(OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, (a, b, c, d, ""))
#define OBFH_SF_LAYOUT_SOURCE_15(A, B, C, D, E, F, G, H, I, context) \
    A(context, OBFH_SF_CALL_AT("3151f", "0") OBFH_SF_EPILOGUE OBFH_SF_GAP "3151:;" OBFH_SF_FRAME_ALT("frame_a"), 1) \
    B(context, OBFH_SF_ENTRY_PADDING("1") "testb $1, %%al; jz 3154f;" OBFH_SF_CALL_AT("3152f", "1") "jmp 3155f; 3154:" OBFH_SF_CALL_AT("3158f", "2") "3155:" OBFH_SF_EPILOGUE_ALT OBFH_SF_GAP "3152:;" OBFH_SF_FRAME("frame_b"), 2) \
    C(context, OBFH_SF_ENTRY_PADDING("2") OBFH_SF_CALL_AT("3153f", "3") OBFH_SF_EPILOGUE OBFH_SF_GAP "3158:;" OBFH_SF_FRAME("frame_d"), 4) \
    D(context, OBFH_SF_ENTRY_PADDING("8") OBFH_SF_CALL_AT("3153f", "4") OBFH_SF_EPILOGUE OBFH_SF_GAP "3153:;" OBFH_SF_FRAME("frame_c"), 3) \
    E(context, OBFH_SF_ENTRY_PADDING("3") OBFH_SF_EPILOGUE, 0)
#define OBFH_SF_LAYOUT_15(a, b, c, d) OBFH_SF_LAYOUT_SOURCE_15(OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, (a, b, c, d, ""))
#define OBFH_SF_LAYOUT_SOURCE_16(A, B, C, D, E, F, G, H, I, context) \
    A(context, OBFH_SF_CALL_AT("3161f", "0") OBFH_SF_EPILOGUE OBFH_SF_GAP "3161:;" OBFH_SF_FRAME("frame_a"), 1) \
    B(context, OBFH_SF_ENTRY_PADDING("1") OBFH_SF_CALL_AT("3162f", "1") OBFH_SF_EPILOGUE OBFH_SF_GAP "3162:;" OBFH_SF_FRAME("frame_b"), 2) \
    C(context, OBFH_SF_ENTRY_PADDING("2") "testl %%edx, %%edx; jz 3164f;" OBFH_SF_CALL_AT("3162b", "2") "3164:" OBFH_SF_CALL_AT("3163f", "3") OBFH_SF_EPILOGUE OBFH_SF_GAP "3163:;" OBFH_SF_FRAME_ALT("frame_c"), 3) \
    D(context, OBFH_SF_ENTRY_PADDING("3") OBFH_SF_EPILOGUE_ALT, 0)
#define OBFH_SF_LAYOUT_16(a, b, c, d) OBFH_SF_LAYOUT_SOURCE_16(OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, (a, b, c, d, ""))
#define OBFH_SF_LAYOUT_SOURCE_17(A, B, C, D, E, F, G, H, I, context) \
    A(context, OBFH_SF_CALL_AT("3171f", "0") OBFH_SF_EPILOGUE OBFH_SF_GAP "3171:;" OBFH_SF_FRAME("frame_a"), 1) \
    B(context, OBFH_SF_ENTRY_PADDING("1") "cmpl %%edx, %%eax; jb 3174f;" OBFH_SF_CALL_AT("3172f", "1") "jmp 3175f; 3174:" OBFH_SF_CALL_AT("3173f", "2") "3175:" OBFH_SF_EPILOGUE OBFH_SF_GAP "3172:;" OBFH_SF_FRAME("frame_b"), 2) \
    C(context, OBFH_SF_ENTRY_PADDING("2") "leave; jmp 3173f;" OBFH_SF_GAP "3173:;" OBFH_SF_FRAME_ALT("frame_c"), 3) \
    D(context, OBFH_SF_ENTRY_PADDING("3") OBFH_SF_CALL_AT("3171b", "3") OBFH_SF_EPILOGUE_ALT, 0)
#define OBFH_SF_LAYOUT_17(a, b, c, d) OBFH_SF_LAYOUT_SOURCE_17(OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, (a, b, c, d, ""))
#define OBFH_SF_LAYOUT_SOURCE_18(A, B, C, D, E, F, G, H, I, context) \
    A(context, OBFH_SF_CALL_AT("3183f", "0") OBFH_SF_EPILOGUE OBFH_SF_GAP "3181:;" OBFH_SF_FRAME("frame_a"), 1) \
    B(context, OBFH_SF_ENTRY_PADDING("1") OBFH_SF_EPILOGUE OBFH_SF_GAP "3182:;" OBFH_SF_FRAME_ALT("frame_b"), 2) \
    C(context, OBFH_SF_ENTRY_PADDING("2") OBFH_SF_CALL_AT("3181b", "1") OBFH_SF_EPILOGUE_ALT OBFH_SF_GAP "3183:;" OBFH_SF_FRAME("frame_c"), 3) \
    D(context, OBFH_SF_ENTRY_PADDING("3") OBFH_SF_CALL_AT("3182b", "2") OBFH_SF_EPILOGUE, 0)
#define OBFH_SF_LAYOUT_18(a, b, c, d) OBFH_SF_LAYOUT_SOURCE_18(OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, (a, b, c, d, ""))
#define OBFH_SF_LAYOUT_SOURCE_19(A, B, C, D, E, F, G, H, I, context) \
    A(context, "testb $1, %%al; jz 3194f;" OBFH_SF_CALL_AT("3191f", "0") "jmp 3195f; 3194:" OBFH_SF_CALL_AT("3193f", "1") "3195:" OBFH_SF_EPILOGUE OBFH_SF_GAP "3191:;" OBFH_SF_FRAME("frame_a"), 1) \
    B(context, OBFH_SF_ENTRY_PADDING("1") OBFH_SF_CALL_AT("3192f", "2") OBFH_SF_EPILOGUE OBFH_SF_GAP "3192:;" OBFH_SF_FRAME_ALT("frame_b"), 2) \
    C(context, OBFH_SF_ENTRY_PADDING("2") OBFH_SF_EPILOGUE_ALT OBFH_SF_GAP "3193:;" OBFH_SF_FRAME("frame_c"), 3) \
    D(context, OBFH_SF_ENTRY_PADDING("3") OBFH_SF_CALL_AT("3192b", "3") OBFH_SF_EPILOGUE, 0)
#define OBFH_SF_LAYOUT_19(a, b, c, d) OBFH_SF_LAYOUT_SOURCE_19(OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, (a, b, c, d, ""))
#define OBFH_SF_LAYOUT_SOURCE_20(A, B, C, D, E, F, G, H, I, context) \
    A(context, OBFH_SF_CALL_AT("3201f", "0") OBFH_SF_EPILOGUE OBFH_SF_GAP "3202:;" OBFH_SF_FRAME("frame_b"), 2) \
    B(context, OBFH_SF_ENTRY_PADDING("2") OBFH_SF_CALL_AT("3208f", "1") OBFH_SF_EPILOGUE OBFH_SF_GAP "3208:;" OBFH_SF_FRAME_ALT("frame_d"), 4) \
    C(context, OBFH_SF_ENTRY_PADDING("8") OBFH_SF_CALL_AT("3203f", "2") OBFH_SF_EPILOGUE_ALT OBFH_SF_GAP "3201:;" OBFH_SF_FRAME("frame_a"), 1) \
    D(context, OBFH_SF_ENTRY_PADDING("1") "testb $1, %%al; jz 3204f;" OBFH_SF_CALL_AT("3202b", "3") "jmp 3205f; 3204:" OBFH_SF_CALL_AT("3203f", "4") "3205:" OBFH_SF_EPILOGUE OBFH_SF_GAP "3203:;" OBFH_SF_FRAME("frame_c"), 3) \
    E(context, OBFH_SF_ENTRY_PADDING("3") OBFH_SF_CALL_AT("3201b", "5") OBFH_SF_EPILOGUE, 0)
#define OBFH_SF_LAYOUT_20(a, b, c, d) OBFH_SF_LAYOUT_SOURCE_20(OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, (a, b, c, d, ""))
#define OBFH_SF_LAYOUT_SOURCE_21(A, B, C, D, E, F, G, H, I, context) \
    A(context, OBFH_SF_CALL_AT("3211f", "0") OBFH_SF_EPILOGUE OBFH_SF_GAP "3213:;" OBFH_SF_FRAME("frame_c"), 3) \
    B(context, OBFH_SF_ENTRY_PADDING("3") OBFH_SF_EPILOGUE OBFH_SF_GAP "3218:;" OBFH_SF_FRAME_ALT("frame_d"), 4) \
    C(context, OBFH_SF_ENTRY_PADDING("8") OBFH_SF_CALL_AT("3213b", "1") OBFH_SF_EPILOGUE_ALT OBFH_SF_GAP "3212:;" OBFH_SF_FRAME("frame_b"), 2) \
    D(context, OBFH_SF_ENTRY_PADDING("2") OBFH_SF_CALL_AT("3218b", "2") OBFH_SF_EPILOGUE OBFH_SF_GAP "3211:;" OBFH_SF_FRAME("frame_a"), 1) \
    E(context, OBFH_SF_ENTRY_PADDING("1") OBFH_SF_CALL_AT("3212b", "3") OBFH_SF_CALL_AT("3213b", "4") OBFH_SF_EPILOGUE, 0)
#define OBFH_SF_LAYOUT_21(a, b, c, d) OBFH_SF_LAYOUT_SOURCE_21(OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, (a, b, c, d, ""))
// Skipped native helpers: register-only and scratch-stack forms share no RBP frame.
// Their arithmetic/opcodes vary per expansion; all stack edits follow the live guard.
#if defined(__x86_64__)
#define OBFH_SF_LEAF_ARGS OBFH_SF_ARGS ".fill (%c[phantom] & 1), 1, 0x90;"
#define OBFH_SF_LEAF_ENTER "subq $.Lsl_frame, %%rsp;"
#define OBFH_SF_LEAF_STORE "movl %%eax, .Lsl_slot(%%rsp);"
#define OBFH_SF_LEAF_LOAD "movl .Lsl_slot(%%rsp), %%ecx;"
#define OBFH_SF_LEAF_EXIT "addq $.Lsl_frame, %%rsp; ret;"
#else
#define OBFH_SF_LEAF_ARGS "movl 4(%%esp), %%eax; movl 8(%%esp), %%edx; .fill (%c[phantom] & 1), 1, 0x90;"
#define OBFH_SF_LEAF_ENTER "subl $.Lsl_frame, %%esp;"
#define OBFH_SF_LEAF_STORE "movl %%eax, .Lsl_slot(%%esp);"
#define OBFH_SF_LEAF_LOAD "movl .Lsl_slot(%%esp), %%ecx;"
#define OBFH_SF_LEAF_EXIT "addl $.Lsl_frame, %%esp; ret;"
#endif
// Shared leaf immediates emit no bytes; each native insertion initializes them once.
#define OBFH_SF_LEAF_CONSTANTS ".set .Lsl_frame,%c[leaf_frame];.set .Lsl_slot,%c[leaf_slot];.set .Lsl_alu_a,%c[leaf_alu_a];.set .Lsl_alu_b,%c[leaf_alu_b];.set .Lsl_shift,%c[leaf_shift];"
#define OBFH_SF_LEAF_ALU_A ".byte .Lsl_alu_a; .long .Lsa;"
#define OBFH_SF_LEAF_ALU_B ".byte 0x81, .Lsl_alu_b; .long .Lsb;"
#define OBFH_SF_LEAF_SHIFT ".byte (0xc1 + 0x10 * ((.Lsr == 1) & 1)), .Lsl_shift; .fill ((.Lsr != 1) & 1), 1, .Lsr;"
// Reverse CMP operands together with its condition. Only these bounded leaf
// branches use a rel8 displacement and must overwrite flags on both paths.
// A/L retain their CMP: their ALU_B can consume its carry through ADC/SBB.
#define OBFH_SF_LEAF_COMPARE(reg, cc, reversed, bit, skip) ".short 0x39+256*(0xc0+8*" reg "+0)+((%c[phantom]>>" bit ")&1)*1792*(0-" reg "); .byte " cc "+((%c[phantom]>>" bit ")&1)*(" reversed "-" cc "); .byte " skip ";"
// Both pairs compute EDX-EAX modulo 2^32 in four bytes. ALU_A replaces flags.
#define OBFH_SF_LEAF_REVERSE_SUB ".long 0xd8f7d029+((%c[phantom]>>13)&1)*(0xd001d8f7-0xd8f7d029);"
#define OBFH_SF_LEAF_CORE_A OBFH_SF_LEAF_ALU_A "cmpl %%edx, %%eax; jbe 6f;" OBFH_SF_LEAF_SHIFT "subl %%edx, %%eax; ret; 6:" OBFH_SF_LEAF_ALU_B "leal (%%eax, %%edx, 2), %%eax; ret;"
#define OBFH_SF_LEAF_A OBFH_SF_LEAF_ARGS OBFH_SF_LEAF_CORE_A
#define OBFH_SF_LEAF_INNER_B OBFH_SF_LEAF_ALU_A OBFH_SF_LEAF_SHIFT OBFH_SF_LEAF_LOAD "xorl %%ecx, %%eax; addl %%edx, %%eax;"
#define OBFH_SF_LEAF_CORE_B OBFH_SF_LEAF_ENTER OBFH_SF_LEAF_STORE OBFH_SF_LEAF_INNER_B OBFH_SF_LEAF_EXIT
#define OBFH_SF_LEAF_B OBFH_SF_LEAF_ARGS OBFH_SF_LEAF_CORE_B
#define OBFH_SF_LEAF_INNER_C "movl %%edx, %%ecx; roll $%c[rotate], %%ecx;" OBFH_SF_LEAF_COMPARE("1", "0x7d", "0x7e", "11", "9") OBFH_SF_LEAF_ALU_A "xorl %%ecx, %%eax; .byte 0xeb,(4 + ((.Lsr != 1) & 1)); 6:" OBFH_SF_LEAF_SHIFT "addl %%edx, %%eax; 7:" OBFH_SF_LEAF_LOAD "subl %%ecx, %%eax;"
#define OBFH_SF_LEAF_CORE_C OBFH_SF_LEAF_ENTER OBFH_SF_LEAF_STORE OBFH_SF_LEAF_INNER_C OBFH_SF_LEAF_EXIT
#define OBFH_SF_LEAF_C OBFH_SF_LEAF_ARGS OBFH_SF_LEAF_CORE_C
#define OBFH_SF_LEAF_CORE_D "movl $%c[loops], %%ecx; 6:" OBFH_SF_LEAF_ALU_A OBFH_SF_LEAF_SHIFT OBFH_SF_LEAF_ALU_B "addl %%edx, %%eax; decl %%ecx; jnz 6b; ret;"
#define OBFH_SF_LEAF_D OBFH_SF_LEAF_ARGS OBFH_SF_LEAF_CORE_D
#define OBFH_SF_LEAF_CORE_E "cmpl $%c[key_a], %%eax; jb 6f; bswap %%eax;" OBFH_SF_LEAF_ALU_A "ret; 6: movl $%c[factor], %%ecx; xorl %%edx, %%edx; divl %%ecx; xorl %%edx, %%eax;" OBFH_SF_LEAF_SHIFT "ret;"
#define OBFH_SF_LEAF_E OBFH_SF_LEAF_ARGS OBFH_SF_LEAF_CORE_E
#define OBFH_SF_LEAF_INNER_F OBFH_SF_LEAF_REVERSE_SUB OBFH_SF_LEAF_ALU_A OBFH_SF_LEAF_LOAD "xorl %%ecx, %%eax;" OBFH_SF_LEAF_ALU_B "adcl %%edx, %%eax;"
#define OBFH_SF_LEAF_CORE_F OBFH_SF_LEAF_ENTER OBFH_SF_LEAF_STORE OBFH_SF_LEAF_INNER_F OBFH_SF_LEAF_EXIT
#define OBFH_SF_LEAF_F OBFH_SF_LEAF_ARGS OBFH_SF_LEAF_CORE_F
#define OBFH_SF_LEAF_CORE_G "movl %%eax, %%ecx;" OBFH_SF_LEAF_SHIFT "testl $%c[mask], %%ecx; jnz 6f;" OBFH_SF_LEAF_ALU_A "ret; 6: imull $%c[factor], %%eax;" OBFH_SF_LEAF_ALU_B "xorl %%edx, %%eax; ret;"
#define OBFH_SF_LEAF_G OBFH_SF_LEAF_ARGS OBFH_SF_LEAF_CORE_G
#define OBFH_SF_LEAF_INNER_H "movl $%c[loops], %%ecx; 6:" OBFH_SF_LEAF_SHIFT "addl %%edx, %%eax; decl %%ecx; jnz 6b;" OBFH_SF_LEAF_LOAD "xorl %%ecx, %%eax;"
#define OBFH_SF_LEAF_CORE_H OBFH_SF_LEAF_ENTER OBFH_SF_LEAF_STORE OBFH_SF_LEAF_INNER_H OBFH_SF_LEAF_EXIT
#define OBFH_SF_LEAF_H OBFH_SF_LEAF_ARGS OBFH_SF_LEAF_CORE_H
#define OBFH_SF_LEAF_CORE_I "bswap %%eax;" OBFH_SF_LEAF_ALU_A "cmpl %%edx, %%eax; jne 6f;" OBFH_SF_LEAF_SHIFT "ret; 6: imull $%c[factor], %%eax; subl %%edx, %%eax; ret;"
#define OBFH_SF_LEAF_I OBFH_SF_LEAF_ARGS OBFH_SF_LEAF_CORE_I
#define OBFH_SF_LEAF_INNER_J OBFH_SF_LEAF_ALU_B "testl %%edx, %%edx; js 6f;" OBFH_SF_LEAF_SHIFT ".byte 0xeb,2; 6: negl %%eax; 7:" OBFH_SF_LEAF_LOAD "addl %%ecx, %%eax;"
#define OBFH_SF_LEAF_CORE_J OBFH_SF_LEAF_ENTER OBFH_SF_LEAF_STORE OBFH_SF_LEAF_INNER_J OBFH_SF_LEAF_EXIT
#define OBFH_SF_LEAF_J OBFH_SF_LEAF_ARGS OBFH_SF_LEAF_CORE_J
#define OBFH_SF_LEAF_CORE_K "movzbl %%al, %%ecx; shrl $8, %%eax; imull $%c[factor], %%ecx; xorl %%ecx, %%eax;" OBFH_SF_LEAF_ALU_A OBFH_SF_LEAF_SHIFT "addl %%edx, %%eax; ret;"
#define OBFH_SF_LEAF_K OBFH_SF_LEAF_ARGS OBFH_SF_LEAF_CORE_K
#define OBFH_SF_LEAF_INNER_L "cmpl %%edx, %%eax; jbe 6f;" OBFH_SF_LEAF_ALU_A ".byte 0xeb,8; 6:" OBFH_SF_LEAF_ALU_B "xorl %%edx, %%eax; 7:" OBFH_SF_LEAF_LOAD "subl %%ecx, %%eax;"
#define OBFH_SF_LEAF_CORE_L OBFH_SF_LEAF_ENTER OBFH_SF_LEAF_STORE OBFH_SF_LEAF_INNER_L OBFH_SF_LEAF_EXIT
#define OBFH_SF_LEAF_L OBFH_SF_LEAF_ARGS OBFH_SF_LEAF_CORE_L
#define OBFH_SF_LEAF_ENTRY(label, body) label ": " body
#define OBFH_SF_LAYOUT_SOURCE_22(A, B, C, D, E, F, G, H, I, context) \
    A(context, OBFH_SF_CALL_AT("3221f", "0") OBFH_SF_EPILOGUE OBFH_SF_GAP "3222:;", 3) \
    B(context, OBFH_SF_GAP "2220:;3221:;" OBFH_SF_FRAME("frame_a"), \
      1) \
    C(context, OBFH_SF_ENTRY_PADDING("1") OBFH_SF_CALL_AT("3222b", "1") OBFH_SF_EPILOGUE_ALT, 0)
#define OBFH_SF_LAYOUT_22(a, b, c, d) OBFH_SF_LAYOUT_SOURCE_22(OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, (a, b, c, d, ""))
#define OBFH_SF_LAYOUT_SOURCE_23(A, B, C, D, E, F, G, H, I, context) \
    A(context, OBFH_SF_CALL_AT("3231f", "0") OBFH_SF_EPILOGUE_ALT OBFH_SF_GAP "2230:;3231:;", \
      3) \
    B(context, OBFH_SF_GAP "3232:;", 4) \
    C(context, "", 0)
#define OBFH_SF_LAYOUT_23(a, b, c, d) OBFH_SF_LAYOUT_SOURCE_23(OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, (a, b, c, d, ""))
#define OBFH_SF_LAYOUT_SOURCE_24(A, B, C, D, E, F, G, H, I, context) \
    A(context, OBFH_SF_CALL_AT("3241f", "0") OBFH_SF_EPILOGUE OBFH_SF_GAP "2240:;3241:;" OBFH_SF_FRAME_ALT("frame_a"), \
      1) \
    B(context, OBFH_SF_ENTRY_PADDING("1") OBFH_SF_CALL_AT("3243f", "1") OBFH_SF_CALL_AT("3242f", "2") OBFH_SF_EPILOGUE_ALT OBFH_SF_GAP "3242:;", 3) \
    C(context, OBFH_SF_GAP "3243:;", 4) \
    D(context, "", 0)
#define OBFH_SF_LAYOUT_24(a, b, c, d) OBFH_SF_LAYOUT_SOURCE_24(OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, (a, b, c, d, ""))
#define OBFH_SF_LAYOUT_SOURCE_25(A, B, C, D, E, F, G, H, I, context) \
    A(context, OBFH_SF_CALL_AT("3251f", "0") OBFH_SF_EPILOGUE_ALT OBFH_SF_GAP "3253:;", 4) \
    B(context, OBFH_SF_GAP "2250:;3251:;" OBFH_SF_FRAME("frame_a"), \
      1) \
    C(context, OBFH_SF_ENTRY_PADDING("1") OBFH_SF_CALL_AT("3252f", "1") OBFH_SF_EPILOGUE OBFH_SF_GAP "3252:;" OBFH_SF_FRAME_ALT("frame_b"), 2) \
    D(context, OBFH_SF_ENTRY_PADDING("2") OBFH_SF_CALL_AT("3253b", "2") OBFH_SF_EPILOGUE_ALT, 0)
#define OBFH_SF_LAYOUT_25(a, b, c, d) OBFH_SF_LAYOUT_SOURCE_25(OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, (a, b, c, d, ""))
#define OBFH_SF_LAYOUT_SOURCE_26(A, B, C, D, E, F, G, H, I, context) \
    A(context, OBFH_SF_CALL_AT("3261f", "0") OBFH_SF_EPILOGUE OBFH_SF_GAP "3262:;", 3) \
    B(context, OBFH_SF_GAP "2260:;3261:;" OBFH_SF_FRAME_ALT("frame_a"), \
      1) \
    C(context, OBFH_SF_ENTRY_PADDING("1") "testl %%eax, %%eax; jz 3264f;" OBFH_SF_CALL_AT("3262b", "1") "3264:" OBFH_SF_EPILOGUE_ALT OBFH_SF_GAP "3263:;", 4) \
    D(context, "", 0)
#define OBFH_SF_LAYOUT_26(a, b, c, d) OBFH_SF_LAYOUT_SOURCE_26(OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, (a, b, c, d, ""))
#define OBFH_SF_LAYOUT_SOURCE_27(A, B, C, D, E, F, G, H, I, context) \
    A(context, OBFH_SF_CALL_AT("3272f", "0") OBFH_SF_EPILOGUE_ALT OBFH_SF_GAP "3271:;", 3) \
    B(context, OBFH_SF_GAP "3273:;", 4) \
    C(context, OBFH_SF_GAP "3272:;" OBFH_SF_FRAME("frame_b"), 2) \
    D(context, OBFH_SF_ENTRY_PADDING("2") OBFH_SF_CALL_AT("3271b", "1") OBFH_SF_CALL_AT("3273b", "2") OBFH_SF_EPILOGUE, 0)
#define OBFH_SF_LAYOUT_27(a, b, c, d) OBFH_SF_LAYOUT_SOURCE_27(OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, (a, b, c, d, ""))
#define OBFH_SF_LAYOUT_SOURCE_28(A, B, C, D, E, F, G, H, I, context) \
    A(context, OBFH_SF_CALL_AT("3281f", "0") OBFH_SF_EPILOGUE OBFH_SF_GAP "3282:;", 3) \
    B(context, OBFH_SF_GAP "2280:;3281:;" OBFH_SF_FRAME("frame_a"), \
      1) \
    C(context, OBFH_SF_ENTRY_PADDING("1") OBFH_SF_CALL_AT("3282b", "1") OBFH_SF_CALL_AT("3283f", "2") OBFH_SF_EPILOGUE_ALT OBFH_SF_GAP "3283:;" OBFH_SF_FRAME_ALT("frame_b"), 2) \
    D(context, OBFH_SF_ENTRY_PADDING("3") OBFH_SF_CALL_AT("3288f", "3") OBFH_SF_CALL_AT("32810f", "4") OBFH_SF_CALL_AT("32811f", "5") OBFH_SF_EPILOGUE OBFH_SF_GAP "3288:;", 4) \
    E(context, OBFH_SF_GAP "32810:;", 3) \
    F(context, OBFH_SF_GAP "32811:;" OBFH_SF_FRAME("frame_d"), 5) \
    G(context, OBFH_SF_ENTRY_PADDING("11") OBFH_SF_CALL_AT("32813f", "1") OBFH_SF_CALL_AT("32812f", "2") OBFH_SF_EPILOGUE_ALT OBFH_SF_GAP "32812:;", 4) \
    H(context, OBFH_SF_GAP "32813:;", 3) \
    I(context, "", 0)
#define OBFH_SF_LAYOUT_28(a, b, c, d, repeated_a) OBFH_SF_LAYOUT_SOURCE_28(OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, OBFH_SF_SLOT_REFERENCE, (a, b, c, d, repeated_a))
#define OBFH_SF_DECLARE_ROLES(index) enum { __obfh_sf_roles_##index = ((0 OBFH_SF_SLOT_0(OBFH_SF_LAYOUT_SOURCE_##index, OBFH_SF_SLOT_ROLE, unused)) << 0) | ((0 OBFH_SF_SLOT_1(OBFH_SF_LAYOUT_SOURCE_##index, OBFH_SF_SLOT_ROLE, unused)) << 3) | ((0 OBFH_SF_SLOT_2(OBFH_SF_LAYOUT_SOURCE_##index, OBFH_SF_SLOT_ROLE, unused)) << 6) | ((0 OBFH_SF_SLOT_3(OBFH_SF_LAYOUT_SOURCE_##index, OBFH_SF_SLOT_ROLE, unused)) << 9) | ((0 OBFH_SF_SLOT_4(OBFH_SF_LAYOUT_SOURCE_##index, OBFH_SF_SLOT_ROLE, unused)) << 12) | ((0 OBFH_SF_SLOT_5(OBFH_SF_LAYOUT_SOURCE_##index, OBFH_SF_SLOT_ROLE, unused)) << 15) | ((0 OBFH_SF_SLOT_6(OBFH_SF_LAYOUT_SOURCE_##index, OBFH_SF_SLOT_ROLE, unused)) << 18) | ((0 OBFH_SF_SLOT_7(OBFH_SF_LAYOUT_SOURCE_##index, OBFH_SF_SLOT_ROLE, unused)) << 21) | ((0 OBFH_SF_SLOT_8(OBFH_SF_LAYOUT_SOURCE_##index, OBFH_SF_SLOT_ROLE, unused)) << 24) };
// Compiler-only constants shorten repeated selector arithmetic in ASM strings.
// Each insertion defines them before its fragments; no bytes or runtime state.
// .LspN is phantom bit N; .Lsl[e/z/n]N selects link enable/condition/register.
#define OBFH_SF_ASM_BITS \
    ".set .Lsp,%c[phantom];.set .Lsp0,(.Lsp>>0)&1;.set .Lsp6,(.Lsp>>6)&1;.set .Lsp7,(.Lsp>>7)&1;.set .Lsp8,(.Lsp>>8)&1;.set .Lsp9,(.Lsp>>9)&1;.set .Lsp10,(.Lsp>>10)&1;.set .Lsp11,(.Lsp>>11)&1;.set .Lsp12,(.Lsp>>12)&1;.set .Lsp13,(.Lsp>>13)&1;.set .Lsp14,(.Lsp>>14)&1;.set .Lsp15,(.Lsp>>15)&1;.set .Lsp16,(.Lsp>>16)&1;"
OBFH_SF_DECLARE_ROLES(0);
OBFH_SF_DECLARE_ROLES(1);
OBFH_SF_DECLARE_ROLES(2);
OBFH_SF_DECLARE_ROLES(3);
OBFH_SF_DECLARE_ROLES(4);
OBFH_SF_DECLARE_ROLES(5);
OBFH_SF_DECLARE_ROLES(6);
OBFH_SF_DECLARE_ROLES(7);
OBFH_SF_DECLARE_ROLES(8);
OBFH_SF_DECLARE_ROLES(9);
OBFH_SF_DECLARE_ROLES(22);
OBFH_SF_DECLARE_ROLES(23);
OBFH_SF_DECLARE_ROLES(24);
OBFH_SF_DECLARE_ROLES(25);
OBFH_SF_DECLARE_ROLES(26);
OBFH_SF_DECLARE_ROLES(28);
#define OBFH_SF_ASM_CONSTANTS \
    ".set .Lss_frame_a,%c[frame_a];.set .Lss_frame_b,%c[frame_b];.set .Lss_frame_c,%c[frame_c];.set .Lss_frame_d,%c[frame_d];.set .Lss_arg_a,%c[arg_a];.set .Lss_arg_b,%c[arg_b];.set .Lss_pad,%c[pad];.set .Lss_pad_b,%c[pad_b];.set .Lss_noise_a,%c[noise_a];.set .Lss_noise_b,%c[noise_b];" OBFH_SF_ASM_BITS ".set .Lsa,%c[key_a];.set .Lsle0,((.Lsp^.Lsa)>>12)&1;.set .Lslz0,((.Lsp^.Lsa)>>13)&1;.set .Lsln0,((.Lsp^.Lsa)>>14)&1;.set .Lsle1,((.Lsp^.Lsa)>>15)&1;.set .Lslz1,((.Lsp^.Lsa)>>16)&1;.set .Lsln1,((.Lsp^.Lsa)>>17)&1;.set .Lsle2,((.Lsp^.Lsa)>>18)&1;.set .Lslz2,((.Lsp^.Lsa)>>19)&1;.set .Lsln2,((.Lsp^.Lsa)>>20)&1;.set .Lsle3,((.Lsp^.Lsa)>>21)&1;.set .Lslz3,((.Lsp^.Lsa)>>22)&1;.set .Lsln3,((.Lsp^.Lsa)>>23)&1;.set .Lsle4,((.Lsp^.Lsa)>>24)&1;.set .Lslz4,((.Lsp^.Lsa)>>25)&1;.set .Lsln4,((.Lsp^.Lsa)>>26)&1;.set .Lsle5,((.Lsp^.Lsa)>>27)&1;.set .Lslz5,((.Lsp^.Lsa)>>28)&1;.set .Lsln5,((.Lsp^.Lsa)>>29)&1;" OBFH_SF_SPELLING_CONSTANTS OBFH_SF_LEAF_CONSTANTS OBFH_SF_FRAME_CONSTANTS("frame_a") OBFH_SF_FRAME_CONSTANTS("frame_b") OBFH_SF_FRAME_CONSTANTS("frame_c") OBFH_SF_FRAME_CONSTANTS("frame_d") OBFH_SF_LINK_CONSTANT("0") OBFH_SF_LINK_CONSTANT("1") OBFH_SF_LINK_CONSTANT("2") OBFH_SF_LINK_CONSTANT("3") OBFH_SF_LINK_CONSTANT("4") OBFH_SF_LINK_CONSTANT("5") OBFH_SF_CALL_CONSTANTS
#define OBFH_SF_ROLES_SELECT __builtin_choose_expr((__obfh_pool_layout) <= 7u, __builtin_choose_expr((__obfh_pool_layout) <= 3u, __builtin_choose_expr((__obfh_pool_layout) <= 1u, __builtin_choose_expr((__obfh_pool_layout) <= 0u, __obfh_sf_roles_0, __obfh_sf_roles_1), __builtin_choose_expr((__obfh_pool_layout) <= 2u, __obfh_sf_roles_2, __obfh_sf_roles_3)), __builtin_choose_expr((__obfh_pool_layout) <= 5u, __builtin_choose_expr((__obfh_pool_layout) <= 4u, __obfh_sf_roles_4, __obfh_sf_roles_5), __builtin_choose_expr((__obfh_pool_layout) <= 6u, __obfh_sf_roles_6, __obfh_sf_roles_7))), __builtin_choose_expr((__obfh_pool_layout) <= 23u, __builtin_choose_expr((__obfh_pool_layout) <= 9u, __builtin_choose_expr((__obfh_pool_layout) <= 8u, __obfh_sf_roles_8, __obfh_sf_roles_9), __builtin_choose_expr((__obfh_pool_layout) <= 22u, __obfh_sf_roles_22, __obfh_sf_roles_23)), __builtin_choose_expr((__obfh_pool_layout) <= 25u, __builtin_choose_expr((__obfh_pool_layout) <= 24u, __obfh_sf_roles_24, __obfh_sf_roles_25), __builtin_choose_expr((__obfh_pool_layout) <= 26u, __obfh_sf_roles_26, __obfh_sf_roles_28))))
// Select a compact compile-time recipe before expanding native assembly.
// The recipe is source metadata only: all native families remain independent.
#define OBFH_SF_ID_BODY_A 0u
#define OBFH_SF_ID_BODY_B 1u
#define OBFH_SF_ID_BODY_C 2u
#define OBFH_SF_ID_BODY_D 3u
#define OBFH_SF_ID_BODY_E 4u
#define OBFH_SF_ID_BODY_F 5u
#define OBFH_SF_ID_BODY_G 6u
#define OBFH_SF_ID_BODY_H 7u
#define OBFH_SF_ID_BODY_I 8u
#define OBFH_SF_ID_BODY_J 9u
#define OBFH_SF_ID_BODY_K 10u
#define OBFH_SF_ID_BODY_L 11u
#define OBFH_SF_ID_BODY_M 12u
#define OBFH_SF_ID_BODY_N 13u
#define OBFH_SF_ID_BODY_O 14u
#define OBFH_SF_ID_BODY_P 15u
#define OBFH_SF_ID_BODY_Q 16u
#define OBFH_SF_ID_BODY_R 17u
#define OBFH_SF_ID_BODY_S 18u
#define OBFH_SF_ID_BODY_T 19u
#define OBFH_SF_ID_LEAF_A 20u
#define OBFH_SF_ID_LEAF_B 21u
#define OBFH_SF_ID_LEAF_C 22u
#define OBFH_SF_ID_LEAF_D 23u
#define OBFH_SF_ID_LEAF_E 24u
#define OBFH_SF_ID_LEAF_F 25u
#define OBFH_SF_ID_LEAF_G 26u
#define OBFH_SF_ID_LEAF_H 27u
#define OBFH_SF_ID_LEAF_I 28u
#define OBFH_SF_ID_LEAF_J 29u
#define OBFH_SF_ID_LEAF_K 30u
#define OBFH_SF_ID_LEAF_L 31u
#define OBFH_SF_RECIPE(g, l, a, b, c, d) ((g) | ((l) << 4) | (OBFH_SF_ID_##a << 9) | (OBFH_SF_ID_##b << 14) | (OBFH_SF_ID_##c << 19) | (OBFH_SF_ID_##d << 24))
#define OBFH_SF_SPEC_0(emit) emit(0u, 0u, BODY_A, BODY_B, BODY_C, BODY_A)
#define OBFH_SF_VARIANT_0 OBFH_SF_SELECT(0u)
#define OBFH_SF_SPEC_1(emit) emit(0u, 1u, BODY_B, BODY_C, BODY_D, BODY_A)
#define OBFH_SF_VARIANT_1 OBFH_SF_SELECT(1u)
#define OBFH_SF_SPEC_2(emit) emit(0u, 2u, BODY_C, BODY_D, BODY_A, BODY_A)
#define OBFH_SF_VARIANT_2 OBFH_SF_SELECT(2u)
#define OBFH_SF_SPEC_3(emit) emit(0u, 3u, BODY_D, BODY_A, BODY_B, BODY_A)
#define OBFH_SF_VARIANT_3 OBFH_SF_SELECT(3u)
#define OBFH_SF_SPEC_4(emit) emit(1u, 0u, BODY_A, BODY_B, BODY_C, BODY_A)
#define OBFH_SF_VARIANT_4 OBFH_SF_SELECT(4u)
#define OBFH_SF_SPEC_5(emit) emit(1u, 1u, BODY_B, BODY_C, BODY_D, BODY_A)
#define OBFH_SF_VARIANT_5 OBFH_SF_SELECT(5u)
#define OBFH_SF_SPEC_6(emit) emit(1u, 2u, BODY_C, BODY_D, BODY_A, BODY_A)
#define OBFH_SF_VARIANT_6 OBFH_SF_SELECT(6u)
#define OBFH_SF_SPEC_7(emit) emit(1u, 3u, BODY_D, BODY_A, BODY_B, BODY_A)
#define OBFH_SF_VARIANT_7 OBFH_SF_SELECT(7u)
#define OBFH_SF_SPEC_8(emit) emit(2u, 0u, BODY_A, BODY_B, BODY_C, BODY_A)
#define OBFH_SF_VARIANT_8 OBFH_SF_SELECT(8u)
#define OBFH_SF_SPEC_9(emit) emit(2u, 1u, BODY_B, BODY_C, BODY_D, BODY_A)
#define OBFH_SF_VARIANT_9 OBFH_SF_SELECT(9u)
#define OBFH_SF_SPEC_10(emit) emit(2u, 2u, BODY_C, BODY_D, BODY_A, BODY_A)
#define OBFH_SF_VARIANT_10 OBFH_SF_SELECT(10u)
#define OBFH_SF_SPEC_11(emit) emit(2u, 3u, BODY_D, BODY_A, BODY_B, BODY_A)
#define OBFH_SF_VARIANT_11 OBFH_SF_SELECT(11u)
#define OBFH_SF_SPEC_12(emit) emit(3u, 0u, BODY_A, BODY_B, BODY_C, BODY_A)
#define OBFH_SF_VARIANT_12 OBFH_SF_SELECT(12u)
#define OBFH_SF_SPEC_13(emit) emit(3u, 1u, BODY_B, BODY_C, BODY_D, BODY_A)
#define OBFH_SF_VARIANT_13 OBFH_SF_SELECT(13u)
#define OBFH_SF_SPEC_14(emit) emit(3u, 2u, BODY_C, BODY_D, BODY_A, BODY_A)
#define OBFH_SF_VARIANT_14 OBFH_SF_SELECT(14u)
#define OBFH_SF_SPEC_15(emit) emit(3u, 3u, BODY_D, BODY_A, BODY_B, BODY_A)
#define OBFH_SF_VARIANT_15 OBFH_SF_SELECT(15u)
#define OBFH_SF_SPEC_16(emit) emit(4u, 4u, BODY_E, BODY_J, BODY_B, BODY_A)
#define OBFH_SF_VARIANT_16 OBFH_SF_SELECT(16u)
#define OBFH_SF_SPEC_17(emit) emit(4u, 5u, BODY_F, BODY_K, BODY_C, BODY_A)
#define OBFH_SF_VARIANT_17 OBFH_SF_SELECT(17u)
#define OBFH_SF_SPEC_18(emit) emit(4u, 6u, BODY_G, BODY_L, BODY_D, BODY_A)
#define OBFH_SF_VARIANT_18 OBFH_SF_SELECT(18u)
#define OBFH_SF_SPEC_19(emit) emit(4u, 7u, BODY_H, BODY_A, BODY_E, BODY_A)
#define OBFH_SF_VARIANT_19 OBFH_SF_SELECT(19u)
#define OBFH_SF_SPEC_20(emit) emit(4u, 8u, BODY_I, BODY_B, BODY_F, BODY_A)
#define OBFH_SF_VARIANT_20 OBFH_SF_SELECT(20u)
#define OBFH_SF_SPEC_21(emit) emit(4u, 9u, BODY_J, BODY_C, BODY_G, BODY_A)
#define OBFH_SF_VARIANT_21 OBFH_SF_SELECT(21u)
#define OBFH_SF_SPEC_22(emit) emit(5u, 4u, BODY_K, BODY_D, BODY_H, BODY_A)
#define OBFH_SF_VARIANT_22 OBFH_SF_SELECT(22u)
#define OBFH_SF_SPEC_23(emit) emit(5u, 5u, BODY_L, BODY_E, BODY_I, BODY_A)
#define OBFH_SF_VARIANT_23 OBFH_SF_SELECT(23u)
#define OBFH_SF_SPEC_24(emit) emit(5u, 6u, BODY_A, BODY_F, BODY_J, BODY_A)
#define OBFH_SF_VARIANT_24 OBFH_SF_SELECT(24u)
#define OBFH_SF_SPEC_25(emit) emit(5u, 7u, BODY_B, BODY_G, BODY_K, BODY_A)
#define OBFH_SF_VARIANT_25 OBFH_SF_SELECT(25u)
#define OBFH_SF_SPEC_26(emit) emit(5u, 8u, BODY_C, BODY_H, BODY_L, BODY_A)
#define OBFH_SF_VARIANT_26 OBFH_SF_SELECT(26u)
#define OBFH_SF_SPEC_27(emit) emit(5u, 9u, BODY_D, BODY_I, BODY_A, BODY_A)
#define OBFH_SF_VARIANT_27 OBFH_SF_SELECT(27u)
#define OBFH_SF_SPEC_28(emit) emit(6u, 4u, BODY_E, BODY_J, BODY_B, BODY_A)
#define OBFH_SF_VARIANT_28 OBFH_SF_SELECT(28u)
#define OBFH_SF_SPEC_29(emit) emit(6u, 5u, BODY_F, BODY_K, BODY_C, BODY_A)
#define OBFH_SF_VARIANT_29 OBFH_SF_SELECT(29u)
#define OBFH_SF_SPEC_30(emit) emit(6u, 6u, BODY_G, BODY_L, BODY_D, BODY_A)
#define OBFH_SF_VARIANT_30 OBFH_SF_SELECT(30u)
#define OBFH_SF_SPEC_31(emit) emit(6u, 7u, BODY_H, BODY_A, BODY_E, BODY_A)
#define OBFH_SF_VARIANT_31 OBFH_SF_SELECT(31u)
#define OBFH_SF_SPEC_32(emit) emit(6u, 8u, BODY_I, BODY_B, BODY_F, BODY_A)
#define OBFH_SF_VARIANT_32 OBFH_SF_SELECT(32u)
#define OBFH_SF_SPEC_33(emit) emit(6u, 9u, BODY_J, BODY_C, BODY_G, BODY_A)
#define OBFH_SF_VARIANT_33 OBFH_SF_SELECT(33u)
#define OBFH_SF_SPEC_34(emit) emit(7u, 4u, BODY_K, BODY_D, BODY_H, BODY_A)
#define OBFH_SF_VARIANT_34 OBFH_SF_SELECT(34u)
#define OBFH_SF_SPEC_35(emit) emit(7u, 5u, BODY_L, BODY_E, BODY_I, BODY_A)
#define OBFH_SF_VARIANT_35 OBFH_SF_SELECT(35u)
#define OBFH_SF_SPEC_36(emit) emit(7u, 6u, BODY_A, BODY_F, BODY_J, BODY_A)
#define OBFH_SF_VARIANT_36 OBFH_SF_SELECT(36u)
#define OBFH_SF_SPEC_37(emit) emit(7u, 7u, BODY_B, BODY_G, BODY_K, BODY_A)
#define OBFH_SF_VARIANT_37 OBFH_SF_SELECT(37u)
#define OBFH_SF_SPEC_38(emit) emit(7u, 8u, BODY_C, BODY_H, BODY_L, BODY_A)
#define OBFH_SF_VARIANT_38 OBFH_SF_SELECT(38u)
#define OBFH_SF_SPEC_39(emit) emit(7u, 9u, BODY_D, BODY_I, BODY_A, BODY_A)
#define OBFH_SF_VARIANT_39 OBFH_SF_SELECT(39u)
#define OBFH_SF_SPEC_40(emit) emit(8u, 4u, BODY_E, BODY_J, BODY_B, BODY_A)
#define OBFH_SF_VARIANT_40 OBFH_SF_SELECT(40u)
#define OBFH_SF_SPEC_41(emit) emit(8u, 5u, BODY_F, BODY_K, BODY_C, BODY_A)
#define OBFH_SF_VARIANT_41 OBFH_SF_SELECT(41u)
#define OBFH_SF_SPEC_42(emit) emit(8u, 6u, BODY_G, BODY_L, BODY_D, BODY_A)
#define OBFH_SF_VARIANT_42 OBFH_SF_SELECT(42u)
#define OBFH_SF_SPEC_43(emit) emit(8u, 7u, BODY_H, BODY_A, BODY_E, BODY_A)
#define OBFH_SF_VARIANT_43 OBFH_SF_SELECT(43u)
#define OBFH_SF_SPEC_44(emit) emit(8u, 8u, BODY_I, BODY_B, BODY_F, BODY_A)
#define OBFH_SF_VARIANT_44 OBFH_SF_SELECT(44u)
#define OBFH_SF_SPEC_45(emit) emit(8u, 9u, BODY_J, BODY_C, BODY_G, BODY_A)
#define OBFH_SF_VARIANT_45 OBFH_SF_SELECT(45u)
#define OBFH_SF_SPEC_46(emit) emit(0u, 22u, BODY_A, BODY_H, LEAF_A, LEAF_E)
#define OBFH_SF_VARIANT_46 OBFH_SF_SELECT(46u)
#define OBFH_SF_SPEC_47(emit) emit(0u, 23u, BODY_D, BODY_K, LEAF_F, LEAF_L)
#define OBFH_SF_VARIANT_47 OBFH_SF_SELECT(47u)
#define OBFH_SF_SPEC_48(emit) emit(0u, 24u, BODY_G, BODY_N, LEAF_K, LEAF_G)
#define OBFH_SF_VARIANT_48 OBFH_SF_SELECT(48u)
#define OBFH_SF_SPEC_49(emit) emit(0u, 25u, BODY_J, BODY_Q, LEAF_D, LEAF_C)
#define OBFH_SF_VARIANT_49 OBFH_SF_SELECT(49u)
#define OBFH_SF_SPEC_50(emit) emit(0u, 26u, BODY_M, BODY_T, LEAF_I, LEAF_J)
#define OBFH_SF_VARIANT_50 OBFH_SF_SELECT(50u)
#define OBFH_SF_SPEC_51(emit) emit(0u, 28u, BODY_P, BODY_C, LEAF_B, LEAF_E)
#define OBFH_SF_VARIANT_51 OBFH_SF_SELECT(51u)
#define OBFH_SF_SPEC_52(emit) emit(1u, 22u, BODY_S, BODY_F, LEAF_H, LEAF_A)
#define OBFH_SF_VARIANT_52 OBFH_SF_SELECT(52u)
#define OBFH_SF_SPEC_53(emit) emit(1u, 23u, BODY_B, BODY_I, LEAF_A, LEAF_H)
#define OBFH_SF_VARIANT_53 OBFH_SF_SELECT(53u)
#define OBFH_SF_SPEC_54(emit) emit(1u, 24u, BODY_E, BODY_L, LEAF_F, LEAF_C)
#define OBFH_SF_VARIANT_54 OBFH_SF_SELECT(54u)
#define OBFH_SF_SPEC_55(emit) emit(1u, 25u, BODY_H, BODY_O, LEAF_K, LEAF_K)
#define OBFH_SF_VARIANT_55 OBFH_SF_SELECT(55u)
#define OBFH_SF_SPEC_56(emit) emit(1u, 26u, BODY_K, BODY_R, LEAF_D, LEAF_F)
#define OBFH_SF_VARIANT_56 OBFH_SF_SELECT(56u)
#define OBFH_SF_SPEC_57(emit) emit(1u, 28u, BODY_N, BODY_A, LEAF_I, LEAF_A)
#define OBFH_SF_VARIANT_57 OBFH_SF_SELECT(57u)
#define OBFH_SF_SPEC_58(emit) emit(2u, 22u, BODY_Q, BODY_D, LEAF_C, LEAF_I)
#define OBFH_SF_VARIANT_58 OBFH_SF_SELECT(58u)
#define OBFH_SF_SPEC_59(emit) emit(2u, 23u, BODY_T, BODY_G, LEAF_H, LEAF_D)
#define OBFH_SF_VARIANT_59 OBFH_SF_SELECT(59u)
#define OBFH_SF_SPEC_60(emit) emit(2u, 24u, BODY_C, BODY_J, LEAF_A, LEAF_K)
#define OBFH_SF_VARIANT_60 OBFH_SF_SELECT(60u)
#define OBFH_SF_SPEC_61(emit) emit(2u, 25u, BODY_F, BODY_M, LEAF_F, LEAF_G)
#define OBFH_SF_VARIANT_61 OBFH_SF_SELECT(61u)
#define OBFH_SF_SPEC_62(emit) emit(2u, 26u, BODY_I, BODY_P, LEAF_K, LEAF_B)
#define OBFH_SF_VARIANT_62 OBFH_SF_SELECT(62u)
#define OBFH_SF_SPEC_63(emit) emit(2u, 28u, BODY_L, BODY_S, LEAF_D, LEAF_I)
#define OBFH_SF_VARIANT_63 OBFH_SF_SELECT(63u)
#define OBFH_SF_SPEC_64(emit) emit(3u, 22u, BODY_O, BODY_B, LEAF_J, LEAF_E)
#define OBFH_SF_VARIANT_64 OBFH_SF_SELECT(64u)
#define OBFH_SF_SPEC_65(emit) emit(3u, 23u, BODY_R, BODY_E, LEAF_C, LEAF_L)
#define OBFH_SF_VARIANT_65 OBFH_SF_SELECT(65u)
#define OBFH_SF_SPEC_66(emit) emit(3u, 24u, BODY_A, BODY_H, LEAF_H, LEAF_G)
#define OBFH_SF_VARIANT_66 OBFH_SF_SELECT(66u)
#define OBFH_SF_SPEC_67(emit) emit(3u, 25u, BODY_D, BODY_K, LEAF_A, LEAF_C)
#define OBFH_SF_VARIANT_67 OBFH_SF_SELECT(67u)
#define OBFH_SF_SPEC_68(emit) emit(3u, 26u, BODY_G, BODY_N, LEAF_F, LEAF_J)
#define OBFH_SF_VARIANT_68 OBFH_SF_SELECT(68u)
#define OBFH_SF_SPEC_69(emit) emit(3u, 28u, BODY_J, BODY_Q, LEAF_K, LEAF_E)
#define OBFH_SF_VARIANT_69 OBFH_SF_SELECT(69u)
#define OBFH_SF_SPEC_70(emit) emit(4u, 22u, BODY_M, BODY_T, LEAF_E, LEAF_A)
#define OBFH_SF_VARIANT_70 OBFH_SF_SELECT(70u)
#define OBFH_SF_SPEC_71(emit) emit(4u, 23u, BODY_P, BODY_C, LEAF_J, LEAF_H)
#define OBFH_SF_VARIANT_71 OBFH_SF_SELECT(71u)
#define OBFH_SF_SPEC_72(emit) emit(4u, 24u, BODY_S, BODY_F, LEAF_C, LEAF_C)
#define OBFH_SF_VARIANT_72 OBFH_SF_SELECT(72u)
#define OBFH_SF_SPEC_73(emit) emit(4u, 25u, BODY_B, BODY_I, LEAF_H, LEAF_K)
#define OBFH_SF_VARIANT_73 OBFH_SF_SELECT(73u)
#define OBFH_SF_SPEC_74(emit) emit(4u, 26u, BODY_E, BODY_L, LEAF_A, LEAF_F)
#define OBFH_SF_VARIANT_74 OBFH_SF_SELECT(74u)
#define OBFH_SF_SPEC_75(emit) emit(4u, 28u, BODY_H, BODY_O, LEAF_F, LEAF_A)
#define OBFH_SF_VARIANT_75 OBFH_SF_SELECT(75u)
#define OBFH_SF_SPEC_76(emit) emit(5u, 22u, BODY_K, BODY_R, LEAF_L, LEAF_I)
#define OBFH_SF_VARIANT_76 OBFH_SF_SELECT(76u)
#define OBFH_SF_SPEC_77(emit) emit(5u, 23u, BODY_N, BODY_A, LEAF_E, LEAF_D)
#define OBFH_SF_VARIANT_77 OBFH_SF_SELECT(77u)
#define OBFH_SF_SPEC_78(emit) emit(5u, 24u, BODY_Q, BODY_D, LEAF_J, LEAF_K)
#define OBFH_SF_VARIANT_78 OBFH_SF_SELECT(78u)
#define OBFH_SF_SPEC_79(emit) emit(5u, 25u, BODY_T, BODY_G, LEAF_C, LEAF_G)
#define OBFH_SF_VARIANT_79 OBFH_SF_SELECT(79u)
#define OBFH_SF_SPEC_80(emit) emit(5u, 26u, BODY_C, BODY_J, LEAF_H, LEAF_B)
#define OBFH_SF_VARIANT_80 OBFH_SF_SELECT(80u)
#define OBFH_SF_SPEC_81(emit) emit(5u, 28u, BODY_F, BODY_M, LEAF_A, LEAF_I)
#define OBFH_SF_VARIANT_81 OBFH_SF_SELECT(81u)
#define OBFH_SF_SPEC_82(emit) emit(6u, 22u, BODY_I, BODY_P, LEAF_G, LEAF_E)
#define OBFH_SF_VARIANT_82 OBFH_SF_SELECT(82u)
#define OBFH_SF_SPEC_83(emit) emit(6u, 23u, BODY_L, BODY_S, LEAF_L, LEAF_L)
#define OBFH_SF_VARIANT_83 OBFH_SF_SELECT(83u)
#define OBFH_SF_SPEC_84(emit) emit(6u, 24u, BODY_O, BODY_B, LEAF_E, LEAF_G)
#define OBFH_SF_VARIANT_84 OBFH_SF_SELECT(84u)
#define OBFH_SF_SPEC_85(emit) emit(6u, 25u, BODY_R, BODY_E, LEAF_J, LEAF_C)
#define OBFH_SF_VARIANT_85 OBFH_SF_SELECT(85u)
#define OBFH_SF_SPEC_86(emit) emit(6u, 26u, BODY_A, BODY_H, LEAF_C, LEAF_J)
#define OBFH_SF_VARIANT_86 OBFH_SF_SELECT(86u)
#define OBFH_SF_SPEC_87(emit) emit(6u, 28u, BODY_D, BODY_K, LEAF_H, LEAF_E)
#define OBFH_SF_VARIANT_87 OBFH_SF_SELECT(87u)
#define OBFH_SF_SPEC_88(emit) emit(7u, 22u, BODY_G, BODY_N, LEAF_B, LEAF_A)
#define OBFH_SF_VARIANT_88 OBFH_SF_SELECT(88u)
#define OBFH_SF_SPEC_89(emit) emit(7u, 23u, BODY_J, BODY_Q, LEAF_G, LEAF_H)
#define OBFH_SF_VARIANT_89 OBFH_SF_SELECT(89u)
#define OBFH_SF_SPEC_90(emit) emit(7u, 24u, BODY_M, BODY_T, LEAF_L, LEAF_C)
#define OBFH_SF_VARIANT_90 OBFH_SF_SELECT(90u)
#define OBFH_SF_SPEC_91(emit) emit(7u, 25u, BODY_P, BODY_C, LEAF_E, LEAF_K)
#define OBFH_SF_VARIANT_91 OBFH_SF_SELECT(91u)
#define OBFH_SF_SPEC_92(emit) emit(7u, 26u, BODY_S, BODY_F, LEAF_J, LEAF_F)
#define OBFH_SF_VARIANT_92 OBFH_SF_SELECT(92u)
#define OBFH_SF_SPEC_93(emit) emit(7u, 28u, BODY_B, BODY_I, LEAF_C, LEAF_A)
#define OBFH_SF_VARIANT_93 OBFH_SF_SELECT(93u)
#define OBFH_SF_SPEC_94(emit) emit(8u, 22u, BODY_E, BODY_L, LEAF_I, LEAF_I)
#define OBFH_SF_VARIANT_94 OBFH_SF_SELECT(94u)
#define OBFH_SF_SPEC_95(emit) emit(8u, 23u, BODY_H, BODY_O, LEAF_B, LEAF_D)
#define OBFH_SF_VARIANT_95 OBFH_SF_SELECT(95u)
#define OBFH_SF_SPEC_96(emit) emit(8u, 24u, BODY_K, BODY_R, LEAF_G, LEAF_K)
#define OBFH_SF_VARIANT_96 OBFH_SF_SELECT(96u)
#define OBFH_SF_SPEC_97(emit) emit(8u, 25u, BODY_N, BODY_A, LEAF_L, LEAF_G)
#define OBFH_SF_VARIANT_97 OBFH_SF_SELECT(97u)
#define OBFH_SF_SPEC_98(emit) emit(8u, 26u, BODY_Q, BODY_D, LEAF_E, LEAF_B)
#define OBFH_SF_VARIANT_98 OBFH_SF_SELECT(98u)
#define OBFH_SF_SPEC_99(emit) emit(8u, 28u, BODY_T, BODY_G, LEAF_J, LEAF_I)
#define OBFH_SF_VARIANT_99 OBFH_SF_SELECT(99u)
#define OBFH_SF_SPEC_100(emit) emit(9u, 22u, BODY_C, BODY_J, LEAF_D, LEAF_E)
#define OBFH_SF_VARIANT_100 OBFH_SF_SELECT(100u)
#define OBFH_SF_SPEC_101(emit) emit(9u, 23u, BODY_F, BODY_M, LEAF_I, LEAF_L)
#define OBFH_SF_VARIANT_101 OBFH_SF_SELECT(101u)
#define OBFH_SF_SPEC_102(emit) emit(9u, 24u, BODY_I, BODY_P, LEAF_B, LEAF_G)
#define OBFH_SF_VARIANT_102 OBFH_SF_SELECT(102u)
#define OBFH_SF_SPEC_103(emit) emit(9u, 25u, BODY_L, BODY_S, LEAF_G, LEAF_C)
#define OBFH_SF_VARIANT_103 OBFH_SF_SELECT(103u)
#define OBFH_SF_SPEC_104(emit) emit(9u, 26u, BODY_O, BODY_B, LEAF_L, LEAF_J)
#define OBFH_SF_VARIANT_104 OBFH_SF_SELECT(104u)
#define OBFH_SF_SPEC_105(emit) emit(9u, 28u, BODY_R, BODY_E, LEAF_E, LEAF_E)
#define OBFH_SF_VARIANT_105 OBFH_SF_SELECT(105u)
#define OBFH_SF_SPEC_106(emit) emit(10u, 22u, BODY_A, BODY_H, LEAF_K, LEAF_A)
#define OBFH_SF_VARIANT_106 OBFH_SF_SELECT(106u)
#define OBFH_SF_SPEC_107(emit) emit(10u, 23u, BODY_D, BODY_K, LEAF_D, LEAF_H)
#define OBFH_SF_VARIANT_107 OBFH_SF_SELECT(107u)
#define OBFH_SF_SPEC_108(emit) emit(10u, 24u, BODY_G, BODY_N, LEAF_I, LEAF_C)
#define OBFH_SF_VARIANT_108 OBFH_SF_SELECT(108u)
#define OBFH_SF_SPEC_109(emit) emit(10u, 25u, BODY_J, BODY_Q, LEAF_B, LEAF_K)
#define OBFH_SF_VARIANT_109 OBFH_SF_SELECT(109u)
#define OBFH_SF_SPEC_110(emit) emit(10u, 26u, BODY_M, BODY_T, LEAF_G, LEAF_F)
#define OBFH_SF_VARIANT_110 OBFH_SF_SELECT(110u)
#define OBFH_SF_SPEC_111(emit) emit(10u, 28u, BODY_P, BODY_C, LEAF_L, LEAF_A)
#define OBFH_SF_VARIANT_111 OBFH_SF_SELECT(111u)
#define OBFH_SF_SPEC_112(emit) emit(11u, 22u, BODY_S, BODY_F, LEAF_F, LEAF_I)
#define OBFH_SF_VARIANT_112 OBFH_SF_SELECT(112u)
#define OBFH_SF_SPEC_113(emit) emit(11u, 23u, BODY_B, BODY_I, LEAF_K, LEAF_D)
#define OBFH_SF_VARIANT_113 OBFH_SF_SELECT(113u)
#define OBFH_SF_SPEC_114(emit) emit(11u, 24u, BODY_E, BODY_L, LEAF_D, LEAF_K)
#define OBFH_SF_VARIANT_114 OBFH_SF_SELECT(114u)
#define OBFH_SF_SPEC_115(emit) emit(11u, 25u, BODY_H, BODY_O, LEAF_I, LEAF_G)
#define OBFH_SF_VARIANT_115 OBFH_SF_SELECT(115u)
#define OBFH_SF_SPEC_116(emit) emit(11u, 26u, BODY_K, BODY_R, LEAF_B, LEAF_B)
#define OBFH_SF_VARIANT_116 OBFH_SF_SELECT(116u)
#define OBFH_SF_SPEC_117(emit) emit(11u, 28u, BODY_N, BODY_A, LEAF_G, LEAF_I)
#define OBFH_SF_VARIANT_117 OBFH_SF_SELECT(117u)
#define OBFH_SF_SPEC_118(emit) emit(12u, 22u, BODY_Q, BODY_D, LEAF_A, LEAF_E)
#define OBFH_SF_VARIANT_118 OBFH_SF_SELECT(118u)
#define OBFH_SF_SPEC_119(emit) emit(12u, 23u, BODY_T, BODY_G, LEAF_F, LEAF_L)
#define OBFH_SF_VARIANT_119 OBFH_SF_SELECT(119u)
#define OBFH_SF_SPEC_120(emit) emit(12u, 24u, BODY_C, BODY_J, LEAF_K, LEAF_G)
#define OBFH_SF_VARIANT_120 OBFH_SF_SELECT(120u)
#define OBFH_SF_SPEC_121(emit) emit(12u, 25u, BODY_F, BODY_M, LEAF_D, LEAF_C)
#define OBFH_SF_VARIANT_121 OBFH_SF_SELECT(121u)
#define OBFH_SF_SPEC_122(emit) emit(12u, 26u, BODY_I, BODY_P, LEAF_I, LEAF_J)
#define OBFH_SF_VARIANT_122 OBFH_SF_SELECT(122u)
#define OBFH_SF_SPEC_123(emit) emit(12u, 28u, BODY_L, BODY_S, LEAF_B, LEAF_E)
#define OBFH_SF_VARIANT_123 OBFH_SF_SELECT(123u)
#define OBFH_SF_SPEC_124(emit) emit(13u, 22u, BODY_O, BODY_B, LEAF_H, LEAF_A)
#define OBFH_SF_VARIANT_124 OBFH_SF_SELECT(124u)
#define OBFH_SF_SPEC_125(emit) emit(13u, 23u, BODY_R, BODY_E, LEAF_A, LEAF_H)
#define OBFH_SF_VARIANT_125 OBFH_SF_SELECT(125u)
#define OBFH_SF_SPEC_126(emit) emit(13u, 24u, BODY_A, BODY_H, LEAF_F, LEAF_C)
#define OBFH_SF_VARIANT_126 OBFH_SF_SELECT(126u)
#define OBFH_SF_SPEC_127(emit) emit(13u, 25u, BODY_D, BODY_K, LEAF_K, LEAF_K)
#define OBFH_SF_VARIANT_127 OBFH_SF_SELECT(127u)
// Define invariant recipes once as ordinary enum constants from their source specs.
// No pre-expanded table, external generation step, binary data or runtime lookup.
#define OBFH_SF_DECLARE_RECIPE(index) enum { __obfh_sf_recipe_##index = OBFH_SF_SPEC_##index(OBFH_SF_RECIPE) };
OBFH_SF_DECLARE_RECIPE(0)
OBFH_SF_DECLARE_RECIPE(1)
OBFH_SF_DECLARE_RECIPE(2)
OBFH_SF_DECLARE_RECIPE(3)
OBFH_SF_DECLARE_RECIPE(4)
OBFH_SF_DECLARE_RECIPE(5)
OBFH_SF_DECLARE_RECIPE(6)
OBFH_SF_DECLARE_RECIPE(7)
OBFH_SF_DECLARE_RECIPE(8)
OBFH_SF_DECLARE_RECIPE(9)
OBFH_SF_DECLARE_RECIPE(10)
OBFH_SF_DECLARE_RECIPE(11)
OBFH_SF_DECLARE_RECIPE(12)
OBFH_SF_DECLARE_RECIPE(13)
OBFH_SF_DECLARE_RECIPE(14)
OBFH_SF_DECLARE_RECIPE(15)
OBFH_SF_DECLARE_RECIPE(16)
OBFH_SF_DECLARE_RECIPE(17)
OBFH_SF_DECLARE_RECIPE(18)
OBFH_SF_DECLARE_RECIPE(19)
OBFH_SF_DECLARE_RECIPE(20)
OBFH_SF_DECLARE_RECIPE(21)
OBFH_SF_DECLARE_RECIPE(22)
OBFH_SF_DECLARE_RECIPE(23)
OBFH_SF_DECLARE_RECIPE(24)
OBFH_SF_DECLARE_RECIPE(25)
OBFH_SF_DECLARE_RECIPE(26)
OBFH_SF_DECLARE_RECIPE(27)
OBFH_SF_DECLARE_RECIPE(28)
OBFH_SF_DECLARE_RECIPE(29)
OBFH_SF_DECLARE_RECIPE(30)
OBFH_SF_DECLARE_RECIPE(31)
OBFH_SF_DECLARE_RECIPE(32)
OBFH_SF_DECLARE_RECIPE(33)
OBFH_SF_DECLARE_RECIPE(34)
OBFH_SF_DECLARE_RECIPE(35)
OBFH_SF_DECLARE_RECIPE(36)
OBFH_SF_DECLARE_RECIPE(37)
OBFH_SF_DECLARE_RECIPE(38)
OBFH_SF_DECLARE_RECIPE(39)
OBFH_SF_DECLARE_RECIPE(40)
OBFH_SF_DECLARE_RECIPE(41)
OBFH_SF_DECLARE_RECIPE(42)
OBFH_SF_DECLARE_RECIPE(43)
OBFH_SF_DECLARE_RECIPE(44)
OBFH_SF_DECLARE_RECIPE(45)
OBFH_SF_DECLARE_RECIPE(46)
OBFH_SF_DECLARE_RECIPE(47)
OBFH_SF_DECLARE_RECIPE(48)
OBFH_SF_DECLARE_RECIPE(49)
OBFH_SF_DECLARE_RECIPE(50)
OBFH_SF_DECLARE_RECIPE(51)
OBFH_SF_DECLARE_RECIPE(52)
OBFH_SF_DECLARE_RECIPE(53)
OBFH_SF_DECLARE_RECIPE(54)
OBFH_SF_DECLARE_RECIPE(55)
OBFH_SF_DECLARE_RECIPE(56)
OBFH_SF_DECLARE_RECIPE(57)
OBFH_SF_DECLARE_RECIPE(58)
OBFH_SF_DECLARE_RECIPE(59)
OBFH_SF_DECLARE_RECIPE(60)
OBFH_SF_DECLARE_RECIPE(61)
OBFH_SF_DECLARE_RECIPE(62)
OBFH_SF_DECLARE_RECIPE(63)
OBFH_SF_DECLARE_RECIPE(64)
OBFH_SF_DECLARE_RECIPE(65)
OBFH_SF_DECLARE_RECIPE(66)
OBFH_SF_DECLARE_RECIPE(67)
OBFH_SF_DECLARE_RECIPE(68)
OBFH_SF_DECLARE_RECIPE(69)
OBFH_SF_DECLARE_RECIPE(70)
OBFH_SF_DECLARE_RECIPE(71)
OBFH_SF_DECLARE_RECIPE(72)
OBFH_SF_DECLARE_RECIPE(73)
OBFH_SF_DECLARE_RECIPE(74)
OBFH_SF_DECLARE_RECIPE(75)
OBFH_SF_DECLARE_RECIPE(76)
OBFH_SF_DECLARE_RECIPE(77)
OBFH_SF_DECLARE_RECIPE(78)
OBFH_SF_DECLARE_RECIPE(79)
OBFH_SF_DECLARE_RECIPE(80)
OBFH_SF_DECLARE_RECIPE(81)
OBFH_SF_DECLARE_RECIPE(82)
OBFH_SF_DECLARE_RECIPE(83)
OBFH_SF_DECLARE_RECIPE(84)
OBFH_SF_DECLARE_RECIPE(85)
OBFH_SF_DECLARE_RECIPE(86)
OBFH_SF_DECLARE_RECIPE(87)
OBFH_SF_DECLARE_RECIPE(88)
OBFH_SF_DECLARE_RECIPE(89)
OBFH_SF_DECLARE_RECIPE(90)
OBFH_SF_DECLARE_RECIPE(91)
OBFH_SF_DECLARE_RECIPE(92)
OBFH_SF_DECLARE_RECIPE(93)
OBFH_SF_DECLARE_RECIPE(94)
OBFH_SF_DECLARE_RECIPE(95)
OBFH_SF_DECLARE_RECIPE(96)
OBFH_SF_DECLARE_RECIPE(97)
OBFH_SF_DECLARE_RECIPE(98)
OBFH_SF_DECLARE_RECIPE(99)
OBFH_SF_DECLARE_RECIPE(100)
OBFH_SF_DECLARE_RECIPE(101)
OBFH_SF_DECLARE_RECIPE(102)
OBFH_SF_DECLARE_RECIPE(103)
OBFH_SF_DECLARE_RECIPE(104)
OBFH_SF_DECLARE_RECIPE(105)
OBFH_SF_DECLARE_RECIPE(106)
OBFH_SF_DECLARE_RECIPE(107)
OBFH_SF_DECLARE_RECIPE(108)
OBFH_SF_DECLARE_RECIPE(109)
OBFH_SF_DECLARE_RECIPE(110)
OBFH_SF_DECLARE_RECIPE(111)
OBFH_SF_DECLARE_RECIPE(112)
OBFH_SF_DECLARE_RECIPE(113)
OBFH_SF_DECLARE_RECIPE(114)
OBFH_SF_DECLARE_RECIPE(115)
OBFH_SF_DECLARE_RECIPE(116)
OBFH_SF_DECLARE_RECIPE(117)
OBFH_SF_DECLARE_RECIPE(118)
OBFH_SF_DECLARE_RECIPE(119)
OBFH_SF_DECLARE_RECIPE(120)
OBFH_SF_DECLARE_RECIPE(121)
OBFH_SF_DECLARE_RECIPE(122)
OBFH_SF_DECLARE_RECIPE(123)
OBFH_SF_DECLARE_RECIPE(124)
OBFH_SF_DECLARE_RECIPE(125)
OBFH_SF_DECLARE_RECIPE(126)
OBFH_SF_DECLARE_RECIPE(127)
#define OBFH_SF_RECIPE_GROUP_0(index) __builtin_choose_expr((index) <= 3u, __builtin_choose_expr((index) <= 1u, __builtin_choose_expr((index) <= 0u, __obfh_sf_recipe_0, __obfh_sf_recipe_1), __builtin_choose_expr((index) <= 2u, __obfh_sf_recipe_2, __obfh_sf_recipe_3)), __builtin_choose_expr((index) <= 5u, __builtin_choose_expr((index) <= 4u, __obfh_sf_recipe_4, __obfh_sf_recipe_5), __builtin_choose_expr((index) <= 6u, __obfh_sf_recipe_6, __obfh_sf_recipe_7)))
#define OBFH_SF_RECIPE_GROUP_1(index) __builtin_choose_expr((index) <= 11u, __builtin_choose_expr((index) <= 9u, __builtin_choose_expr((index) <= 8u, __obfh_sf_recipe_8, __obfh_sf_recipe_9), __builtin_choose_expr((index) <= 10u, __obfh_sf_recipe_10, __obfh_sf_recipe_11)), __builtin_choose_expr((index) <= 13u, __builtin_choose_expr((index) <= 12u, __obfh_sf_recipe_12, __obfh_sf_recipe_13), __builtin_choose_expr((index) <= 14u, __obfh_sf_recipe_14, __obfh_sf_recipe_15)))
#define OBFH_SF_RECIPE_GROUP_2(index) __builtin_choose_expr((index) <= 19u, __builtin_choose_expr((index) <= 17u, __builtin_choose_expr((index) <= 16u, __obfh_sf_recipe_16, __obfh_sf_recipe_17), __builtin_choose_expr((index) <= 18u, __obfh_sf_recipe_18, __obfh_sf_recipe_19)), __builtin_choose_expr((index) <= 21u, __builtin_choose_expr((index) <= 20u, __obfh_sf_recipe_20, __obfh_sf_recipe_21), __builtin_choose_expr((index) <= 22u, __obfh_sf_recipe_22, __obfh_sf_recipe_23)))
#define OBFH_SF_RECIPE_GROUP_3(index) __builtin_choose_expr((index) <= 27u, __builtin_choose_expr((index) <= 25u, __builtin_choose_expr((index) <= 24u, __obfh_sf_recipe_24, __obfh_sf_recipe_25), __builtin_choose_expr((index) <= 26u, __obfh_sf_recipe_26, __obfh_sf_recipe_27)), __builtin_choose_expr((index) <= 29u, __builtin_choose_expr((index) <= 28u, __obfh_sf_recipe_28, __obfh_sf_recipe_29), __builtin_choose_expr((index) <= 30u, __obfh_sf_recipe_30, __obfh_sf_recipe_31)))
#define OBFH_SF_RECIPE_GROUP_4(index) __builtin_choose_expr((index) <= 35u, __builtin_choose_expr((index) <= 33u, __builtin_choose_expr((index) <= 32u, __obfh_sf_recipe_32, __obfh_sf_recipe_33), __builtin_choose_expr((index) <= 34u, __obfh_sf_recipe_34, __obfh_sf_recipe_35)), __builtin_choose_expr((index) <= 37u, __builtin_choose_expr((index) <= 36u, __obfh_sf_recipe_36, __obfh_sf_recipe_37), __builtin_choose_expr((index) <= 38u, __obfh_sf_recipe_38, __obfh_sf_recipe_39)))
#define OBFH_SF_RECIPE_GROUP_5(index) __builtin_choose_expr((index) <= 43u, __builtin_choose_expr((index) <= 41u, __builtin_choose_expr((index) <= 40u, __obfh_sf_recipe_40, __obfh_sf_recipe_41), __builtin_choose_expr((index) <= 42u, __obfh_sf_recipe_42, __obfh_sf_recipe_43)), __builtin_choose_expr((index) <= 45u, __builtin_choose_expr((index) <= 44u, __obfh_sf_recipe_44, __obfh_sf_recipe_45), __builtin_choose_expr((index) <= 46u, __obfh_sf_recipe_46, __obfh_sf_recipe_47)))
#define OBFH_SF_RECIPE_GROUP_6(index) __builtin_choose_expr((index) <= 51u, __builtin_choose_expr((index) <= 49u, __builtin_choose_expr((index) <= 48u, __obfh_sf_recipe_48, __obfh_sf_recipe_49), __builtin_choose_expr((index) <= 50u, __obfh_sf_recipe_50, __obfh_sf_recipe_51)), __builtin_choose_expr((index) <= 53u, __builtin_choose_expr((index) <= 52u, __obfh_sf_recipe_52, __obfh_sf_recipe_53), __builtin_choose_expr((index) <= 54u, __obfh_sf_recipe_54, __obfh_sf_recipe_55)))
#define OBFH_SF_RECIPE_GROUP_7(index) __builtin_choose_expr((index) <= 59u, __builtin_choose_expr((index) <= 57u, __builtin_choose_expr((index) <= 56u, __obfh_sf_recipe_56, __obfh_sf_recipe_57), __builtin_choose_expr((index) <= 58u, __obfh_sf_recipe_58, __obfh_sf_recipe_59)), __builtin_choose_expr((index) <= 61u, __builtin_choose_expr((index) <= 60u, __obfh_sf_recipe_60, __obfh_sf_recipe_61), __builtin_choose_expr((index) <= 62u, __obfh_sf_recipe_62, __obfh_sf_recipe_63)))
#define OBFH_SF_RECIPE_GROUP_8(index) __builtin_choose_expr((index) <= 67u, __builtin_choose_expr((index) <= 65u, __builtin_choose_expr((index) <= 64u, __obfh_sf_recipe_64, __obfh_sf_recipe_65), __builtin_choose_expr((index) <= 66u, __obfh_sf_recipe_66, __obfh_sf_recipe_67)), __builtin_choose_expr((index) <= 69u, __builtin_choose_expr((index) <= 68u, __obfh_sf_recipe_68, __obfh_sf_recipe_69), __builtin_choose_expr((index) <= 70u, __obfh_sf_recipe_70, __obfh_sf_recipe_71)))
#define OBFH_SF_RECIPE_GROUP_9(index) __builtin_choose_expr((index) <= 75u, __builtin_choose_expr((index) <= 73u, __builtin_choose_expr((index) <= 72u, __obfh_sf_recipe_72, __obfh_sf_recipe_73), __builtin_choose_expr((index) <= 74u, __obfh_sf_recipe_74, __obfh_sf_recipe_75)), __builtin_choose_expr((index) <= 77u, __builtin_choose_expr((index) <= 76u, __obfh_sf_recipe_76, __obfh_sf_recipe_77), __builtin_choose_expr((index) <= 78u, __obfh_sf_recipe_78, __obfh_sf_recipe_79)))
#define OBFH_SF_RECIPE_GROUP_10(index) __builtin_choose_expr((index) <= 83u, __builtin_choose_expr((index) <= 81u, __builtin_choose_expr((index) <= 80u, __obfh_sf_recipe_80, __obfh_sf_recipe_81), __builtin_choose_expr((index) <= 82u, __obfh_sf_recipe_82, __obfh_sf_recipe_83)), __builtin_choose_expr((index) <= 85u, __builtin_choose_expr((index) <= 84u, __obfh_sf_recipe_84, __obfh_sf_recipe_85), __builtin_choose_expr((index) <= 86u, __obfh_sf_recipe_86, __obfh_sf_recipe_87)))
#define OBFH_SF_RECIPE_GROUP_11(index) __builtin_choose_expr((index) <= 91u, __builtin_choose_expr((index) <= 89u, __builtin_choose_expr((index) <= 88u, __obfh_sf_recipe_88, __obfh_sf_recipe_89), __builtin_choose_expr((index) <= 90u, __obfh_sf_recipe_90, __obfh_sf_recipe_91)), __builtin_choose_expr((index) <= 93u, __builtin_choose_expr((index) <= 92u, __obfh_sf_recipe_92, __obfh_sf_recipe_93), __builtin_choose_expr((index) <= 94u, __obfh_sf_recipe_94, __obfh_sf_recipe_95)))
#define OBFH_SF_RECIPE_GROUP_12(index) __builtin_choose_expr((index) <= 99u, __builtin_choose_expr((index) <= 97u, __builtin_choose_expr((index) <= 96u, __obfh_sf_recipe_96, __obfh_sf_recipe_97), __builtin_choose_expr((index) <= 98u, __obfh_sf_recipe_98, __obfh_sf_recipe_99)), __builtin_choose_expr((index) <= 101u, __builtin_choose_expr((index) <= 100u, __obfh_sf_recipe_100, __obfh_sf_recipe_101), __builtin_choose_expr((index) <= 102u, __obfh_sf_recipe_102, __obfh_sf_recipe_103)))
#define OBFH_SF_RECIPE_GROUP_13(index) __builtin_choose_expr((index) <= 107u, __builtin_choose_expr((index) <= 105u, __builtin_choose_expr((index) <= 104u, __obfh_sf_recipe_104, __obfh_sf_recipe_105), __builtin_choose_expr((index) <= 106u, __obfh_sf_recipe_106, __obfh_sf_recipe_107)), __builtin_choose_expr((index) <= 109u, __builtin_choose_expr((index) <= 108u, __obfh_sf_recipe_108, __obfh_sf_recipe_109), __builtin_choose_expr((index) <= 110u, __obfh_sf_recipe_110, __obfh_sf_recipe_111)))
#define OBFH_SF_RECIPE_GROUP_14(index) __builtin_choose_expr((index) <= 115u, __builtin_choose_expr((index) <= 113u, __builtin_choose_expr((index) <= 112u, __obfh_sf_recipe_112, __obfh_sf_recipe_113), __builtin_choose_expr((index) <= 114u, __obfh_sf_recipe_114, __obfh_sf_recipe_115)), __builtin_choose_expr((index) <= 117u, __builtin_choose_expr((index) <= 116u, __obfh_sf_recipe_116, __obfh_sf_recipe_117), __builtin_choose_expr((index) <= 118u, __obfh_sf_recipe_118, __obfh_sf_recipe_119)))
#define OBFH_SF_RECIPE_GROUP_15(index) __builtin_choose_expr((index) <= 123u, __builtin_choose_expr((index) <= 121u, __builtin_choose_expr((index) <= 120u, __obfh_sf_recipe_120, __obfh_sf_recipe_121), __builtin_choose_expr((index) <= 122u, __obfh_sf_recipe_122, __obfh_sf_recipe_123)), __builtin_choose_expr((index) <= 125u, __builtin_choose_expr((index) <= 124u, __obfh_sf_recipe_124, __obfh_sf_recipe_125), __builtin_choose_expr((index) <= 126u, __obfh_sf_recipe_126, __obfh_sf_recipe_127)))
#define OBFH_SF_RECIPE_SELECT(index) __builtin_choose_expr((index) <= 63u, __builtin_choose_expr((index) <= 31u, __builtin_choose_expr((index) <= 15u, __builtin_choose_expr((index) <= 7u, OBFH_SF_RECIPE_GROUP_0(index), OBFH_SF_RECIPE_GROUP_1(index)), __builtin_choose_expr((index) <= 23u, OBFH_SF_RECIPE_GROUP_2(index), OBFH_SF_RECIPE_GROUP_3(index))), __builtin_choose_expr((index) <= 47u, __builtin_choose_expr((index) <= 39u, OBFH_SF_RECIPE_GROUP_4(index), OBFH_SF_RECIPE_GROUP_5(index)), __builtin_choose_expr((index) <= 55u, OBFH_SF_RECIPE_GROUP_6(index), OBFH_SF_RECIPE_GROUP_7(index)))), __builtin_choose_expr((index) <= 95u, __builtin_choose_expr((index) <= 79u, __builtin_choose_expr((index) <= 71u, OBFH_SF_RECIPE_GROUP_8(index), OBFH_SF_RECIPE_GROUP_9(index)), __builtin_choose_expr((index) <= 87u, OBFH_SF_RECIPE_GROUP_10(index), OBFH_SF_RECIPE_GROUP_11(index))), __builtin_choose_expr((index) <= 111u, __builtin_choose_expr((index) <= 103u, OBFH_SF_RECIPE_GROUP_12(index), OBFH_SF_RECIPE_GROUP_13(index)), __builtin_choose_expr((index) <= 119u, OBFH_SF_RECIPE_GROUP_14(index), OBFH_SF_RECIPE_GROUP_15(index)))))
#define OBFH_SF_POOL_INPUTS [pool_id] "i"(__obfh_pool_id), [pool_layout] "i"(__obfh_pool_layout), OBFH_SF_NATIVE_INPUTS
#define OBFH_SF_POOL_ASM(text) __obfh_asm__(text \
                                            : \
                                            : OBFH_SF_POOL_INPUTS)
#define OBFH_SF_START(guard) __obfh_asm__(guard \
                                          : \
                                          : OBFH_SF_GUARD_INPUTS \
                                          : "eax", "edx", "ecx", "cc", "memory")
#define OBFH_SF_END __obfh_asm__("9:")
#define OBFH_SF_POOL_GUARDS(start) __builtin_choose_expr((__obfh_pool_guard) <= 6u, __builtin_choose_expr((__obfh_pool_guard) <= 2u, __builtin_choose_expr((__obfh_pool_guard) <= 0u, ({ start(OBFH_SF_GUARD_0); }), __builtin_choose_expr((__obfh_pool_guard) <= 1u, ({ start(OBFH_SF_GUARD_1); }), ({ start(OBFH_SF_GUARD_2); }))), __builtin_choose_expr((__obfh_pool_guard) <= 4u, __builtin_choose_expr((__obfh_pool_guard) <= 3u, ({ start(OBFH_SF_GUARD_3); }), ({ start(OBFH_SF_GUARD_4); })), __builtin_choose_expr((__obfh_pool_guard) <= 5u, ({ start(OBFH_SF_GUARD_5); }), ({ start(OBFH_SF_GUARD_6); })))), __builtin_choose_expr((__obfh_pool_guard) <= 9u, __builtin_choose_expr((__obfh_pool_guard) <= 7u, ({ start(OBFH_SF_GUARD_7); }), __builtin_choose_expr((__obfh_pool_guard) <= 8u, ({ start(OBFH_SF_GUARD_8); }), ({ start(OBFH_SF_GUARD_9); }))), __builtin_choose_expr((__obfh_pool_guard) <= 11u, __builtin_choose_expr((__obfh_pool_guard) <= 10u, ({ start(OBFH_SF_GUARD_10); }), ({ start(OBFH_SF_GUARD_11); })), __builtin_choose_expr((__obfh_pool_guard) <= 12u, ({ start(OBFH_SF_GUARD_12); }), ({ start(OBFH_SF_GUARD_13); })))))
// Pass each body only the immediate operands it actually references.
#define OBFH_SF_OPERAND(name) [name] "i"(__obfh_sf_##name)
#define OBFH_SF_FRAME_ASM(text) __obfh_asm__(text \
                                             : \
                                             : [local_a] "i"(__obfh_sf_local_a), [local_b] "i"(__obfh_sf_local_b))
#define OBFH_SF_LEAF_ASM(text, ...) __obfh_asm__(text \
                                                 : \
                                                 : __VA_ARGS__)
#define OBFH_SF_SKELETON_ASM(text) __obfh_asm__(text \
                                                : \
                                                :)
// Early slots 0/2 exclude framed N/P/R/T; the full selector retains all 20 cores.
#define OBFH_SF_EARLY_FRAME_CORE(body) __builtin_choose_expr((body) <= OBFH_SF_ID_BODY_H, __builtin_choose_expr((body) <= OBFH_SF_ID_BODY_D, __builtin_choose_expr((body) <= OBFH_SF_ID_BODY_B, __builtin_choose_expr((body) <= OBFH_SF_ID_BODY_A, ({ OBFH_SF_LEAF_ASM(OBFH_SF_CORE_A, OBFH_SF_OPERAND(factor), OBFH_SF_OPERAND(key_a)); }), ({ OBFH_SF_LEAF_ASM(OBFH_SF_CORE_B, ); })), __builtin_choose_expr((body) <= OBFH_SF_ID_BODY_C, ({ OBFH_SF_LEAF_ASM(OBFH_SF_CORE_C, ); }), ({ OBFH_SF_LEAF_ASM(OBFH_SF_CORE_D, OBFH_SF_OPERAND(factor)); }))), __builtin_choose_expr((body) <= OBFH_SF_ID_BODY_F, __builtin_choose_expr((body) <= OBFH_SF_ID_BODY_E, ({ OBFH_SF_LEAF_ASM(OBFH_SF_CORE_E, ); }), ({ OBFH_SF_LEAF_ASM(OBFH_SF_CORE_F, OBFH_SF_OPERAND(factor), OBFH_SF_OPERAND(key_b), OBFH_SF_OPERAND(loops)); })), __builtin_choose_expr((body) <= OBFH_SF_ID_BODY_G, ({ OBFH_SF_LEAF_ASM(OBFH_SF_CORE_G, ); }), ({ OBFH_SF_LEAF_ASM(OBFH_SF_CORE_H, OBFH_SF_OPERAND(key_b)); })))), __builtin_choose_expr((body) <= OBFH_SF_ID_BODY_L, __builtin_choose_expr((body) <= OBFH_SF_ID_BODY_J, __builtin_choose_expr((body) <= OBFH_SF_ID_BODY_I, ({ OBFH_SF_LEAF_ASM(OBFH_SF_CORE_I, OBFH_SF_OPERAND(mask)); }), ({ OBFH_SF_LEAF_ASM(OBFH_SF_CORE_J, OBFH_SF_OPERAND(key_a), OBFH_SF_OPERAND(rotate)); })), __builtin_choose_expr((body) <= OBFH_SF_ID_BODY_K, ({ OBFH_SF_LEAF_ASM(OBFH_SF_CORE_K, ); }), ({ OBFH_SF_LEAF_ASM(OBFH_SF_CORE_L, OBFH_SF_OPERAND(factor)); }))), __builtin_choose_expr((body) <= OBFH_SF_ID_BODY_O, __builtin_choose_expr((body) <= OBFH_SF_ID_BODY_M, ({ OBFH_SF_LEAF_ASM(OBFH_SF_CORE_M, OBFH_SF_OPERAND(factor)); }), ({ OBFH_SF_LEAF_ASM(OBFH_SF_CORE_O, ); })), __builtin_choose_expr((body) <= OBFH_SF_ID_BODY_Q, ({ OBFH_SF_LEAF_ASM(OBFH_SF_CORE_Q, ); }), ({ OBFH_SF_LEAF_ASM(OBFH_SF_CORE_S, OBFH_SF_OPERAND(mask), OBFH_SF_OPERAND(rotate)); })))))
// Emit each physical skeleton/body slot in order. All operands are immediate;
// the compiler emits no transfer code between the assembly fragments.
#define OBFH_SF_SLOT_ASM(slot) __builtin_choose_expr((__obfh_pool_layout) <= 7u, __builtin_choose_expr((__obfh_pool_layout) <= 3u, __builtin_choose_expr((__obfh_pool_layout) <= 1u, __builtin_choose_expr((__obfh_pool_layout) <= 0u, ({ OBFH_SF_SKELETON_ASM("" OBFH_SF_SLOT_##slot(OBFH_SF_LAYOUT_SOURCE_0, OBFH_SF_SLOT_TEXT, unused)); }), ({ OBFH_SF_SKELETON_ASM("" OBFH_SF_SLOT_##slot(OBFH_SF_LAYOUT_SOURCE_1, OBFH_SF_SLOT_TEXT, unused)); })), __builtin_choose_expr((__obfh_pool_layout) <= 2u, ({ OBFH_SF_SKELETON_ASM("" OBFH_SF_SLOT_##slot(OBFH_SF_LAYOUT_SOURCE_2, OBFH_SF_SLOT_TEXT, unused)); }), ({ OBFH_SF_SKELETON_ASM("" OBFH_SF_SLOT_##slot(OBFH_SF_LAYOUT_SOURCE_3, OBFH_SF_SLOT_TEXT, unused)); }))), __builtin_choose_expr((__obfh_pool_layout) <= 5u, __builtin_choose_expr((__obfh_pool_layout) <= 4u, ({ OBFH_SF_SKELETON_ASM("" OBFH_SF_SLOT_##slot(OBFH_SF_LAYOUT_SOURCE_4, OBFH_SF_SLOT_TEXT, unused)); }), ({ OBFH_SF_SKELETON_ASM("" OBFH_SF_SLOT_##slot(OBFH_SF_LAYOUT_SOURCE_5, OBFH_SF_SLOT_TEXT, unused)); })), __builtin_choose_expr((__obfh_pool_layout) <= 6u, ({ OBFH_SF_SKELETON_ASM("" OBFH_SF_SLOT_##slot(OBFH_SF_LAYOUT_SOURCE_6, OBFH_SF_SLOT_TEXT, unused)); }), ({ OBFH_SF_SKELETON_ASM("" OBFH_SF_SLOT_##slot(OBFH_SF_LAYOUT_SOURCE_7, OBFH_SF_SLOT_TEXT, unused)); })))), __builtin_choose_expr((__obfh_pool_layout) <= 23u, __builtin_choose_expr((__obfh_pool_layout) <= 9u, __builtin_choose_expr((__obfh_pool_layout) <= 8u, ({ OBFH_SF_SKELETON_ASM("" OBFH_SF_SLOT_##slot(OBFH_SF_LAYOUT_SOURCE_8, OBFH_SF_SLOT_TEXT, unused)); }), ({ OBFH_SF_SKELETON_ASM("" OBFH_SF_SLOT_##slot(OBFH_SF_LAYOUT_SOURCE_9, OBFH_SF_SLOT_TEXT, unused)); })), __builtin_choose_expr((__obfh_pool_layout) <= 22u, ({ OBFH_SF_SKELETON_ASM("" OBFH_SF_SLOT_##slot(OBFH_SF_LAYOUT_SOURCE_22, OBFH_SF_SLOT_TEXT, unused)); }), ({ OBFH_SF_SKELETON_ASM("" OBFH_SF_SLOT_##slot(OBFH_SF_LAYOUT_SOURCE_23, OBFH_SF_SLOT_TEXT, unused)); }))), __builtin_choose_expr((__obfh_pool_layout) <= 25u, __builtin_choose_expr((__obfh_pool_layout) <= 24u, ({ OBFH_SF_SKELETON_ASM("" OBFH_SF_SLOT_##slot(OBFH_SF_LAYOUT_SOURCE_24, OBFH_SF_SLOT_TEXT, unused)); }), ({ OBFH_SF_SKELETON_ASM("" OBFH_SF_SLOT_##slot(OBFH_SF_LAYOUT_SOURCE_25, OBFH_SF_SLOT_TEXT, unused)); })), __builtin_choose_expr((__obfh_pool_layout) <= 26u, ({ OBFH_SF_SKELETON_ASM("" OBFH_SF_SLOT_##slot(OBFH_SF_LAYOUT_SOURCE_26, OBFH_SF_SLOT_TEXT, unused)); }), ({ OBFH_SF_SKELETON_ASM("" OBFH_SF_SLOT_##slot(OBFH_SF_LAYOUT_SOURCE_28, OBFH_SF_SLOT_TEXT, unused)); })))))
#define OBFH_SF_SLOT_FRAMED(body) ({ OBFH_SF_LEAF_ASM(OBFH_SF_ARGS, ); __builtin_choose_expr((body)==OBFH_SF_ID_BODY_N || (body)==OBFH_SF_ID_BODY_P || (body)==OBFH_SF_ID_BODY_R || (body)==OBFH_SF_ID_BODY_T, __builtin_choose_expr((body)<=OBFH_SF_ID_BODY_P, __builtin_choose_expr((body)<=OBFH_SF_ID_BODY_N, ({ OBFH_SF_LEAF_ASM(OBFH_SF_CORE_N, OBFH_SF_OPERAND(key_a), OBFH_SF_OPERAND(key_b)); }), ({ OBFH_SF_LEAF_ASM(OBFH_SF_CORE_P, OBFH_SF_OPERAND(factor)); })), __builtin_choose_expr((body)<=OBFH_SF_ID_BODY_R, ({ OBFH_SF_LEAF_ASM(OBFH_SF_CORE_R, OBFH_SF_OPERAND(key_a), OBFH_SF_OPERAND(loops)); }), ({ OBFH_SF_LEAF_ASM(OBFH_SF_CORE_T, ); }))), OBFH_SF_EARLY_FRAME_CORE(body)); OBFH_SF_FRAME_ASM(OBFH_SF_LOCAL); })
#define OBFH_SF_SLOT_LEAF(body) ({ OBFH_SF_LEAF_ASM(OBFH_SF_LEAF_ARGS, OBFH_SF_OPERAND(phantom)); __builtin_choose_expr((body)==OBFH_SF_ID_LEAF_B || (body)==OBFH_SF_ID_LEAF_C || (body)==OBFH_SF_ID_LEAF_F || (body)==OBFH_SF_ID_LEAF_H || (body)==OBFH_SF_ID_LEAF_J || (body)==OBFH_SF_ID_LEAF_L, ({ OBFH_SF_LEAF_ASM(OBFH_SF_LEAF_ENTER OBFH_SF_LEAF_STORE, ); __builtin_choose_expr((body)<=25u, __builtin_choose_expr((body)<=21u, ({ OBFH_SF_LEAF_ASM(OBFH_SF_LEAF_INNER_B, ); }), __builtin_choose_expr((body)<=22u, ({ OBFH_SF_LEAF_ASM(OBFH_SF_LEAF_INNER_C, OBFH_SF_OPERAND(phantom), OBFH_SF_OPERAND(rotate)); }), ({ OBFH_SF_LEAF_ASM(OBFH_SF_LEAF_INNER_F, OBFH_SF_OPERAND(phantom)); }))), __builtin_choose_expr((body)<=27u, ({ OBFH_SF_LEAF_ASM(OBFH_SF_LEAF_INNER_H, OBFH_SF_OPERAND(loops) ); }), __builtin_choose_expr((body)<=29u, ({ OBFH_SF_LEAF_ASM(OBFH_SF_LEAF_INNER_J, ); }), ({ OBFH_SF_LEAF_ASM(OBFH_SF_LEAF_INNER_L, ); })))); OBFH_SF_LEAF_ASM(OBFH_SF_LEAF_EXIT, ); }), __builtin_choose_expr((body)<=24u, __builtin_choose_expr((body)<=20u, ({ OBFH_SF_LEAF_ASM(OBFH_SF_LEAF_CORE_A, ); }), __builtin_choose_expr((body)<=23u, ({ OBFH_SF_LEAF_ASM(OBFH_SF_LEAF_CORE_D, OBFH_SF_OPERAND(loops) ); }), ({ OBFH_SF_LEAF_ASM(OBFH_SF_LEAF_CORE_E, OBFH_SF_OPERAND(factor), OBFH_SF_OPERAND(key_a)); }))), __builtin_choose_expr((body)<=26u, ({ OBFH_SF_LEAF_ASM(OBFH_SF_LEAF_CORE_G, OBFH_SF_OPERAND(factor), OBFH_SF_OPERAND(mask) ); }), __builtin_choose_expr((body)<=28u, ({ OBFH_SF_LEAF_ASM(OBFH_SF_LEAF_CORE_I, OBFH_SF_OPERAND(factor) ); }), ({ OBFH_SF_LEAF_ASM(OBFH_SF_LEAF_CORE_K, OBFH_SF_OPERAND(factor) ); }))))); })
#define OBFH_SF_SLOT_BODY(body) __builtin_choose_expr((body) < 20u, OBFH_SF_SLOT_FRAMED(body), OBFH_SF_SLOT_LEAF(body))
#define OBFH_SF_SLOT_BODY_0(body) __builtin_choose_expr((body) < 20u, ({ OBFH_SF_LEAF_ASM(OBFH_SF_ARGS, ); OBFH_SF_EARLY_FRAME_CORE(body); OBFH_SF_FRAME_ASM(OBFH_SF_LOCAL); }), OBFH_SF_SLOT_LEAF(body))
#define OBFH_SF_SLOT_BODY_1(body) OBFH_SF_SLOT_BODY(body)
#define OBFH_SF_SLOT_BODY_2(body) OBFH_SF_SLOT_BODY_0(body)
#define OBFH_SF_SLOT_BODY_3(body) OBFH_SF_SLOT_LEAF_REGISTER(body)
#define OBFH_SF_SLOT_BODY_4(body) OBFH_SF_SLOT_LEAF(body)
#define OBFH_SF_SLOT_BODY_5(body) OBFH_SF_SLOT_REPEATED_FRAME(body)
#define OBFH_SF_SLOT_BODY_6(body) OBFH_SF_SLOT_LEAF_REGISTER(body)
#define OBFH_SF_SLOT_BODY_7(body) OBFH_SF_SLOT_LEAF(body)
#define OBFH_SF_SLOT_BODY_8(body) ((void)0)
#define OBFH_SF_SLOT_ROLE_SELECT(slot) ((__obfh_pool_roles >> (3u * (slot))) & 7u)
#define OBFH_SF_SLOT_RUN(slot) ({ enum { __obfh_slot_role=OBFH_SF_SLOT_ROLE_SELECT(slot), __obfh_slot_body=__obfh_slot_role==1||__obfh_slot_role==5?__obfh_pool_body_0:__obfh_slot_role==2?__obfh_pool_body_1:__obfh_slot_role==3?__obfh_pool_body_2:__obfh_pool_body_3 }; OBFH_SF_SLOT_ASM(slot); __builtin_choose_expr(__obfh_slot_role!=0,OBFH_SF_SLOT_BODY_##slot(__obfh_slot_body),((void)0)); })
#define OBFH_SF_SLOT_LATE(slot) __builtin_choose_expr(__obfh_pool_layout == 28, ({enum {__obfh_slot_role=(0 OBFH_SF_SLOT_##slot(OBFH_SF_LAYOUT_SOURCE_28,OBFH_SF_SLOT_ROLE,unused)),__obfh_slot_body=__obfh_slot_role==1||__obfh_slot_role==5?__obfh_pool_body_0:__obfh_slot_role==2?__obfh_pool_body_1:__obfh_slot_role==3?__obfh_pool_body_2:__obfh_pool_body_3}; OBFH_SF_SKELETON_ASM("" OBFH_SF_SLOT_##slot(OBFH_SF_LAYOUT_SOURCE_28,OBFH_SF_SLOT_TEXT,unused)); OBFH_SF_SLOT_BODY_##slot(__obfh_slot_body); }), ((void)0))
// Keep source coordinates and immediate-only ASM adjacency stable; slots 3/6 use leaf A/E/I, slot 5 uses framed B/D/F/H/J/L/N/P/R/T.
#define OBFH_SF_SLOT_LEAF_REGISTER(body) ({ OBFH_SF_LEAF_ASM(OBFH_SF_LEAF_ARGS, OBFH_SF_OPERAND(phantom)); __builtin_choose_expr((body)==OBFH_SF_ID_LEAF_A, ({ OBFH_SF_LEAF_ASM(OBFH_SF_LEAF_CORE_A, ); }), __builtin_choose_expr((body)==OBFH_SF_ID_LEAF_E, ({ OBFH_SF_LEAF_ASM(OBFH_SF_LEAF_CORE_E, OBFH_SF_OPERAND(factor), OBFH_SF_OPERAND(key_a)); }), ({ OBFH_SF_LEAF_ASM(OBFH_SF_LEAF_CORE_I, OBFH_SF_OPERAND(factor)); }))); })
#define OBFH_SF_SLOT_REPEATED_FRAME(body) ({ OBFH_SF_LEAF_ASM(OBFH_SF_ARGS, ); __builtin_choose_expr((body)<=OBFH_SF_ID_BODY_J, __builtin_choose_expr((body)<=OBFH_SF_ID_BODY_D, __builtin_choose_expr((body)<=OBFH_SF_ID_BODY_B, ({ OBFH_SF_LEAF_ASM(OBFH_SF_CORE_B, ); }), ({ OBFH_SF_LEAF_ASM(OBFH_SF_CORE_D, OBFH_SF_OPERAND(factor)); })), __builtin_choose_expr((body)<=OBFH_SF_ID_BODY_F, ({ OBFH_SF_LEAF_ASM(OBFH_SF_CORE_F, OBFH_SF_OPERAND(factor), OBFH_SF_OPERAND(key_b), OBFH_SF_OPERAND(loops)); }), __builtin_choose_expr((body)<=OBFH_SF_ID_BODY_H, ({ OBFH_SF_LEAF_ASM(OBFH_SF_CORE_H, OBFH_SF_OPERAND(key_b)); }), ({ OBFH_SF_LEAF_ASM(OBFH_SF_CORE_J, OBFH_SF_OPERAND(key_a), OBFH_SF_OPERAND(rotate)); })))), __builtin_choose_expr((body)<=OBFH_SF_ID_BODY_N, __builtin_choose_expr((body)<=OBFH_SF_ID_BODY_L, ({ OBFH_SF_LEAF_ASM(OBFH_SF_CORE_L, OBFH_SF_OPERAND(factor)); }), ({ OBFH_SF_LEAF_ASM(OBFH_SF_CORE_N, OBFH_SF_OPERAND(key_a), OBFH_SF_OPERAND(key_b)); })), __builtin_choose_expr((body)<=OBFH_SF_ID_BODY_P, ({ OBFH_SF_LEAF_ASM(OBFH_SF_CORE_P, OBFH_SF_OPERAND(factor)); }), __builtin_choose_expr((body)<=OBFH_SF_ID_BODY_R, ({ OBFH_SF_LEAF_ASM(OBFH_SF_CORE_R, OBFH_SF_OPERAND(key_a), OBFH_SF_OPERAND(loops)); }), ({ OBFH_SF_LEAF_ASM(OBFH_SF_CORE_T, ); }))))); OBFH_SF_FRAME_ASM(OBFH_SF_LOCAL); })
#define OBFH_SF_POOL_RUN(recipe, start, end, ...) ({ enum { __obfh_pool_id=__COUNTER__,__obfh_pool_recipe=(recipe),__obfh_pool_guard=__obfh_pool_recipe&15u,__obfh_pool_layout=(__obfh_pool_recipe>>4)&31u,__obfh_pool_roles=OBFH_SF_ROLES_SELECT,__obfh_pool_body_0=(__obfh_pool_recipe>>9)&31u,__obfh_pool_body_1=(__obfh_pool_recipe>>14)&31u,__obfh_pool_body_2=(__obfh_pool_recipe>>19)&31u,__obfh_pool_body_3=(__obfh_pool_recipe>>24)&31u }; __obfh_asm__(OBFH_SF_ASM_CONSTANTS : : [phantom] "i"(__obfh_sf_phantom), [key_a] "i"(__obfh_sf_key_a), [key_b] "i"(__obfh_sf_key_b), [rotate] "i"(__obfh_sf_rotate), [leaf_frame] "i"(__obfh_sf_leaf_frame), [leaf_slot] "i"(__obfh_sf_leaf_slot), [leaf_alu_a] "i"(__obfh_sf_leaf_alu_a), [leaf_alu_b] "i"(__obfh_sf_leaf_alu_b), [leaf_shift] "i"(__obfh_sf_leaf_shift), [frame_a] "i"(__obfh_sf_frame_a), [frame_b] "i"(__obfh_sf_frame_b), [frame_c] "i"(__obfh_sf_frame_c), [frame_d] "i"(__obfh_sf_frame_d), [arg_a] "i"(__obfh_sf_arg_a), [arg_b] "i"(__obfh_sf_arg_b), [pad] "i"(__obfh_sf_pad), [pad_b] "i"(__obfh_sf_pad_b), [noise_a] "i"(__obfh_sf_noise_a), [noise_b] "i"(__obfh_sf_noise_b)); __VA_ARGS__; OBFH_SF_POOL_GUARDS(start);OBFH_SF_SLOT_RUN(0);OBFH_SF_SLOT_RUN(1);OBFH_SF_SLOT_RUN(2);OBFH_SF_SLOT_RUN(3);OBFH_SF_SLOT_LATE(4);OBFH_SF_SLOT_LATE(5);OBFH_SF_SLOT_LATE(6);OBFH_SF_SLOT_LATE(7);OBFH_SF_SLOT_LATE(8);end;(void)0; })
#define OBFH_SF_SELECT(index) ({OBFH_SF_CAPTURE;OBFH_SF_POOL_RUN(OBFH_SF_RECIPE_SELECT(index),OBFH_SF_START,OBFH_SF_END,OBFH_SF_START_PREFIX); })
#define OBFH_SF_VARIANT_COUNT 128u

#define STACK_PROXY_FUNCTIONS ({ \
    enum { __obfh_sf_id = __COUNTER__, \
           OBFH_MIX_FIELDS(__obfh_sf_draw, (unsigned int)__obfh_sf_id ^ (unsigned int)OBFH_SEED ^ (unsigned int)__LINE__ ^ 0x53504631u), \
           __obfh_sf_variant = (unsigned int)__obfh_sf_draw % OBFH_SF_VARIANT_COUNT }; \
    OBFH_SF_SELECT(__obfh_sf_variant); \
    (void)0; \
})

// #endregion

// #region 08. Junk primitives and scalar/string transport helpers
// ============================================================================
// 08. Junk primitives and scalar/string transport helpers
// ============================================================================

#if defined(__x86_64__)
#define BAD_JMP __obfh_asm__("xorl %eax, %eax; mov %eax, %rax; mov %ebx, %edx; .byte 0xFF, 0x25, 0xF1, 0xF2, 0xF3, 0xF4;")
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

#define __CRASH \
    __obfh_asm__( \
        ".byte 0xED;int $3;"); \
    exit(1);

#define TRUE ((((_9 + _7 + (RND(0, 1000) * _0))) / _8) - _1)
#define FALSE (((_3 + _6 + (RND(0, 1000) * _0)) - _9) * RND(0, 255))

typedef struct {
#if ((OBFH_SEED >> 19) & 7u) % 6u == 0u
    volatile LONG state;
    unsigned char name[32];
    ULONG_PTR address;
#endif
#if ((OBFH_SEED >> 19) & 7u) % 6u == 1u
    volatile LONG state;
    ULONG_PTR address;
    unsigned char name[32];
#endif
#if ((OBFH_SEED >> 19) & 7u) % 6u == 2u
    unsigned char name[32];
    volatile LONG state;
    ULONG_PTR address;
#endif
#if ((OBFH_SEED >> 19) & 7u) % 6u == 3u
    unsigned char name[32];
    ULONG_PTR address;
    volatile LONG state;
#endif
#if ((OBFH_SEED >> 19) & 7u) % 6u == 4u
    ULONG_PTR address;
    volatile LONG state;
    unsigned char name[32];
#endif
#if ((OBFH_SEED >> 19) & 7u) % 6u == 5u
    ULONG_PTR address;
    unsigned char name[32];
    volatile LONG state;
#endif
} OBFH_CRT_ENTRY;
static inline FARPROC obfh_crt_lookup(const char *name);
static inline FARPROC obfh_crt_site(const char *name, void *storage, ULONG_PTR key);
#define obfh_crt_resolve(name) ({ \
    enum { __obfh_crt_site = __COUNTER__, \
           __obfh_crt_draw = OBFH_MIX_B(OBFH_JUNK_WORD ^ __obfh_crt_site) }; \
    static OBFH_CRT_ENTRY __obfh_crt_slot; \
    const char *__obfh_cache_input = (name); \
    ULONG_PTR __obfh_crt_key = (ULONG_PTR)__obfh_crt_draw ^ ((ULONG_PTR)OBFH_SEED << (sizeof(ULONG_PTR) == 8 ? 32 : 0)); \
    obfh_crt_site(__obfh_cache_input, &__obfh_crt_slot, __obfh_crt_key); \
})
#define OBFH_CRT_TARGET(type, name) ((type)obfh_crt_resolve(name))
#define OBFH_CRT_PROXY_ENTER \
    STACK_PROXY_FUNCTIONS; \
    BREAK_STACK_CFLOW
#define OBFH_CRT_PROXY_RETURN(value) \
    PHANTOM_NOP; \
    STACK_PROXY_FUNCTIONS; \
    RET_BY_VAR(value)
static inline void *malloc_proxy(size_t size) {
    OBFH_CRT_PROXY_ENTER;
    char name[7];
    OBFH_NAME_ORDER_FOR(OBFH_ALPHA_LOW | OBFH_ALPHA_HIGH,
                        (OBFH_NAME_PAIR(1, name[OBFH_NAME_INDEX(0)] = _m, name[OBFH_NAME_INDEX(1)] = _a), OBFH_NAME_PAIR(2, name[OBFH_NAME_INDEX(2)] = _l, name[OBFH_NAME_INDEX(3)] = _l), OBFH_NAME_PAIR(3, name[OBFH_NAME_INDEX(4)] = _o, name[OBFH_NAME_INDEX(5)] = _c), name[OBFH_NAME_INDEX(6)] = _0),
                        (OBFH_NAME_PAIR(1, name[OBFH_NAME_INDEX(6)] = _0, name[OBFH_NAME_INDEX(5)] = _c), OBFH_NAME_PAIR(2, name[OBFH_NAME_INDEX(4)] = _o, name[OBFH_NAME_INDEX(3)] = _l), OBFH_NAME_PAIR(3, name[OBFH_NAME_INDEX(2)] = _l, name[OBFH_NAME_INDEX(1)] = _a), name[OBFH_NAME_INDEX(0)] = _m));
    PHANTOM_NOP;
    void *result = OBFH_CRT_TARGET(void *(*)(size_t), name)(size);
    STACK_PROXY_FUNCTIONS;
    RET_BY_VAR(result);
}
#define malloc(...) malloc_proxy(__VA_ARGS__)

static float rndValueToProxy = RND(0, 10);

static inline int obfh_int_proxy(int value) OBFH_CODE_SECTION_ATTRIBUTE {
    // Keep index calls small; the value still uses protected return transport.
    PHANTOM_NOP;
    RET_BY_VAR(value);
}

// Preserve pointer and SIZE_T width on both Windows targets.
static inline ULONG_PTR obfh_uintptr_proxy(ULONG_PTR value) OBFH_CODE_SECTION_ATTRIBUTE {
    BREAK_STACK_CFLOW;
    PHANTOM_NOP;
    RET_BY_VAR(value);
}

#define OBFH_PTR(type, value) ((type)obfh_uintptr_proxy((ULONG_PTR)(value)))

static inline double obfh_double_proxy(double value) OBFH_CODE_SECTION_ATTRIBUTE {
    BREAK_STACK_CFLOW;
    PHANTOM_NOP;
    RET_BY_VAR(value);
}

static inline float obfh_condition_true();

// Hidden string access
static inline char *obfh_process_hidden_string(char *string, ...) OBFH_CODE_SECTION_ATTRIBUTE {
    BREAK_STACK_CFLOW;
    PHANTOM_NOP;

    if (!obfh_condition_true() || _0) {
        BAD_JMP;
    }

    // ['\0', 's', 't', 'r', 'i', 'n', 'g'] => "string"
    // STACK_STRING storage belongs to the HIDE_STRING call site.
    return string + 1;
}

static inline float obfh_condition_true() OBFH_CODE_SECTION_ATTRIBUTE {
    BREAK_STACK_CFLOW;
    PHANTOM_NOP;
    return _1 && TRUE;
}

static inline int obfh_condition_proxy(float junk, float condition, ...) OBFH_CODE_SECTION_ATTRIBUTE {
    BREAK_STACK_CFLOW;
    PHANTOM_NOP;
    RET_BY_VAR(condition);
}

static inline long double __s_rdtsc(float junk, ...) OBFH_CODE_SECTION_ATTRIBUTE {
    BREAK_STACK_CFLOW;
    unsigned long long rdtsc_result;
    {
        unsigned int lo, hi;
        __obfh_asm__(".byte 0x0f, 0x31;"  // rdtsc
                     : "=a"(lo), "=d"(hi)
                     :
                     : "memory");

        rdtsc_result = ((unsigned long long)hi << 32) | lo;

        if (rdtsc_result == obfh_int_proxy(0))
            __obfh_asm__(".byte 0xE8;");
    }

    // This is entropy for hidden-string transport, not an ordered timing measurement.
    // Reuse the timestamp already read; no feature probe or serializing read is needed.
    return rdtsc_result;
}

// #endregion

// #region 09. CRT cache and VM value transport
// ============================================================================
// 09. CRT cache and VM value transport
// ============================================================================

// Cache entries are immutable after atomic publication. Module references are
// retained by the resolver so cached pointers cannot outlive their DLL.
// A published slot is immutable. Variable-name calls resolve other names cold.
// Seed-selected mix order keeps two operations and no runtime selector.
// Paired unsigned address transforms; selected before compilation.
#if (OBFH_SEED >> 23) & 1u
#define OBFH_CRT_BIAS -
#define OBFH_CRT_UNBIAS +
#else
#define OBFH_CRT_BIAS +
#define OBFH_CRT_UNBIAS -
#endif
// Byte truncation is part of the inverse for modular add/subtract forms.
#if (OBFH_SEED % 3u) == 0u
#define OBFH_CRT_BYTE_ENCODE(value, mask) ((unsigned char)((value) ^ (mask)))
#define OBFH_CRT_BYTE_DECODE(value, mask) ((unsigned char)((value) ^ (mask)))
#elif (OBFH_SEED % 3u) == 1u
#define OBFH_CRT_BYTE_ENCODE(value, mask) ((unsigned char)((value) + (mask)))
#define OBFH_CRT_BYTE_DECODE(value, mask) ((unsigned char)((value) - (mask)))
#else
#define OBFH_CRT_BYTE_ENCODE(value, mask) ((unsigned char)((value) - (mask)))
#define OBFH_CRT_BYTE_DECODE(value, mask) ((unsigned char)((value) + (mask)))
#endif
static inline FARPROC obfh_crt_site(const char *name, void *storage, ULONG_PTR key) {
    OBFH_CRT_ENTRY *entry = (OBFH_CRT_ENTRY *)storage;
    unsigned int length = 0;
    if (!name) return NULL;
    while (length < 32 && name[length]) ++length;
    if (length < 32 && OBFH_LOADER_READ(entry->state) == 2) {
        unsigned int i = 0;
        for (; i <= length; ++i) {
            unsigned char mask = (unsigned char)(key >> ((i % sizeof(key)) * 8u)) + (unsigned char)(i * 13u);
            if (OBFH_CRT_BYTE_DECODE(entry->name[i], mask) != (unsigned char)name[i]) break;
        }
        if (i > length) {
#if (OBFH_SEED & 1u)
            return (FARPROC)((entry->address ^ ~key) OBFH_CRT_UNBIAS key);
#else
            return (FARPROC)((entry->address OBFH_CRT_UNBIAS key) ^ ~key);
#endif
        }
    }
    PHANTOM_NOP;
    FARPROC function = obfh_crt_lookup(name);
    BREAK_STACK_CFLOW;
    if (function && length < 32 && InterlockedCompareExchange(&entry->state, 1, 0) == 0) {
        for (unsigned int i = 0; i <= length; ++i) {
            unsigned char mask = (unsigned char)(key >> ((i % sizeof(key)) * 8u)) + (unsigned char)(i * 13u);
            entry->name[i] = OBFH_CRT_BYTE_ENCODE((unsigned char)name[i], mask);
        }
#if (OBFH_SEED & 1u)
        entry->address = ((ULONG_PTR)function OBFH_CRT_BIAS key) ^ ~key;
#else
        entry->address = ((ULONG_PTR)function ^ ~key) OBFH_CRT_BIAS key;
#endif
        InterlockedExchange(&entry->state, 2);
    }
    return function;
}

// Transport object bytes instead of adding a salt to the numeric value.
// This preserves subnormals, signed zero, infinities and NaN payloads.
typedef struct {
    unsigned char bytes[sizeof(long double)];
    unsigned char floating;
    unsigned int nonce;
} OBFH_VM_VALUE;

// Both forms move object bytes, without changing floating-point values.
#if (OBFH_SEED >> 10) & 1u
#define OBFH_VM_BYTE_ENCODE(value, add, mask) ((unsigned char)((value) ^ (mask)) + (add))
#define OBFH_VM_BYTE_DECODE(value, add, mask) ((unsigned char)((value) - (add)) ^ (mask))
#else
#define OBFH_VM_BYTE_ENCODE(value, add, mask) ((unsigned char)((value) + (add)) ^ (mask))
#define OBFH_VM_BYTE_DECODE(value, add, mask) ((unsigned char)((value) ^ (mask)) - (add))
#endif

// Reverse and mirror the same xorshift triple: four compile-time state maps.
// Reversal plus transposition preserves the original map's period; bit reversal
// exchanges left/right shifts. Both transports must use the same map.
#define OBFH_VM_STATE_FIRST (((OBFH_SEED >> 22) & 1u) ? 5 : 13)
#define OBFH_VM_STATE_LAST (((OBFH_SEED >> 22) & 1u) ? 13 : 5)
#if (OBFH_SEED >> 21) & 1u
#define OBFH_VM_STATE_STEP(state) \
    do { \
        (state) ^= (state) >> OBFH_VM_STATE_FIRST; \
        (state) ^= (state) << 17; \
        (state) ^= (state) >> OBFH_VM_STATE_LAST; \
    } while (0)
#else
#define OBFH_VM_STATE_STEP(state) \
    do { \
        (state) ^= (state) << OBFH_VM_STATE_FIRST; \
        (state) ^= (state) >> 17; \
        (state) ^= (state) << OBFH_VM_STATE_LAST; \
    } while (0)
#endif

static inline OBFH_VM_VALUE obfh_vm_encode(long double value, int salt, unsigned int floating) OBFH_CODE_SECTION_ATTRIBUTE {
    BREAK_STACK_CFLOW;
    OBFH_VM_VALUE encoded;
    volatile int key = (int)obfh_double_proxy((double)(float)obfh_int_proxy(salt));
    const unsigned char *bytes = (const unsigned char *)&value;
    encoded.nonce = (floating >> 1) ^ (unsigned int)OBFH_SEED;
    encoded.floating = floating & 1u;
    unsigned int state = encoded.nonce ^ ((unsigned int)key * 2246822519u) ^ 0x9e3779b9u;
    size_t offset = encoded.nonce % sizeof(value);
    PHANTOM_NOP;
    for (size_t i = 0; i < sizeof(value); ++i) {
        OBFH_VM_STATE_STEP(state);
        unsigned int rotate = (state >> 24) & 7u;
        unsigned char rotated = (unsigned char)((bytes[i] << rotate) | (bytes[i] >> (8u - rotate)));
        encoded.bytes[(i + offset) % sizeof(value)] = OBFH_VM_BYTE_ENCODE(rotated, (unsigned char)(state >> 8), (unsigned char)(state ^ (state >> 16)));
    }
    return encoded;
}

static inline long double obfh_vm_decode(OBFH_VM_VALUE encoded, int salt) OBFH_CODE_SECTION_ATTRIBUTE {
    BREAK_STACK_CFLOW;
    long double value;
    volatile int key = obfh_condition_proxy((float)salt, (float)obfh_int_proxy(salt));
    unsigned char *bytes = (unsigned char *)&value;
    unsigned int state = encoded.nonce ^ ((unsigned int)key * 2246822519u) ^ 0x9e3779b9u;
    size_t offset = encoded.nonce % sizeof(value);
    PHANTOM_NOP;
    for (size_t i = 0; i < sizeof(value); ++i) {
        OBFH_VM_STATE_STEP(state);
        unsigned int rotate = (state >> 24) & 7u;
        unsigned char rotated = OBFH_VM_BYTE_DECODE(encoded.bytes[(i + offset) % sizeof(value)], (unsigned char)(state >> 8), (unsigned char)(state ^ (state >> 16)));
        bytes[i] = (unsigned char)((rotated >> rotate) | (rotated << (8u - rotate)));
    }
    RET_BY_VAR(value);
}

// #endregion

// #region 10. CFLOW helpers: selectors, permutations and local graphs
// ============================================================================
// 10. CFLOW helpers: selectors, permutations and local graphs
// ============================================================================

#if OBFH_DISABLE_CONTROL_FLOW != 1

#ifndef OBFH_BETTER_CONTROL_FLOW
#define OBFH_BETTER_CONTROL_FLOW 0
#endif

// Instruction starts and their following bytes belong only to skipped regions.
#define OBFH_FLOW_OPCODE(kind) \
    ((((kind)&4u ? 0xF3660FFFu : 0xE9E8898Bu) >> (((kind)&3u) * 8u)) & 255u)
#define OBFH_FLOW_DEAD_BYTES \
    ({ \
        enum { __obfh_dead_kind = OBFH_JUNK_BYTE & 7u }; \
        __obfh_asm__(".byte %c0, %c1, %c2, %c3; .long %c4; .fill %c5, 1,((( %c6)&0xffffffff)^0x80000000)-0x80000000;" \
                     : \
                     : "i"(OBFH_FLOW_OPCODE(__obfh_dead_kind)), \
                       "i"(OBFH_JUNK_BYTE), "i"(OBFH_JUNK_BYTE), "i"(OBFH_JUNK_BYTE), \
                       "i"(OBFH_JUNK_WORD), "i"(OBFH_JUNK_BYTE & 7u), "i"(OBFH_JUNK_BYTE)); \
    })

// Tag equality/inequality survives the existing bijective live ladder.
// Other sites use scalar transport, whose implementation retains BREAK_STACK_CFLOW.
// Completion consumes the transported result instead of returning a C constant.
#define OBFH_PUBLIC_ELSE_GUARD \
    (({ \
        enum { __obfh_else_site = __COUNTER__, \
               OBFH_MIX_FIELDS(__obfh_flow_hash, (unsigned int)OBFH_SEED ^ \
                                                     (unsigned int)__obfh_else_site ^ (unsigned int)__LINE__ * 2654435761u ^ 0x454C5345u), \
               __obfh_false_tag = OBFH_MIX_A(__obfh_flow_hash ^ 0x15729f81u), \
               __obfh_true_tag = __obfh_false_tag ^ (OBFH_MIX_B(__obfh_flow_hash ^ 0x74ba953du) | 1u) }; \
        OBFH_FLOW_ELSE_TRANSPORT; \
    }))
#define OBFH_FLOW_ELSE_GUARD OBFH_PUBLIC_ELSE_GUARD

#if defined(__TINYC__) && defined(__x86_64__) && defined(_WIN32) && !OBFH_DISABLE_PDATA_DECOYS
#define OBFH_FLOW_ELSE_TRANSPORT \
    __builtin_choose_expr(OBFH_PD_LIVE_ENABLED && ((__obfh_flow_hash & 15u) == 0u), ({ \
                              unsigned int __obfh_flow_state = ((0xa8u >> ((__obfh_flow_hash >> 16) & 7u)) & 1u) ? __obfh_true_tag : __obfh_false_tag; \
                              unsigned int __obfh_flow_tag = __obfh_true_tag, __obfh_flow_result; \
                              ULONG_PTR __obfh_cookie = 0; \
                              OBFH_P_CHAIN(__obfh_true_tag); \
                              OBFH_P_FINISH((__obfh_flow_hash >> 16) & 7u); \
                              __obfh_flow_result; \
                          }), \
                          ((unsigned int)obfh_uintptr_proxy(__obfh_false_tag) == __obfh_true_tag))
#else
#define OBFH_FLOW_ELSE_TRANSPORT \
    ((unsigned int)obfh_uintptr_proxy(__obfh_false_tag) == __obfh_true_tag)
#endif

// Guard outputs are part of local condition transport. Skipped bodies stay skipped.
// Each expression describes EAX at label 9 in the corresponding native guard.
// A live ladder is selected at a subset of intercepted if sites. All nodes
// are existing x64 native carriers; disabled modes keep their previous bodies.
#if defined(__TINYC__) && defined(__x86_64__) && defined(_WIN32) && !OBFH_DISABLE_PDATA_DECOYS
// The third argument becomes the next node's second argument; NULL ends a ladder.
typedef unsigned int (*OBFH_PD_LIVE_FN)(unsigned int, void *, void *);
#define OBFH_PD_LIVE_ADDRESS(site) ((OBFH_PD_LIVE_FN)__obfh_pdata_decoy_##site)
static const OBFH_PD_LIVE_FN obfh_pd_live_entries[86] = {
    OBFH_PD_LIVE_ADDRESS(0), OBFH_PD_LIVE_ADDRESS(1), OBFH_PD_LIVE_ADDRESS(2), OBFH_PD_LIVE_ADDRESS(3),
    OBFH_PD_LIVE_ADDRESS(4), OBFH_PD_LIVE_ADDRESS(5), OBFH_PD_LIVE_ADDRESS(6), OBFH_PD_LIVE_ADDRESS(7),
    OBFH_PD_LIVE_ADDRESS(8), OBFH_PD_LIVE_ADDRESS(9), OBFH_PD_LIVE_ADDRESS(10), OBFH_PD_LIVE_ADDRESS(11),
    OBFH_PD_LIVE_ADDRESS(12), OBFH_PD_LIVE_ADDRESS(13), OBFH_PD_LIVE_ADDRESS(14), OBFH_PD_LIVE_ADDRESS(15),
    OBFH_PD_LIVE_ADDRESS(16), OBFH_PD_LIVE_ADDRESS(17), OBFH_PD_LIVE_ADDRESS(18), OBFH_PD_LIVE_ADDRESS(19),
    OBFH_PD_LIVE_ADDRESS(20), OBFH_PD_LIVE_ADDRESS(21), OBFH_PD_LIVE_ADDRESS(22), OBFH_PD_LIVE_ADDRESS(23),
    OBFH_PD_LIVE_ADDRESS(24), OBFH_PD_LIVE_ADDRESS(25), OBFH_PD_LIVE_ADDRESS(26), OBFH_PD_LIVE_ADDRESS(27),
    OBFH_PD_LIVE_ADDRESS(28), OBFH_PD_LIVE_ADDRESS(29), OBFH_PD_LIVE_ADDRESS(30), OBFH_PD_LIVE_ADDRESS(31),
    OBFH_PD_LIVE_ADDRESS(32), OBFH_PD_LIVE_ADDRESS(33), OBFH_PD_LIVE_ADDRESS(34), OBFH_PD_LIVE_ADDRESS(35),
    OBFH_PD_LIVE_ADDRESS(36), OBFH_PD_LIVE_ADDRESS(37), OBFH_PD_LIVE_ADDRESS(38), OBFH_PD_LIVE_ADDRESS(39),
    OBFH_PD_LIVE_ADDRESS(40), OBFH_PD_LIVE_ADDRESS(41), OBFH_PD_LIVE_ADDRESS(42), OBFH_PD_LIVE_ADDRESS(43),
    OBFH_PD_LIVE_ADDRESS(44), OBFH_PD_LIVE_ADDRESS(45), OBFH_PD_LIVE_ADDRESS(46), OBFH_PD_LIVE_ADDRESS(47),
    OBFH_PD_LIVE_ADDRESS(48), OBFH_PD_LIVE_ADDRESS(49), OBFH_PD_LIVE_ADDRESS(50), OBFH_PD_LIVE_ADDRESS(51),
    OBFH_PD_LIVE_ADDRESS(52), OBFH_PD_LIVE_ADDRESS(53), OBFH_PD_LIVE_ADDRESS(54), OBFH_PD_LIVE_ADDRESS(55),
    OBFH_PD_LIVE_ADDRESS(56), OBFH_PD_LIVE_ADDRESS(57), OBFH_PD_LIVE_ADDRESS(58), OBFH_PD_LIVE_ADDRESS(59),
    OBFH_PD_LIVE_ADDRESS(60), OBFH_PD_LIVE_ADDRESS(61), OBFH_PD_LIVE_ADDRESS(62), OBFH_PD_LIVE_ADDRESS(63),
    OBFH_PD_LIVE_ADDRESS(64), OBFH_PD_LIVE_ADDRESS(65), OBFH_PD_LIVE_ADDRESS(66), OBFH_PD_LIVE_ADDRESS(67),
    OBFH_PD_LIVE_ADDRESS(68), OBFH_PD_LIVE_ADDRESS(69), OBFH_PD_LIVE_ADDRESS(70), OBFH_PD_LIVE_ADDRESS(71),
    OBFH_PD_LIVE_ADDRESS(72), OBFH_PD_LIVE_ADDRESS(73), OBFH_PD_LIVE_ADDRESS(74), OBFH_PD_LIVE_ADDRESS(75),
    OBFH_PD_LIVE_ADDRESS(76), OBFH_PD_LIVE_ADDRESS(77), OBFH_PD_LIVE_ADDRESS(78), OBFH_PD_LIVE_ADDRESS(79),
    OBFH_PD_LIVE_ADDRESS(80), OBFH_PD_LIVE_ADDRESS(81), OBFH_PD_LIVE_ADDRESS(82), OBFH_PD_LIVE_ADDRESS(83),
    OBFH_PD_LIVE_ADDRESS(84), OBFH_PD_LIVE_ADDRESS(85)};
#define OBFH_PD_LIVE_NODE(node) obfh_pd_live_entries[OBFH_PD_LIVE_SITE(node)]
#define OBFH_PD_LIVE_PARAMETERS(stage, site) \
    enum { __obfh_lkind##stage = OBFH_PD_DRAW(site, 1u) & 7u, \
           __obfh_lkey##stage = OBFH_PD_DRAW(site, 4u), \
           __obfh_ladd##stage = OBFH_PD_DRAW(site, 5u), \
           __obfh_lmul##stage = OBFH_PD_DRAW(site, 6u) | 1u, \
           __obfh_lrot##stage = 1u + (OBFH_PD_DRAW(site, 7u) % 31u) }
#define OBFH_PD_LIVE_ROL(x, n) (((unsigned int)(x) << (n)) | ((unsigned int)(x) >> (32u - (n))))
#define OBFH_PD_LIVE_ROR(x, n) (((unsigned int)(x) >> (n)) | ((unsigned int)(x) << (32u - (n))))
#define OBFH_PD_LIVE_SWAP(x) ((((unsigned int)(x)&255u) << 24) | (((unsigned int)(x)&65280u) << 8) | (((unsigned int)(x) >> 8) & 65280u) | ((unsigned int)(x) >> 24))
#define OBFH_PD_LIVE_PRE(stage, x) \
    (__obfh_lkind##stage == 0u ? (((unsigned int)(x) ^ __obfh_lkey##stage) * __obfh_lmul##stage + __obfh_ladd##stage) : __obfh_lkind##stage == 1u ? ((OBFH_PD_LIVE_ROL(x, __obfh_lrot##stage) ^ __obfh_lkey##stage) + __obfh_ladd##stage) \
                                                                                                                    : __obfh_lkind##stage == 2u   ? (((unsigned int)(x) + __obfh_lkey##stage) ^ (((unsigned int)(x) + __obfh_lkey##stage) >> __obfh_lrot##stage)) \
                                                                                                                    : __obfh_lkind##stage == 3u   ? ((OBFH_PD_LIVE_SWAP(x) ^ __obfh_lkey##stage) + __obfh_ladd##stage) \
                                                                                                                    : __obfh_lkind##stage == 4u   ? ((~(unsigned int)(x) + __obfh_lkey##stage) ^ __obfh_ladd##stage) \
                                                                                                                    : __obfh_lkind##stage == 5u   ? (((unsigned int)(x) ^ ((unsigned int)(x) >> 13)) * __obfh_lmul##stage) \
                                                                                                                    : __obfh_lkind##stage == 6u   ? OBFH_PD_LIVE_SWAP(OBFH_PD_LIVE_ROR(x, __obfh_lrot##stage) + __obfh_lkey##stage) \
                                                                                                                                                  : (OBFH_PD_LIVE_ROL((unsigned int)(x)-__obfh_lkey##stage, __obfh_lrot##stage) ^ __obfh_ladd##stage))
// The two post routes preserve parity, so their images remain disjoint.
#define OBFH_PD_LIVE_POST(stage, x) \
    (((unsigned int)(x)&1u) ? (((unsigned int)(x) ^ (__obfh_ladd##stage & ~1u)) * __obfh_lmul##stage + (__obfh_lkey##stage & ~1u)) : (((unsigned int)(x)*__obfh_lmul##stage + (__obfh_lkey##stage & ~1u)) ^ (__obfh_ladd##stage & ~1u)))
#define OBFH_P_CHAIN(true_tag) ({ \
    enum { __obfh_lnode0 = (__obfh_flow_hash >> 3) & 15u, \
           __obfh_lnode1 = (__obfh_lnode0 + (((__obfh_flow_hash >> 11) & 7u) * 2u + 1u)) & 15u, \
           __obfh_lnode2 = (__obfh_lnode0 + 8u) & 15u, \
           __obfh_lsite0 = OBFH_PD_LIVE_SITE(__obfh_lnode0), \
           __obfh_lsite1 = OBFH_PD_LIVE_SITE(__obfh_lnode1), \
           __obfh_lsite2 = OBFH_PD_LIVE_SITE(__obfh_lnode2) }; \
    OBFH_PD_LIVE_PARAMETERS(0, __obfh_lsite0); \
    OBFH_PD_LIVE_PARAMETERS(1, __obfh_lsite1); \
    OBFH_PD_LIVE_PARAMETERS(2, __obfh_lsite2); \
    enum { __obfh_ltag0 = OBFH_PD_LIVE_PRE(0, true_tag), \
           __obfh_ltag1 = OBFH_PD_LIVE_PRE(1, __obfh_ltag0), \
           __obfh_ltag2 = OBFH_PD_LIVE_PRE(2, __obfh_ltag1), \
           __obfh_ltag3 = OBFH_PD_LIVE_POST(2, __obfh_ltag2), \
           __obfh_ltag4 = OBFH_PD_LIVE_POST(1, __obfh_ltag3), \
           __obfh_ltag5 = OBFH_PD_LIVE_POST(0, __obfh_ltag4) }; \
    __obfh_flow_tag = __obfh_ltag5; \
    __obfh_flow_state = obfh_pd_live_entries[__obfh_lsite0](__obfh_flow_state, \
                                                            (void *)obfh_pd_live_entries[__obfh_lsite1], (void *)obfh_pd_live_entries[__obfh_lsite2]); \
    __obfh_cookie ^= (ULONG_PTR)__obfh_flow_state; \
})
#else
#define OBFH_P_CHAIN(true_tag) ((void)0)
#endif

#define OBFH_SF_GUARD_0_VALUE(x) ((x) * (x))
#define OBFH_SF_GUARD_1_VALUE(x) ((x) * ((x) + 1u))
#define OBFH_SF_GUARD_2_VALUE(x) (((x) * (x)) ^ (x))
#define OBFH_SF_GUARD_3_VALUE(x) (x)
#define OBFH_SF_GUARD_4_VALUE(x) ((x) + __obfh_sf_mask)
#define OBFH_SF_GUARD_5_VALUE(x) (x)
#define OBFH_SF_GUARD_6_VALUE(x) (x)
#define OBFH_SF_GUARD_7_VALUE(x) ((x) * ((x)-1u))
#define OBFH_SF_GUARD_8_VALUE(x) ((x) ^ (unsigned int)__obfh_sf_key_a)
#define OBFH_SF_GUARD_9_VALUE(x) ((x) + __obfh_sf_mask)
#define OBFH_SF_GUARD_10_VALUE(x) ((x) & (0u - (x)))
#define OBFH_SF_GUARD_11_VALUE(x) ((x)*__obfh_sf_factor)
#define OBFH_SF_GUARD_12_VALUE(x) (((x) + __obfh_sf_key_a) * __obfh_sf_factor)
#define OBFH_SF_GUARD_13_VALUE(x) ((x) * (2u * (unsigned int)__obfh_sf_key_a) + (unsigned int)__obfh_sf_key_a * (unsigned int)__obfh_sf_key_a)
#define OBFH_SF_GUARD_14_VALUE(x) (x)
// Prepare once per insertion, outside the 128-way emission choice.
#define OBFH_SF_FLOW_PREPARE \
    __obfh_live_input = (unsigned int)__obfh_cookie ^ __obfh_flow_hash; \
    unsigned int __obfh_live_transformed = __builtin_choose_expr(__si_f == 1, __obfh_live_input + __obfh_sf_salt, \
                                                                 __builtin_choose_expr(__si_f == 2, __obfh_live_input - __obfh_sf_salt, __obfh_live_input ^ __obfh_sf_salt)); \
    __obfh_live_transformed = __builtin_choose_expr(__si_f == 3, OBFH_INPUT_BSWAP(__obfh_live_transformed), \
                                                    (__obfh_live_transformed << __obfh_sf_rotate) | (__obfh_live_transformed >> (32u - __obfh_sf_rotate))); \
    enum { __obfh_sf_guard = __obfh_sf_variant < 16 ? __obfh_sf_variant / 4 : __obfh_sf_variant < 46 ? 4 + (__obfh_sf_variant - 16) / 6 \
                                                                          : __obfh_sf_variant < 100  ? (__obfh_sf_variant - 46) / 6 \
                                                                                                     : 9 + (__obfh_sf_variant - 100) / 6 }; \
    __obfh_live_expected = __builtin_choose_expr(__obfh_sf_guard == 0u, OBFH_SF_GUARD_0_VALUE(__obfh_live_transformed), \
                                                 __builtin_choose_expr(__obfh_sf_guard == 1u, OBFH_SF_GUARD_1_VALUE(__obfh_live_transformed), \
                                                                       __builtin_choose_expr(__obfh_sf_guard == 2u, OBFH_SF_GUARD_2_VALUE(__obfh_live_transformed), \
                                                                                             __builtin_choose_expr(__obfh_sf_guard == 3u, OBFH_SF_GUARD_3_VALUE(__obfh_live_transformed), \
                                                                                                                   __builtin_choose_expr(__obfh_sf_guard == 4u, OBFH_SF_GUARD_4_VALUE(__obfh_live_transformed), \
                                                                                                                                         __builtin_choose_expr(__obfh_sf_guard == 5u, OBFH_SF_GUARD_5_VALUE(__obfh_live_transformed), \
                                                                                                                                                               __builtin_choose_expr(__obfh_sf_guard == 6u, OBFH_SF_GUARD_6_VALUE(__obfh_live_transformed), \
                                                                                                                                                                                     __builtin_choose_expr(__obfh_sf_guard == 7u, OBFH_SF_GUARD_7_VALUE(__obfh_live_transformed), \
                                                                                                                                                                                                           __builtin_choose_expr(__obfh_sf_guard == 8u, OBFH_SF_GUARD_8_VALUE(__obfh_live_transformed), \
                                                                                                                                                                                                                                 __builtin_choose_expr(__obfh_sf_guard == 9u, OBFH_SF_GUARD_9_VALUE(__obfh_live_transformed), \
                                                                                                                                                                                                                                                       __builtin_choose_expr(__obfh_sf_guard == 10u, OBFH_SF_GUARD_10_VALUE(__obfh_live_transformed), \
                                                                                                                                                                                                                                                                             __builtin_choose_expr(__obfh_sf_guard == 11u, OBFH_SF_GUARD_11_VALUE(__obfh_live_transformed), \
                                                                                                                                                                                                                                                                                                   __builtin_choose_expr(__obfh_sf_guard == 12u, OBFH_SF_GUARD_12_VALUE(__obfh_live_transformed), \
                                                                                                                                                                                                                                                                                                                         OBFH_SF_GUARD_13_VALUE(__obfh_live_transformed)))))))))))))); \
    __obfh_live_actual = __obfh_live_input; \
    (void)0
// Commit outside the layout choice: one expansion and direct memory updates.
// Each output remains dependent on the live value; no ABI or frame changes.
#if defined(__x86_64__)
#define OBFH_SF_COOKIE_UPDATE "xorq %%rax, %[flow_cookie];"
#else
#define OBFH_SF_COOKIE_UPDATE "xorl %%eax, %[flow_cookie];"
#endif
#define OBFH_SF_FLOW_COMMIT_TEXT \
    "xorl %%eax, %[flow_state]; xorl %%edx, %[flow_tag]; " OBFH_SF_COOKIE_UPDATE
#define OBFH_SF_FLOW_COMMIT_ALT_TEXT \
    OBFH_SF_COOKIE_UPDATE "xorl %%edx, %[flow_tag]; xorl %%eax, %[flow_state];"
#define OBFH_SF_FLOW_COMMIT_ASM(text) \
    __obfh_asm__("xorl %[live_input], %%edx; " text \
                 : [flow_state] "+m"(__obfh_flow_state), [flow_tag] "+m"(__obfh_flow_tag), [flow_cookie] "+m"(__obfh_cookie), "+d"(__obfh_live_expected) \
                 : "a"(__obfh_live_actual), [live_input] "m"(__obfh_live_input) \
                 : "cc")
#define OBFH_SF_FLOW_COMMIT \
    __builtin_choose_expr((__obfh_flow_hash >> 18) & 1u, \
                          ({ OBFH_SF_FLOW_COMMIT_ASM(OBFH_SF_FLOW_COMMIT_ALT_TEXT); }), \
                          ({ OBFH_SF_FLOW_COMMIT_ASM(OBFH_SF_FLOW_COMMIT_TEXT); }))
#define OBFH_SF_FLOW_START(guard) __obfh_asm__(guard \
                                               : \
                                               : OBFH_SF_GUARD_INPUTS \
                                               : "edx", "ecx", "cc", "memory")
#define OBFH_SF_FLOW_END __obfh_asm__("9:xorl %[li],%%eax;" \
                                      : "=a"(__obfh_live_actual) \
                                      : [li] "m"(__obfh_live_input) \
                                      : "cc", "memory")
#define OBFH_SF_FLOW_EMIT(g, l, a, b, c, d) OBFH_SF_POOL_RUN(OBFH_SF_RECIPE(g, l, a, b, c, d), OBFH_SF_FLOW_START, OBFH_SF_FLOW_END, OBFH_SF_FLOW_START_PREFIX)
#define OBFH_P_PROXY ({ \
    unsigned int __obfh_live_actual, __obfh_live_expected, __obfh_live_input; \
    enum { __obfh_sf_id = __COUNTER__, \
           __obfh_sf_seed = (unsigned int)__obfh_sf_id ^ (unsigned int)OBFH_SEED ^ (unsigned int)__LINE__ ^ 0x53504631u, \
           __obfh_sf_mixed = OBFH_MIX_A(__obfh_sf_seed), \
           __obfh_sf_variant = OBFH_MIX_B(__obfh_sf_mixed) % OBFH_SF_VARIANT_COUNT }; \
    __builtin_choose_expr((__obfh_sf_variant & 3u) == 0u, OBFH_P_CHAIN(__obfh_true_tag), ((void)0)); \
    OBFH_SF_CAPTURE; \
    OBFH_SF_FLOW_PREPARE; \
    OBFH_SF_POOL_RUN(OBFH_SF_RECIPE_SELECT(__obfh_sf_variant), OBFH_SF_FLOW_START, OBFH_SF_FLOW_END, OBFH_SF_FLOW_START_PREFIX); \
    OBFH_SF_FLOW_COMMIT; \
})

// Local condition transport and encoded selector targets.
#if defined(__x86_64__)
#define OBFH_P_ADDRESS_NOT_WIDTH 3
#define OBFH_P_ADDRESS_NOT_A 0xd0f748
#define OBFH_P_ADDRESS_NOT_D 0xd2f748
#define OBFH_P_ADDRESS_PREFIX ".byte 0x48;"
#define OBFH_P_MASK_CAPTURE_0 \
    "movq %[cookie], %%rcx; .fill (.Lps_carry^1),3,(((0xc8c148+65536*%c[maskreg]+4096*((.Lps_rotate==1)&1))&0xffffffff)^0x80000000)-0x80000000; .fill (.Lps_carry^1)*((.Lps_rotate!=1)&1),1,.Lps_rotate; .fill .Lps_carry,4,(((0xe0ba0f48+16777216*%c[maskreg])&0xffffffff)^0x80000000)-0x80000000; .fill .Lps_carry,1,.Lps_bit; sbbq %%rcx, %%rcx;"

#define OBFH_P_MASK_CAPTURE_1 \
    "movq %[cookie], %%rdx; .fill (.Lps_carry^1),3,(((0xc8c148+65536*%c[maskreg]+4096*((.Lps_rotate==1)&1))&0xffffffff)^0x80000000)-0x80000000; .fill (.Lps_carry^1)*((.Lps_rotate!=1)&1),1,.Lps_rotate; .fill .Lps_carry,4,(((0xe0ba0f48+16777216*%c[maskreg])&0xffffffff)^0x80000000)-0x80000000; .fill .Lps_carry,1,.Lps_bit; sbbq %%rdx, %%rdx;"

#define OBFH_P_WORD "q"
#define OBFH_P_CX "%%rcx"
#define OBFH_P_INC_CX ".fill .Lps_parity,2,0xc1ff;"
#define OBFH_P_DX "%%rdx"
#define OBFH_P_SCALE "8"
#define OBFH_P_CMOV_PREFIX "0x48,"
#define OBFH_P_CMOV_WIDTH "4"
#define OBFH_P_CMOV_VALUE "0x0f48"
#define OBFH_P_CMOV_SHIFT "*256"
#define OBFH_P_CMOV_REG_SHIFT "16777216"
#define OBFH_P_COPY_WIDTH "3"
#define OBFH_P_COPY_VALUE "0x8948"
#define OBFH_P_COPY_REG_SHIFT "65536"
#define OBFH_P_DECODE_PREFIX ".byte 0x48;"
#else
#define OBFH_P_ADDRESS_NOT_WIDTH 2
#define OBFH_P_ADDRESS_NOT_A 0xd0f7
#define OBFH_P_ADDRESS_NOT_D 0xd2f7
#define OBFH_P_ADDRESS_PREFIX ""
#define OBFH_P_MASK_CAPTURE_0 \
    "movl %[cookie], %%ecx; .fill (.Lps_carry^1),2,(((0xc8c1+256*%c[maskreg]+16*((.Lps_rotate==1)&1))&0xffffffff)^0x80000000)-0x80000000; .fill (.Lps_carry^1)*((.Lps_rotate!=1)&1),1,.Lps_rotate; .fill .Lps_carry,3,(((0xe0ba0f+65536*%c[maskreg])&0xffffffff)^0x80000000)-0x80000000; .fill .Lps_carry,1,.Lps_bit; sbbl %%ecx, %%ecx;"

#define OBFH_P_MASK_CAPTURE_1 \
    "movl %[cookie], %%edx; .fill (.Lps_carry^1),2,(((0xc8c1+256*%c[maskreg]+16*((.Lps_rotate==1)&1))&0xffffffff)^0x80000000)-0x80000000; .fill (.Lps_carry^1)*((.Lps_rotate!=1)&1),1,.Lps_rotate; .fill .Lps_carry,3,(((0xe0ba0f+65536*%c[maskreg])&0xffffffff)^0x80000000)-0x80000000; .fill .Lps_carry,1,.Lps_bit; sbbl %%edx, %%edx;"

#define OBFH_P_WORD "l"
#define OBFH_P_CX "%%ecx"
#define OBFH_P_INC_CX ".fill .Lps_parity,1,0x41;"
#define OBFH_P_DX "%%edx"
#define OBFH_P_SCALE "4"
#define OBFH_P_CMOV_PREFIX ""
#define OBFH_P_CMOV_WIDTH "3"
#define OBFH_P_CMOV_VALUE "0x0f"
#define OBFH_P_CMOV_SHIFT ""
#define OBFH_P_CMOV_REG_SHIFT "65536"
#define OBFH_P_COPY_WIDTH "2"
#define OBFH_P_COPY_VALUE "0x89"
#define OBFH_P_COPY_REG_SHIFT "256"
#define OBFH_P_DECODE_PREFIX ""
#endif
// Local address representation is selected once; no runtime dispatcher.
// Raw labels are loaded before encoding, so every key can use all three representations.
#define OBFH_P_SELECTOR_META(s) \
    enum { __obfh_sel_branch##s = (__obfh_flow_hash >> ((s) + 10)) & 1u, \
           __obfh_mask_carry##s = (__obfh_flow_hash >> ((s) + 14)) & 1u, \
           __obfh_address_form##s = (__obfh_flow_hash >> ((s) + 22)) % 3u, \
           __obfh_shift_form##s = (__obfh_flow_hash >> ((s) + 26)) % 3u, \
           __obfh_index_form##s = (__obfh_flow_hash >> ((s) + 25)) & 1u, \
           __obfh_bitcount##s = __obfh_shift_form##s == 2 ? ((32u - (s)) & 31u) : (s), \
           __obfh_bitcode##s = (0xc1u + 0x10u * (__obfh_bitcount##s == 1u)) | ((__obfh_shift_form##s == 0 ? 0xe9u : (__obfh_shift_form##s == 1 ? 0xc9u : 0xc1u)) << 8) | \
                               (__obfh_bitcount##s << 16), \
           __obfh_sel_order##s = (__obfh_flow_hash >> ((s) + 6)) & 1u, \
           __obfh_sel_lea##s = __obfh_address_form##s != 0 && ((__obfh_flow_hash >> ((s) + 19)) & 1u), \
           __obfh_sel_decode##s = __obfh_sel_lea##s ? 0x928du : 0x81u | ((__obfh_address_form##s == 0 ? 0xf2u : (__obfh_address_form##s == 1 ? 0xc2u : 0xeau)) << 8), \
           __obfh_address_disp##s = __obfh_sel_lea##s && __obfh_address_form##s == 2 ? 0u - __obfh_flow_key : __obfh_flow_key, \
           __obfh_sel_cmov##s = 0x44u + ((__obfh_flow_hash >> ((s) + 20)) & 1u), \
           __obfh_address_encode_a##s = __obfh_address_form##s == 0 ? 0xf0 : __obfh_address_form##s == 1 ? 0xe8 \
                                                                                                         : 0xc0, \
           __obfh_address_encode_d##s = __obfh_address_form##s == 0 ? 0xf2 : __obfh_address_form##s == 1 ? 0xea \
                                                                                                         : 0xc2, \
           __obfh_address_not_a##s = __obfh_address_form##s == 0 && __obfh_sel_order##s, \
           __obfh_address_not_d##s = __obfh_address_form##s == 0 && !__obfh_sel_order##s, \
           __obfh_address_key_a##s = __obfh_address_form##s == 0 && __obfh_sel_order##s ? ~__obfh_flow_key : __obfh_flow_key, \
           __obfh_address_key_d##s = __obfh_address_form##s == 0 && !__obfh_sel_order##s ? ~__obfh_flow_key : __obfh_flow_key };
// One encoded pair, with matched raw-label inputs. The XOR form complements
// one register and the immediate: ~(address XOR ~key) == address XOR key.
// This remains pointer-width arithmetic, including sign extension on x64.
#define OBFH_P_ADDRESS_PAIR(shift, label_false, label_true, zero, one) \
    __obfh_asm__( \
        ".fill %c[not_a]," OBFH_P_STRINGIFY(OBFH_P_ADDRESS_NOT_WIDTH) ",(((" OBFH_P_STRINGIFY(OBFH_P_ADDRESS_NOT_A) ")&0xffffffff)^0x80000000)-0x80000000; " OBFH_P_ADDRESS_PREFIX ".byte 0x81; .byte %c[encode_a]; .long %c[key_a]; " OBFH_P_ADDRESS_PREFIX ".byte 0x81; .byte %c[encode_d]; .long %c[key_d]; .fill %c[not_d]," OBFH_P_STRINGIFY(OBFH_P_ADDRESS_NOT_WIDTH) ",(((" OBFH_P_STRINGIFY(OBFH_P_ADDRESS_NOT_D) ")&0xffffffff)^0x80000000)-0x80000000; " \
        : "=a"(__builtin_choose_expr(__obfh_sel_order##shift, one, zero)), \
          "=d"(__builtin_choose_expr(__obfh_sel_order##shift, zero, one)) \
        : "0"(__builtin_choose_expr(__obfh_sel_order##shift, (ULONG_PTR) && label_true, (ULONG_PTR) && label_false)), \
          "1"(__builtin_choose_expr(__obfh_sel_order##shift, (ULONG_PTR) && label_false, (ULONG_PTR) && label_true)), \
          [encode_a] "i"(__obfh_address_encode_a##shift), \
          [encode_d] "i"(__obfh_address_encode_d##shift), \
          [not_a] "i"(__obfh_address_not_a##shift), \
          [not_d] "i"(__obfh_address_not_d##shift), \
          [key_a] "i"(__obfh_address_key_a##shift), \
          [key_d] "i"(__obfh_address_key_d##shift) \
        : "cc")

#define OBFH_P_STRINGIFY_INNER(value) #value
#define OBFH_P_STRINGIFY(value) OBFH_P_STRINGIFY_INNER(value)
#define OBFH_P_MASK_TEXT(mask_register) \
    "mov" OBFH_P_WORD " %[zero], %[target]; xor" OBFH_P_WORD \
    " %[one], %[target]; and" OBFH_P_WORD mask_register ", %[target]; .byte " OBFH_P_CMOV_PREFIX "0x89+2*((.Lps_copy>>15)&1),%c[cmovreg]+((.Lps_copy>>15)&1)*(7-14*((%c[cmovreg]==0xd1)&1)); xor" OBFH_P_WORD " %[zero], %[target]; xor" OBFH_P_WORD " %[one], " mask_register \
    "; cmpl %[tag], %%eax; .fill (.Lps_branch^1)," OBFH_P_CMOV_WIDTH ",(((" OBFH_P_CMOV_VALUE "+256*.Lps_cmov" OBFH_P_CMOV_SHIFT "+" OBFH_P_CMOV_REG_SHIFT "*%c[cmovreg])&0xffffffff)^0x80000000)-0x80000000; .fill .Lps_branch,2,((((0x75-(.Lps_cmov&1))+256*" OBFH_P_COPY_WIDTH ")&0xffffffff)^0x80000000)-0x80000000; .fill .Lps_branch," OBFH_P_COPY_WIDTH ",(((" OBFH_P_COPY_VALUE "+" OBFH_P_COPY_REG_SHIFT "*(%c[cmovreg]+7-14*((%c[cmovreg]==0xd1)&1)))&0xffffffff)^0x80000000)-0x80000000; " OBFH_P_DECODE_PREFIX ".short %c[decode]; .long .Lps_key;"

// Arithmetic blending produces the same ordered candidate pair modulo pointer width.
// Consume the mask before reusing its register for the complementary candidate.
#define OBFH_P_MASK_ARITH_TEXT(mask_register) \
    "mov" OBFH_P_WORD " %[one], %[target]; sub" OBFH_P_WORD " %[zero], %[target]; and" OBFH_P_WORD mask_register ", %[target]; mov" OBFH_P_WORD " %[one], " mask_register "; sub" OBFH_P_WORD " %[target], " mask_register "; add" OBFH_P_WORD " %[zero], %[target]; cmpl %%eax, %[tag]; .fill (.Lps_branch^1)," OBFH_P_CMOV_WIDTH ",(((" OBFH_P_CMOV_VALUE "+256*.Lps_cmov" OBFH_P_CMOV_SHIFT "+" OBFH_P_CMOV_REG_SHIFT "*%c[cmovreg])&0xffffffff)^0x80000000)-0x80000000; .fill .Lps_branch,2,((((0x75-(.Lps_cmov&1))+256*" OBFH_P_COPY_WIDTH ")&0xffffffff)^0x80000000)-0x80000000; .fill .Lps_branch," OBFH_P_COPY_WIDTH ",(((" OBFH_P_COPY_VALUE "+" OBFH_P_COPY_REG_SHIFT "*(%c[cmovreg]+7-14*((%c[cmovreg]==0xd1)&1)))&0xffffffff)^0x80000000)-0x80000000; " OBFH_P_DECODE_PREFIX ".short %c[decode]; .long .Lps_key;"

#define OBFH_P_MASK_ALT_TEXT(mask_register) \
    "mov" OBFH_P_WORD " %[zero], %[target]; xor" OBFH_P_WORD \
    " %[one], %[target]; and" OBFH_P_WORD mask_register ", %[target]; .byte " OBFH_P_CMOV_PREFIX "0x89+2*((.Lps_copy>>15)&1),%c[cmovreg]+((.Lps_copy>>15)&1)*(7-14*((%c[cmovreg]==0xd1)&1)); xor" OBFH_P_WORD " %[one], " mask_register "; xor" OBFH_P_WORD " %[zero], %[target]; cmpl %%eax, %[tag]; .fill (.Lps_branch^1)," OBFH_P_CMOV_WIDTH ",(((" OBFH_P_CMOV_VALUE "+256*.Lps_cmov" OBFH_P_CMOV_SHIFT "+" OBFH_P_CMOV_REG_SHIFT "*%c[cmovreg])&0xffffffff)^0x80000000)-0x80000000; .fill .Lps_branch,2,((((0x75-(.Lps_cmov&1))+256*" OBFH_P_COPY_WIDTH ")&0xffffffff)^0x80000000)-0x80000000; .fill .Lps_branch," OBFH_P_COPY_WIDTH ",(((" OBFH_P_COPY_VALUE "+" OBFH_P_COPY_REG_SHIFT "*(%c[cmovreg]+7-14*((%c[cmovreg]==0xd1)&1)))&0xffffffff)^0x80000000)-0x80000000; " OBFH_P_DECODE_PREFIX ".short %c[decode]; .long .Lps_key;"

// Select the index before obtaining the table base. EDX holds the index,
// ECX becomes the base afterwards; no fourth register or callee-save push is needed.
#define OBFH_P_TABLE_TEXT \
    "movl %[cookie], %%ecx; .fill 1,(3 - (((.Lps_bitcode & 255) == 0xd1) & 1)),(((.Lps_bitcode)&0xffffffff)^0x80000000)-0x80000000; andl $1, %%ecx; movl %%ecx, %%edx; .byte 0xf7,.Lps_index_op; " OBFH_P_INC_CX " .fill (.Lps_parity^1),3,0x01e183; cmpl %[tag], %%eax; .fill (.Lps_branch^1),3,(((0x0f+256*.Lps_cmov+65536*0xd1)&0xffffffff)^0x80000000)-0x80000000; .fill .Lps_branch,2,((((0x75-(.Lps_cmov&1))+256*2)&0xffffffff)^0x80000000)-0x80000000; .fill .Lps_branch,2,(((0x89+256*0xca)&0xffffffff)^0x80000000)-0x80000000; lea" OBFH_P_WORD " %[table], " OBFH_P_CX "; mov" OBFH_P_WORD " (" OBFH_P_CX ", " OBFH_P_DX ", " OBFH_P_SCALE "), %[target]; " OBFH_P_DECODE_PREFIX ".short %c[decode]; .long .Lps_key;"

#define OBFH_P_TABLE_ALT_TEXT \
    "movl %[cookie], %%ecx; .fill 1,(3 - (((.Lps_bitcode & 255) == 0xd1) & 1)),(((.Lps_bitcode)&0xffffffff)^0x80000000)-0x80000000; andl $1, %%ecx; movl %%ecx, %%edx; xorl $1, %%ecx; cmpl %%eax, %[tag]; .fill (.Lps_branch^1),3,(((0x0f+256*.Lps_cmov+65536*0xd1)&0xffffffff)^0x80000000)-0x80000000; .fill .Lps_branch,2,((((0x75-(.Lps_cmov&1))+256*2)&0xffffffff)^0x80000000)-0x80000000; .fill .Lps_branch,2,(((0x89+256*0xca)&0xffffffff)^0x80000000)-0x80000000; lea" OBFH_P_WORD " %[table], " OBFH_P_CX "; mov" OBFH_P_WORD " (" OBFH_P_CX ", " OBFH_P_DX ", " OBFH_P_SCALE "), %[target]; " OBFH_P_DECODE_PREFIX ".short %c[decode]; .long .Lps_key;"

#define OBFH_P_TABLE_INDEX_TEXT \
    "movl %[cookie], %%ecx; .fill 1,(3 - (((.Lps_bitcode & 255) == 0xd1) & 1)),(((.Lps_bitcode)&0xffffffff)^0x80000000)-0x80000000; andl $1, %%ecx; cmpl %%eax, %[tag]; .byte 0x0f,.Lps_cmov+0x50,0xc2; movzbl %%dl, %%edx; .fill (.Lps_parity^1),2,0xca31; .fill .Lps_parity,2,0xca01; .fill .Lps_parity,3,0x01e283; lea" OBFH_P_WORD " %[table], " OBFH_P_CX "; mov" OBFH_P_WORD " (" OBFH_P_CX ", " OBFH_P_DX ", " OBFH_P_SCALE "), %[target]; " OBFH_P_DECODE_PREFIX ".short %c[decode]; .long .Lps_key;"

// Selector text is assembled directly from readable architecture fragments.
#define OBFH_P_MASK_INSTRUCTIONS OBFH_P_MASK_TEXT(OBFH_P_CX)
#define OBFH_P_MASK_SWAPPED_INSTRUCTIONS OBFH_P_MASK_TEXT(OBFH_P_DX)
#define OBFH_P_MASK_ALT_INSTRUCTIONS OBFH_P_MASK_ALT_TEXT(OBFH_P_CX)
#define OBFH_P_MASK_ALT_SWAPPED_INSTRUCTIONS OBFH_P_MASK_ALT_TEXT(OBFH_P_DX)
#define OBFH_P_MASK_ARITH_INSTRUCTIONS OBFH_P_MASK_ARITH_TEXT(OBFH_P_CX)
#define OBFH_P_MASK_ARITH_SWAPPED_INSTRUCTIONS OBFH_P_MASK_ARITH_TEXT(OBFH_P_DX)
#define OBFH_P_TABLE_INSTRUCTIONS OBFH_P_TABLE_TEXT
#define OBFH_P_TABLE_ALT_INSTRUCTIONS OBFH_P_TABLE_ALT_TEXT
#define OBFH_P_TABLE_INDEX_INSTRUCTIONS OBFH_P_TABLE_INDEX_TEXT
#define OBFH_P_MASK_CONSTANTS(shift) __obfh_asm__(".set .Lps_key,%c[key];.set .Lps_copy,%c[copy];.set .Lps_cmov,%c[cmov];.set .Lps_branch,%c[branch];.set .Lps_carry,%c[carry];.set .Lps_bit,%c[bit];.set .Lps_rotate,%c[rotate];" \
                                                  : \
                                                  : [key] "i"(__obfh_address_disp##shift), [copy] "i"(__obfh_flow_hash), [cmov] "i"(__obfh_sel_cmov##shift), [branch] "i"(__obfh_sel_branch##shift), [carry] "i"(__obfh_mask_carry##shift), [bit] "i"(shift), [rotate] "i"((shift) + 1))
#define OBFH_P_TABLE_CONSTANTS(shift) __obfh_asm__(".set .Lps_key,%c[key];.set .Lps_cmov,%c[cmov];.set .Lps_branch,%c[branch];.set .Lps_bitcode,%c[bitcode];.set .Lps_index_op,%c[index_op];.set .Lps_parity,%c[parity];" \
                                                   : \
                                                   : [key] "i"(__obfh_address_disp##shift), [cmov] "i"(__obfh_sel_cmov##shift), [branch] "i"(__obfh_sel_branch##shift), [bitcode] "i"(__obfh_bitcode##shift), [index_op] "i"(__obfh_index_form##shift ? 0xd9 : 0xd1), [parity] "i"(__obfh_index_form##shift))
#define OBFH_P_MASK_ASM(shift, instructions, output, scratch, swapped) \
    __obfh_asm__(OBFH_P_MASK_CAPTURE_##swapped instructions \
                 : [target] output(__obfh_target) \
                 : "a"(__obfh_flow_state), \
                   [tag] "m"(__obfh_selector_expected), [cookie] "m"(__obfh_cookie), [maskreg] "i"((swapped) ? 2 : 1), [zero] "m"(__obfh_target_zero), \
                   [one] "m"(__obfh_target_one), [decode] "i"((__obfh_sel_decode##shift) - ((swapped) * (__obfh_sel_lea##shift ? 0x900u : 0x100u))), [cmovreg] "i"((swapped) ? 0xca : 0xd1) \
                 : scratch, "cc", "memory");
#define OBFH_P_MASK(shift, label_false, label_true) \
    ({ \
        unsigned int __obfh_selector_expected = __obfh_point##shift; \
        ULONG_PTR __obfh_target; \
        ULONG_PTR __obfh_target_zero, __obfh_target_one; \
        OBFH_P_ADDRESS_PAIR(shift, label_false, label_true, __obfh_target_zero, __obfh_target_one); \
        OBFH_P_MASK_CONSTANTS(shift); \
        __builtin_choose_expr(__obfh_sel_order##shift, \
                              ({ OBFH_P_MASK_ASM(shift, OBFH_P_MASK_ARITH_SWAPPED_INSTRUCTIONS, "=&c", "edx", 1); }), \
                              ({ OBFH_P_MASK_ASM(shift, OBFH_P_MASK_INSTRUCTIONS, "=&d", "ecx", 0); })); \
        goto *(void *)__obfh_target; \
    })
#define OBFH_P_TABLE_ASM(shift, instructions) \
    __obfh_asm__(instructions \
                 : [target] "=&d"(__obfh_target) \
                 : "a"(__obfh_flow_state), \
                   [tag] "m"(__obfh_selector_expected), [cookie] "m"(__obfh_cookie), \
                   [table] "m"(__obfh_targets[0]), [decode] "i"(__obfh_sel_decode##shift) \
                 : "ecx", "cc", "memory");
#define OBFH_P_TABLE(shift, label_false, label_true) \
    ({ \
        unsigned int __obfh_selector_expected = __obfh_point##shift; \
        ULONG_PTR __obfh_targets[2], __obfh_target; \
        OBFH_P_ADDRESS_PAIR(shift, label_false, label_true, __obfh_targets[0], __obfh_targets[1]); \
        OBFH_P_TABLE_CONSTANTS(shift); \
        __builtin_choose_expr(__obfh_sel_order##shift, \
                              ({ OBFH_P_TABLE_ASM(shift, OBFH_P_TABLE_INDEX_INSTRUCTIONS); }), \
                              ({ OBFH_P_TABLE_ASM(shift, OBFH_P_TABLE_INSTRUCTIONS); })); \
        goto *(void *)__obfh_target; \
    })
// V1 has one graph: do not instantiate the unused second-stage constants.
#if OBFH_BETTER_CONTROL_FLOW
#define OBFH_P_SECOND_CONSTANTS \
    enum { \
        __obfh_k20 = OBFH_MIX_A(__obfh_flow_hash ^ 2688253889u), \
        __obfh_a20 = OBFH_MIX_B(__obfh_k20 ^ 0x85ebca6bu), \
        __obfh_r20 = ((__obfh_k20 >> 27) % 31u) + 1u, \
        __obfh_k21 = OBFH_MIX_A(__obfh_flow_hash ^ 3566311242u), \
        __obfh_a21 = OBFH_MIX_B(__obfh_k21 ^ 0x85ebca6bu), \
        __obfh_r21 = ((__obfh_k21 >> 27) % 31u) + 1u, \
        __obfh_k30 = OBFH_MIX_A(__obfh_flow_hash ^ 2705162949u), \
        __obfh_a30 = OBFH_MIX_B(__obfh_k30 ^ 0x85ebca6bu), \
        __obfh_r30 = ((__obfh_k30 >> 27) % 31u) + 1u, \
        __obfh_k31 = OBFH_MIX_A(__obfh_flow_hash ^ 3583220302u), \
        __obfh_a31 = OBFH_MIX_B(__obfh_k31 ^ 0x85ebca6bu), \
        __obfh_r31 = ((__obfh_k31 >> 27) % 31u) + 1u \
    };
#define OBFH_P_SECOND_PREPARE \
    OBFH_P_SELECTOR_META(2); \
    OBFH_P_SELECTOR_META(3); \
    OBFH_P_PREPARE(2, 0); \
    OBFH_P_PREPARE(2, 1); \
    OBFH_P_PREPARE(3, 0); \
    OBFH_P_PREPARE(3, 1);
#define OBFH_P_FIRST_GRAPH OBFH_P_GRAPH(__obfh_flow_first, 0, 1, 0)
#define OBFH_P_SECOND_GRAPH OBFH_P_GRAPH(__obfh_flow_second, 2, 3, 1)
#else
#define OBFH_P_SECOND_CONSTANTS
#define OBFH_P_SECOND_PREPARE
#define OBFH_P_FIRST_GRAPH OBFH_P_GRAPH(__obfh_flow_first, 0, 1, 1)
#define OBFH_P_SECOND_GRAPH ((void)0)
#endif
// Reuse four skipped bytes per insertion at existing graph jumps. No new jump.
#define OBFH_P_GAP(shift, part, split) \
    __obfh_asm__(".fill 1,%c0,(((%c1)&0xffffffff)^0x80000000)-0x80000000;" \
                 : \
                 : "i"((OBFH_BETTER_CONTROL_FLOW ? 2 : 4) * (!(split) | (((__obfh_flow_hash >> ((shift) + 24)) & 1u) == (part)))), \
                   "i"(__obfh_flow_hash ^ (0x9e3779b9u * ((shift) + 1u))))

#define OBFH_P_FINISH_ASM(instructions, ...) \
    ({ \
        __obfh_flow_result = (__obfh_flow_hash & 2u) ? __obfh_flow_tag : __obfh_flow_state; \
        unsigned int __obfh_finish_other = (__obfh_flow_hash & 2u) ? __obfh_flow_state : __obfh_flow_tag; \
        __obfh_asm__(instructions \
                     : "+a"(__obfh_flow_result) \
                     : [tag] "r"(__obfh_finish_other), ##__VA_ARGS__ \
                     : "ecx", "cc", "memory"); \
    })
#define OBFH_P_EXIT_0 OBFH_P_FINISH_ASM("subl %[tag], %%eax; negl %%eax; sbbl %%eax, %%eax; addl $1, %%eax;")
#define OBFH_P_EXIT_1 OBFH_P_FINISH_ASM("xorl %[tag], %%eax; subl $1, %%eax; sbbl %%eax, %%eax; negl %%eax;")
#define OBFH_P_EXIT_2 \
    OBFH_P_FINISH_ASM(OBFH_ASM_ZERO32("1", "%c[zero_form]") "cmpl %[tag], %%eax; movl $1, %%eax; cmovnel %%ecx, %%eax;", [zero_form] "i"((__obfh_flow_hash >> 11) % 3u))
#define OBFH_P_EXIT_3 OBFH_P_FINISH_ASM("cmpl %[tag], %%eax; setne %%cl; movzbl %%cl, %%eax;")
#define OBFH_P_EXIT_4 \
    OBFH_P_FINISH_ASM("subl %[tag], %%eax; negl %%eax; sbbl %%eax, %%eax; notl %%eax; andl $1, %%eax;")
#define OBFH_P_EXIT_5 OBFH_P_FINISH_ASM("cmpl %[tag], %%eax; sete %%cl; movzbl %%cl, %%eax; xorl $1, %%eax;")
#define OBFH_P_EXIT_6 \
    OBFH_P_FINISH_ASM( \
        "xorl %[tag], %%eax; movl %%eax, %%ecx; negl %%ecx; orl %%ecx, %%eax; shrl $31, %%eax; xorl $1, %%eax;")
#define OBFH_P_EXIT_7 \
    OBFH_P_FINISH_ASM("xorl %[tag], %%eax; movl $1, %%eax; jnz 9f; movl $0, %%eax; 9:")
#define OBFH_P_EXIT_SELECT_6(style) __builtin_choose_expr(((style)&7u) == 6u, OBFH_P_EXIT_6, OBFH_P_EXIT_7)
#define OBFH_P_EXIT_SELECT_5(style) \
    __builtin_choose_expr(((style)&7u) == 5u, OBFH_P_EXIT_5, OBFH_P_EXIT_SELECT_6(style))
#define OBFH_P_EXIT_SELECT_4(style) \
    __builtin_choose_expr(((style)&7u) == 4u, OBFH_P_EXIT_4, OBFH_P_EXIT_SELECT_5(style))
#define OBFH_P_EXIT_SELECT_3(style) \
    __builtin_choose_expr(((style)&7u) == 3u, OBFH_P_EXIT_3, OBFH_P_EXIT_SELECT_4(style))
#define OBFH_P_EXIT_SELECT_2(style) \
    __builtin_choose_expr(((style)&7u) == 2u, OBFH_P_EXIT_2, OBFH_P_EXIT_SELECT_3(style))
#define OBFH_P_EXIT_SELECT_1(style) \
    __builtin_choose_expr(((style)&7u) == 1u, OBFH_P_EXIT_1, OBFH_P_EXIT_SELECT_2(style))
#define OBFH_P_EXIT_SELECT_0(style) \
    __builtin_choose_expr(((style)&7u) == 0u, OBFH_P_EXIT_0, OBFH_P_EXIT_SELECT_1(style))
#define OBFH_P_FINISH(style) OBFH_P_EXIT_SELECT_0(style)
// The final encoded state and its expected value stay in EAX/EDX through completion.
// Each row retains an existing exit family; fixed words use their signed 32-bit spelling.
#define OBFH_P_TERMINAL_TEXT \
    ".fill ((%c[finish]==2)&(%c[zero_form]!=2)&1),2,(0xc931-8*((%c[zero_form]==1)&1));.fill ((%c[finish]==2)&(%c[zero_form]==2)&1),1,0xb9; .fill ((%c[finish]==2)&(%c[zero_form]==2)&1),4,0;" /* sub/xor edx; neg; sbb; add 1 */ ".fill ((%c[finish]==0)&1),4,(0xd8f7d029-0x100000000+8*(%c[zero_form]&1)); .fill ((%c[finish]==0)&1),4,(0xc083c019-0x100000000); .fill ((%c[finish]==0)&1),1,0x1;" /* xor/sub edx; sub 1; sbb; neg */ ".fill ((%c[finish]==1)&1),4,(0xe883d031-0x100000000-8*(%c[zero_form]&1)); .fill ((%c[finish]==1)&1),4,(0xf7c01901-0x100000000); .fill ((%c[finish]==1)&1),1,0xd8;" /* cmp edx; mov 1; cmovne ecx */ ".fill ((%c[finish]==2)&1),4,0x1b8d039; .fill ((%c[finish]==2)&1),4,0xf000000; .fill ((%c[finish]==2)&1),2,0xc145;" /* cmp edx; setne; movzx */ ".fill ((%c[finish]==3)&1),4,(0x950fd039-0x100000000); .fill ((%c[finish]==3)&1),4,(0xc1b60fc1-0x100000000);" /* sub/xor edx; neg; sbb; not; and 1 */ ".fill ((%c[finish]==4)&1),4,(0xd8f7d029-0x100000000+8*(%c[zero_form]&1)); .fill ((%c[finish]==4)&1),4,(0xd0f7c019-0x100000000); .fill ((%c[finish]==4)&1),3,0x1e083;" /* cmp edx; sete; movzx; xor 1 */ ".fill ((%c[finish]==5)&1),4,(0x940fd039-0x100000000); .fill ((%c[finish]==5)&1),4,(0xc1b60fc1-0x100000000); .fill ((%c[finish]==5)&1),3,0x1f083;" /* xor/sub edx; mov ecx; neg ecx; or; shr 31; xor 1 */ ".fill ((%c[finish]==6)&1),4,(0xc189d031-0x100000000-8*(%c[zero_form]&1)); .fill ((%c[finish]==6)&1),4,(0xc809d9f7-0x100000000); .fill ((%c[finish]==6)&1),4,(0x831fe8c1-0x100000000); .fill ((%c[finish]==6)&1),2,0x1f0;" /* XOR/SUB supplies ZF; mov 1; jnz +5; mov 0 */ ".fill ((%c[finish]==7)&1),2,(0xd031-8*(%c[zero_form]&1)); .fill ((%c[finish]==7)&1),4,0x1b8; .fill ((%c[finish]==7)&1),4,(0xb8057500-0x100000000); .fill ((%c[finish]==7)&1),4,0x0;"

// The last-stage flag is a preprocessing literal: never expand an unused terminal.
#define OBFH_P_LAST_STEP_0(s, p, style) OBFH_P_STEP(s, p)
#define OBFH_P_LAST_STEP_1(s, p, style) OBFH_P_TERMINAL(s, p, style)
#define OBFH_P_LAST_STEP_I(last, s, p, style) OBFH_P_LAST_STEP_##last(s, p, style)
#define OBFH_P_LAST_STEP(s, p, last, style) OBFH_P_LAST_STEP_I(last, s, p, style)

// Choose the order of block names before expanding their bodies. Passing expanded
// bodies through an ordering macro would make the preprocessor scan them again.
// The separator always follows the first emitted block, preserving fallthrough.
#if ((OBFH_SEED ^ (OBFH_SEED >> 9) ^ (OBFH_SEED >> 19)) >> 0) & 1u
#define OBFH_P_ORDER_0(left, right, between, args) right args between args left args
#else
#define OBFH_P_ORDER_0(left, right, between, args) left args between args right args
#endif
#if ((OBFH_SEED ^ (OBFH_SEED >> 9) ^ (OBFH_SEED >> 19)) >> 1) & 1u
#define OBFH_P_ORDER_1(left, right, between, args) right args between args left args
#else
#define OBFH_P_ORDER_1(left, right, between, args) left args between args right args
#endif
#if ((OBFH_SEED ^ (OBFH_SEED >> 9) ^ (OBFH_SEED >> 19)) >> 2) & 1u
#define OBFH_P_ORDER_2(left, right, between, args) right args between args left args
#else
#define OBFH_P_ORDER_2(left, right, between, args) left args between args right args
#endif
#if ((OBFH_SEED ^ (OBFH_SEED >> 9) ^ (OBFH_SEED >> 19)) >> 3) & 1u
#define OBFH_P_ORDER_3(left, right, between, args) right args between args left args
#else
#define OBFH_P_ORDER_3(left, right, between, args) left args between args right args
#endif
// Block fragments share the enclosing graph's labels; none creates a new scope.
#define OBFH_P_BLOCK_JOIN(s, t, last) \
    goto join; \
    OBFH_P_GAP(s, 0, 1);
#define OBFH_P_BLOCK_DONE_DIAMOND(s, t, last) \
    goto done; \
    OBFH_P_GAP(s, 1, 1);
#define OBFH_P_BLOCK_DONE_SPLIT(s, t, last) \
    goto done; \
    OBFH_P_GAP(s, 0, 0);
#define OBFH_P_BLOCK_EMPTY(s, t, last)
#define OBFH_P_BLOCK_FIRST_A(s, t, last) \
    a: \
    OBFH_P_STEP(s, 0);
#define OBFH_P_BLOCK_FIRST_B(s, t, last) \
    b: \
    OBFH_P_STEP(s, 1);
#define OBFH_P_BLOCK_LAST_C(s, t, last) \
    c: \
    OBFH_P_LAST_STEP(t, 0, last, __obfh_flow_exit);
#define OBFH_P_BLOCK_LAST_D(s, t, last) \
    d: \
    OBFH_P_LAST_STEP(t, 1, last, __obfh_flow_exit);
#define OBFH_P_BLOCK_SPLIT_A(s, t, last) \
    a: \
    OBFH_P_STEP(s, 0); \
    OBFH_P_TABLE(t, c, d);
#define OBFH_P_BLOCK_SPLIT_B(s, t, last) \
    b: \
    OBFH_P_STEP(s, 1); \
    OBFH_P_TABLE(t, d, c);
// Alternate exits preserve polarity: equality {0,1,2,4,6}, inequality {3,5,7}.
#define OBFH_P_BLOCK_CROSS_BC(s, t, last) \
    b: \
    OBFH_P_STEP(s, 1); \
    OBFH_P_MASK(t, c, d); \
    c: \
    OBFH_P_LAST_STEP(t, 0, last, ((0x30765421u >> (4u * __obfh_flow_exit)) & 7u));
#define OBFH_P_BLOCK_CROSS_AD(s, t, last) \
    a: \
    OBFH_P_STEP(s, 0); \
    OBFH_P_MASK(t, d, c); \
    d: \
    OBFH_P_LAST_STEP(t, 1, last, __obfh_flow_exit);
#define OBFH_P_BLOCK_EXITS_AD(s, t, last) \
    a: \
    OBFH_P_STEP(s, 0); \
    OBFH_P_TABLE(t, c, d); \
    d: \
    OBFH_P_LAST_STEP(t, 1, last, ((0x54327106u >> (4u * __obfh_flow_exit)) & 7u));
#define OBFH_P_BLOCK_EXITS_BC(s, t, last) \
    b: \
    OBFH_P_STEP(s, 1); \
    OBFH_P_TABLE(t, c, d); \
    c: \
    OBFH_P_LAST_STEP(t, 0, last, __obfh_flow_exit);
// Sequential diamonds.
#define OBFH_P_GRAPH_0(s, t, last) \
    ({ \
        __label__ a, b, c, d, join, done; \
        OBFH_P_MASK(s, a, b); \
        OBFH_P_ORDER_##s(OBFH_P_BLOCK_FIRST_A, OBFH_P_BLOCK_FIRST_B, OBFH_P_BLOCK_JOIN, (s, t, last)); \
    join: \
        OBFH_P_MASK(t, c, d); \
        OBFH_P_ORDER_##t(OBFH_P_BLOCK_LAST_C, OBFH_P_BLOCK_LAST_D, OBFH_P_BLOCK_DONE_DIAMOND, (s, t, last)); \
    done: \
        ((void)0); \
    })
// Split selectors.
#define OBFH_P_GRAPH_1(s, t, last) \
    ({ \
        __label__ a, b, c, d, join, done; \
        OBFH_P_TABLE(s, a, b); \
        OBFH_P_ORDER_##s(OBFH_P_BLOCK_SPLIT_A, OBFH_P_BLOCK_SPLIT_B, OBFH_P_BLOCK_EMPTY, (s, t, last)); \
        OBFH_P_ORDER_##t(OBFH_P_BLOCK_LAST_C, OBFH_P_BLOCK_LAST_D, OBFH_P_BLOCK_DONE_SPLIT, (s, t, last)); \
    done: \
        ((void)0); \
    })
// Crossed paths.
#define OBFH_P_GRAPH_2(s, t, last) \
    ({ \
        __label__ a, b, c, d, join, done; \
        OBFH_P_MASK(s, a, b); \
        OBFH_P_ORDER_##s(OBFH_P_BLOCK_CROSS_BC, OBFH_P_BLOCK_CROSS_AD, OBFH_P_BLOCK_DONE_SPLIT, (s, t, last)); \
    done: \
        ((void)0); \
    })
// Split exits.
#define OBFH_P_GRAPH_3(s, t, last) \
    ({ \
        __label__ a, b, c, d, join, done; \
        OBFH_P_TABLE(s, a, b); \
        OBFH_P_ORDER_##s(OBFH_P_BLOCK_EXITS_AD, OBFH_P_BLOCK_EXITS_BC, OBFH_P_BLOCK_DONE_SPLIT, (s, t, last)); \
    done: \
        ((void)0); \
    })
#define OBFH_P_GRAPH(f, s, t, last) \
    __builtin_choose_expr( \
        (f) == 0, OBFH_P_GRAPH_0(s, t, last), \
        __builtin_choose_expr((f) == 1, OBFH_P_GRAPH_1(s, t, last), \
                              __builtin_choose_expr((f) == 2, OBFH_P_GRAPH_2(s, t, last), OBFH_P_GRAPH_3(s, t, last))))
// Capture the condition once; all later stages use only local encoded state.
// A stage maps a compile-time input representation to its successor.
// Only the actual lane executes; the true lane is folded into enum constants.
#define OBFH_C_NEXT_0 1
#define OBFH_C_NEXT_1 2
#define OBFH_C_NEXT_2 3
#define OBFH_C_NEXT_3 4
#define OBFH_C_POINT_I(s) __obfh_point##s
#define OBFH_C_POINT(s) OBFH_C_POINT_I(s)
#define OBFH_C_NEXT_I(s) OBFH_C_NEXT_##s
#define OBFH_C_NEXT(s) OBFH_C_NEXT_I(s)
#define OBFH_C_IMM(x, k, f) ((f) == 0 ? ((unsigned)(x) ^ (unsigned)(k)) : (f) == 1 ? ((unsigned)(x) + (unsigned)(k)) \
                                                                                   : ((unsigned)(x) - (unsigned)(k)))
#define OBFH_C_ROT(x, r, f) ((f) == 0 ? (((unsigned)(x) << (r)) | ((unsigned)(x) >> (32u - (r)))) : (f) == 1 ? (((unsigned)(x) >> (r)) | ((unsigned)(x) << (32u - (r)))) \
                                                                                                : (f) == 2   ? ~(unsigned)(x) \
                                                                                                             : (0u - (unsigned)(x)))
#define OBFH_C_VALUE(s, p, n, previous) \
    enum { __c_kind##s##p##n = (__c_order##s##p >> (2 * ((n)-1))) & 3u, \
           __c_rotated##s##p##n = OBFH_C_ROT(__c_value##s##p##previous, __obfh_r##s##p, __c_rotation##s##p), \
           __c_rform##s##p##n = __c_rotated##s##p##n == __c_value##s##p##previous ? 2 : __c_rotation##s##p, \
           __c_scaled##s##p##n = (unsigned)__c_value##s##p##previous * (1u + (2u << __c_scale##s##p)), \
           __c_lform##s##p##n = __c_scaled##s##p##n == __c_value##s##p##previous, \
           __c_key##s##p##n = (__c_kind##s##p##n == 0 ? __obfh_k##s##p : __obfh_a##s##p) | 1u, \
           __c_form##s##p##n = __c_kind##s##p##n == 0 ? __c_forma##s##p : __c_formb##s##p, \
           __c_value##s##p##n = __c_kind##s##p##n == 1   ? (__c_rotated##s##p##n == __c_value##s##p##previous ? ~(unsigned)__c_value##s##p##previous : (unsigned)__c_rotated##s##p##n) \
                                : __c_kind##s##p##n == 2 ? (__c_lform##s##p##n ? ~(unsigned)__c_value##s##p##previous : (unsigned)__c_scaled##s##p##n) \
                                                         : OBFH_C_IMM(__c_value##s##p##previous, __c_key##s##p##n, __c_form##s##p##n) };
#define OBFH_C_FIELDS(s, p) \
    enum { __c_rank##s##p = (__obfh_k##s##p >> 8) % 24u, \
           __c_commit##s##p = (__obfh_k##s##p >> 28) % 3u, \
           __c_first##s##p = __c_rank##s##p / 6u, \
           __c_second##s##p = ((__c_rank##s##p / 2u) % 3u) + (((__c_rank##s##p / 2u) % 3u) >= __c_first##s##p), \
           __c_z##s##p = (__c_rank##s##p & 1u) + ((__c_rank##s##p & 1u) >= (__c_first##s##p < __c_second##s##p ? __c_first##s##p : __c_second##s##p)), \
           __c_third##s##p = __c_z##s##p + (__c_z##s##p >= (__c_first##s##p > __c_second##s##p ? __c_first##s##p : __c_second##s##p)), \
           __c_order##s##p = __c_first##s##p | (__c_second##s##p << 2) | (__c_third##s##p << 4) | ((6u - __c_first##s##p - __c_second##s##p - __c_third##s##p) << 6), \
           __c_forma##s##p = (__obfh_k##s##p >> 24) % 3u, \
           __c_formb##s##p = (__obfh_k##s##p >> 21) % 3u, \
           __c_rotation##s##p = (__obfh_k##s##p >> 16) & 3u, \
           __c_scale##s##p = (__obfh_k##s##p >> 20) % 3u, \
           __c_value##s##p##0 = OBFH_C_POINT(s) }; \
    OBFH_C_VALUE(s, p, 1, 0) \
    OBFH_C_VALUE(s, p, 2, 1) \
    OBFH_C_VALUE(s, p, 3, 2) \
    OBFH_C_VALUE(s, p, 4, 3) \
    OBFH_C_ENCODING(s, p, 1) \
    OBFH_C_ENCODING(s, p, 2) \
    OBFH_C_ENCODING(s, p, 3) \
    OBFH_C_ENCODING(s, p, 4)
#define OBFH_C_BAD(s, v) ((v) == __c_value##s##0##4 || (v) == __c_value##s##1##4 || (v) == OBFH_C_POINT(s))
#define OBFH_C_TARGET(s) \
    enum { __c_base##s = __obfh_k##s##0 ^ __obfh_k##s##1, \
           __c_avoid##s = OBFH_C_BAD(s, (unsigned int)__c_base##s), \
           __c_avoidb##s = OBFH_C_BAD(s, (unsigned int)__c_base##s + __c_avoid##s), \
           __c_avoidc##s = OBFH_C_BAD(s, (unsigned int)__c_base##s + __c_avoid##s + __c_avoidb##s), \
           OBFH_C_POINT(OBFH_C_NEXT(s)) = (unsigned int)__c_base##s + __c_avoid##s + __c_avoidb##s + __c_avoidc##s };
#define OBFH_C_PREPARE_0(s) OBFH_C_FIELDS(s, 0)
#define OBFH_C_PREPARE_1(s) OBFH_C_FIELDS(s, 1) OBFH_C_TARGET(s)
#define OBFH_C_PREPARE_I(s, p) OBFH_C_PREPARE_##p(s)
#define OBFH_P_PREPARE(s, p) OBFH_C_PREPARE_I(s, p)
#define OBFH_C_SLOT(n) ".fill 1,(%c[code" #n "]>>24)&3,%c[code" #n "]&0xffffff; .fill (%c[code" #n "]>>26)&1,4,(((%c[key" #n "])&0xffffffff)^0x80000000)-0x80000000;"
#define OBFH_C_TEXT OBFH_C_SLOT(1) OBFH_C_SLOT(2) OBFH_C_SLOT(3) OBFH_C_SLOT(4) ".byte %c[correction_op]; .long %c[correction];"
// Bits 0..23 are the original instruction; 24..25 hold its length, bit 26 its immediate flag.
#define OBFH_C_ENCODING(s, p, n) \
    enum { __obfh_primitive_code##s##p##n = __c_kind##s##p##n == 0 || __c_kind##s##p##n == 3 \
                                                ? (0x05000000u | (__c_form##s##p##n == 0 ? 0x35u : __c_form##s##p##n == 1 ? 0x05u \
                                                                                                                          : 0x2du)) \
                                            : __c_kind##s##p##n == 1 \
                                                ? (__c_rform##s##p##n < 2    ? 0x0300c0c1u + (__c_rform##s##p##n << 11) + (__obfh_r##s##p << 16) \
                                                   : __c_rform##s##p##n == 2 ? 0x0200d0f7u \
                                                                             : 0x0200d8f7u) \
                                                : (__c_lform##s##p##n ? 0x0200d0f7u : 0x0340048du + (__c_scale##s##p << 22)) };

#define OBFH_C_ARG(s, p, n) \
    [key##n] "i"(__c_key##s##p##n), [code##n] "i"(__obfh_primitive_code##s##p##n)
#define OBFH_C_ARGS(s, p) OBFH_C_ARG(s, p, 1), OBFH_C_ARG(s, p, 2), OBFH_C_ARG(s, p, 3), OBFH_C_ARG(s, p, 4), \
                          [correction_op] "i"(__c_commit##s##p == 0 ? 0x05 : __c_commit##s##p == 1 ? 0x2d \
                                                                                                   : 0x35), \
                          [correction] "i"(__c_commit##s##p == 0   ? (unsigned)OBFH_C_POINT(OBFH_C_NEXT(s)) - (unsigned)__c_value##s##p##4 \
                                           : __c_commit##s##p == 1 ? (unsigned)__c_value##s##p##4 - (unsigned)OBFH_C_POINT(OBFH_C_NEXT(s)) \
                                                                   : (unsigned)__c_value##s##p##4 ^ (unsigned)OBFH_C_POINT(OBFH_C_NEXT(s)))
#define OBFH_P_STEP(s, p) \
    ({ __obfh_asm__(OBFH_C_TEXT \
                    : "+a"(__obfh_flow_state) \
                    : OBFH_C_ARGS(s, p) \
                    : "cc"); })
#define OBFH_P_TERMINAL(s, p, style) \
    ({ __obfh_asm__(OBFH_C_TEXT "movl $%c[expected], %%edx;" OBFH_P_TERMINAL_TEXT \
                    : "=&a"(__obfh_flow_result) \
                    : "0"(__obfh_flow_state), OBFH_C_ARGS(s, p), [expected] "i"(OBFH_C_POINT(OBFH_C_NEXT(s))), \
                      [finish] "i"((style)&7u), [zero_form] "i"((__obfh_flow_hash >> 11) % 3u) \
                    : "edx", "ecx", "cc"); })

// Capture each integer/pointer input once. The unsigned word view shares storage
// on little-endian x86/x64, preserving high-only values without wide shifts.
// Scalar assignment deliberately avoids aggregate initialization and its zeroing.
#define OBFH_FLOW_VALUE_INPUT(condition) \
    ({ \
        union { \
            __typeof__(__builtin_choose_expr(sizeof(__obfh_condition_type) <= 4, 0u, 0ull)) value; \
            unsigned int words[sizeof(__obfh_condition_type) <= 4 ? 1 : 2]; \
        } __obfh_input; \
        __obfh_input.value = (__typeof__(__obfh_input.value))(condition); \
        __builtin_choose_expr(sizeof(__obfh_condition_type) <= 4, (void)0, (void)(__obfh_input.words[0] |= __obfh_input.words[1])); \
        __obfh_asm__(".byte %c[op]; .long %c[key];" \
                     : "+a"(__obfh_input.words[0]) \
                     : [op] "i"(__obfh_flow_hash % 3u == 0 ? 0x35 : __obfh_flow_hash % 3u == 1 ? 0x05 \
                                                                                               : 0x2d), \
                       [key] "i"(__obfh_flow_hash % 3u == 2 ? 0u - __obfh_point0 : __obfh_point0) \
                     : "cc"); \
        __obfh_input.words[0]; \
    })

// Keep integer/pointer values in the transport instead of replacing them with
// two tags. OR-fold wide values without losing high-only nonzero inputs. Floating
// conditions retain C's original comparison path (including NaN and signed zero).
#define OBFH_FLOW_CONDITION(condition, site_value, ...) \
    ({ \
        typedef __typeof__((condition) + 0) __obfh_condition_type; \
        enum { __obfh_value_input = sizeof(__obfh_condition_type) <= 8 && \
                                    !__builtin_types_compatible_p(__obfh_condition_type, float) && \
                                    !__builtin_types_compatible_p(__obfh_condition_type, double) && \
                                    !__builtin_types_compatible_p(__obfh_condition_type, long double) }; \
        enum { \
            __obfh_flow_site = (site_value), \
            __obfh_flow_seed = (unsigned int)__obfh_flow_site ^ (unsigned int)OBFH_SEED ^ 0x43464C57u, \
            __obfh_flow_mixed = OBFH_MIX_A(__obfh_flow_seed), \
            __obfh_flow_hash = OBFH_MIX_B(__obfh_flow_mixed), \
            __obfh_flow_layout = __obfh_flow_hash % (OBFH_BETTER_CONTROL_FLOW ? 8u : 4u), \
            __obfh_flow_key = OBFH_JUNK_WORD & 0x7fffffffu, \
            __obfh_flow_exit = (__obfh_flow_hash >> 16) & 7u, \
            __obfh_false_tag = OBFH_MIX_A(__obfh_flow_hash ^ 0x15729f81u), \
            __obfh_true_tag = __obfh_false_tag ^ (OBFH_MIX_B(__obfh_flow_hash ^ 0x74ba953du) | 1u), \
            __obfh_flow_first = OBFH_BETTER_CONTROL_FLOW ? (__obfh_flow_layout == 0 || __obfh_flow_layout == 2   ? 0 \
                                                            : __obfh_flow_layout == 1 || __obfh_flow_layout == 4 ? 1 \
                                                            : __obfh_flow_layout == 5 || __obfh_flow_layout == 6 ? 2 \
                                                                                                                 : 3) \
                                                         : __obfh_flow_layout, \
            __obfh_flow_second = __obfh_flow_layout == 0 || __obfh_flow_layout == 5   ? 1 \
                                 : __obfh_flow_layout == 1 || __obfh_flow_layout == 3 ? 0 \
                                 : __obfh_flow_layout == 2 || __obfh_flow_layout == 6 ? 3 \
                                                                                      : 2, \
            __obfh_k00 = OBFH_MIX_A(__obfh_flow_hash ^ 2654435769u), \
            __obfh_a00 = OBFH_MIX_B(__obfh_k00 ^ 0x85ebca6bu), \
            __obfh_r00 = ((__obfh_k00 >> 27) % 31u) + 1u, \
            __obfh_k01 = OBFH_MIX_A(__obfh_flow_hash ^ 3532493122u), \
            __obfh_a01 = OBFH_MIX_B(__obfh_k01 ^ 0x85ebca6bu), \
            __obfh_r01 = ((__obfh_k01 >> 27) % 31u) + 1u, \
            __obfh_k10 = OBFH_MIX_A(__obfh_flow_hash ^ 2671344829u), \
            __obfh_a10 = OBFH_MIX_B(__obfh_k10 ^ 0x85ebca6bu), \
            __obfh_r10 = ((__obfh_k10 >> 27) % 31u) + 1u, \
            __obfh_k11 = OBFH_MIX_A(__obfh_flow_hash ^ 3549402182u), \
            __obfh_a11 = OBFH_MIX_B(__obfh_k11 ^ 0x85ebca6bu), \
            __obfh_r11 = ((__obfh_k11 >> 27) % 31u) + 1u, \
        }; \
        OBFH_P_SECOND_CONSTANTS \
        OBFH_P_SELECTOR_META(0); \
        OBFH_P_SELECTOR_META(1); \
        enum { __obfh_point0 = __obfh_true_tag }; \
        OBFH_P_PREPARE(0, 0); \
        OBFH_P_PREPARE(0, 1); \
        OBFH_P_PREPARE(1, 0); \
        OBFH_P_PREPARE(1, 1); \
        OBFH_P_SECOND_PREPARE \
        /* Zero is the reference point for value inputs; floating inputs retain their original polarity. */ \
        /* Preserve the scalar value once; both compile-time input paths reuse it. */ \
        __obfh_condition_type __obfh_condition_value = (condition); \
        unsigned int __obfh_flow_state = __builtin_choose_expr(__obfh_value_input, \
                                                               OBFH_FLOW_VALUE_INPUT(__obfh_condition_value), \
                                                               (__obfh_condition_value) ? (__obfh_flow_hash & 1u ? __obfh_true_tag : __obfh_false_tag) \
                                                                                        : (__obfh_flow_hash & 1u ? __obfh_false_tag : __obfh_true_tag)); \
        __builtin_choose_expr(__obfh_value_input || (__obfh_flow_hash & 1u) ^ (__obfh_flow_exit == 3u || __obfh_flow_exit == 5u || __obfh_flow_exit == 7u), (void)0, \
                              (void)(__obfh_flow_state ^= __obfh_false_tag ^ __obfh_true_tag)); \
        unsigned int __obfh_flow_tag = __obfh_true_tag, __obfh_flow_result; \
        ULONG_PTR __obfh_cookie = ((ULONG_PTR)&__obfh_flow_state >> 4) ^ (ULONG_PTR)__obfh_flow_hash; \
        __obfh_asm__( \
            "jmp 7f; .byte %c[byte], %c[extra]; .fill %c[gap],1,(((%c[byte])&0xffffffff)^0x80000000)-0x80000000; 7:" \
            : "+a"( \
                __obfh_flow_state) \
            : [byte] "i"(OBFH_FLOW_OPCODE(OBFH_JUNK_BYTE & 7u)), \
              [extra] "i"(OBFH_JUNK_BYTE), [payload] "i"(OBFH_JUNK_WORD), [gap] "i"(OBFH_JUNK_BYTE & 7u) \
            : "memory"); \
        __VA_ARGS__; \
        __obfh_flow_state ^= __obfh_flow_tag ^ __obfh_point0; \
        OBFH_P_FIRST_GRAPH; \
        OBFH_P_SECOND_GRAPH; \
        __builtin_choose_expr(__obfh_value_input && !(__obfh_flow_exit == 3u || __obfh_flow_exit == 5u || __obfh_flow_exit == 7u), \
                              __obfh_flow_result ^ 1u, __obfh_flow_result); \
    })

#endif /* CFLOW helpers; kernel precedes keyword interception. */
typedef enum { SALT_NUM1 = RND(16, 48),
               SALT_NUM2 = RND(16, 48) } VM_SALT;
// #endregion

// #region 11. VM kernel: registers, flags, decoding and instruction execution
// ============================================================================
// 11. VM kernel: registers, flags, decoding and instruction execution
// ============================================================================
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
    OBFH_V_BAND,
    OBFH_V_BOR,
    OBFH_V_BXOR,
    OBFH_V_BNOT,
    OBFH_V_BSHL,
    OBFH_V_BSHR,
    OBFH_V_LOAD_PAIR,
    OBFH_V_COPY_RETURN,
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
    OBFH_VOP_TRUTH,
    OBFH_VOP_BAND,
    OBFH_VOP_BOR,
    OBFH_VOP_BXOR,
    OBFH_VOP_BNOT,
    OBFH_VOP_BSHL,
    OBFH_VOP_BSHR
};
typedef struct
{
    unsigned char bytes[sizeof(long double)];
    unsigned int mask;
} OBFH_V_REGISTER;
typedef struct
{
#if (OBFH_SEED >> 4) % 4u == 0u
    OBFH_V_REGISTER registers[4];
    unsigned int controls[4];
    unsigned int flags, flag_key, pc, steps, initialized, key, status, flags_ready;
    long double result;
#endif
#if (OBFH_SEED >> 4) % 4u == 1u
    long double result;
    unsigned int key, initialized, steps, pc, flags_ready, status, flag_key, flags;
    unsigned int controls[4];
    OBFH_V_REGISTER registers[4];
#endif
#if (OBFH_SEED >> 4) % 4u == 2u
    unsigned int controls[4];
    unsigned int pc, key, status, flags, steps, flag_key, flags_ready, initialized;
    long double result;
    OBFH_V_REGISTER registers[4];
#endif
#if !(((OBFH_SEED >> 4) % 4u == 0u) || ((OBFH_SEED >> 4) % 4u == 1u) || ((OBFH_SEED >> 4) % 4u == 2u))
    unsigned int status, flags_ready, key, initialized, flag_key, steps, flags, pc;
    OBFH_V_REGISTER registers[4];
    long double result;
    unsigned int controls[4];
#endif
} OBFH_V_CONTEXT;

// Six byte permutations with different dependencies, selected at compile time.
// Every decoder costs one multiply and one add/subtract or XOR; no runtime selector.
#define OBFH_V_OPCODE_MODE ((OBFH_SEED >> 24) % 6u)
#define OBFH_V_OPCODE_MULTIPLIER(key) ((((unsigned int)(key) >> 8) & 255u) | 1u)
#if OBFH_V_OPCODE_MODE == 0u
#define OBFH_V_OPCODE_ENCODE(op, key) (((unsigned int)(op)*OBFH_V_OPCODE_MULTIPLIER(key) + ((key)&255u)) & 255u)
#define OBFH_V_OPCODE_DECODE(op, key, inverse) ((((op) - ((key)&255u)) * (inverse)) & 255u)
#endif
#if OBFH_V_OPCODE_MODE == 1u
#define OBFH_V_OPCODE_ENCODE(op, key) (((unsigned int)(op)*OBFH_V_OPCODE_MULTIPLIER(key) ^ ((key)&255u)) & 255u)
#define OBFH_V_OPCODE_DECODE(op, key, inverse) ((((op) ^ ((key)&255u)) * (inverse)) & 255u)
#endif
#if OBFH_V_OPCODE_MODE == 2u
#define OBFH_V_OPCODE_ENCODE(op, key) ((((unsigned int)(op) ^ ((key)&255u)) * OBFH_V_OPCODE_MULTIPLIER(key)) & 255u)
#define OBFH_V_OPCODE_DECODE(op, key, inverse) ((((op) * (inverse)) ^ ((key)&255u)) & 255u)
#endif
#if OBFH_V_OPCODE_MODE == 3u
#define OBFH_V_OPCODE_ENCODE(op, key) ((((unsigned int)(op) + ((key)&255u)) * OBFH_V_OPCODE_MULTIPLIER(key)) & 255u)
#define OBFH_V_OPCODE_DECODE(op, key, inverse) ((((op) * (inverse)) - ((key)&255u)) & 255u)
#endif
#if OBFH_V_OPCODE_MODE == 4u
#define OBFH_V_OPCODE_ENCODE(op, key) (((unsigned int)(op)*OBFH_V_OPCODE_MULTIPLIER(key) - ((key)&255u)) & 255u)
#define OBFH_V_OPCODE_DECODE(op, key, inverse) ((((op) + ((key)&255u)) * (inverse)) & 255u)
#endif
#if !((OBFH_V_OPCODE_MODE == 0u) || (OBFH_V_OPCODE_MODE == 1u) || (OBFH_V_OPCODE_MODE == 2u) || (OBFH_V_OPCODE_MODE == 3u) || (OBFH_V_OPCODE_MODE == 4u))
#define OBFH_V_OPCODE_ENCODE(op, key) ((((unsigned int)(op) - ((key)&255u)) * OBFH_V_OPCODE_MULTIPLIER(key)) & 255u)
#define OBFH_V_OPCODE_DECODE(op, key, inverse) ((((op) * (inverse)) + ((key)&255u)) & 255u)
#endif

// Word masking changes independently of opcode encoding and context placement.
#define OBFH_V_WORD_MODE (((OBFH_SEED >> 6) & 15u) % 9u)
#if OBFH_V_WORD_MODE % 3u == 0u
#define OBFH_V_WORD_MASK(key, pc) ((key) + (unsigned int)(pc)*0x9e3779b9u)
#endif
#if OBFH_V_WORD_MODE % 3u == 1u
#define OBFH_V_WORD_MASK(key, pc) ((key) ^ (unsigned int)(pc)*0x9e3779b9u)
#endif
#if !((OBFH_V_WORD_MODE % 3u == 0u) || (OBFH_V_WORD_MODE % 3u == 1u))
#define OBFH_V_WORD_MASK(key, pc) ((key) - (unsigned int)(pc)*0x9e3779b9u)
#endif
#if OBFH_V_WORD_MODE / 3u == 0u
#define OBFH_V_WORD_ENCODE(word, key, pc) ((word) ^ OBFH_V_WORD_MASK(key, pc))
#define OBFH_V_WORD_DECODE(word, key, pc) OBFH_V_WORD_ENCODE(word, key, pc)
#endif
#if OBFH_V_WORD_MODE / 3u == 1u
#define OBFH_V_WORD_ENCODE(word, key, pc) ((word) + OBFH_V_WORD_MASK(key, pc))
#define OBFH_V_WORD_DECODE(word, key, pc) ((word)-OBFH_V_WORD_MASK(key, pc))
#endif
#if !((OBFH_V_WORD_MODE / 3u == 0u) || (OBFH_V_WORD_MODE / 3u == 1u))
#define OBFH_V_WORD_ENCODE(word, key, pc) ((word)-OBFH_V_WORD_MASK(key, pc))
#define OBFH_V_WORD_DECODE(word, key, pc) ((word) + OBFH_V_WORD_MASK(key, pc))
#endif

// Capture seed mixing once. Xorshifts and odd multiplication stay bijective.
// Direction and distance change without adding runtime operations.
enum { OBFH_MIX_FIELDS(obfh_v_helper_draw, OBFH_SEED ^ 0x6d2b79f5u) };
#define OBFH_V_MIX_SHIFT_A (11u + (obfh_v_helper_draw & 7u))
#define OBFH_V_MIX_SHIFT_B (12u + ((obfh_v_helper_draw >> 3) & 7u))
#define OBFH_V_MIX_MULTIPLIER (obfh_v_helper_draw % 3u == 0u ? 2246822507u : obfh_v_helper_draw % 3u == 1u ? 3266489909u \
                                                                                                           : 2221713035u)
static inline unsigned int obfh_v_mix(unsigned int x) {
    x ^= __builtin_choose_expr(obfh_v_helper_draw & 0x40u, x << OBFH_V_MIX_SHIFT_A, x >> OBFH_V_MIX_SHIFT_A);
    x *= OBFH_V_MIX_MULTIPLIER;
    PHANTOM_NOP;
    x ^= __builtin_choose_expr((obfh_v_helper_draw & 0xc0u) == 0x80u, x << OBFH_V_MIX_SHIFT_B, x >> OBFH_V_MIX_SHIFT_B);
    return x;
}
/* Encodings are local to an execution; only decoded uint32 values enter ALU operations. */
#define OBFH_V_CONTROL_MASK(context, reg) \
    obfh_v_mix(__builtin_choose_expr(obfh_v_helper_draw & 0x200u, (context)->key ^ ((reg)*3266489917u), (context)->key + ((reg)*3266489917u)))
static inline unsigned int obfh_v_control_read(const OBFH_V_CONTEXT *c, unsigned int r) {
    PHANTOM_NOP;
    return c->controls[r] ^ OBFH_V_CONTROL_MASK(c, r);
}
static inline void obfh_v_control_write(OBFH_V_CONTEXT *c, unsigned int r, unsigned int value) {
    c->controls[r] = value ^ OBFH_V_CONTROL_MASK(c, r);
    PHANTOM_NOP;
}
// Controls retain XOR: CXOR updates their encoded bytes directly.
// Flags and numeric register bytes can use either modular arithmetic or XOR.
#if (OBFH_SEED >> 8) % 3u == 0u
#define OBFH_V_VALUE_ENCODE(value, mask) ((value) ^ (mask))
#define OBFH_V_VALUE_DECODE(value, mask) ((value) ^ (mask))
#endif
#if (OBFH_SEED >> 8) % 3u == 1u
#define OBFH_V_VALUE_ENCODE(value, mask) ((value) + (mask))
#define OBFH_V_VALUE_DECODE(value, mask) ((value) - (mask))
#endif
#if !(((OBFH_SEED >> 8) % 3u == 0u) || ((OBFH_SEED >> 8) % 3u == 1u))
#define OBFH_V_VALUE_ENCODE(value, mask) ((value) - (mask))
#define OBFH_V_VALUE_DECODE(value, mask) ((value) + (mask))
#endif
static inline unsigned int obfh_v_flags(const OBFH_V_CONTEXT *c) {
    PHANTOM_NOP;
    return __builtin_choose_expr(obfh_v_helper_draw % 3u == 0u, c->flags ^ c->flag_key,
                                 __builtin_choose_expr(obfh_v_helper_draw % 3u == 1u, c->flags - c->flag_key, c->flags + c->flag_key));
}
static inline void obfh_v_set_flags(OBFH_V_CONTEXT *c, unsigned int flags) {
    c->flags = __builtin_choose_expr(obfh_v_helper_draw % 3u == 0u, flags ^ c->flag_key,
                                     __builtin_choose_expr(obfh_v_helper_draw % 3u == 1u, flags + c->flag_key, flags - c->flag_key));
    PHANTOM_NOP;
}
// Reverse traversal changes the mask chain without extra per-byte indexing.
#if (OBFH_SEED >> 27) & 1u
#define OBFH_V_BYTES(size) for (unsigned int i = (size); i-- > 0;)
#else
#define OBFH_V_BYTES(size) for (unsigned int i = 0; i < (size); ++i)
#endif
static inline void obfh_v_write(OBFH_V_CONTEXT *c, unsigned int r, long double v) {
    unsigned int m = obfh_v_mix(c->key ^ (r * 3266489917u) ^ c->steps);
    const unsigned char *p = (const unsigned char *)&v;
    c->registers[r].mask = m;
    PHANTOM_NOP;
    OBFH_V_BYTES(sizeof(v)) {
        m = obfh_v_mix(m + i + 1u);
        c->registers[r].bytes[i] = OBFH_V_VALUE_ENCODE(p[i], (unsigned char)m);
    }
    c->initialized |= 1u << r;
}
static inline long double obfh_v_read(const OBFH_V_CONTEXT *c, unsigned int r) {
    long double v;
    unsigned char *p = (unsigned char *)&v;
    unsigned int m = c->registers[r].mask;
    PHANTOM_NOP;
    OBFH_V_BYTES(sizeof(v)) {
        m = obfh_v_mix(m + i + 1u);
        p[i] = OBFH_V_VALUE_DECODE(c->registers[r].bytes[i], (unsigned char)m);
    }
    return v;
}
static inline void obfh_v_copy(OBFH_V_CONTEXT *c, unsigned int d, unsigned int a) {
    /* Encoded copy preserves numeric object bytes without an FP operation. */
    c->registers[d] = c->registers[a];
    PHANTOM_NOP;
    c->initialized |= 1u << d;
}
static inline void obfh_v_failure(OBFH_V_CONTEXT *c, unsigned int status) {
    c->status = status;
    PHANTOM_NOP;
    ExitProcess(0xE0BF0000u | status);
}
static inline unsigned int obfh_v_decode(unsigned int word, unsigned int key, unsigned int pc, unsigned int rotate) {
    word = OBFH_V_WORD_DECODE(word, key, pc);
    PHANTOM_NOP;
    return (word << rotate) | (word >> (32u - rotate));
}
static inline unsigned int obfh_v_opcode(unsigned int word, unsigned int key) {
    unsigned int multiplier = ((key >> 8) & 255u) | 1u, inverse = 1u;
    inverse *= 2u - multiplier * inverse;
    PHANTOM_NOP;
    inverse *= 2u - multiplier * inverse;
    inverse *= 2u - multiplier * inverse;
    return OBFH_V_OPCODE_DECODE(word & 255u, key, inverse);
}

// Decoder order is selected at preprocessing time; only the selected decoder executes its optional NOP.
// Permute field ownership, independently of register allocation and decoder order.
#if ((OBFH_SEED >> 14) % 6u) == 0u
#define OBFH_VM_FIELD_D 8u
#define OBFH_VM_FIELD_A 10u
#define OBFH_VM_FIELD_B 12u
#endif
#if ((OBFH_SEED >> 14) % 6u) == 1u
#define OBFH_VM_FIELD_D 8u
#define OBFH_VM_FIELD_A 12u
#define OBFH_VM_FIELD_B 10u
#endif
#if ((OBFH_SEED >> 14) % 6u) == 2u
#define OBFH_VM_FIELD_D 10u
#define OBFH_VM_FIELD_A 12u
#define OBFH_VM_FIELD_B 8u
#endif
#if ((OBFH_SEED >> 14) % 6u) == 3u
#define OBFH_VM_FIELD_D 10u
#define OBFH_VM_FIELD_A 8u
#define OBFH_VM_FIELD_B 12u
#endif
#if ((OBFH_SEED >> 14) % 6u) == 4u
#define OBFH_VM_FIELD_D 12u
#define OBFH_VM_FIELD_A 8u
#define OBFH_VM_FIELD_B 10u
#endif
#if ((OBFH_SEED >> 14) % 6u) == 5u
#define OBFH_VM_FIELD_D 12u
#define OBFH_VM_FIELD_A 10u
#define OBFH_VM_FIELD_B 8u
#endif
#define OBFH_VM_FIELDS_0(finish) \
    fields_0: \
    OBFH_PHANTOM_NOP_AT(__LINE__ + 0u); \
    op = word; \
    d = (word >> OBFH_VM_FIELD_D) & 3u; \
    a = (word >> OBFH_VM_FIELD_A) & 3u; \
    b = (word >> OBFH_VM_FIELD_B) & 3u; \
    imm = word >> 14; \
    finish
#define OBFH_VM_FIELDS_1(finish) \
    fields_1: \
    OBFH_PHANTOM_NOP_AT(__LINE__ + 1u); \
    op = word >> 24; \
    d = (word >> (OBFH_VM_FIELD_D + 10u)) & 3u; \
    a = (word >> (OBFH_VM_FIELD_A + 10u)) & 3u; \
    b = (word >> (OBFH_VM_FIELD_B + 10u)) & 3u; \
    imm = word & 0x3ffffu; \
    finish
#define OBFH_VM_FIELDS_2(finish) \
    fields_2: \
    OBFH_PHANTOM_NOP_AT(__LINE__ + 2u); \
    op = word >> 6; \
    d = (word >> (OBFH_VM_FIELD_D - 8u)) & 3u; \
    a = (word >> (OBFH_VM_FIELD_A - 8u)) & 3u; \
    b = (word >> (OBFH_VM_FIELD_B - 8u)) & 3u; \
    imm = word >> 14; \
    finish
#define OBFH_VM_FIELDS_3(finish) \
    fields_3: \
    OBFH_PHANTOM_NOP_AT(__LINE__ + 3u); \
    op = word; \
    d = (word >> (OBFH_VM_FIELD_D == 8u ? 12u : OBFH_VM_FIELD_D - 2u)) & 3u; \
    a = (word >> (OBFH_VM_FIELD_A == 8u ? 12u : OBFH_VM_FIELD_A - 2u)) & 3u; \
    b = (word >> (OBFH_VM_FIELD_B == 8u ? 12u : OBFH_VM_FIELD_B - 2u)) & 3u; \
    imm = word >> 14; \
    finish
#define OBFH_VM_FIELDS_ORDER(a, b, c, d) a(goto fields_ready;) b(goto fields_ready;) c(goto fields_ready;) d()
#define OBFH_VM_FIELDS_EXPAND(...) OBFH_VM_FIELDS_ORDER(__VA_ARGS__)
#define OBFH_VM_FIELDS_DRAW (((OBFH_SEED ^ 0x564d4644u) * 2654435761u) & 0xffffffffu)
#if (OBFH_VM_FIELDS_DRAW / 4u) % 6u == 0u
#define OBFH_VM_FIELDS_TAIL(a, b, c) a, b, c
#endif
#if (OBFH_VM_FIELDS_DRAW / 4u) % 6u == 1u
#define OBFH_VM_FIELDS_TAIL(a, b, c) a, c, b
#endif
#if (OBFH_VM_FIELDS_DRAW / 4u) % 6u == 2u
#define OBFH_VM_FIELDS_TAIL(a, b, c) b, a, c
#endif
#if (OBFH_VM_FIELDS_DRAW / 4u) % 6u == 3u
#define OBFH_VM_FIELDS_TAIL(a, b, c) b, c, a
#endif
#if (OBFH_VM_FIELDS_DRAW / 4u) % 6u == 4u
#define OBFH_VM_FIELDS_TAIL(a, b, c) c, a, b
#endif
#if (OBFH_VM_FIELDS_DRAW / 4u) % 6u == 5u
#define OBFH_VM_FIELDS_TAIL(a, b, c) c, b, a
#endif
#if OBFH_VM_FIELDS_DRAW % 4u == 0u
#define OBFH_VM_FIELDS_LAYOUT OBFH_VM_FIELDS_EXPAND(OBFH_VM_FIELDS_0, OBFH_VM_FIELDS_TAIL(OBFH_VM_FIELDS_1, OBFH_VM_FIELDS_2, OBFH_VM_FIELDS_3))
#endif
#if OBFH_VM_FIELDS_DRAW % 4u == 1u
#define OBFH_VM_FIELDS_LAYOUT OBFH_VM_FIELDS_EXPAND(OBFH_VM_FIELDS_1, OBFH_VM_FIELDS_TAIL(OBFH_VM_FIELDS_2, OBFH_VM_FIELDS_3, OBFH_VM_FIELDS_0))
#endif
#if OBFH_VM_FIELDS_DRAW % 4u == 2u
#define OBFH_VM_FIELDS_LAYOUT OBFH_VM_FIELDS_EXPAND(OBFH_VM_FIELDS_2, OBFH_VM_FIELDS_TAIL(OBFH_VM_FIELDS_3, OBFH_VM_FIELDS_0, OBFH_VM_FIELDS_1))
#endif
#if OBFH_VM_FIELDS_DRAW % 4u == 3u
#define OBFH_VM_FIELDS_LAYOUT OBFH_VM_FIELDS_EXPAND(OBFH_VM_FIELDS_3, OBFH_VM_FIELDS_TAIL(OBFH_VM_FIELDS_0, OBFH_VM_FIELDS_1, OBFH_VM_FIELDS_2))
#endif

static inline long double Obfh_VirtualMachine(const unsigned int *program, unsigned int length, unsigned int key, OBFH_VM_VALUE input_a,
                                              OBFH_VM_VALUE input_b) OBFH_CODE_SECTION_ATTRIBUTE {
#if OBFH_DISABLE_CONTROL_FLOW != 1
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
    BREAK_STACK_CFLOW;
    // Key-dependent inverses are invariant for this execution.
    unsigned int decode_rotate = (key % 31u) + 1u;
    unsigned int decode_multiplier = ((key >> 8) & 255u) | 1u, decode_inverse = 1u;
    decode_inverse *= 2u - decode_multiplier * decode_inverse;
    decode_inverse *= 2u - decode_multiplier * decode_inverse;
    decode_inverse *= 2u - decode_multiplier * decode_inverse;
    // Choose a local decoder once; each executed decoder uses immediate shifts.
    void *const field_decoders[4] = {&&fields_0, &&fields_1, &&fields_2, &&fields_3};
    void *field_decoder = field_decoders[(key >> 29) & 3u];
    /* OBFH_VM_TRACE_ENTER */
    while (c.steps < 128) {
        if (c.pc >= length)
            obfh_v_failure(&c, 2);
        unsigned int at = c.pc++, word = obfh_v_decode(program[at], key, at, decode_rotate);
        unsigned int op, d, a, b, imm;
        goto *field_decoder;
        OBFH_VM_FIELDS_LAYOUT
    fields_ready:
        PHANTOM_NOP;
        op = OBFH_V_OPCODE_DECODE(op & 255u, key, decode_inverse);
        ++c.steps;
        if (op < OBFH_V_LOAD_A || op > OBFH_V_RETURN)
            obfh_v_failure(&c, 3);
        if (op == OBFH_V_MOVE || op == OBFH_V_SWAP || (op >= OBFH_V_ADD && op <= OBFH_V_COMPARE) || op == OBFH_V_TEST ||
            op == OBFH_V_RETURN || op == OBFH_V_COPY_RETURN || (op >= OBFH_V_BAND && op <= OBFH_V_BSHR))
            if (!(c.initialized & (1u << a)))
                obfh_v_failure(&c, 4);
        if (op == OBFH_V_SWAP || (op >= OBFH_V_ADD && op <= OBFH_V_COMPARE) ||
            (op >= OBFH_V_BAND && op <= OBFH_V_BSHR && op != OBFH_V_BNOT))
            if (!(c.initialized & (1u << b)))
                obfh_v_failure(&c, 4);
        if ((op == OBFH_V_JUMP || op == OBFH_V_JFLAG) && imm >= length)
            obfh_v_failure(&c, 5);
            /* OBFH_VM_TRACE_STEP */
#if OBFH_DISABLE_CONTROL_FLOW != 1
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
            case OBFH_V_BAND:
                STACK_PROXY_FUNCTIONS;
                goto bit_and;
            case OBFH_V_BOR:
                STACK_PROXY_FUNCTIONS;
                goto bit_or;
            case OBFH_V_BXOR:
                STACK_PROXY_FUNCTIONS;
                goto bit_xor;
            case OBFH_V_BNOT:
                STACK_PROXY_FUNCTIONS;
                goto bit_not;
            case OBFH_V_BSHL:
                STACK_PROXY_FUNCTIONS;
                goto bit_shl;
            case OBFH_V_BSHR:
                STACK_PROXY_FUNCTIONS;
                goto bit_shr;
            case OBFH_V_LOAD_PAIR:
                STACK_PROXY_FUNCTIONS;
                goto load_pair;
            case OBFH_V_COPY_RETURN:
                STACK_PROXY_FUNCTIONS;
                goto copy_return;
            case OBFH_V_RETURN:
                STACK_PROXY_FUNCTIONS;
                goto finish;
        }
    invalid : {
        STACK_PROXY_FUNCTIONS;
        obfh_v_failure(&c, 3);
        continue;
    }
    load_pair : {
        STACK_PROXY_FUNCTIONS;
        if (d == a) obfh_v_failure(&c, 8);
        obfh_v_write(&c, d, operands[0]);
        obfh_v_write(&c, a, operands[1]);
        continue;
    }
    copy_return : {
        STACK_PROXY_FUNCTIONS;
        obfh_v_copy(&c, d, a);
        a = d;
        goto finish;
    }
    load_a : {
        STACK_PROXY_FUNCTIONS;
        obfh_v_write(&c, d, operands[0]);
        continue;
    }
    load_b : {
        STACK_PROXY_FUNCTIONS;
        obfh_v_write(&c, d, operands[1]);
        continue;
    }
    move : {
        STACK_PROXY_FUNCTIONS;
        obfh_v_copy(&c, d, a);
        continue;
    }
    swap : {
        STACK_PROXY_FUNCTIONS;
        OBFH_V_REGISTER temporary = c.registers[a];
        c.registers[a] = c.registers[b];
        c.registers[b] = temporary;
        continue;
    }
    add : {
        STACK_PROXY_FUNCTIONS;
        obfh_v_write(&c, d, obfh_v_read(&c, a) + obfh_v_read(&c, b));
        continue;
    }
    sub : {
        STACK_PROXY_FUNCTIONS;
        obfh_v_write(&c, d, obfh_v_read(&c, a) - obfh_v_read(&c, b));
        continue;
    }
    mul : {
        STACK_PROXY_FUNCTIONS;
        obfh_v_write(&c, d, obfh_v_read(&c, a) * obfh_v_read(&c, b));
        continue;
    }
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
    bit_and : {
        STACK_PROXY_FUNCTIONS;
        unsigned int x = (unsigned int)obfh_v_read(&c, a), y = (unsigned int)obfh_v_read(&c, b);
        obfh_v_write(&c, d, x & y);
        continue;
    }
    bit_or : {
        STACK_PROXY_FUNCTIONS;
        unsigned int x = (unsigned int)obfh_v_read(&c, a), y = (unsigned int)obfh_v_read(&c, b);
        obfh_v_write(&c, d, x | y);
        continue;
    }
    bit_xor : {
        STACK_PROXY_FUNCTIONS;
        unsigned int x = (unsigned int)obfh_v_read(&c, a), y = (unsigned int)obfh_v_read(&c, b);
        obfh_v_write(&c, d, x ^ y);
        continue;
    }
    bit_not : {
        STACK_PROXY_FUNCTIONS;
        unsigned int x = (unsigned int)obfh_v_read(&c, a);
        obfh_v_write(&c, d, ~x);
        continue;
    }
    bit_shl : {
        STACK_PROXY_FUNCTIONS;
        unsigned int x = (unsigned int)obfh_v_read(&c, a), y = (unsigned int)obfh_v_read(&c, b);
        obfh_v_write(&c, d, x << (y & 31u));
        continue;
    }
    bit_shr : {
        STACK_PROXY_FUNCTIONS;
        unsigned int x = (unsigned int)obfh_v_read(&c, a), y = (unsigned int)obfh_v_read(&c, b);
        obfh_v_write(&c, d, x >> (y & 31u));
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
#if OBFH_DISABLE_CONTROL_FLOW != 1
            case (-1 * RND(0xBAD, 0xBEEF)):
                OBFH_FLOW_DEAD_BYTES;
#endif
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
    test : {
        STACK_PROXY_FUNCTIONS;
        obfh_v_set_flags(&c, (obfh_v_flags(&c) & OBFH_V_CONTROL) | (obfh_v_read(&c, a) != 0 ? OBFH_V_TRUE : 0));
        c.flags_ready = 1;
        continue;
    }
    cadd : {
        STACK_PROXY_FUNCTIONS;
        unsigned int signed_imm = (imm & 0x20000u) ? imm | 0xfffc0000u : imm;
        unsigned int value = obfh_v_control_read(&c, d) + signed_imm;
        obfh_v_control_write(&c, d, value);
        obfh_v_set_flags(&c, (obfh_v_flags(&c) & ~OBFH_V_CONTROL) | (value ? OBFH_V_CONTROL : 0));
        c.flags_ready = 1;
        continue;
    }
    cxor : {
        STACK_PROXY_FUNCTIONS;
        c.controls[d] ^= imm;
        continue;
    }
    crol : {
        STACK_PROXY_FUNCTIONS;
        unsigned int n = (imm % 31u) + 1u, value = obfh_v_control_read(&c, d);
        obfh_v_control_write(&c, d, (value << n) | (value >> (32u - n)));
        continue;
    }
    jump : {
        STACK_PROXY_FUNCTIONS;
        c.pc = imm;
        continue;
    }
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
#if OBFH_DISABLE_CONTROL_FLOW != 1
        if (!OBFH_FLOW_CONDITION(c.initialized != 0, __obfh_vm_finish_site))
            obfh_v_failure(&c, 4);
#endif
        c.result = obfh_v_read(&c, a);
        return c.result;
    }
    obfh_v_failure(&c, 7);
    return 0;
}

// #endregion

// #region 12. Inline string and memory kernels
// ============================================================================
// 12. Inline string and memory kernels
// ============================================================================

// Local branches contain no protected keywords and survive nested macro rescans.
// Fall through on false; only the taken exit needs a local jump.
#define OBFH_INLINE_EXIT(condition, label) ((condition) ? ({ goto label; }) : (void)0)

// One draw per kernel. Selection is folded by TCC; no runtime dispatcher.
// PAIR is only for independent local updates, never ordered memory accesses.
// CUSTOM_NOP is PHANTOM_NOP emission using the captured draw. Separate slots
// with real cold setup instructions; adjacent identical NOPs only vary length.
#define OBFH_CUSTOM_BEGIN \
    OBFH_PHANTOM_CAPTURE(__obfh_custom_site, __obfh_custom_draw, __COUNTER__)
#define OBFH_CUSTOM_BIT ((__obfh_custom_draw >> (__COUNTER__ & 31u)) & 1u)
#define OBFH_CUSTOM_SELECT(condition, yes, no) \
    __builtin_choose_expr(OBFH_CUSTOM_BIT, (condition) ? (yes) : (no), !(condition) ? (no) : (yes))
#define OBFH_CUSTOM_EXIT(condition, label) OBFH_CUSTOM_SELECT(condition, ({ goto label; }), (void)0)
#define OBFH_CUSTOM_PAIR(first, second) \
    __builtin_choose_expr(OBFH_CUSTOM_BIT, ((void)(first), (void)(second)), ((void)(second), (void)(first)))
#define OBFH_CUSTOM_NOP \
    __obfh_asm__(".fill %c0, 1, 0x90;" \
                 : \
                 : "i"(OBFH_CUSTOM_BIT))
#define OBFH_CUSTOM_ASM(first, second, ...) \
    __builtin_choose_expr(OBFH_CUSTOM_BIT, ({ __obfh_asm__(first __VA_ARGS__); }), ({ __obfh_asm__(second __VA_ARGS__); }))

// Inline kernels retain entry protection and native internal control flow.
#define memchr(...) ({ \
    __label__ __obfh_memchr_loop, __obfh_memchr_found, __obfh_memchr_done; \
    OBFH_CUSTOM_BEGIN; \
    OBFH_CUSTOM_NOP; \
    struct { \
        const void *__obfh_memchr_memory; \
        int __obfh_memchr_value; \
        size_t __obfh_memchr_count; \
    } __obfh_memchr_args = {__VA_ARGS__}; \
    OBFH_CUSTOM_NOP; \
    unsigned char __obfh_memchr_wanted = (unsigned char)__obfh_memchr_args.__obfh_memchr_value; \
    void *__obfh_memchr_result = NULL; \
    BREAK_STACK_CFLOW; \
    OBFH_CUSTOM_NOP; \
__obfh_memchr_loop: \
    OBFH_CUSTOM_EXIT(!__obfh_memchr_args.__obfh_memchr_count, __obfh_memchr_done); \
    OBFH_CUSTOM_EXIT(*(const unsigned char *)__obfh_memchr_args.__obfh_memchr_memory == __obfh_memchr_wanted, __obfh_memchr_found); \
    OBFH_CUSTOM_PAIR(__obfh_memchr_args.__obfh_memchr_memory = (const unsigned char *)__obfh_memchr_args.__obfh_memchr_memory + 1, \
                     --__obfh_memchr_args.__obfh_memchr_count); \
    goto __obfh_memchr_loop; \
__obfh_memchr_found: \
    __obfh_memchr_result = (void *)__obfh_memchr_args.__obfh_memchr_memory; \
__obfh_memchr_done:; \
    OBFH_CUSTOM_NOP; \
    __obfh_memchr_result; \
})

#define strspn(...) ({ \
    __label__ __obfh_strspn_loop, __obfh_strspn_match, __obfh_strspn_select, __obfh_strspn_done; \
    OBFH_CUSTOM_BEGIN; \
    OBFH_CUSTOM_NOP; \
    struct { \
        const char *__obfh_strspn_text; \
        const char *__obfh_strspn_set; \
    } __obfh_strspn_args = {__VA_ARGS__}; \
    OBFH_CUSTOM_NOP; \
    const unsigned char *__obfh_strspn_cursor = (const unsigned char *)__obfh_strspn_args.__obfh_strspn_text; \
    const unsigned char *__obfh_strspn_member; \
    BREAK_STACK_CFLOW; \
    OBFH_CUSTOM_NOP; \
__obfh_strspn_loop: \
    OBFH_CUSTOM_EXIT(!*__obfh_strspn_cursor, __obfh_strspn_done); \
    __obfh_strspn_member = (const unsigned char *)__obfh_strspn_args.__obfh_strspn_set; \
__obfh_strspn_match: \
    OBFH_CUSTOM_EXIT(!*__obfh_strspn_member || *__obfh_strspn_member == *__obfh_strspn_cursor, __obfh_strspn_select); \
    ++__obfh_strspn_member; \
    goto __obfh_strspn_match; \
__obfh_strspn_select: \
    OBFH_CUSTOM_EXIT(!*__obfh_strspn_member, __obfh_strspn_done); \
    ++__obfh_strspn_cursor; \
    goto __obfh_strspn_loop; \
__obfh_strspn_done:; \
    OBFH_CUSTOM_NOP; \
    (size_t)(__obfh_strspn_cursor - (const unsigned char *)__obfh_strspn_args.__obfh_strspn_text); \
})

#define strcspn(...) ({ \
    __label__ __obfh_strcspn_loop, __obfh_strcspn_match, __obfh_strcspn_select, __obfh_strcspn_done; \
    OBFH_CUSTOM_BEGIN; \
    OBFH_CUSTOM_NOP; \
    struct { \
        const char *__obfh_strcspn_text; \
        const char *__obfh_strcspn_set; \
    } __obfh_strcspn_args = {__VA_ARGS__}; \
    OBFH_CUSTOM_NOP; \
    const unsigned char *__obfh_strcspn_cursor = (const unsigned char *)__obfh_strcspn_args.__obfh_strcspn_text; \
    const unsigned char *__obfh_strcspn_member; \
    BREAK_STACK_CFLOW; \
    OBFH_CUSTOM_NOP; \
__obfh_strcspn_loop: \
    OBFH_CUSTOM_EXIT(!*__obfh_strcspn_cursor, __obfh_strcspn_done); \
    __obfh_strcspn_member = (const unsigned char *)__obfh_strcspn_args.__obfh_strcspn_set; \
__obfh_strcspn_match: \
    OBFH_CUSTOM_EXIT(!*__obfh_strcspn_member || *__obfh_strcspn_member == *__obfh_strcspn_cursor, __obfh_strcspn_select); \
    ++__obfh_strcspn_member; \
    goto __obfh_strcspn_match; \
__obfh_strcspn_select: \
    OBFH_CUSTOM_EXIT(*__obfh_strcspn_member, __obfh_strcspn_done); \
    ++__obfh_strcspn_cursor; \
    goto __obfh_strcspn_loop; \
__obfh_strcspn_done:; \
    OBFH_CUSTOM_NOP; \
    (size_t)(__obfh_strcspn_cursor - (const unsigned char *)__obfh_strcspn_args.__obfh_strcspn_text); \
})

#define strpbrk(...) ({ \
    __label__ __obfh_strpbrk_loop, __obfh_strpbrk_match, __obfh_strpbrk_select, __obfh_strpbrk_done; \
    OBFH_CUSTOM_BEGIN; \
    OBFH_CUSTOM_NOP; \
    struct { \
        const char *__obfh_strpbrk_text; \
        const char *__obfh_strpbrk_set; \
    } __obfh_strpbrk_args = {__VA_ARGS__}; \
    OBFH_CUSTOM_NOP; \
    const unsigned char *__obfh_strpbrk_cursor = (const unsigned char *)__obfh_strpbrk_args.__obfh_strpbrk_text; \
    const unsigned char *__obfh_strpbrk_member; \
    BREAK_STACK_CFLOW; \
    OBFH_CUSTOM_NOP; \
__obfh_strpbrk_loop: \
    OBFH_CUSTOM_EXIT(!*__obfh_strpbrk_cursor, __obfh_strpbrk_done); \
    __obfh_strpbrk_member = (const unsigned char *)__obfh_strpbrk_args.__obfh_strpbrk_set; \
__obfh_strpbrk_match: \
    OBFH_CUSTOM_EXIT(!*__obfh_strpbrk_member || *__obfh_strpbrk_member == *__obfh_strpbrk_cursor, __obfh_strpbrk_select); \
    ++__obfh_strpbrk_member; \
    goto __obfh_strpbrk_match; \
__obfh_strpbrk_select: \
    OBFH_CUSTOM_EXIT(*__obfh_strpbrk_member, __obfh_strpbrk_done); \
    ++__obfh_strpbrk_cursor; \
    goto __obfh_strpbrk_loop; \
__obfh_strpbrk_done:; \
    OBFH_CUSTOM_NOP; \
    (char *)(*__obfh_strpbrk_cursor ? (char *)__obfh_strpbrk_cursor : NULL); \
})

#define memcmp(...) ({ \
    __label__ __obfh_memcmp_loop, __obfh_memcmp_done; \
    OBFH_CUSTOM_BEGIN; \
    OBFH_CUSTOM_NOP; \
    struct { \
        const void *__obfh_memcmp_left; \
        const void *__obfh_memcmp_right; \
        size_t __obfh_memcmp_count; \
    } __obfh_memcmp_args = {__VA_ARGS__}; \
    OBFH_CUSTOM_NOP; \
    int __obfh_memcmp_result = 0; \
    BREAK_STACK_CFLOW; \
    OBFH_CUSTOM_NOP; \
__obfh_memcmp_loop: \
    OBFH_CUSTOM_EXIT(!__obfh_memcmp_args.__obfh_memcmp_count, __obfh_memcmp_done); \
    __obfh_memcmp_result = (int)*(const unsigned char *)__obfh_memcmp_args.__obfh_memcmp_left - (int)*(const unsigned char *)__obfh_memcmp_args.__obfh_memcmp_right; \
    OBFH_CUSTOM_EXIT(__obfh_memcmp_result, __obfh_memcmp_done); \
    __obfh_memcmp_args.__obfh_memcmp_left = (const unsigned char *)__obfh_memcmp_args.__obfh_memcmp_left + 1; \
    __obfh_memcmp_args.__obfh_memcmp_right = (const unsigned char *)__obfh_memcmp_args.__obfh_memcmp_right + 1; \
    --__obfh_memcmp_args.__obfh_memcmp_count; \
    goto __obfh_memcmp_loop; \
__obfh_memcmp_done:; \
    OBFH_CUSTOM_NOP; \
    __obfh_memcmp_result; \
})

#define strncmp(...) ({ \
    __label__ __obfh_strncmp_loop, __obfh_strncmp_done; \
    OBFH_CUSTOM_BEGIN; \
    OBFH_CUSTOM_NOP; \
    struct { \
        const char *__obfh_strncmp_left; \
        const char *__obfh_strncmp_right; \
        size_t __obfh_strncmp_count; \
    } __obfh_strncmp_args = {__VA_ARGS__}; \
    OBFH_CUSTOM_NOP; \
    int __obfh_strncmp_result = 0; \
    BREAK_STACK_CFLOW; \
    OBFH_CUSTOM_NOP; \
__obfh_strncmp_loop: \
    OBFH_CUSTOM_EXIT(!__obfh_strncmp_args.__obfh_strncmp_count, __obfh_strncmp_done); \
    __obfh_strncmp_result = (int)*(const unsigned char *)__obfh_strncmp_args.__obfh_strncmp_left - (int)*(const unsigned char *)__obfh_strncmp_args.__obfh_strncmp_right; \
    OBFH_CUSTOM_EXIT(__obfh_strncmp_result || !*(const unsigned char *)__obfh_strncmp_args.__obfh_strncmp_left, __obfh_strncmp_done); \
    OBFH_CUSTOM_PAIR(++__obfh_strncmp_args.__obfh_strncmp_left, ++__obfh_strncmp_args.__obfh_strncmp_right); \
    --__obfh_strncmp_args.__obfh_strncmp_count; \
    goto __obfh_strncmp_loop; \
__obfh_strncmp_done:; \
    OBFH_CUSTOM_NOP; \
    __obfh_strncmp_result; \
})

#define strstr(...) ({ \
    __label__ __obfh_strstr_loop, __obfh_strstr_match, __obfh_strstr_advance, __obfh_strstr_found, __obfh_strstr_done; \
    OBFH_CUSTOM_BEGIN; \
    OBFH_CUSTOM_NOP; \
    struct { \
        const char *__obfh_strstr_text; \
        const char *__obfh_strstr_pattern; \
    } __obfh_strstr_args = {__VA_ARGS__}; \
    OBFH_CUSTOM_NOP; \
    const char *__obfh_strstr_a; \
    const char *__obfh_strstr_b; \
    unsigned char __obfh_strstr_first = (unsigned char)*__obfh_strstr_args.__obfh_strstr_pattern; \
    char *__obfh_strstr_result = NULL; \
    BREAK_STACK_CFLOW; \
    OBFH_CUSTOM_NOP; \
    OBFH_CUSTOM_EXIT(!__obfh_strstr_first, __obfh_strstr_found); \
__obfh_strstr_loop: \
    OBFH_CUSTOM_EXIT(!*__obfh_strstr_args.__obfh_strstr_text, __obfh_strstr_done); \
    OBFH_CUSTOM_EXIT((unsigned char)*__obfh_strstr_args.__obfh_strstr_text != __obfh_strstr_first, __obfh_strstr_advance); \
    __obfh_strstr_a = __obfh_strstr_args.__obfh_strstr_text + 1; \
    __obfh_strstr_b = __obfh_strstr_args.__obfh_strstr_pattern + 1; \
__obfh_strstr_match: \
    OBFH_CUSTOM_EXIT(!*__obfh_strstr_b, __obfh_strstr_found); \
    OBFH_CUSTOM_EXIT(*__obfh_strstr_a != *__obfh_strstr_b, __obfh_strstr_advance); \
    OBFH_CUSTOM_PAIR(++__obfh_strstr_a, ++__obfh_strstr_b); \
    goto __obfh_strstr_match; \
__obfh_strstr_advance: \
    ++__obfh_strstr_args.__obfh_strstr_text; \
    goto __obfh_strstr_loop; \
__obfh_strstr_found: \
    __obfh_strstr_result = (char *)__obfh_strstr_args.__obfh_strstr_text; \
__obfh_strstr_done:; \
    OBFH_CUSTOM_NOP; \
    __obfh_strstr_result; \
})

// Local copying, character search and wide-string kernels.
// REP copy/fill and backward word/SSE2 loads accept unaligned addresses.
// Backward moves of 64+ bytes require SSE2 on x86/x64; byte tails never change DF.
#define strchr(...) ({ \
    __label__ __obfh_strchr_loop, __obfh_strchr_found, __obfh_strchr_done; \
    OBFH_CUSTOM_BEGIN; \
    OBFH_CUSTOM_NOP; \
    struct { \
        const char *text; \
        int character; \
    } __obfh_strchr_args = {__VA_ARGS__}; \
    OBFH_CUSTOM_NOP; \
    const char *__obfh_strchr_cursor = __obfh_strchr_args.text; \
    unsigned char __obfh_strchr_wanted = (unsigned char)__obfh_strchr_args.character; \
    char *__obfh_strchr_result = NULL; \
    BREAK_STACK_CFLOW; \
    OBFH_CUSTOM_NOP; \
__obfh_strchr_loop: \
    OBFH_CUSTOM_EXIT((unsigned char)*__obfh_strchr_cursor == __obfh_strchr_wanted, __obfh_strchr_found); \
    OBFH_CUSTOM_EXIT(!*__obfh_strchr_cursor, __obfh_strchr_done); \
    ++__obfh_strchr_cursor; \
    goto __obfh_strchr_loop; \
__obfh_strchr_found: \
    __obfh_strchr_result = (char *)__obfh_strchr_cursor; \
__obfh_strchr_done:; \
    OBFH_CUSTOM_NOP; \
    __obfh_strchr_result; \
})

#define strrchr(...) ({ \
    __label__ __obfh_strrchr_loop, __obfh_strrchr_found, __obfh_strrchr_done; \
    OBFH_CUSTOM_BEGIN; \
    OBFH_CUSTOM_NOP; \
    struct { \
        const char *text; \
        int character; \
    } __obfh_strrchr_args = {__VA_ARGS__}; \
    OBFH_CUSTOM_NOP; \
    const char *__obfh_strrchr_cursor = __obfh_strrchr_args.text; \
    unsigned char __obfh_strrchr_wanted = (unsigned char)__obfh_strrchr_args.character; \
    char *__obfh_strrchr_result = NULL; \
    BREAK_STACK_CFLOW; \
    OBFH_CUSTOM_NOP; \
__obfh_strrchr_loop: \
    OBFH_CUSTOM_EXIT((unsigned char)*__obfh_strrchr_cursor == __obfh_strrchr_wanted, __obfh_strrchr_found); \
    OBFH_CUSTOM_EXIT(!*__obfh_strrchr_cursor, __obfh_strrchr_done); \
    ++__obfh_strrchr_cursor; \
    goto __obfh_strrchr_loop; \
__obfh_strrchr_found: \
    __obfh_strrchr_result = (char *)__obfh_strrchr_cursor; \
    OBFH_CUSTOM_EXIT(!*__obfh_strrchr_cursor, __obfh_strrchr_done); \
    ++__obfh_strrchr_cursor; \
    goto __obfh_strrchr_loop; \
__obfh_strrchr_done:; \
    OBFH_CUSTOM_NOP; \
    __obfh_strrchr_result; \
})

#define wcschr(...) ({ \
    __label__ __obfh_wcschr_loop, __obfh_wcschr_found, __obfh_wcschr_done; \
    OBFH_CUSTOM_BEGIN; \
    OBFH_CUSTOM_NOP; \
    struct { \
        const wchar_t *text; \
        wchar_t character; \
    } __obfh_wcschr_args = {__VA_ARGS__}; \
    OBFH_CUSTOM_NOP; \
    const wchar_t *__obfh_wcschr_cursor = __obfh_wcschr_args.text; \
    wchar_t __obfh_wcschr_wanted = (wchar_t)__obfh_wcschr_args.character; \
    wchar_t *__obfh_wcschr_result = NULL; \
    BREAK_STACK_CFLOW; \
    OBFH_CUSTOM_NOP; \
__obfh_wcschr_loop: \
    OBFH_CUSTOM_EXIT(*__obfh_wcschr_cursor == __obfh_wcschr_wanted, __obfh_wcschr_found); \
    OBFH_CUSTOM_EXIT(!*__obfh_wcschr_cursor, __obfh_wcschr_done); \
    ++__obfh_wcschr_cursor; \
    goto __obfh_wcschr_loop; \
__obfh_wcschr_found: \
    __obfh_wcschr_result = (wchar_t *)__obfh_wcschr_cursor; \
__obfh_wcschr_done:; \
    OBFH_CUSTOM_NOP; \
    __obfh_wcschr_result; \
})

#define wcsrchr(...) ({ \
    __label__ __obfh_wcsrchr_loop, __obfh_wcsrchr_found, __obfh_wcsrchr_done; \
    OBFH_CUSTOM_BEGIN; \
    OBFH_CUSTOM_NOP; \
    struct { \
        const wchar_t *text; \
        wchar_t character; \
    } __obfh_wcsrchr_args = {__VA_ARGS__}; \
    OBFH_CUSTOM_NOP; \
    const wchar_t *__obfh_wcsrchr_cursor = __obfh_wcsrchr_args.text; \
    wchar_t __obfh_wcsrchr_wanted = (wchar_t)__obfh_wcsrchr_args.character; \
    wchar_t *__obfh_wcsrchr_result = NULL; \
    BREAK_STACK_CFLOW; \
    OBFH_CUSTOM_NOP; \
__obfh_wcsrchr_loop: \
    OBFH_CUSTOM_EXIT(*__obfh_wcsrchr_cursor == __obfh_wcsrchr_wanted, __obfh_wcsrchr_found); \
    OBFH_CUSTOM_EXIT(!*__obfh_wcsrchr_cursor, __obfh_wcsrchr_done); \
    ++__obfh_wcsrchr_cursor; \
    goto __obfh_wcsrchr_loop; \
__obfh_wcsrchr_found: \
    __obfh_wcsrchr_result = (wchar_t *)__obfh_wcsrchr_cursor; \
    OBFH_CUSTOM_EXIT(!*__obfh_wcsrchr_cursor, __obfh_wcsrchr_done); \
    ++__obfh_wcsrchr_cursor; \
    goto __obfh_wcsrchr_loop; \
__obfh_wcsrchr_done:; \
    OBFH_CUSTOM_NOP; \
    __obfh_wcsrchr_result; \
})

#define strcpy(...) ({ \
    __label__ __obfh_strcpy_copy, __obfh_strcpy_done; \
    OBFH_CUSTOM_BEGIN; \
    OBFH_CUSTOM_NOP; \
    struct { \
        char *destination; \
        const char *text; \
    } __obfh_strcpy_args = {__VA_ARGS__}; \
    OBFH_CUSTOM_NOP; \
    char *__obfh_strcpy_target = __obfh_strcpy_args.destination; \
    const char *__obfh_strcpy_source = __obfh_strcpy_args.text; \
    char __obfh_strcpy_value; \
    BREAK_STACK_CFLOW; \
    OBFH_CUSTOM_NOP; \
__obfh_strcpy_copy: \
    __obfh_strcpy_value = *__obfh_strcpy_source; \
    *__obfh_strcpy_target++ = __obfh_strcpy_value; \
    OBFH_CUSTOM_EXIT(!__obfh_strcpy_value, __obfh_strcpy_done); \
    ++__obfh_strcpy_source; \
    goto __obfh_strcpy_copy; \
__obfh_strcpy_done:; \
    OBFH_CUSTOM_NOP; \
    __obfh_strcpy_args.destination; \
})

#define strcat(...) ({ \
    __label__ __obfh_strcat_scan, __obfh_strcat_copy, __obfh_strcat_done; \
    OBFH_CUSTOM_BEGIN; \
    OBFH_CUSTOM_NOP; \
    struct { \
        char *destination; \
        const char *text; \
    } __obfh_strcat_args = {__VA_ARGS__}; \
    OBFH_CUSTOM_NOP; \
    char *__obfh_strcat_target = __obfh_strcat_args.destination; \
    const char *__obfh_strcat_source = __obfh_strcat_args.text; \
    char __obfh_strcat_value; \
    BREAK_STACK_CFLOW; \
    OBFH_CUSTOM_NOP; \
__obfh_strcat_scan: \
    OBFH_CUSTOM_EXIT(!*__obfh_strcat_target, __obfh_strcat_copy); \
    ++__obfh_strcat_target; \
    goto __obfh_strcat_scan; \
__obfh_strcat_copy: \
    __obfh_strcat_value = *__obfh_strcat_source; \
    *__obfh_strcat_target++ = __obfh_strcat_value; \
    OBFH_CUSTOM_EXIT(!__obfh_strcat_value, __obfh_strcat_done); \
    ++__obfh_strcat_source; \
    goto __obfh_strcat_copy; \
__obfh_strcat_done:; \
    OBFH_CUSTOM_NOP; \
    __obfh_strcat_args.destination; \
})

#define strncpy(...) ({ \
    __label__ __obfh_strncpy_copy, __obfh_strncpy_fill, __obfh_strncpy_done; \
    OBFH_CUSTOM_BEGIN; \
    OBFH_CUSTOM_NOP; \
    struct { \
        char *destination; \
        const char *text; \
        size_t count; \
    } __obfh_strncpy_args = {__VA_ARGS__}; \
    OBFH_CUSTOM_NOP; \
    char *__obfh_strncpy_target = __obfh_strncpy_args.destination; \
    const char *__obfh_strncpy_source = __obfh_strncpy_args.text; \
    char __obfh_strncpy_value; \
    BREAK_STACK_CFLOW; \
    OBFH_CUSTOM_NOP; \
__obfh_strncpy_copy: \
    OBFH_CUSTOM_EXIT(!__obfh_strncpy_args.count, __obfh_strncpy_done); \
    __obfh_strncpy_value = *__obfh_strncpy_source; \
    *__obfh_strncpy_target++ = __obfh_strncpy_value; \
    --__obfh_strncpy_args.count; \
    OBFH_CUSTOM_EXIT(!__obfh_strncpy_value, __obfh_strncpy_fill); \
    ++__obfh_strncpy_source; \
    goto __obfh_strncpy_copy; \
__obfh_strncpy_fill: \
    OBFH_CUSTOM_EXIT(!__obfh_strncpy_args.count, __obfh_strncpy_done); \
    *__obfh_strncpy_target++ = 0; \
    --__obfh_strncpy_args.count; \
    goto __obfh_strncpy_fill; \
__obfh_strncpy_done:; \
    OBFH_CUSTOM_NOP; \
    __obfh_strncpy_args.destination; \
})

#define strncat(...) ({ \
    __label__ __obfh_strncat_scan, __obfh_strncat_copy, __obfh_strncat_fill, __obfh_strncat_done; \
    OBFH_CUSTOM_BEGIN; \
    OBFH_CUSTOM_NOP; \
    struct { \
        char *destination; \
        const char *text; \
        size_t count; \
    } __obfh_strncat_args = {__VA_ARGS__}; \
    OBFH_CUSTOM_NOP; \
    char *__obfh_strncat_target = __obfh_strncat_args.destination; \
    const char *__obfh_strncat_source = __obfh_strncat_args.text; \
    char __obfh_strncat_value; \
    BREAK_STACK_CFLOW; \
    OBFH_CUSTOM_NOP; \
__obfh_strncat_scan: \
    OBFH_CUSTOM_EXIT(!*__obfh_strncat_target, __obfh_strncat_copy); \
    ++__obfh_strncat_target; \
    goto __obfh_strncat_scan; \
__obfh_strncat_copy: \
    OBFH_CUSTOM_EXIT(!__obfh_strncat_args.count, __obfh_strncat_fill); \
    __obfh_strncat_value = *__obfh_strncat_source; \
    *__obfh_strncat_target++ = __obfh_strncat_value; \
    --__obfh_strncat_args.count; \
    OBFH_CUSTOM_EXIT(!__obfh_strncat_value, __obfh_strncat_done); \
    ++__obfh_strncat_source; \
    goto __obfh_strncat_copy; \
__obfh_strncat_fill: \
    *__obfh_strncat_target = 0; \
__obfh_strncat_done:; \
    OBFH_CUSTOM_NOP; \
    __obfh_strncat_args.destination; \
})

#define memcpy(...) ({ \
    OBFH_CUSTOM_BEGIN; \
    OBFH_CUSTOM_NOP; \
    struct { \
        void *destination; \
        const void *text; \
        size_t count; \
    } __obfh_memcpy_args = {__VA_ARGS__}; \
    OBFH_CUSTOM_NOP; \
    void *__obfh_memcpy_target = __obfh_memcpy_args.destination; \
    const void *__obfh_memcpy_source = __obfh_memcpy_args.text; \
    size_t __obfh_memcpy_count = __obfh_memcpy_args.count; \
    BREAK_STACK_CFLOW; \
    OBFH_CUSTOM_NOP; \
    __obfh_asm__("rep movsb" \
                 : "+D"(__obfh_memcpy_target), "+S"(__obfh_memcpy_source), \
                   "+c"(__obfh_memcpy_count) \
                 : \
                 : "memory"); \
    __obfh_memcpy_args.destination; \
})

#define memset(...) ({ \
    OBFH_CUSTOM_BEGIN; \
    OBFH_CUSTOM_NOP; \
    struct { \
        void *destination; \
        int character; \
        size_t count; \
    } __obfh_memset_args = {__VA_ARGS__}; \
    OBFH_CUSTOM_NOP; \
    void *__obfh_memset_target = __obfh_memset_args.destination; \
    size_t __obfh_memset_count = __obfh_memset_args.count; \
    BREAK_STACK_CFLOW; \
    OBFH_CUSTOM_NOP; \
    __obfh_asm__("rep stosb" \
                 : "+D"(__obfh_memset_target), "+c"(__obfh_memset_count) \
                 : "a"((unsigned char)__obfh_memset_args.character) \
                 : "memory"); \
    __obfh_memset_args.destination; \
})

#if defined(__x86_64__)
#define OBFH_MOVE_WIDTH "q"
#define OBFH_MOVE_REX ".byte 0x48;"
#define OBFH_MOVE_STEP "8"
#else
#define OBFH_MOVE_WIDTH "l"
#define OBFH_MOVE_REX ""
#define OBFH_MOVE_STEP "4"
#endif
// Same instructions/access order; only independent pointer updates are permuted.
#define OBFH_MOVE_BACKWARD_ORDER_A(step) \
    "sub" OBFH_MOVE_WIDTH " $" step ", %0; sub" OBFH_MOVE_WIDTH " $" step ", %1; "
#define OBFH_MOVE_BACKWARD_ORDER_B(step) \
    "sub" OBFH_MOVE_WIDTH " $" step ", %1; sub" OBFH_MOVE_WIDTH " $" step ", %0; "
#define OBFH_MOVE_TAIL(order) \
    "test" OBFH_MOVE_WIDTH " %2, %2; jz 2f; 1: .byte 0x8a,(0x06+%c3); .byte 0x88,(0x07+%c3); " order "dec" OBFH_MOVE_WIDTH " %2; jnz 1b; 2:"
#define OBFH_MOVE_TAIL_ORDER_A \
    "dec" OBFH_MOVE_WIDTH " %0; dec" OBFH_MOVE_WIDTH " %1; "
#define OBFH_MOVE_TAIL_ORDER_B \
    "dec" OBFH_MOVE_WIDTH " %1; dec" OBFH_MOVE_WIDTH " %0; "
#define OBFH_MOVE_WORD(order) \
    "1: " OBFH_MOVE_REX ".byte 0x8b,(0x06+%c3); " OBFH_MOVE_REX ".byte 0x89,(0x07+%c3); " order "dec" OBFH_MOVE_WIDTH " %2; jnz 1b;"
// All forms copy the same 128 bits, including unaligned operands.
#if (OBFH_SEED >> 23) % 3u == 0u
#define OBFH_MOVE_VECTOR_LOAD "0xf3,0x0f,0x6f,"
#define OBFH_MOVE_VECTOR_STORE "0xf3,0x0f,0x7f,"
#endif
#if (OBFH_SEED >> 23) % 3u == 1u
#define OBFH_MOVE_VECTOR_LOAD "0x0f,0x10,"
#define OBFH_MOVE_VECTOR_STORE "0x0f,0x11,"
#endif
#if !(((OBFH_SEED >> 23) % 3u == 0u) || ((OBFH_SEED >> 23) % 3u == 1u))
#define OBFH_MOVE_VECTOR_LOAD "0x66,0x0f,0x10,"
#define OBFH_MOVE_VECTOR_STORE "0x66,0x0f,0x11,"
#endif
#define OBFH_MOVE_VECTOR(order) \
    ".byte " OBFH_MOVE_VECTOR_STORE "(0x02+%c4); 1: .byte " OBFH_MOVE_VECTOR_LOAD "(0x06+%c4); .byte " OBFH_MOVE_VECTOR_STORE "(0x07+%c4); " order "dec" OBFH_MOVE_WIDTH " %2; jnz 1b; .byte " OBFH_MOVE_VECTOR_LOAD "(0x02+%c4);"
#define memmove(...) ({ \
    __label__ __obfh_memmove_backward, __obfh_memmove_vector, __obfh_memmove_done; \
    OBFH_CUSTOM_BEGIN; \
    OBFH_CUSTOM_NOP; \
    struct { \
        void *destination; \
        const void *text; \
        size_t count; \
    } __obfh_memmove_args = {__VA_ARGS__}; \
    OBFH_CUSTOM_NOP; \
    void *__obfh_memmove_target = __obfh_memmove_args.destination; \
    const void *__obfh_memmove_source = __obfh_memmove_args.text; \
    size_t __obfh_memmove_count = __obfh_memmove_args.count; \
    BREAK_STACK_CFLOW; \
    OBFH_CUSTOM_NOP; \
    OBFH_CUSTOM_EXIT(!__obfh_memmove_count, __obfh_memmove_done); \
    OBFH_CUSTOM_EXIT((ULONG_PTR)__obfh_memmove_target > (ULONG_PTR)__obfh_memmove_source && \
                         (ULONG_PTR)__obfh_memmove_target - (ULONG_PTR)__obfh_memmove_source < __obfh_memmove_count, \
                     __obfh_memmove_backward); \
    __obfh_asm__("rep movsb" \
                 : "+D"(__obfh_memmove_target), "+S"(__obfh_memmove_source), \
                   "+c"(__obfh_memmove_count) \
                 : \
                 : "memory"); \
    goto __obfh_memmove_done; \
__obfh_memmove_backward: \
    OBFH_CUSTOM_EXIT(__obfh_memmove_count >= 64, __obfh_memmove_vector); \
    OBFH_CUSTOM_PAIR(__obfh_memmove_target = (unsigned char *)__obfh_memmove_target + __obfh_memmove_count - 1, \
                     __obfh_memmove_source = (const unsigned char *)__obfh_memmove_source + __obfh_memmove_count - 1); \
    size_t __obfh_memmove_tail = __obfh_memmove_count % sizeof(ULONG_PTR); \
    __obfh_memmove_count /= sizeof(ULONG_PTR); \
    OBFH_CUSTOM_ASM(OBFH_MOVE_TAIL(OBFH_MOVE_TAIL_ORDER_A), OBFH_MOVE_TAIL(OBFH_MOVE_TAIL_ORDER_B), \
                    : "+D"(__obfh_memmove_target), "+S"(__obfh_memmove_source), "+c"(__obfh_memmove_tail) \
                    : "i"(((__obfh_custom_draw >> 4) & 1u) * 16u) \
                    : "eax", "edx", "cc", "memory"); \
    OBFH_CUSTOM_EXIT(!__obfh_memmove_count, __obfh_memmove_done); \
    OBFH_CUSTOM_PAIR(__obfh_memmove_target = (unsigned char *)__obfh_memmove_target - (sizeof(ULONG_PTR) - 1), \
                     __obfh_memmove_source = (const unsigned char *)__obfh_memmove_source - (sizeof(ULONG_PTR) - 1)); \
    OBFH_CUSTOM_ASM(OBFH_MOVE_WORD(OBFH_MOVE_BACKWARD_ORDER_A(OBFH_MOVE_STEP)), \
                    OBFH_MOVE_WORD(OBFH_MOVE_BACKWARD_ORDER_B(OBFH_MOVE_STEP)), \
                    : "+D"(__obfh_memmove_target), "+S"(__obfh_memmove_source), "+c"(__obfh_memmove_count) \
                    : "i"(((__obfh_custom_draw >> 4) & 1u) * 16u) \
                    : "eax", "edx", "cc", "memory"); \
    goto __obfh_memmove_done; \
__obfh_memmove_vector: \
    OBFH_CUSTOM_PAIR(__obfh_memmove_target = (unsigned char *)__obfh_memmove_target + __obfh_memmove_count - 1, \
                     __obfh_memmove_source = (const unsigned char *)__obfh_memmove_source + __obfh_memmove_count - 1); \
    size_t __obfh_memmove_vector_tail = __obfh_memmove_count & 15u; \
    __obfh_memmove_count >>= 4; \
    OBFH_CUSTOM_ASM(OBFH_MOVE_TAIL(OBFH_MOVE_TAIL_ORDER_A), OBFH_MOVE_TAIL(OBFH_MOVE_TAIL_ORDER_B), \
                    : "+D"(__obfh_memmove_target), "+S"(__obfh_memmove_source), "+c"(__obfh_memmove_vector_tail) \
                    : "i"(((__obfh_custom_draw >> 4) & 1u) * 16u) \
                    : "eax", "edx", "cc", "memory"); \
    OBFH_CUSTOM_PAIR(__obfh_memmove_target = (unsigned char *)__obfh_memmove_target - 15, \
                     __obfh_memmove_source = (const unsigned char *)__obfh_memmove_source - 15); \
    /* Fixed D/S/d addresses; save/restore the selected XMM0..7 (TCC lacks XMM clobbers). */ \
    unsigned char __obfh_memmove_xmm_save[16]; \
    OBFH_CUSTOM_ASM(OBFH_MOVE_VECTOR(OBFH_MOVE_BACKWARD_ORDER_A("16")), \
                    OBFH_MOVE_VECTOR(OBFH_MOVE_BACKWARD_ORDER_B("16")), \
                    : "+D"(__obfh_memmove_target), "+S"(__obfh_memmove_source), "+c"(__obfh_memmove_count) \
                    : "d"(__obfh_memmove_xmm_save), "i"(((__obfh_custom_draw >> 8) & 7u) * 8u) \
                    : "cc", "memory"); \
__obfh_memmove_done:; \
    OBFH_CUSTOM_NOP; \
    __obfh_memmove_args.destination; \
})

#define wcslen(...) ({ \
    __label__ __obfh_wcslen_loop, __obfh_wcslen_done; \
    OBFH_CUSTOM_BEGIN; \
    OBFH_CUSTOM_NOP; \
    struct { \
        const wchar_t *text; \
    } __obfh_wcslen_args = {__VA_ARGS__}; \
    OBFH_CUSTOM_NOP; \
    size_t __obfh_wcslen_index = 0; \
    BREAK_STACK_CFLOW; \
    OBFH_CUSTOM_NOP; \
__obfh_wcslen_loop: \
    OBFH_CUSTOM_EXIT(!__obfh_wcslen_args.text[__obfh_wcslen_index], __obfh_wcslen_done); \
    ++__obfh_wcslen_index; \
    goto __obfh_wcslen_loop; \
__obfh_wcslen_done:; \
    OBFH_CUSTOM_NOP; \
    __obfh_wcslen_index; \
})

#define wcscmp(...) ({ \
    __label__ __obfh_wcscmp_loop, __obfh_wcscmp_done; \
    OBFH_CUSTOM_BEGIN; \
    OBFH_CUSTOM_NOP; \
    struct { \
        const wchar_t *left; \
        const wchar_t *right; \
    } __obfh_wcscmp_args = {__VA_ARGS__}; \
    OBFH_CUSTOM_NOP; \
    int __obfh_wcscmp_result = 0; \
    BREAK_STACK_CFLOW; \
    OBFH_CUSTOM_NOP; \
__obfh_wcscmp_loop: \
    __obfh_wcscmp_result = (*__obfh_wcscmp_args.left > *__obfh_wcscmp_args.right) - (*__obfh_wcscmp_args.left < *__obfh_wcscmp_args.right); \
    OBFH_CUSTOM_EXIT(__obfh_wcscmp_result || !*__obfh_wcscmp_args.left, __obfh_wcscmp_done); \
    OBFH_CUSTOM_PAIR(++__obfh_wcscmp_args.left, ++__obfh_wcscmp_args.right); \
    goto __obfh_wcscmp_loop; \
__obfh_wcscmp_done:; \
    OBFH_CUSTOM_NOP; \
    __obfh_wcscmp_result; \
})

#define wcsncmp(...) ({ \
    __label__ __obfh_wcsncmp_loop, __obfh_wcsncmp_done; \
    OBFH_CUSTOM_BEGIN; \
    OBFH_CUSTOM_NOP; \
    struct { \
        const wchar_t *left; \
        const wchar_t *right; \
        size_t count; \
    } __obfh_wcsncmp_args = {__VA_ARGS__}; \
    OBFH_CUSTOM_NOP; \
    int __obfh_wcsncmp_result = 0; \
    BREAK_STACK_CFLOW; \
    OBFH_CUSTOM_NOP; \
__obfh_wcsncmp_loop: \
    OBFH_CUSTOM_EXIT(!__obfh_wcsncmp_args.count, __obfh_wcsncmp_done); \
    __obfh_wcsncmp_result = (*__obfh_wcsncmp_args.left > *__obfh_wcsncmp_args.right) - (*__obfh_wcsncmp_args.left < *__obfh_wcsncmp_args.right); \
    OBFH_CUSTOM_EXIT(__obfh_wcsncmp_result || !*__obfh_wcsncmp_args.left, __obfh_wcsncmp_done); \
    OBFH_CUSTOM_PAIR(++__obfh_wcsncmp_args.left, ++__obfh_wcsncmp_args.right); \
    --__obfh_wcsncmp_args.count; \
    goto __obfh_wcsncmp_loop; \
__obfh_wcsncmp_done:; \
    OBFH_CUSTOM_NOP; \
    __obfh_wcsncmp_result; \
})

// Wide copies/searches and bounded lengths retain native internal branches.
#define wcscpy(...) ({ \
    __label__ __obfh_wcscpy_copy, __obfh_wcscpy_done; \
    OBFH_CUSTOM_BEGIN; \
    OBFH_CUSTOM_NOP; \
    struct { \
        wchar_t *destination; \
        const wchar_t *text; \
    } __obfh_wcscpy_args = {__VA_ARGS__}; \
    OBFH_CUSTOM_NOP; \
    wchar_t *__obfh_wcscpy_target = __obfh_wcscpy_args.destination; \
    const wchar_t *__obfh_wcscpy_source = __obfh_wcscpy_args.text; \
    wchar_t __obfh_wcscpy_value; \
    BREAK_STACK_CFLOW; \
    OBFH_CUSTOM_NOP; \
__obfh_wcscpy_copy: \
    __obfh_wcscpy_value = *__obfh_wcscpy_source; \
    *__obfh_wcscpy_target++ = __obfh_wcscpy_value; \
    OBFH_CUSTOM_EXIT(!__obfh_wcscpy_value, __obfh_wcscpy_done); \
    ++__obfh_wcscpy_source; \
    goto __obfh_wcscpy_copy; \
__obfh_wcscpy_done:; \
    OBFH_CUSTOM_NOP; \
    __obfh_wcscpy_args.destination; \
})

#define wcscat(...) ({ \
    __label__ __obfh_wcscat_scan, __obfh_wcscat_copy, __obfh_wcscat_done; \
    OBFH_CUSTOM_BEGIN; \
    OBFH_CUSTOM_NOP; \
    struct { \
        wchar_t *destination; \
        const wchar_t *text; \
    } __obfh_wcscat_args = {__VA_ARGS__}; \
    OBFH_CUSTOM_NOP; \
    wchar_t *__obfh_wcscat_target = __obfh_wcscat_args.destination; \
    const wchar_t *__obfh_wcscat_source = __obfh_wcscat_args.text; \
    wchar_t __obfh_wcscat_value; \
    BREAK_STACK_CFLOW; \
    OBFH_CUSTOM_NOP; \
__obfh_wcscat_scan: \
    OBFH_CUSTOM_EXIT(!*__obfh_wcscat_target, __obfh_wcscat_copy); \
    ++__obfh_wcscat_target; \
    goto __obfh_wcscat_scan; \
__obfh_wcscat_copy: \
    __obfh_wcscat_value = *__obfh_wcscat_source; \
    *__obfh_wcscat_target++ = __obfh_wcscat_value; \
    OBFH_CUSTOM_EXIT(!__obfh_wcscat_value, __obfh_wcscat_done); \
    ++__obfh_wcscat_source; \
    goto __obfh_wcscat_copy; \
__obfh_wcscat_done:; \
    OBFH_CUSTOM_NOP; \
    __obfh_wcscat_args.destination; \
})

#define wcsncpy(...) ({ \
    __label__ __obfh_wcsncpy_copy, __obfh_wcsncpy_fill, __obfh_wcsncpy_done; \
    OBFH_CUSTOM_BEGIN; \
    OBFH_CUSTOM_NOP; \
    struct { \
        wchar_t *destination; \
        const wchar_t *text; \
        size_t count; \
    } __obfh_wcsncpy_args = {__VA_ARGS__}; \
    OBFH_CUSTOM_NOP; \
    wchar_t *__obfh_wcsncpy_target = __obfh_wcsncpy_args.destination; \
    const wchar_t *__obfh_wcsncpy_source = __obfh_wcsncpy_args.text; \
    wchar_t __obfh_wcsncpy_value; \
    BREAK_STACK_CFLOW; \
    OBFH_CUSTOM_NOP; \
__obfh_wcsncpy_copy: \
    OBFH_CUSTOM_EXIT(!__obfh_wcsncpy_args.count, __obfh_wcsncpy_done); \
    __obfh_wcsncpy_value = *__obfh_wcsncpy_source; \
    *__obfh_wcsncpy_target++ = __obfh_wcsncpy_value; \
    --__obfh_wcsncpy_args.count; \
    OBFH_CUSTOM_EXIT(!__obfh_wcsncpy_value, __obfh_wcsncpy_fill); \
    ++__obfh_wcsncpy_source; \
    goto __obfh_wcsncpy_copy; \
__obfh_wcsncpy_fill: \
    OBFH_CUSTOM_EXIT(!__obfh_wcsncpy_args.count, __obfh_wcsncpy_done); \
    *__obfh_wcsncpy_target++ = 0; \
    --__obfh_wcsncpy_args.count; \
    goto __obfh_wcsncpy_fill; \
__obfh_wcsncpy_done:; \
    OBFH_CUSTOM_NOP; \
    __obfh_wcsncpy_args.destination; \
})

#define wcsncat(...) ({ \
    __label__ __obfh_wcsncat_scan, __obfh_wcsncat_copy, __obfh_wcsncat_fill, __obfh_wcsncat_done; \
    OBFH_CUSTOM_BEGIN; \
    OBFH_CUSTOM_NOP; \
    struct { \
        wchar_t *destination; \
        const wchar_t *text; \
        size_t count; \
    } __obfh_wcsncat_args = {__VA_ARGS__}; \
    OBFH_CUSTOM_NOP; \
    wchar_t *__obfh_wcsncat_target = __obfh_wcsncat_args.destination; \
    const wchar_t *__obfh_wcsncat_source = __obfh_wcsncat_args.text; \
    wchar_t __obfh_wcsncat_value; \
    BREAK_STACK_CFLOW; \
    OBFH_CUSTOM_NOP; \
__obfh_wcsncat_scan: \
    OBFH_CUSTOM_EXIT(!*__obfh_wcsncat_target, __obfh_wcsncat_copy); \
    ++__obfh_wcsncat_target; \
    goto __obfh_wcsncat_scan; \
__obfh_wcsncat_copy: \
    OBFH_CUSTOM_EXIT(!__obfh_wcsncat_args.count, __obfh_wcsncat_fill); \
    __obfh_wcsncat_value = *__obfh_wcsncat_source; \
    *__obfh_wcsncat_target++ = __obfh_wcsncat_value; \
    --__obfh_wcsncat_args.count; \
    OBFH_CUSTOM_EXIT(!__obfh_wcsncat_value, __obfh_wcsncat_done); \
    ++__obfh_wcsncat_source; \
    goto __obfh_wcsncat_copy; \
__obfh_wcsncat_fill: \
    *__obfh_wcsncat_target = 0; \
__obfh_wcsncat_done:; \
    OBFH_CUSTOM_NOP; \
    __obfh_wcsncat_args.destination; \
})

#define wcsstr(...) ({ \
    __label__ __obfh_wcsstr_loop, __obfh_wcsstr_match, __obfh_wcsstr_advance, __obfh_wcsstr_found, __obfh_wcsstr_done; \
    OBFH_CUSTOM_BEGIN; \
    OBFH_CUSTOM_NOP; \
    struct { \
        const wchar_t *__obfh_wcsstr_text; \
        const wchar_t *__obfh_wcsstr_pattern; \
    } __obfh_wcsstr_args = {__VA_ARGS__}; \
    OBFH_CUSTOM_NOP; \
    const wchar_t *__obfh_wcsstr_a; \
    const wchar_t *__obfh_wcsstr_b; \
    wchar_t __obfh_wcsstr_first = (wchar_t)*__obfh_wcsstr_args.__obfh_wcsstr_pattern; \
    wchar_t *__obfh_wcsstr_result = NULL; \
    BREAK_STACK_CFLOW; \
    OBFH_CUSTOM_NOP; \
    OBFH_CUSTOM_EXIT(!__obfh_wcsstr_first, __obfh_wcsstr_found); \
__obfh_wcsstr_loop: \
    OBFH_CUSTOM_EXIT(!*__obfh_wcsstr_args.__obfh_wcsstr_text, __obfh_wcsstr_done); \
    OBFH_CUSTOM_EXIT((wchar_t)*__obfh_wcsstr_args.__obfh_wcsstr_text != __obfh_wcsstr_first, __obfh_wcsstr_advance); \
    __obfh_wcsstr_a = __obfh_wcsstr_args.__obfh_wcsstr_text + 1; \
    __obfh_wcsstr_b = __obfh_wcsstr_args.__obfh_wcsstr_pattern + 1; \
__obfh_wcsstr_match: \
    OBFH_CUSTOM_EXIT(!*__obfh_wcsstr_b, __obfh_wcsstr_found); \
    OBFH_CUSTOM_EXIT(*__obfh_wcsstr_a != *__obfh_wcsstr_b, __obfh_wcsstr_advance); \
    OBFH_CUSTOM_PAIR(++__obfh_wcsstr_a, ++__obfh_wcsstr_b); \
    goto __obfh_wcsstr_match; \
__obfh_wcsstr_advance: \
    ++__obfh_wcsstr_args.__obfh_wcsstr_text; \
    goto __obfh_wcsstr_loop; \
__obfh_wcsstr_found: \
    __obfh_wcsstr_result = (wchar_t *)__obfh_wcsstr_args.__obfh_wcsstr_text; \
__obfh_wcsstr_done:; \
    OBFH_CUSTOM_NOP; \
    __obfh_wcsstr_result; \
})

#define wcsspn(...) ({ \
    __label__ __obfh_wcsspn_loop, __obfh_wcsspn_match, __obfh_wcsspn_select, __obfh_wcsspn_done; \
    OBFH_CUSTOM_BEGIN; \
    OBFH_CUSTOM_NOP; \
    struct { \
        const wchar_t *__obfh_wcsspn_text; \
        const wchar_t *__obfh_wcsspn_set; \
    } __obfh_wcsspn_args = {__VA_ARGS__}; \
    OBFH_CUSTOM_NOP; \
    const wchar_t *__obfh_wcsspn_cursor = (const wchar_t *)__obfh_wcsspn_args.__obfh_wcsspn_text; \
    const wchar_t *__obfh_wcsspn_member; \
    BREAK_STACK_CFLOW; \
    OBFH_CUSTOM_NOP; \
__obfh_wcsspn_loop: \
    OBFH_CUSTOM_EXIT(!*__obfh_wcsspn_cursor, __obfh_wcsspn_done); \
    __obfh_wcsspn_member = (const wchar_t *)__obfh_wcsspn_args.__obfh_wcsspn_set; \
__obfh_wcsspn_match: \
    OBFH_CUSTOM_EXIT(!*__obfh_wcsspn_member || *__obfh_wcsspn_member == *__obfh_wcsspn_cursor, __obfh_wcsspn_select); \
    ++__obfh_wcsspn_member; \
    goto __obfh_wcsspn_match; \
__obfh_wcsspn_select: \
    OBFH_CUSTOM_EXIT(!*__obfh_wcsspn_member, __obfh_wcsspn_done); \
    ++__obfh_wcsspn_cursor; \
    goto __obfh_wcsspn_loop; \
__obfh_wcsspn_done:; \
    OBFH_CUSTOM_NOP; \
    (size_t)(__obfh_wcsspn_cursor - (const wchar_t *)__obfh_wcsspn_args.__obfh_wcsspn_text); \
})

#define wcscspn(...) ({ \
    __label__ __obfh_wcscspn_loop, __obfh_wcscspn_match, __obfh_wcscspn_select, __obfh_wcscspn_done; \
    OBFH_CUSTOM_BEGIN; \
    OBFH_CUSTOM_NOP; \
    struct { \
        const wchar_t *__obfh_wcscspn_text; \
        const wchar_t *__obfh_wcscspn_set; \
    } __obfh_wcscspn_args = {__VA_ARGS__}; \
    OBFH_CUSTOM_NOP; \
    const wchar_t *__obfh_wcscspn_cursor = (const wchar_t *)__obfh_wcscspn_args.__obfh_wcscspn_text; \
    const wchar_t *__obfh_wcscspn_member; \
    BREAK_STACK_CFLOW; \
    OBFH_CUSTOM_NOP; \
__obfh_wcscspn_loop: \
    OBFH_CUSTOM_EXIT(!*__obfh_wcscspn_cursor, __obfh_wcscspn_done); \
    __obfh_wcscspn_member = (const wchar_t *)__obfh_wcscspn_args.__obfh_wcscspn_set; \
__obfh_wcscspn_match: \
    OBFH_CUSTOM_EXIT(!*__obfh_wcscspn_member || *__obfh_wcscspn_member == *__obfh_wcscspn_cursor, __obfh_wcscspn_select); \
    ++__obfh_wcscspn_member; \
    goto __obfh_wcscspn_match; \
__obfh_wcscspn_select: \
    OBFH_CUSTOM_EXIT(*__obfh_wcscspn_member, __obfh_wcscspn_done); \
    ++__obfh_wcscspn_cursor; \
    goto __obfh_wcscspn_loop; \
__obfh_wcscspn_done:; \
    OBFH_CUSTOM_NOP; \
    (size_t)(__obfh_wcscspn_cursor - (const wchar_t *)__obfh_wcscspn_args.__obfh_wcscspn_text); \
})

#define wcspbrk(...) ({ \
    __label__ __obfh_wcspbrk_loop, __obfh_wcspbrk_match, __obfh_wcspbrk_select, __obfh_wcspbrk_done; \
    OBFH_CUSTOM_BEGIN; \
    OBFH_CUSTOM_NOP; \
    struct { \
        const wchar_t *__obfh_wcspbrk_text; \
        const wchar_t *__obfh_wcspbrk_set; \
    } __obfh_wcspbrk_args = {__VA_ARGS__}; \
    OBFH_CUSTOM_NOP; \
    const wchar_t *__obfh_wcspbrk_cursor = (const wchar_t *)__obfh_wcspbrk_args.__obfh_wcspbrk_text; \
    const wchar_t *__obfh_wcspbrk_member; \
    BREAK_STACK_CFLOW; \
    OBFH_CUSTOM_NOP; \
__obfh_wcspbrk_loop: \
    OBFH_CUSTOM_EXIT(!*__obfh_wcspbrk_cursor, __obfh_wcspbrk_done); \
    __obfh_wcspbrk_member = (const wchar_t *)__obfh_wcspbrk_args.__obfh_wcspbrk_set; \
__obfh_wcspbrk_match: \
    OBFH_CUSTOM_EXIT(!*__obfh_wcspbrk_member || *__obfh_wcspbrk_member == *__obfh_wcspbrk_cursor, __obfh_wcspbrk_select); \
    ++__obfh_wcspbrk_member; \
    goto __obfh_wcspbrk_match; \
__obfh_wcspbrk_select: \
    OBFH_CUSTOM_EXIT(*__obfh_wcspbrk_member, __obfh_wcspbrk_done); \
    ++__obfh_wcspbrk_cursor; \
    goto __obfh_wcspbrk_loop; \
__obfh_wcspbrk_done:; \
    OBFH_CUSTOM_NOP; \
    (wchar_t *)(*__obfh_wcspbrk_cursor ? (wchar_t *)__obfh_wcspbrk_cursor : NULL); \
})

#define strnlen(...) ({ \
    __label__ __obfh_strnlen_loop, __obfh_strnlen_done; \
    OBFH_CUSTOM_BEGIN; \
    OBFH_CUSTOM_NOP; \
    struct { \
        const char *text; \
        size_t count; \
    } __obfh_strnlen_args = {__VA_ARGS__}; \
    OBFH_CUSTOM_NOP; \
    size_t __obfh_strnlen_length = 0; \
    BREAK_STACK_CFLOW; \
    OBFH_CUSTOM_NOP; \
__obfh_strnlen_loop: \
    OBFH_CUSTOM_EXIT(__obfh_strnlen_length == __obfh_strnlen_args.count || !__obfh_strnlen_args.text[__obfh_strnlen_length], __obfh_strnlen_done); \
    ++__obfh_strnlen_length; \
    goto __obfh_strnlen_loop; \
__obfh_strnlen_done:; \
    OBFH_CUSTOM_NOP; \
    __obfh_strnlen_length; \
})

#define wcsnlen(...) ({ \
    __label__ __obfh_wcsnlen_loop, __obfh_wcsnlen_done; \
    OBFH_CUSTOM_BEGIN; \
    OBFH_CUSTOM_NOP; \
    struct { \
        const wchar_t *text; \
        size_t count; \
    } __obfh_wcsnlen_args = {__VA_ARGS__}; \
    OBFH_CUSTOM_NOP; \
    size_t __obfh_wcsnlen_length = 0; \
    BREAK_STACK_CFLOW; \
    OBFH_CUSTOM_NOP; \
__obfh_wcsnlen_loop: \
    OBFH_CUSTOM_EXIT(__obfh_wcsnlen_length == __obfh_wcsnlen_args.count || !__obfh_wcsnlen_args.text[__obfh_wcsnlen_length], __obfh_wcsnlen_done); \
    ++__obfh_wcsnlen_length; \
    goto __obfh_wcsnlen_loop; \
__obfh_wcsnlen_done:; \
    OBFH_CUSTOM_NOP; \
    __obfh_wcsnlen_length; \
})

#define wmemcpy(...) ({ \
    OBFH_CUSTOM_BEGIN; \
    OBFH_CUSTOM_NOP; \
    struct { \
        wchar_t *destination; \
        const wchar_t *text; \
        size_t count; \
    } __obfh_wmemcpy_args = {__VA_ARGS__}; \
    OBFH_CUSTOM_NOP; \
    (wchar_t *)memcpy(__obfh_wmemcpy_args.destination, __obfh_wmemcpy_args.text, __obfh_wmemcpy_args.count * sizeof(wchar_t)); \
})

#define wmemmove(...) ({ \
    OBFH_CUSTOM_BEGIN; \
    OBFH_CUSTOM_NOP; \
    struct { \
        wchar_t *destination; \
        const wchar_t *text; \
        size_t count; \
    } __obfh_wmemmove_args = {__VA_ARGS__}; \
    OBFH_CUSTOM_NOP; \
    (wchar_t *)memmove(__obfh_wmemmove_args.destination, __obfh_wmemmove_args.text, __obfh_wmemmove_args.count * sizeof(wchar_t)); \
})

#define wmemset(...) ({ \
    OBFH_CUSTOM_BEGIN; \
    OBFH_CUSTOM_NOP; \
    struct { \
        wchar_t *destination; \
        wchar_t value; \
        size_t count; \
    } __obfh_wmemset_args = {__VA_ARGS__}; \
    OBFH_CUSTOM_NOP; \
    wchar_t *__obfh_wmemset_target = __obfh_wmemset_args.destination; \
    size_t __obfh_wmemset_count = __obfh_wmemset_args.count; \
    BREAK_STACK_CFLOW; \
    OBFH_CUSTOM_NOP; \
    __obfh_asm__("rep stosw" \
                 : "+D"(__obfh_wmemset_target), "+c"(__obfh_wmemset_count) \
                 : "a"(__obfh_wmemset_args.value) \
                 : "memory"); \
    __obfh_wmemset_args.destination; \
})

#define wmemcmp(...) ({ \
    __label__ __obfh_wmemcmp_loop, __obfh_wmemcmp_done; \
    OBFH_CUSTOM_BEGIN; \
    OBFH_CUSTOM_NOP; \
    struct { \
        const wchar_t *left; \
        const wchar_t *right; \
        size_t count; \
    } __obfh_wmemcmp_args = {__VA_ARGS__}; \
    OBFH_CUSTOM_NOP; \
    int __obfh_wmemcmp_result = 0; \
    BREAK_STACK_CFLOW; \
    OBFH_CUSTOM_NOP; \
__obfh_wmemcmp_loop: \
    OBFH_CUSTOM_EXIT(!__obfh_wmemcmp_args.count, __obfh_wmemcmp_done); \
    __obfh_wmemcmp_result = (*__obfh_wmemcmp_args.left > *__obfh_wmemcmp_args.right) - (*__obfh_wmemcmp_args.left < *__obfh_wmemcmp_args.right); \
    OBFH_CUSTOM_EXIT(__obfh_wmemcmp_result, __obfh_wmemcmp_done); \
    OBFH_CUSTOM_PAIR(++__obfh_wmemcmp_args.left, ++__obfh_wmemcmp_args.right); \
    --__obfh_wmemcmp_args.count; \
    goto __obfh_wmemcmp_loop; \
__obfh_wmemcmp_done:; \
    OBFH_CUSTOM_NOP; \
    __obfh_wmemcmp_result; \
})

#define wmemchr(...) ({ \
    __label__ __obfh_wmemchr_loop, __obfh_wmemchr_found, __obfh_wmemchr_done; \
    OBFH_CUSTOM_BEGIN; \
    OBFH_CUSTOM_NOP; \
    struct { \
        const wchar_t *text; \
        wchar_t value; \
        size_t count; \
    } __obfh_wmemchr_args = {__VA_ARGS__}; \
    OBFH_CUSTOM_NOP; \
    wchar_t *__obfh_wmemchr_result = NULL; \
    BREAK_STACK_CFLOW; \
    OBFH_CUSTOM_NOP; \
__obfh_wmemchr_loop: \
    OBFH_CUSTOM_EXIT(!__obfh_wmemchr_args.count, __obfh_wmemchr_done); \
    OBFH_CUSTOM_EXIT(*__obfh_wmemchr_args.text == __obfh_wmemchr_args.value, __obfh_wmemchr_found); \
    OBFH_CUSTOM_PAIR(++__obfh_wmemchr_args.text, --__obfh_wmemchr_args.count); \
    goto __obfh_wmemchr_loop; \
__obfh_wmemchr_found: \
    __obfh_wmemchr_result = (wchar_t *)__obfh_wmemchr_args.text; \
__obfh_wmemchr_done:; \
    OBFH_CUSTOM_NOP; \
    __obfh_wmemchr_result; \
})

#define bsearch(...) ({ \
    __label__ __obfh_bsearch_loop, __obfh_bsearch_found, __obfh_bsearch_done; \
    OBFH_CUSTOM_BEGIN; \
    OBFH_CUSTOM_NOP; \
    struct { \
        const void *key; \
        const void *base; \
        size_t count; \
        size_t width; \
        int(__cdecl * compare)(const void *, const void *); \
    } __obfh_bsearch_args = {__VA_ARGS__}; \
    OBFH_CUSTOM_NOP; \
    const unsigned char *__obfh_bsearch_base = (const unsigned char *)__obfh_bsearch_args.base; \
    const unsigned char *__obfh_bsearch_middle; \
    size_t __obfh_bsearch_half; \
    int __obfh_bsearch_order; \
    void *__obfh_bsearch_result = NULL; \
    BREAK_STACK_CFLOW; \
    OBFH_CUSTOM_NOP; \
__obfh_bsearch_loop: \
    OBFH_CUSTOM_EXIT(!__obfh_bsearch_args.count, __obfh_bsearch_done); \
    __obfh_bsearch_half = __obfh_bsearch_args.count / 2; \
    __obfh_bsearch_middle = __obfh_bsearch_base + __obfh_bsearch_half * __obfh_bsearch_args.width; \
    __obfh_bsearch_order = __obfh_bsearch_args.compare(__obfh_bsearch_args.key, __obfh_bsearch_middle); \
    OBFH_CUSTOM_EXIT(!__obfh_bsearch_order, __obfh_bsearch_found); \
    __obfh_bsearch_base = __obfh_bsearch_order < 0 ? __obfh_bsearch_base : __obfh_bsearch_middle + __obfh_bsearch_args.width; \
    __obfh_bsearch_args.count = __obfh_bsearch_order < 0 ? __obfh_bsearch_half : __obfh_bsearch_args.count - __obfh_bsearch_half - 1; \
    goto __obfh_bsearch_loop; \
__obfh_bsearch_found: \
    __obfh_bsearch_result = (void *)__obfh_bsearch_middle; \
__obfh_bsearch_done:; \
    OBFH_CUSTOM_NOP; \
    __obfh_bsearch_result; \
})

// In-place heapsort: bounded O(n log n) comparisons, no allocation or CRT state.
#define qsort(...) ({ \
    __label__ __obfh_sort_next, __obfh_sort_sift, __obfh_sort_child, __obfh_sort_swap, __obfh_sort_bytes, __obfh_sort_swapped, __obfh_sort_done; \
    OBFH_CUSTOM_BEGIN; \
    OBFH_CUSTOM_NOP; \
    struct { \
        void *base; \
        size_t count; \
        size_t width; \
        int(__cdecl * compare)(const void *, const void *); \
    } __obfh_sort_args = {__VA_ARGS__}; \
    OBFH_CUSTOM_NOP; \
    unsigned char *__obfh_sort_base = (unsigned char *)__obfh_sort_args.base; \
    unsigned char *__obfh_sort_a, *__obfh_sort_b, __obfh_sort_byte; \
    size_t __obfh_sort_start = __obfh_sort_args.count / 2, __obfh_sort_end = __obfh_sort_args.count; \
    size_t __obfh_sort_root, __obfh_sort_child_index, __obfh_sort_i; \
    int __obfh_sort_extract; \
    BREAK_STACK_CFLOW; \
    OBFH_CUSTOM_NOP; \
    OBFH_CUSTOM_EXIT(__obfh_sort_end < 2 || !__obfh_sort_args.width, __obfh_sort_done); \
__obfh_sort_next: \
    OBFH_CUSTOM_EXIT(!__obfh_sort_start, __obfh_sort_child); \
    __obfh_sort_root = --__obfh_sort_start; \
    goto __obfh_sort_sift; \
__obfh_sort_child: \
    OBFH_CUSTOM_EXIT(__obfh_sort_end < 2, __obfh_sort_done); \
    --__obfh_sort_end; \
    __obfh_sort_root = 0; \
    __obfh_sort_child_index = __obfh_sort_end; \
    __obfh_sort_extract = 1; \
    goto __obfh_sort_swap; \
__obfh_sort_sift: \
    OBFH_CUSTOM_EXIT(__obfh_sort_root >= __obfh_sort_end / 2, __obfh_sort_next); \
    __obfh_sort_child_index = __obfh_sort_root * 2 + 1; \
    OBFH_CUSTOM_EXIT(__obfh_sort_child_index + 1 >= __obfh_sort_end, __obfh_sort_swapped); \
    __obfh_sort_child_index += __obfh_sort_args.compare( \
                                   __obfh_sort_base + __obfh_sort_child_index * __obfh_sort_args.width, \
                                   __obfh_sort_base + (__obfh_sort_child_index + 1) * __obfh_sort_args.width) < 0; \
__obfh_sort_swapped: \
    OBFH_CUSTOM_EXIT(__obfh_sort_args.compare( \
                         __obfh_sort_base + __obfh_sort_root * __obfh_sort_args.width, \
                         __obfh_sort_base + __obfh_sort_child_index * __obfh_sort_args.width) >= 0, \
                     __obfh_sort_next); \
    __obfh_sort_extract = 0; \
__obfh_sort_swap: \
    __obfh_sort_a = __obfh_sort_base + __obfh_sort_root * __obfh_sort_args.width; \
    __obfh_sort_b = __obfh_sort_base + __obfh_sort_child_index * __obfh_sort_args.width; \
    __obfh_sort_i = 0; \
__obfh_sort_bytes: \
    __obfh_sort_byte = __obfh_sort_a[__obfh_sort_i]; \
    __obfh_sort_a[__obfh_sort_i] = __obfh_sort_b[__obfh_sort_i]; \
    __obfh_sort_b[__obfh_sort_i] = __obfh_sort_byte; \
    ++__obfh_sort_i; \
    OBFH_CUSTOM_EXIT(__obfh_sort_i < __obfh_sort_args.width, __obfh_sort_bytes); \
    __obfh_sort_root = __obfh_sort_extract ? 0 : __obfh_sort_child_index; \
    goto __obfh_sort_sift; \
__obfh_sort_done:; \
    OBFH_CUSTOM_NOP; \
    (void)0; \
})

#define strcmp(...) ({ \
    __label__ __obfh_strcmp_loop, __obfh_strcmp_advance, __obfh_strcmp_equal, __obfh_strcmp_done; \
    OBFH_CUSTOM_BEGIN; \
    OBFH_CUSTOM_NOP; \
    struct { \
        const char *__obfh_strcmp_str1; \
        const char *__obfh_strcmp_str2; \
    } __obfh_strcmp_args = {__VA_ARGS__}; \
    OBFH_CUSTOM_NOP; \
    int __obfh_strcmp_result; \
    int __obfh_strcmp_a; \
    int __obfh_strcmp_b; \
    BREAK_STACK_CFLOW; \
    OBFH_CUSTOM_NOP; \
__obfh_strcmp_loop: \
    __obfh_strcmp_a = (unsigned char)*__obfh_strcmp_args.__obfh_strcmp_str1; \
    __obfh_strcmp_b = (unsigned char)*__obfh_strcmp_args.__obfh_strcmp_str2; \
    OBFH_CUSTOM_EXIT(!(__obfh_strcmp_a || __obfh_strcmp_b), __obfh_strcmp_equal); \
    __obfh_strcmp_a = obfh_int_proxy(__obfh_strcmp_a); \
    __obfh_strcmp_b = obfh_int_proxy(__obfh_strcmp_b); \
    OBFH_CUSTOM_EXIT(__obfh_strcmp_a == __obfh_strcmp_b, __obfh_strcmp_advance); \
    __obfh_strcmp_result = obfh_int_proxy((__obfh_strcmp_a > __obfh_strcmp_b) - (__obfh_strcmp_a < __obfh_strcmp_b)); \
    goto __obfh_strcmp_done; \
__obfh_strcmp_advance: \
    __obfh_strcmp_args.__obfh_strcmp_str1 += obfh_int_proxy(_1); \
    __obfh_strcmp_args.__obfh_strcmp_str2 += obfh_int_proxy(_2 - _1); \
    goto __obfh_strcmp_loop; \
__obfh_strcmp_equal: \
    __obfh_strcmp_result = _0; \
__obfh_strcmp_done:; \
    OBFH_CUSTOM_NOP; \
    __obfh_strcmp_result; \
})

#define strlen(...) ({ \
    __label__ __obfh_strlen_loop, __obfh_strlen_done; \
    OBFH_CUSTOM_BEGIN; \
    OBFH_CUSTOM_NOP; \
    struct { \
        const char *str; \
    } __obfh_strlen_args = {__VA_ARGS__}; \
    OBFH_CUSTOM_NOP; \
    int __obfh_strlen_step; \
    size_t __obfh_strlen_length = _0; \
    BREAK_STACK_CFLOW; \
    OBFH_CUSTOM_NOP; \
__obfh_strlen_loop: \
    OBFH_CUSTOM_EXIT(!*__obfh_strlen_args.str, __obfh_strlen_done); \
    __obfh_strlen_step = obfh_int_proxy(_1); \
    __obfh_strlen_length += __obfh_strlen_step; \
    __obfh_strlen_args.str += __obfh_strlen_step; \
    goto __obfh_strlen_loop; \
__obfh_strlen_done: \
    OBFH_CUSTOM_NOP; \
    (size_t) obfh_uintptr_proxy(__obfh_strlen_length + (RND(0, 1000) * _0)); \
})

// The legacy Windows CRT accepts these 18 BMP decimal digit blocks.
// Only non-ASCII digit candidates need the table; letter digits remain ASCII.
#define OBFH_WIDE_DIGIT(code) ({ \
    __label__ __obfh_wdigit_loop, __obfh_wdigit_done; \
    OBFH_CUSTOM_BEGIN; \
    static const unsigned short __obfh_wdigit_zeros[] = {0x30, 0x660, 0x6f0, 0x966, 0x9e6, 0xa66, 0xae6, 0xb66, 0xc66, 0xce6, 0xd66, 0xe50, 0xed0, 0xf20, 0x1040, 0x17e0, 0x1810, 0xff10}; \
    unsigned int __obfh_wdigit_code = (code), __obfh_wdigit_index = 0, __obfh_wdigit_value = 36; \
__obfh_wdigit_loop: \
    __obfh_wdigit_value = __obfh_wdigit_code - __obfh_wdigit_zeros[__obfh_wdigit_index]; \
    OBFH_CUSTOM_EXIT(__obfh_wdigit_value < 10, __obfh_wdigit_done); \
    ++__obfh_wdigit_index; \
    OBFH_CUSTOM_EXIT(__obfh_wdigit_index < 18, __obfh_wdigit_loop); \
    __obfh_wdigit_value = 36; \
__obfh_wdigit_done:; \
    __obfh_wdigit_value; \
})
#define OBFH_PARSE_ZERO_0(code) ((code) == '0')
#define OBFH_PARSE_ZERO_1(code) (OBFH_WIDE_DIGIT(code) == 0)
#define OBFH_PARSE_DIGIT_0(code, mapped) (mapped)
#define OBFH_PARSE_DIGIT_1(code, mapped) ((mapped) == 36 ? OBFH_WIDE_DIGIT(code) : (mapped))

// Integer conversion uses native locale classification and Windows CRT limits.
#define OBFH_INTEGER_PARSE_CORE(input, ending, radix, mode, unsigned_type, signed_max, unsigned_max, preserve_errno, char_type, code_type, space_fn, wide) \
    ({ \
        __label__ __obfh_parse_space, __obfh_parse_sign, __obfh_parse_base, __obfh_parse_loop, __obfh_parse_finish, __obfh_parse_end; \
        OBFH_CUSTOM_BEGIN; \
        OBFH_CUSTOM_NOP; \
        struct { \
            const char_type *text; \
            char_type **end; \
            int base; \
        } __obfh_parse_args = {(input), (ending), (radix)}; \
        OBFH_CUSTOM_NOP; \
        const code_type *__obfh_parse_p = (const code_type *)__obfh_parse_args.text; \
        unsigned_type __obfh_parse_value = 0, __obfh_parse_limit, __obfh_parse_cutoff, __obfh_parse_tail; \
        enum { __obfh_parse_form = RND(0, 1) }; \
        unsigned int __obfh_parse_digit, __obfh_parse_negative = 0, __obfh_parse_any = 0, __obfh_parse_overflow = 0; \
        int *__obfh_parse_errno = _errno(); \
        *__obfh_parse_errno = (preserve_errno) || ((mode) == 2 && !__obfh_parse_args.text) ? *__obfh_parse_errno : 0; \
        OBFH_CUSTOM_EXIT(!__obfh_parse_args.text || \
                             (__obfh_parse_args.base && (__obfh_parse_args.base < 2 || __obfh_parse_args.base > 36)), \
                         __obfh_parse_end); \
    __obfh_parse_space: \
        OBFH_CUSTOM_EXIT(!(space_fn)(*__obfh_parse_p), __obfh_parse_sign); \
        ++__obfh_parse_p; \
        goto __obfh_parse_space; \
    __obfh_parse_sign: \
        __obfh_parse_negative = *__obfh_parse_p == '-'; \
        __obfh_parse_p += (*__obfh_parse_p == '+' || *__obfh_parse_p == '-'); \
        OBFH_CUSTOM_EXIT((__obfh_parse_args.base && __obfh_parse_args.base != 16) || \
                             !OBFH_PARSE_ZERO_##wide(__obfh_parse_p[0]) || \
                             (__obfh_parse_p[1] != 'x' && __obfh_parse_p[1] != 'X'), \
                         __obfh_parse_base); \
        __obfh_parse_args.base = 16; \
        __obfh_parse_p += 2; \
    __obfh_parse_base: \
        __obfh_parse_args.base = __obfh_parse_args.base ? __obfh_parse_args.base : (OBFH_PARSE_ZERO_##wide(*__obfh_parse_p) ? 8 : 10); \
        __obfh_parse_limit = (mode) == 0 || (mode) == 3 ? (unsigned_type)(signed_max) + __obfh_parse_negative : (unsigned_type)(unsigned_max); \
        __obfh_parse_cutoff = (mode) == 2 ? 0 : __obfh_parse_limit / (unsigned int)__obfh_parse_args.base; \
        __obfh_parse_tail = (mode) == 2 ? 0 : __obfh_parse_limit % (unsigned int)__obfh_parse_args.base; \
        OBFH_CUSTOM_NOP; \
    __obfh_parse_loop: \
        __obfh_parse_digit = *__obfh_parse_p; \
        __obfh_parse_digit = __builtin_choose_expr( \
            __obfh_parse_form, \
            __obfh_parse_digit >= '0' && __obfh_parse_digit <= '9'   ? __obfh_parse_digit - '0' \
            : __obfh_parse_digit >= 'A' && __obfh_parse_digit <= 'Z' ? __obfh_parse_digit - 'A' + 10u \
            : __obfh_parse_digit >= 'a' && __obfh_parse_digit <= 'z' ? __obfh_parse_digit - 'a' + 10u \
                                                                     : 36u, \
            (__obfh_parse_digit - (unsigned int)'0' < 10u \
                 ? __obfh_parse_digit - '0' \
                 : ((__obfh_parse_digit | 32u) - (unsigned int)'a' < 26u ? (__obfh_parse_digit | 32u) - 'a' + 10u : 36u))); \
        __obfh_parse_digit = OBFH_PARSE_DIGIT_##wide(*__obfh_parse_p, __obfh_parse_digit); \
        OBFH_CUSTOM_EXIT(__obfh_parse_digit >= (unsigned int)__obfh_parse_args.base, __obfh_parse_finish); \
        __obfh_parse_any = 1; \
        __obfh_parse_overflow |= __obfh_parse_value > __obfh_parse_cutoff || \
                                 (__obfh_parse_value == __obfh_parse_cutoff && __obfh_parse_digit > __obfh_parse_tail); \
        __obfh_parse_value = (mode) == 2 || !__obfh_parse_overflow \
                                 ? __obfh_parse_value * (unsigned int)__obfh_parse_args.base + __obfh_parse_digit \
                                 : __obfh_parse_limit; \
        ++__obfh_parse_p; \
        goto __obfh_parse_loop; \
    __obfh_parse_finish: \
        OBFH_CUSTOM_EXIT(!__obfh_parse_any, __obfh_parse_end); \
        *__obfh_parse_errno = (mode) != 2 && __obfh_parse_overflow ? ERANGE : ((preserve_errno) ? *__obfh_parse_errno : 0); \
        __obfh_parse_value = __obfh_parse_negative ? 0ul - __obfh_parse_value : __obfh_parse_value; \
        goto __obfh_parse_end; \
    __obfh_parse_end: \
        *__obfh_parse_errno = (!__obfh_parse_args.text && (mode) != 2 && (mode) != 3) || \
                                      (__obfh_parse_args.base && (__obfh_parse_args.base < 2 || __obfh_parse_args.base > 36)) \
                                  ? EINVAL \
                                  : *__obfh_parse_errno; \
        __obfh_parse_args.end \
            ? (void)(*__obfh_parse_args.end = (char_type *)(__obfh_parse_any ? __obfh_parse_p : (const code_type *)__obfh_parse_args.text)) \
            : (void)0; \
        __obfh_parse_value; \
    })
#define OBFH_INTEGER_PARSE_TYPED(input, ending, radix, mode, unsigned_type, signed_max, unsigned_max, preserve_errno) \
    OBFH_INTEGER_PARSE_CORE(input, ending, radix, mode, unsigned_type, signed_max, unsigned_max, preserve_errno, char, unsigned char, isspace, 0)
#define OBFH_INTEGER_PARSE(input, ending, radix, mode) \
    OBFH_INTEGER_PARSE_TYPED(input, ending, radix, mode, unsigned long, LONG_MAX, ULONG_MAX, 0)
#define strtol(...) ((long)OBFH_INTEGER_PARSE(__VA_ARGS__, 0))

#define strtoul(...) OBFH_INTEGER_PARSE(__VA_ARGS__, 1)

#define atoi(text) ((int)OBFH_INTEGER_PARSE(text, NULL, 10, 2))

#define atol(text) ((long)OBFH_INTEGER_PARSE(text, NULL, 10, 2))

// Windows 64-bit conversions preserve errno on successful/no-digit conversion.
#define OBFH_INTEGER_PARSE64_ARGS(mode, text, ending, radix) \
    OBFH_INTEGER_PARSE_TYPED(text, ending, radix, mode, unsigned long long, LLONG_MAX, ULLONG_MAX, 1)
#undef strtoll
#undef strtoull
#define strtoll(...) ((long long)OBFH_INTEGER_PARSE64_ARGS(0, __VA_ARGS__))
#define strtoull(...) OBFH_INTEGER_PARSE64_ARGS(1, __VA_ARGS__)
#define atoll(text) ((long long)OBFH_INTEGER_PARSE_TYPED(text, NULL, 10, 3, unsigned long long, LLONG_MAX, ULLONG_MAX, 1))
#define _strtoi64(...) strtoll(__VA_ARGS__)
#define _strtoui64(...) strtoull(__VA_ARGS__)
#define _atoi64(...) atoll(__VA_ARGS__)

// Only byte case mapping stays native: replacing it with ASCII rules loses LC_CTYPE.
#define OBFH_CASE_COMPARE(text1, text2, limit, bounded) ({ \
    __label__ __obfh_case_loop, __obfh_case_done; \
    OBFH_CUSTOM_BEGIN; \
    OBFH_CUSTOM_NOP; \
    struct { \
        const char *a; \
        const char *b; \
        size_t count; \
    } __obfh_case_args = {(text1), (text2), (limit)}; \
    OBFH_CUSTOM_NOP; \
    int __obfh_case_a, __obfh_case_b, __obfh_case_result = 0; \
    BREAK_STACK_CFLOW; \
    OBFH_CUSTOM_NOP; \
__obfh_case_loop: \
    OBFH_CUSTOM_EXIT((bounded) && !__obfh_case_args.count, __obfh_case_done); \
    __obfh_case_a = (tolower)((unsigned char)*__obfh_case_args.a); \
    __obfh_case_b = (tolower)((unsigned char)*__obfh_case_args.b); \
    __obfh_case_result = __obfh_case_a - __obfh_case_b; \
    OBFH_CUSTOM_EXIT(__obfh_case_result || !*__obfh_case_args.a, __obfh_case_done); \
    OBFH_CUSTOM_PAIR(++__obfh_case_args.a, ++__obfh_case_args.b); \
    __obfh_case_args.count -= (bounded); \
    goto __obfh_case_loop; \
__obfh_case_done:; \
    OBFH_CUSTOM_NOP; \
    obfh_int_proxy(__obfh_case_result); \
})
#define stricmp(a, b) OBFH_CASE_COMPARE(a, b, 0, 0)
#define strnicmp(a, b, count) OBFH_CASE_COMPARE(a, b, count, 1)
#define _stricmp(...) stricmp(__VA_ARGS__)
#define _strnicmp(...) strnicmp(__VA_ARGS__)

#define OBFH_WIDE_PARSE(mode, unsigned_type, signed_max, unsigned_max, preserve, text, ending, radix) \
    OBFH_INTEGER_PARSE_CORE(text, ending, radix, mode, unsigned_type, signed_max, unsigned_max, preserve, wchar_t, wchar_t, iswspace, 1)
#define wcstol(...) ((long)OBFH_WIDE_PARSE(0, unsigned long, LONG_MAX, ULONG_MAX, 0, __VA_ARGS__))
#define wcstoul(...) OBFH_WIDE_PARSE(1, unsigned long, LONG_MAX, ULONG_MAX, 0, __VA_ARGS__)
#define _wcstoi64(...) ((long long)OBFH_WIDE_PARSE(0, unsigned long long, LLONG_MAX, ULLONG_MAX, 1, __VA_ARGS__))
#define _wcstoui64(...) OBFH_WIDE_PARSE(1, unsigned long long, LLONG_MAX, ULLONG_MAX, 1, __VA_ARGS__)

#define OBFH_WIDE_CASE_COMPARE(text1, text2, limit, bounded) ({ \
    __label__ __obfh_wcase_loop, __obfh_wcase_done; \
    OBFH_CUSTOM_BEGIN; \
    OBFH_CUSTOM_NOP; \
    struct { \
        const wchar_t *a; \
        const wchar_t *b; \
        size_t count; \
    } __obfh_wcase_args = {(text1), (text2), (limit)}; \
    OBFH_CUSTOM_NOP; \
    int __obfh_wcase_a, __obfh_wcase_b, __obfh_wcase_result = 0; \
    BREAK_STACK_CFLOW; \
    OBFH_CUSTOM_NOP; \
__obfh_wcase_loop: \
    OBFH_CUSTOM_EXIT((bounded) && !__obfh_wcase_args.count, __obfh_wcase_done); \
    __obfh_wcase_a = (towlower)(*__obfh_wcase_args.a); \
    __obfh_wcase_b = (towlower)(*__obfh_wcase_args.b); \
    __obfh_wcase_result = __obfh_wcase_a - __obfh_wcase_b; \
    OBFH_CUSTOM_EXIT(__obfh_wcase_result || !*__obfh_wcase_args.a, __obfh_wcase_done); \
    OBFH_CUSTOM_PAIR(++__obfh_wcase_args.a, ++__obfh_wcase_args.b); \
    __obfh_wcase_args.count -= (bounded); \
    goto __obfh_wcase_loop; \
__obfh_wcase_done:; \
    OBFH_CUSTOM_NOP; \
    obfh_int_proxy(__obfh_wcase_result); \
})
#define _wcsicmp(a, b) OBFH_WIDE_CASE_COMPARE(a, b, 0, 0)
#define _wcsnicmp(a, b, count) OBFH_WIDE_CASE_COMPARE(a, b, count, 1)

#define _memicmp(...) ({ \
    __label__ __obfh_mcase_loop, __obfh_mcase_done; \
    OBFH_CUSTOM_BEGIN; \
    OBFH_CUSTOM_NOP; \
    struct { \
        const void *a; \
        const void *b; \
        size_t count; \
    } __obfh_mcase_args = {__VA_ARGS__}; \
    OBFH_CUSTOM_NOP; \
    const unsigned char *__obfh_mcase_a = __obfh_mcase_args.a, *__obfh_mcase_b = __obfh_mcase_args.b; \
    int __obfh_mcase_result = 0; \
    BREAK_STACK_CFLOW; \
    OBFH_CUSTOM_NOP; \
__obfh_mcase_loop: \
    OBFH_CUSTOM_EXIT(!__obfh_mcase_args.count, __obfh_mcase_done); \
    __obfh_mcase_result = (tolower)(*__obfh_mcase_a) - (tolower)(*__obfh_mcase_b); \
    OBFH_CUSTOM_EXIT(__obfh_mcase_result, __obfh_mcase_done); \
    OBFH_CUSTOM_PAIR(++__obfh_mcase_a, ++__obfh_mcase_b); \
    --__obfh_mcase_args.count; \
    goto __obfh_mcase_loop; \
__obfh_mcase_done:; \
    OBFH_CUSTOM_NOP; \
    obfh_int_proxy(__obfh_mcase_result); \
})

#define OBFH_STRING_CASE(input, char_type, code_type, convert) ({ \
    __label__ __obfh_scase_loop, __obfh_scase_done; \
    OBFH_CUSTOM_BEGIN; \
    OBFH_CUSTOM_NOP; \
    char_type *__obfh_scase_base = (input), *__obfh_scase_p = __obfh_scase_base; \
    BREAK_STACK_CFLOW; \
    OBFH_CUSTOM_NOP; \
__obfh_scase_loop: \
    OBFH_CUSTOM_EXIT(!*__obfh_scase_p, __obfh_scase_done); \
    *__obfh_scase_p = (char_type)(convert)((code_type)*__obfh_scase_p); \
    ++__obfh_scase_p; \
    goto __obfh_scase_loop; \
__obfh_scase_done:; \
    OBFH_CUSTOM_NOP; \
    (char_type *)obfh_uintptr_proxy((uintptr_t)__obfh_scase_base); \
})
// DBCS case mapping is a whole-string NLS operation: byte-at-a-time tolower
// corrupts trail bytes and changes invalid sequences that CRT leaves untouched.
#define OBFH_DBCS_CASE(input, map_flags) ({ \
    __label__ __obfh_dbcs_invalid, __obfh_dbcs_free, __obfh_dbcs_done; \
    OBFH_CUSTOM_BEGIN; \
    OBFH_CUSTOM_NOP; \
    char *__obfh_dbcs_text = (input); \
    size_t __obfh_dbcs_bytes = strlen(__obfh_dbcs_text) + 1; \
    unsigned int __obfh_dbcs_cp = OBFH_CRT_TARGET(unsigned int(__cdecl *)(void), HIDE_STRING("___lc_codepage_func"))(); \
    unsigned long *__obfh_dbcs_handles = OBFH_CRT_TARGET(unsigned long *(__cdecl *)(void), HIDE_STRING("___lc_handle_func"))(); \
    wchar_t *__obfh_dbcs_storage = NULL, *__obfh_dbcs_mapped; \
    char *__obfh_dbcs_output; \
    int __obfh_dbcs_count, __obfh_dbcs_written; \
    OBFH_CUSTOM_EXIT(__obfh_dbcs_bytes > INT_MAX || __obfh_dbcs_bytes > SIZE_MAX / 5, __obfh_dbcs_invalid); \
    __obfh_dbcs_storage = malloc(__obfh_dbcs_bytes * 5); \
    OBFH_CUSTOM_EXIT(!__obfh_dbcs_storage, __obfh_dbcs_done); \
    __obfh_dbcs_mapped = __obfh_dbcs_storage + __obfh_dbcs_bytes; \
    __obfh_dbcs_output = (char *)(__obfh_dbcs_mapped + __obfh_dbcs_bytes); \
    __obfh_dbcs_count = (MultiByteToWideChar)(__obfh_dbcs_cp, MB_ERR_INVALID_CHARS, __obfh_dbcs_text, -1, __obfh_dbcs_storage, (int)__obfh_dbcs_bytes); \
    OBFH_CUSTOM_EXIT(!__obfh_dbcs_count, __obfh_dbcs_invalid); \
    __obfh_dbcs_written = (LCMapStringW)(__obfh_dbcs_handles[LC_CTYPE], map_flags, __obfh_dbcs_storage, __obfh_dbcs_count, __obfh_dbcs_mapped, (int)__obfh_dbcs_bytes); \
    OBFH_CUSTOM_EXIT(!__obfh_dbcs_written, __obfh_dbcs_invalid); \
    __obfh_dbcs_written = (WideCharToMultiByte)(__obfh_dbcs_cp, 0, __obfh_dbcs_mapped, __obfh_dbcs_written, __obfh_dbcs_output, (int)__obfh_dbcs_bytes, NULL, NULL); \
    OBFH_CUSTOM_EXIT(!__obfh_dbcs_written, __obfh_dbcs_invalid); \
    memcpy(__obfh_dbcs_text, __obfh_dbcs_output, (size_t)__obfh_dbcs_written); \
    goto __obfh_dbcs_free; \
__obfh_dbcs_invalid: \
    *_errno() = EILSEQ; \
__obfh_dbcs_free: \
    free(__obfh_dbcs_storage); \
__obfh_dbcs_done:; \
    OBFH_CUSTOM_NOP; \
    __obfh_dbcs_text; \
})
#define OBFH_NARROW_CASE(input, convert, flags) ({ \
    __label__ __obfh_ncase_simple, __obfh_ncase_done; \
    OBFH_CUSTOM_BEGIN; \
    OBFH_CUSTOM_NOP; \
    char *__obfh_ncase_text = (input); \
    BREAK_STACK_CFLOW; \
    OBFH_CUSTOM_NOP; \
    OBFH_CUSTOM_EXIT(OBFH_CRT_TARGET(int(__cdecl *)(void), HIDE_STRING("___mb_cur_max_func"))() == 1, __obfh_ncase_simple); \
    OBFH_DBCS_CASE(__obfh_ncase_text, flags); \
    goto __obfh_ncase_done; \
__obfh_ncase_simple: \
    OBFH_STRING_CASE(__obfh_ncase_text, char, unsigned char, convert); \
__obfh_ncase_done:; \
    OBFH_CUSTOM_NOP; \
    (char *)obfh_uintptr_proxy((uintptr_t)__obfh_ncase_text); \
})
#define _strlwr(text) OBFH_NARROW_CASE(text, tolower, LCMAP_LOWERCASE)
#define _strupr(text) OBFH_NARROW_CASE(text, toupper, LCMAP_UPPERCASE)
#define OBFH_WIDE_STRING_CASE(input, convert, flags) ({ \
    __label__ __obfh_wcmap_simple, __obfh_wcmap_invalid, __obfh_wcmap_free, __obfh_wcmap_done; \
    OBFH_CUSTOM_BEGIN; \
    OBFH_CUSTOM_NOP; \
    wchar_t *__obfh_wcmap_text = (input), *__obfh_wcmap_scratch = NULL; \
    unsigned long *__obfh_wcmap_handles = OBFH_CRT_TARGET(unsigned long *(__cdecl *)(void), HIDE_STRING("___lc_handle_func"))(); \
    size_t __obfh_wcmap_count; \
    int __obfh_wcmap_written; \
    BREAK_STACK_CFLOW; \
    OBFH_CUSTOM_NOP; \
    OBFH_CUSTOM_EXIT(!__obfh_wcmap_handles[LC_CTYPE], __obfh_wcmap_simple); \
    __obfh_wcmap_count = wcslen(__obfh_wcmap_text) + 1; \
    OBFH_CUSTOM_EXIT(__obfh_wcmap_count > INT_MAX || __obfh_wcmap_count > SIZE_MAX / sizeof(wchar_t), __obfh_wcmap_invalid); \
    __obfh_wcmap_scratch = malloc(__obfh_wcmap_count * sizeof(wchar_t)); \
    OBFH_CUSTOM_EXIT(!__obfh_wcmap_scratch, __obfh_wcmap_done); \
    __obfh_wcmap_written = (LCMapStringW)(__obfh_wcmap_handles[LC_CTYPE], flags, __obfh_wcmap_text, (int)__obfh_wcmap_count, __obfh_wcmap_scratch, (int)__obfh_wcmap_count); \
    OBFH_CUSTOM_EXIT(!__obfh_wcmap_written, __obfh_wcmap_invalid); \
    memcpy(__obfh_wcmap_text, __obfh_wcmap_scratch, (size_t)__obfh_wcmap_written * sizeof(wchar_t)); \
    goto __obfh_wcmap_free; \
__obfh_wcmap_invalid: \
    *_errno() = EILSEQ; \
__obfh_wcmap_free: \
    free(__obfh_wcmap_scratch); \
    goto __obfh_wcmap_done; \
__obfh_wcmap_simple: \
    OBFH_STRING_CASE(__obfh_wcmap_text, wchar_t, wchar_t, convert); \
__obfh_wcmap_done:; \
    OBFH_CUSTOM_NOP; \
    (wchar_t *)obfh_uintptr_proxy((uintptr_t)__obfh_wcmap_text); \
})
#define _wcslwr(text) OBFH_WIDE_STRING_CASE(text, towlower, LCMAP_LOWERCASE)
#define _wcsupr(text) OBFH_WIDE_STRING_CASE(text, towupper, LCMAP_UPPERCASE)

#define OBFH_STRING_SET(input_text, input_value, limit, bounded, char_type, value_type) ({ \
    __label__ __obfh_sset_loop, __obfh_sset_done; \
    OBFH_CUSTOM_BEGIN; \
    OBFH_CUSTOM_NOP; \
    struct { \
        char_type *text; \
        value_type value; \
        size_t count; \
    } __obfh_sset_args = {(input_text), (input_value), (limit)}; \
    OBFH_CUSTOM_NOP; \
    char_type *__obfh_sset_p = __obfh_sset_args.text; \
    BREAK_STACK_CFLOW; \
    OBFH_CUSTOM_NOP; \
__obfh_sset_loop: \
    OBFH_CUSTOM_EXIT(((bounded) && !__obfh_sset_args.count) || !*__obfh_sset_p, __obfh_sset_done); \
    *__obfh_sset_p++ = (char_type)__obfh_sset_args.value; \
    __obfh_sset_args.count -= (bounded); \
    goto __obfh_sset_loop; \
__obfh_sset_done:; \
    OBFH_CUSTOM_NOP; \
    (char_type *)obfh_uintptr_proxy((uintptr_t)__obfh_sset_args.text); \
})
#define _strset(text, value) OBFH_STRING_SET(text, value, 0, 0, char, int)
#define _strnset(text, value, count) OBFH_STRING_SET(text, value, count, 1, char, int)
#define _wcsset(text, value) OBFH_STRING_SET(text, value, 0, 0, wchar_t, wchar_t)
#define _wcsnset(text, value, count) OBFH_STRING_SET(text, value, count, 1, wchar_t, wchar_t)

#define OBFH_INTEGER_ABS(input, signed_type, unsigned_type) ({ \
    OBFH_CUSTOM_BEGIN; \
    OBFH_CUSTOM_NOP; \
    signed_type __obfh_abs_value = (input); \
    BREAK_STACK_CFLOW; \
    OBFH_CUSTOM_NOP; \
    (signed_type) OBFH_CUSTOM_SELECT(__obfh_abs_value < 0, (unsigned_type)0 - (unsigned_type)__obfh_abs_value, (unsigned_type)__obfh_abs_value); \
})
#define labs(value) OBFH_INTEGER_ABS(value, long, unsigned long)
#define llabs(value) OBFH_INTEGER_ABS(value, long long, unsigned long long)
#define _abs64(value) OBFH_INTEGER_ABS(value, long long, unsigned long long)
#define OBFH_INTEGER_DIV(numerator, denominator, scalar_type, result_type) ({ \
    OBFH_CUSTOM_BEGIN; \
    OBFH_CUSTOM_NOP; \
    struct { \
        scalar_type n; \
        scalar_type d; \
    } __obfh_div_args = {(numerator), (denominator)}; \
    OBFH_CUSTOM_NOP; \
    result_type __obfh_div_result; \
    BREAK_STACK_CFLOW; \
    OBFH_CUSTOM_NOP; \
    __obfh_div_result.quot = __obfh_div_args.n / __obfh_div_args.d; \
    __obfh_div_result.rem = __builtin_choose_expr(OBFH_CUSTOM_BIT, \
                                                  __obfh_div_args.n - __obfh_div_result.quot * __obfh_div_args.d, \
                                                  __obfh_div_args.n - __obfh_div_args.d * __obfh_div_result.quot); \
    __obfh_div_result; \
})
#define div(n, d) OBFH_INTEGER_DIV(n, d, int, div_t)
#define ldiv(n, d) OBFH_INTEGER_DIV(n, d, long, ldiv_t)
#define lldiv(n, d) OBFH_INTEGER_DIV(n, d, long long, lldiv_t)

#define OBFH_INTEGER_TEXT(input_value, input_buffer, input_radix, value_type, unsigned_type, char_type, signed_value) ({ \
    __label__ __obfh_itoa_digits, __obfh_itoa_reverse, __obfh_itoa_invalid, __obfh_itoa_done; \
    OBFH_CUSTOM_BEGIN; \
    OBFH_CUSTOM_NOP; \
    struct { \
        value_type value; \
        char_type *buffer; \
        int base; \
    } __obfh_itoa_args = {(input_value), (input_buffer), (input_radix)}; \
    OBFH_CUSTOM_NOP; \
    unsigned_type __obfh_itoa_magnitude = (unsigned_type)__obfh_itoa_args.value; \
    size_t __obfh_itoa_start, __obfh_itoa_count, __obfh_itoa_left, __obfh_itoa_right; \
    unsigned int __obfh_itoa_digit; \
    char_type __obfh_itoa_swap; \
    int __obfh_itoa_negative = (signed_value) && __obfh_itoa_args.base == 10 && __obfh_itoa_args.value < 0; \
    char_type *__obfh_itoa_result = __obfh_itoa_args.buffer; \
    BREAK_STACK_CFLOW; \
    OBFH_CUSTOM_NOP; \
    OBFH_CUSTOM_EXIT(!__obfh_itoa_result || __obfh_itoa_args.base < 2 || __obfh_itoa_args.base > 36, __obfh_itoa_invalid); \
    __obfh_itoa_magnitude = __obfh_itoa_negative ? (unsigned_type)0 - __obfh_itoa_magnitude : __obfh_itoa_magnitude; \
    __obfh_itoa_start = __obfh_itoa_negative; \
    __obfh_itoa_count = __obfh_itoa_start; \
    OBFH_CUSTOM_EXIT(!__obfh_itoa_negative, __obfh_itoa_digits); \
    __obfh_itoa_result[0] = '-'; \
__obfh_itoa_digits: \
    __obfh_itoa_digit = __obfh_itoa_magnitude % (unsigned int)__obfh_itoa_args.base; \
    __obfh_itoa_result[__obfh_itoa_count++] = (char_type)(__obfh_itoa_digit < 10 ? '0' + __obfh_itoa_digit : 'a' + __obfh_itoa_digit - 10); \
    __obfh_itoa_magnitude /= (unsigned int)__obfh_itoa_args.base; \
    OBFH_CUSTOM_EXIT(__obfh_itoa_magnitude, __obfh_itoa_digits); \
    __obfh_itoa_result[__obfh_itoa_count] = 0; \
    __obfh_itoa_left = __obfh_itoa_start; \
    __obfh_itoa_right = __obfh_itoa_count - 1; \
__obfh_itoa_reverse: \
    OBFH_CUSTOM_EXIT(__obfh_itoa_left >= __obfh_itoa_right, __obfh_itoa_done); \
    __obfh_itoa_swap = __obfh_itoa_result[__obfh_itoa_left]; \
    __obfh_itoa_result[__obfh_itoa_left++] = __obfh_itoa_result[__obfh_itoa_right]; \
    __obfh_itoa_result[__obfh_itoa_right--] = __obfh_itoa_swap; \
    goto __obfh_itoa_reverse; \
__obfh_itoa_invalid: \
    *_errno() = EINVAL; \
    __obfh_itoa_result = NULL; \
__obfh_itoa_done:; \
    OBFH_CUSTOM_NOP; \
    (char_type *)obfh_uintptr_proxy((uintptr_t)__obfh_itoa_result); \
})
#define _itoa(v, b, r) OBFH_INTEGER_TEXT(v, b, r, int, unsigned int, char, 1)
#define _ltoa(v, b, r) OBFH_INTEGER_TEXT(v, b, r, long, unsigned long, char, 1)
#define _ultoa(v, b, r) OBFH_INTEGER_TEXT(v, b, r, unsigned long, unsigned long, char, 0)
#define _i64toa(v, b, r) OBFH_INTEGER_TEXT(v, b, r, long long, unsigned long long, char, 1)
#define _ui64toa(v, b, r) OBFH_INTEGER_TEXT(v, b, r, unsigned long long, unsigned long long, char, 0)
#define _itow(v, b, r) OBFH_INTEGER_TEXT(v, b, r, int, unsigned int, wchar_t, 1)
#define _ltow(v, b, r) OBFH_INTEGER_TEXT(v, b, r, long, unsigned long, wchar_t, 1)
#define _ultow(v, b, r) OBFH_INTEGER_TEXT(v, b, r, unsigned long, unsigned long, wchar_t, 0)
#define _i64tow(v, b, r) OBFH_INTEGER_TEXT(v, b, r, long long, unsigned long long, wchar_t, 1)
#define _ui64tow(v, b, r) OBFH_INTEGER_TEXT(v, b, r, unsigned long long, unsigned long long, wchar_t, 0)

// Microsoft three-argument tokenizers share only caller-owned context.
#define OBFH_TOKENIZE(input_text, input_delimiters, input_context, char_type) ({ \
    __label__ __obfh_token_skip, __obfh_token_skip_set, __obfh_token_skip_next, __obfh_token_start, __obfh_token_scan, __obfh_token_scan_set, __obfh_token_scan_next, __obfh_token_split, __obfh_token_end, __obfh_token_invalid, __obfh_token_done; \
    OBFH_CUSTOM_BEGIN; \
    OBFH_CUSTOM_NOP; \
    struct { \
        char_type *text; \
        const char_type *delimiters; \
        char_type **context; \
    } __obfh_token_args = {(input_text), (input_delimiters), (input_context)}; \
    OBFH_CUSTOM_NOP; \
    char_type *__obfh_token_p, *__obfh_token_result = NULL; \
    const char_type *__obfh_token_d; \
    BREAK_STACK_CFLOW; \
    OBFH_CUSTOM_NOP; \
    OBFH_CUSTOM_EXIT(!__obfh_token_args.context || !__obfh_token_args.delimiters, __obfh_token_invalid); \
    __obfh_token_p = __obfh_token_args.text ? __obfh_token_args.text : *__obfh_token_args.context; \
    OBFH_CUSTOM_EXIT(!__obfh_token_p, __obfh_token_invalid); \
__obfh_token_skip: \
    OBFH_CUSTOM_EXIT(!*__obfh_token_p, __obfh_token_end); \
    __obfh_token_d = __obfh_token_args.delimiters; \
__obfh_token_skip_set: \
    OBFH_CUSTOM_EXIT(!*__obfh_token_d, __obfh_token_start); \
    OBFH_CUSTOM_EXIT(*__obfh_token_d == *__obfh_token_p, __obfh_token_skip_next); \
    ++__obfh_token_d; \
    goto __obfh_token_skip_set; \
__obfh_token_skip_next: \
    ++__obfh_token_p; \
    goto __obfh_token_skip; \
__obfh_token_start: \
    __obfh_token_result = __obfh_token_p; \
__obfh_token_scan: \
    OBFH_CUSTOM_EXIT(!*__obfh_token_p, __obfh_token_end); \
    __obfh_token_d = __obfh_token_args.delimiters; \
__obfh_token_scan_set: \
    OBFH_CUSTOM_EXIT(!*__obfh_token_d, __obfh_token_scan_next); \
    OBFH_CUSTOM_EXIT(*__obfh_token_d == *__obfh_token_p, __obfh_token_split); \
    ++__obfh_token_d; \
    goto __obfh_token_scan_set; \
__obfh_token_scan_next: \
    ++__obfh_token_p; \
    goto __obfh_token_scan; \
__obfh_token_split: \
    *__obfh_token_p++ = 0; \
__obfh_token_end: \
    *__obfh_token_args.context = __obfh_token_p; \
    goto __obfh_token_done; \
__obfh_token_invalid: \
    *_errno() = EINVAL; \
    OBFH_CRT_TARGET(void(__cdecl *)(const wchar_t *, const wchar_t *, const wchar_t *, unsigned int, uintptr_t), \
                    HIDE_STRING("_invalid_parameter")) \
    (NULL, NULL, NULL, 0, 0); \
__obfh_token_done:; \
    OBFH_CUSTOM_NOP; \
    (char_type *)obfh_uintptr_proxy((uintptr_t)__obfh_token_result); \
})
#define strtok_s(text, delimiters, context) OBFH_TOKENIZE(text, delimiters, context, char)
#define wcstok_s(text, delimiters, context) OBFH_TOKENIZE(text, delimiters, context, wchar_t)

#define strdup(input) \
    ({ \
        __label__ __obfh_dup_done; \
        OBFH_CUSTOM_BEGIN; \
        OBFH_CUSTOM_NOP; \
        const char *__obfh_dup_source = (input); \
        size_t __obfh_dup_length = __obfh_dup_source ? strlen(__obfh_dup_source) : 0; \
        char *__obfh_dup_result = NULL; \
        OBFH_CUSTOM_EXIT(!__obfh_dup_source || __obfh_dup_length > SIZE_MAX / sizeof(*__obfh_dup_result) - 1u, __obfh_dup_done); \
        __obfh_dup_result = (char *)malloc((__obfh_dup_length + 1u) * sizeof(*__obfh_dup_result)); \
        OBFH_CUSTOM_EXIT(!__obfh_dup_result, __obfh_dup_done); \
        memcpy(__obfh_dup_result, __obfh_dup_source, __obfh_dup_length + 1u); \
    __obfh_dup_done: \
        OBFH_CUSTOM_NOP; \
        __obfh_dup_result; \
    })
#define _strdup(...) strdup(__VA_ARGS__)

#define wcsdup(input) \
    ({ \
        __label__ __obfh_dup_done; \
        OBFH_CUSTOM_BEGIN; \
        OBFH_CUSTOM_NOP; \
        const wchar_t *__obfh_dup_source = (input); \
        size_t __obfh_dup_length = __obfh_dup_source ? wcslen(__obfh_dup_source) : 0; \
        wchar_t *__obfh_dup_result = NULL; \
        OBFH_CUSTOM_EXIT(!__obfh_dup_source || __obfh_dup_length > SIZE_MAX / sizeof(*__obfh_dup_result) - 1u, __obfh_dup_done); \
        __obfh_dup_result = (wchar_t *)malloc((__obfh_dup_length + 1u) * sizeof(*__obfh_dup_result)); \
        OBFH_CUSTOM_EXIT(!__obfh_dup_result, __obfh_dup_done); \
        wmemcpy(__obfh_dup_result, __obfh_dup_source, __obfh_dup_length + 1u); \
    __obfh_dup_done: \
        OBFH_CUSTOM_NOP; \
        __obfh_dup_result; \
    })
#define _wcsdup(...) wcsdup(__VA_ARGS__)

#define strnlen_s(...) \
    ({ \
        __label__ __obfh_safe_length_loop, __obfh_safe_length_done; \
        OBFH_CUSTOM_BEGIN; \
        OBFH_CUSTOM_NOP; \
        struct { \
            const char *text; \
            size_t count; \
        } __obfh_safe_length_args = {__VA_ARGS__}; \
        OBFH_CUSTOM_NOP; \
        size_t __obfh_safe_length_value = 0; \
        BREAK_STACK_CFLOW; \
        OBFH_CUSTOM_EXIT(!__obfh_safe_length_args.text, __obfh_safe_length_done); \
    __obfh_safe_length_loop: \
        OBFH_CUSTOM_EXIT(__obfh_safe_length_value == __obfh_safe_length_args.count || \
                             !__obfh_safe_length_args.text[__obfh_safe_length_value], \
                         __obfh_safe_length_done); \
        ++__obfh_safe_length_value; \
        goto __obfh_safe_length_loop; \
    __obfh_safe_length_done: \
        OBFH_CUSTOM_NOP; \
        __obfh_safe_length_value; \
    })

#define wcsnlen_s(...) \
    ({ \
        __label__ __obfh_safe_length_loop, __obfh_safe_length_done; \
        OBFH_CUSTOM_BEGIN; \
        OBFH_CUSTOM_NOP; \
        struct { \
            const wchar_t *text; \
            size_t count; \
        } __obfh_safe_length_args = {__VA_ARGS__}; \
        OBFH_CUSTOM_NOP; \
        size_t __obfh_safe_length_value = 0; \
        BREAK_STACK_CFLOW; \
        OBFH_CUSTOM_EXIT(!__obfh_safe_length_args.text, __obfh_safe_length_done); \
    __obfh_safe_length_loop: \
        OBFH_CUSTOM_EXIT(__obfh_safe_length_value == __obfh_safe_length_args.count || \
                             !__obfh_safe_length_args.text[__obfh_safe_length_value], \
                         __obfh_safe_length_done); \
        ++__obfh_safe_length_value; \
        goto __obfh_safe_length_loop; \
    __obfh_safe_length_done: \
        OBFH_CUSTOM_NOP; \
        __obfh_safe_length_value; \
    })

#define strrev(input) \
    ({ \
        __label__ __obfh_reverse_loop, __obfh_reverse_done; \
        OBFH_CUSTOM_BEGIN; \
        OBFH_CUSTOM_NOP; \
        char *__obfh_reverse_text = (input); \
        size_t __obfh_reverse_left = 0, __obfh_reverse_right = strlen(__obfh_reverse_text); \
        OBFH_CUSTOM_NOP; \
    __obfh_reverse_loop: \
        OBFH_CUSTOM_EXIT(__obfh_reverse_left >= __obfh_reverse_right || __obfh_reverse_left >= --__obfh_reverse_right, \
                         __obfh_reverse_done); \
        char __obfh_reverse_value = __obfh_reverse_text[__obfh_reverse_left]; \
        __obfh_reverse_text[__obfh_reverse_left] = __obfh_reverse_text[__obfh_reverse_right]; \
        __obfh_reverse_text[__obfh_reverse_right] = __obfh_reverse_value; \
        ++__obfh_reverse_left; \
        goto __obfh_reverse_loop; \
    __obfh_reverse_done: \
        OBFH_CUSTOM_NOP; \
        __obfh_reverse_text; \
    })
#define _strrev(...) strrev(__VA_ARGS__)

#define wcsrev(input) \
    ({ \
        __label__ __obfh_reverse_loop, __obfh_reverse_done; \
        OBFH_CUSTOM_BEGIN; \
        OBFH_CUSTOM_NOP; \
        wchar_t *__obfh_reverse_text = (input); \
        size_t __obfh_reverse_left = 0, __obfh_reverse_right = wcslen(__obfh_reverse_text); \
        OBFH_CUSTOM_NOP; \
    __obfh_reverse_loop: \
        OBFH_CUSTOM_EXIT(__obfh_reverse_left >= __obfh_reverse_right || __obfh_reverse_left >= --__obfh_reverse_right, \
                         __obfh_reverse_done); \
        wchar_t __obfh_reverse_value = __obfh_reverse_text[__obfh_reverse_left]; \
        __obfh_reverse_text[__obfh_reverse_left] = __obfh_reverse_text[__obfh_reverse_right]; \
        __obfh_reverse_text[__obfh_reverse_right] = __obfh_reverse_value; \
        ++__obfh_reverse_left; \
        goto __obfh_reverse_loop; \
    __obfh_reverse_done: \
        OBFH_CUSTOM_NOP; \
        __obfh_reverse_text; \
    })
#define _wcsrev(...) wcsrev(__VA_ARGS__)

// API cache publication precedes invocation, allowing callback reentry.
typedef struct {
#if (OBFH_SEED >> 22) & 1u
    LONG volatile ready;
    PVOID volatile encoded;
#else
    PVOID volatile encoded;
    LONG volatile ready;
#endif
} OBFH_GUI_SLOT;
#define OBFH_GUI_COUNT 275
static PVOID volatile obfh_gui_modules[4];
static inline HMODULE LoadLibraryA_proxy(LPCSTR name);
static inline FARPROC obfh_find_export(HMODULE module, LPCSTR name, unsigned depth);
#define OBFH_GUI_DRAW(index, salt) OBFH_MIX_B(OBFH_MIX_A((unsigned int)(index) ^ (unsigned int)OBFH_SEED ^ (salt)))
#if defined(__x86_64__)
#define OBFH_GUI_WIDTH "q"
#define OBFH_GUI_KEY(index) ((ULONG_PTR)OBFH_GUI_DRAW(index, 0x47554931u) | ((ULONG_PTR)OBFH_GUI_DRAW(index, 0x47554932u) << 32))
#else
#define OBFH_GUI_WIDTH "l"
#define OBFH_GUI_KEY(index) ((ULONG_PTR)OBFH_GUI_DRAW(index, 0x47554931u))
#endif
#define OBFH_GUI_BIAS(index) ((ULONG_PTR)OBFH_GUI_DRAW(index, 0x47554933u))
#define OBFH_GUI_FIELDS(name, index, salt) \
    OBFH_MIX_FIELDS(name, (unsigned int)(index) ^ (unsigned int)OBFH_SEED ^ (salt))
#if defined(__x86_64__)
#define OBFH_GUI_KEY_FIELDS(index) \
    OBFH_GUI_FIELDS(__obfh_gui_key_lo, index, 0x47554931u), \
        OBFH_GUI_FIELDS(__obfh_gui_key_hi, index, 0x47554932u)
#define OBFH_GUI_SITE_KEY ((ULONG_PTR)(unsigned int)__obfh_gui_key_lo | ((ULONG_PTR)(unsigned int)__obfh_gui_key_hi << 32))
#else
#define OBFH_GUI_KEY_FIELDS(index) OBFH_GUI_FIELDS(__obfh_gui_key_lo, index, 0x47554931u)
#define OBFH_GUI_SITE_KEY ((ULONG_PTR)(unsigned int)__obfh_gui_key_lo)
#endif
#define OBFH_GUI_ROTATE(index) (OBFH_GUI_DRAW(index, 0x47554934u) % (sizeof(ULONG_PTR) * 8 - 1) + 1)
// Cold-path name words are decoded at each call site, with no shared name key.
// Select inverse rotation directions once per build, without expanding both forms.
#if (OBFH_SEED & 8u)
#define OBFH_GUI_NAME_ROTATE(value, count) (((value) >> (count)) | ((value) << (32u - (count))))
#define OBFH_GUI_NAME_UNROTATE "roll"
#else
#define OBFH_GUI_NAME_ROTATE(value, count) (((value) << (count)) | ((value) >> (32u - (count))))
#define OBFH_GUI_NAME_UNROTATE "rorl"
#endif
#define OBFH_GUI_NAME_ASM(buffer, position, instructions) ({ \
    __obfh_asm__(OBFH_GUI_NAME_UNROTATE " %4, %0; " instructions "; movl %0, %1" \
                 : "+&r"(__obfh_nc_value), "=m"(*(unsigned char(*)[4])((buffer) + (position))) \
                 : "r"((unsigned int)__obfh_nc_key), "r"((unsigned int)__obfh_nc_bias), \
                   "i"(__obfh_nc_rotate) \
                 : "cc"); \
})
#define OBFH_GUI_NAME_WORD(buffer, position, api, word) ({ \
    enum { __obfh_nc_site = __COUNTER__, \
           __obfh_nc_input = (unsigned int)(word), \
           __obfh_nc_key_a = OBFH_MIX_A(__obfh_nc_site ^ (api) ^ (position) ^ (unsigned int)OBFH_SEED ^ 0x4e414d45u), \
           __obfh_nc_key = OBFH_MIX_B(__obfh_nc_key_a), \
           __obfh_nc_bias_a = OBFH_MIX_A(__obfh_nc_site ^ (unsigned int)OBFH_SEED ^ 0x42595445u), \
           __obfh_nc_bias = OBFH_MIX_B(__obfh_nc_bias_a), \
           __obfh_nc_form = __obfh_nc_key & 3u, \
           __obfh_nc_rotate = (__obfh_nc_key % 31u) + 1u }; \
    unsigned int __obfh_nc_value = \
        __builtin_choose_expr(__obfh_nc_form == 0, ((unsigned int)__obfh_nc_input ^ __obfh_nc_key) + __obfh_nc_bias, \
                              __builtin_choose_expr(__obfh_nc_form == 1, ((unsigned int)__obfh_nc_input + __obfh_nc_bias) ^ __obfh_nc_key, \
                                                    __builtin_choose_expr(__obfh_nc_form == 2, ((unsigned int)__obfh_nc_input ^ __obfh_nc_key) - __obfh_nc_bias, \
                                                                          ((unsigned int)__obfh_nc_input - __obfh_nc_bias) ^ __obfh_nc_key))); \
    __obfh_nc_value = OBFH_GUI_NAME_ROTATE(__obfh_nc_value, __obfh_nc_rotate); \
    __builtin_choose_expr(__obfh_nc_form == 0, OBFH_GUI_NAME_ASM(buffer, position, "subl %3, %0; xorl %2, %0"), \
                          __builtin_choose_expr(__obfh_nc_form == 1, OBFH_GUI_NAME_ASM(buffer, position, "xorl %2, %0; subl %3, %0"), \
                                                __builtin_choose_expr(__obfh_nc_form == 2, OBFH_GUI_NAME_ASM(buffer, position, "addl %3, %0; xorl %2, %0"), \
                                                                      OBFH_GUI_NAME_ASM(buffer, position, "xorl %2, %0; addl %3, %0")))); \
})

// Internal fixed names use caller-owned storage and the same compile-time store selector.
static inline char *getKernel32Name_proxy(char *name) {
    BREAK_STACK_CFLOW;
    OBFH_HIDE_JUNK;
    OBFH_NAME_ORDER_FOR(OBFH_ALPHA_LOW | OBFH_ALPHA_HIGH,
                        (name[OBFH_NAME_INDEX(0)] = _k, name[OBFH_NAME_INDEX(1)] = _e, name[OBFH_NAME_INDEX(2)] = _r, name[OBFH_NAME_INDEX(3)] = _n, name[OBFH_NAME_INDEX(4)] = _e, name[OBFH_NAME_INDEX(5)] = _l, name[6] = ('3'), name[7] = ('2'), name[OBFH_NAME_INDEX(8)] = _0),
                        (name[OBFH_NAME_INDEX(8)] = _0, name[7] = ('2'), name[6] = ('3'), name[OBFH_NAME_INDEX(5)] = _l, name[OBFH_NAME_INDEX(4)] = _e, name[OBFH_NAME_INDEX(3)] = _n, name[OBFH_NAME_INDEX(2)] = _r, name[OBFH_NAME_INDEX(1)] = _e, name[OBFH_NAME_INDEX(0)] = _k));
    PHANTOM_NOP;
    return name;
}

static inline char *getUser32Name_proxy(char *name) {
    BREAK_STACK_CFLOW;
    OBFH_HIDE_JUNK;
    OBFH_NAME_ORDER_FOR(OBFH_ALPHA_LOW | OBFH_ALPHA_HIGH,
                        (name[OBFH_NAME_INDEX(0)] = _u, name[OBFH_NAME_INDEX(1)] = _s, name[OBFH_NAME_INDEX(2)] = _e, name[OBFH_NAME_INDEX(3)] = _r, name[4] = ('3'), name[5] = ('2'), name[OBFH_NAME_INDEX(6)] = _0),
                        (name[OBFH_NAME_INDEX(6)] = _0, name[5] = ('2'), name[4] = ('3'), name[OBFH_NAME_INDEX(3)] = _r, name[OBFH_NAME_INDEX(2)] = _e, name[OBFH_NAME_INDEX(1)] = _s, name[OBFH_NAME_INDEX(0)] = _u));
    PHANTOM_NOP;
    return name;
}

static inline char *getGdi32Name_proxy(char *name) {
    BREAK_STACK_CFLOW;
    OBFH_HIDE_JUNK;
    OBFH_NAME_ORDER_FOR(OBFH_ALPHA_LOW,
                        (name[OBFH_NAME_INDEX(0)] = _g, name[OBFH_NAME_INDEX(1)] = _d, name[OBFH_NAME_INDEX(2)] = _i, name[3] = ('3'), name[4] = ('2'), name[OBFH_NAME_INDEX(5)] = _0),
                        (name[OBFH_NAME_INDEX(5)] = _0, name[4] = ('2'), name[3] = ('3'), name[OBFH_NAME_INDEX(2)] = _i, name[OBFH_NAME_INDEX(1)] = _d, name[OBFH_NAME_INDEX(0)] = _g));
    PHANTOM_NOP;
    return name;
}

static inline char *getAdvapi32Name_proxy(char *name) {
    BREAK_STACK_CFLOW;
    OBFH_HIDE_JUNK;
    OBFH_NAME_ORDER_FOR(OBFH_ALPHA_LOW | OBFH_ALPHA_HIGH,
                        (name[OBFH_NAME_INDEX(0)] = _a, name[OBFH_NAME_INDEX(1)] = _d, name[OBFH_NAME_INDEX(2)] = _v, name[OBFH_NAME_INDEX(3)] = _a, name[OBFH_NAME_INDEX(4)] = _p, name[OBFH_NAME_INDEX(5)] = _i, name[6] = ('3'), name[7] = ('2'), name[OBFH_NAME_INDEX(8)] = _0),
                        (name[OBFH_NAME_INDEX(8)] = _0, name[7] = ('2'), name[6] = ('3'), name[OBFH_NAME_INDEX(5)] = _i, name[OBFH_NAME_INDEX(4)] = _p, name[OBFH_NAME_INDEX(3)] = _a, name[OBFH_NAME_INDEX(2)] = _v, name[OBFH_NAME_INDEX(1)] = _d, name[OBFH_NAME_INDEX(0)] = _a));
    PHANTOM_NOP;
    return name;
}

static inline char *getLoaderName_proxy(char *name) {
    BREAK_STACK_CFLOW;
    OBFH_HIDE_JUNK;
    unsigned char local[16];
    OBFH_GUI_NAME_WORD(local, 0, 0x4c4f4144u, ((unsigned int)'L' | ((unsigned int)'o' << 8) | ((unsigned int)'a' << 16) | ((unsigned int)'d' << 24)));
    OBFH_GUI_NAME_WORD(local, 4, 0x4c4f4144u, ((unsigned int)'L' | ((unsigned int)'i' << 8) | ((unsigned int)'b' << 16) | ((unsigned int)'r' << 24)));
    OBFH_GUI_NAME_WORD(local, 8, 0x4c4f4144u, ((unsigned int)'a' | ((unsigned int)'r' << 8) | ((unsigned int)'y' << 16) | ((unsigned int)'A' << 24)));
    OBFH_GUI_NAME_WORD(local, 12, 0x4c4f4144u, 0u);
    for (unsigned int i = 0; i < 13; ++i)
        name[i] = (char)local[i];
    PHANTOM_NOP;
    return name;
}

static inline char *getDebuggerName_proxy(char *name) {
    BREAK_STACK_CFLOW;
    OBFH_HIDE_JUNK;
    OBFH_NAME_ORDER_FOR(OBFH_ALPHA_LOW | OBFH_ALPHA_HIGH | OBFH_ALPHA_UPPER,
                        (OBFH_NAME_PAIR(1, name[OBFH_NAME_INDEX(0)] = _I, name[OBFH_NAME_INDEX(1)] = _s), OBFH_NAME_PAIR(2, name[OBFH_NAME_INDEX(2)] = _D, name[OBFH_NAME_INDEX(3)] = _e), OBFH_NAME_PAIR(3, name[OBFH_NAME_INDEX(4)] = _b, name[OBFH_NAME_INDEX(5)] = _u), OBFH_NAME_PAIR(4, name[OBFH_NAME_INDEX(6)] = _g, name[OBFH_NAME_INDEX(7)] = _g), OBFH_NAME_PAIR(5, name[OBFH_NAME_INDEX(8)] = _e, name[OBFH_NAME_INDEX(9)] = _r), OBFH_NAME_PAIR(6, name[OBFH_NAME_INDEX(10)] = _P, name[OBFH_NAME_INDEX(11)] = _r), OBFH_NAME_PAIR(7, name[OBFH_NAME_INDEX(12)] = _e, name[OBFH_NAME_INDEX(13)] = _s), OBFH_NAME_PAIR(8, name[OBFH_NAME_INDEX(14)] = _e, name[OBFH_NAME_INDEX(15)] = _n), OBFH_NAME_PAIR(9, name[OBFH_NAME_INDEX(16)] = _t, name[OBFH_NAME_INDEX(17)] = _0)),
                        (OBFH_NAME_PAIR(1, name[OBFH_NAME_INDEX(17)] = _0, name[OBFH_NAME_INDEX(16)] = _t), OBFH_NAME_PAIR(2, name[OBFH_NAME_INDEX(15)] = _n, name[OBFH_NAME_INDEX(14)] = _e), OBFH_NAME_PAIR(3, name[OBFH_NAME_INDEX(13)] = _s, name[OBFH_NAME_INDEX(12)] = _e), OBFH_NAME_PAIR(4, name[OBFH_NAME_INDEX(11)] = _r, name[OBFH_NAME_INDEX(10)] = _P), OBFH_NAME_PAIR(5, name[OBFH_NAME_INDEX(9)] = _r, name[OBFH_NAME_INDEX(8)] = _e), OBFH_NAME_PAIR(6, name[OBFH_NAME_INDEX(7)] = _g, name[OBFH_NAME_INDEX(6)] = _g), OBFH_NAME_PAIR(7, name[OBFH_NAME_INDEX(5)] = _u, name[OBFH_NAME_INDEX(4)] = _b), OBFH_NAME_PAIR(8, name[OBFH_NAME_INDEX(3)] = _e, name[OBFH_NAME_INDEX(2)] = _D), OBFH_NAME_PAIR(9, name[OBFH_NAME_INDEX(1)] = _s, name[OBFH_NAME_INDEX(0)] = _I)));
    PHANTOM_NOP;
    return name;
}

// Native cold-path control avoids expanding CFLOW at every cache check.
static inline ULONG_PTR obfh_gui_cold(unsigned int module_id, OBFH_GUI_SLOT *slot, unsigned int key_id,
                                      const unsigned char *name, size_t length) {
    DWORD saved_error = GetLastError();
    BREAK_STACK_CFLOW;
    PHANTOM_NOP;
    if (module_id >= 4 || !slot || !name || !length || length > 64)
        ExitProcess(0xe0bf4701u);
    HMODULE module = (HMODULE)InterlockedCompareExchangePointer(&obfh_gui_modules[module_id], NULL, NULL);
    if (!module) {
        char library[9];
        module_id == 3 ? getAdvapi32Name_proxy(library)
                       : (module_id == 2 ? getKernel32Name_proxy(library)
                                         : (module_id ? getGdi32Name_proxy(library) : getUser32Name_proxy(library)));
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
    if (name[length - 1])
        ExitProcess(0xe0bf4701u);
    FARPROC address = obfh_find_export(module, (const char *)name, 0);
    if (!address)
        ExitProcess(0xe0bf4701u);
    ULONG_PTR value = (ULONG_PTR)address, key = OBFH_GUI_KEY(key_id), bias = OBFH_GUI_BIAS(key_id);
    unsigned int rotate = OBFH_GUI_ROTATE(key_id), form = OBFH_GUI_DRAW(key_id, 0x47554936u) % 6u;
    // Only the cold resolver branches on the form. Warm calls keep three instructions.
    // Encode in the reverse order of the selected warm decoder.
    if (form == 5)
        value = (value << rotate) | (value >> (sizeof(ULONG_PTR) * 8 - rotate));
    if (form == 0 || form == 3) value ^= key;
    if (form == 1 || form == 3 || form == 4 || form == 5) value += bias;
    if (form == 4 || form == 5) value ^= key;
    if (form != 5)
        value = (value << rotate) | (value >> (sizeof(ULONG_PTR) * 8 - rotate));
    if (form == 1 || form == 2) value ^= key;
    if (form == 0 || form == 2) value += bias;
    // Zero is a valid encoding; ready is published after the atomic pointer write.
    InterlockedCompareExchangePointer(&slot->encoded, (PVOID)value, NULL);
    InterlockedExchange(&slot->ready, 1);
    SetLastError(saved_error);
    return value;
}
// #endregion

// #region 13. Keyword interception: if / else / case / break / switch / while / for / continue / default / do
// ============================================================================
// 13. Keyword interception: if / else / case / break / switch / while / for / continue / default / do
// ============================================================================

#if OBFH_DISABLE_CONTROL_FLOW != 1
// One short jump, optionally two inverse LEAs on a caller-scratch register.
// Full register width is preserved, including x64 upper bits; no flags, memory
// or stack changes. Each complete insertion is 10..25 bytes on both targets.
// Capture one full draw; derive other fields from enum names instead of
// repeatedly expanding the source/seed formula at every operand.
#define OBFH_FLOW_FAST_TRANSFER \
    ({ \
        enum { __obfh_fast_site = __COUNTER__, \
               OBFH_DATA_FIELDS(__obfh_fast_key, __obfh_fast_site), \
               __obfh_fast_alt = OBFH_MIX_A(__obfh_fast_key ^ 0xD15EA5E1u), \
               __obfh_fast_word = OBFH_MIX_B(__obfh_fast_alt ^ 0xA3617B29u), \
               __obfh_fast_pair = (__obfh_fast_alt >> 16) & 1u, \
               __obfh_fast_before = (__obfh_fast_alt >> 17) & 1u, \
               __obfh_fast_wide = sizeof(void *) == 8, \
               __obfh_fast_modrm = 0x40u + 9u * ((__obfh_fast_alt >> 18) % 3u), \
               __obfh_fast_magnitude = 1u + ((__obfh_fast_alt >> 1) % 127u), \
               __obfh_fast_delta = (__obfh_fast_word & 1u) ? __obfh_fast_magnitude : 256u - __obfh_fast_magnitude, \
               __obfh_fast_lea = (__obfh_fast_wide ? 0x8D48u : 0x8Du) | (__obfh_fast_modrm << (8u + 8u * __obfh_fast_wide)), \
               __obfh_fast_add = __obfh_fast_lea | (__obfh_fast_delta << (16u + 8u * __obfh_fast_wide)), \
               __obfh_fast_sub = __obfh_fast_lea | ((256u - __obfh_fast_delta) << (16u + 8u * __obfh_fast_wide)) }; \
        __obfh_asm__(".fill %c0,%c1,%c2; .fill %c0,%c1,%c3; .byte 0xeb,8+%c4; .fill %c5,1,%c6; .long %c7,%c8; .fill %c9,1,%c6; .fill %c10,%c1,%c2; .fill %c10,%c1,%c3;" \
                     : \
                     : "i"(__obfh_fast_pair * __obfh_fast_before), "i"(3u + __obfh_fast_wide), \
                       "i"(__obfh_fast_add), "i"(__obfh_fast_sub), "i"((__obfh_fast_alt >> 8) & 7u), \
                       "i"(((__obfh_fast_alt >> 8) & 7u) * ((__obfh_fast_alt >> 12) & 1u)), \
                       "i"(__obfh_fast_alt >> 24), \
                       "i"((__obfh_fast_alt & 1u) ? __obfh_fast_key : __obfh_fast_word), \
                       "i"((__obfh_fast_alt & 1u) ? __obfh_fast_word : __obfh_fast_key), \
                       "i"(((__obfh_fast_alt >> 8) & 7u) * (1u - ((__obfh_fast_alt >> 12) & 1u))), \
                       "i"(__obfh_fast_pair * (1u - __obfh_fast_before))); \
    })

// A statement expression consumes the caller's semicolon without introducing
// a loop (which would capture continue) or an unmatched if/else.
#define continue ({ OBFH_FLOW_FAST_TRANSFER; continue; })

#define OBFH_DEFAULT_LABEL_I(site) __obfh_default_entry_##site
#define OBFH_DEFAULT_LABEL(site) OBFH_DEFAULT_LABEL_I(site)
// The user's colon terminates the unique ordinary label. Fallthrough executes
// the same harmless insertion as direct switch dispatch. Labels are function-wide.
#define default \
    default: \
        OBFH_FLOW_FAST_TRANSFER; \
        OBFH_DEFAULT_LABEL(__COUNTER__)

// Constrain a transformed 32-bit source around an anchored bit. The scan is
// defined (source is never zero) and always returns that bit's index, for every
// input. BSF/BSR, mix opcode, masks, bit index and optional rotation compose
// independently at compile time; there is no runtime selector or shared state.
#define OBFH_FLOW_DO_FIELDS(site) \
    enum { OBFH_DATA_FIELDS(__obfh_do_key, site), \
           OBFH_DATA_FIELDS(__obfh_do_aux, (site) ^ 0xC47A31B5u), \
           OBFH_DATA_FIELDS(__obfh_do_bits, (site) ^ 0x951D8E63u), \
           __obfh_do_index = 1u + ((__obfh_do_aux >> 8) % 30u), \
           __obfh_do_reverse = __obfh_do_aux & 1u, \
           __obfh_do_anchor = 1u << __obfh_do_index, \
           __obfh_do_neighbor = __obfh_do_reverse ? __obfh_do_anchor >> 1 : __obfh_do_anchor << 1, \
           __obfh_do_range = __obfh_do_reverse ? __obfh_do_anchor - 1u : ~(__obfh_do_anchor | (__obfh_do_anchor - 1u)), \
           __obfh_do_mask = (__obfh_do_bits & __obfh_do_range) | __obfh_do_neighbor, \
           __obfh_do_force = (__obfh_do_key & __obfh_do_range & ~__obfh_do_neighbor) | __obfh_do_anchor }
// prefix supplies the source in EAX. The production prefix only reads ESP;
// tests can feed arbitrary uint32 inputs to the exact same emitted operations.
#define OBFH_FLOW_DO_SCAN(output, prefix, ...) \
    __obfh_asm__(prefix ".byte %c1; .long %c2; .fill %c3,1,(0xc1 + 0x10 * ((%c4 == 1) & 1)); .fill %c3,1,0xc0; .fill (%c3 * ((%c4 != 1) & 1)),1,%c4; andl $%c5, %%eax; orl $%c6, %%eax; .byte 0x0f,%c7,0xc0;" \
                 : "=a"(output) \
                 : "i"((__obfh_do_aux & 2u) ? 0x35u : 0x05u), "i"(__obfh_do_key), \
                   "i"((__obfh_do_aux >> 2) & 1u), "i"(1u + ((__obfh_do_aux >> 16) % 31u)), \
                   "i"(__obfh_do_mask), "i"(__obfh_do_force), "i"(0xbcu + __obfh_do_reverse), ##__VA_ARGS__ \
                 : "cc")

// No extra loop or switch: break/continue retain their native targets. Splitting
// if from its parentheses avoids recursively protecting the internal guard.
// Five or six ASM instructions, then TCC's ordinary result store/test/branch.
#define OBFH_FLOW_DO_ENTRY \
    ({ \
        enum { __obfh_do_site = __COUNTER__ }; \
        OBFH_FLOW_DO_FIELDS(__obfh_do_site); \
        unsigned int __obfh_do_result; \
        OBFH_FLOW_DO_SCAN(__obfh_do_result, "movl %%esp, %%eax;"); \
        __obfh_do_result == __obfh_do_index; \
    })
#define OBFH_FLOW_DO_GUARD (OBFH_FLOW_DO_ENTRY)
#define do \
    do \
        if OBFH_FLOW_DO_GUARD

// Each intercepted if links its selected proxy guard to encoded condition state.
#define OBFH_PUBLIC_IF_CONDITION(...) ({ \
    enum { __obfh_if_site = RND(1, 65535) }; \
    OBFH_FLOW_CONDITION((__VA_ARGS__), __obfh_if_site, OBFH_P_PROXY; BREAK_STACK_CFLOW); \
})
#define if(...) if (OBFH_PUBLIC_IF_CONDITION(__VA_ARGS__))

// Select the guard before expanding it: while else is active its token is
// suppressed; while case is active, else expands to the two tokens else case.
// Paste/rescan distinguishes those contexts without a runtime selector or
// expanding the heavier else guard at case sites.
#define OBFH_CASE_CONTEXT_I(tokens) OBFH_CASE_CONTEXT_##tokens##__obfh_guard
#define OBFH_CASE_CONTEXT(tokens) OBFH_CASE_CONTEXT_I(tokens)
#define OBFH_CASE_CONTEXT_else__obfh_guard() \
    if OBFH_FLOW_ELSE_GUARD { \
    } else
#define OBFH_CASE_LABEL_I(site) __obfh_case_entry_##site
#define OBFH_CASE_LABEL(site) OBFH_CASE_LABEL_I(site)
// An ordinary label keeps the bytes emitted even after break/return. It adds
// no instruction and wraps the guard as one statement, including under if.
#define OBFH_CASE_FAST_GUARD (({ OBFH_FLOW_FAST_TRANSFER; 0; }))
#define OBFH_CASE_CONTEXT_else() \
    OBFH_CASE_LABEL(__COUNTER__) : if OBFH_CASE_FAST_GUARD {} \
    else
#define case__obfh_guard
#define OBFH_CASE_RESCAN(tokens) tokens
#define OBFH_CASE_ENTRY OBFH_CASE_RESCAN(OBFH_CASE_CONTEXT(else))()
// Mutual rescan preserves a single matched statement at a case label.
// From else, case expands and the trailing else is suppressed. From case,
// else expands and its case is suppressed, leaving the native case keyword.
// Direct dispatch bypasses the insertion; fallthrough executes the compact
// transfer. Ordinary else keeps its original guard and transport unchanged.
#define else else case
#define case OBFH_CASE_ENTRY

#define OBFUS_CONDITION_BLOCK(...) OBFH_FLOW_CONDITION((__VA_ARGS__), RND(1, 65535))

// Keep the native break outside the expression so it retains its original target.
#define OBFH_FLOW_BREAK_GUARD \
    (({ \
        if (OBFUS_CONDITION_BLOCK(RND(1, 255))) \
            BREAK_STACK_CFLOW; \
        1; \
    }))

// A matched conditional consumes the caller's semicolon and preserves dangling else.
#define break \
    if OBFH_FLOW_BREAK_GUARD \
        break; \
    else \
        (void)0

#define switch(...) \
    switch (({ \
        __typeof__((__VA_ARGS__)) __obfh_switch_value = (__VA_ARGS__); \
        volatile ULONG_PTR __obfh_switch_shift = (ULONG_PTR)OBFUS_CONDITION_BLOCK(RND(1, 255)) * SALT_SHIFT; \
        ULONG_PTR __obfh_switch_address = obfh_uintptr_proxy((ULONG_PTR)&__obfh_switch_value ^ __obfh_switch_shift); \
        *(__typeof__(&__obfh_switch_value))(__obfh_switch_address ^ __obfh_switch_shift); \
    }))

#define while(...) while (OBFUS_CONDITION_BLOCK((__VA_ARGS__)))

// Protect every iteration and retain randomized bytes in the never-taken arm.
#define for(...) \
    for (__VA_ARGS__) \
        if (0) { \
            OBFH_FLOW_DEAD_BYTES; \
        } else

// Service branches keep a live, bijective condition dependency without copying
// the full public condition graph into every cache/name adapter. All choices
// are compile-time; the original condition is evaluated exactly once.
#define OBFH_HELPER_ROTATE_VALUE(x, count, right) \
    ((right) ? (((unsigned int)(x) >> (count)) | ((unsigned int)(x) << (32u - (count)))) : (((unsigned int)(x) << (count)) | ((unsigned int)(x) >> (32u - (count)))))
// Three instruction slots: rotate before, between, or after the two mixes.
// EAX's immediate encodings keep each arithmetic slot at five bytes.
#define OBFH_HELPER_MIX(state, key, bias, form) \
    __obfh_asm__(".fill 1,%c[len0],%c[code0]; .fill %c[imm0],4,%c[key0];.fill 1,%c[len1],%c[code1]; .fill %c[imm1],4,%c[key1];.fill 1,%c[len2],%c[code2]; .fill %c[imm2],4,%c[key2];" \
                 : "+a"(state) \
                 : [len0] "i"(__obfh_helper_position == 0 ? 3 : 1), [imm0] "i"(__obfh_helper_position != 0), \
                   [code0] "i"(__obfh_helper_position == 0 ? __obfh_helper_rotation : __obfh_helper_op0), [key0] "i"(key), \
                   [len1] "i"(__obfh_helper_position == 1 ? 3 : 1), [imm1] "i"(__obfh_helper_position != 1), \
                   [code1] "i"(__obfh_helper_position == 1 ? __obfh_helper_rotation : __obfh_helper_position == 0 ? __obfh_helper_op0 \
                                                                                                                  : __obfh_helper_op1), \
                   [key1] "i"(__obfh_helper_position == 0 ? (key) : (bias)), \
                   [len2] "i"(__obfh_helper_position == 2 ? 3 : 1), [imm2] "i"(__obfh_helper_position != 2), \
                   [code2] "i"(__obfh_helper_position == 2 ? __obfh_helper_rotation : __obfh_helper_op1), [key2] "i"(bias) \
                 : "cc")

#define OBFH_HELPER_CONDITION_AT(condition, site) \
    ({ \
        enum { OBFH_DATA_FIELDS(__obfh_helper_draw, site), \
               __obfh_helper_key = OBFH_MIX_A(__obfh_helper_draw ^ 0xA571D393u), \
               __obfh_helper_bias = OBFH_MIX_B(__obfh_helper_draw ^ 0x61E29B47u), \
               __obfh_helper_form = __obfh_helper_draw & 3u, \
               __obfh_helper_right = (__obfh_helper_draw >> 2) & 1u, \
               __obfh_helper_position = (__obfh_helper_draw >> 10) % 3u, \
               __obfh_helper_count = 1u + ((__obfh_helper_draw >> 4) % 31u), \
               __obfh_helper_true = (__obfh_helper_draw & 512u) ? 0u : 1u, \
               __obfh_helper_op0 = __obfh_helper_form == 0 || __obfh_helper_form == 3 ? 0x35 : __obfh_helper_form == 1 ? 0x05 \
                                                                                                                       : 0x2d, \
               __obfh_helper_op1 = __obfh_helper_form == 0 ? 0x05 : __obfh_helper_form == 3 ? 0x2d \
                                                                                            : 0x35, \
               __obfh_helper_rotation = 0xc1u | ((__obfh_helper_right ? 0xc8u : 0xc0u) << 8) | (__obfh_helper_count << 16), \
               __obfh_helper_start = __obfh_helper_position == 0 ? OBFH_HELPER_ROTATE_VALUE(__obfh_helper_true, __obfh_helper_count, __obfh_helper_right) : __obfh_helper_true, \
               __obfh_helper_mid = (unsigned int)(__obfh_helper_op0 == 0x35 ? __obfh_helper_start ^ __obfh_helper_key : __obfh_helper_op0 == 0x05 ? __obfh_helper_start + __obfh_helper_key \
                                                                                                                                                  : __obfh_helper_start - __obfh_helper_key), \
               __obfh_helper_rotated = __obfh_helper_position == 1 ? OBFH_HELPER_ROTATE_VALUE(__obfh_helper_mid, __obfh_helper_count, __obfh_helper_right) : __obfh_helper_mid, \
               __obfh_helper_end = (unsigned int)(__obfh_helper_op1 == 0x35 ? __obfh_helper_rotated ^ __obfh_helper_bias : __obfh_helper_op1 == 0x05 ? __obfh_helper_rotated + __obfh_helper_bias \
                                                                                                                                                     : __obfh_helper_rotated - __obfh_helper_bias), \
               __obfh_helper_expected = __obfh_helper_position == 2 ? OBFH_HELPER_ROTATE_VALUE(__obfh_helper_end, __obfh_helper_count, __obfh_helper_right) : __obfh_helper_end }; \
        unsigned int __obfh_helper_state = __builtin_choose_expr(__obfh_helper_true, !!(condition), !(condition)); \
        OBFH_HELPER_MIX(__obfh_helper_state, __obfh_helper_key, __obfh_helper_bias, __obfh_helper_form); \
        __obfh_helper_state == (unsigned int)__obfh_helper_expected; \
    })
#define OBFH_HELPER_CONDITION(condition) OBFH_HELPER_CONDITION_AT(condition, __COUNTER__)
#define OBFH_HELPER_ELSE_GUARD (OBFH_HELPER_CONDITION(0))

#endif

// #endregion

// #region 14. Public VM interface and instruction-program templates
// ============================================================================
// 14. Public VM interface and instruction-program templates
// ============================================================================

// Virtualization (instruction programs)

/* Each expression owns an immutable encoded instruction stream. */
#define OBFH_V_INSTRUCTION(op, d, a, b, imm) \
    (OBFH_V_OPCODE_ENCODE(op, __obfh_vkey) | ((unsigned int)(d) << OBFH_VM_FIELD_D) | \
     ((unsigned int)(a) << OBFH_VM_FIELD_A) | ((unsigned int)(b) << OBFH_VM_FIELD_B) | (((unsigned int)(imm)&0x3ffffu) << 14))
#define OBFH_V_OPERATION_OPCODE_0 \
    (__obfh_voperation >= OBFH_VOP_BAND ? OBFH_V_BAND + __obfh_voperation - OBFH_VOP_BAND : __obfh_voperation < 5 ? OBFH_V_ADD + __obfh_voperation \
                                                                                                                  : (__obfh_voperation < 11 ? OBFH_V_COMPARE : (__obfh_voperation == OBFH_VOP_ID ? OBFH_V_MOVE : OBFH_V_TEST)))
#define OBFH_V_OPERATION_OPCODE_1 \
    (((__obfh_voperation >= 5 && __obfh_voperation < 11) || __obfh_voperation == OBFH_VOP_TRUTH) ? OBFH_V_BOOLEAN : OBFH_V_CXOR)
#define OBFH_V_OPERATION_IMMEDIATE \
    (__obfh_voperation == OBFH_VOP_TRUTH ? 6 : (__obfh_voperation >= 5 ? __obfh_voperation - 5 : 0))
#define OBFH_V_OPERATION_0(d, a, b) OBFH_V_INSTRUCTION(OBFH_V_OPERATION_OPCODE_0, d, a, b, 0)
#define OBFH_V_OPERATION_1(d, a) OBFH_V_INSTRUCTION(OBFH_V_OPERATION_OPCODE_1, d, a, 0, OBFH_V_OPERATION_IMMEDIATE)
#define OBFH_V_PACK(w) \
    __builtin_choose_expr(((__obfh_vkey >> 29) & 3u) == 1, \
                          (((w)&255u) << 24) | ((((w) >> 8) & 3u) << 18) | ((((w) >> 10) & 3u) << 20) | ((((w) >> 12) & 3u) << 22) | ((w) >> 14), \
                          __builtin_choose_expr(((__obfh_vkey >> 29) & 3u) == 2, \
                                                (((w)&255u) << 6) | (((w) >> 8) & 3u) | ((((w) >> 10) & 3u) << 2) | ((((w) >> 12) & 3u) << 4) | ((w)&0xffffc000u), \
                                                __builtin_choose_expr(((__obfh_vkey >> 29) & 3u) == 3, \
                                                                      ((w)&255u) | ((((w) >> 8) & 3u) << 12) | ((((w) >> 10) & 3u) << 8) | ((((w) >> 12) & 3u) << 10) | ((w)&0xffffc000u), (w))))
#define OBFH_V_ENCODE_RAW(word, pc) \
    OBFH_V_WORD_ENCODE((((unsigned int)(word) >> ((__obfh_vkey % 31u) + 1u)) | ((unsigned int)(word) << (32u - ((__obfh_vkey % 31u) + 1u)))), __obfh_vkey, pc)
#define OBFH_V_ENCODE(word, pc) OBFH_V_ENCODE_RAW(OBFH_V_PACK(word), pc)
// Construct words in their selected packed layout, without repeatedly extracting each field.
#define OBFH_V_PACKED_FIELD(field) \
    (__obfh_vlayout == 1 ? (field) + 10u : __obfh_vlayout == 2 ? (field)-8u \
                                       : __obfh_vlayout == 3   ? ((field) == 8u ? 12u : (field)-2u) \
                                                               : (field))
#define OBFH_V_PACKED_INSTRUCTION(op, d, a, b, imm) \
    ((OBFH_V_OPCODE_ENCODE(op, __obfh_vkey) << __obfh_vopcode_shift) | ((unsigned int)(d) << __obfh_vfield_d) | \
     ((unsigned int)(a) << __obfh_vfield_a) | ((unsigned int)(b) << __obfh_vfield_b) | (((unsigned int)(imm)&0x3ffffu) << __obfh_vimm_shift))
#define OBFH_V_PACKED_OPERATION_0(d, a, b) OBFH_V_PACKED_INSTRUCTION(OBFH_V_OPERATION_OPCODE_0, d, a, b, 0)
#define OBFH_V_PACKED_OPERATION_1(d, a) OBFH_V_PACKED_INSTRUCTION(OBFH_V_OPERATION_OPCODE_1, d, a, 0, OBFH_V_OPERATION_IMMEDIATE)
// Capture layout positions and reordered loads once per VM expression.
#define OBFH_V_PROGRAM_SETUP \
    enum { \
        __obfh_vlayout = (__obfh_vkey >> 29) & 3u, \
        __obfh_vopcode_shift = __obfh_vlayout == 1 ? 24u : __obfh_vlayout == 2 ? 6u \
                                                                               : 0u, \
        __obfh_vimm_shift = __obfh_vlayout == 1 ? 0u : 14u, \
        __obfh_vfield_d = OBFH_V_PACKED_FIELD(OBFH_VM_FIELD_D), \
        __obfh_vfield_a = OBFH_V_PACKED_FIELD(OBFH_VM_FIELD_A), \
        __obfh_vfield_b = OBFH_V_PACKED_FIELD(OBFH_VM_FIELD_B), \
        __obfh_vload0 = OBFH_V_PACKED_INSTRUCTION((__obfh_vkey & 32u) ? OBFH_V_LOAD_A : OBFH_V_LOAD_B, (__obfh_vkey & 32u) ? __obfh_va : __obfh_vb, 0, 0, 0), \
        __obfh_vload1 = OBFH_V_PACKED_INSTRUCTION((__obfh_vkey & 32u) ? OBFH_V_LOAD_B : OBFH_V_LOAD_A, (__obfh_vkey & 32u) ? __obfh_vb : __obfh_va, 0, 0, 0), \
        __obfh_vprep0 = ((__obfh_vkey & 64u) ? OBFH_V_PACKED_INSTRUCTION(OBFH_V_CROL, 1, 0, 0, 7) : OBFH_V_PACKED_INSTRUCTION(OBFH_V_MOVE, __obfh_vd, __obfh_va, 0, 0)), \
        __obfh_vprep1 = ((__obfh_vkey & 64u) ? OBFH_V_PACKED_INSTRUCTION(OBFH_V_MOVE, __obfh_vd, __obfh_va, 0, 0) : OBFH_V_PACKED_INSTRUCTION(OBFH_V_CROL, 1, 0, 0, 7)) \
    }
#define OBFH_V_PROGRAM_0 \
    (const unsigned int[]) { \
        OBFH_V_ENCODE_RAW(OBFH_V_PACKED_INSTRUCTION(OBFH_V_LOAD_PAIR, __obfh_va, __obfh_vb, 0, 0), 0), \
            OBFH_V_ENCODE_RAW(OBFH_V_PACKED_OPERATION_0(__obfh_vc, __obfh_va, __obfh_vb), 1), \
            OBFH_V_ENCODE_RAW(OBFH_V_PACKED_OPERATION_1(__obfh_vc, __obfh_va), 2), \
            OBFH_V_ENCODE_RAW(OBFH_V_PACKED_INSTRUCTION(OBFH_V_RETURN, 0, __obfh_vc, 0, 0), 3) \
    }
#define OBFH_V_PROGRAM_1 \
    (const unsigned int[]) { \
        OBFH_V_ENCODE_RAW(__obfh_vload0, 0), \
            OBFH_V_ENCODE_RAW(__obfh_vload1, 1), \
            OBFH_V_ENCODE_RAW(OBFH_V_PACKED_INSTRUCTION(OBFH_V_MOVE, __obfh_vd, __obfh_va, 0, 0), 2), \
            OBFH_V_ENCODE_RAW(OBFH_V_PACKED_OPERATION_0(__obfh_vc, __obfh_vd, __obfh_vb), 3), \
            OBFH_V_ENCODE_RAW(OBFH_V_PACKED_OPERATION_1(__obfh_vc, __obfh_vd), 4), \
            OBFH_V_ENCODE_RAW(OBFH_V_PACKED_INSTRUCTION(OBFH_V_COPY_RETURN, __obfh_va, __obfh_vc, 0, 0), 5) \
    }
#define OBFH_V_PROGRAM_2 \
    (const unsigned int[]) { \
        OBFH_V_ENCODE_RAW(OBFH_V_PACKED_INSTRUCTION(OBFH_V_LOAD_PAIR, __obfh_va, __obfh_vb, 0, 0), 0), \
            OBFH_V_ENCODE_RAW(OBFH_V_PACKED_INSTRUCTION(OBFH_V_TEST, 0, __obfh_va, 0, 0), 1), \
            OBFH_V_ENCODE_RAW(OBFH_V_PACKED_INSTRUCTION(OBFH_V_JFLAG, 0, 1, 0, 7), 2), \
            OBFH_V_ENCODE_RAW(OBFH_V_PACKED_OPERATION_0(__obfh_vc, __obfh_va, __obfh_vb), 3), \
            OBFH_V_ENCODE_RAW(OBFH_V_PACKED_OPERATION_1(__obfh_vc, __obfh_va), 4), \
            OBFH_V_ENCODE_RAW(OBFH_V_PACKED_INSTRUCTION(OBFH_V_JUMP, 0, 0, 0, 10), 5), \
            OBFH_V_ENCODE_RAW(OBFH_V_PACKED_INSTRUCTION(OBFH_V_CROL, 0, 0, 0, 3), 6), \
            OBFH_V_ENCODE_RAW(OBFH_V_PACKED_OPERATION_0(__obfh_vd, __obfh_va, __obfh_vb), 7), \
            OBFH_V_ENCODE_RAW(OBFH_V_PACKED_OPERATION_1(__obfh_vd, __obfh_va), 8), \
            OBFH_V_ENCODE_RAW(OBFH_V_PACKED_INSTRUCTION(OBFH_V_MOVE, __obfh_vc, __obfh_vd, 0, 0), 9), \
            OBFH_V_ENCODE_RAW(OBFH_V_PACKED_INSTRUCTION(OBFH_V_RETURN, 0, __obfh_vc, 0, 0), 10) \
    }
#define OBFH_V_PROGRAM_3 \
    (const unsigned int[]) { \
        OBFH_V_ENCODE_RAW(OBFH_V_PACKED_INSTRUCTION(OBFH_V_LOAD_PAIR, __obfh_va, __obfh_vb, 0, 0), 0), \
            OBFH_V_ENCODE_RAW(OBFH_V_PACKED_INSTRUCTION(OBFH_V_TEST, 0, __obfh_va, 0, 0), 1), \
            OBFH_V_ENCODE_RAW(OBFH_V_PACKED_INSTRUCTION(OBFH_V_JFLAG, 0, 1, 0, 7), 2), \
            OBFH_V_ENCODE_RAW(OBFH_V_PACKED_OPERATION_0(__obfh_vc, __obfh_va, __obfh_vb), 3), \
            OBFH_V_ENCODE_RAW(OBFH_V_PACKED_OPERATION_1(__obfh_vc, __obfh_va), 4), \
            OBFH_V_ENCODE_RAW(OBFH_V_PACKED_INSTRUCTION(OBFH_V_RETURN, 0, __obfh_vc, 0, 0), 5), \
            OBFH_V_ENCODE_RAW(OBFH_V_PACKED_INSTRUCTION(OBFH_V_CXOR, 0, 0, 0, 5), 6), \
            OBFH_V_ENCODE_RAW(OBFH_V_PACKED_OPERATION_0(__obfh_vd, __obfh_va, __obfh_vb), 7), \
            OBFH_V_ENCODE_RAW(OBFH_V_PACKED_OPERATION_1(__obfh_vd, __obfh_va), 8), \
            OBFH_V_ENCODE_RAW(OBFH_V_PACKED_INSTRUCTION(OBFH_V_RETURN, 0, __obfh_vd, 0, 0), 9) \
    }
#define OBFH_V_PROGRAM_4 \
    (const unsigned int[]) { \
        OBFH_V_ENCODE_RAW(OBFH_V_PACKED_INSTRUCTION(OBFH_V_JUMP, 0, 0, 0, 4), 0), \
            OBFH_V_ENCODE_RAW(OBFH_V_PACKED_OPERATION_0(__obfh_vc, __obfh_va, __obfh_vb), 1), \
            OBFH_V_ENCODE_RAW(OBFH_V_PACKED_OPERATION_1(__obfh_vc, __obfh_va), 2), \
            OBFH_V_ENCODE_RAW(OBFH_V_PACKED_INSTRUCTION(OBFH_V_RETURN, 0, __obfh_vc, 0, 0), 3), \
            OBFH_V_ENCODE_RAW(OBFH_V_PACKED_INSTRUCTION(OBFH_V_LOAD_PAIR, __obfh_va, __obfh_vb, 0, 0), 4), \
            OBFH_V_ENCODE_RAW(OBFH_V_PACKED_INSTRUCTION(OBFH_V_TEST, 0, __obfh_va, 0, 0), 5), \
            OBFH_V_ENCODE_RAW(OBFH_V_PACKED_INSTRUCTION(OBFH_V_JFLAG, 0, 1, 0, 9), 6), \
            OBFH_V_ENCODE_RAW(OBFH_V_PACKED_INSTRUCTION(OBFH_V_MOVE, __obfh_vd, __obfh_va, 0, 0), 7), \
            OBFH_V_ENCODE_RAW(OBFH_V_PACKED_INSTRUCTION(OBFH_V_JUMP, 0, 0, 0, 1), 8), \
            OBFH_V_ENCODE_RAW(OBFH_V_PACKED_INSTRUCTION(OBFH_V_SWAP, 0, __obfh_va, __obfh_vb, 0), 9), \
            OBFH_V_ENCODE_RAW(OBFH_V_PACKED_INSTRUCTION(OBFH_V_SWAP, 0, __obfh_va, __obfh_vb, 0), 10), \
            OBFH_V_ENCODE_RAW(OBFH_V_PACKED_INSTRUCTION(OBFH_V_JUMP, 0, 0, 0, 1), 11) \
    }
#define OBFH_V_PROGRAM_5 \
    (const unsigned int[]) { \
        OBFH_V_ENCODE_RAW(OBFH_V_PACKED_INSTRUCTION(OBFH_V_LOAD_PAIR, __obfh_va, __obfh_vb, 0, 0), 0), \
            OBFH_V_ENCODE_RAW(OBFH_V_PACKED_INSTRUCTION(OBFH_V_SWAP, 0, __obfh_va, __obfh_vb, 0), 1), \
            OBFH_V_ENCODE_RAW(OBFH_V_PACKED_INSTRUCTION(OBFH_V_TEST, 0, __obfh_vb, 0, 0), 2), \
            OBFH_V_ENCODE_RAW(OBFH_V_PACKED_INSTRUCTION(OBFH_V_JFLAG, 0, 1, 0, 7), 3), \
            OBFH_V_ENCODE_RAW(OBFH_V_PACKED_OPERATION_0(__obfh_vc, __obfh_vb, __obfh_va), 4), \
            OBFH_V_ENCODE_RAW(OBFH_V_PACKED_OPERATION_1(__obfh_vc, __obfh_vb), 5), \
            OBFH_V_ENCODE_RAW(OBFH_V_PACKED_INSTRUCTION(OBFH_V_JUMP, 0, 0, 0, 10), 6), \
            OBFH_V_ENCODE_RAW(OBFH_V_PACKED_OPERATION_0(__obfh_vd, __obfh_vb, __obfh_va), 7), \
            OBFH_V_ENCODE_RAW(OBFH_V_PACKED_OPERATION_1(__obfh_vd, __obfh_vb), 8), \
            OBFH_V_ENCODE_RAW(OBFH_V_PACKED_INSTRUCTION(OBFH_V_MOVE, __obfh_vc, __obfh_vd, 0, 0), 9), \
            OBFH_V_ENCODE_RAW(OBFH_V_PACKED_INSTRUCTION(OBFH_V_RETURN, 0, __obfh_vc, 0, 0), 10) \
    }
#define OBFH_V_PROGRAM_6 \
    (const unsigned int[]) { \
        OBFH_V_ENCODE_RAW(OBFH_V_PACKED_INSTRUCTION(OBFH_V_LOAD_PAIR, __obfh_va, __obfh_vb, 0, 0), 0), \
            OBFH_V_ENCODE_RAW(OBFH_V_PACKED_INSTRUCTION(OBFH_V_CADD, 0, 0, 0, 2), 1), \
            OBFH_V_ENCODE_RAW(OBFH_V_PACKED_INSTRUCTION(OBFH_V_CADD, 0, 0, 0, 262143), 2), \
            OBFH_V_ENCODE_RAW(OBFH_V_PACKED_INSTRUCTION(OBFH_V_JFLAG, 1, 1, 0, 2), 3), \
            OBFH_V_ENCODE_RAW(OBFH_V_PACKED_INSTRUCTION(OBFH_V_MOVE, __obfh_vd, __obfh_va, 0, 0), 4), \
            OBFH_V_ENCODE_RAW(OBFH_V_PACKED_OPERATION_0(__obfh_vc, __obfh_vd, __obfh_vb), 5), \
            OBFH_V_ENCODE_RAW(OBFH_V_PACKED_OPERATION_1(__obfh_vc, __obfh_vd), 6), \
            OBFH_V_ENCODE_RAW(OBFH_V_PACKED_INSTRUCTION(OBFH_V_RETURN, 0, __obfh_vc, 0, 0), 7) \
    }
#define OBFH_V_PROGRAM_7 \
    (const unsigned int[]) { \
        OBFH_V_ENCODE_RAW(OBFH_V_PACKED_INSTRUCTION(OBFH_V_LOAD_PAIR, __obfh_va, __obfh_vb, 0, 0), 0), \
            OBFH_V_ENCODE_RAW(OBFH_V_PACKED_INSTRUCTION(OBFH_V_TEST, 0, __obfh_va, 0, 0), 1), \
            OBFH_V_ENCODE_RAW(OBFH_V_PACKED_INSTRUCTION(OBFH_V_JFLAG, 0, 1, 0, 6), 2), \
            OBFH_V_ENCODE_RAW(__obfh_vprep0, 3), \
            OBFH_V_ENCODE_RAW(__obfh_vprep1, 4), \
            OBFH_V_ENCODE_RAW(OBFH_V_PACKED_INSTRUCTION(OBFH_V_JUMP, 0, 0, 0, 8), 5), \
            OBFH_V_ENCODE_RAW(OBFH_V_PACKED_INSTRUCTION(OBFH_V_MOVE, __obfh_vd, __obfh_va, 0, 0), 6), \
            OBFH_V_ENCODE_RAW(OBFH_V_PACKED_INSTRUCTION(OBFH_V_CXOR, 1, 0, 0, 57), 7), \
            OBFH_V_ENCODE_RAW(OBFH_V_PACKED_OPERATION_0(__obfh_vc, __obfh_vd, __obfh_vb), 8), \
            OBFH_V_ENCODE_RAW(OBFH_V_PACKED_OPERATION_1(__obfh_vc, __obfh_vd), 9), \
            OBFH_V_ENCODE_RAW(OBFH_V_PACKED_INSTRUCTION(OBFH_V_TEST, 0, __obfh_vc, 0, 0), 10), \
            OBFH_V_ENCODE_RAW(OBFH_V_PACKED_INSTRUCTION(OBFH_V_JFLAG, 0, 1, 0, 14), 11), \
            OBFH_V_ENCODE_RAW(OBFH_V_PACKED_INSTRUCTION(OBFH_V_MOVE, __obfh_va, __obfh_vc, 0, 0), 12), \
            OBFH_V_ENCODE_RAW(OBFH_V_PACKED_INSTRUCTION(OBFH_V_RETURN, 0, __obfh_va, 0, 0), 13), \
            OBFH_V_ENCODE_RAW(OBFH_V_PACKED_INSTRUCTION(OBFH_V_RETURN, 0, __obfh_vc, 0, 0), 14) \
    }
#define OBFH_V_PROGRAM \
    __builtin_choose_expr( \
        __obfh_vvariant == 0, OBFH_V_PROGRAM_0, \
        __builtin_choose_expr( \
            __obfh_vvariant == 1, OBFH_V_PROGRAM_1, \
            __builtin_choose_expr( \
                __obfh_vvariant == 2, OBFH_V_PROGRAM_2, \
                __builtin_choose_expr( \
                    __obfh_vvariant == 3, OBFH_V_PROGRAM_3, \
                    __builtin_choose_expr( \
                        __obfh_vvariant == 4, OBFH_V_PROGRAM_4, \
                        __builtin_choose_expr(__obfh_vvariant == 5, OBFH_V_PROGRAM_5, \
                                              __builtin_choose_expr(__obfh_vvariant == 6, OBFH_V_PROGRAM_6, OBFH_V_PROGRAM_7)))))))
#define OBFH_VM_EXEC(executor, operation, value_a, value_b, floating) \
    ({ \
        enum { \
            __obfh_vsite = __COUNTER__, \
            __obfh_voperation = (operation), \
            __obfh_vkey = OBFH_MIX_A(OBFH_JUNK_WORD ^ __obfh_vsite ^ (unsigned int)OBFH_SEED), \
            __obfh_vvariant = (__obfh_vkey >> 24) & 7u, \
            __obfh_va = __obfh_vkey & 3u, \
            __obfh_vb_offset = 1u + ((__obfh_vkey >> 3) % 3u), \
            __obfh_vc_pick = 1u + ((__obfh_vkey >> 5) & 1u), \
            __obfh_vc_offset = __obfh_vc_pick + (__obfh_vc_pick >= __obfh_vb_offset), \
            __obfh_vb = (__obfh_va + __obfh_vb_offset) & 3u, \
            __obfh_vc = (__obfh_va + __obfh_vc_offset) & 3u, \
            __obfh_vd = (__obfh_va + 6u - __obfh_vb_offset - __obfh_vc_offset) & 3u \
        }; \
        OBFH_V_PROGRAM_SETUP; \
        const unsigned int *__obfh_vprogram = OBFH_V_PROGRAM; \
        enum { \
            __obfh_vlength = __obfh_vvariant == 0 ? 4 : __obfh_vvariant == 1 ? 6 \
                                                    : __obfh_vvariant == 2   ? 11 \
                                                    : __obfh_vvariant == 3   ? 10 \
                                                    : __obfh_vvariant == 4   ? 12 \
                                                    : __obfh_vvariant == 5   ? 11 \
                                                    : __obfh_vvariant == 6   ? 8 \
                                                                             : 15 \
        }; \
        executor(__obfh_vprogram, __obfh_vlength, __obfh_vkey, OBFH_VM_OPERAND(value_a, SALT_NUM1, floating), \
                 OBFH_VM_OPERAND(value_b, SALT_NUM2, floating)); \
    })
#define OBFH_VM_OPERAND(value, salt, floating) obfh_vm_encode(value, salt, (floating) | (RND(1, 2147483647u) << 1))
// Bitwise operations use uint32 bit patterns; shift counts wrap modulo 32.
#define VM_AND(a, b) (unsigned int)OBFH_VM_EXEC(Obfh_VirtualMachine, OBFH_VOP_BAND, (long double)(unsigned int)(a), (long double)(unsigned int)(b), 0u)
#define VM_OR(a, b) (unsigned int)OBFH_VM_EXEC(Obfh_VirtualMachine, OBFH_VOP_BOR, (long double)(unsigned int)(a), (long double)(unsigned int)(b), 0u)
#define VM_XOR(a, b) (unsigned int)OBFH_VM_EXEC(Obfh_VirtualMachine, OBFH_VOP_BXOR, (long double)(unsigned int)(a), (long double)(unsigned int)(b), 0u)
#define VM_NOT(value) (unsigned int)OBFH_VM_EXEC(Obfh_VirtualMachine, OBFH_VOP_BNOT, (long double)(unsigned int)(value), (long double)0, 0u)
#define VM_SHL(a, b) (unsigned int)OBFH_VM_EXEC(Obfh_VirtualMachine, OBFH_VOP_BSHL, (long double)(unsigned int)(a), (long double)(unsigned int)(b), 0u)
#define VM_SHR(a, b) (unsigned int)OBFH_VM_EXEC(Obfh_VirtualMachine, OBFH_VOP_BSHR, (long double)(unsigned int)(a), (long double)(unsigned int)(b), 0u)
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

// #endregion

// #region 15. Bootstrap console/module calls and PE export lookup
// ============================================================================
// 15. Bootstrap console/module calls and PE export lookup
// ============================================================================

// Caller-owned storage keeps the mask valid and avoids shared-buffer races.

// Bootstrap calls below are defined before public API interception.
// They cannot use the lazy API cache while that cache is resolving its loader.
// WriteConsoleA
static inline BOOL obfh_bootstrap_write_console(HANDLE hConsoleOutput, const void *lpBuffer, DWORD nNumberOfCharsToWrite, LPDWORD lpNumberOfCharsWritten, LPVOID lpReserved) OBFH_CODE_SECTION_ATTRIBUTE {
    BREAK_STACK_CFLOW;
    PHANTOM_NOP;
    return WriteConsoleA(hConsoleOutput, lpBuffer, nNumberOfCharsToWrite, lpNumberOfCharsWritten, lpReserved);
}
#define WriteConsoleA(...) obfh_bootstrap_write_console(__VA_ARGS__)

static inline HANDLE obfh_bootstrap_std_handle(DWORD nStdHandle) OBFH_CODE_SECTION_ATTRIBUTE {
    BREAK_STACK_CFLOW;
    PHANTOM_NOP;
    return GetStdHandle(obfh_int_proxy(nStdHandle));
}
#define GetStdHandle(...) obfh_bootstrap_std_handle(__VA_ARGS__)

static inline HMODULE obfh_bootstrap_module(LPCSTR lpModuleName) OBFH_CODE_SECTION_ATTRIBUTE {
    BREAK_STACK_CFLOW;
    PHANTOM_NOP;
    return GetModuleHandleA(lpModuleName);
}
#define GetModuleHandleA(...) obfh_bootstrap_module(__VA_ARGS__)

// Forward declaration for forwarded-export module loading.
static inline HMODULE LoadLibraryA_proxy(LPCSTR lpLibFileName);

// Bounded string scan used by the export parser.
static inline const char *obfh_find_zero(const void *buffer, size_t count) {
    OBFH_CUSTOM_BEGIN;
    OBFH_CUSTOM_NOP;
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
static inline int obfh_image_range(DWORD size, DWORD rva, size_t length) {
    OBFH_CUSTOM_BEGIN;
    OBFH_CUSTOM_NOP;
    PHANTOM_NOP;
    return rva <= size && length <= (size_t)(size - rva);
}
// GetProcAddress: custom PE export lookup, including ordinals and forwarders.
// Windows may bind a forwarded EAT entry to code outside the source image.
// Accept it only from a loader-owned module and an executable image page.
static inline FARPROC obfh_bound_export(HMODULE source, DWORD rva) {
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

static inline FARPROC obfh_find_export(HMODULE hModule, LPCSTR lpProcName, unsigned int depth) OBFH_CODE_SECTION_ATTRIBUTE {
    OBFH_CUSTOM_BEGIN;
    OBFH_CUSTOM_NOP;
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
        // PE export names are sorted lexicographically. Validate each visited name
        // before comparing; subtraction keeps the midpoint safe from overflow.
        DWORD low = 0, high = table->NumberOfNames;
        while (low < high) {
            DWORD middle = low + (high - low) / 2;
            DWORD name_rva = names[middle];
            if (!obfh_image_range(size, name_rva, 1) || !obfh_find_zero(base + name_rva, size - name_rva))
                return NULL;
            int order = strcmp(lpProcName, (const char *)(base + name_rva));
            if (order == 0) {
                index = ordinals[middle];
                break;
            }
            if (order < 0)
                high = middle;
            else
                low = middle + 1;
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
#define GetProcAddress(...) ({ \
    __label__ __obfh_getprocaddress_done; \
    OBFH_CUSTOM_BEGIN; \
    OBFH_CUSTOM_NOP; \
    struct { \
        HMODULE hModule; \
        LPCSTR lpProcName; \
    } __obfh_getprocaddress_args = {__VA_ARGS__}; \
    OBFH_CUSTOM_NOP; \
    DWORD __obfh_getprocaddress_error = GetLastError(); \
    FARPROC __obfh_getprocaddress_output; \
    FARPROC __obfh_getprocaddress_result = obfh_find_export(__obfh_getprocaddress_args.hModule, __obfh_getprocaddress_args.lpProcName, 0); \
    SetLastError(__obfh_getprocaddress_result ? __obfh_getprocaddress_error \
                                              : (__obfh_getprocaddress_args.hModule && __obfh_getprocaddress_args.lpProcName ? ERROR_PROC_NOT_FOUND \
                                                                                                                             : __obfh_getprocaddress_error)); \
    OBFH_CUSTOM_NOP; \
    BREAK_STACK_CFLOW; \
    { \
        __obfh_getprocaddress_output = (__obfh_getprocaddress_result); \
        goto __obfh_getprocaddress_done; \
    } \
__obfh_getprocaddress_done: \
    __obfh_getprocaddress_output; \
})

// #endregion

// #region 16. Module loading and loader proxy chain
// ============================================================================
// 16. Module loading and loader proxy chain
// ============================================================================

#if OBFH_DISABLE_CONTROL_FLOW != 1
#undef if
#define if(...) if (OBFH_HELPER_CONDITION((__VA_ARGS__)))
#undef OBFH_FLOW_ELSE_GUARD
#define OBFH_FLOW_ELSE_GUARD OBFH_HELPER_ELSE_GUARD
#endif

// LoadLibraryA: dynamic loader resolution and proxy chain.
static inline HMODULE obfh_load_library_core(LPCSTR lpLibFileName) OBFH_CODE_SECTION_ATTRIBUTE {
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
                char libName[9], funcName[13];
                getKernel32Name_proxy(libName);
                HMODULE kernel = GetModuleHandleA(libName);
                if (!kernel)
                    return NULL;
                obfh_junk_func_args(_0 + RND(1, 5));
                BREAK_STACK_CFLOW;
                getLoaderName_proxy(funcName);
                loader = (LoadLibraryAFunc)GetProcAddress(kernel, funcName);
                if (loader)
                    InterlockedCompareExchangePointer(&cachedLoader, (PVOID)loader, NULL);
            }
#if OBFH_MEMORY_CLEANER
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

static inline HMODULE LoadLibraryA_proxy(LPCSTR lpLibFileName) {
    // Two native hops; both the argument and result use full-width, randomized
    // address transport instead of five repeated forwarding wrappers.
    HMODULE result = obfh_load_library_core(OBFH_PTR(LPCSTR, lpLibFileName));
    RET_BY_VAR(result);
}
#define LoadLibraryA(...) LoadLibraryA_proxy(__VA_ARGS__)

#if OBFH_DISABLE_CONTROL_FLOW != 1
#undef if
#define if(...) if (OBFH_PUBLIC_IF_CONDITION(__VA_ARGS__))
#undef OBFH_FLOW_ELSE_GUARD
#define OBFH_FLOW_ELSE_GUARD OBFH_PUBLIC_ELSE_GUARD
#endif

// #endregion

// #region 17. Anti-debug probes and response routing
// ============================================================================
// 17. Anti-debug probes and response routing
// ============================================================================
// Probes collect evidence; response routing is separate from the call site.
#if OBFH_DISABLE_ANTIDEBUG != 1

#if OBFH_BETTER_ANTIDEBUG == 1
// Both worker paths inspect the same registers and always resume after suspension.
// Arguments are stable local expressions; ownership stays with each caller.
#define OBFH_AD_READ_REGISTERS(thread, detected) \
    { \
        if (SuspendThread(thread) != (DWORD)-1) { \
            CONTEXT context = {0}; \
            context.ContextFlags = CONTEXT_DEBUG_REGISTERS; \
            if (GetThreadContext(thread, &context)) { \
                /* Only enabled local/global breakpoints count; DR6 is not evidence. */ \
                (detected) = (context.Dr7 & 0xffu) != 0; \
            } \
            ResumeThread(thread); \
        } \
    }
// The worker owns the duplicated handle, including after a failed/timed-out wait.
static inline DWORD WINAPI obfh_ad_register_worker(void *argument) {
    HANDLE thread = (HANDLE)argument;
    DWORD detected = 0;
    BREAK_STACK_CFLOW;
    PHANTOM_NOP;
    OBFH_AD_READ_REGISTERS(thread, detected);
    CloseHandle(thread);
    return detected;
}

static inline int obfh_ad_register_oneshot(void) {
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

// Managed callbacks reuse Windows workers; each request still owns a fresh context probe.
// Opaque callback types keep compatibility with TinyCC's older Windows SDK declarations.
typedef void(WINAPI *OBFH_AD_CALLBACK)(void *, void *);
typedef BOOL(WINAPI *OBFH_AD_SUBMIT)(OBFH_AD_CALLBACK, void *, void *);
typedef void(WINAPI *OBFH_AD_MODULE_RELEASE)(void *, HMODULE);
typedef struct {
    HANDLE target, completed;
    HMODULE module;
    OBFH_AD_MODULE_RELEASE release_module;
    LONG volatile references, ready;
    DWORD detected;
} OBFH_AD_REQUEST;

static inline void obfh_ad_request_release(OBFH_AD_REQUEST *request) {
    if (InterlockedDecrement(&request->references) == 0) {
        CloseHandle(request->target);
        CloseHandle(request->completed);
        HeapFree(GetProcessHeap(), 0, request);
    }
}

static inline void WINAPI obfh_ad_register_callback(void *instance, void *argument) {
    OBFH_AD_REQUEST *request = (OBFH_AD_REQUEST *)argument;
    // The OS releases the module after returning from its code, including on timeout.
    request->release_module(instance, request->module);
    DWORD detected = 0;
    BREAK_STACK_CFLOW;
    OBFH_AD_READ_REGISTERS(request->target, detected);
    request->detected = detected;
    InterlockedExchange(&request->ready, 1);
    SetEvent(request->completed);
    obfh_ad_request_release(request);
}

static inline int obfh_ad_pool_apis(OBFH_AD_SUBMIT *submit, OBFH_AD_MODULE_RELEASE *release) {
    static struct {
        PVOID volatile submit, release;
        LONG volatile ready;
    } cached;
    enum { __obfh_pool_key = RND(1, 2147483646),
           __obfh_pool_bias = RND(1, 2147483646) };
    ULONG_PTR key = (ULONG_PTR)&cached ^ (ULONG_PTR)__obfh_pool_key;
    if (!OBFH_LOADER_READ(cached.ready)) {
        char library[9];
        HMODULE kernel = GetModuleHandleA(getKernel32Name_proxy(library));
        FARPROC send = kernel ? GetProcAddress(kernel, HIDE_STRING("TrySubmitThreadpoolCallback")) : NULL;
        FARPROC drop = kernel ? GetProcAddress(kernel, HIDE_STRING("FreeLibraryWhenCallbackReturns")) : NULL;
        if (!send || !drop)
            return 0;
#if (OBFH_SEED & 2u)
        InterlockedCompareExchangePointer(&cached.submit, (PVOID)(((ULONG_PTR)send + __obfh_pool_bias) ^ key), NULL);
        InterlockedCompareExchangePointer(&cached.release, (PVOID)(((ULONG_PTR)drop + __obfh_pool_bias) ^ key), NULL);
#else
        InterlockedCompareExchangePointer(&cached.submit, (PVOID)(((ULONG_PTR)send ^ key) + __obfh_pool_bias), NULL);
        InterlockedCompareExchangePointer(&cached.release, (PVOID)(((ULONG_PTR)drop ^ key) + __obfh_pool_bias), NULL);
#endif
        InterlockedExchange(&cached.ready, 1);
    }
#if (OBFH_SEED & 2u)
    *submit = (OBFH_AD_SUBMIT)(((ULONG_PTR)cached.submit ^ key) - __obfh_pool_bias);
    *release = (OBFH_AD_MODULE_RELEASE)(((ULONG_PTR)cached.release ^ key) - __obfh_pool_bias);
#else
    *submit = (OBFH_AD_SUBMIT)(((ULONG_PTR)cached.submit - __obfh_pool_bias) ^ key);
    *release = (OBFH_AD_MODULE_RELEASE)(((ULONG_PTR)cached.release - __obfh_pool_bias) ^ key);
#endif
    return 1;
}

static inline int obfh_ad_register_probe(void) {
    OBFH_AD_SUBMIT submit;
    OBFH_AD_MODULE_RELEASE release;
    if (!obfh_ad_pool_apis(&submit, &release))
        return obfh_ad_register_oneshot();
    OBFH_AD_REQUEST *request = (OBFH_AD_REQUEST *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(*request));
    if (!request)
        return obfh_ad_register_oneshot();
    request->completed = CreateEventA(NULL, TRUE, FALSE, NULL);
    if (!request->completed ||
        !DuplicateHandle(GetCurrentProcess(), GetCurrentThread(), GetCurrentProcess(), &request->target, 0, FALSE, DUPLICATE_SAME_ACCESS) ||
        !GetModuleHandleExA(4u, (const char *)obfh_ad_register_callback, &request->module)) {
        if (request->target) CloseHandle(request->target);
        if (request->completed) CloseHandle(request->completed);
        HeapFree(GetProcessHeap(), 0, request);
        return obfh_ad_register_oneshot();
    }
    request->release_module = release;
    request->references = 2;
    if (!submit(obfh_ad_register_callback, request, NULL)) {
        FreeLibrary(request->module);
        request->references = 1;
        obfh_ad_request_release(request);
        return obfh_ad_register_oneshot();
    }
    WaitForSingleObject(request->completed, 5000);
    int ready = OBFH_LOADER_READ(request->ready) != 0;
    int detected = ready && request->detected != 0;
    // A late callback owns the second reference; no stack pointers outlive this call.
    obfh_ad_request_release(request);
    // Pool starvation or a broken event must not silently omit the hardware probe.
    if (!ready)
        return obfh_ad_register_oneshot();
    return detected;
}
#endif

static inline int obfh_ad_process_probe(void) {
    typedef BOOL(WINAPI * ObfhDebuggerCheck)(void);
    static struct {
        PVOID volatile encoded;
        LONG volatile ready;
    } cached;
    enum { __obfh_ad_key = RND(1, 2147483646),
           __obfh_ad_bias = RND(1, 2147483646) };
    ULONG_PTR key = (ULONG_PTR)&cached ^ (ULONG_PTR)__obfh_ad_key;
    ObfhDebuggerCheck check;
    BREAK_STACK_CFLOW;
    // Cache the encoded address, never the debugger status. Kernel32 remains
    // loaded for the process lifetime; every probe still invokes the real API.
    if (OBFH_LOADER_READ(cached.ready)) {
#if (OBFH_SEED & 4u)
        check = (ObfhDebuggerCheck)(((ULONG_PTR)cached.encoded ^ key) - (ULONG_PTR)__obfh_ad_bias);
#else
        check = (ObfhDebuggerCheck)(((ULONG_PTR)cached.encoded - (ULONG_PTR)__obfh_ad_bias) ^ key);
#endif
    } else {
        // Caller-owned names remain alive through the export lookup.
        char library[9], symbol[18];
        HMODULE kernel = GetModuleHandleA(getKernel32Name_proxy(library));
        BREAK_STACK_CFLOW;
        check = kernel ? (ObfhDebuggerCheck)GetProcAddress(kernel, getDebuggerName_proxy(symbol)) : NULL;
        if (check) {
#if (OBFH_SEED & 4u)
            ULONG_PTR encoded = ((ULONG_PTR)check + (ULONG_PTR)__obfh_ad_bias) ^ key;
#else
            ULONG_PTR encoded = ((ULONG_PTR)check ^ key) + (ULONG_PTR)__obfh_ad_bias;
#endif
            InterlockedCompareExchangePointer(&cached.encoded, (PVOID)encoded, NULL);
            InterlockedExchange(&cached.ready, 1);
        }
    }
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

static inline int IsDebuggerPresent_proxy(void) OBFH_CODE_SECTION_ATTRIBUTE {
    int detected = obfh_ad_process_probe();
    BREAK_STACK_CFLOW;
    PHANTOM_NOP;
#if OBFH_BETTER_ANTIDEBUG == 1
    if (!detected)
        detected = obfh_ad_register_probe();
#endif
    return detected;
}

// Live paths use defined unsigned arithmetic and leave SP untouched. Random
// instructions come from the shared pool and reside on its skipped paths.
static inline void obfh_ad_sink_a(unsigned int initial) OBFH_CODE_SECTION_ATTRIBUTE {
    volatile unsigned int state = initial | 1u;
    PHANTOM_NOP;
    for (;;) {
        BREAK_STACK_CFLOW;
        unsigned int next = state ^ RND(1, 2147483646);
        state = ((next << 7) | (next >> 25)) + RND(1, 65535);
    }
}

static inline void obfh_ad_sink_b(unsigned int initial) OBFH_CODE_SECTION_ATTRIBUTE {
    volatile unsigned int state = initial;
    PHANTOM_NOP;
    for (;;) {
        state = state * (RND(1, 32767) * 2u + 1u) + RND(1, 65535);
        BREAK_STACK_CFLOW;
        state ^= state >> 13;
    }
}

static inline void obfh_ad_react(unsigned int nonce, unsigned int route) OBFH_CODE_SECTION_ATTRIBUTE {
    typedef void (*ObfhResponse)(unsigned int);
    ObfhResponse volatile response = (route & 1u) ? obfh_ad_sink_a : obfh_ad_sink_b;
    PHANTOM_NOP;
    BREAK_STACK_CFLOW;
    response(nonce ^ RND(1, 2147483646));
}

#define ANTI_DEBUG \
    do { \
        BREAK_STACK_CFLOW; \
        if (IsDebuggerPresent_proxy()) \
            obfh_ad_react(RND(1, 2147483646), RND(0, 1)); \
    } while (0)
#else
#define ANTI_DEBUG \
    do { \
    } while (0)
#endif

// #endregion

// #region 18. CRT resolution, formatted output and compact call adapters
// ============================================================================
// 18. CRT resolution, formatted output and compact call adapters
// ============================================================================

// Bounded CRT module name in caller-owned storage.
static inline char *getStdLibName_proxy(char *name, size_t capacity) {
    BREAK_STACK_CFLOW;
    if (!name || capacity < sizeof("msvcrt"))
        return NULL;
    OBFH_HIDE_JUNK;
    OBFH_LOADER_FOR(OBFH_ALPHA_LOW | OBFH_ALPHA_HIGH);
    // Three independently ordered pairs and group reversal: 16 store orders.
    enum { OBFH_MIX_FIELDS(name_order, OBFH_SEED ^ 0x91e10da5u) };
#define OBFH_CRT_NAME_PAIR(bit, a, x, b, y) \
    __builtin_choose_expr(name_order & (bit), \
                          (name[OBFH_NAME_INDEX(a)] = (x), name[OBFH_NAME_INDEX(b)] = (y)), \
                          (name[OBFH_NAME_INDEX(b)] = (y), name[OBFH_NAME_INDEX(a)] = (x)))
    __builtin_choose_expr(name_order & 8u,
                          (OBFH_CRT_NAME_PAIR(1u, 0, _m, 5, _t), OBFH_CRT_NAME_PAIR(2u, 1, _s, 4, _r), OBFH_CRT_NAME_PAIR(4u, 2, _v, 3, _c)),
                          (OBFH_CRT_NAME_PAIR(4u, 2, _v, 3, _c), OBFH_CRT_NAME_PAIR(2u, 1, _s, 4, _r), OBFH_CRT_NAME_PAIR(1u, 0, _m, 5, _t)));
#undef OBFH_CRT_NAME_PAIR
    name[OBFH_NAME_INDEX(6)] = _0;
    PHANTOM_NOP;
    return name;
}

// Resolve through the custom loader/export chain once, then keep its DLL alive.
static inline FARPROC obfh_crt_lookup(const char *name) {
    BREAK_STACK_CFLOW;
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
    return function;
}

// A count conversion writes to user memory and must not run in a sizing pass.
static inline int obfh_format_has_count(const char *format) OBFH_CODE_SECTION_ATTRIBUTE {
    OBFH_LOADER_FOR(OBFH_ALPHA_HIGH);
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
static inline int obfh_printf_variadic(int junk, const char *format, ...) {
    OBFH_CUSTOM_BEGIN;
    OBFH_CUSTOM_NOP;
    BREAK_STACK_CFLOW;
    va_list args;
    obfh_junk_func_args(RND(0, 1000) + junk);
    HANDLE console = (HANDLE)obfh_uintptr_proxy((ULONG_PTR)GetStdHandle(obfh_int_proxy(STD_OUTPUT_HANDLE)));
    obfh_junk_func_args((int)((ULONG_PTR)console & 0x3fffffff) + junk);
    va_start(args, format);
    char functionName[8];
    OBFH_NAME_ORDER_FOR(OBFH_ALPHA_LOW | OBFH_ALPHA_HIGH,
                        (OBFH_NAME_PAIR(1, functionName[OBFH_NAME_INDEX(0)] = _v, functionName[OBFH_NAME_INDEX(1)] = _p), OBFH_NAME_PAIR(2, functionName[OBFH_NAME_INDEX(2)] = _r, functionName[OBFH_NAME_INDEX(3)] = _i), OBFH_NAME_PAIR(3, functionName[OBFH_NAME_INDEX(4)] = _n, functionName[OBFH_NAME_INDEX(5)] = _t), OBFH_NAME_PAIR(4, functionName[OBFH_NAME_INDEX(6)] = _f, functionName[OBFH_NAME_INDEX(7)] = _0)),
                        (OBFH_NAME_PAIR(1, functionName[OBFH_NAME_INDEX(7)] = _0, functionName[OBFH_NAME_INDEX(6)] = _f), OBFH_NAME_PAIR(2, functionName[OBFH_NAME_INDEX(5)] = _t, functionName[OBFH_NAME_INDEX(4)] = _n), OBFH_NAME_PAIR(3, functionName[OBFH_NAME_INDEX(3)] = _i, functionName[OBFH_NAME_INDEX(2)] = _r), OBFH_NAME_PAIR(4, functionName[OBFH_NAME_INDEX(1)] = _p, functionName[OBFH_NAME_INDEX(0)] = _v)));
    int result;
    DWORD mode;
    PHANTOM_NOP;
    if (GetConsoleMode(console, &mode) && !obfh_format_has_count(format)) {
        char countName[11];
        OBFH_NAME_ORDER_FOR(OBFH_ALPHA_LOW | OBFH_ALPHA_HIGH,
                            (countName[OBFH_NAME_INDEX(0)] = '_', countName[OBFH_NAME_INDEX(1)] = _v, countName[OBFH_NAME_INDEX(2)] = _s, countName[OBFH_NAME_INDEX(3)] = _c, countName[OBFH_NAME_INDEX(4)] = _p, countName[OBFH_NAME_INDEX(5)] = _r, countName[OBFH_NAME_INDEX(6)] = _i, countName[OBFH_NAME_INDEX(7)] = _n, countName[OBFH_NAME_INDEX(8)] = _t, countName[OBFH_NAME_INDEX(9)] = _f, countName[OBFH_NAME_INDEX(10)] = _0),
                            (countName[OBFH_NAME_INDEX(10)] = _0, countName[OBFH_NAME_INDEX(9)] = _f, countName[OBFH_NAME_INDEX(8)] = _t, countName[OBFH_NAME_INDEX(7)] = _n, countName[OBFH_NAME_INDEX(6)] = _i, countName[OBFH_NAME_INDEX(5)] = _r, countName[OBFH_NAME_INDEX(4)] = _p, countName[OBFH_NAME_INDEX(3)] = _c, countName[OBFH_NAME_INDEX(2)] = _s, countName[OBFH_NAME_INDEX(1)] = _v, countName[OBFH_NAME_INDEX(0)] = '_'));
        va_list countArgs;
        va_copy(countArgs, args);
        int length = OBFH_CRT_TARGET(int (*)(const char *, va_list), countName)(format, countArgs);
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
        result = OBFH_CRT_TARGET(int (*)(const char *, va_list), functionName)(format, args);
    }
    va_end(args);
    return result;
}
#define printf(...) \
    ({ \
        int __obfh_printf_result; \
        do { \
            BREAK_STACK_CFLOW; \
            obfh_junk_func_args((RND(0, 1000) * 3) < _0); \
            __obfh_printf_result = obfh_printf_variadic(RND(0, 1000), __VA_ARGS__); \
        } while (_0 > ((unsigned long long)RND(0, 100000000000) * (unsigned char)_2) + 82); \
        __obfh_printf_result; \
    })

// Reuse the CRT resolver without a formatting pass or an extra proxy function.
#define puts(string) ({ \
    const char *__obfh_puts_string = (string); \
    char __obfh_puts_name[5]; \
    OBFH_NAME_ORDER_FOR(OBFH_ALPHA_HIGH, \
                        (OBFH_NAME_PAIR(1, __obfh_puts_name[OBFH_NAME_INDEX(0)] = _p, __obfh_puts_name[OBFH_NAME_INDEX(1)] = _u), OBFH_NAME_PAIR(2, __obfh_puts_name[OBFH_NAME_INDEX(2)] = _t, __obfh_puts_name[OBFH_NAME_INDEX(3)] = _s), __obfh_puts_name[OBFH_NAME_INDEX(4)] = _0), \
                        (OBFH_NAME_PAIR(1, __obfh_puts_name[OBFH_NAME_INDEX(4)] = _0, __obfh_puts_name[OBFH_NAME_INDEX(3)] = _s), OBFH_NAME_PAIR(2, __obfh_puts_name[OBFH_NAME_INDEX(2)] = _t, __obfh_puts_name[OBFH_NAME_INDEX(1)] = _u), __obfh_puts_name[OBFH_NAME_INDEX(0)] = _p)); \
    OBFH_CRT_TARGET(int (*)(const char *), __obfh_puts_name) \
    (__obfh_puts_string); \
})

// Build the name in caller-owned storage, then invoke the resolved typed function.
#define OBFH_CRT_INVOKE(builder, type, entry, ...) ({ \
    char __obfh_crt_name[32]; \
    entry; \
    OBFH_CRT_TARGET(type, builder(__obfh_crt_name)) \
    (__VA_ARGS__); \
})
#define OBFH_CRT_CALL(builder, type, ...) \
    OBFH_CRT_INVOKE(builder, type, STACK_PROXY_FUNCTIONS, __VA_ARGS__)
#define OBFH_CRT_COMPACT_CALL(builder, type, ...) \
    OBFH_CRT_INVOKE(builder, type, ((void)0), __VA_ARGS__)

static inline char *getScanfName_proxy(char *name) {
    BREAK_STACK_CFLOW;
    OBFH_NAME_ORDER_FOR(OBFH_ALPHA_LOW | OBFH_ALPHA_HIGH,
                        (OBFH_NAME_PAIR(1, name[OBFH_NAME_INDEX(0)] = _s, name[OBFH_NAME_INDEX(1)] = _c), OBFH_NAME_PAIR(2, name[OBFH_NAME_INDEX(2)] = _a, name[OBFH_NAME_INDEX(3)] = _n), OBFH_NAME_PAIR(3, name[OBFH_NAME_INDEX(4)] = _f, name[OBFH_NAME_INDEX(5)] = _0)),
                        (OBFH_NAME_PAIR(1, name[OBFH_NAME_INDEX(5)] = _0, name[OBFH_NAME_INDEX(4)] = _f), OBFH_NAME_PAIR(2, name[OBFH_NAME_INDEX(3)] = _n, name[OBFH_NAME_INDEX(2)] = _a), OBFH_NAME_PAIR(3, name[OBFH_NAME_INDEX(1)] = _c, name[OBFH_NAME_INDEX(0)] = _s)));
    PHANTOM_NOP;
#if OBFH_BETTER_CONTROL_FLOW
    BREAK_STACK_CFLOW;
#endif
    return name;
}
#define scanf(...) OBFH_CRT_CALL(getScanfName_proxy, int (*)(const char *, ...), __VA_ARGS__)

// #endregion

// #region 19. CRT name builders and public function aliases
// ============================================================================
// 19. CRT name builders and public function aliases
// ============================================================================

#if OBFH_DISABLE_CONTROL_FLOW != 1
#undef if
#define if(...) if (OBFH_HELPER_CONDITION((__VA_ARGS__)))
#undef OBFH_FLOW_ELSE_GUARD
#define OBFH_FLOW_ELSE_GUARD OBFH_HELPER_ELSE_GUARD
#endif

static inline char *getFreeName_proxy(char *name) {
    OBFH_NAME_ORDER_FOR(OBFH_ALPHA_LOW | OBFH_ALPHA_HIGH,
                        (OBFH_NAME_PAIR(1, name[OBFH_NAME_INDEX(0)] = _f, name[OBFH_NAME_INDEX(1)] = _r), OBFH_NAME_PAIR(2, name[OBFH_NAME_INDEX(2)] = _e, name[OBFH_NAME_INDEX(3)] = _e), name[OBFH_NAME_INDEX(4)] = _0),
                        (OBFH_NAME_PAIR(1, name[OBFH_NAME_INDEX(4)] = _0, name[OBFH_NAME_INDEX(3)] = _e), OBFH_NAME_PAIR(2, name[OBFH_NAME_INDEX(2)] = _e, name[OBFH_NAME_INDEX(1)] = _r), name[OBFH_NAME_INDEX(0)] = _f));
    PHANTOM_NOP;
    return name;
}
#define free(...) OBFH_CRT_COMPACT_CALL(getFreeName_proxy, void (*)(void *), __VA_ARGS__)

static inline char *getAtofName_proxy(char *name) {
    OBFH_NAME_ORDER_FOR(OBFH_ALPHA_LOW | OBFH_ALPHA_HIGH,
                        (OBFH_NAME_PAIR(1, name[OBFH_NAME_INDEX(0)] = _a, name[OBFH_NAME_INDEX(1)] = _t), OBFH_NAME_PAIR(2, name[OBFH_NAME_INDEX(2)] = _o, name[OBFH_NAME_INDEX(3)] = _f), name[OBFH_NAME_INDEX(4)] = _0),
                        (OBFH_NAME_PAIR(1, name[OBFH_NAME_INDEX(4)] = _0, name[OBFH_NAME_INDEX(3)] = _f), OBFH_NAME_PAIR(2, name[OBFH_NAME_INDEX(2)] = _o, name[OBFH_NAME_INDEX(1)] = _t), name[OBFH_NAME_INDEX(0)] = _a));
    PHANTOM_NOP;
    return name;
}
#define atof(...) OBFH_CRT_COMPACT_CALL(getAtofName_proxy, double (*)(const char *), __VA_ARGS__)

static inline char *getStrtodName_proxy(char *name) {
    OBFH_NAME_ORDER_FOR(OBFH_ALPHA_LOW | OBFH_ALPHA_HIGH,
                        (OBFH_NAME_PAIR(1, name[OBFH_NAME_INDEX(0)] = _s, name[OBFH_NAME_INDEX(1)] = _t), OBFH_NAME_PAIR(2, name[OBFH_NAME_INDEX(2)] = _r, name[OBFH_NAME_INDEX(3)] = _t), OBFH_NAME_PAIR(3, name[OBFH_NAME_INDEX(4)] = _o, name[OBFH_NAME_INDEX(5)] = _d), name[OBFH_NAME_INDEX(6)] = _0),
                        (OBFH_NAME_PAIR(1, name[OBFH_NAME_INDEX(6)] = _0, name[OBFH_NAME_INDEX(5)] = _d), OBFH_NAME_PAIR(2, name[OBFH_NAME_INDEX(4)] = _o, name[OBFH_NAME_INDEX(3)] = _t), OBFH_NAME_PAIR(3, name[OBFH_NAME_INDEX(2)] = _r, name[OBFH_NAME_INDEX(1)] = _t), name[OBFH_NAME_INDEX(0)] = _s));
    PHANTOM_NOP;
    return name;
}
#define strtod(...) OBFH_CRT_COMPACT_CALL(getStrtodName_proxy, double (*)(const char *, char **), __VA_ARGS__)

static inline char *getSrandName_proxy(char *name) {
    OBFH_NAME_ORDER_FOR(OBFH_ALPHA_LOW | OBFH_ALPHA_HIGH,
                        (OBFH_NAME_PAIR(1, name[OBFH_NAME_INDEX(0)] = _s, name[OBFH_NAME_INDEX(1)] = _r), OBFH_NAME_PAIR(2, name[OBFH_NAME_INDEX(2)] = _a, name[OBFH_NAME_INDEX(3)] = _n), OBFH_NAME_PAIR(3, name[OBFH_NAME_INDEX(4)] = _d, name[OBFH_NAME_INDEX(5)] = _0)),
                        (OBFH_NAME_PAIR(1, name[OBFH_NAME_INDEX(5)] = _0, name[OBFH_NAME_INDEX(4)] = _d), OBFH_NAME_PAIR(2, name[OBFH_NAME_INDEX(3)] = _n, name[OBFH_NAME_INDEX(2)] = _a), OBFH_NAME_PAIR(3, name[OBFH_NAME_INDEX(1)] = _r, name[OBFH_NAME_INDEX(0)] = _s)));
    PHANTOM_NOP;
    return name;
}
#define srand(...) OBFH_CRT_COMPACT_CALL(getSrandName_proxy, void (*)(unsigned int), __VA_ARGS__)

static inline char *getFgetsName_proxy(char *name) {
    OBFH_NAME_ORDER_FOR(OBFH_ALPHA_LOW | OBFH_ALPHA_HIGH,
                        (OBFH_NAME_PAIR(1, name[OBFH_NAME_INDEX(0)] = _f, name[OBFH_NAME_INDEX(1)] = _g), OBFH_NAME_PAIR(2, name[OBFH_NAME_INDEX(2)] = _e, name[OBFH_NAME_INDEX(3)] = _t), OBFH_NAME_PAIR(3, name[OBFH_NAME_INDEX(4)] = _s, name[OBFH_NAME_INDEX(5)] = _0)),
                        (OBFH_NAME_PAIR(1, name[OBFH_NAME_INDEX(5)] = _0, name[OBFH_NAME_INDEX(4)] = _s), OBFH_NAME_PAIR(2, name[OBFH_NAME_INDEX(3)] = _t, name[OBFH_NAME_INDEX(2)] = _e), OBFH_NAME_PAIR(3, name[OBFH_NAME_INDEX(1)] = _g, name[OBFH_NAME_INDEX(0)] = _f)));
    PHANTOM_NOP;
    return name;
}
#define fgets(...) OBFH_CRT_COMPACT_CALL(getFgetsName_proxy, char *(*)(char *, int, FILE *), __VA_ARGS__)

static inline char *getFputsName_proxy(char *name) {
    OBFH_NAME_ORDER_FOR(OBFH_ALPHA_LOW | OBFH_ALPHA_HIGH,
                        (OBFH_NAME_PAIR(1, name[OBFH_NAME_INDEX(0)] = _f, name[OBFH_NAME_INDEX(1)] = _p), OBFH_NAME_PAIR(2, name[OBFH_NAME_INDEX(2)] = _u, name[OBFH_NAME_INDEX(3)] = _t), OBFH_NAME_PAIR(3, name[OBFH_NAME_INDEX(4)] = _s, name[OBFH_NAME_INDEX(5)] = _0)),
                        (OBFH_NAME_PAIR(1, name[OBFH_NAME_INDEX(5)] = _0, name[OBFH_NAME_INDEX(4)] = _s), OBFH_NAME_PAIR(2, name[OBFH_NAME_INDEX(3)] = _t, name[OBFH_NAME_INDEX(2)] = _u), OBFH_NAME_PAIR(3, name[OBFH_NAME_INDEX(1)] = _p, name[OBFH_NAME_INDEX(0)] = _f)));
    PHANTOM_NOP;
    return name;
}
#define fputs(...) OBFH_CRT_COMPACT_CALL(getFputsName_proxy, int (*)(const char *, FILE *), __VA_ARGS__)

static inline char *getFprintfName_proxy(char *name) {
    OBFH_NAME_ORDER_FOR(OBFH_ALPHA_LOW | OBFH_ALPHA_HIGH,
                        (OBFH_NAME_PAIR(1, name[OBFH_NAME_INDEX(0)] = _f, name[OBFH_NAME_INDEX(1)] = _p), OBFH_NAME_PAIR(2, name[OBFH_NAME_INDEX(2)] = _r, name[OBFH_NAME_INDEX(3)] = _i), OBFH_NAME_PAIR(3, name[OBFH_NAME_INDEX(4)] = _n, name[OBFH_NAME_INDEX(5)] = _t), OBFH_NAME_PAIR(4, name[OBFH_NAME_INDEX(6)] = _f, name[OBFH_NAME_INDEX(7)] = _0)),
                        (OBFH_NAME_PAIR(1, name[OBFH_NAME_INDEX(7)] = _0, name[OBFH_NAME_INDEX(6)] = _f), OBFH_NAME_PAIR(2, name[OBFH_NAME_INDEX(5)] = _t, name[OBFH_NAME_INDEX(4)] = _n), OBFH_NAME_PAIR(3, name[OBFH_NAME_INDEX(3)] = _i, name[OBFH_NAME_INDEX(2)] = _r), OBFH_NAME_PAIR(4, name[OBFH_NAME_INDEX(1)] = _p, name[OBFH_NAME_INDEX(0)] = _f)));
    PHANTOM_NOP;
    return name;
}
#define fprintf(...) OBFH_CRT_COMPACT_CALL(getFprintfName_proxy, int (*)(FILE *, const char *, ...), __VA_ARGS__)

static inline char *getFflushName_proxy(char *name) {
    OBFH_NAME_ORDER_FOR(OBFH_ALPHA_LOW | OBFH_ALPHA_HIGH,
                        (OBFH_NAME_PAIR(1, name[OBFH_NAME_INDEX(0)] = _f, name[OBFH_NAME_INDEX(1)] = _f), OBFH_NAME_PAIR(2, name[OBFH_NAME_INDEX(2)] = _l, name[OBFH_NAME_INDEX(3)] = _u), OBFH_NAME_PAIR(3, name[OBFH_NAME_INDEX(4)] = _s, name[OBFH_NAME_INDEX(5)] = _h), name[OBFH_NAME_INDEX(6)] = _0),
                        (OBFH_NAME_PAIR(1, name[OBFH_NAME_INDEX(6)] = _0, name[OBFH_NAME_INDEX(5)] = _h), OBFH_NAME_PAIR(2, name[OBFH_NAME_INDEX(4)] = _s, name[OBFH_NAME_INDEX(3)] = _u), OBFH_NAME_PAIR(3, name[OBFH_NAME_INDEX(2)] = _l, name[OBFH_NAME_INDEX(1)] = _f), name[OBFH_NAME_INDEX(0)] = _f));
    PHANTOM_NOP;
    return name;
}
#define fflush(...) OBFH_CRT_COMPACT_CALL(getFflushName_proxy, int (*)(FILE *), __VA_ARGS__)

static inline char *getFseekName_proxy(char *name) {
    OBFH_NAME_ORDER_FOR(OBFH_ALPHA_LOW | OBFH_ALPHA_HIGH,
                        (OBFH_NAME_PAIR(1, name[OBFH_NAME_INDEX(0)] = _f, name[OBFH_NAME_INDEX(1)] = _s), OBFH_NAME_PAIR(2, name[OBFH_NAME_INDEX(2)] = _e, name[OBFH_NAME_INDEX(3)] = _e), OBFH_NAME_PAIR(3, name[OBFH_NAME_INDEX(4)] = _k, name[OBFH_NAME_INDEX(5)] = _0)),
                        (OBFH_NAME_PAIR(1, name[OBFH_NAME_INDEX(5)] = _0, name[OBFH_NAME_INDEX(4)] = _k), OBFH_NAME_PAIR(2, name[OBFH_NAME_INDEX(3)] = _e, name[OBFH_NAME_INDEX(2)] = _e), OBFH_NAME_PAIR(3, name[OBFH_NAME_INDEX(1)] = _s, name[OBFH_NAME_INDEX(0)] = _f)));
    PHANTOM_NOP;
    return name;
}
#define fseek(...) OBFH_CRT_COMPACT_CALL(getFseekName_proxy, int (*)(FILE *, long, int), __VA_ARGS__)

static inline char *getFtellName_proxy(char *name) {
    OBFH_NAME_ORDER_FOR(OBFH_ALPHA_LOW | OBFH_ALPHA_HIGH,
                        (OBFH_NAME_PAIR(1, name[OBFH_NAME_INDEX(0)] = _f, name[OBFH_NAME_INDEX(1)] = _t), OBFH_NAME_PAIR(2, name[OBFH_NAME_INDEX(2)] = _e, name[OBFH_NAME_INDEX(3)] = _l), OBFH_NAME_PAIR(3, name[OBFH_NAME_INDEX(4)] = _l, name[OBFH_NAME_INDEX(5)] = _0)),
                        (OBFH_NAME_PAIR(1, name[OBFH_NAME_INDEX(5)] = _0, name[OBFH_NAME_INDEX(4)] = _l), OBFH_NAME_PAIR(2, name[OBFH_NAME_INDEX(3)] = _l, name[OBFH_NAME_INDEX(2)] = _e), OBFH_NAME_PAIR(3, name[OBFH_NAME_INDEX(1)] = _t, name[OBFH_NAME_INDEX(0)] = _f)));
    PHANTOM_NOP;
    return name;
}
#define ftell(...) OBFH_CRT_COMPACT_CALL(getFtellName_proxy, long (*)(FILE *), __VA_ARGS__)

static inline char *getFgetcName_proxy(char *name) {
    OBFH_NAME_ORDER_FOR(OBFH_ALPHA_LOW | OBFH_ALPHA_HIGH,
                        (OBFH_NAME_PAIR(1, name[OBFH_NAME_INDEX(0)] = _f, name[OBFH_NAME_INDEX(1)] = _g), OBFH_NAME_PAIR(2, name[OBFH_NAME_INDEX(2)] = _e, name[OBFH_NAME_INDEX(3)] = _t), OBFH_NAME_PAIR(3, name[OBFH_NAME_INDEX(4)] = _c, name[OBFH_NAME_INDEX(5)] = _0)),
                        (OBFH_NAME_PAIR(1, name[OBFH_NAME_INDEX(5)] = _0, name[OBFH_NAME_INDEX(4)] = _c), OBFH_NAME_PAIR(2, name[OBFH_NAME_INDEX(3)] = _t, name[OBFH_NAME_INDEX(2)] = _e), OBFH_NAME_PAIR(3, name[OBFH_NAME_INDEX(1)] = _g, name[OBFH_NAME_INDEX(0)] = _f)));
    PHANTOM_NOP;
    return name;
}
#define fgetc(...) OBFH_CRT_COMPACT_CALL(getFgetcName_proxy, int (*)(FILE *), __VA_ARGS__)

static inline char *getFputcName_proxy(char *name) {
    OBFH_NAME_ORDER_FOR(OBFH_ALPHA_LOW | OBFH_ALPHA_HIGH,
                        (OBFH_NAME_PAIR(1, name[OBFH_NAME_INDEX(0)] = _f, name[OBFH_NAME_INDEX(1)] = _p), OBFH_NAME_PAIR(2, name[OBFH_NAME_INDEX(2)] = _u, name[OBFH_NAME_INDEX(3)] = _t), OBFH_NAME_PAIR(3, name[OBFH_NAME_INDEX(4)] = _c, name[OBFH_NAME_INDEX(5)] = _0)),
                        (OBFH_NAME_PAIR(1, name[OBFH_NAME_INDEX(5)] = _0, name[OBFH_NAME_INDEX(4)] = _c), OBFH_NAME_PAIR(2, name[OBFH_NAME_INDEX(3)] = _t, name[OBFH_NAME_INDEX(2)] = _u), OBFH_NAME_PAIR(3, name[OBFH_NAME_INDEX(1)] = _p, name[OBFH_NAME_INDEX(0)] = _f)));
    PHANTOM_NOP;
    return name;
}
#define fputc(...) OBFH_CRT_COMPACT_CALL(getFputcName_proxy, int (*)(int, FILE *), __VA_ARGS__)

static inline char *getGetcharName_proxy(char *name) {
    OBFH_NAME_ORDER_FOR(OBFH_ALPHA_LOW | OBFH_ALPHA_HIGH,
                        (OBFH_NAME_PAIR(1, name[OBFH_NAME_INDEX(0)] = _g, name[OBFH_NAME_INDEX(1)] = _e), OBFH_NAME_PAIR(2, name[OBFH_NAME_INDEX(2)] = _t, name[OBFH_NAME_INDEX(3)] = _c), OBFH_NAME_PAIR(3, name[OBFH_NAME_INDEX(4)] = _h, name[OBFH_NAME_INDEX(5)] = _a), OBFH_NAME_PAIR(4, name[OBFH_NAME_INDEX(6)] = _r, name[OBFH_NAME_INDEX(7)] = _0)),
                        (OBFH_NAME_PAIR(1, name[OBFH_NAME_INDEX(7)] = _0, name[OBFH_NAME_INDEX(6)] = _r), OBFH_NAME_PAIR(2, name[OBFH_NAME_INDEX(5)] = _a, name[OBFH_NAME_INDEX(4)] = _h), OBFH_NAME_PAIR(3, name[OBFH_NAME_INDEX(3)] = _c, name[OBFH_NAME_INDEX(2)] = _t), OBFH_NAME_PAIR(4, name[OBFH_NAME_INDEX(1)] = _e, name[OBFH_NAME_INDEX(0)] = _g)));
    PHANTOM_NOP;
    return name;
}
#undef getchar
#define getchar(...) OBFH_CRT_COMPACT_CALL(getGetcharName_proxy, int (*)(void), __VA_ARGS__)

static inline char *getPutcharName_proxy(char *name) {
    OBFH_NAME_ORDER_FOR(OBFH_ALPHA_LOW | OBFH_ALPHA_HIGH,
                        (OBFH_NAME_PAIR(1, name[OBFH_NAME_INDEX(0)] = _p, name[OBFH_NAME_INDEX(1)] = _u), OBFH_NAME_PAIR(2, name[OBFH_NAME_INDEX(2)] = _t, name[OBFH_NAME_INDEX(3)] = _c), OBFH_NAME_PAIR(3, name[OBFH_NAME_INDEX(4)] = _h, name[OBFH_NAME_INDEX(5)] = _a), OBFH_NAME_PAIR(4, name[OBFH_NAME_INDEX(6)] = _r, name[OBFH_NAME_INDEX(7)] = _0)),
                        (OBFH_NAME_PAIR(1, name[OBFH_NAME_INDEX(7)] = _0, name[OBFH_NAME_INDEX(6)] = _r), OBFH_NAME_PAIR(2, name[OBFH_NAME_INDEX(5)] = _a, name[OBFH_NAME_INDEX(4)] = _h), OBFH_NAME_PAIR(3, name[OBFH_NAME_INDEX(3)] = _c, name[OBFH_NAME_INDEX(2)] = _t), OBFH_NAME_PAIR(4, name[OBFH_NAME_INDEX(1)] = _u, name[OBFH_NAME_INDEX(0)] = _p)));
    PHANTOM_NOP;
    return name;
}
#undef putchar
#define putchar(...) OBFH_CRT_COMPACT_CALL(getPutcharName_proxy, int (*)(int), __VA_ARGS__)

static inline char *getFeofName_proxy(char *name) {
    OBFH_NAME_ORDER_FOR(OBFH_ALPHA_LOW | OBFH_ALPHA_HIGH,
                        (OBFH_NAME_PAIR(1, name[OBFH_NAME_INDEX(0)] = _f, name[OBFH_NAME_INDEX(1)] = _e), OBFH_NAME_PAIR(2, name[OBFH_NAME_INDEX(2)] = _o, name[OBFH_NAME_INDEX(3)] = _f), name[OBFH_NAME_INDEX(4)] = _0),
                        (OBFH_NAME_PAIR(1, name[OBFH_NAME_INDEX(4)] = _0, name[OBFH_NAME_INDEX(3)] = _f), OBFH_NAME_PAIR(2, name[OBFH_NAME_INDEX(2)] = _o, name[OBFH_NAME_INDEX(1)] = _e), name[OBFH_NAME_INDEX(0)] = _f));
    PHANTOM_NOP;
    return name;
}
#undef feof
#define feof(...) OBFH_CRT_COMPACT_CALL(getFeofName_proxy, int (*)(FILE *), __VA_ARGS__)

static inline char *getFerrorName_proxy(char *name) {
    OBFH_NAME_ORDER_FOR(OBFH_ALPHA_LOW | OBFH_ALPHA_HIGH,
                        (OBFH_NAME_PAIR(1, name[OBFH_NAME_INDEX(0)] = _f, name[OBFH_NAME_INDEX(1)] = _e), OBFH_NAME_PAIR(2, name[OBFH_NAME_INDEX(2)] = _r, name[OBFH_NAME_INDEX(3)] = _r), OBFH_NAME_PAIR(3, name[OBFH_NAME_INDEX(4)] = _o, name[OBFH_NAME_INDEX(5)] = _r), name[OBFH_NAME_INDEX(6)] = _0),
                        (OBFH_NAME_PAIR(1, name[OBFH_NAME_INDEX(6)] = _0, name[OBFH_NAME_INDEX(5)] = _r), OBFH_NAME_PAIR(2, name[OBFH_NAME_INDEX(4)] = _o, name[OBFH_NAME_INDEX(3)] = _r), OBFH_NAME_PAIR(3, name[OBFH_NAME_INDEX(2)] = _r, name[OBFH_NAME_INDEX(1)] = _e), name[OBFH_NAME_INDEX(0)] = _f));
    PHANTOM_NOP;
    return name;
}
#undef ferror
#define ferror(...) OBFH_CRT_COMPACT_CALL(getFerrorName_proxy, int (*)(FILE *), __VA_ARGS__)

static inline char *getClearerrName_proxy(char *name) {
    OBFH_NAME_ORDER_FOR(OBFH_ALPHA_LOW | OBFH_ALPHA_HIGH,
                        (OBFH_NAME_PAIR(1, name[OBFH_NAME_INDEX(0)] = _c, name[OBFH_NAME_INDEX(1)] = _l), OBFH_NAME_PAIR(2, name[OBFH_NAME_INDEX(2)] = _e, name[OBFH_NAME_INDEX(3)] = _a), OBFH_NAME_PAIR(3, name[OBFH_NAME_INDEX(4)] = _r, name[OBFH_NAME_INDEX(5)] = _e), OBFH_NAME_PAIR(4, name[OBFH_NAME_INDEX(6)] = _r, name[OBFH_NAME_INDEX(7)] = _r), name[OBFH_NAME_INDEX(8)] = _0),
                        (OBFH_NAME_PAIR(1, name[OBFH_NAME_INDEX(8)] = _0, name[OBFH_NAME_INDEX(7)] = _r), OBFH_NAME_PAIR(2, name[OBFH_NAME_INDEX(6)] = _r, name[OBFH_NAME_INDEX(5)] = _e), OBFH_NAME_PAIR(3, name[OBFH_NAME_INDEX(4)] = _r, name[OBFH_NAME_INDEX(3)] = _a), OBFH_NAME_PAIR(4, name[OBFH_NAME_INDEX(2)] = _e, name[OBFH_NAME_INDEX(1)] = _l), name[OBFH_NAME_INDEX(0)] = _c));
    PHANTOM_NOP;
    return name;
}
#undef clearerr
#define clearerr(...) OBFH_CRT_COMPACT_CALL(getClearerrName_proxy, void (*)(FILE *), __VA_ARGS__)

static inline char *getRewindName_proxy(char *name) {
    OBFH_NAME_ORDER_FOR(OBFH_ALPHA_LOW | OBFH_ALPHA_HIGH,
                        (OBFH_NAME_PAIR(1, name[OBFH_NAME_INDEX(0)] = _r, name[OBFH_NAME_INDEX(1)] = _e), OBFH_NAME_PAIR(2, name[OBFH_NAME_INDEX(2)] = _w, name[OBFH_NAME_INDEX(3)] = _i), OBFH_NAME_PAIR(3, name[OBFH_NAME_INDEX(4)] = _n, name[OBFH_NAME_INDEX(5)] = _d), name[OBFH_NAME_INDEX(6)] = _0),
                        (OBFH_NAME_PAIR(1, name[OBFH_NAME_INDEX(6)] = _0, name[OBFH_NAME_INDEX(5)] = _d), OBFH_NAME_PAIR(2, name[OBFH_NAME_INDEX(4)] = _n, name[OBFH_NAME_INDEX(3)] = _i), OBFH_NAME_PAIR(3, name[OBFH_NAME_INDEX(2)] = _w, name[OBFH_NAME_INDEX(1)] = _e), name[OBFH_NAME_INDEX(0)] = _r));
    PHANTOM_NOP;
    return name;
}
#undef rewind
#define rewind(...) OBFH_CRT_COMPACT_CALL(getRewindName_proxy, void (*)(FILE *), __VA_ARGS__)

static inline char *getRemoveName_proxy(char *name) {
    OBFH_NAME_ORDER_FOR(OBFH_ALPHA_LOW | OBFH_ALPHA_HIGH,
                        (OBFH_NAME_PAIR(1, name[OBFH_NAME_INDEX(0)] = _r, name[OBFH_NAME_INDEX(1)] = _e), OBFH_NAME_PAIR(2, name[OBFH_NAME_INDEX(2)] = _m, name[OBFH_NAME_INDEX(3)] = _o), OBFH_NAME_PAIR(3, name[OBFH_NAME_INDEX(4)] = _v, name[OBFH_NAME_INDEX(5)] = _e), name[OBFH_NAME_INDEX(6)] = _0),
                        (OBFH_NAME_PAIR(1, name[OBFH_NAME_INDEX(6)] = _0, name[OBFH_NAME_INDEX(5)] = _e), OBFH_NAME_PAIR(2, name[OBFH_NAME_INDEX(4)] = _v, name[OBFH_NAME_INDEX(3)] = _o), OBFH_NAME_PAIR(3, name[OBFH_NAME_INDEX(2)] = _m, name[OBFH_NAME_INDEX(1)] = _e), name[OBFH_NAME_INDEX(0)] = _r));
    PHANTOM_NOP;
    return name;
}
#undef remove
#define remove(...) OBFH_CRT_COMPACT_CALL(getRemoveName_proxy, int (*)(const char *), __VA_ARGS__)

static inline char *getRenameName_proxy(char *name) {
    OBFH_NAME_ORDER_FOR(OBFH_ALPHA_LOW | OBFH_ALPHA_HIGH,
                        (OBFH_NAME_PAIR(1, name[OBFH_NAME_INDEX(0)] = _r, name[OBFH_NAME_INDEX(1)] = _e), OBFH_NAME_PAIR(2, name[OBFH_NAME_INDEX(2)] = _n, name[OBFH_NAME_INDEX(3)] = _a), OBFH_NAME_PAIR(3, name[OBFH_NAME_INDEX(4)] = _m, name[OBFH_NAME_INDEX(5)] = _e), name[OBFH_NAME_INDEX(6)] = _0),
                        (OBFH_NAME_PAIR(1, name[OBFH_NAME_INDEX(6)] = _0, name[OBFH_NAME_INDEX(5)] = _e), OBFH_NAME_PAIR(2, name[OBFH_NAME_INDEX(4)] = _m, name[OBFH_NAME_INDEX(3)] = _a), OBFH_NAME_PAIR(3, name[OBFH_NAME_INDEX(2)] = _n, name[OBFH_NAME_INDEX(1)] = _e), name[OBFH_NAME_INDEX(0)] = _r));
    PHANTOM_NOP;
    return name;
}
#undef rename
#define rename(...) OBFH_CRT_COMPACT_CALL(getRenameName_proxy, int (*)(const char *, const char *), __VA_ARGS__)

static inline char *getFreopenName_proxy(char *name) {
    OBFH_NAME_ORDER_FOR(OBFH_ALPHA_LOW | OBFH_ALPHA_HIGH,
                        (OBFH_NAME_PAIR(1, name[OBFH_NAME_INDEX(0)] = _f, name[OBFH_NAME_INDEX(1)] = _r), OBFH_NAME_PAIR(2, name[OBFH_NAME_INDEX(2)] = _e, name[OBFH_NAME_INDEX(3)] = _o), OBFH_NAME_PAIR(3, name[OBFH_NAME_INDEX(4)] = _p, name[OBFH_NAME_INDEX(5)] = _e), OBFH_NAME_PAIR(4, name[OBFH_NAME_INDEX(6)] = _n, name[OBFH_NAME_INDEX(7)] = _0)),
                        (OBFH_NAME_PAIR(1, name[OBFH_NAME_INDEX(7)] = _0, name[OBFH_NAME_INDEX(6)] = _n), OBFH_NAME_PAIR(2, name[OBFH_NAME_INDEX(5)] = _e, name[OBFH_NAME_INDEX(4)] = _p), OBFH_NAME_PAIR(3, name[OBFH_NAME_INDEX(3)] = _o, name[OBFH_NAME_INDEX(2)] = _e), OBFH_NAME_PAIR(4, name[OBFH_NAME_INDEX(1)] = _r, name[OBFH_NAME_INDEX(0)] = _f)));
    PHANTOM_NOP;
    return name;
}
#undef freopen
#define freopen(...) OBFH_CRT_COMPACT_CALL(getFreopenName_proxy, FILE *(*)(const char *, const char *, FILE *), __VA_ARGS__)

static inline void perror_proxy(const char *message) OBFH_CODE_SECTION_ATTRIBUTE {
    int saved_errno = errno;
    char name[7];
    OBFH_NAME_ORDER_FOR(OBFH_ALPHA_LOW | OBFH_ALPHA_HIGH,
                        (OBFH_NAME_PAIR(1, name[OBFH_NAME_INDEX(0)] = _p, name[OBFH_NAME_INDEX(1)] = _e), OBFH_NAME_PAIR(2, name[OBFH_NAME_INDEX(2)] = _r, name[OBFH_NAME_INDEX(3)] = _r), OBFH_NAME_PAIR(3, name[OBFH_NAME_INDEX(4)] = _o, name[OBFH_NAME_INDEX(5)] = _r), name[OBFH_NAME_INDEX(6)] = _0),
                        (OBFH_NAME_PAIR(1, name[OBFH_NAME_INDEX(6)] = _0, name[OBFH_NAME_INDEX(5)] = _r), OBFH_NAME_PAIR(2, name[OBFH_NAME_INDEX(4)] = _o, name[OBFH_NAME_INDEX(3)] = _r), OBFH_NAME_PAIR(3, name[OBFH_NAME_INDEX(2)] = _r, name[OBFH_NAME_INDEX(1)] = _e), name[OBFH_NAME_INDEX(0)] = _p));
    PHANTOM_NOP;
    FARPROC function = obfh_crt_resolve(name);
    errno = saved_errno;
    ((void (*)(const char *))function)(message);
}
#define perror(message) perror_proxy(message)

static inline char *getSprintfName_proxy(char *name) {
    BREAK_STACK_CFLOW;
    OBFH_NAME_ORDER_FOR(OBFH_ALPHA_LOW | OBFH_ALPHA_HIGH,
                        (OBFH_NAME_PAIR(1, name[OBFH_NAME_INDEX(0)] = _s, name[OBFH_NAME_INDEX(1)] = _p), OBFH_NAME_PAIR(2, name[OBFH_NAME_INDEX(2)] = _r, name[OBFH_NAME_INDEX(3)] = _i), OBFH_NAME_PAIR(3, name[OBFH_NAME_INDEX(4)] = _n, name[OBFH_NAME_INDEX(5)] = _t), OBFH_NAME_PAIR(4, name[OBFH_NAME_INDEX(6)] = _f, name[OBFH_NAME_INDEX(7)] = _0)),
                        (OBFH_NAME_PAIR(1, name[OBFH_NAME_INDEX(7)] = _0, name[OBFH_NAME_INDEX(6)] = _f), OBFH_NAME_PAIR(2, name[OBFH_NAME_INDEX(5)] = _t, name[OBFH_NAME_INDEX(4)] = _n), OBFH_NAME_PAIR(3, name[OBFH_NAME_INDEX(3)] = _i, name[OBFH_NAME_INDEX(2)] = _r), OBFH_NAME_PAIR(4, name[OBFH_NAME_INDEX(1)] = _p, name[OBFH_NAME_INDEX(0)] = _s)));
    PHANTOM_NOP;
#if OBFH_BETTER_CONTROL_FLOW
    BREAK_STACK_CFLOW;
#endif
    return name;
}
#define sprintf(...) OBFH_CRT_CALL(getSprintfName_proxy, int (*)(char *, const char *, ...), __VA_ARGS__)

static inline char *getFcloseName_proxy(char *name) {
    BREAK_STACK_CFLOW;
    OBFH_NAME_ORDER_FOR(OBFH_ALPHA_LOW | OBFH_ALPHA_HIGH,
                        (OBFH_NAME_PAIR(1, name[OBFH_NAME_INDEX(0)] = _f, name[OBFH_NAME_INDEX(1)] = _c), OBFH_NAME_PAIR(2, name[OBFH_NAME_INDEX(2)] = _l, name[OBFH_NAME_INDEX(3)] = _o), OBFH_NAME_PAIR(3, name[OBFH_NAME_INDEX(4)] = _s, name[OBFH_NAME_INDEX(5)] = _e), name[OBFH_NAME_INDEX(6)] = _0),
                        (OBFH_NAME_PAIR(1, name[OBFH_NAME_INDEX(6)] = _0, name[OBFH_NAME_INDEX(5)] = _e), OBFH_NAME_PAIR(2, name[OBFH_NAME_INDEX(4)] = _s, name[OBFH_NAME_INDEX(3)] = _o), OBFH_NAME_PAIR(3, name[OBFH_NAME_INDEX(2)] = _l, name[OBFH_NAME_INDEX(1)] = _c), name[OBFH_NAME_INDEX(0)] = _f));
    PHANTOM_NOP;
#if OBFH_BETTER_CONTROL_FLOW
    BREAK_STACK_CFLOW;
#endif
    return name;
}
#define fclose(...) OBFH_CRT_CALL(getFcloseName_proxy, int (*)(FILE *), __VA_ARGS__)

static inline char *getFopenName_proxy(char *name) {
    BREAK_STACK_CFLOW;
    OBFH_NAME_ORDER_FOR(OBFH_ALPHA_LOW | OBFH_ALPHA_HIGH,
                        (OBFH_NAME_PAIR(1, name[OBFH_NAME_INDEX(0)] = _f, name[OBFH_NAME_INDEX(1)] = _o), OBFH_NAME_PAIR(2, name[OBFH_NAME_INDEX(2)] = _p, name[OBFH_NAME_INDEX(3)] = _e), OBFH_NAME_PAIR(3, name[OBFH_NAME_INDEX(4)] = _n, name[OBFH_NAME_INDEX(5)] = _0)),
                        (OBFH_NAME_PAIR(1, name[OBFH_NAME_INDEX(5)] = _0, name[OBFH_NAME_INDEX(4)] = _n), OBFH_NAME_PAIR(2, name[OBFH_NAME_INDEX(3)] = _e, name[OBFH_NAME_INDEX(2)] = _p), OBFH_NAME_PAIR(3, name[OBFH_NAME_INDEX(1)] = _o, name[OBFH_NAME_INDEX(0)] = _f)));
    PHANTOM_NOP;
#if OBFH_BETTER_CONTROL_FLOW
    BREAK_STACK_CFLOW;
#endif
    return name;
}
#define fopen(...) OBFH_CRT_CALL(getFopenName_proxy, FILE *(*)(const char *, const char *), __VA_ARGS__)

static inline char *getFreadName_proxy(char *name) {
    BREAK_STACK_CFLOW;
    OBFH_NAME_ORDER_FOR(OBFH_ALPHA_LOW | OBFH_ALPHA_HIGH,
                        (OBFH_NAME_PAIR(1, name[OBFH_NAME_INDEX(0)] = _f, name[OBFH_NAME_INDEX(1)] = _r), OBFH_NAME_PAIR(2, name[OBFH_NAME_INDEX(2)] = _e, name[OBFH_NAME_INDEX(3)] = _a), OBFH_NAME_PAIR(3, name[OBFH_NAME_INDEX(4)] = _d, name[OBFH_NAME_INDEX(5)] = _0)),
                        (OBFH_NAME_PAIR(1, name[OBFH_NAME_INDEX(5)] = _0, name[OBFH_NAME_INDEX(4)] = _d), OBFH_NAME_PAIR(2, name[OBFH_NAME_INDEX(3)] = _a, name[OBFH_NAME_INDEX(2)] = _e), OBFH_NAME_PAIR(3, name[OBFH_NAME_INDEX(1)] = _r, name[OBFH_NAME_INDEX(0)] = _f)));
    PHANTOM_NOP;
#if OBFH_BETTER_CONTROL_FLOW
    BREAK_STACK_CFLOW;
#endif
    return name;
}
#define fread(...) OBFH_CRT_CALL(getFreadName_proxy, size_t (*)(void *, size_t, size_t, FILE *), __VA_ARGS__)

static inline char *getFwriteName_proxy(char *name) {
    BREAK_STACK_CFLOW;
    OBFH_NAME_ORDER_FOR(OBFH_ALPHA_LOW | OBFH_ALPHA_HIGH,
                        (OBFH_NAME_PAIR(1, name[OBFH_NAME_INDEX(0)] = _f, name[OBFH_NAME_INDEX(1)] = _w), OBFH_NAME_PAIR(2, name[OBFH_NAME_INDEX(2)] = _r, name[OBFH_NAME_INDEX(3)] = _i), OBFH_NAME_PAIR(3, name[OBFH_NAME_INDEX(4)] = _t, name[OBFH_NAME_INDEX(5)] = _e), name[OBFH_NAME_INDEX(6)] = _0),
                        (OBFH_NAME_PAIR(1, name[OBFH_NAME_INDEX(6)] = _0, name[OBFH_NAME_INDEX(5)] = _e), OBFH_NAME_PAIR(2, name[OBFH_NAME_INDEX(4)] = _t, name[OBFH_NAME_INDEX(3)] = _i), OBFH_NAME_PAIR(3, name[OBFH_NAME_INDEX(2)] = _r, name[OBFH_NAME_INDEX(1)] = _w), name[OBFH_NAME_INDEX(0)] = _f));
    PHANTOM_NOP;
#if OBFH_BETTER_CONTROL_FLOW
    BREAK_STACK_CFLOW;
#endif
    return name;
}
#define fwrite(...) OBFH_CRT_CALL(getFwriteName_proxy, size_t (*)(const void *, size_t, size_t, FILE *), __VA_ARGS__)

static inline char *getExitName_proxy(char *name) {
    BREAK_STACK_CFLOW;
    OBFH_NAME_ORDER_FOR(OBFH_ALPHA_LOW | OBFH_ALPHA_HIGH,
                        (OBFH_NAME_PAIR(1, name[OBFH_NAME_INDEX(0)] = _e, name[OBFH_NAME_INDEX(1)] = _x), OBFH_NAME_PAIR(2, name[OBFH_NAME_INDEX(2)] = _i, name[OBFH_NAME_INDEX(3)] = _t), name[OBFH_NAME_INDEX(4)] = _0),
                        (OBFH_NAME_PAIR(1, name[OBFH_NAME_INDEX(4)] = _0, name[OBFH_NAME_INDEX(3)] = _t), OBFH_NAME_PAIR(2, name[OBFH_NAME_INDEX(2)] = _i, name[OBFH_NAME_INDEX(1)] = _x), name[OBFH_NAME_INDEX(0)] = _e));
    PHANTOM_NOP;
#if OBFH_BETTER_CONTROL_FLOW
    BREAK_STACK_CFLOW;
#endif
    return name;
}
#define exit(...) OBFH_CRT_CALL(getExitName_proxy, void (*)(int), __VA_ARGS__)

static inline char *getStrtokName_proxy(char *name) {
    OBFH_CUSTOM_BEGIN;
    OBFH_CUSTOM_NOP;
    BREAK_STACK_CFLOW;
    OBFH_NAME_ORDER_FOR(OBFH_ALPHA_LOW | OBFH_ALPHA_HIGH,
                        (OBFH_NAME_PAIR(1, name[OBFH_NAME_INDEX(0)] = _s, name[OBFH_NAME_INDEX(1)] = _t), OBFH_NAME_PAIR(2, name[OBFH_NAME_INDEX(2)] = _r, name[OBFH_NAME_INDEX(3)] = _t), OBFH_NAME_PAIR(3, name[OBFH_NAME_INDEX(4)] = _o, name[OBFH_NAME_INDEX(5)] = _k), name[OBFH_NAME_INDEX(6)] = _0),
                        (OBFH_NAME_PAIR(1, name[OBFH_NAME_INDEX(6)] = _0, name[OBFH_NAME_INDEX(5)] = _k), OBFH_NAME_PAIR(2, name[OBFH_NAME_INDEX(4)] = _o, name[OBFH_NAME_INDEX(3)] = _t), OBFH_NAME_PAIR(3, name[OBFH_NAME_INDEX(2)] = _r, name[OBFH_NAME_INDEX(1)] = _t), name[OBFH_NAME_INDEX(0)] = _s));
    PHANTOM_NOP;
#if OBFH_BETTER_CONTROL_FLOW
    BREAK_STACK_CFLOW;
#endif
    return name;
}
#define strtok(...) OBFH_CRT_CALL(getStrtokName_proxy, char *(*)(char *, const char *), __VA_ARGS__)

static inline char *getRandName_proxy(char *name) {
    BREAK_STACK_CFLOW;
    OBFH_NAME_ORDER_FOR(OBFH_ALPHA_LOW | OBFH_ALPHA_HIGH,
                        (OBFH_NAME_PAIR(1, name[OBFH_NAME_INDEX(0)] = _r, name[OBFH_NAME_INDEX(1)] = _a), OBFH_NAME_PAIR(2, name[OBFH_NAME_INDEX(2)] = _n, name[OBFH_NAME_INDEX(3)] = _d), name[OBFH_NAME_INDEX(4)] = _0),
                        (OBFH_NAME_PAIR(1, name[OBFH_NAME_INDEX(4)] = _0, name[OBFH_NAME_INDEX(3)] = _d), OBFH_NAME_PAIR(2, name[OBFH_NAME_INDEX(2)] = _n, name[OBFH_NAME_INDEX(1)] = _a), name[OBFH_NAME_INDEX(0)] = _r));
    PHANTOM_NOP;
#if OBFH_BETTER_CONTROL_FLOW
    BREAK_STACK_CFLOW;
#endif
    return name;
}
#define rand(...) OBFH_CRT_CALL(getRandName_proxy, int (*)(void), __VA_ARGS__)

static inline char *getReallocName_proxy(char *name) OBFH_CODE_SECTION_ATTRIBUTE {
    BREAK_STACK_CFLOW;
    OBFH_NAME_ORDER_FOR(OBFH_ALPHA_LOW | OBFH_ALPHA_HIGH,
                        (OBFH_NAME_PAIR(1, name[OBFH_NAME_INDEX(0)] = _r, name[OBFH_NAME_INDEX(1)] = _e), OBFH_NAME_PAIR(2, name[OBFH_NAME_INDEX(2)] = _a, name[OBFH_NAME_INDEX(3)] = _l), OBFH_NAME_PAIR(3, name[OBFH_NAME_INDEX(4)] = _l, name[OBFH_NAME_INDEX(5)] = _o), OBFH_NAME_PAIR(4, name[OBFH_NAME_INDEX(6)] = _c, name[OBFH_NAME_INDEX(7)] = _0)),
                        (OBFH_NAME_PAIR(1, name[OBFH_NAME_INDEX(7)] = _0, name[OBFH_NAME_INDEX(6)] = _c), OBFH_NAME_PAIR(2, name[OBFH_NAME_INDEX(5)] = _o, name[OBFH_NAME_INDEX(4)] = _l), OBFH_NAME_PAIR(3, name[OBFH_NAME_INDEX(3)] = _l, name[OBFH_NAME_INDEX(2)] = _a), OBFH_NAME_PAIR(4, name[OBFH_NAME_INDEX(1)] = _e, name[OBFH_NAME_INDEX(0)] = _r)));
    PHANTOM_NOP;
#if OBFH_BETTER_CONTROL_FLOW
    BREAK_STACK_CFLOW;
#endif
    return name;
}

// #endregion

// #region 20. CRT adapters with argument/result transport
// ============================================================================
// 20. CRT adapters with argument/result transport
// ============================================================================

static inline void *calloc_proxy(size_t nmemb, size_t size) OBFH_CODE_SECTION_ATTRIBUTE {
    OBFH_CRT_PROXY_ENTER;
    char __obfh_resolved_name[7];
    OBFH_NAME_ORDER_FOR(OBFH_ALPHA_LOW | OBFH_ALPHA_HIGH,
                        (OBFH_NAME_PAIR(1, __obfh_resolved_name[OBFH_NAME_INDEX(0)] = _c, __obfh_resolved_name[OBFH_NAME_INDEX(1)] = _a), OBFH_NAME_PAIR(2, __obfh_resolved_name[OBFH_NAME_INDEX(2)] = _l, __obfh_resolved_name[OBFH_NAME_INDEX(3)] = _l), OBFH_NAME_PAIR(3, __obfh_resolved_name[OBFH_NAME_INDEX(4)] = _o, __obfh_resolved_name[OBFH_NAME_INDEX(5)] = _c), __obfh_resolved_name[OBFH_NAME_INDEX(6)] = _0),
                        (OBFH_NAME_PAIR(1, __obfh_resolved_name[OBFH_NAME_INDEX(6)] = _0, __obfh_resolved_name[OBFH_NAME_INDEX(5)] = _c), OBFH_NAME_PAIR(2, __obfh_resolved_name[OBFH_NAME_INDEX(4)] = _o, __obfh_resolved_name[OBFH_NAME_INDEX(3)] = _l), OBFH_NAME_PAIR(3, __obfh_resolved_name[OBFH_NAME_INDEX(2)] = _l, __obfh_resolved_name[OBFH_NAME_INDEX(1)] = _a), __obfh_resolved_name[OBFH_NAME_INDEX(0)] = _c));
    void *result = OBFH_CRT_TARGET(void *(*)(size_t, size_t), __obfh_resolved_name)(nmemb, size);
    OBFH_CRT_PROXY_RETURN(result);
}
#define calloc(nmemb, size) calloc_proxy(nmemb, size)

#undef realloc
static inline void *realloc_proxy(void *ptr, size_t size) OBFH_CODE_SECTION_ATTRIBUTE {
    OBFH_CRT_PROXY_ENTER;
    char name[32];
    PHANTOM_NOP;
    void *result = OBFH_CRT_TARGET(void *(*)(void *, size_t), getReallocName_proxy(name))(ptr, size);
    STACK_PROXY_FUNCTIONS;
    RET_BY_VAR(result);
}
#define realloc(ptr, size) realloc_proxy(ptr, size)

static inline char *gets_proxy(char *s) OBFH_CODE_SECTION_ATTRIBUTE {
    OBFH_CRT_PROXY_ENTER;
    char __obfh_resolved_name[5];
    OBFH_NAME_ORDER_FOR(OBFH_ALPHA_LOW | OBFH_ALPHA_HIGH,
                        (OBFH_NAME_PAIR(1, __obfh_resolved_name[OBFH_NAME_INDEX(0)] = _g, __obfh_resolved_name[OBFH_NAME_INDEX(1)] = _e), OBFH_NAME_PAIR(2, __obfh_resolved_name[OBFH_NAME_INDEX(2)] = _t, __obfh_resolved_name[OBFH_NAME_INDEX(3)] = _s), __obfh_resolved_name[OBFH_NAME_INDEX(4)] = _0),
                        (OBFH_NAME_PAIR(1, __obfh_resolved_name[OBFH_NAME_INDEX(4)] = _0, __obfh_resolved_name[OBFH_NAME_INDEX(3)] = _s), OBFH_NAME_PAIR(2, __obfh_resolved_name[OBFH_NAME_INDEX(2)] = _t, __obfh_resolved_name[OBFH_NAME_INDEX(1)] = _e), __obfh_resolved_name[OBFH_NAME_INDEX(0)] = _g));
    char *result = OBFH_CRT_TARGET(char *(*)(char *), __obfh_resolved_name)(s);
    OBFH_CRT_PROXY_RETURN(result);
}
#define gets(s) gets_proxy(s)

static inline int snprintf_proxy(char *str, size_t size, const char *format, ...) OBFH_CODE_SECTION_ATTRIBUTE {
    OBFH_CRT_PROXY_ENTER;
    va_list args;
    va_start(args, format);
    PHANTOM_NOP;
    int result = vsnprintf(str, size, format, args);
    va_end(args);
    STACK_PROXY_FUNCTIONS;
    RET_BY_VAR(result);
}
#define snprintf(...) snprintf_proxy(__VA_ARGS__)

static inline int vsprintf_proxy(char *str, const char *format, va_list args) OBFH_CODE_SECTION_ATTRIBUTE {
    OBFH_CRT_PROXY_ENTER;
    char __obfh_resolved_name[9];
    OBFH_NAME_ORDER_FOR(OBFH_ALPHA_LOW | OBFH_ALPHA_HIGH,
                        (OBFH_NAME_PAIR(1, __obfh_resolved_name[OBFH_NAME_INDEX(0)] = _v, __obfh_resolved_name[OBFH_NAME_INDEX(1)] = _s), OBFH_NAME_PAIR(2, __obfh_resolved_name[OBFH_NAME_INDEX(2)] = _p, __obfh_resolved_name[OBFH_NAME_INDEX(3)] = _r), OBFH_NAME_PAIR(3, __obfh_resolved_name[OBFH_NAME_INDEX(4)] = _i, __obfh_resolved_name[OBFH_NAME_INDEX(5)] = _n), OBFH_NAME_PAIR(4, __obfh_resolved_name[OBFH_NAME_INDEX(6)] = _t, __obfh_resolved_name[OBFH_NAME_INDEX(7)] = _f), __obfh_resolved_name[OBFH_NAME_INDEX(8)] = _0),
                        (OBFH_NAME_PAIR(1, __obfh_resolved_name[OBFH_NAME_INDEX(8)] = _0, __obfh_resolved_name[OBFH_NAME_INDEX(7)] = _f), OBFH_NAME_PAIR(2, __obfh_resolved_name[OBFH_NAME_INDEX(6)] = _t, __obfh_resolved_name[OBFH_NAME_INDEX(5)] = _n), OBFH_NAME_PAIR(3, __obfh_resolved_name[OBFH_NAME_INDEX(4)] = _i, __obfh_resolved_name[OBFH_NAME_INDEX(3)] = _r), OBFH_NAME_PAIR(4, __obfh_resolved_name[OBFH_NAME_INDEX(2)] = _p, __obfh_resolved_name[OBFH_NAME_INDEX(1)] = _s), __obfh_resolved_name[OBFH_NAME_INDEX(0)] = _v));
    int result = OBFH_CRT_TARGET(int (*)(char *, const char *, va_list), __obfh_resolved_name)(str, format, args);
    OBFH_CRT_PROXY_RETURN(result);
}
#define vsprintf(str, format, args) vsprintf_proxy(str, format, args)

static inline int vsnprintf_proxy(char *str, size_t size, const char *format, va_list args) OBFH_CODE_SECTION_ATTRIBUTE {
    OBFH_CRT_PROXY_ENTER;
    // Keep TCC's formatting adapter: it need not be an msvcrt export.
    int result = vsnprintf(str, size, format, args);
    OBFH_CRT_PROXY_RETURN(result);
}
#define vsnprintf(str, size, format, args) vsnprintf_proxy(str, size, format, args)

static inline char *getenv_proxy(const char *name) OBFH_CODE_SECTION_ATTRIBUTE {
    OBFH_CRT_PROXY_ENTER;
    char __obfh_resolved_name[7];
    OBFH_NAME_ORDER_FOR(OBFH_ALPHA_LOW | OBFH_ALPHA_HIGH,
                        (OBFH_NAME_PAIR(1, __obfh_resolved_name[OBFH_NAME_INDEX(0)] = _g, __obfh_resolved_name[OBFH_NAME_INDEX(1)] = _e), OBFH_NAME_PAIR(2, __obfh_resolved_name[OBFH_NAME_INDEX(2)] = _t, __obfh_resolved_name[OBFH_NAME_INDEX(3)] = _e), OBFH_NAME_PAIR(3, __obfh_resolved_name[OBFH_NAME_INDEX(4)] = _n, __obfh_resolved_name[OBFH_NAME_INDEX(5)] = _v), __obfh_resolved_name[OBFH_NAME_INDEX(6)] = _0),
                        (OBFH_NAME_PAIR(1, __obfh_resolved_name[OBFH_NAME_INDEX(6)] = _0, __obfh_resolved_name[OBFH_NAME_INDEX(5)] = _v), OBFH_NAME_PAIR(2, __obfh_resolved_name[OBFH_NAME_INDEX(4)] = _n, __obfh_resolved_name[OBFH_NAME_INDEX(3)] = _e), OBFH_NAME_PAIR(3, __obfh_resolved_name[OBFH_NAME_INDEX(2)] = _t, __obfh_resolved_name[OBFH_NAME_INDEX(1)] = _e), __obfh_resolved_name[OBFH_NAME_INDEX(0)] = _g));
    char *result = OBFH_CRT_TARGET(char *(*)(const char *), __obfh_resolved_name)(name);
    OBFH_CRT_PROXY_RETURN(result);
}
#define getenv(name) getenv_proxy(name)

static inline int system_proxy(const char *command) OBFH_CODE_SECTION_ATTRIBUTE {
    OBFH_CRT_PROXY_ENTER;
    char __obfh_resolved_name[7];
    OBFH_NAME_ORDER_FOR(OBFH_ALPHA_LOW | OBFH_ALPHA_HIGH,
                        (OBFH_NAME_PAIR(1, __obfh_resolved_name[OBFH_NAME_INDEX(0)] = _s, __obfh_resolved_name[OBFH_NAME_INDEX(1)] = _y), OBFH_NAME_PAIR(2, __obfh_resolved_name[OBFH_NAME_INDEX(2)] = _s, __obfh_resolved_name[OBFH_NAME_INDEX(3)] = _t), OBFH_NAME_PAIR(3, __obfh_resolved_name[OBFH_NAME_INDEX(4)] = _e, __obfh_resolved_name[OBFH_NAME_INDEX(5)] = _m), __obfh_resolved_name[OBFH_NAME_INDEX(6)] = _0),
                        (OBFH_NAME_PAIR(1, __obfh_resolved_name[OBFH_NAME_INDEX(6)] = _0, __obfh_resolved_name[OBFH_NAME_INDEX(5)] = _m), OBFH_NAME_PAIR(2, __obfh_resolved_name[OBFH_NAME_INDEX(4)] = _e, __obfh_resolved_name[OBFH_NAME_INDEX(3)] = _t), OBFH_NAME_PAIR(3, __obfh_resolved_name[OBFH_NAME_INDEX(2)] = _s, __obfh_resolved_name[OBFH_NAME_INDEX(1)] = _y), __obfh_resolved_name[OBFH_NAME_INDEX(0)] = _s));
    int result = OBFH_CRT_TARGET(int (*)(const char *), __obfh_resolved_name)(command);
    OBFH_CRT_PROXY_RETURN(result);
}
#define system(command) system_proxy(command)

static inline void abort_proxy(void) OBFH_CODE_SECTION_ATTRIBUTE {
    OBFH_CRT_PROXY_ENTER;
    PHANTOM_NOP;
    char __obfh_resolved_name[6];
    OBFH_NAME_ORDER_FOR(OBFH_ALPHA_LOW | OBFH_ALPHA_HIGH,
                        (OBFH_NAME_PAIR(1, __obfh_resolved_name[OBFH_NAME_INDEX(0)] = _a, __obfh_resolved_name[OBFH_NAME_INDEX(1)] = _b), OBFH_NAME_PAIR(2, __obfh_resolved_name[OBFH_NAME_INDEX(2)] = _o, __obfh_resolved_name[OBFH_NAME_INDEX(3)] = _r), OBFH_NAME_PAIR(3, __obfh_resolved_name[OBFH_NAME_INDEX(4)] = _t, __obfh_resolved_name[OBFH_NAME_INDEX(5)] = _0)),
                        (OBFH_NAME_PAIR(1, __obfh_resolved_name[OBFH_NAME_INDEX(5)] = _0, __obfh_resolved_name[OBFH_NAME_INDEX(4)] = _t), OBFH_NAME_PAIR(2, __obfh_resolved_name[OBFH_NAME_INDEX(3)] = _r, __obfh_resolved_name[OBFH_NAME_INDEX(2)] = _o), OBFH_NAME_PAIR(3, __obfh_resolved_name[OBFH_NAME_INDEX(1)] = _b, __obfh_resolved_name[OBFH_NAME_INDEX(0)] = _a)));
    OBFH_CRT_TARGET(void (*)(void), __obfh_resolved_name)
    ();
}
#define abort(...) abort_proxy(__VA_ARGS__)

static inline int atexit_proxy(void (*func)(void)) OBFH_CODE_SECTION_ATTRIBUTE {
    OBFH_CRT_PROXY_ENTER;
    char __obfh_resolved_name[7];
    OBFH_NAME_ORDER_FOR(OBFH_ALPHA_LOW | OBFH_ALPHA_HIGH,
                        (OBFH_NAME_PAIR(1, __obfh_resolved_name[OBFH_NAME_INDEX(0)] = _a, __obfh_resolved_name[OBFH_NAME_INDEX(1)] = _t), OBFH_NAME_PAIR(2, __obfh_resolved_name[OBFH_NAME_INDEX(2)] = _e, __obfh_resolved_name[OBFH_NAME_INDEX(3)] = _x), OBFH_NAME_PAIR(3, __obfh_resolved_name[OBFH_NAME_INDEX(4)] = _i, __obfh_resolved_name[OBFH_NAME_INDEX(5)] = _t), __obfh_resolved_name[OBFH_NAME_INDEX(6)] = _0),
                        (OBFH_NAME_PAIR(1, __obfh_resolved_name[OBFH_NAME_INDEX(6)] = _0, __obfh_resolved_name[OBFH_NAME_INDEX(5)] = _t), OBFH_NAME_PAIR(2, __obfh_resolved_name[OBFH_NAME_INDEX(4)] = _i, __obfh_resolved_name[OBFH_NAME_INDEX(3)] = _x), OBFH_NAME_PAIR(3, __obfh_resolved_name[OBFH_NAME_INDEX(2)] = _e, __obfh_resolved_name[OBFH_NAME_INDEX(1)] = _t), __obfh_resolved_name[OBFH_NAME_INDEX(0)] = _a));
    int result = OBFH_CRT_TARGET(int (*)(void (*)(void)), __obfh_resolved_name)(func);
    OBFH_CRT_PROXY_RETURN(result);
}
#define atexit(func) atexit_proxy(func)

static inline char *getcwd_proxy(char *buf, size_t size) OBFH_CODE_SECTION_ATTRIBUTE {
    OBFH_CRT_PROXY_ENTER;
    char __obfh_resolved_name[8];
    OBFH_NAME_ORDER_FOR(OBFH_ALPHA_LOW | OBFH_ALPHA_HIGH,
                        (__obfh_resolved_name[OBFH_NAME_INDEX(0)] = '_', __obfh_resolved_name[OBFH_NAME_INDEX(1)] = _g, __obfh_resolved_name[OBFH_NAME_INDEX(2)] = _e, __obfh_resolved_name[OBFH_NAME_INDEX(3)] = _t, __obfh_resolved_name[OBFH_NAME_INDEX(4)] = _c, __obfh_resolved_name[OBFH_NAME_INDEX(5)] = _w, __obfh_resolved_name[OBFH_NAME_INDEX(6)] = _d, __obfh_resolved_name[OBFH_NAME_INDEX(7)] = _0),
                        (__obfh_resolved_name[OBFH_NAME_INDEX(7)] = _0, __obfh_resolved_name[OBFH_NAME_INDEX(6)] = _d, __obfh_resolved_name[OBFH_NAME_INDEX(5)] = _w, __obfh_resolved_name[OBFH_NAME_INDEX(4)] = _c, __obfh_resolved_name[OBFH_NAME_INDEX(3)] = _t, __obfh_resolved_name[OBFH_NAME_INDEX(2)] = _e, __obfh_resolved_name[OBFH_NAME_INDEX(1)] = _g, __obfh_resolved_name[OBFH_NAME_INDEX(0)] = '_'));
    char *result = OBFH_CRT_TARGET(char *(*)(char *, int), __obfh_resolved_name)(buf, (int)size);
    OBFH_CRT_PROXY_RETURN(result);
}
#define getcwd(buf, size) ((char *)getcwd_proxy(buf, size))

static inline int tolower_proxy(int c) OBFH_CODE_SECTION_ATTRIBUTE {
    OBFH_CRT_PROXY_ENTER;
    char __obfh_resolved_name[8];
    OBFH_NAME_ORDER_FOR(OBFH_ALPHA_LOW | OBFH_ALPHA_HIGH,
                        (OBFH_NAME_PAIR(1, __obfh_resolved_name[OBFH_NAME_INDEX(0)] = _t, __obfh_resolved_name[OBFH_NAME_INDEX(1)] = _o), OBFH_NAME_PAIR(2, __obfh_resolved_name[OBFH_NAME_INDEX(2)] = _l, __obfh_resolved_name[OBFH_NAME_INDEX(3)] = _o), OBFH_NAME_PAIR(3, __obfh_resolved_name[OBFH_NAME_INDEX(4)] = _w, __obfh_resolved_name[OBFH_NAME_INDEX(5)] = _e), OBFH_NAME_PAIR(4, __obfh_resolved_name[OBFH_NAME_INDEX(6)] = _r, __obfh_resolved_name[OBFH_NAME_INDEX(7)] = _0)),
                        (OBFH_NAME_PAIR(1, __obfh_resolved_name[OBFH_NAME_INDEX(7)] = _0, __obfh_resolved_name[OBFH_NAME_INDEX(6)] = _r), OBFH_NAME_PAIR(2, __obfh_resolved_name[OBFH_NAME_INDEX(5)] = _e, __obfh_resolved_name[OBFH_NAME_INDEX(4)] = _w), OBFH_NAME_PAIR(3, __obfh_resolved_name[OBFH_NAME_INDEX(3)] = _o, __obfh_resolved_name[OBFH_NAME_INDEX(2)] = _l), OBFH_NAME_PAIR(4, __obfh_resolved_name[OBFH_NAME_INDEX(1)] = _o, __obfh_resolved_name[OBFH_NAME_INDEX(0)] = _t)));
    int result = OBFH_CRT_TARGET(int (*)(int), __obfh_resolved_name)(c);
    OBFH_CRT_PROXY_RETURN(result);
}
#define tolower(c) tolower_proxy(c)

static inline int toupper_proxy(int c) OBFH_CODE_SECTION_ATTRIBUTE {
    OBFH_CRT_PROXY_ENTER;
    char __obfh_resolved_name[8];
    OBFH_NAME_ORDER_FOR(OBFH_ALPHA_LOW | OBFH_ALPHA_HIGH,
                        (OBFH_NAME_PAIR(1, __obfh_resolved_name[OBFH_NAME_INDEX(0)] = _t, __obfh_resolved_name[OBFH_NAME_INDEX(1)] = _o), OBFH_NAME_PAIR(2, __obfh_resolved_name[OBFH_NAME_INDEX(2)] = _u, __obfh_resolved_name[OBFH_NAME_INDEX(3)] = _p), OBFH_NAME_PAIR(3, __obfh_resolved_name[OBFH_NAME_INDEX(4)] = _p, __obfh_resolved_name[OBFH_NAME_INDEX(5)] = _e), OBFH_NAME_PAIR(4, __obfh_resolved_name[OBFH_NAME_INDEX(6)] = _r, __obfh_resolved_name[OBFH_NAME_INDEX(7)] = _0)),
                        (OBFH_NAME_PAIR(1, __obfh_resolved_name[OBFH_NAME_INDEX(7)] = _0, __obfh_resolved_name[OBFH_NAME_INDEX(6)] = _r), OBFH_NAME_PAIR(2, __obfh_resolved_name[OBFH_NAME_INDEX(5)] = _e, __obfh_resolved_name[OBFH_NAME_INDEX(4)] = _p), OBFH_NAME_PAIR(3, __obfh_resolved_name[OBFH_NAME_INDEX(3)] = _p, __obfh_resolved_name[OBFH_NAME_INDEX(2)] = _u), OBFH_NAME_PAIR(4, __obfh_resolved_name[OBFH_NAME_INDEX(1)] = _o, __obfh_resolved_name[OBFH_NAME_INDEX(0)] = _t)));
    int result = OBFH_CRT_TARGET(int (*)(int), __obfh_resolved_name)(c);
    OBFH_CRT_PROXY_RETURN(result);
}
#define toupper(c) toupper_proxy(c)

// #endregion

// #region 21. Console and Windows API aliases
// ============================================================================
// 21. Console and Windows API aliases
// ============================================================================

// getch, _getch
#define _getch(...) obfh_int_proxy(_getch(__VA_ARGS__) * TRUE)
#define getch(...) obfh_int_proxy(getch(__VA_ARGS__) + FALSE)

#ifdef UNICODE
#define OBFH_WINAPI(name) name##W
#else
#define OBFH_WINAPI(name) name##A
#endif

// Object aliases preserve calls, parenthesized names and function addresses.
#undef CreateFile
#define CreateFile OBFH_WINAPI(CreateFile)
#undef GetModuleHandle
#define GetModuleHandle OBFH_WINAPI(GetModuleHandle)
#undef GetStartupInfo
#define GetStartupInfo OBFH_WINAPI(GetStartupInfo)
#undef GetModuleFileName
#define GetModuleFileName OBFH_WINAPI(GetModuleFileName)
#undef GetTempPath
#define GetTempPath OBFH_WINAPI(GetTempPath)

// USER32/GDI32: covered function-like calls use cached indirect targets.
// Six local decode layouts; arguments appear once in the typed invocation.
#define OBFH_GUI_DECODE(index, key, code) ({ \
    __obfh_asm__(code \
                 : "+&r"(__obfh_gui_value) \
                 : "r"((ULONG_PTR)(key)), "r"(((ULONG_PTR)(unsigned int)__obfh_gui_bias)), "i"(__obfh_gui_rotate) \
                 : "cc"); \
})
#define OBFH_GUI_FORM_0(index) OBFH_GUI_DECODE(index, OBFH_GUI_SITE_KEY, \
                                               "sub" OBFH_GUI_WIDTH " %2, %0; ror" OBFH_GUI_WIDTH " %3, %0; xor" OBFH_GUI_WIDTH " %1, %0;")
#define OBFH_GUI_FORM_1(index) OBFH_GUI_DECODE(index, OBFH_GUI_SITE_KEY, \
                                               "xor" OBFH_GUI_WIDTH " %1, %0; ror" OBFH_GUI_WIDTH " %3, %0; sub" OBFH_GUI_WIDTH " %2, %0;")
#define OBFH_GUI_FORM_2(index) OBFH_GUI_DECODE(index, OBFH_GUI_SITE_KEY, \
                                               "sub" OBFH_GUI_WIDTH " %2, %0; xor" OBFH_GUI_WIDTH " %1, %0; ror" OBFH_GUI_WIDTH " %3, %0;")
#define OBFH_GUI_FORM_3(index) OBFH_GUI_DECODE(index, OBFH_GUI_SITE_KEY, \
                                               "ror" OBFH_GUI_WIDTH " %3, %0; sub" OBFH_GUI_WIDTH " %2, %0; xor" OBFH_GUI_WIDTH " %1, %0;")
#define OBFH_GUI_FORM_4(index) OBFH_GUI_DECODE(index, OBFH_GUI_SITE_KEY, \
                                               "ror" OBFH_GUI_WIDTH " %3, %0; xor" OBFH_GUI_WIDTH " %1, %0; sub" OBFH_GUI_WIDTH " %2, %0;")
#define OBFH_GUI_FORM_5(index) OBFH_GUI_DECODE(index, OBFH_GUI_SITE_KEY, \
                                               "xor" OBFH_GUI_WIDTH " %1, %0; sub" OBFH_GUI_WIDTH " %2, %0; ror" OBFH_GUI_WIDTH " %3, %0;")
// Optional internal observer for cache-isolation regression tests.
#ifdef OBFH_GUI_CACHE_OBSERVER
#define OBFH_GUI_CACHE_VISIT(slot, site) OBFH_GUI_CACHE_OBSERVER(slot, site)
#else
#define OBFH_GUI_CACHE_VISIT(slot, site) ((void)0)
#endif
#define OBFH_GUI_CALL(index, module, name, type, ...) ({ \
    __label__ __obfh_gui_cached, __obfh_gui_decode; \
    enum { __obfh_gui_site = __COUNTER__, \
           OBFH_GUI_FIELDS(__obfh_gui_form_draw, __obfh_gui_site, 0x47554936u), \
           __obfh_gui_form = __obfh_gui_form_draw % 6u, \
           OBFH_GUI_FIELDS(__obfh_gui_gap_draw, __obfh_gui_site, 0x534c4f54u), \
           __obfh_gui_gap = __obfh_gui_gap_draw & 31u, \
           OBFH_GUI_KEY_FIELDS(__obfh_gui_site), \
           OBFH_GUI_FIELDS(__obfh_gui_bias, __obfh_gui_site, 0x47554933u), \
           OBFH_GUI_FIELDS(__obfh_gui_rotate_draw, __obfh_gui_site, 0x47554934u), \
           __obfh_gui_rotate = (unsigned int)__obfh_gui_rotate_draw % (sizeof(ULONG_PTR) * 8 - 1) + 1 }; \
    static struct { \
        unsigned char prefix[1u + __obfh_gui_gap]; \
        OBFH_GUI_SLOT cache __attribute__((aligned(16))); \
    } __obfh_gui_storage; \
    ULONG_PTR __obfh_gui_value; \
    OBFH_GUI_CACHE_VISIT(&__obfh_gui_storage.cache, __obfh_gui_site); \
    OBFH_INLINE_EXIT(__obfh_gui_storage.cache.ready, __obfh_gui_cached); \
    { \
        unsigned char __obfh_gui_name[64]; \
        size_t __obfh_gui_length = name(__obfh_gui_name); \
        __obfh_gui_value = obfh_gui_cold(module, &__obfh_gui_storage.cache, __obfh_gui_site, __obfh_gui_name, __obfh_gui_length); \
    } \
    goto __obfh_gui_decode; \
__obfh_gui_cached: \
    __obfh_asm__("" \
                 : \
                 : \
                 : "memory"); \
    __obfh_gui_value = (ULONG_PTR)__obfh_gui_storage.cache.encoded; \
__obfh_gui_decode: \
    __builtin_choose_expr(__obfh_gui_form == 0, OBFH_GUI_FORM_0(__obfh_gui_site), \
                          __builtin_choose_expr(__obfh_gui_form == 1, OBFH_GUI_FORM_1(__obfh_gui_site), \
                                                __builtin_choose_expr(__obfh_gui_form == 2, OBFH_GUI_FORM_2(__obfh_gui_site), \
                                                                      __builtin_choose_expr(__obfh_gui_form == 3, OBFH_GUI_FORM_3(__obfh_gui_site), \
                                                                                            __builtin_choose_expr(__obfh_gui_form == 4, OBFH_GUI_FORM_4(__obfh_gui_site), OBFH_GUI_FORM_5(__obfh_gui_site)))))); \
    ((type)__obfh_gui_value)(__VA_ARGS__); \
})

// One typed front end; export names are assembled only on the cold path.
#define OBFH_API_CALL(module, name, ...) \
    OBFH_GUI_CALL(OBFH_GUI_ID_##name, module, OBFH_GUI_NAME_##name, __typeof__(&name), __VA_ARGS__)

#define OBFH_GUI_ID_GetCurrentThreadId 136
#define OBFH_GUI_NAME_GetCurrentThreadId(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 136, (((unsigned int)'G' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'C' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 136, (((unsigned int)'u' << 0) | ((unsigned int)'r' << 8) | ((unsigned int)'r' << 16) | ((unsigned int)'e' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 136, (((unsigned int)'n' << 0) | ((unsigned int)'t' << 8) | ((unsigned int)'T' << 16) | ((unsigned int)'h' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 136, (((unsigned int)'r' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'a' << 16) | ((unsigned int)'d' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 16, 136, (((unsigned int)'I' << 0) | ((unsigned int)'d' << 8) | 0u | 0u)); \
    19u; \
})
#define GetCurrentThreadId(...) OBFH_API_CALL(2, GetCurrentThreadId, __VA_ARGS__)

// Additional process, file, window, text and resource APIs.
#if defined(__TINYC__)
WINBASEAPI int WINAPI MultiByteToWideChar(UINT codepage, DWORD flags, LPCSTR input, int input_length, LPWSTR output, int output_length);
WINBASEAPI int WINAPI WideCharToMultiByte(UINT codepage, DWORD flags, LPCWSTR input, int input_length, LPSTR output, int output_length, LPCSTR fallback, LPBOOL used_fallback);
#endif
#define OBFH_GUI_ID_GetCurrentProcess 137
#define OBFH_GUI_NAME_GetCurrentProcess(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 137, (((unsigned int)'G' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'C' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 137, (((unsigned int)'u' << 0) | ((unsigned int)'r' << 8) | ((unsigned int)'r' << 16) | ((unsigned int)'e' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 137, (((unsigned int)'n' << 0) | ((unsigned int)'t' << 8) | ((unsigned int)'P' << 16) | ((unsigned int)'r' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 137, (((unsigned int)'o' << 0) | ((unsigned int)'c' << 8) | ((unsigned int)'e' << 16) | ((unsigned int)'s' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 16, 137, (((unsigned int)'s' << 0) | 0u | 0u | 0u)); \
    18u; \
})
#undef GetCurrentProcess
#define GetCurrentProcess(...) OBFH_API_CALL(2, GetCurrentProcess, __VA_ARGS__)

#define OBFH_GUI_ID_Sleep 138
#define OBFH_GUI_NAME_Sleep(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 138, (((unsigned int)'S' << 0) | ((unsigned int)'l' << 8) | ((unsigned int)'e' << 16) | ((unsigned int)'e' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 138, (((unsigned int)'p' << 0) | 0u | 0u | 0u)); \
    6u; \
})
#undef Sleep
#define Sleep(...) OBFH_API_CALL(2, Sleep, __VA_ARGS__)

#define OBFH_GUI_ID_HeapCreate 139
#define OBFH_GUI_NAME_HeapCreate(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 139, (((unsigned int)'H' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'a' << 16) | ((unsigned int)'p' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 139, (((unsigned int)'C' << 0) | ((unsigned int)'r' << 8) | ((unsigned int)'e' << 16) | ((unsigned int)'a' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 139, (((unsigned int)'t' << 0) | ((unsigned int)'e' << 8) | 0u | 0u)); \
    11u; \
})
#undef HeapCreate
#define HeapCreate(...) OBFH_API_CALL(2, HeapCreate, __VA_ARGS__)

#define OBFH_GUI_ID_SetConsoleTextAttribute 140
#define OBFH_GUI_NAME_SetConsoleTextAttribute(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 140, (((unsigned int)'S' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'C' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 140, (((unsigned int)'o' << 0) | ((unsigned int)'n' << 8) | ((unsigned int)'s' << 16) | ((unsigned int)'o' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 140, (((unsigned int)'l' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'T' << 16) | ((unsigned int)'e' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 140, (((unsigned int)'x' << 0) | ((unsigned int)'t' << 8) | ((unsigned int)'A' << 16) | ((unsigned int)'t' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 16, 140, (((unsigned int)'t' << 0) | ((unsigned int)'r' << 8) | ((unsigned int)'i' << 16) | ((unsigned int)'b' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 20, 140, (((unsigned int)'u' << 0) | ((unsigned int)'t' << 8) | ((unsigned int)'e' << 16) | 0u)); \
    24u; \
})
#undef SetConsoleTextAttribute
#define SetConsoleTextAttribute(...) OBFH_API_CALL(2, SetConsoleTextAttribute, __VA_ARGS__)

#define OBFH_GUI_ID_GetCurrentProcessId 141
#define OBFH_GUI_NAME_GetCurrentProcessId(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 141, (((unsigned int)'G' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'C' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 141, (((unsigned int)'u' << 0) | ((unsigned int)'r' << 8) | ((unsigned int)'r' << 16) | ((unsigned int)'e' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 141, (((unsigned int)'n' << 0) | ((unsigned int)'t' << 8) | ((unsigned int)'P' << 16) | ((unsigned int)'r' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 141, (((unsigned int)'o' << 0) | ((unsigned int)'c' << 8) | ((unsigned int)'e' << 16) | ((unsigned int)'s' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 16, 141, (((unsigned int)'s' << 0) | ((unsigned int)'I' << 8) | ((unsigned int)'d' << 16) | 0u)); \
    20u; \
})
#undef GetCurrentProcessId
#define GetCurrentProcessId(...) OBFH_API_CALL(2, GetCurrentProcessId, __VA_ARGS__)

#define OBFH_GUI_ID_GetCurrentThread 142
#define OBFH_GUI_NAME_GetCurrentThread(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 142, (((unsigned int)'G' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'C' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 142, (((unsigned int)'u' << 0) | ((unsigned int)'r' << 8) | ((unsigned int)'r' << 16) | ((unsigned int)'e' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 142, (((unsigned int)'n' << 0) | ((unsigned int)'t' << 8) | ((unsigned int)'T' << 16) | ((unsigned int)'h' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 142, (((unsigned int)'r' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'a' << 16) | ((unsigned int)'d' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 16, 142, (0u | 0u | 0u | 0u)); \
    17u; \
})
#undef GetCurrentThread
#define GetCurrentThread(...) OBFH_API_CALL(2, GetCurrentThread, __VA_ARGS__)

#define OBFH_GUI_ID_GetExitCodeThread 143
#define OBFH_GUI_NAME_GetExitCodeThread(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 143, (((unsigned int)'G' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'E' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 143, (((unsigned int)'x' << 0) | ((unsigned int)'i' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'C' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 143, (((unsigned int)'o' << 0) | ((unsigned int)'d' << 8) | ((unsigned int)'e' << 16) | ((unsigned int)'T' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 143, (((unsigned int)'h' << 0) | ((unsigned int)'r' << 8) | ((unsigned int)'e' << 16) | ((unsigned int)'a' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 16, 143, (((unsigned int)'d' << 0) | 0u | 0u | 0u)); \
    18u; \
})
#undef GetExitCodeThread
#define GetExitCodeThread(...) OBFH_API_CALL(2, GetExitCodeThread, __VA_ARGS__)

#define OBFH_GUI_ID_DuplicateHandle 144
#define OBFH_GUI_NAME_DuplicateHandle(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 144, (((unsigned int)'D' << 0) | ((unsigned int)'u' << 8) | ((unsigned int)'p' << 16) | ((unsigned int)'l' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 144, (((unsigned int)'i' << 0) | ((unsigned int)'c' << 8) | ((unsigned int)'a' << 16) | ((unsigned int)'t' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 144, (((unsigned int)'e' << 0) | ((unsigned int)'H' << 8) | ((unsigned int)'a' << 16) | ((unsigned int)'n' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 144, (((unsigned int)'d' << 0) | ((unsigned int)'l' << 8) | ((unsigned int)'e' << 16) | 0u)); \
    16u; \
})
#undef DuplicateHandle
#define DuplicateHandle(...) OBFH_API_CALL(2, DuplicateHandle, __VA_ARGS__)

#define OBFH_GUI_ID_QueryPerformanceCounter 145
#define OBFH_GUI_NAME_QueryPerformanceCounter(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 145, (((unsigned int)'Q' << 0) | ((unsigned int)'u' << 8) | ((unsigned int)'e' << 16) | ((unsigned int)'r' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 145, (((unsigned int)'y' << 0) | ((unsigned int)'P' << 8) | ((unsigned int)'e' << 16) | ((unsigned int)'r' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 145, (((unsigned int)'f' << 0) | ((unsigned int)'o' << 8) | ((unsigned int)'r' << 16) | ((unsigned int)'m' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 145, (((unsigned int)'a' << 0) | ((unsigned int)'n' << 8) | ((unsigned int)'c' << 16) | ((unsigned int)'e' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 16, 145, (((unsigned int)'C' << 0) | ((unsigned int)'o' << 8) | ((unsigned int)'u' << 16) | ((unsigned int)'n' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 20, 145, (((unsigned int)'t' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'r' << 16) | 0u)); \
    24u; \
})
#undef QueryPerformanceCounter
#define QueryPerformanceCounter(...) OBFH_API_CALL(2, QueryPerformanceCounter, __VA_ARGS__)

#define OBFH_GUI_ID_QueryPerformanceFrequency 146
#define OBFH_GUI_NAME_QueryPerformanceFrequency(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 146, (((unsigned int)'Q' << 0) | ((unsigned int)'u' << 8) | ((unsigned int)'e' << 16) | ((unsigned int)'r' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 146, (((unsigned int)'y' << 0) | ((unsigned int)'P' << 8) | ((unsigned int)'e' << 16) | ((unsigned int)'r' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 146, (((unsigned int)'f' << 0) | ((unsigned int)'o' << 8) | ((unsigned int)'r' << 16) | ((unsigned int)'m' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 146, (((unsigned int)'a' << 0) | ((unsigned int)'n' << 8) | ((unsigned int)'c' << 16) | ((unsigned int)'e' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 16, 146, (((unsigned int)'F' << 0) | ((unsigned int)'r' << 8) | ((unsigned int)'e' << 16) | ((unsigned int)'q' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 20, 146, (((unsigned int)'u' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'n' << 16) | ((unsigned int)'c' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 24, 146, (((unsigned int)'y' << 0) | 0u | 0u | 0u)); \
    26u; \
})
#undef QueryPerformanceFrequency
#define QueryPerformanceFrequency(...) OBFH_API_CALL(2, QueryPerformanceFrequency, __VA_ARGS__)

#define OBFH_GUI_ID_GetTickCount 147
#define OBFH_GUI_NAME_GetTickCount(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 147, (((unsigned int)'G' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'T' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 147, (((unsigned int)'i' << 0) | ((unsigned int)'c' << 8) | ((unsigned int)'k' << 16) | ((unsigned int)'C' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 147, (((unsigned int)'o' << 0) | ((unsigned int)'u' << 8) | ((unsigned int)'n' << 16) | ((unsigned int)'t' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 147, (0u | 0u | 0u | 0u)); \
    13u; \
})
#undef GetTickCount
#define GetTickCount(...) OBFH_API_CALL(2, GetTickCount, __VA_ARGS__)

// Older TCC platform headers omit this declaration.
#if defined(__TINYC__)
WINBASEAPI ULONGLONG WINAPI GetTickCount64(void);
#endif
#define OBFH_GUI_ID_GetTickCount64 148
#define OBFH_GUI_NAME_GetTickCount64(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 148, (((unsigned int)'G' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'T' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 148, (((unsigned int)'i' << 0) | ((unsigned int)'c' << 8) | ((unsigned int)'k' << 16) | ((unsigned int)'C' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 148, (((unsigned int)'o' << 0) | ((unsigned int)'u' << 8) | ((unsigned int)'n' << 16) | ((unsigned int)'t' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 148, (((unsigned int)'6' << 0) | ((unsigned int)'4' << 8) | 0u | 0u)); \
    15u; \
})
#undef GetTickCount64
#define GetTickCount64(...) OBFH_API_CALL(2, GetTickCount64, __VA_ARGS__)

#define OBFH_GUI_ID_MultiByteToWideChar 149
#define OBFH_GUI_NAME_MultiByteToWideChar(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 149, (((unsigned int)'M' << 0) | ((unsigned int)'u' << 8) | ((unsigned int)'l' << 16) | ((unsigned int)'t' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 149, (((unsigned int)'i' << 0) | ((unsigned int)'B' << 8) | ((unsigned int)'y' << 16) | ((unsigned int)'t' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 149, (((unsigned int)'e' << 0) | ((unsigned int)'T' << 8) | ((unsigned int)'o' << 16) | ((unsigned int)'W' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 149, (((unsigned int)'i' << 0) | ((unsigned int)'d' << 8) | ((unsigned int)'e' << 16) | ((unsigned int)'C' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 16, 149, (((unsigned int)'h' << 0) | ((unsigned int)'a' << 8) | ((unsigned int)'r' << 16) | 0u)); \
    20u; \
})
#undef MultiByteToWideChar
#define MultiByteToWideChar(...) OBFH_API_CALL(2, MultiByteToWideChar, __VA_ARGS__)

#define OBFH_GUI_ID_WideCharToMultiByte 150
#define OBFH_GUI_NAME_WideCharToMultiByte(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 150, (((unsigned int)'W' << 0) | ((unsigned int)'i' << 8) | ((unsigned int)'d' << 16) | ((unsigned int)'e' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 150, (((unsigned int)'C' << 0) | ((unsigned int)'h' << 8) | ((unsigned int)'a' << 16) | ((unsigned int)'r' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 150, (((unsigned int)'T' << 0) | ((unsigned int)'o' << 8) | ((unsigned int)'M' << 16) | ((unsigned int)'u' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 150, (((unsigned int)'l' << 0) | ((unsigned int)'t' << 8) | ((unsigned int)'i' << 16) | ((unsigned int)'B' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 16, 150, (((unsigned int)'y' << 0) | ((unsigned int)'t' << 8) | ((unsigned int)'e' << 16) | 0u)); \
    20u; \
})
#undef WideCharToMultiByte
#define WideCharToMultiByte(...) OBFH_API_CALL(2, WideCharToMultiByte, __VA_ARGS__)

#define OBFH_GUI_ID_LoadResource 151
#define OBFH_GUI_NAME_LoadResource(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 151, (((unsigned int)'L' << 0) | ((unsigned int)'o' << 8) | ((unsigned int)'a' << 16) | ((unsigned int)'d' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 151, (((unsigned int)'R' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'s' << 16) | ((unsigned int)'o' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 151, (((unsigned int)'u' << 0) | ((unsigned int)'r' << 8) | ((unsigned int)'c' << 16) | ((unsigned int)'e' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 151, (0u | 0u | 0u | 0u)); \
    13u; \
})
#undef LoadResource
#define LoadResource(...) OBFH_API_CALL(2, LoadResource, __VA_ARGS__)

#define OBFH_GUI_ID_LockResource 152
#define OBFH_GUI_NAME_LockResource(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 152, (((unsigned int)'L' << 0) | ((unsigned int)'o' << 8) | ((unsigned int)'c' << 16) | ((unsigned int)'k' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 152, (((unsigned int)'R' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'s' << 16) | ((unsigned int)'o' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 152, (((unsigned int)'u' << 0) | ((unsigned int)'r' << 8) | ((unsigned int)'c' << 16) | ((unsigned int)'e' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 152, (0u | 0u | 0u | 0u)); \
    13u; \
})
#undef LockResource
#define LockResource(...) OBFH_API_CALL(2, LockResource, __VA_ARGS__)

#define OBFH_GUI_ID_SizeofResource 153
#define OBFH_GUI_NAME_SizeofResource(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 153, (((unsigned int)'S' << 0) | ((unsigned int)'i' << 8) | ((unsigned int)'z' << 16) | ((unsigned int)'e' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 153, (((unsigned int)'o' << 0) | ((unsigned int)'f' << 8) | ((unsigned int)'R' << 16) | ((unsigned int)'e' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 153, (((unsigned int)'s' << 0) | ((unsigned int)'o' << 8) | ((unsigned int)'u' << 16) | ((unsigned int)'r' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 153, (((unsigned int)'c' << 0) | ((unsigned int)'e' << 8) | 0u | 0u)); \
    15u; \
})
#undef SizeofResource
#define SizeofResource(...) OBFH_API_CALL(2, SizeofResource, __VA_ARGS__)

#define OBFH_GUI_ID_FindClose 154
#define OBFH_GUI_NAME_FindClose(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 154, (((unsigned int)'F' << 0) | ((unsigned int)'i' << 8) | ((unsigned int)'n' << 16) | ((unsigned int)'d' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 154, (((unsigned int)'C' << 0) | ((unsigned int)'l' << 8) | ((unsigned int)'o' << 16) | ((unsigned int)'s' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 154, (((unsigned int)'e' << 0) | 0u | 0u | 0u)); \
    10u; \
})
#undef FindClose
#define FindClose(...) OBFH_API_CALL(2, FindClose, __VA_ARGS__)

#define OBFH_GUI_ID_GetDesktopWindow 155
#define OBFH_GUI_NAME_GetDesktopWindow(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 155, (((unsigned int)'G' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'D' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 155, (((unsigned int)'e' << 0) | ((unsigned int)'s' << 8) | ((unsigned int)'k' << 16) | ((unsigned int)'t' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 155, (((unsigned int)'o' << 0) | ((unsigned int)'p' << 8) | ((unsigned int)'W' << 16) | ((unsigned int)'i' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 155, (((unsigned int)'n' << 0) | ((unsigned int)'d' << 8) | ((unsigned int)'o' << 16) | ((unsigned int)'w' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 16, 155, (0u | 0u | 0u | 0u)); \
    17u; \
})
#undef GetDesktopWindow
#define GetDesktopWindow(...) OBFH_API_CALL(0, GetDesktopWindow, __VA_ARGS__)

#define OBFH_GUI_ID_GetParent 156
#define OBFH_GUI_NAME_GetParent(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 156, (((unsigned int)'G' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'P' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 156, (((unsigned int)'a' << 0) | ((unsigned int)'r' << 8) | ((unsigned int)'e' << 16) | ((unsigned int)'n' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 156, (((unsigned int)'t' << 0) | 0u | 0u | 0u)); \
    10u; \
})
#undef GetParent
#define GetParent(...) OBFH_API_CALL(0, GetParent, __VA_ARGS__)

#define OBFH_GUI_ID_SetTimer 157
#define OBFH_GUI_NAME_SetTimer(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 157, (((unsigned int)'S' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'T' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 157, (((unsigned int)'i' << 0) | ((unsigned int)'m' << 8) | ((unsigned int)'e' << 16) | ((unsigned int)'r' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 157, (0u | 0u | 0u | 0u)); \
    9u; \
})
#undef SetTimer
#define SetTimer(...) OBFH_API_CALL(0, SetTimer, __VA_ARGS__)

#define OBFH_GUI_ID_KillTimer 158
#define OBFH_GUI_NAME_KillTimer(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 158, (((unsigned int)'K' << 0) | ((unsigned int)'i' << 8) | ((unsigned int)'l' << 16) | ((unsigned int)'l' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 158, (((unsigned int)'T' << 0) | ((unsigned int)'i' << 8) | ((unsigned int)'m' << 16) | ((unsigned int)'e' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 158, (((unsigned int)'r' << 0) | 0u | 0u | 0u)); \
    10u; \
})
#undef KillTimer
#define KillTimer(...) OBFH_API_CALL(0, KillTimer, __VA_ARGS__)

#define OBFH_GUI_ID_EnableWindow 159
#define OBFH_GUI_NAME_EnableWindow(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 159, (((unsigned int)'E' << 0) | ((unsigned int)'n' << 8) | ((unsigned int)'a' << 16) | ((unsigned int)'b' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 159, (((unsigned int)'l' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'W' << 16) | ((unsigned int)'i' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 159, (((unsigned int)'n' << 0) | ((unsigned int)'d' << 8) | ((unsigned int)'o' << 16) | ((unsigned int)'w' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 159, (0u | 0u | 0u | 0u)); \
    13u; \
})
#undef EnableWindow
#define EnableWindow(...) OBFH_API_CALL(0, EnableWindow, __VA_ARGS__)

#define OBFH_GUI_ID_IsWindow 160
#define OBFH_GUI_NAME_IsWindow(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 160, (((unsigned int)'I' << 0) | ((unsigned int)'s' << 8) | ((unsigned int)'W' << 16) | ((unsigned int)'i' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 160, (((unsigned int)'n' << 0) | ((unsigned int)'d' << 8) | ((unsigned int)'o' << 16) | ((unsigned int)'w' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 160, (0u | 0u | 0u | 0u)); \
    9u; \
})
#undef IsWindow
#define IsWindow(...) OBFH_API_CALL(0, IsWindow, __VA_ARGS__)

#define OBFH_GUI_ID_GetFileAttributesA 161
#define OBFH_GUI_NAME_GetFileAttributesA(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 161, (((unsigned int)'G' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'F' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 161, (((unsigned int)'i' << 0) | ((unsigned int)'l' << 8) | ((unsigned int)'e' << 16) | ((unsigned int)'A' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 161, (((unsigned int)'t' << 0) | ((unsigned int)'t' << 8) | ((unsigned int)'r' << 16) | ((unsigned int)'i' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 161, (((unsigned int)'b' << 0) | ((unsigned int)'u' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'e' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 16, 161, (((unsigned int)'s' << 0) | ((unsigned int)'A' << 8) | 0u | 0u)); \
    19u; \
})
#undef GetFileAttributesA
#define GetFileAttributesA(...) OBFH_API_CALL(2, GetFileAttributesA, __VA_ARGS__)

#define OBFH_GUI_ID_GetFileAttributesW 162
#define OBFH_GUI_NAME_GetFileAttributesW(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 162, (((unsigned int)'G' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'F' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 162, (((unsigned int)'i' << 0) | ((unsigned int)'l' << 8) | ((unsigned int)'e' << 16) | ((unsigned int)'A' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 162, (((unsigned int)'t' << 0) | ((unsigned int)'t' << 8) | ((unsigned int)'r' << 16) | ((unsigned int)'i' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 162, (((unsigned int)'b' << 0) | ((unsigned int)'u' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'e' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 16, 162, (((unsigned int)'s' << 0) | ((unsigned int)'W' << 8) | 0u | 0u)); \
    19u; \
})
#undef GetFileAttributesW
#define GetFileAttributesW(...) OBFH_API_CALL(2, GetFileAttributesW, __VA_ARGS__)

#define OBFH_GUI_ID_FindFirstFileA 163
#define OBFH_GUI_NAME_FindFirstFileA(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 163, (((unsigned int)'F' << 0) | ((unsigned int)'i' << 8) | ((unsigned int)'n' << 16) | ((unsigned int)'d' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 163, (((unsigned int)'F' << 0) | ((unsigned int)'i' << 8) | ((unsigned int)'r' << 16) | ((unsigned int)'s' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 163, (((unsigned int)'t' << 0) | ((unsigned int)'F' << 8) | ((unsigned int)'i' << 16) | ((unsigned int)'l' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 163, (((unsigned int)'e' << 0) | ((unsigned int)'A' << 8) | 0u | 0u)); \
    15u; \
})
#undef FindFirstFileA
#define FindFirstFileA(...) OBFH_API_CALL(2, FindFirstFileA, __VA_ARGS__)

#define OBFH_GUI_ID_FindFirstFileW 164
#define OBFH_GUI_NAME_FindFirstFileW(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 164, (((unsigned int)'F' << 0) | ((unsigned int)'i' << 8) | ((unsigned int)'n' << 16) | ((unsigned int)'d' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 164, (((unsigned int)'F' << 0) | ((unsigned int)'i' << 8) | ((unsigned int)'r' << 16) | ((unsigned int)'s' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 164, (((unsigned int)'t' << 0) | ((unsigned int)'F' << 8) | ((unsigned int)'i' << 16) | ((unsigned int)'l' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 164, (((unsigned int)'e' << 0) | ((unsigned int)'W' << 8) | 0u | 0u)); \
    15u; \
})
#undef FindFirstFileW
#define FindFirstFileW(...) OBFH_API_CALL(2, FindFirstFileW, __VA_ARGS__)

#define OBFH_GUI_ID_FindNextFileA 165
#define OBFH_GUI_NAME_FindNextFileA(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 165, (((unsigned int)'F' << 0) | ((unsigned int)'i' << 8) | ((unsigned int)'n' << 16) | ((unsigned int)'d' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 165, (((unsigned int)'N' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'x' << 16) | ((unsigned int)'t' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 165, (((unsigned int)'F' << 0) | ((unsigned int)'i' << 8) | ((unsigned int)'l' << 16) | ((unsigned int)'e' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 165, (((unsigned int)'A' << 0) | 0u | 0u | 0u)); \
    14u; \
})
#undef FindNextFileA
#define FindNextFileA(...) OBFH_API_CALL(2, FindNextFileA, __VA_ARGS__)

#define OBFH_GUI_ID_FindNextFileW 166
#define OBFH_GUI_NAME_FindNextFileW(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 166, (((unsigned int)'F' << 0) | ((unsigned int)'i' << 8) | ((unsigned int)'n' << 16) | ((unsigned int)'d' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 166, (((unsigned int)'N' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'x' << 16) | ((unsigned int)'t' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 166, (((unsigned int)'F' << 0) | ((unsigned int)'i' << 8) | ((unsigned int)'l' << 16) | ((unsigned int)'e' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 166, (((unsigned int)'W' << 0) | 0u | 0u | 0u)); \
    14u; \
})
#undef FindNextFileW
#define FindNextFileW(...) OBFH_API_CALL(2, FindNextFileW, __VA_ARGS__)

#define OBFH_GUI_ID_DeleteFileA 167
#define OBFH_GUI_NAME_DeleteFileA(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 167, (((unsigned int)'D' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'l' << 16) | ((unsigned int)'e' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 167, (((unsigned int)'t' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'F' << 16) | ((unsigned int)'i' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 167, (((unsigned int)'l' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'A' << 16) | 0u)); \
    12u; \
})
#undef DeleteFileA
#define DeleteFileA(...) OBFH_API_CALL(2, DeleteFileA, __VA_ARGS__)

#define OBFH_GUI_ID_DeleteFileW 168
#define OBFH_GUI_NAME_DeleteFileW(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 168, (((unsigned int)'D' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'l' << 16) | ((unsigned int)'e' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 168, (((unsigned int)'t' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'F' << 16) | ((unsigned int)'i' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 168, (((unsigned int)'l' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'W' << 16) | 0u)); \
    12u; \
})
#undef DeleteFileW
#define DeleteFileW(...) OBFH_API_CALL(2, DeleteFileW, __VA_ARGS__)

#define OBFH_GUI_ID_CopyFileA 169
#define OBFH_GUI_NAME_CopyFileA(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 169, (((unsigned int)'C' << 0) | ((unsigned int)'o' << 8) | ((unsigned int)'p' << 16) | ((unsigned int)'y' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 169, (((unsigned int)'F' << 0) | ((unsigned int)'i' << 8) | ((unsigned int)'l' << 16) | ((unsigned int)'e' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 169, (((unsigned int)'A' << 0) | 0u | 0u | 0u)); \
    10u; \
})
#undef CopyFileA
#define CopyFileA(...) OBFH_API_CALL(2, CopyFileA, __VA_ARGS__)

#define OBFH_GUI_ID_CopyFileW 170
#define OBFH_GUI_NAME_CopyFileW(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 170, (((unsigned int)'C' << 0) | ((unsigned int)'o' << 8) | ((unsigned int)'p' << 16) | ((unsigned int)'y' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 170, (((unsigned int)'F' << 0) | ((unsigned int)'i' << 8) | ((unsigned int)'l' << 16) | ((unsigned int)'e' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 170, (((unsigned int)'W' << 0) | 0u | 0u | 0u)); \
    10u; \
})
#undef CopyFileW
#define CopyFileW(...) OBFH_API_CALL(2, CopyFileW, __VA_ARGS__)

#define OBFH_GUI_ID_MoveFileExA 171
#define OBFH_GUI_NAME_MoveFileExA(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 171, (((unsigned int)'M' << 0) | ((unsigned int)'o' << 8) | ((unsigned int)'v' << 16) | ((unsigned int)'e' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 171, (((unsigned int)'F' << 0) | ((unsigned int)'i' << 8) | ((unsigned int)'l' << 16) | ((unsigned int)'e' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 171, (((unsigned int)'E' << 0) | ((unsigned int)'x' << 8) | ((unsigned int)'A' << 16) | 0u)); \
    12u; \
})
#undef MoveFileExA
#define MoveFileExA(...) OBFH_API_CALL(2, MoveFileExA, __VA_ARGS__)

#define OBFH_GUI_ID_MoveFileExW 172
#define OBFH_GUI_NAME_MoveFileExW(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 172, (((unsigned int)'M' << 0) | ((unsigned int)'o' << 8) | ((unsigned int)'v' << 16) | ((unsigned int)'e' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 172, (((unsigned int)'F' << 0) | ((unsigned int)'i' << 8) | ((unsigned int)'l' << 16) | ((unsigned int)'e' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 172, (((unsigned int)'E' << 0) | ((unsigned int)'x' << 8) | ((unsigned int)'W' << 16) | 0u)); \
    12u; \
})
#undef MoveFileExW
#define MoveFileExW(...) OBFH_API_CALL(2, MoveFileExW, __VA_ARGS__)

#define OBFH_GUI_ID_FormatMessageA 173
#define OBFH_GUI_NAME_FormatMessageA(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 173, (((unsigned int)'F' << 0) | ((unsigned int)'o' << 8) | ((unsigned int)'r' << 16) | ((unsigned int)'m' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 173, (((unsigned int)'a' << 0) | ((unsigned int)'t' << 8) | ((unsigned int)'M' << 16) | ((unsigned int)'e' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 173, (((unsigned int)'s' << 0) | ((unsigned int)'s' << 8) | ((unsigned int)'a' << 16) | ((unsigned int)'g' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 173, (((unsigned int)'e' << 0) | ((unsigned int)'A' << 8) | 0u | 0u)); \
    15u; \
})
#undef FormatMessageA
#define FormatMessageA(...) OBFH_API_CALL(2, FormatMessageA, __VA_ARGS__)

#define OBFH_GUI_ID_FormatMessageW 174
#define OBFH_GUI_NAME_FormatMessageW(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 174, (((unsigned int)'F' << 0) | ((unsigned int)'o' << 8) | ((unsigned int)'r' << 16) | ((unsigned int)'m' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 174, (((unsigned int)'a' << 0) | ((unsigned int)'t' << 8) | ((unsigned int)'M' << 16) | ((unsigned int)'e' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 174, (((unsigned int)'s' << 0) | ((unsigned int)'s' << 8) | ((unsigned int)'a' << 16) | ((unsigned int)'g' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 174, (((unsigned int)'e' << 0) | ((unsigned int)'W' << 8) | 0u | 0u)); \
    15u; \
})
#undef FormatMessageW
#define FormatMessageW(...) OBFH_API_CALL(2, FormatMessageW, __VA_ARGS__)

#define OBFH_GUI_ID_FindResourceA 175
#define OBFH_GUI_NAME_FindResourceA(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 175, (((unsigned int)'F' << 0) | ((unsigned int)'i' << 8) | ((unsigned int)'n' << 16) | ((unsigned int)'d' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 175, (((unsigned int)'R' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'s' << 16) | ((unsigned int)'o' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 175, (((unsigned int)'u' << 0) | ((unsigned int)'r' << 8) | ((unsigned int)'c' << 16) | ((unsigned int)'e' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 175, (((unsigned int)'A' << 0) | 0u | 0u | 0u)); \
    14u; \
})
#undef FindResourceA
#define FindResourceA(...) OBFH_API_CALL(2, FindResourceA, __VA_ARGS__)

#define OBFH_GUI_ID_FindResourceW 176
#define OBFH_GUI_NAME_FindResourceW(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 176, (((unsigned int)'F' << 0) | ((unsigned int)'i' << 8) | ((unsigned int)'n' << 16) | ((unsigned int)'d' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 176, (((unsigned int)'R' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'s' << 16) | ((unsigned int)'o' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 176, (((unsigned int)'u' << 0) | ((unsigned int)'r' << 8) | ((unsigned int)'c' << 16) | ((unsigned int)'e' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 176, (((unsigned int)'W' << 0) | 0u | 0u | 0u)); \
    14u; \
})
#undef FindResourceW
#define FindResourceW(...) OBFH_API_CALL(2, FindResourceW, __VA_ARGS__)

#define OBFH_GUI_ID_PostMessageA 177
#define OBFH_GUI_NAME_PostMessageA(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 177, (((unsigned int)'P' << 0) | ((unsigned int)'o' << 8) | ((unsigned int)'s' << 16) | ((unsigned int)'t' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 177, (((unsigned int)'M' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'s' << 16) | ((unsigned int)'s' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 177, (((unsigned int)'a' << 0) | ((unsigned int)'g' << 8) | ((unsigned int)'e' << 16) | ((unsigned int)'A' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 177, (0u | 0u | 0u | 0u)); \
    13u; \
})
#undef PostMessageA
#define PostMessageA(...) OBFH_API_CALL(0, PostMessageA, __VA_ARGS__)

#define OBFH_GUI_ID_PostMessageW 178
#define OBFH_GUI_NAME_PostMessageW(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 178, (((unsigned int)'P' << 0) | ((unsigned int)'o' << 8) | ((unsigned int)'s' << 16) | ((unsigned int)'t' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 178, (((unsigned int)'M' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'s' << 16) | ((unsigned int)'s' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 178, (((unsigned int)'a' << 0) | ((unsigned int)'g' << 8) | ((unsigned int)'e' << 16) | ((unsigned int)'W' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 178, (0u | 0u | 0u | 0u)); \
    13u; \
})
#undef PostMessageW
#define PostMessageW(...) OBFH_API_CALL(0, PostMessageW, __VA_ARGS__)

#define OBFH_GUI_ID_GetWindowLongA 179
#define OBFH_GUI_NAME_GetWindowLongA(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 179, (((unsigned int)'G' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'W' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 179, (((unsigned int)'i' << 0) | ((unsigned int)'n' << 8) | ((unsigned int)'d' << 16) | ((unsigned int)'o' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 179, (((unsigned int)'w' << 0) | ((unsigned int)'L' << 8) | ((unsigned int)'o' << 16) | ((unsigned int)'n' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 179, (((unsigned int)'g' << 0) | ((unsigned int)'A' << 8) | 0u | 0u)); \
    15u; \
})
#undef GetWindowLongA
#define GetWindowLongA(...) OBFH_API_CALL(0, GetWindowLongA, __VA_ARGS__)

#define OBFH_GUI_ID_GetWindowLongW 180
#define OBFH_GUI_NAME_GetWindowLongW(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 180, (((unsigned int)'G' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'W' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 180, (((unsigned int)'i' << 0) | ((unsigned int)'n' << 8) | ((unsigned int)'d' << 16) | ((unsigned int)'o' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 180, (((unsigned int)'w' << 0) | ((unsigned int)'L' << 8) | ((unsigned int)'o' << 16) | ((unsigned int)'n' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 180, (((unsigned int)'g' << 0) | ((unsigned int)'W' << 8) | 0u | 0u)); \
    15u; \
})
#undef GetWindowLongW
#define GetWindowLongW(...) OBFH_API_CALL(0, GetWindowLongW, __VA_ARGS__)

#define OBFH_GUI_ID_SetWindowLongA 181
#define OBFH_GUI_NAME_SetWindowLongA(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 181, (((unsigned int)'S' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'W' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 181, (((unsigned int)'i' << 0) | ((unsigned int)'n' << 8) | ((unsigned int)'d' << 16) | ((unsigned int)'o' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 181, (((unsigned int)'w' << 0) | ((unsigned int)'L' << 8) | ((unsigned int)'o' << 16) | ((unsigned int)'n' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 181, (((unsigned int)'g' << 0) | ((unsigned int)'A' << 8) | 0u | 0u)); \
    15u; \
})
#undef SetWindowLongA
#define SetWindowLongA(...) OBFH_API_CALL(0, SetWindowLongA, __VA_ARGS__)

#define OBFH_GUI_ID_SetWindowLongW 182
#define OBFH_GUI_NAME_SetWindowLongW(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 182, (((unsigned int)'S' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'W' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 182, (((unsigned int)'i' << 0) | ((unsigned int)'n' << 8) | ((unsigned int)'d' << 16) | ((unsigned int)'o' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 182, (((unsigned int)'w' << 0) | ((unsigned int)'L' << 8) | ((unsigned int)'o' << 16) | ((unsigned int)'n' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 182, (((unsigned int)'g' << 0) | ((unsigned int)'W' << 8) | 0u | 0u)); \
    15u; \
})
#undef SetWindowLongW
#define SetWindowLongW(...) OBFH_API_CALL(0, SetWindowLongW, __VA_ARGS__)

#define OBFH_GUI_ID_LoadStringA 183
#define OBFH_GUI_NAME_LoadStringA(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 183, (((unsigned int)'L' << 0) | ((unsigned int)'o' << 8) | ((unsigned int)'a' << 16) | ((unsigned int)'d' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 183, (((unsigned int)'S' << 0) | ((unsigned int)'t' << 8) | ((unsigned int)'r' << 16) | ((unsigned int)'i' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 183, (((unsigned int)'n' << 0) | ((unsigned int)'g' << 8) | ((unsigned int)'A' << 16) | 0u)); \
    12u; \
})
#undef LoadStringA
#define LoadStringA(...) OBFH_API_CALL(0, LoadStringA, __VA_ARGS__)

#define OBFH_GUI_ID_LoadStringW 184
#define OBFH_GUI_NAME_LoadStringW(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 184, (((unsigned int)'L' << 0) | ((unsigned int)'o' << 8) | ((unsigned int)'a' << 16) | ((unsigned int)'d' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 184, (((unsigned int)'S' << 0) | ((unsigned int)'t' << 8) | ((unsigned int)'r' << 16) | ((unsigned int)'i' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 184, (((unsigned int)'n' << 0) | ((unsigned int)'g' << 8) | ((unsigned int)'W' << 16) | 0u)); \
    12u; \
})
#undef LoadStringW
#define LoadStringW(...) OBFH_API_CALL(0, LoadStringW, __VA_ARGS__)

#if defined(__x86_64__)
#define OBFH_GUI_ID_GetWindowLongPtrA 185
#define OBFH_GUI_NAME_GetWindowLongPtrA(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 185, (((unsigned int)'G' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'W' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 185, (((unsigned int)'i' << 0) | ((unsigned int)'n' << 8) | ((unsigned int)'d' << 16) | ((unsigned int)'o' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 185, (((unsigned int)'w' << 0) | ((unsigned int)'L' << 8) | ((unsigned int)'o' << 16) | ((unsigned int)'n' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 185, (((unsigned int)'g' << 0) | ((unsigned int)'P' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'r' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 16, 185, (((unsigned int)'A' << 0) | 0u | 0u | 0u)); \
    18u; \
})
#undef GetWindowLongPtrA
#define GetWindowLongPtrA(...) OBFH_API_CALL(0, GetWindowLongPtrA, __VA_ARGS__)
#endif

#if defined(__x86_64__)
#define OBFH_GUI_ID_GetWindowLongPtrW 186
#define OBFH_GUI_NAME_GetWindowLongPtrW(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 186, (((unsigned int)'G' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'W' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 186, (((unsigned int)'i' << 0) | ((unsigned int)'n' << 8) | ((unsigned int)'d' << 16) | ((unsigned int)'o' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 186, (((unsigned int)'w' << 0) | ((unsigned int)'L' << 8) | ((unsigned int)'o' << 16) | ((unsigned int)'n' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 186, (((unsigned int)'g' << 0) | ((unsigned int)'P' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'r' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 16, 186, (((unsigned int)'W' << 0) | 0u | 0u | 0u)); \
    18u; \
})
#undef GetWindowLongPtrW
#define GetWindowLongPtrW(...) OBFH_API_CALL(0, GetWindowLongPtrW, __VA_ARGS__)
#endif

#if defined(__x86_64__)
#define OBFH_GUI_ID_SetWindowLongPtrA 187
#define OBFH_GUI_NAME_SetWindowLongPtrA(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 187, (((unsigned int)'S' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'W' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 187, (((unsigned int)'i' << 0) | ((unsigned int)'n' << 8) | ((unsigned int)'d' << 16) | ((unsigned int)'o' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 187, (((unsigned int)'w' << 0) | ((unsigned int)'L' << 8) | ((unsigned int)'o' << 16) | ((unsigned int)'n' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 187, (((unsigned int)'g' << 0) | ((unsigned int)'P' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'r' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 16, 187, (((unsigned int)'A' << 0) | 0u | 0u | 0u)); \
    18u; \
})
#undef SetWindowLongPtrA
#define SetWindowLongPtrA(...) OBFH_API_CALL(0, SetWindowLongPtrA, __VA_ARGS__)
#endif

#if defined(__x86_64__)
#define OBFH_GUI_ID_SetWindowLongPtrW 188
#define OBFH_GUI_NAME_SetWindowLongPtrW(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 188, (((unsigned int)'S' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'W' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 188, (((unsigned int)'i' << 0) | ((unsigned int)'n' << 8) | ((unsigned int)'d' << 16) | ((unsigned int)'o' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 188, (((unsigned int)'w' << 0) | ((unsigned int)'L' << 8) | ((unsigned int)'o' << 16) | ((unsigned int)'n' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 188, (((unsigned int)'g' << 0) | ((unsigned int)'P' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'r' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 16, 188, (((unsigned int)'W' << 0) | 0u | 0u | 0u)); \
    18u; \
})
#undef SetWindowLongPtrW
#define SetWindowLongPtrW(...) OBFH_API_CALL(0, SetWindowLongPtrW, __VA_ARGS__)
#endif

#define OBFH_GUI_ID_AppendMenuA 0
#define OBFH_GUI_NAME_AppendMenuA(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 0, (((unsigned int)'A' << 0) | ((unsigned int)'p' << 8) | ((unsigned int)'p' << 16) | ((unsigned int)'e' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 0, (((unsigned int)'n' << 0) | ((unsigned int)'d' << 8) | ((unsigned int)'M' << 16) | ((unsigned int)'e' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 0, (((unsigned int)'n' << 0) | ((unsigned int)'u' << 8) | ((unsigned int)'A' << 16) | 0u)); \
    12u; \
})
#define AppendMenuA(...) OBFH_API_CALL(0, AppendMenuA, __VA_ARGS__)

#define OBFH_GUI_ID_AppendMenuW 1
#define OBFH_GUI_NAME_AppendMenuW(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 1, (((unsigned int)'A' << 0) | ((unsigned int)'p' << 8) | ((unsigned int)'p' << 16) | ((unsigned int)'e' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 1, (((unsigned int)'n' << 0) | ((unsigned int)'d' << 8) | ((unsigned int)'M' << 16) | ((unsigned int)'e' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 1, (((unsigned int)'n' << 0) | ((unsigned int)'u' << 8) | ((unsigned int)'W' << 16) | 0u)); \
    12u; \
})
#define AppendMenuW(...) OBFH_API_CALL(0, AppendMenuW, __VA_ARGS__)

#define OBFH_GUI_ID_CheckMenuItem 2
#define OBFH_GUI_NAME_CheckMenuItem(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 2, (((unsigned int)'C' << 0) | ((unsigned int)'h' << 8) | ((unsigned int)'e' << 16) | ((unsigned int)'c' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 2, (((unsigned int)'k' << 0) | ((unsigned int)'M' << 8) | ((unsigned int)'e' << 16) | ((unsigned int)'n' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 2, (((unsigned int)'u' << 0) | ((unsigned int)'I' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'e' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 2, (((unsigned int)'m' << 0) | 0u | 0u | 0u)); \
    14u; \
})
#define CheckMenuItem(...) OBFH_API_CALL(0, CheckMenuItem, __VA_ARGS__)

#define OBFH_GUI_ID_CloseClipboard 3
#define OBFH_GUI_NAME_CloseClipboard(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 3, (((unsigned int)'C' << 0) | ((unsigned int)'l' << 8) | ((unsigned int)'o' << 16) | ((unsigned int)'s' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 3, (((unsigned int)'e' << 0) | ((unsigned int)'C' << 8) | ((unsigned int)'l' << 16) | ((unsigned int)'i' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 3, (((unsigned int)'p' << 0) | ((unsigned int)'b' << 8) | ((unsigned int)'o' << 16) | ((unsigned int)'a' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 3, (((unsigned int)'r' << 0) | ((unsigned int)'d' << 8) | 0u | 0u)); \
    15u; \
})
#define CloseClipboard(...) OBFH_API_CALL(0, CloseClipboard, __VA_ARGS__)

#define OBFH_GUI_ID_CreateMenu 4
#define OBFH_GUI_NAME_CreateMenu(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 4, (((unsigned int)'C' << 0) | ((unsigned int)'r' << 8) | ((unsigned int)'e' << 16) | ((unsigned int)'a' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 4, (((unsigned int)'t' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'M' << 16) | ((unsigned int)'e' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 4, (((unsigned int)'n' << 0) | ((unsigned int)'u' << 8) | 0u | 0u)); \
    11u; \
})
#define CreateMenu(...) OBFH_API_CALL(0, CreateMenu, __VA_ARGS__)

#define OBFH_GUI_ID_CreatePopupMenu 5
#define OBFH_GUI_NAME_CreatePopupMenu(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 5, (((unsigned int)'C' << 0) | ((unsigned int)'r' << 8) | ((unsigned int)'e' << 16) | ((unsigned int)'a' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 5, (((unsigned int)'t' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'P' << 16) | ((unsigned int)'o' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 5, (((unsigned int)'p' << 0) | ((unsigned int)'u' << 8) | ((unsigned int)'p' << 16) | ((unsigned int)'M' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 5, (((unsigned int)'e' << 0) | ((unsigned int)'n' << 8) | ((unsigned int)'u' << 16) | 0u)); \
    16u; \
})
#define CreatePopupMenu(...) OBFH_API_CALL(0, CreatePopupMenu, __VA_ARGS__)

#define OBFH_GUI_ID_CreateWindowExA 6
#define OBFH_GUI_NAME_CreateWindowExA(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 6, (((unsigned int)'C' << 0) | ((unsigned int)'r' << 8) | ((unsigned int)'e' << 16) | ((unsigned int)'a' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 6, (((unsigned int)'t' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'W' << 16) | ((unsigned int)'i' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 6, (((unsigned int)'n' << 0) | ((unsigned int)'d' << 8) | ((unsigned int)'o' << 16) | ((unsigned int)'w' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 6, (((unsigned int)'E' << 0) | ((unsigned int)'x' << 8) | ((unsigned int)'A' << 16) | 0u)); \
    16u; \
})
#define CreateWindowExA(...) OBFH_API_CALL(0, CreateWindowExA, __VA_ARGS__)

#define OBFH_GUI_ID_CreateWindowExW 7
#define OBFH_GUI_NAME_CreateWindowExW(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 7, (((unsigned int)'C' << 0) | ((unsigned int)'r' << 8) | ((unsigned int)'e' << 16) | ((unsigned int)'a' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 7, (((unsigned int)'t' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'W' << 16) | ((unsigned int)'i' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 7, (((unsigned int)'n' << 0) | ((unsigned int)'d' << 8) | ((unsigned int)'o' << 16) | ((unsigned int)'w' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 7, (((unsigned int)'E' << 0) | ((unsigned int)'x' << 8) | ((unsigned int)'W' << 16) | 0u)); \
    16u; \
})
#define CreateWindowExW(...) OBFH_API_CALL(0, CreateWindowExW, __VA_ARGS__)

#define OBFH_GUI_ID_DefWindowProcA 8
#define OBFH_GUI_NAME_DefWindowProcA(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 8, (((unsigned int)'D' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'f' << 16) | ((unsigned int)'W' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 8, (((unsigned int)'i' << 0) | ((unsigned int)'n' << 8) | ((unsigned int)'d' << 16) | ((unsigned int)'o' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 8, (((unsigned int)'w' << 0) | ((unsigned int)'P' << 8) | ((unsigned int)'r' << 16) | ((unsigned int)'o' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 8, (((unsigned int)'c' << 0) | ((unsigned int)'A' << 8) | 0u | 0u)); \
    15u; \
})
#define DefWindowProcA(...) OBFH_API_CALL(0, DefWindowProcA, __VA_ARGS__)

#define OBFH_GUI_ID_DefWindowProcW 9
#define OBFH_GUI_NAME_DefWindowProcW(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 9, (((unsigned int)'D' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'f' << 16) | ((unsigned int)'W' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 9, (((unsigned int)'i' << 0) | ((unsigned int)'n' << 8) | ((unsigned int)'d' << 16) | ((unsigned int)'o' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 9, (((unsigned int)'w' << 0) | ((unsigned int)'P' << 8) | ((unsigned int)'r' << 16) | ((unsigned int)'o' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 9, (((unsigned int)'c' << 0) | ((unsigned int)'W' << 8) | 0u | 0u)); \
    15u; \
})
#define DefWindowProcW(...) OBFH_API_CALL(0, DefWindowProcW, __VA_ARGS__)

#define OBFH_GUI_ID_DestroyWindow 10
#define OBFH_GUI_NAME_DestroyWindow(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 10, (((unsigned int)'D' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'s' << 16) | ((unsigned int)'t' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 10, (((unsigned int)'r' << 0) | ((unsigned int)'o' << 8) | ((unsigned int)'y' << 16) | ((unsigned int)'W' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 10, (((unsigned int)'i' << 0) | ((unsigned int)'n' << 8) | ((unsigned int)'d' << 16) | ((unsigned int)'o' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 10, (((unsigned int)'w' << 0) | 0u | 0u | 0u)); \
    14u; \
})
#define DestroyWindow(...) OBFH_API_CALL(0, DestroyWindow, __VA_ARGS__)

#define OBFH_GUI_ID_DispatchMessageA 11
#define OBFH_GUI_NAME_DispatchMessageA(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 11, (((unsigned int)'D' << 0) | ((unsigned int)'i' << 8) | ((unsigned int)'s' << 16) | ((unsigned int)'p' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 11, (((unsigned int)'a' << 0) | ((unsigned int)'t' << 8) | ((unsigned int)'c' << 16) | ((unsigned int)'h' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 11, (((unsigned int)'M' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'s' << 16) | ((unsigned int)'s' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 11, (((unsigned int)'a' << 0) | ((unsigned int)'g' << 8) | ((unsigned int)'e' << 16) | ((unsigned int)'A' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 16, 11, (0u | 0u | 0u | 0u)); \
    17u; \
})
#define DispatchMessageA(...) OBFH_API_CALL(0, DispatchMessageA, __VA_ARGS__)

#define OBFH_GUI_ID_DispatchMessageW 12
#define OBFH_GUI_NAME_DispatchMessageW(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 12, (((unsigned int)'D' << 0) | ((unsigned int)'i' << 8) | ((unsigned int)'s' << 16) | ((unsigned int)'p' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 12, (((unsigned int)'a' << 0) | ((unsigned int)'t' << 8) | ((unsigned int)'c' << 16) | ((unsigned int)'h' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 12, (((unsigned int)'M' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'s' << 16) | ((unsigned int)'s' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 12, (((unsigned int)'a' << 0) | ((unsigned int)'g' << 8) | ((unsigned int)'e' << 16) | ((unsigned int)'W' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 16, 12, (0u | 0u | 0u | 0u)); \
    17u; \
})
#define DispatchMessageW(...) OBFH_API_CALL(0, DispatchMessageW, __VA_ARGS__)

#define OBFH_GUI_ID_EmptyClipboard 13
#define OBFH_GUI_NAME_EmptyClipboard(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 13, (((unsigned int)'E' << 0) | ((unsigned int)'m' << 8) | ((unsigned int)'p' << 16) | ((unsigned int)'t' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 13, (((unsigned int)'y' << 0) | ((unsigned int)'C' << 8) | ((unsigned int)'l' << 16) | ((unsigned int)'i' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 13, (((unsigned int)'p' << 0) | ((unsigned int)'b' << 8) | ((unsigned int)'o' << 16) | ((unsigned int)'a' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 13, (((unsigned int)'r' << 0) | ((unsigned int)'d' << 8) | 0u | 0u)); \
    15u; \
})
#define EmptyClipboard(...) OBFH_API_CALL(0, EmptyClipboard, __VA_ARGS__)

#define OBFH_GUI_ID_GetClientRect 14
#define OBFH_GUI_NAME_GetClientRect(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 14, (((unsigned int)'G' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'C' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 14, (((unsigned int)'l' << 0) | ((unsigned int)'i' << 8) | ((unsigned int)'e' << 16) | ((unsigned int)'n' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 14, (((unsigned int)'t' << 0) | ((unsigned int)'R' << 8) | ((unsigned int)'e' << 16) | ((unsigned int)'c' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 14, (((unsigned int)'t' << 0) | 0u | 0u | 0u)); \
    14u; \
})
#define GetClientRect(...) OBFH_API_CALL(0, GetClientRect, __VA_ARGS__)

#define OBFH_GUI_ID_GetDC 15
#define OBFH_GUI_NAME_GetDC(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 15, (((unsigned int)'G' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'D' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 15, (((unsigned int)'C' << 0) | 0u | 0u | 0u)); \
    6u; \
})
#define GetDC(...) OBFH_API_CALL(0, GetDC, __VA_ARGS__)

#define OBFH_GUI_ID_GetDlgItem 16
#define OBFH_GUI_NAME_GetDlgItem(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 16, (((unsigned int)'G' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'D' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 16, (((unsigned int)'l' << 0) | ((unsigned int)'g' << 8) | ((unsigned int)'I' << 16) | ((unsigned int)'t' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 16, (((unsigned int)'e' << 0) | ((unsigned int)'m' << 8) | 0u | 0u)); \
    11u; \
})
#define GetDlgItem(...) OBFH_API_CALL(0, GetDlgItem, __VA_ARGS__)

#define OBFH_GUI_ID_GetKeyState 17
#define OBFH_GUI_NAME_GetKeyState(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 17, (((unsigned int)'G' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'K' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 17, (((unsigned int)'e' << 0) | ((unsigned int)'y' << 8) | ((unsigned int)'S' << 16) | ((unsigned int)'t' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 17, (((unsigned int)'a' << 0) | ((unsigned int)'t' << 8) | ((unsigned int)'e' << 16) | 0u)); \
    12u; \
})
#define GetKeyState(...) OBFH_API_CALL(0, GetKeyState, __VA_ARGS__)

#define OBFH_GUI_ID_GetMenu 18
#define OBFH_GUI_NAME_GetMenu(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 18, (((unsigned int)'G' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'M' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 18, (((unsigned int)'e' << 0) | ((unsigned int)'n' << 8) | ((unsigned int)'u' << 16) | 0u)); \
    8u; \
})
#define GetMenu(...) OBFH_API_CALL(0, GetMenu, __VA_ARGS__)

#define OBFH_GUI_ID_GetMessageA 19
#define OBFH_GUI_NAME_GetMessageA(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 19, (((unsigned int)'G' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'M' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 19, (((unsigned int)'e' << 0) | ((unsigned int)'s' << 8) | ((unsigned int)'s' << 16) | ((unsigned int)'a' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 19, (((unsigned int)'g' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'A' << 16) | 0u)); \
    12u; \
})
#define GetMessageA(...) OBFH_API_CALL(0, GetMessageA, __VA_ARGS__)

#define OBFH_GUI_ID_GetMessageW 20
#define OBFH_GUI_NAME_GetMessageW(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 20, (((unsigned int)'G' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'M' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 20, (((unsigned int)'e' << 0) | ((unsigned int)'s' << 8) | ((unsigned int)'s' << 16) | ((unsigned int)'a' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 20, (((unsigned int)'g' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'W' << 16) | 0u)); \
    12u; \
})
#define GetMessageW(...) OBFH_API_CALL(0, GetMessageW, __VA_ARGS__)

#define OBFH_GUI_ID_GetSysColor 21
#define OBFH_GUI_NAME_GetSysColor(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 21, (((unsigned int)'G' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'S' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 21, (((unsigned int)'y' << 0) | ((unsigned int)'s' << 8) | ((unsigned int)'C' << 16) | ((unsigned int)'o' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 21, (((unsigned int)'l' << 0) | ((unsigned int)'o' << 8) | ((unsigned int)'r' << 16) | 0u)); \
    12u; \
})
#define GetSysColor(...) OBFH_API_CALL(0, GetSysColor, __VA_ARGS__)

#define OBFH_GUI_ID_GetSysColorBrush 22
#define OBFH_GUI_NAME_GetSysColorBrush(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 22, (((unsigned int)'G' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'S' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 22, (((unsigned int)'y' << 0) | ((unsigned int)'s' << 8) | ((unsigned int)'C' << 16) | ((unsigned int)'o' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 22, (((unsigned int)'l' << 0) | ((unsigned int)'o' << 8) | ((unsigned int)'r' << 16) | ((unsigned int)'B' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 22, (((unsigned int)'r' << 0) | ((unsigned int)'u' << 8) | ((unsigned int)'s' << 16) | ((unsigned int)'h' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 16, 22, (0u | 0u | 0u | 0u)); \
    17u; \
})
#define GetSysColorBrush(...) OBFH_API_CALL(0, GetSysColorBrush, __VA_ARGS__)

#define OBFH_GUI_ID_GetSystemMetrics 23
#define OBFH_GUI_NAME_GetSystemMetrics(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 23, (((unsigned int)'G' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'S' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 23, (((unsigned int)'y' << 0) | ((unsigned int)'s' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'e' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 23, (((unsigned int)'m' << 0) | ((unsigned int)'M' << 8) | ((unsigned int)'e' << 16) | ((unsigned int)'t' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 23, (((unsigned int)'r' << 0) | ((unsigned int)'i' << 8) | ((unsigned int)'c' << 16) | ((unsigned int)'s' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 16, 23, (0u | 0u | 0u | 0u)); \
    17u; \
})
#define GetSystemMetrics(...) OBFH_API_CALL(0, GetSystemMetrics, __VA_ARGS__)

#define OBFH_GUI_ID_GetWindowRect 24
#define OBFH_GUI_NAME_GetWindowRect(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 24, (((unsigned int)'G' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'W' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 24, (((unsigned int)'i' << 0) | ((unsigned int)'n' << 8) | ((unsigned int)'d' << 16) | ((unsigned int)'o' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 24, (((unsigned int)'w' << 0) | ((unsigned int)'R' << 8) | ((unsigned int)'e' << 16) | ((unsigned int)'c' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 24, (((unsigned int)'t' << 0) | 0u | 0u | 0u)); \
    14u; \
})
#define GetWindowRect(...) OBFH_API_CALL(0, GetWindowRect, __VA_ARGS__)

#define OBFH_GUI_ID_GetWindowTextA 25
#define OBFH_GUI_NAME_GetWindowTextA(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 25, (((unsigned int)'G' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'W' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 25, (((unsigned int)'i' << 0) | ((unsigned int)'n' << 8) | ((unsigned int)'d' << 16) | ((unsigned int)'o' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 25, (((unsigned int)'w' << 0) | ((unsigned int)'T' << 8) | ((unsigned int)'e' << 16) | ((unsigned int)'x' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 25, (((unsigned int)'t' << 0) | ((unsigned int)'A' << 8) | 0u | 0u)); \
    15u; \
})
#define GetWindowTextA(...) OBFH_API_CALL(0, GetWindowTextA, __VA_ARGS__)

#define OBFH_GUI_ID_GetWindowTextW 26
#define OBFH_GUI_NAME_GetWindowTextW(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 26, (((unsigned int)'G' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'W' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 26, (((unsigned int)'i' << 0) | ((unsigned int)'n' << 8) | ((unsigned int)'d' << 16) | ((unsigned int)'o' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 26, (((unsigned int)'w' << 0) | ((unsigned int)'T' << 8) | ((unsigned int)'e' << 16) | ((unsigned int)'x' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 26, (((unsigned int)'t' << 0) | ((unsigned int)'W' << 8) | 0u | 0u)); \
    15u; \
})
#define GetWindowTextW(...) OBFH_API_CALL(0, GetWindowTextW, __VA_ARGS__)

#define OBFH_GUI_ID_GetWindowTextLengthA 27
#define OBFH_GUI_NAME_GetWindowTextLengthA(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 27, (((unsigned int)'G' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'W' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 27, (((unsigned int)'i' << 0) | ((unsigned int)'n' << 8) | ((unsigned int)'d' << 16) | ((unsigned int)'o' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 27, (((unsigned int)'w' << 0) | ((unsigned int)'T' << 8) | ((unsigned int)'e' << 16) | ((unsigned int)'x' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 27, (((unsigned int)'t' << 0) | ((unsigned int)'L' << 8) | ((unsigned int)'e' << 16) | ((unsigned int)'n' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 16, 27, (((unsigned int)'g' << 0) | ((unsigned int)'t' << 8) | ((unsigned int)'h' << 16) | ((unsigned int)'A' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 20, 27, (0u | 0u | 0u | 0u)); \
    21u; \
})
#define GetWindowTextLengthA(...) OBFH_API_CALL(0, GetWindowTextLengthA, __VA_ARGS__)

#define OBFH_GUI_ID_GetWindowTextLengthW 28
#define OBFH_GUI_NAME_GetWindowTextLengthW(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 28, (((unsigned int)'G' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'W' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 28, (((unsigned int)'i' << 0) | ((unsigned int)'n' << 8) | ((unsigned int)'d' << 16) | ((unsigned int)'o' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 28, (((unsigned int)'w' << 0) | ((unsigned int)'T' << 8) | ((unsigned int)'e' << 16) | ((unsigned int)'x' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 28, (((unsigned int)'t' << 0) | ((unsigned int)'L' << 8) | ((unsigned int)'e' << 16) | ((unsigned int)'n' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 16, 28, (((unsigned int)'g' << 0) | ((unsigned int)'t' << 8) | ((unsigned int)'h' << 16) | ((unsigned int)'W' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 20, 28, (0u | 0u | 0u | 0u)); \
    21u; \
})
#define GetWindowTextLengthW(...) OBFH_API_CALL(0, GetWindowTextLengthW, __VA_ARGS__)

#define OBFH_GUI_ID_IsDialogMessageA 29
#define OBFH_GUI_NAME_IsDialogMessageA(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 29, (((unsigned int)'I' << 0) | ((unsigned int)'s' << 8) | ((unsigned int)'D' << 16) | ((unsigned int)'i' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 29, (((unsigned int)'a' << 0) | ((unsigned int)'l' << 8) | ((unsigned int)'o' << 16) | ((unsigned int)'g' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 29, (((unsigned int)'M' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'s' << 16) | ((unsigned int)'s' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 29, (((unsigned int)'a' << 0) | ((unsigned int)'g' << 8) | ((unsigned int)'e' << 16) | ((unsigned int)'A' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 16, 29, (0u | 0u | 0u | 0u)); \
    17u; \
})
#define IsDialogMessageA(...) OBFH_API_CALL(0, IsDialogMessageA, __VA_ARGS__)

#define OBFH_GUI_ID_IsDialogMessageW 30
#define OBFH_GUI_NAME_IsDialogMessageW(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 30, (((unsigned int)'I' << 0) | ((unsigned int)'s' << 8) | ((unsigned int)'D' << 16) | ((unsigned int)'i' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 30, (((unsigned int)'a' << 0) | ((unsigned int)'l' << 8) | ((unsigned int)'o' << 16) | ((unsigned int)'g' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 30, (((unsigned int)'M' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'s' << 16) | ((unsigned int)'s' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 30, (((unsigned int)'a' << 0) | ((unsigned int)'g' << 8) | ((unsigned int)'e' << 16) | ((unsigned int)'W' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 16, 30, (0u | 0u | 0u | 0u)); \
    17u; \
})
#define IsDialogMessageW(...) OBFH_API_CALL(0, IsDialogMessageW, __VA_ARGS__)

#define OBFH_GUI_ID_LoadCursorA 31
#define OBFH_GUI_NAME_LoadCursorA(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 31, (((unsigned int)'L' << 0) | ((unsigned int)'o' << 8) | ((unsigned int)'a' << 16) | ((unsigned int)'d' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 31, (((unsigned int)'C' << 0) | ((unsigned int)'u' << 8) | ((unsigned int)'r' << 16) | ((unsigned int)'s' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 31, (((unsigned int)'o' << 0) | ((unsigned int)'r' << 8) | ((unsigned int)'A' << 16) | 0u)); \
    12u; \
})
#define LoadCursorA(...) OBFH_API_CALL(0, LoadCursorA, __VA_ARGS__)

#define OBFH_GUI_ID_LoadCursorW 32
#define OBFH_GUI_NAME_LoadCursorW(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 32, (((unsigned int)'L' << 0) | ((unsigned int)'o' << 8) | ((unsigned int)'a' << 16) | ((unsigned int)'d' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 32, (((unsigned int)'C' << 0) | ((unsigned int)'u' << 8) | ((unsigned int)'r' << 16) | ((unsigned int)'s' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 32, (((unsigned int)'o' << 0) | ((unsigned int)'r' << 8) | ((unsigned int)'W' << 16) | 0u)); \
    12u; \
})
#define LoadCursorW(...) OBFH_API_CALL(0, LoadCursorW, __VA_ARGS__)

#define OBFH_GUI_ID_LoadIconA 33
#define OBFH_GUI_NAME_LoadIconA(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 33, (((unsigned int)'L' << 0) | ((unsigned int)'o' << 8) | ((unsigned int)'a' << 16) | ((unsigned int)'d' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 33, (((unsigned int)'I' << 0) | ((unsigned int)'c' << 8) | ((unsigned int)'o' << 16) | ((unsigned int)'n' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 33, (((unsigned int)'A' << 0) | 0u | 0u | 0u)); \
    10u; \
})
#define LoadIconA(...) OBFH_API_CALL(0, LoadIconA, __VA_ARGS__)

#define OBFH_GUI_ID_LoadIconW 34
#define OBFH_GUI_NAME_LoadIconW(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 34, (((unsigned int)'L' << 0) | ((unsigned int)'o' << 8) | ((unsigned int)'a' << 16) | ((unsigned int)'d' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 34, (((unsigned int)'I' << 0) | ((unsigned int)'c' << 8) | ((unsigned int)'o' << 16) | ((unsigned int)'n' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 34, (((unsigned int)'W' << 0) | 0u | 0u | 0u)); \
    10u; \
})
#define LoadIconW(...) OBFH_API_CALL(0, LoadIconW, __VA_ARGS__)

#define OBFH_GUI_ID_MessageBeep 35
#define OBFH_GUI_NAME_MessageBeep(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 35, (((unsigned int)'M' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'s' << 16) | ((unsigned int)'s' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 35, (((unsigned int)'a' << 0) | ((unsigned int)'g' << 8) | ((unsigned int)'e' << 16) | ((unsigned int)'B' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 35, (((unsigned int)'e' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'p' << 16) | 0u)); \
    12u; \
})
#define MessageBeep(...) OBFH_API_CALL(0, MessageBeep, __VA_ARGS__)

#define OBFH_GUI_ID_MessageBoxA 36
#define OBFH_GUI_NAME_MessageBoxA(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 36, (((unsigned int)'M' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'s' << 16) | ((unsigned int)'s' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 36, (((unsigned int)'a' << 0) | ((unsigned int)'g' << 8) | ((unsigned int)'e' << 16) | ((unsigned int)'B' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 36, (((unsigned int)'o' << 0) | ((unsigned int)'x' << 8) | ((unsigned int)'A' << 16) | 0u)); \
    12u; \
})
#define MessageBoxA(...) OBFH_API_CALL(0, MessageBoxA, __VA_ARGS__)

#define OBFH_GUI_ID_MessageBoxW 37
#define OBFH_GUI_NAME_MessageBoxW(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 37, (((unsigned int)'M' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'s' << 16) | ((unsigned int)'s' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 37, (((unsigned int)'a' << 0) | ((unsigned int)'g' << 8) | ((unsigned int)'e' << 16) | ((unsigned int)'B' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 37, (((unsigned int)'o' << 0) | ((unsigned int)'x' << 8) | ((unsigned int)'W' << 16) | 0u)); \
    12u; \
})
#define MessageBoxW(...) OBFH_API_CALL(0, MessageBoxW, __VA_ARGS__)

#define OBFH_GUI_ID_MoveWindow 38
#define OBFH_GUI_NAME_MoveWindow(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 38, (((unsigned int)'M' << 0) | ((unsigned int)'o' << 8) | ((unsigned int)'v' << 16) | ((unsigned int)'e' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 38, (((unsigned int)'W' << 0) | ((unsigned int)'i' << 8) | ((unsigned int)'n' << 16) | ((unsigned int)'d' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 38, (((unsigned int)'o' << 0) | ((unsigned int)'w' << 8) | 0u | 0u)); \
    11u; \
})
#define MoveWindow(...) OBFH_API_CALL(0, MoveWindow, __VA_ARGS__)

#define OBFH_GUI_ID_OpenClipboard 39
#define OBFH_GUI_NAME_OpenClipboard(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 39, (((unsigned int)'O' << 0) | ((unsigned int)'p' << 8) | ((unsigned int)'e' << 16) | ((unsigned int)'n' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 39, (((unsigned int)'C' << 0) | ((unsigned int)'l' << 8) | ((unsigned int)'i' << 16) | ((unsigned int)'p' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 39, (((unsigned int)'b' << 0) | ((unsigned int)'o' << 8) | ((unsigned int)'a' << 16) | ((unsigned int)'r' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 39, (((unsigned int)'d' << 0) | 0u | 0u | 0u)); \
    14u; \
})
#define OpenClipboard(...) OBFH_API_CALL(0, OpenClipboard, __VA_ARGS__)

#define OBFH_GUI_ID_PostQuitMessage 40
#define OBFH_GUI_NAME_PostQuitMessage(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 40, (((unsigned int)'P' << 0) | ((unsigned int)'o' << 8) | ((unsigned int)'s' << 16) | ((unsigned int)'t' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 40, (((unsigned int)'Q' << 0) | ((unsigned int)'u' << 8) | ((unsigned int)'i' << 16) | ((unsigned int)'t' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 40, (((unsigned int)'M' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'s' << 16) | ((unsigned int)'s' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 40, (((unsigned int)'a' << 0) | ((unsigned int)'g' << 8) | ((unsigned int)'e' << 16) | 0u)); \
    16u; \
})
#define PostQuitMessage(...) OBFH_API_CALL(0, PostQuitMessage, __VA_ARGS__)

#define OBFH_GUI_ID_RegisterClassExA 41
#define OBFH_GUI_NAME_RegisterClassExA(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 41, (((unsigned int)'R' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'g' << 16) | ((unsigned int)'i' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 41, (((unsigned int)'s' << 0) | ((unsigned int)'t' << 8) | ((unsigned int)'e' << 16) | ((unsigned int)'r' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 41, (((unsigned int)'C' << 0) | ((unsigned int)'l' << 8) | ((unsigned int)'a' << 16) | ((unsigned int)'s' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 41, (((unsigned int)'s' << 0) | ((unsigned int)'E' << 8) | ((unsigned int)'x' << 16) | ((unsigned int)'A' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 16, 41, (0u | 0u | 0u | 0u)); \
    17u; \
})
#define RegisterClassExA(...) OBFH_API_CALL(0, RegisterClassExA, __VA_ARGS__)

#define OBFH_GUI_ID_RegisterClassExW 42
#define OBFH_GUI_NAME_RegisterClassExW(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 42, (((unsigned int)'R' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'g' << 16) | ((unsigned int)'i' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 42, (((unsigned int)'s' << 0) | ((unsigned int)'t' << 8) | ((unsigned int)'e' << 16) | ((unsigned int)'r' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 42, (((unsigned int)'C' << 0) | ((unsigned int)'l' << 8) | ((unsigned int)'a' << 16) | ((unsigned int)'s' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 42, (((unsigned int)'s' << 0) | ((unsigned int)'E' << 8) | ((unsigned int)'x' << 16) | ((unsigned int)'W' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 16, 42, (0u | 0u | 0u | 0u)); \
    17u; \
})
#define RegisterClassExW(...) OBFH_API_CALL(0, RegisterClassExW, __VA_ARGS__)

#define OBFH_GUI_ID_ReleaseDC 43
#define OBFH_GUI_NAME_ReleaseDC(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 43, (((unsigned int)'R' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'l' << 16) | ((unsigned int)'e' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 43, (((unsigned int)'a' << 0) | ((unsigned int)'s' << 8) | ((unsigned int)'e' << 16) | ((unsigned int)'D' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 43, (((unsigned int)'C' << 0) | 0u | 0u | 0u)); \
    10u; \
})
#define ReleaseDC(...) OBFH_API_CALL(0, ReleaseDC, __VA_ARGS__)

#define OBFH_GUI_ID_SendMessageA 44
#define OBFH_GUI_NAME_SendMessageA(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 44, (((unsigned int)'S' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'n' << 16) | ((unsigned int)'d' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 44, (((unsigned int)'M' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'s' << 16) | ((unsigned int)'s' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 44, (((unsigned int)'a' << 0) | ((unsigned int)'g' << 8) | ((unsigned int)'e' << 16) | ((unsigned int)'A' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 44, (0u | 0u | 0u | 0u)); \
    13u; \
})
#define SendMessageA(...) OBFH_API_CALL(0, SendMessageA, __VA_ARGS__)

#define OBFH_GUI_ID_SendMessageW 45
#define OBFH_GUI_NAME_SendMessageW(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 45, (((unsigned int)'S' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'n' << 16) | ((unsigned int)'d' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 45, (((unsigned int)'M' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'s' << 16) | ((unsigned int)'s' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 45, (((unsigned int)'a' << 0) | ((unsigned int)'g' << 8) | ((unsigned int)'e' << 16) | ((unsigned int)'W' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 45, (0u | 0u | 0u | 0u)); \
    13u; \
})
#define SendMessageW(...) OBFH_API_CALL(0, SendMessageW, __VA_ARGS__)

#define OBFH_GUI_ID_SetClipboardData 46
#define OBFH_GUI_NAME_SetClipboardData(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 46, (((unsigned int)'S' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'C' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 46, (((unsigned int)'l' << 0) | ((unsigned int)'i' << 8) | ((unsigned int)'p' << 16) | ((unsigned int)'b' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 46, (((unsigned int)'o' << 0) | ((unsigned int)'a' << 8) | ((unsigned int)'r' << 16) | ((unsigned int)'d' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 46, (((unsigned int)'D' << 0) | ((unsigned int)'a' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'a' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 16, 46, (0u | 0u | 0u | 0u)); \
    17u; \
})
#define SetClipboardData(...) OBFH_API_CALL(0, SetClipboardData, __VA_ARGS__)

#define OBFH_GUI_ID_SetFocus 47
#define OBFH_GUI_NAME_SetFocus(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 47, (((unsigned int)'S' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'F' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 47, (((unsigned int)'o' << 0) | ((unsigned int)'c' << 8) | ((unsigned int)'u' << 16) | ((unsigned int)'s' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 47, (0u | 0u | 0u | 0u)); \
    9u; \
})
#define SetFocus(...) OBFH_API_CALL(0, SetFocus, __VA_ARGS__)

#define OBFH_GUI_ID_SetWindowPos 48
#define OBFH_GUI_NAME_SetWindowPos(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 48, (((unsigned int)'S' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'W' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 48, (((unsigned int)'i' << 0) | ((unsigned int)'n' << 8) | ((unsigned int)'d' << 16) | ((unsigned int)'o' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 48, (((unsigned int)'w' << 0) | ((unsigned int)'P' << 8) | ((unsigned int)'o' << 16) | ((unsigned int)'s' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 48, (0u | 0u | 0u | 0u)); \
    13u; \
})
#define SetWindowPos(...) OBFH_API_CALL(0, SetWindowPos, __VA_ARGS__)

#define OBFH_GUI_ID_SetWindowTextA 49
#define OBFH_GUI_NAME_SetWindowTextA(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 49, (((unsigned int)'S' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'W' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 49, (((unsigned int)'i' << 0) | ((unsigned int)'n' << 8) | ((unsigned int)'d' << 16) | ((unsigned int)'o' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 49, (((unsigned int)'w' << 0) | ((unsigned int)'T' << 8) | ((unsigned int)'e' << 16) | ((unsigned int)'x' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 49, (((unsigned int)'t' << 0) | ((unsigned int)'A' << 8) | 0u | 0u)); \
    15u; \
})
#define SetWindowTextA(...) OBFH_API_CALL(0, SetWindowTextA, __VA_ARGS__)

#define OBFH_GUI_ID_SetWindowTextW 50
#define OBFH_GUI_NAME_SetWindowTextW(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 50, (((unsigned int)'S' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'W' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 50, (((unsigned int)'i' << 0) | ((unsigned int)'n' << 8) | ((unsigned int)'d' << 16) | ((unsigned int)'o' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 50, (((unsigned int)'w' << 0) | ((unsigned int)'T' << 8) | ((unsigned int)'e' << 16) | ((unsigned int)'x' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 50, (((unsigned int)'t' << 0) | ((unsigned int)'W' << 8) | 0u | 0u)); \
    15u; \
})
#define SetWindowTextW(...) OBFH_API_CALL(0, SetWindowTextW, __VA_ARGS__)

#define OBFH_GUI_ID_ShowWindow 51
#define OBFH_GUI_NAME_ShowWindow(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 51, (((unsigned int)'S' << 0) | ((unsigned int)'h' << 8) | ((unsigned int)'o' << 16) | ((unsigned int)'w' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 51, (((unsigned int)'W' << 0) | ((unsigned int)'i' << 8) | ((unsigned int)'n' << 16) | ((unsigned int)'d' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 51, (((unsigned int)'o' << 0) | ((unsigned int)'w' << 8) | 0u | 0u)); \
    11u; \
})
#define ShowWindow(...) OBFH_API_CALL(0, ShowWindow, __VA_ARGS__)

#define OBFH_GUI_ID_TranslateMessage 52
#define OBFH_GUI_NAME_TranslateMessage(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 52, (((unsigned int)'T' << 0) | ((unsigned int)'r' << 8) | ((unsigned int)'a' << 16) | ((unsigned int)'n' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 52, (((unsigned int)'s' << 0) | ((unsigned int)'l' << 8) | ((unsigned int)'a' << 16) | ((unsigned int)'t' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 52, (((unsigned int)'e' << 0) | ((unsigned int)'M' << 8) | ((unsigned int)'e' << 16) | ((unsigned int)'s' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 52, (((unsigned int)'s' << 0) | ((unsigned int)'a' << 8) | ((unsigned int)'g' << 16) | ((unsigned int)'e' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 16, 52, (0u | 0u | 0u | 0u)); \
    17u; \
})
#define TranslateMessage(...) OBFH_API_CALL(0, TranslateMessage, __VA_ARGS__)

#define OBFH_GUI_ID_UnregisterClassA 53
#define OBFH_GUI_NAME_UnregisterClassA(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 53, (((unsigned int)'U' << 0) | ((unsigned int)'n' << 8) | ((unsigned int)'r' << 16) | ((unsigned int)'e' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 53, (((unsigned int)'g' << 0) | ((unsigned int)'i' << 8) | ((unsigned int)'s' << 16) | ((unsigned int)'t' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 53, (((unsigned int)'e' << 0) | ((unsigned int)'r' << 8) | ((unsigned int)'C' << 16) | ((unsigned int)'l' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 53, (((unsigned int)'a' << 0) | ((unsigned int)'s' << 8) | ((unsigned int)'s' << 16) | ((unsigned int)'A' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 16, 53, (0u | 0u | 0u | 0u)); \
    17u; \
})
#define UnregisterClassA(...) OBFH_API_CALL(0, UnregisterClassA, __VA_ARGS__)

#define OBFH_GUI_ID_UnregisterClassW 54
#define OBFH_GUI_NAME_UnregisterClassW(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 54, (((unsigned int)'U' << 0) | ((unsigned int)'n' << 8) | ((unsigned int)'r' << 16) | ((unsigned int)'e' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 54, (((unsigned int)'g' << 0) | ((unsigned int)'i' << 8) | ((unsigned int)'s' << 16) | ((unsigned int)'t' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 54, (((unsigned int)'e' << 0) | ((unsigned int)'r' << 8) | ((unsigned int)'C' << 16) | ((unsigned int)'l' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 54, (((unsigned int)'a' << 0) | ((unsigned int)'s' << 8) | ((unsigned int)'s' << 16) | ((unsigned int)'W' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 16, 54, (0u | 0u | 0u | 0u)); \
    17u; \
})
#define UnregisterClassW(...) OBFH_API_CALL(0, UnregisterClassW, __VA_ARGS__)

#define OBFH_GUI_ID_UpdateWindow 55
#define OBFH_GUI_NAME_UpdateWindow(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 55, (((unsigned int)'U' << 0) | ((unsigned int)'p' << 8) | ((unsigned int)'d' << 16) | ((unsigned int)'a' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 55, (((unsigned int)'t' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'W' << 16) | ((unsigned int)'i' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 55, (((unsigned int)'n' << 0) | ((unsigned int)'d' << 8) | ((unsigned int)'o' << 16) | ((unsigned int)'w' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 55, (0u | 0u | 0u | 0u)); \
    13u; \
})
#define UpdateWindow(...) OBFH_API_CALL(0, UpdateWindow, __VA_ARGS__)

#define OBFH_GUI_ID_CreateFontA 56
#define OBFH_GUI_NAME_CreateFontA(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 56, (((unsigned int)'C' << 0) | ((unsigned int)'r' << 8) | ((unsigned int)'e' << 16) | ((unsigned int)'a' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 56, (((unsigned int)'t' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'F' << 16) | ((unsigned int)'o' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 56, (((unsigned int)'n' << 0) | ((unsigned int)'t' << 8) | ((unsigned int)'A' << 16) | 0u)); \
    12u; \
})
#define CreateFontA(...) OBFH_API_CALL(1, CreateFontA, __VA_ARGS__)

#define OBFH_GUI_ID_CreateFontW 57
#define OBFH_GUI_NAME_CreateFontW(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 57, (((unsigned int)'C' << 0) | ((unsigned int)'r' << 8) | ((unsigned int)'e' << 16) | ((unsigned int)'a' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 57, (((unsigned int)'t' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'F' << 16) | ((unsigned int)'o' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 57, (((unsigned int)'n' << 0) | ((unsigned int)'t' << 8) | ((unsigned int)'W' << 16) | 0u)); \
    12u; \
})
#define CreateFontW(...) OBFH_API_CALL(1, CreateFontW, __VA_ARGS__)

#define OBFH_GUI_ID_CreateFontIndirectA 58
#define OBFH_GUI_NAME_CreateFontIndirectA(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 58, (((unsigned int)'C' << 0) | ((unsigned int)'r' << 8) | ((unsigned int)'e' << 16) | ((unsigned int)'a' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 58, (((unsigned int)'t' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'F' << 16) | ((unsigned int)'o' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 58, (((unsigned int)'n' << 0) | ((unsigned int)'t' << 8) | ((unsigned int)'I' << 16) | ((unsigned int)'n' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 58, (((unsigned int)'d' << 0) | ((unsigned int)'i' << 8) | ((unsigned int)'r' << 16) | ((unsigned int)'e' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 16, 58, (((unsigned int)'c' << 0) | ((unsigned int)'t' << 8) | ((unsigned int)'A' << 16) | 0u)); \
    20u; \
})
#define CreateFontIndirectA(...) OBFH_API_CALL(1, CreateFontIndirectA, __VA_ARGS__)

#define OBFH_GUI_ID_CreateFontIndirectW 59
#define OBFH_GUI_NAME_CreateFontIndirectW(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 59, (((unsigned int)'C' << 0) | ((unsigned int)'r' << 8) | ((unsigned int)'e' << 16) | ((unsigned int)'a' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 59, (((unsigned int)'t' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'F' << 16) | ((unsigned int)'o' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 59, (((unsigned int)'n' << 0) | ((unsigned int)'t' << 8) | ((unsigned int)'I' << 16) | ((unsigned int)'n' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 59, (((unsigned int)'d' << 0) | ((unsigned int)'i' << 8) | ((unsigned int)'r' << 16) | ((unsigned int)'e' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 16, 59, (((unsigned int)'c' << 0) | ((unsigned int)'t' << 8) | ((unsigned int)'W' << 16) | 0u)); \
    20u; \
})
#define CreateFontIndirectW(...) OBFH_API_CALL(1, CreateFontIndirectW, __VA_ARGS__)

#define OBFH_GUI_ID_DeleteObject 60
#define OBFH_GUI_NAME_DeleteObject(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 60, (((unsigned int)'D' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'l' << 16) | ((unsigned int)'e' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 60, (((unsigned int)'t' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'O' << 16) | ((unsigned int)'b' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 60, (((unsigned int)'j' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'c' << 16) | ((unsigned int)'t' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 60, (0u | 0u | 0u | 0u)); \
    13u; \
})
#define DeleteObject(...) OBFH_API_CALL(1, DeleteObject, __VA_ARGS__)

#define OBFH_GUI_ID_GetDeviceCaps 61
#define OBFH_GUI_NAME_GetDeviceCaps(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 61, (((unsigned int)'G' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'D' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 61, (((unsigned int)'e' << 0) | ((unsigned int)'v' << 8) | ((unsigned int)'i' << 16) | ((unsigned int)'c' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 61, (((unsigned int)'e' << 0) | ((unsigned int)'C' << 8) | ((unsigned int)'a' << 16) | ((unsigned int)'p' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 61, (((unsigned int)'s' << 0) | 0u | 0u | 0u)); \
    14u; \
})
#define GetDeviceCaps(...) OBFH_API_CALL(1, GetDeviceCaps, __VA_ARGS__)

#define OBFH_GUI_ID_SelectObject 62
#define OBFH_GUI_NAME_SelectObject(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 62, (((unsigned int)'S' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'l' << 16) | ((unsigned int)'e' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 62, (((unsigned int)'c' << 0) | ((unsigned int)'t' << 8) | ((unsigned int)'O' << 16) | ((unsigned int)'b' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 62, (((unsigned int)'j' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'c' << 16) | ((unsigned int)'t' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 62, (0u | 0u | 0u | 0u)); \
    13u; \
})
#define SelectObject(...) OBFH_API_CALL(1, SelectObject, __VA_ARGS__)

#define OBFH_GUI_ID_SetBkColor 63
#define OBFH_GUI_NAME_SetBkColor(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 63, (((unsigned int)'S' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'B' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 63, (((unsigned int)'k' << 0) | ((unsigned int)'C' << 8) | ((unsigned int)'o' << 16) | ((unsigned int)'l' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 63, (((unsigned int)'o' << 0) | ((unsigned int)'r' << 8) | 0u | 0u)); \
    11u; \
})
#define SetBkColor(...) OBFH_API_CALL(1, SetBkColor, __VA_ARGS__)

#define OBFH_GUI_ID_SetTextColor 64
#define OBFH_GUI_NAME_SetTextColor(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 64, (((unsigned int)'S' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'T' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 64, (((unsigned int)'e' << 0) | ((unsigned int)'x' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'C' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 64, (((unsigned int)'o' << 0) | ((unsigned int)'l' << 8) | ((unsigned int)'o' << 16) | ((unsigned int)'r' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 64, (0u | 0u | 0u | 0u)); \
    13u; \
})
#define SetTextColor(...) OBFH_API_CALL(1, SetTextColor, __VA_ARGS__)
// KERNEL32 user call sites share the cache; resolver definitions above use
// native bootstrap calls and cannot recurse through these late intercepts.
#define OBFH_GUI_ID_ExitProcess 65
#define OBFH_GUI_NAME_ExitProcess(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 65, (((unsigned int)'E' << 0) | ((unsigned int)'x' << 8) | ((unsigned int)'i' << 16) | ((unsigned int)'t' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 65, (((unsigned int)'P' << 0) | ((unsigned int)'r' << 8) | ((unsigned int)'o' << 16) | ((unsigned int)'c' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 65, (((unsigned int)'e' << 0) | ((unsigned int)'s' << 8) | ((unsigned int)'s' << 16) | 0u)); \
    12u; \
})
#undef ExitProcess
#define ExitProcess(...) OBFH_API_CALL(2, ExitProcess, __VA_ARGS__)

#define OBFH_GUI_ID_GetLastError 66
#define OBFH_GUI_NAME_GetLastError(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 66, (((unsigned int)'G' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'L' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 66, (((unsigned int)'a' << 0) | ((unsigned int)'s' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'E' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 66, (((unsigned int)'r' << 0) | ((unsigned int)'r' << 8) | ((unsigned int)'o' << 16) | ((unsigned int)'r' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 66, (0u | 0u | 0u | 0u)); \
    13u; \
})
#undef GetLastError
#define GetLastError(...) OBFH_API_CALL(2, GetLastError, __VA_ARGS__)

#define OBFH_GUI_ID_FreeLibrary 67
#define OBFH_GUI_NAME_FreeLibrary(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 67, (((unsigned int)'F' << 0) | ((unsigned int)'r' << 8) | ((unsigned int)'e' << 16) | ((unsigned int)'e' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 67, (((unsigned int)'L' << 0) | ((unsigned int)'i' << 8) | ((unsigned int)'b' << 16) | ((unsigned int)'r' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 67, (((unsigned int)'a' << 0) | ((unsigned int)'r' << 8) | ((unsigned int)'y' << 16) | 0u)); \
    12u; \
})
#undef FreeLibrary
#define FreeLibrary(...) OBFH_API_CALL(2, FreeLibrary, __VA_ARGS__)

#define OBFH_GUI_ID_SetLastError 68
#define OBFH_GUI_NAME_SetLastError(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 68, (((unsigned int)'S' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'L' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 68, (((unsigned int)'a' << 0) | ((unsigned int)'s' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'E' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 68, (((unsigned int)'r' << 0) | ((unsigned int)'r' << 8) | ((unsigned int)'o' << 16) | ((unsigned int)'r' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 68, (0u | 0u | 0u | 0u)); \
    13u; \
})
#undef SetLastError
#define SetLastError(...) OBFH_API_CALL(2, SetLastError, __VA_ARGS__)

#define OBFH_GUI_ID_WriteConsoleA 69
#define OBFH_GUI_NAME_WriteConsoleA(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 69, (((unsigned int)'W' << 0) | ((unsigned int)'r' << 8) | ((unsigned int)'i' << 16) | ((unsigned int)'t' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 69, (((unsigned int)'e' << 0) | ((unsigned int)'C' << 8) | ((unsigned int)'o' << 16) | ((unsigned int)'n' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 69, (((unsigned int)'s' << 0) | ((unsigned int)'o' << 8) | ((unsigned int)'l' << 16) | ((unsigned int)'e' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 69, (((unsigned int)'A' << 0) | 0u | 0u | 0u)); \
    14u; \
})
#undef WriteConsoleA
#define WriteConsoleA(...) OBFH_API_CALL(2, WriteConsoleA, __VA_ARGS__)

#define OBFH_GUI_ID_GetStdHandle 70
#define OBFH_GUI_NAME_GetStdHandle(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 70, (((unsigned int)'G' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'S' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 70, (((unsigned int)'t' << 0) | ((unsigned int)'d' << 8) | ((unsigned int)'H' << 16) | ((unsigned int)'a' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 70, (((unsigned int)'n' << 0) | ((unsigned int)'d' << 8) | ((unsigned int)'l' << 16) | ((unsigned int)'e' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 70, (0u | 0u | 0u | 0u)); \
    13u; \
})
#undef GetStdHandle
#define GetStdHandle(...) OBFH_API_CALL(2, GetStdHandle, __VA_ARGS__)

#define OBFH_GUI_ID_GetModuleHandleA 71
#define OBFH_GUI_NAME_GetModuleHandleA(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 71, (((unsigned int)'G' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'M' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 71, (((unsigned int)'o' << 0) | ((unsigned int)'d' << 8) | ((unsigned int)'u' << 16) | ((unsigned int)'l' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 71, (((unsigned int)'e' << 0) | ((unsigned int)'H' << 8) | ((unsigned int)'a' << 16) | ((unsigned int)'n' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 71, (((unsigned int)'d' << 0) | ((unsigned int)'l' << 8) | ((unsigned int)'e' << 16) | ((unsigned int)'A' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 16, 71, (0u | 0u | 0u | 0u)); \
    17u; \
})
#undef GetModuleHandleA
#define GetModuleHandleA(...) OBFH_API_CALL(2, GetModuleHandleA, __VA_ARGS__)

#define OBFH_GUI_ID_GetModuleHandleExA 72
#define OBFH_GUI_NAME_GetModuleHandleExA(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 72, (((unsigned int)'G' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'M' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 72, (((unsigned int)'o' << 0) | ((unsigned int)'d' << 8) | ((unsigned int)'u' << 16) | ((unsigned int)'l' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 72, (((unsigned int)'e' << 0) | ((unsigned int)'H' << 8) | ((unsigned int)'a' << 16) | ((unsigned int)'n' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 72, (((unsigned int)'d' << 0) | ((unsigned int)'l' << 8) | ((unsigned int)'e' << 16) | ((unsigned int)'E' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 16, 72, (((unsigned int)'x' << 0) | ((unsigned int)'A' << 8) | 0u | 0u)); \
    19u; \
})
#undef GetModuleHandleExA
#define GetModuleHandleExA(...) OBFH_API_CALL(2, GetModuleHandleExA, __VA_ARGS__)

#define OBFH_GUI_ID_VirtualQuery 73
#define OBFH_GUI_NAME_VirtualQuery(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 73, (((unsigned int)'V' << 0) | ((unsigned int)'i' << 8) | ((unsigned int)'r' << 16) | ((unsigned int)'t' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 73, (((unsigned int)'u' << 0) | ((unsigned int)'a' << 8) | ((unsigned int)'l' << 16) | ((unsigned int)'Q' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 73, (((unsigned int)'u' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'r' << 16) | ((unsigned int)'y' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 73, (0u | 0u | 0u | 0u)); \
    13u; \
})
#undef VirtualQuery
#define VirtualQuery(...) OBFH_API_CALL(2, VirtualQuery, __VA_ARGS__)

#define OBFH_GUI_ID_GetConsoleMode 74
#define OBFH_GUI_NAME_GetConsoleMode(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 74, (((unsigned int)'G' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'C' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 74, (((unsigned int)'o' << 0) | ((unsigned int)'n' << 8) | ((unsigned int)'s' << 16) | ((unsigned int)'o' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 74, (((unsigned int)'l' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'M' << 16) | ((unsigned int)'o' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 74, (((unsigned int)'d' << 0) | ((unsigned int)'e' << 8) | 0u | 0u)); \
    15u; \
})
#undef GetConsoleMode
#define GetConsoleMode(...) OBFH_API_CALL(2, GetConsoleMode, __VA_ARGS__)

#define OBFH_GUI_ID_MulDiv 75
#define OBFH_GUI_NAME_MulDiv(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 75, (((unsigned int)'M' << 0) | ((unsigned int)'u' << 8) | ((unsigned int)'l' << 16) | ((unsigned int)'D' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 75, (((unsigned int)'i' << 0) | ((unsigned int)'v' << 8) | 0u | 0u)); \
    7u; \
})
#undef MulDiv
#define MulDiv(...) OBFH_API_CALL(2, MulDiv, __VA_ARGS__)

#define OBFH_GUI_ID_GlobalAlloc 76
#define OBFH_GUI_NAME_GlobalAlloc(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 76, (((unsigned int)'G' << 0) | ((unsigned int)'l' << 8) | ((unsigned int)'o' << 16) | ((unsigned int)'b' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 76, (((unsigned int)'a' << 0) | ((unsigned int)'l' << 8) | ((unsigned int)'A' << 16) | ((unsigned int)'l' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 76, (((unsigned int)'l' << 0) | ((unsigned int)'o' << 8) | ((unsigned int)'c' << 16) | 0u)); \
    12u; \
})
#undef GlobalAlloc
#define GlobalAlloc(...) OBFH_API_CALL(2, GlobalAlloc, __VA_ARGS__)

#define OBFH_GUI_ID_GlobalLock 77
#define OBFH_GUI_NAME_GlobalLock(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 77, (((unsigned int)'G' << 0) | ((unsigned int)'l' << 8) | ((unsigned int)'o' << 16) | ((unsigned int)'b' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 77, (((unsigned int)'a' << 0) | ((unsigned int)'l' << 8) | ((unsigned int)'L' << 16) | ((unsigned int)'o' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 77, (((unsigned int)'c' << 0) | ((unsigned int)'k' << 8) | 0u | 0u)); \
    11u; \
})
#undef GlobalLock
#define GlobalLock(...) OBFH_API_CALL(2, GlobalLock, __VA_ARGS__)

#define OBFH_GUI_ID_GlobalFree 78
#define OBFH_GUI_NAME_GlobalFree(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 78, (((unsigned int)'G' << 0) | ((unsigned int)'l' << 8) | ((unsigned int)'o' << 16) | ((unsigned int)'b' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 78, (((unsigned int)'a' << 0) | ((unsigned int)'l' << 8) | ((unsigned int)'F' << 16) | ((unsigned int)'r' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 78, (((unsigned int)'e' << 0) | ((unsigned int)'e' << 8) | 0u | 0u)); \
    11u; \
})
#undef GlobalFree
#define GlobalFree(...) OBFH_API_CALL(2, GlobalFree, __VA_ARGS__)

#define OBFH_GUI_ID_GlobalUnlock 79
#define OBFH_GUI_NAME_GlobalUnlock(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 79, (((unsigned int)'G' << 0) | ((unsigned int)'l' << 8) | ((unsigned int)'o' << 16) | ((unsigned int)'b' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 79, (((unsigned int)'a' << 0) | ((unsigned int)'l' << 8) | ((unsigned int)'U' << 16) | ((unsigned int)'n' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 79, (((unsigned int)'l' << 0) | ((unsigned int)'o' << 8) | ((unsigned int)'c' << 16) | ((unsigned int)'k' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 79, (0u | 0u | 0u | 0u)); \
    13u; \
})
#undef GlobalUnlock
#define GlobalUnlock(...) OBFH_API_CALL(2, GlobalUnlock, __VA_ARGS__)

#define OBFH_GUI_ID_GetStartupInfoA 80
#define OBFH_GUI_NAME_GetStartupInfoA(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 80, (((unsigned int)'G' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'S' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 80, (((unsigned int)'t' << 0) | ((unsigned int)'a' << 8) | ((unsigned int)'r' << 16) | ((unsigned int)'t' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 80, (((unsigned int)'u' << 0) | ((unsigned int)'p' << 8) | ((unsigned int)'I' << 16) | ((unsigned int)'n' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 80, (((unsigned int)'f' << 0) | ((unsigned int)'o' << 8) | ((unsigned int)'A' << 16) | 0u)); \
    16u; \
})
#undef GetStartupInfoA
#define GetStartupInfoA(...) OBFH_API_CALL(2, GetStartupInfoA, __VA_ARGS__)

#define OBFH_GUI_ID_GetCommandLineA 81
#define OBFH_GUI_NAME_GetCommandLineA(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 81, (((unsigned int)'G' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'C' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 81, (((unsigned int)'o' << 0) | ((unsigned int)'m' << 8) | ((unsigned int)'m' << 16) | ((unsigned int)'a' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 81, (((unsigned int)'n' << 0) | ((unsigned int)'d' << 8) | ((unsigned int)'L' << 16) | ((unsigned int)'i' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 81, (((unsigned int)'n' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'A' << 16) | 0u)); \
    16u; \
})
#undef GetCommandLineA
#define GetCommandLineA(...) OBFH_API_CALL(2, GetCommandLineA, __VA_ARGS__)

#define OBFH_GUI_ID_WriteConsoleW 82
#define OBFH_GUI_NAME_WriteConsoleW(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 82, (((unsigned int)'W' << 0) | ((unsigned int)'r' << 8) | ((unsigned int)'i' << 16) | ((unsigned int)'t' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 82, (((unsigned int)'e' << 0) | ((unsigned int)'C' << 8) | ((unsigned int)'o' << 16) | ((unsigned int)'n' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 82, (((unsigned int)'s' << 0) | ((unsigned int)'o' << 8) | ((unsigned int)'l' << 16) | ((unsigned int)'e' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 82, (((unsigned int)'W' << 0) | 0u | 0u | 0u)); \
    14u; \
})
#undef WriteConsoleW
#define WriteConsoleW(...) OBFH_API_CALL(2, WriteConsoleW, __VA_ARGS__)

#define OBFH_GUI_ID_GetModuleHandleW 83
#define OBFH_GUI_NAME_GetModuleHandleW(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 83, (((unsigned int)'G' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'M' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 83, (((unsigned int)'o' << 0) | ((unsigned int)'d' << 8) | ((unsigned int)'u' << 16) | ((unsigned int)'l' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 83, (((unsigned int)'e' << 0) | ((unsigned int)'H' << 8) | ((unsigned int)'a' << 16) | ((unsigned int)'n' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 83, (((unsigned int)'d' << 0) | ((unsigned int)'l' << 8) | ((unsigned int)'e' << 16) | ((unsigned int)'W' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 16, 83, (0u | 0u | 0u | 0u)); \
    17u; \
})
#undef GetModuleHandleW
#define GetModuleHandleW(...) OBFH_API_CALL(2, GetModuleHandleW, __VA_ARGS__)

#define OBFH_GUI_ID_GetModuleHandleExW 84
#define OBFH_GUI_NAME_GetModuleHandleExW(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 84, (((unsigned int)'G' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'M' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 84, (((unsigned int)'o' << 0) | ((unsigned int)'d' << 8) | ((unsigned int)'u' << 16) | ((unsigned int)'l' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 84, (((unsigned int)'e' << 0) | ((unsigned int)'H' << 8) | ((unsigned int)'a' << 16) | ((unsigned int)'n' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 84, (((unsigned int)'d' << 0) | ((unsigned int)'l' << 8) | ((unsigned int)'e' << 16) | ((unsigned int)'E' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 16, 84, (((unsigned int)'x' << 0) | ((unsigned int)'W' << 8) | 0u | 0u)); \
    19u; \
})
#undef GetModuleHandleExW
#define GetModuleHandleExW(...) OBFH_API_CALL(2, GetModuleHandleExW, __VA_ARGS__)

#define OBFH_GUI_ID_GetStartupInfoW 85
#define OBFH_GUI_NAME_GetStartupInfoW(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 85, (((unsigned int)'G' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'S' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 85, (((unsigned int)'t' << 0) | ((unsigned int)'a' << 8) | ((unsigned int)'r' << 16) | ((unsigned int)'t' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 85, (((unsigned int)'u' << 0) | ((unsigned int)'p' << 8) | ((unsigned int)'I' << 16) | ((unsigned int)'n' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 85, (((unsigned int)'f' << 0) | ((unsigned int)'o' << 8) | ((unsigned int)'W' << 16) | 0u)); \
    16u; \
})
#undef GetStartupInfoW
#define GetStartupInfoW(...) OBFH_API_CALL(2, GetStartupInfoW, __VA_ARGS__)

#define OBFH_GUI_ID_GetCommandLineW 86
#define OBFH_GUI_NAME_GetCommandLineW(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 86, (((unsigned int)'G' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'C' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 86, (((unsigned int)'o' << 0) | ((unsigned int)'m' << 8) | ((unsigned int)'m' << 16) | ((unsigned int)'a' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 86, (((unsigned int)'n' << 0) | ((unsigned int)'d' << 8) | ((unsigned int)'L' << 16) | ((unsigned int)'i' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 86, (((unsigned int)'n' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'W' << 16) | 0u)); \
    16u; \
})
#undef GetCommandLineW
#define GetCommandLineW(...) OBFH_API_CALL(2, GetCommandLineW, __VA_ARGS__)

// Files.

#define OBFH_GUI_ID_CreateFileA 87
#define OBFH_GUI_NAME_CreateFileA(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 87, (((unsigned int)'C' << 0) | ((unsigned int)'r' << 8) | ((unsigned int)'e' << 16) | ((unsigned int)'a' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 87, (((unsigned int)'t' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'F' << 16) | ((unsigned int)'i' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 87, (((unsigned int)'l' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'A' << 16) | 0u)); \
    12u; \
})
#undef CreateFileA
#define CreateFileA(...) OBFH_API_CALL(2, CreateFileA, __VA_ARGS__)

#define OBFH_GUI_ID_CreateFileW 88
#define OBFH_GUI_NAME_CreateFileW(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 88, (((unsigned int)'C' << 0) | ((unsigned int)'r' << 8) | ((unsigned int)'e' << 16) | ((unsigned int)'a' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 88, (((unsigned int)'t' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'F' << 16) | ((unsigned int)'i' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 88, (((unsigned int)'l' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'W' << 16) | 0u)); \
    12u; \
})
#undef CreateFileW
#define CreateFileW(...) OBFH_API_CALL(2, CreateFileW, __VA_ARGS__)

#define OBFH_GUI_ID_ReadFile 89
#define OBFH_GUI_NAME_ReadFile(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 89, (((unsigned int)'R' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'a' << 16) | ((unsigned int)'d' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 89, (((unsigned int)'F' << 0) | ((unsigned int)'i' << 8) | ((unsigned int)'l' << 16) | ((unsigned int)'e' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 89, (0u | 0u | 0u | 0u)); \
    9u; \
})
#undef ReadFile
#define ReadFile(...) OBFH_API_CALL(2, ReadFile, __VA_ARGS__)

#define OBFH_GUI_ID_WriteFile 90
#define OBFH_GUI_NAME_WriteFile(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 90, (((unsigned int)'W' << 0) | ((unsigned int)'r' << 8) | ((unsigned int)'i' << 16) | ((unsigned int)'t' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 90, (((unsigned int)'e' << 0) | ((unsigned int)'F' << 8) | ((unsigned int)'i' << 16) | ((unsigned int)'l' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 90, (((unsigned int)'e' << 0) | 0u | 0u | 0u)); \
    10u; \
})
#undef WriteFile
#define WriteFile(...) OBFH_API_CALL(2, WriteFile, __VA_ARGS__)

#define OBFH_GUI_ID_CloseHandle 91
#define OBFH_GUI_NAME_CloseHandle(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 91, (((unsigned int)'C' << 0) | ((unsigned int)'l' << 8) | ((unsigned int)'o' << 16) | ((unsigned int)'s' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 91, (((unsigned int)'e' << 0) | ((unsigned int)'H' << 8) | ((unsigned int)'a' << 16) | ((unsigned int)'n' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 91, (((unsigned int)'d' << 0) | ((unsigned int)'l' << 8) | ((unsigned int)'e' << 16) | 0u)); \
    12u; \
})
#undef CloseHandle
#define CloseHandle(...) OBFH_API_CALL(2, CloseHandle, __VA_ARGS__)

#define OBFH_GUI_ID_GetFileSizeEx 92
#define OBFH_GUI_NAME_GetFileSizeEx(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 92, (((unsigned int)'G' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'F' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 92, (((unsigned int)'i' << 0) | ((unsigned int)'l' << 8) | ((unsigned int)'e' << 16) | ((unsigned int)'S' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 92, (((unsigned int)'i' << 0) | ((unsigned int)'z' << 8) | ((unsigned int)'e' << 16) | ((unsigned int)'E' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 92, (((unsigned int)'x' << 0) | 0u | 0u | 0u)); \
    14u; \
})
#undef GetFileSizeEx
#define GetFileSizeEx(...) OBFH_API_CALL(2, GetFileSizeEx, __VA_ARGS__)

#define OBFH_GUI_ID_SetFilePointerEx 93
#define OBFH_GUI_NAME_SetFilePointerEx(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 93, (((unsigned int)'S' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'F' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 93, (((unsigned int)'i' << 0) | ((unsigned int)'l' << 8) | ((unsigned int)'e' << 16) | ((unsigned int)'P' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 93, (((unsigned int)'o' << 0) | ((unsigned int)'i' << 8) | ((unsigned int)'n' << 16) | ((unsigned int)'t' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 93, (((unsigned int)'e' << 0) | ((unsigned int)'r' << 8) | ((unsigned int)'E' << 16) | ((unsigned int)'x' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 16, 93, (0u | 0u | 0u | 0u)); \
    17u; \
})
#undef SetFilePointerEx
#define SetFilePointerEx(...) OBFH_API_CALL(2, SetFilePointerEx, __VA_ARGS__)

// Memory.

#define OBFH_GUI_ID_VirtualAlloc 94
#define OBFH_GUI_NAME_VirtualAlloc(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 94, (((unsigned int)'V' << 0) | ((unsigned int)'i' << 8) | ((unsigned int)'r' << 16) | ((unsigned int)'t' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 94, (((unsigned int)'u' << 0) | ((unsigned int)'a' << 8) | ((unsigned int)'l' << 16) | ((unsigned int)'A' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 94, (((unsigned int)'l' << 0) | ((unsigned int)'l' << 8) | ((unsigned int)'o' << 16) | ((unsigned int)'c' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 94, (0u | 0u | 0u | 0u)); \
    13u; \
})
#undef VirtualAlloc
#define VirtualAlloc(...) OBFH_API_CALL(2, VirtualAlloc, __VA_ARGS__)

#define OBFH_GUI_ID_VirtualProtect 95
#define OBFH_GUI_NAME_VirtualProtect(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 95, (((unsigned int)'V' << 0) | ((unsigned int)'i' << 8) | ((unsigned int)'r' << 16) | ((unsigned int)'t' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 95, (((unsigned int)'u' << 0) | ((unsigned int)'a' << 8) | ((unsigned int)'l' << 16) | ((unsigned int)'P' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 95, (((unsigned int)'r' << 0) | ((unsigned int)'o' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'e' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 95, (((unsigned int)'c' << 0) | ((unsigned int)'t' << 8) | 0u | 0u)); \
    15u; \
})
#undef VirtualProtect
#define VirtualProtect(...) OBFH_API_CALL(2, VirtualProtect, __VA_ARGS__)

#define OBFH_GUI_ID_VirtualFree 96
#define OBFH_GUI_NAME_VirtualFree(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 96, (((unsigned int)'V' << 0) | ((unsigned int)'i' << 8) | ((unsigned int)'r' << 16) | ((unsigned int)'t' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 96, (((unsigned int)'u' << 0) | ((unsigned int)'a' << 8) | ((unsigned int)'l' << 16) | ((unsigned int)'F' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 96, (((unsigned int)'r' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'e' << 16) | 0u)); \
    12u; \
})
#undef VirtualFree
#define VirtualFree(...) OBFH_API_CALL(2, VirtualFree, __VA_ARGS__)

#define OBFH_GUI_ID_GetProcessHeap 97
#define OBFH_GUI_NAME_GetProcessHeap(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 97, (((unsigned int)'G' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'P' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 97, (((unsigned int)'r' << 0) | ((unsigned int)'o' << 8) | ((unsigned int)'c' << 16) | ((unsigned int)'e' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 97, (((unsigned int)'s' << 0) | ((unsigned int)'s' << 8) | ((unsigned int)'H' << 16) | ((unsigned int)'e' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 97, (((unsigned int)'a' << 0) | ((unsigned int)'p' << 8) | 0u | 0u)); \
    15u; \
})
#undef GetProcessHeap
#define GetProcessHeap(...) OBFH_API_CALL(2, GetProcessHeap, __VA_ARGS__)

#define OBFH_GUI_ID_HeapAlloc 98
#define OBFH_GUI_NAME_HeapAlloc(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 98, (((unsigned int)'H' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'a' << 16) | ((unsigned int)'p' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 98, (((unsigned int)'A' << 0) | ((unsigned int)'l' << 8) | ((unsigned int)'l' << 16) | ((unsigned int)'o' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 98, (((unsigned int)'c' << 0) | 0u | 0u | 0u)); \
    10u; \
})
#undef HeapAlloc
#define HeapAlloc(...) OBFH_API_CALL(2, HeapAlloc, __VA_ARGS__)

#define OBFH_GUI_ID_HeapReAlloc 99
#define OBFH_GUI_NAME_HeapReAlloc(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 99, (((unsigned int)'H' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'a' << 16) | ((unsigned int)'p' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 99, (((unsigned int)'R' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'A' << 16) | ((unsigned int)'l' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 99, (((unsigned int)'l' << 0) | ((unsigned int)'o' << 8) | ((unsigned int)'c' << 16) | 0u)); \
    12u; \
})
#undef HeapReAlloc
#define HeapReAlloc(...) OBFH_API_CALL(2, HeapReAlloc, __VA_ARGS__)

#define OBFH_GUI_ID_HeapFree 100
#define OBFH_GUI_NAME_HeapFree(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 100, (((unsigned int)'H' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'a' << 16) | ((unsigned int)'p' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 100, (((unsigned int)'F' << 0) | ((unsigned int)'r' << 8) | ((unsigned int)'e' << 16) | ((unsigned int)'e' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 100, (0u | 0u | 0u | 0u)); \
    9u; \
})
#undef HeapFree
#define HeapFree(...) OBFH_API_CALL(2, HeapFree, __VA_ARGS__)

// Paths and environment.

#define OBFH_GUI_ID_GetModuleFileNameA 101
#define OBFH_GUI_NAME_GetModuleFileNameA(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 101, (((unsigned int)'G' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'M' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 101, (((unsigned int)'o' << 0) | ((unsigned int)'d' << 8) | ((unsigned int)'u' << 16) | ((unsigned int)'l' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 101, (((unsigned int)'e' << 0) | ((unsigned int)'F' << 8) | ((unsigned int)'i' << 16) | ((unsigned int)'l' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 101, (((unsigned int)'e' << 0) | ((unsigned int)'N' << 8) | ((unsigned int)'a' << 16) | ((unsigned int)'m' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 16, 101, (((unsigned int)'e' << 0) | ((unsigned int)'A' << 8) | 0u | 0u)); \
    19u; \
})
#undef GetModuleFileNameA
#define GetModuleFileNameA(...) OBFH_API_CALL(2, GetModuleFileNameA, __VA_ARGS__)

#define OBFH_GUI_ID_GetModuleFileNameW 102
#define OBFH_GUI_NAME_GetModuleFileNameW(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 102, (((unsigned int)'G' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'M' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 102, (((unsigned int)'o' << 0) | ((unsigned int)'d' << 8) | ((unsigned int)'u' << 16) | ((unsigned int)'l' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 102, (((unsigned int)'e' << 0) | ((unsigned int)'F' << 8) | ((unsigned int)'i' << 16) | ((unsigned int)'l' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 102, (((unsigned int)'e' << 0) | ((unsigned int)'N' << 8) | ((unsigned int)'a' << 16) | ((unsigned int)'m' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 16, 102, (((unsigned int)'e' << 0) | ((unsigned int)'W' << 8) | 0u | 0u)); \
    19u; \
})
#undef GetModuleFileNameW
#define GetModuleFileNameW(...) OBFH_API_CALL(2, GetModuleFileNameW, __VA_ARGS__)

#define OBFH_GUI_ID_GetEnvironmentVariableA 103
#define OBFH_GUI_NAME_GetEnvironmentVariableA(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 103, (((unsigned int)'G' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'E' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 103, (((unsigned int)'n' << 0) | ((unsigned int)'v' << 8) | ((unsigned int)'i' << 16) | ((unsigned int)'r' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 103, (((unsigned int)'o' << 0) | ((unsigned int)'n' << 8) | ((unsigned int)'m' << 16) | ((unsigned int)'e' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 103, (((unsigned int)'n' << 0) | ((unsigned int)'t' << 8) | ((unsigned int)'V' << 16) | ((unsigned int)'a' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 16, 103, (((unsigned int)'r' << 0) | ((unsigned int)'i' << 8) | ((unsigned int)'a' << 16) | ((unsigned int)'b' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 20, 103, (((unsigned int)'l' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'A' << 16) | 0u)); \
    24u; \
})
#undef GetEnvironmentVariableA
#define GetEnvironmentVariableA(...) OBFH_API_CALL(2, GetEnvironmentVariableA, __VA_ARGS__)

#define OBFH_GUI_ID_GetEnvironmentVariableW 104
#define OBFH_GUI_NAME_GetEnvironmentVariableW(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 104, (((unsigned int)'G' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'E' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 104, (((unsigned int)'n' << 0) | ((unsigned int)'v' << 8) | ((unsigned int)'i' << 16) | ((unsigned int)'r' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 104, (((unsigned int)'o' << 0) | ((unsigned int)'n' << 8) | ((unsigned int)'m' << 16) | ((unsigned int)'e' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 104, (((unsigned int)'n' << 0) | ((unsigned int)'t' << 8) | ((unsigned int)'V' << 16) | ((unsigned int)'a' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 16, 104, (((unsigned int)'r' << 0) | ((unsigned int)'i' << 8) | ((unsigned int)'a' << 16) | ((unsigned int)'b' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 20, 104, (((unsigned int)'l' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'W' << 16) | 0u)); \
    24u; \
})
#undef GetEnvironmentVariableW
#define GetEnvironmentVariableW(...) OBFH_API_CALL(2, GetEnvironmentVariableW, __VA_ARGS__)

#define OBFH_GUI_ID_GetCurrentDirectoryA 105
#define OBFH_GUI_NAME_GetCurrentDirectoryA(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 105, (((unsigned int)'G' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'C' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 105, (((unsigned int)'u' << 0) | ((unsigned int)'r' << 8) | ((unsigned int)'r' << 16) | ((unsigned int)'e' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 105, (((unsigned int)'n' << 0) | ((unsigned int)'t' << 8) | ((unsigned int)'D' << 16) | ((unsigned int)'i' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 105, (((unsigned int)'r' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'c' << 16) | ((unsigned int)'t' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 16, 105, (((unsigned int)'o' << 0) | ((unsigned int)'r' << 8) | ((unsigned int)'y' << 16) | ((unsigned int)'A' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 20, 105, (0u | 0u | 0u | 0u)); \
    21u; \
})
#undef GetCurrentDirectoryA
#define GetCurrentDirectoryA(...) OBFH_API_CALL(2, GetCurrentDirectoryA, __VA_ARGS__)

#define OBFH_GUI_ID_GetCurrentDirectoryW 106
#define OBFH_GUI_NAME_GetCurrentDirectoryW(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 106, (((unsigned int)'G' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'C' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 106, (((unsigned int)'u' << 0) | ((unsigned int)'r' << 8) | ((unsigned int)'r' << 16) | ((unsigned int)'e' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 106, (((unsigned int)'n' << 0) | ((unsigned int)'t' << 8) | ((unsigned int)'D' << 16) | ((unsigned int)'i' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 106, (((unsigned int)'r' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'c' << 16) | ((unsigned int)'t' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 16, 106, (((unsigned int)'o' << 0) | ((unsigned int)'r' << 8) | ((unsigned int)'y' << 16) | ((unsigned int)'W' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 20, 106, (0u | 0u | 0u | 0u)); \
    21u; \
})
#undef GetCurrentDirectoryW
#define GetCurrentDirectoryW(...) OBFH_API_CALL(2, GetCurrentDirectoryW, __VA_ARGS__)

#define OBFH_GUI_ID_GetTempPathA 107
#define OBFH_GUI_NAME_GetTempPathA(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 107, (((unsigned int)'G' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'T' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 107, (((unsigned int)'e' << 0) | ((unsigned int)'m' << 8) | ((unsigned int)'p' << 16) | ((unsigned int)'P' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 107, (((unsigned int)'a' << 0) | ((unsigned int)'t' << 8) | ((unsigned int)'h' << 16) | ((unsigned int)'A' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 107, (0u | 0u | 0u | 0u)); \
    13u; \
})
#undef GetTempPathA
#define GetTempPathA(...) OBFH_API_CALL(2, GetTempPathA, __VA_ARGS__)

#define OBFH_GUI_ID_GetTempPathW 108
#define OBFH_GUI_NAME_GetTempPathW(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 108, (((unsigned int)'G' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'T' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 108, (((unsigned int)'e' << 0) | ((unsigned int)'m' << 8) | ((unsigned int)'p' << 16) | ((unsigned int)'P' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 108, (((unsigned int)'a' << 0) | ((unsigned int)'t' << 8) | ((unsigned int)'h' << 16) | ((unsigned int)'W' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 108, (0u | 0u | 0u | 0u)); \
    13u; \
})
#undef GetTempPathW
#define GetTempPathW(...) OBFH_API_CALL(2, GetTempPathW, __VA_ARGS__)

// Threads and events.

#define OBFH_GUI_ID_CreateThread 109
#define OBFH_GUI_NAME_CreateThread(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 109, (((unsigned int)'C' << 0) | ((unsigned int)'r' << 8) | ((unsigned int)'e' << 16) | ((unsigned int)'a' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 109, (((unsigned int)'t' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'T' << 16) | ((unsigned int)'h' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 109, (((unsigned int)'r' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'a' << 16) | ((unsigned int)'d' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 109, (0u | 0u | 0u | 0u)); \
    13u; \
})
#undef CreateThread
#define CreateThread(...) OBFH_API_CALL(2, CreateThread, __VA_ARGS__)

#define OBFH_GUI_ID_WaitForSingleObject 110
#define OBFH_GUI_NAME_WaitForSingleObject(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 110, (((unsigned int)'W' << 0) | ((unsigned int)'a' << 8) | ((unsigned int)'i' << 16) | ((unsigned int)'t' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 110, (((unsigned int)'F' << 0) | ((unsigned int)'o' << 8) | ((unsigned int)'r' << 16) | ((unsigned int)'S' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 110, (((unsigned int)'i' << 0) | ((unsigned int)'n' << 8) | ((unsigned int)'g' << 16) | ((unsigned int)'l' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 110, (((unsigned int)'e' << 0) | ((unsigned int)'O' << 8) | ((unsigned int)'b' << 16) | ((unsigned int)'j' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 16, 110, (((unsigned int)'e' << 0) | ((unsigned int)'c' << 8) | ((unsigned int)'t' << 16) | 0u)); \
    20u; \
})
#undef WaitForSingleObject
#define WaitForSingleObject(...) OBFH_API_CALL(2, WaitForSingleObject, __VA_ARGS__)

#define OBFH_GUI_ID_WaitForMultipleObjects 111
#define OBFH_GUI_NAME_WaitForMultipleObjects(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 111, (((unsigned int)'W' << 0) | ((unsigned int)'a' << 8) | ((unsigned int)'i' << 16) | ((unsigned int)'t' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 111, (((unsigned int)'F' << 0) | ((unsigned int)'o' << 8) | ((unsigned int)'r' << 16) | ((unsigned int)'M' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 111, (((unsigned int)'u' << 0) | ((unsigned int)'l' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'i' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 111, (((unsigned int)'p' << 0) | ((unsigned int)'l' << 8) | ((unsigned int)'e' << 16) | ((unsigned int)'O' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 16, 111, (((unsigned int)'b' << 0) | ((unsigned int)'j' << 8) | ((unsigned int)'e' << 16) | ((unsigned int)'c' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 20, 111, (((unsigned int)'t' << 0) | ((unsigned int)'s' << 8) | 0u | 0u)); \
    23u; \
})
#undef WaitForMultipleObjects
#define WaitForMultipleObjects(...) OBFH_API_CALL(2, WaitForMultipleObjects, __VA_ARGS__)

#define OBFH_GUI_ID_CreateEventA 112
#define OBFH_GUI_NAME_CreateEventA(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 112, (((unsigned int)'C' << 0) | ((unsigned int)'r' << 8) | ((unsigned int)'e' << 16) | ((unsigned int)'a' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 112, (((unsigned int)'t' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'E' << 16) | ((unsigned int)'v' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 112, (((unsigned int)'e' << 0) | ((unsigned int)'n' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'A' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 112, (0u | 0u | 0u | 0u)); \
    13u; \
})
#undef CreateEventA
#define CreateEventA(...) OBFH_API_CALL(2, CreateEventA, __VA_ARGS__)

#define OBFH_GUI_ID_CreateEventW 113
#define OBFH_GUI_NAME_CreateEventW(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 113, (((unsigned int)'C' << 0) | ((unsigned int)'r' << 8) | ((unsigned int)'e' << 16) | ((unsigned int)'a' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 113, (((unsigned int)'t' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'E' << 16) | ((unsigned int)'v' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 113, (((unsigned int)'e' << 0) | ((unsigned int)'n' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'W' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 113, (0u | 0u | 0u | 0u)); \
    13u; \
})
#undef CreateEventW
#define CreateEventW(...) OBFH_API_CALL(2, CreateEventW, __VA_ARGS__)

#define OBFH_GUI_ID_SetEvent 114
#define OBFH_GUI_NAME_SetEvent(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 114, (((unsigned int)'S' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'E' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 114, (((unsigned int)'v' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'n' << 16) | ((unsigned int)'t' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 114, (0u | 0u | 0u | 0u)); \
    9u; \
})
#undef SetEvent
#define SetEvent(...) OBFH_API_CALL(2, SetEvent, __VA_ARGS__)

#define OBFH_GUI_ID_ResetEvent 115
#define OBFH_GUI_NAME_ResetEvent(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 115, (((unsigned int)'R' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'s' << 16) | ((unsigned int)'e' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 115, (((unsigned int)'t' << 0) | ((unsigned int)'E' << 8) | ((unsigned int)'v' << 16) | ((unsigned int)'e' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 115, (((unsigned int)'n' << 0) | ((unsigned int)'t' << 8) | 0u | 0u)); \
    11u; \
})
#undef ResetEvent
#define ResetEvent(...) OBFH_API_CALL(2, ResetEvent, __VA_ARGS__)

// Painting.

#define OBFH_GUI_ID_BeginPaint 116
#define OBFH_GUI_NAME_BeginPaint(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 116, (((unsigned int)'B' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'g' << 16) | ((unsigned int)'i' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 116, (((unsigned int)'n' << 0) | ((unsigned int)'P' << 8) | ((unsigned int)'a' << 16) | ((unsigned int)'i' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 116, (((unsigned int)'n' << 0) | ((unsigned int)'t' << 8) | 0u | 0u)); \
    11u; \
})
#undef BeginPaint
#define BeginPaint(...) OBFH_API_CALL(0, BeginPaint, __VA_ARGS__)

#define OBFH_GUI_ID_EndPaint 117
#define OBFH_GUI_NAME_EndPaint(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 117, (((unsigned int)'E' << 0) | ((unsigned int)'n' << 8) | ((unsigned int)'d' << 16) | ((unsigned int)'P' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 117, (((unsigned int)'a' << 0) | ((unsigned int)'i' << 8) | ((unsigned int)'n' << 16) | ((unsigned int)'t' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 117, (0u | 0u | 0u | 0u)); \
    9u; \
})
#undef EndPaint
#define EndPaint(...) OBFH_API_CALL(0, EndPaint, __VA_ARGS__)

#define OBFH_GUI_ID_DrawTextA 118
#define OBFH_GUI_NAME_DrawTextA(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 118, (((unsigned int)'D' << 0) | ((unsigned int)'r' << 8) | ((unsigned int)'a' << 16) | ((unsigned int)'w' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 118, (((unsigned int)'T' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'x' << 16) | ((unsigned int)'t' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 118, (((unsigned int)'A' << 0) | 0u | 0u | 0u)); \
    10u; \
})
#undef DrawTextA
#define DrawTextA(...) OBFH_API_CALL(0, DrawTextA, __VA_ARGS__)

#define OBFH_GUI_ID_DrawTextW 119
#define OBFH_GUI_NAME_DrawTextW(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 119, (((unsigned int)'D' << 0) | ((unsigned int)'r' << 8) | ((unsigned int)'a' << 16) | ((unsigned int)'w' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 119, (((unsigned int)'T' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'x' << 16) | ((unsigned int)'t' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 119, (((unsigned int)'W' << 0) | 0u | 0u | 0u)); \
    10u; \
})
#undef DrawTextW
#define DrawTextW(...) OBFH_API_CALL(0, DrawTextW, __VA_ARGS__)

#define OBFH_GUI_ID_TextOutA 120
#define OBFH_GUI_NAME_TextOutA(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 120, (((unsigned int)'T' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'x' << 16) | ((unsigned int)'t' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 120, (((unsigned int)'O' << 0) | ((unsigned int)'u' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'A' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 120, (0u | 0u | 0u | 0u)); \
    9u; \
})
#undef TextOutA
#define TextOutA(...) OBFH_API_CALL(1, TextOutA, __VA_ARGS__)

#define OBFH_GUI_ID_TextOutW 121
#define OBFH_GUI_NAME_TextOutW(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 121, (((unsigned int)'T' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'x' << 16) | ((unsigned int)'t' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 121, (((unsigned int)'O' << 0) | ((unsigned int)'u' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'W' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 121, (0u | 0u | 0u | 0u)); \
    9u; \
})
#undef TextOutW
#define TextOutW(...) OBFH_API_CALL(1, TextOutW, __VA_ARGS__)

#define OBFH_GUI_ID_BitBlt 122
#define OBFH_GUI_NAME_BitBlt(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 122, (((unsigned int)'B' << 0) | ((unsigned int)'i' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'B' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 122, (((unsigned int)'l' << 0) | ((unsigned int)'t' << 8) | 0u | 0u)); \
    7u; \
})
#undef BitBlt
#define BitBlt(...) OBFH_API_CALL(1, BitBlt, __VA_ARGS__)

#define OBFH_GUI_ID_CreateCompatibleDC 123
#define OBFH_GUI_NAME_CreateCompatibleDC(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 123, (((unsigned int)'C' << 0) | ((unsigned int)'r' << 8) | ((unsigned int)'e' << 16) | ((unsigned int)'a' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 123, (((unsigned int)'t' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'C' << 16) | ((unsigned int)'o' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 123, (((unsigned int)'m' << 0) | ((unsigned int)'p' << 8) | ((unsigned int)'a' << 16) | ((unsigned int)'t' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 123, (((unsigned int)'i' << 0) | ((unsigned int)'b' << 8) | ((unsigned int)'l' << 16) | ((unsigned int)'e' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 16, 123, (((unsigned int)'D' << 0) | ((unsigned int)'C' << 8) | 0u | 0u)); \
    19u; \
})
#undef CreateCompatibleDC
#define CreateCompatibleDC(...) OBFH_API_CALL(1, CreateCompatibleDC, __VA_ARGS__)

#define OBFH_GUI_ID_CreateCompatibleBitmap 124
#define OBFH_GUI_NAME_CreateCompatibleBitmap(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 124, (((unsigned int)'C' << 0) | ((unsigned int)'r' << 8) | ((unsigned int)'e' << 16) | ((unsigned int)'a' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 124, (((unsigned int)'t' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'C' << 16) | ((unsigned int)'o' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 124, (((unsigned int)'m' << 0) | ((unsigned int)'p' << 8) | ((unsigned int)'a' << 16) | ((unsigned int)'t' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 124, (((unsigned int)'i' << 0) | ((unsigned int)'b' << 8) | ((unsigned int)'l' << 16) | ((unsigned int)'e' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 16, 124, (((unsigned int)'B' << 0) | ((unsigned int)'i' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'m' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 20, 124, (((unsigned int)'a' << 0) | ((unsigned int)'p' << 8) | 0u | 0u)); \
    23u; \
})
#undef CreateCompatibleBitmap
#define CreateCompatibleBitmap(...) OBFH_API_CALL(1, CreateCompatibleBitmap, __VA_ARGS__)

#define OBFH_GUI_ID_GetStockObject 125
#define OBFH_GUI_NAME_GetStockObject(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 125, (((unsigned int)'G' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'S' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 125, (((unsigned int)'t' << 0) | ((unsigned int)'o' << 8) | ((unsigned int)'c' << 16) | ((unsigned int)'k' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 125, (((unsigned int)'O' << 0) | ((unsigned int)'b' << 8) | ((unsigned int)'j' << 16) | ((unsigned int)'e' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 125, (((unsigned int)'c' << 0) | ((unsigned int)'t' << 8) | 0u | 0u)); \
    15u; \
})
#undef GetStockObject
#define GetStockObject(...) OBFH_API_CALL(1, GetStockObject, __VA_ARGS__)

// Registry.

#define OBFH_GUI_ID_RegOpenKeyExA 126
#define OBFH_GUI_NAME_RegOpenKeyExA(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 126, (((unsigned int)'R' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'g' << 16) | ((unsigned int)'O' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 126, (((unsigned int)'p' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'n' << 16) | ((unsigned int)'K' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 126, (((unsigned int)'e' << 0) | ((unsigned int)'y' << 8) | ((unsigned int)'E' << 16) | ((unsigned int)'x' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 126, (((unsigned int)'A' << 0) | 0u | 0u | 0u)); \
    14u; \
})
#undef RegOpenKeyExA
#define RegOpenKeyExA(...) OBFH_API_CALL(3, RegOpenKeyExA, __VA_ARGS__)

#define OBFH_GUI_ID_RegOpenKeyExW 127
#define OBFH_GUI_NAME_RegOpenKeyExW(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 127, (((unsigned int)'R' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'g' << 16) | ((unsigned int)'O' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 127, (((unsigned int)'p' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'n' << 16) | ((unsigned int)'K' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 127, (((unsigned int)'e' << 0) | ((unsigned int)'y' << 8) | ((unsigned int)'E' << 16) | ((unsigned int)'x' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 127, (((unsigned int)'W' << 0) | 0u | 0u | 0u)); \
    14u; \
})
#undef RegOpenKeyExW
#define RegOpenKeyExW(...) OBFH_API_CALL(3, RegOpenKeyExW, __VA_ARGS__)

#define OBFH_GUI_ID_RegQueryValueExA 128
#define OBFH_GUI_NAME_RegQueryValueExA(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 128, (((unsigned int)'R' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'g' << 16) | ((unsigned int)'Q' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 128, (((unsigned int)'u' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'r' << 16) | ((unsigned int)'y' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 128, (((unsigned int)'V' << 0) | ((unsigned int)'a' << 8) | ((unsigned int)'l' << 16) | ((unsigned int)'u' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 128, (((unsigned int)'e' << 0) | ((unsigned int)'E' << 8) | ((unsigned int)'x' << 16) | ((unsigned int)'A' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 16, 128, (0u | 0u | 0u | 0u)); \
    17u; \
})
#undef RegQueryValueExA
#define RegQueryValueExA(...) OBFH_API_CALL(3, RegQueryValueExA, __VA_ARGS__)

#define OBFH_GUI_ID_RegQueryValueExW 129
#define OBFH_GUI_NAME_RegQueryValueExW(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 129, (((unsigned int)'R' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'g' << 16) | ((unsigned int)'Q' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 129, (((unsigned int)'u' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'r' << 16) | ((unsigned int)'y' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 129, (((unsigned int)'V' << 0) | ((unsigned int)'a' << 8) | ((unsigned int)'l' << 16) | ((unsigned int)'u' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 129, (((unsigned int)'e' << 0) | ((unsigned int)'E' << 8) | ((unsigned int)'x' << 16) | ((unsigned int)'W' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 16, 129, (0u | 0u | 0u | 0u)); \
    17u; \
})
#undef RegQueryValueExW
#define RegQueryValueExW(...) OBFH_API_CALL(3, RegQueryValueExW, __VA_ARGS__)

#define OBFH_GUI_ID_RegSetValueExA 130
#define OBFH_GUI_NAME_RegSetValueExA(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 130, (((unsigned int)'R' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'g' << 16) | ((unsigned int)'S' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 130, (((unsigned int)'e' << 0) | ((unsigned int)'t' << 8) | ((unsigned int)'V' << 16) | ((unsigned int)'a' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 130, (((unsigned int)'l' << 0) | ((unsigned int)'u' << 8) | ((unsigned int)'e' << 16) | ((unsigned int)'E' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 130, (((unsigned int)'x' << 0) | ((unsigned int)'A' << 8) | 0u | 0u)); \
    15u; \
})
#undef RegSetValueExA
#define RegSetValueExA(...) OBFH_API_CALL(3, RegSetValueExA, __VA_ARGS__)

#define OBFH_GUI_ID_RegSetValueExW 131
#define OBFH_GUI_NAME_RegSetValueExW(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 131, (((unsigned int)'R' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'g' << 16) | ((unsigned int)'S' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 131, (((unsigned int)'e' << 0) | ((unsigned int)'t' << 8) | ((unsigned int)'V' << 16) | ((unsigned int)'a' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 131, (((unsigned int)'l' << 0) | ((unsigned int)'u' << 8) | ((unsigned int)'e' << 16) | ((unsigned int)'E' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 131, (((unsigned int)'x' << 0) | ((unsigned int)'W' << 8) | 0u | 0u)); \
    15u; \
})
#undef RegSetValueExW
#define RegSetValueExW(...) OBFH_API_CALL(3, RegSetValueExW, __VA_ARGS__)

#define OBFH_GUI_ID_RegCloseKey 132
#define OBFH_GUI_NAME_RegCloseKey(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 132, (((unsigned int)'R' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'g' << 16) | ((unsigned int)'C' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 132, (((unsigned int)'l' << 0) | ((unsigned int)'o' << 8) | ((unsigned int)'s' << 16) | ((unsigned int)'e' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 132, (((unsigned int)'K' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'y' << 16) | 0u)); \
    12u; \
})
#undef RegCloseKey
#define RegCloseKey(...) OBFH_API_CALL(3, RegCloseKey, __VA_ARGS__)

// Loading.

#define OBFH_GUI_ID_LoadLibraryW 133
#define OBFH_GUI_NAME_LoadLibraryW(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 133, (((unsigned int)'L' << 0) | ((unsigned int)'o' << 8) | ((unsigned int)'a' << 16) | ((unsigned int)'d' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 133, (((unsigned int)'L' << 0) | ((unsigned int)'i' << 8) | ((unsigned int)'b' << 16) | ((unsigned int)'r' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 133, (((unsigned int)'a' << 0) | ((unsigned int)'r' << 8) | ((unsigned int)'y' << 16) | ((unsigned int)'W' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 133, (0u | 0u | 0u | 0u)); \
    13u; \
})
#undef LoadLibraryW
#define LoadLibraryW(...) OBFH_API_CALL(2, LoadLibraryW, __VA_ARGS__)

#define OBFH_GUI_ID_LoadLibraryExA 134
#define OBFH_GUI_NAME_LoadLibraryExA(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 134, (((unsigned int)'L' << 0) | ((unsigned int)'o' << 8) | ((unsigned int)'a' << 16) | ((unsigned int)'d' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 134, (((unsigned int)'L' << 0) | ((unsigned int)'i' << 8) | ((unsigned int)'b' << 16) | ((unsigned int)'r' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 134, (((unsigned int)'a' << 0) | ((unsigned int)'r' << 8) | ((unsigned int)'y' << 16) | ((unsigned int)'E' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 134, (((unsigned int)'x' << 0) | ((unsigned int)'A' << 8) | 0u | 0u)); \
    15u; \
})
#undef LoadLibraryExA
#define LoadLibraryExA(...) OBFH_API_CALL(2, LoadLibraryExA, __VA_ARGS__)

#define OBFH_GUI_ID_LoadLibraryExW 135
#define OBFH_GUI_NAME_LoadLibraryExW(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 135, (((unsigned int)'L' << 0) | ((unsigned int)'o' << 8) | ((unsigned int)'a' << 16) | ((unsigned int)'d' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 135, (((unsigned int)'L' << 0) | ((unsigned int)'i' << 8) | ((unsigned int)'b' << 16) | ((unsigned int)'r' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 135, (((unsigned int)'a' << 0) | ((unsigned int)'r' << 8) | ((unsigned int)'y' << 16) | ((unsigned int)'E' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 135, (((unsigned int)'x' << 0) | ((unsigned int)'W' << 8) | 0u | 0u)); \
    15u; \
})
#undef LoadLibraryExW
#define LoadLibraryExW(...) OBFH_API_CALL(2, LoadLibraryExW, __VA_ARGS__)

// ============================================================================
// File mappings, process control, synchronization and additional GUI calls.

#define OBFH_GUI_ID_CreateDirectoryA 189
#define OBFH_GUI_NAME_CreateDirectoryA(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 189, (((unsigned int)'C' << 0) | ((unsigned int)'r' << 8) | ((unsigned int)'e' << 16) | ((unsigned int)'a' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 189, (((unsigned int)'t' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'D' << 16) | ((unsigned int)'i' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 189, (((unsigned int)'r' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'c' << 16) | ((unsigned int)'t' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 189, (((unsigned int)'o' << 0) | ((unsigned int)'r' << 8) | ((unsigned int)'y' << 16) | ((unsigned int)'A' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 16, 189, (0u | 0u | 0u | 0u)); \
    17u; \
})
#undef CreateDirectoryA
#define CreateDirectoryA(...) OBFH_API_CALL(2, CreateDirectoryA, __VA_ARGS__)

#define OBFH_GUI_ID_CreateDirectoryW 190
#define OBFH_GUI_NAME_CreateDirectoryW(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 190, (((unsigned int)'C' << 0) | ((unsigned int)'r' << 8) | ((unsigned int)'e' << 16) | ((unsigned int)'a' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 190, (((unsigned int)'t' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'D' << 16) | ((unsigned int)'i' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 190, (((unsigned int)'r' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'c' << 16) | ((unsigned int)'t' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 190, (((unsigned int)'o' << 0) | ((unsigned int)'r' << 8) | ((unsigned int)'y' << 16) | ((unsigned int)'W' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 16, 190, (0u | 0u | 0u | 0u)); \
    17u; \
})
#undef CreateDirectoryW
#define CreateDirectoryW(...) OBFH_API_CALL(2, CreateDirectoryW, __VA_ARGS__)

#define OBFH_GUI_ID_RemoveDirectoryA 191
#define OBFH_GUI_NAME_RemoveDirectoryA(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 191, (((unsigned int)'R' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'m' << 16) | ((unsigned int)'o' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 191, (((unsigned int)'v' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'D' << 16) | ((unsigned int)'i' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 191, (((unsigned int)'r' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'c' << 16) | ((unsigned int)'t' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 191, (((unsigned int)'o' << 0) | ((unsigned int)'r' << 8) | ((unsigned int)'y' << 16) | ((unsigned int)'A' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 16, 191, (0u | 0u | 0u | 0u)); \
    17u; \
})
#undef RemoveDirectoryA
#define RemoveDirectoryA(...) OBFH_API_CALL(2, RemoveDirectoryA, __VA_ARGS__)

#define OBFH_GUI_ID_RemoveDirectoryW 192
#define OBFH_GUI_NAME_RemoveDirectoryW(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 192, (((unsigned int)'R' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'m' << 16) | ((unsigned int)'o' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 192, (((unsigned int)'v' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'D' << 16) | ((unsigned int)'i' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 192, (((unsigned int)'r' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'c' << 16) | ((unsigned int)'t' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 192, (((unsigned int)'o' << 0) | ((unsigned int)'r' << 8) | ((unsigned int)'y' << 16) | ((unsigned int)'W' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 16, 192, (0u | 0u | 0u | 0u)); \
    17u; \
})
#undef RemoveDirectoryW
#define RemoveDirectoryW(...) OBFH_API_CALL(2, RemoveDirectoryW, __VA_ARGS__)

#define OBFH_GUI_ID_SetCurrentDirectoryA 193
#define OBFH_GUI_NAME_SetCurrentDirectoryA(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 193, (((unsigned int)'S' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'C' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 193, (((unsigned int)'u' << 0) | ((unsigned int)'r' << 8) | ((unsigned int)'r' << 16) | ((unsigned int)'e' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 193, (((unsigned int)'n' << 0) | ((unsigned int)'t' << 8) | ((unsigned int)'D' << 16) | ((unsigned int)'i' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 193, (((unsigned int)'r' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'c' << 16) | ((unsigned int)'t' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 16, 193, (((unsigned int)'o' << 0) | ((unsigned int)'r' << 8) | ((unsigned int)'y' << 16) | ((unsigned int)'A' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 20, 193, (0u | 0u | 0u | 0u)); \
    21u; \
})
#undef SetCurrentDirectoryA
#define SetCurrentDirectoryA(...) OBFH_API_CALL(2, SetCurrentDirectoryA, __VA_ARGS__)

#define OBFH_GUI_ID_SetCurrentDirectoryW 194
#define OBFH_GUI_NAME_SetCurrentDirectoryW(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 194, (((unsigned int)'S' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'C' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 194, (((unsigned int)'u' << 0) | ((unsigned int)'r' << 8) | ((unsigned int)'r' << 16) | ((unsigned int)'e' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 194, (((unsigned int)'n' << 0) | ((unsigned int)'t' << 8) | ((unsigned int)'D' << 16) | ((unsigned int)'i' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 194, (((unsigned int)'r' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'c' << 16) | ((unsigned int)'t' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 16, 194, (((unsigned int)'o' << 0) | ((unsigned int)'r' << 8) | ((unsigned int)'y' << 16) | ((unsigned int)'W' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 20, 194, (0u | 0u | 0u | 0u)); \
    21u; \
})
#undef SetCurrentDirectoryW
#define SetCurrentDirectoryW(...) OBFH_API_CALL(2, SetCurrentDirectoryW, __VA_ARGS__)

#define OBFH_GUI_ID_GetFullPathNameA 195
#define OBFH_GUI_NAME_GetFullPathNameA(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 195, (((unsigned int)'G' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'F' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 195, (((unsigned int)'u' << 0) | ((unsigned int)'l' << 8) | ((unsigned int)'l' << 16) | ((unsigned int)'P' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 195, (((unsigned int)'a' << 0) | ((unsigned int)'t' << 8) | ((unsigned int)'h' << 16) | ((unsigned int)'N' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 195, (((unsigned int)'a' << 0) | ((unsigned int)'m' << 8) | ((unsigned int)'e' << 16) | ((unsigned int)'A' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 16, 195, (0u | 0u | 0u | 0u)); \
    17u; \
})
#undef GetFullPathNameA
#define GetFullPathNameA(...) OBFH_API_CALL(2, GetFullPathNameA, __VA_ARGS__)

#define OBFH_GUI_ID_GetFullPathNameW 196
#define OBFH_GUI_NAME_GetFullPathNameW(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 196, (((unsigned int)'G' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'F' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 196, (((unsigned int)'u' << 0) | ((unsigned int)'l' << 8) | ((unsigned int)'l' << 16) | ((unsigned int)'P' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 196, (((unsigned int)'a' << 0) | ((unsigned int)'t' << 8) | ((unsigned int)'h' << 16) | ((unsigned int)'N' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 196, (((unsigned int)'a' << 0) | ((unsigned int)'m' << 8) | ((unsigned int)'e' << 16) | ((unsigned int)'W' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 16, 196, (0u | 0u | 0u | 0u)); \
    17u; \
})
#undef GetFullPathNameW
#define GetFullPathNameW(...) OBFH_API_CALL(2, GetFullPathNameW, __VA_ARGS__)

#define OBFH_GUI_ID_GetTempFileNameA 197
#define OBFH_GUI_NAME_GetTempFileNameA(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 197, (((unsigned int)'G' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'T' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 197, (((unsigned int)'e' << 0) | ((unsigned int)'m' << 8) | ((unsigned int)'p' << 16) | ((unsigned int)'F' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 197, (((unsigned int)'i' << 0) | ((unsigned int)'l' << 8) | ((unsigned int)'e' << 16) | ((unsigned int)'N' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 197, (((unsigned int)'a' << 0) | ((unsigned int)'m' << 8) | ((unsigned int)'e' << 16) | ((unsigned int)'A' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 16, 197, (0u | 0u | 0u | 0u)); \
    17u; \
})
#undef GetTempFileNameA
#define GetTempFileNameA(...) OBFH_API_CALL(2, GetTempFileNameA, __VA_ARGS__)

#define OBFH_GUI_ID_GetTempFileNameW 198
#define OBFH_GUI_NAME_GetTempFileNameW(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 198, (((unsigned int)'G' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'T' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 198, (((unsigned int)'e' << 0) | ((unsigned int)'m' << 8) | ((unsigned int)'p' << 16) | ((unsigned int)'F' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 198, (((unsigned int)'i' << 0) | ((unsigned int)'l' << 8) | ((unsigned int)'e' << 16) | ((unsigned int)'N' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 198, (((unsigned int)'a' << 0) | ((unsigned int)'m' << 8) | ((unsigned int)'e' << 16) | ((unsigned int)'W' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 16, 198, (0u | 0u | 0u | 0u)); \
    17u; \
})
#undef GetTempFileNameW
#define GetTempFileNameW(...) OBFH_API_CALL(2, GetTempFileNameW, __VA_ARGS__)

#define OBFH_GUI_ID_CreateFileMappingA 199
#define OBFH_GUI_NAME_CreateFileMappingA(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 199, (((unsigned int)'C' << 0) | ((unsigned int)'r' << 8) | ((unsigned int)'e' << 16) | ((unsigned int)'a' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 199, (((unsigned int)'t' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'F' << 16) | ((unsigned int)'i' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 199, (((unsigned int)'l' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'M' << 16) | ((unsigned int)'a' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 199, (((unsigned int)'p' << 0) | ((unsigned int)'p' << 8) | ((unsigned int)'i' << 16) | ((unsigned int)'n' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 16, 199, (((unsigned int)'g' << 0) | ((unsigned int)'A' << 8) | 0u | 0u)); \
    19u; \
})
#undef CreateFileMappingA
#define CreateFileMappingA(...) OBFH_API_CALL(2, CreateFileMappingA, __VA_ARGS__)

#define OBFH_GUI_ID_CreateFileMappingW 200
#define OBFH_GUI_NAME_CreateFileMappingW(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 200, (((unsigned int)'C' << 0) | ((unsigned int)'r' << 8) | ((unsigned int)'e' << 16) | ((unsigned int)'a' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 200, (((unsigned int)'t' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'F' << 16) | ((unsigned int)'i' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 200, (((unsigned int)'l' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'M' << 16) | ((unsigned int)'a' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 200, (((unsigned int)'p' << 0) | ((unsigned int)'p' << 8) | ((unsigned int)'i' << 16) | ((unsigned int)'n' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 16, 200, (((unsigned int)'g' << 0) | ((unsigned int)'W' << 8) | 0u | 0u)); \
    19u; \
})
#undef CreateFileMappingW
#define CreateFileMappingW(...) OBFH_API_CALL(2, CreateFileMappingW, __VA_ARGS__)

#define OBFH_GUI_ID_OpenFileMappingA 201
#define OBFH_GUI_NAME_OpenFileMappingA(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 201, (((unsigned int)'O' << 0) | ((unsigned int)'p' << 8) | ((unsigned int)'e' << 16) | ((unsigned int)'n' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 201, (((unsigned int)'F' << 0) | ((unsigned int)'i' << 8) | ((unsigned int)'l' << 16) | ((unsigned int)'e' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 201, (((unsigned int)'M' << 0) | ((unsigned int)'a' << 8) | ((unsigned int)'p' << 16) | ((unsigned int)'p' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 201, (((unsigned int)'i' << 0) | ((unsigned int)'n' << 8) | ((unsigned int)'g' << 16) | ((unsigned int)'A' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 16, 201, (0u | 0u | 0u | 0u)); \
    17u; \
})
#undef OpenFileMappingA
#define OpenFileMappingA(...) OBFH_API_CALL(2, OpenFileMappingA, __VA_ARGS__)

#define OBFH_GUI_ID_OpenFileMappingW 202
#define OBFH_GUI_NAME_OpenFileMappingW(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 202, (((unsigned int)'O' << 0) | ((unsigned int)'p' << 8) | ((unsigned int)'e' << 16) | ((unsigned int)'n' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 202, (((unsigned int)'F' << 0) | ((unsigned int)'i' << 8) | ((unsigned int)'l' << 16) | ((unsigned int)'e' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 202, (((unsigned int)'M' << 0) | ((unsigned int)'a' << 8) | ((unsigned int)'p' << 16) | ((unsigned int)'p' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 202, (((unsigned int)'i' << 0) | ((unsigned int)'n' << 8) | ((unsigned int)'g' << 16) | ((unsigned int)'W' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 16, 202, (0u | 0u | 0u | 0u)); \
    17u; \
})
#undef OpenFileMappingW
#define OpenFileMappingW(...) OBFH_API_CALL(2, OpenFileMappingW, __VA_ARGS__)

#define OBFH_GUI_ID_CreateProcessA 203
#define OBFH_GUI_NAME_CreateProcessA(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 203, (((unsigned int)'C' << 0) | ((unsigned int)'r' << 8) | ((unsigned int)'e' << 16) | ((unsigned int)'a' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 203, (((unsigned int)'t' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'P' << 16) | ((unsigned int)'r' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 203, (((unsigned int)'o' << 0) | ((unsigned int)'c' << 8) | ((unsigned int)'e' << 16) | ((unsigned int)'s' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 203, (((unsigned int)'s' << 0) | ((unsigned int)'A' << 8) | 0u | 0u)); \
    15u; \
})
#undef CreateProcessA
#define CreateProcessA(...) OBFH_API_CALL(2, CreateProcessA, __VA_ARGS__)

#define OBFH_GUI_ID_CreateProcessW 204
#define OBFH_GUI_NAME_CreateProcessW(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 204, (((unsigned int)'C' << 0) | ((unsigned int)'r' << 8) | ((unsigned int)'e' << 16) | ((unsigned int)'a' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 204, (((unsigned int)'t' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'P' << 16) | ((unsigned int)'r' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 204, (((unsigned int)'o' << 0) | ((unsigned int)'c' << 8) | ((unsigned int)'e' << 16) | ((unsigned int)'s' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 204, (((unsigned int)'s' << 0) | ((unsigned int)'W' << 8) | 0u | 0u)); \
    15u; \
})
#undef CreateProcessW
#define CreateProcessW(...) OBFH_API_CALL(2, CreateProcessW, __VA_ARGS__)

#define OBFH_GUI_ID_CreateMutexA 205
#define OBFH_GUI_NAME_CreateMutexA(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 205, (((unsigned int)'C' << 0) | ((unsigned int)'r' << 8) | ((unsigned int)'e' << 16) | ((unsigned int)'a' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 205, (((unsigned int)'t' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'M' << 16) | ((unsigned int)'u' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 205, (((unsigned int)'t' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'x' << 16) | ((unsigned int)'A' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 205, (0u | 0u | 0u | 0u)); \
    13u; \
})
#undef CreateMutexA
#define CreateMutexA(...) OBFH_API_CALL(2, CreateMutexA, __VA_ARGS__)

#define OBFH_GUI_ID_CreateMutexW 206
#define OBFH_GUI_NAME_CreateMutexW(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 206, (((unsigned int)'C' << 0) | ((unsigned int)'r' << 8) | ((unsigned int)'e' << 16) | ((unsigned int)'a' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 206, (((unsigned int)'t' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'M' << 16) | ((unsigned int)'u' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 206, (((unsigned int)'t' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'x' << 16) | ((unsigned int)'W' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 206, (0u | 0u | 0u | 0u)); \
    13u; \
})
#undef CreateMutexW
#define CreateMutexW(...) OBFH_API_CALL(2, CreateMutexW, __VA_ARGS__)

#define OBFH_GUI_ID_CreateSemaphoreA 207
#define OBFH_GUI_NAME_CreateSemaphoreA(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 207, (((unsigned int)'C' << 0) | ((unsigned int)'r' << 8) | ((unsigned int)'e' << 16) | ((unsigned int)'a' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 207, (((unsigned int)'t' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'S' << 16) | ((unsigned int)'e' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 207, (((unsigned int)'m' << 0) | ((unsigned int)'a' << 8) | ((unsigned int)'p' << 16) | ((unsigned int)'h' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 207, (((unsigned int)'o' << 0) | ((unsigned int)'r' << 8) | ((unsigned int)'e' << 16) | ((unsigned int)'A' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 16, 207, (0u | 0u | 0u | 0u)); \
    17u; \
})
#undef CreateSemaphoreA
#define CreateSemaphoreA(...) OBFH_API_CALL(2, CreateSemaphoreA, __VA_ARGS__)

#define OBFH_GUI_ID_CreateSemaphoreW 208
#define OBFH_GUI_NAME_CreateSemaphoreW(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 208, (((unsigned int)'C' << 0) | ((unsigned int)'r' << 8) | ((unsigned int)'e' << 16) | ((unsigned int)'a' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 208, (((unsigned int)'t' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'S' << 16) | ((unsigned int)'e' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 208, (((unsigned int)'m' << 0) | ((unsigned int)'a' << 8) | ((unsigned int)'p' << 16) | ((unsigned int)'h' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 208, (((unsigned int)'o' << 0) | ((unsigned int)'r' << 8) | ((unsigned int)'e' << 16) | ((unsigned int)'W' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 16, 208, (0u | 0u | 0u | 0u)); \
    17u; \
})
#undef CreateSemaphoreW
#define CreateSemaphoreW(...) OBFH_API_CALL(2, CreateSemaphoreW, __VA_ARGS__)

#define OBFH_GUI_ID_ReadConsoleA 209
#define OBFH_GUI_NAME_ReadConsoleA(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 209, (((unsigned int)'R' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'a' << 16) | ((unsigned int)'d' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 209, (((unsigned int)'C' << 0) | ((unsigned int)'o' << 8) | ((unsigned int)'n' << 16) | ((unsigned int)'s' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 209, (((unsigned int)'o' << 0) | ((unsigned int)'l' << 8) | ((unsigned int)'e' << 16) | ((unsigned int)'A' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 209, (0u | 0u | 0u | 0u)); \
    13u; \
})
#undef ReadConsoleA
#define ReadConsoleA(...) OBFH_API_CALL(2, ReadConsoleA, __VA_ARGS__)

#define OBFH_GUI_ID_ReadConsoleW 210
#define OBFH_GUI_NAME_ReadConsoleW(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 210, (((unsigned int)'R' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'a' << 16) | ((unsigned int)'d' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 210, (((unsigned int)'C' << 0) | ((unsigned int)'o' << 8) | ((unsigned int)'n' << 16) | ((unsigned int)'s' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 210, (((unsigned int)'o' << 0) | ((unsigned int)'l' << 8) | ((unsigned int)'e' << 16) | ((unsigned int)'W' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 210, (0u | 0u | 0u | 0u)); \
    13u; \
})
#undef ReadConsoleW
#define ReadConsoleW(...) OBFH_API_CALL(2, ReadConsoleW, __VA_ARGS__)

#define OBFH_GUI_ID_FlushFileBuffers 211
#define OBFH_GUI_NAME_FlushFileBuffers(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 211, (((unsigned int)'F' << 0) | ((unsigned int)'l' << 8) | ((unsigned int)'u' << 16) | ((unsigned int)'s' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 211, (((unsigned int)'h' << 0) | ((unsigned int)'F' << 8) | ((unsigned int)'i' << 16) | ((unsigned int)'l' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 211, (((unsigned int)'e' << 0) | ((unsigned int)'B' << 8) | ((unsigned int)'u' << 16) | ((unsigned int)'f' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 211, (((unsigned int)'f' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'r' << 16) | ((unsigned int)'s' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 16, 211, (0u | 0u | 0u | 0u)); \
    17u; \
})
#undef FlushFileBuffers
#define FlushFileBuffers(...) OBFH_API_CALL(2, FlushFileBuffers, __VA_ARGS__)

#define OBFH_GUI_ID_GetFileTime 212
#define OBFH_GUI_NAME_GetFileTime(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 212, (((unsigned int)'G' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'F' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 212, (((unsigned int)'i' << 0) | ((unsigned int)'l' << 8) | ((unsigned int)'e' << 16) | ((unsigned int)'T' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 212, (((unsigned int)'i' << 0) | ((unsigned int)'m' << 8) | ((unsigned int)'e' << 16) | 0u)); \
    12u; \
})
#undef GetFileTime
#define GetFileTime(...) OBFH_API_CALL(2, GetFileTime, __VA_ARGS__)

#define OBFH_GUI_ID_SetFileTime 213
#define OBFH_GUI_NAME_SetFileTime(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 213, (((unsigned int)'S' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'F' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 213, (((unsigned int)'i' << 0) | ((unsigned int)'l' << 8) | ((unsigned int)'e' << 16) | ((unsigned int)'T' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 213, (((unsigned int)'i' << 0) | ((unsigned int)'m' << 8) | ((unsigned int)'e' << 16) | 0u)); \
    12u; \
})
#undef SetFileTime
#define SetFileTime(...) OBFH_API_CALL(2, SetFileTime, __VA_ARGS__)

#define OBFH_GUI_ID_GetFileInformationByHandle 214
#define OBFH_GUI_NAME_GetFileInformationByHandle(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 214, (((unsigned int)'G' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'F' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 214, (((unsigned int)'i' << 0) | ((unsigned int)'l' << 8) | ((unsigned int)'e' << 16) | ((unsigned int)'I' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 214, (((unsigned int)'n' << 0) | ((unsigned int)'f' << 8) | ((unsigned int)'o' << 16) | ((unsigned int)'r' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 214, (((unsigned int)'m' << 0) | ((unsigned int)'a' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'i' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 16, 214, (((unsigned int)'o' << 0) | ((unsigned int)'n' << 8) | ((unsigned int)'B' << 16) | ((unsigned int)'y' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 20, 214, (((unsigned int)'H' << 0) | ((unsigned int)'a' << 8) | ((unsigned int)'n' << 16) | ((unsigned int)'d' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 24, 214, (((unsigned int)'l' << 0) | ((unsigned int)'e' << 8) | 0u | 0u)); \
    27u; \
})
#undef GetFileInformationByHandle
#define GetFileInformationByHandle(...) OBFH_API_CALL(2, GetFileInformationByHandle, __VA_ARGS__)

#define OBFH_GUI_ID_GetOverlappedResult 215
#define OBFH_GUI_NAME_GetOverlappedResult(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 215, (((unsigned int)'G' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'O' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 215, (((unsigned int)'v' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'r' << 16) | ((unsigned int)'l' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 215, (((unsigned int)'a' << 0) | ((unsigned int)'p' << 8) | ((unsigned int)'p' << 16) | ((unsigned int)'e' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 215, (((unsigned int)'d' << 0) | ((unsigned int)'R' << 8) | ((unsigned int)'e' << 16) | ((unsigned int)'s' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 16, 215, (((unsigned int)'u' << 0) | ((unsigned int)'l' << 8) | ((unsigned int)'t' << 16) | 0u)); \
    20u; \
})
#undef GetOverlappedResult
#define GetOverlappedResult(...) OBFH_API_CALL(2, GetOverlappedResult, __VA_ARGS__)

#define OBFH_GUI_ID_CancelIo 216
#define OBFH_GUI_NAME_CancelIo(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 216, (((unsigned int)'C' << 0) | ((unsigned int)'a' << 8) | ((unsigned int)'n' << 16) | ((unsigned int)'c' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 216, (((unsigned int)'e' << 0) | ((unsigned int)'l' << 8) | ((unsigned int)'I' << 16) | ((unsigned int)'o' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 216, (0u | 0u | 0u | 0u)); \
    9u; \
})
#undef CancelIo
#define CancelIo(...) OBFH_API_CALL(2, CancelIo, __VA_ARGS__)

#define OBFH_GUI_ID_MapViewOfFile 217
#define OBFH_GUI_NAME_MapViewOfFile(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 217, (((unsigned int)'M' << 0) | ((unsigned int)'a' << 8) | ((unsigned int)'p' << 16) | ((unsigned int)'V' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 217, (((unsigned int)'i' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'w' << 16) | ((unsigned int)'O' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 217, (((unsigned int)'f' << 0) | ((unsigned int)'F' << 8) | ((unsigned int)'i' << 16) | ((unsigned int)'l' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 217, (((unsigned int)'e' << 0) | 0u | 0u | 0u)); \
    14u; \
})
#undef MapViewOfFile
#define MapViewOfFile(...) OBFH_API_CALL(2, MapViewOfFile, __VA_ARGS__)

#define OBFH_GUI_ID_UnmapViewOfFile 218
#define OBFH_GUI_NAME_UnmapViewOfFile(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 218, (((unsigned int)'U' << 0) | ((unsigned int)'n' << 8) | ((unsigned int)'m' << 16) | ((unsigned int)'a' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 218, (((unsigned int)'p' << 0) | ((unsigned int)'V' << 8) | ((unsigned int)'i' << 16) | ((unsigned int)'e' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 218, (((unsigned int)'w' << 0) | ((unsigned int)'O' << 8) | ((unsigned int)'f' << 16) | ((unsigned int)'F' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 218, (((unsigned int)'i' << 0) | ((unsigned int)'l' << 8) | ((unsigned int)'e' << 16) | 0u)); \
    16u; \
})
#undef UnmapViewOfFile
#define UnmapViewOfFile(...) OBFH_API_CALL(2, UnmapViewOfFile, __VA_ARGS__)

#define OBFH_GUI_ID_FlushViewOfFile 219
#define OBFH_GUI_NAME_FlushViewOfFile(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 219, (((unsigned int)'F' << 0) | ((unsigned int)'l' << 8) | ((unsigned int)'u' << 16) | ((unsigned int)'s' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 219, (((unsigned int)'h' << 0) | ((unsigned int)'V' << 8) | ((unsigned int)'i' << 16) | ((unsigned int)'e' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 219, (((unsigned int)'w' << 0) | ((unsigned int)'O' << 8) | ((unsigned int)'f' << 16) | ((unsigned int)'F' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 219, (((unsigned int)'i' << 0) | ((unsigned int)'l' << 8) | ((unsigned int)'e' << 16) | 0u)); \
    16u; \
})
#undef FlushViewOfFile
#define FlushViewOfFile(...) OBFH_API_CALL(2, FlushViewOfFile, __VA_ARGS__)

#define OBFH_GUI_ID_OpenProcess 220
#define OBFH_GUI_NAME_OpenProcess(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 220, (((unsigned int)'O' << 0) | ((unsigned int)'p' << 8) | ((unsigned int)'e' << 16) | ((unsigned int)'n' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 220, (((unsigned int)'P' << 0) | ((unsigned int)'r' << 8) | ((unsigned int)'o' << 16) | ((unsigned int)'c' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 220, (((unsigned int)'e' << 0) | ((unsigned int)'s' << 8) | ((unsigned int)'s' << 16) | 0u)); \
    12u; \
})
#undef OpenProcess
#define OpenProcess(...) OBFH_API_CALL(2, OpenProcess, __VA_ARGS__)

#define OBFH_GUI_ID_TerminateProcess 221
#define OBFH_GUI_NAME_TerminateProcess(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 221, (((unsigned int)'T' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'r' << 16) | ((unsigned int)'m' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 221, (((unsigned int)'i' << 0) | ((unsigned int)'n' << 8) | ((unsigned int)'a' << 16) | ((unsigned int)'t' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 221, (((unsigned int)'e' << 0) | ((unsigned int)'P' << 8) | ((unsigned int)'r' << 16) | ((unsigned int)'o' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 221, (((unsigned int)'c' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'s' << 16) | ((unsigned int)'s' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 16, 221, (0u | 0u | 0u | 0u)); \
    17u; \
})
#undef TerminateProcess
#define TerminateProcess(...) OBFH_API_CALL(2, TerminateProcess, __VA_ARGS__)

#define OBFH_GUI_ID_GetExitCodeProcess 222
#define OBFH_GUI_NAME_GetExitCodeProcess(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 222, (((unsigned int)'G' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'E' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 222, (((unsigned int)'x' << 0) | ((unsigned int)'i' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'C' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 222, (((unsigned int)'o' << 0) | ((unsigned int)'d' << 8) | ((unsigned int)'e' << 16) | ((unsigned int)'P' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 222, (((unsigned int)'r' << 0) | ((unsigned int)'o' << 8) | ((unsigned int)'c' << 16) | ((unsigned int)'e' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 16, 222, (((unsigned int)'s' << 0) | ((unsigned int)'s' << 8) | 0u | 0u)); \
    19u; \
})
#undef GetExitCodeProcess
#define GetExitCodeProcess(...) OBFH_API_CALL(2, GetExitCodeProcess, __VA_ARGS__)

#define OBFH_GUI_ID_GetProcessTimes 223
#define OBFH_GUI_NAME_GetProcessTimes(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 223, (((unsigned int)'G' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'P' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 223, (((unsigned int)'r' << 0) | ((unsigned int)'o' << 8) | ((unsigned int)'c' << 16) | ((unsigned int)'e' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 223, (((unsigned int)'s' << 0) | ((unsigned int)'s' << 8) | ((unsigned int)'T' << 16) | ((unsigned int)'i' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 223, (((unsigned int)'m' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'s' << 16) | 0u)); \
    16u; \
})
#undef GetProcessTimes
#define GetProcessTimes(...) OBFH_API_CALL(2, GetProcessTimes, __VA_ARGS__)

#define OBFH_GUI_ID_SleepEx 224
#define OBFH_GUI_NAME_SleepEx(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 224, (((unsigned int)'S' << 0) | ((unsigned int)'l' << 8) | ((unsigned int)'e' << 16) | ((unsigned int)'e' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 224, (((unsigned int)'p' << 0) | ((unsigned int)'E' << 8) | ((unsigned int)'x' << 16) | 0u)); \
    8u; \
})
#undef SleepEx
#define SleepEx(...) OBFH_API_CALL(2, SleepEx, __VA_ARGS__)

#define OBFH_GUI_ID_ReleaseMutex 225
#define OBFH_GUI_NAME_ReleaseMutex(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 225, (((unsigned int)'R' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'l' << 16) | ((unsigned int)'e' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 225, (((unsigned int)'a' << 0) | ((unsigned int)'s' << 8) | ((unsigned int)'e' << 16) | ((unsigned int)'M' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 225, (((unsigned int)'u' << 0) | ((unsigned int)'t' << 8) | ((unsigned int)'e' << 16) | ((unsigned int)'x' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 225, (0u | 0u | 0u | 0u)); \
    13u; \
})
#undef ReleaseMutex
#define ReleaseMutex(...) OBFH_API_CALL(2, ReleaseMutex, __VA_ARGS__)

#define OBFH_GUI_ID_ReleaseSemaphore 226
#define OBFH_GUI_NAME_ReleaseSemaphore(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 226, (((unsigned int)'R' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'l' << 16) | ((unsigned int)'e' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 226, (((unsigned int)'a' << 0) | ((unsigned int)'s' << 8) | ((unsigned int)'e' << 16) | ((unsigned int)'S' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 226, (((unsigned int)'e' << 0) | ((unsigned int)'m' << 8) | ((unsigned int)'a' << 16) | ((unsigned int)'p' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 226, (((unsigned int)'h' << 0) | ((unsigned int)'o' << 8) | ((unsigned int)'r' << 16) | ((unsigned int)'e' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 16, 226, (0u | 0u | 0u | 0u)); \
    17u; \
})
#undef ReleaseSemaphore
#define ReleaseSemaphore(...) OBFH_API_CALL(2, ReleaseSemaphore, __VA_ARGS__)

#define OBFH_GUI_ID_InitializeCriticalSection 227
#define OBFH_GUI_NAME_InitializeCriticalSection(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 227, (((unsigned int)'I' << 0) | ((unsigned int)'n' << 8) | ((unsigned int)'i' << 16) | ((unsigned int)'t' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 227, (((unsigned int)'i' << 0) | ((unsigned int)'a' << 8) | ((unsigned int)'l' << 16) | ((unsigned int)'i' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 227, (((unsigned int)'z' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'C' << 16) | ((unsigned int)'r' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 227, (((unsigned int)'i' << 0) | ((unsigned int)'t' << 8) | ((unsigned int)'i' << 16) | ((unsigned int)'c' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 16, 227, (((unsigned int)'a' << 0) | ((unsigned int)'l' << 8) | ((unsigned int)'S' << 16) | ((unsigned int)'e' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 20, 227, (((unsigned int)'c' << 0) | ((unsigned int)'t' << 8) | ((unsigned int)'i' << 16) | ((unsigned int)'o' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 24, 227, (((unsigned int)'n' << 0) | 0u | 0u | 0u)); \
    26u; \
})
#undef InitializeCriticalSection
#define InitializeCriticalSection(...) OBFH_API_CALL(2, InitializeCriticalSection, __VA_ARGS__)

#define OBFH_GUI_ID_DeleteCriticalSection 228
#define OBFH_GUI_NAME_DeleteCriticalSection(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 228, (((unsigned int)'D' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'l' << 16) | ((unsigned int)'e' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 228, (((unsigned int)'t' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'C' << 16) | ((unsigned int)'r' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 228, (((unsigned int)'i' << 0) | ((unsigned int)'t' << 8) | ((unsigned int)'i' << 16) | ((unsigned int)'c' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 228, (((unsigned int)'a' << 0) | ((unsigned int)'l' << 8) | ((unsigned int)'S' << 16) | ((unsigned int)'e' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 16, 228, (((unsigned int)'c' << 0) | ((unsigned int)'t' << 8) | ((unsigned int)'i' << 16) | ((unsigned int)'o' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 20, 228, (((unsigned int)'n' << 0) | 0u | 0u | 0u)); \
    22u; \
})
#undef DeleteCriticalSection
#define DeleteCriticalSection(...) OBFH_API_CALL(2, DeleteCriticalSection, __VA_ARGS__)

#define OBFH_GUI_ID_EnterCriticalSection 229
#define OBFH_GUI_NAME_EnterCriticalSection(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 229, (((unsigned int)'E' << 0) | ((unsigned int)'n' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'e' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 229, (((unsigned int)'r' << 0) | ((unsigned int)'C' << 8) | ((unsigned int)'r' << 16) | ((unsigned int)'i' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 229, (((unsigned int)'t' << 0) | ((unsigned int)'i' << 8) | ((unsigned int)'c' << 16) | ((unsigned int)'a' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 229, (((unsigned int)'l' << 0) | ((unsigned int)'S' << 8) | ((unsigned int)'e' << 16) | ((unsigned int)'c' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 16, 229, (((unsigned int)'t' << 0) | ((unsigned int)'i' << 8) | ((unsigned int)'o' << 16) | ((unsigned int)'n' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 20, 229, (0u | 0u | 0u | 0u)); \
    21u; \
})
#undef EnterCriticalSection
#define EnterCriticalSection(...) OBFH_API_CALL(2, EnterCriticalSection, __VA_ARGS__)

#define OBFH_GUI_ID_LeaveCriticalSection 230
#define OBFH_GUI_NAME_LeaveCriticalSection(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 230, (((unsigned int)'L' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'a' << 16) | ((unsigned int)'v' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 230, (((unsigned int)'e' << 0) | ((unsigned int)'C' << 8) | ((unsigned int)'r' << 16) | ((unsigned int)'i' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 230, (((unsigned int)'t' << 0) | ((unsigned int)'i' << 8) | ((unsigned int)'c' << 16) | ((unsigned int)'a' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 230, (((unsigned int)'l' << 0) | ((unsigned int)'S' << 8) | ((unsigned int)'e' << 16) | ((unsigned int)'c' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 16, 230, (((unsigned int)'t' << 0) | ((unsigned int)'i' << 8) | ((unsigned int)'o' << 16) | ((unsigned int)'n' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 20, 230, (0u | 0u | 0u | 0u)); \
    21u; \
})
#undef LeaveCriticalSection
#define LeaveCriticalSection(...) OBFH_API_CALL(2, LeaveCriticalSection, __VA_ARGS__)

#define OBFH_GUI_ID_TryEnterCriticalSection 231
#define OBFH_GUI_NAME_TryEnterCriticalSection(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 231, (((unsigned int)'T' << 0) | ((unsigned int)'r' << 8) | ((unsigned int)'y' << 16) | ((unsigned int)'E' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 231, (((unsigned int)'n' << 0) | ((unsigned int)'t' << 8) | ((unsigned int)'e' << 16) | ((unsigned int)'r' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 231, (((unsigned int)'C' << 0) | ((unsigned int)'r' << 8) | ((unsigned int)'i' << 16) | ((unsigned int)'t' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 231, (((unsigned int)'i' << 0) | ((unsigned int)'c' << 8) | ((unsigned int)'a' << 16) | ((unsigned int)'l' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 16, 231, (((unsigned int)'S' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'c' << 16) | ((unsigned int)'t' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 20, 231, (((unsigned int)'i' << 0) | ((unsigned int)'o' << 8) | ((unsigned int)'n' << 16) | 0u)); \
    24u; \
})
#undef TryEnterCriticalSection
#define TryEnterCriticalSection(...) OBFH_API_CALL(2, TryEnterCriticalSection, __VA_ARGS__)

#define OBFH_GUI_ID_AllocConsole 232
#define OBFH_GUI_NAME_AllocConsole(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 232, (((unsigned int)'A' << 0) | ((unsigned int)'l' << 8) | ((unsigned int)'l' << 16) | ((unsigned int)'o' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 232, (((unsigned int)'c' << 0) | ((unsigned int)'C' << 8) | ((unsigned int)'o' << 16) | ((unsigned int)'n' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 232, (((unsigned int)'s' << 0) | ((unsigned int)'o' << 8) | ((unsigned int)'l' << 16) | ((unsigned int)'e' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 232, (0u | 0u | 0u | 0u)); \
    13u; \
})
#undef AllocConsole
#define AllocConsole(...) OBFH_API_CALL(2, AllocConsole, __VA_ARGS__)

#define OBFH_GUI_ID_FreeConsole 233
#define OBFH_GUI_NAME_FreeConsole(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 233, (((unsigned int)'F' << 0) | ((unsigned int)'r' << 8) | ((unsigned int)'e' << 16) | ((unsigned int)'e' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 233, (((unsigned int)'C' << 0) | ((unsigned int)'o' << 8) | ((unsigned int)'n' << 16) | ((unsigned int)'s' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 233, (((unsigned int)'o' << 0) | ((unsigned int)'l' << 8) | ((unsigned int)'e' << 16) | 0u)); \
    12u; \
})
#undef FreeConsole
#define FreeConsole(...) OBFH_API_CALL(2, FreeConsole, __VA_ARGS__)

#define OBFH_GUI_ID_GetConsoleWindow 234
#define OBFH_GUI_NAME_GetConsoleWindow(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 234, (((unsigned int)'G' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'C' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 234, (((unsigned int)'o' << 0) | ((unsigned int)'n' << 8) | ((unsigned int)'s' << 16) | ((unsigned int)'o' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 234, (((unsigned int)'l' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'W' << 16) | ((unsigned int)'i' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 234, (((unsigned int)'n' << 0) | ((unsigned int)'d' << 8) | ((unsigned int)'o' << 16) | ((unsigned int)'w' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 16, 234, (0u | 0u | 0u | 0u)); \
    17u; \
})
#undef GetConsoleWindow
#define GetConsoleWindow(...) OBFH_API_CALL(2, GetConsoleWindow, __VA_ARGS__)

#define OBFH_GUI_ID_SetConsoleMode 235
#define OBFH_GUI_NAME_SetConsoleMode(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 235, (((unsigned int)'S' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'C' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 235, (((unsigned int)'o' << 0) | ((unsigned int)'n' << 8) | ((unsigned int)'s' << 16) | ((unsigned int)'o' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 235, (((unsigned int)'l' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'M' << 16) | ((unsigned int)'o' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 235, (((unsigned int)'d' << 0) | ((unsigned int)'e' << 8) | 0u | 0u)); \
    15u; \
})
#undef SetConsoleMode
#define SetConsoleMode(...) OBFH_API_CALL(2, SetConsoleMode, __VA_ARGS__)

#define OBFH_GUI_ID_GetConsoleCP 236
#define OBFH_GUI_NAME_GetConsoleCP(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 236, (((unsigned int)'G' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'C' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 236, (((unsigned int)'o' << 0) | ((unsigned int)'n' << 8) | ((unsigned int)'s' << 16) | ((unsigned int)'o' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 236, (((unsigned int)'l' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'C' << 16) | ((unsigned int)'P' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 236, (0u | 0u | 0u | 0u)); \
    13u; \
})
#undef GetConsoleCP
#define GetConsoleCP(...) OBFH_API_CALL(2, GetConsoleCP, __VA_ARGS__)

#define OBFH_GUI_ID_GetConsoleOutputCP 237
#define OBFH_GUI_NAME_GetConsoleOutputCP(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 237, (((unsigned int)'G' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'C' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 237, (((unsigned int)'o' << 0) | ((unsigned int)'n' << 8) | ((unsigned int)'s' << 16) | ((unsigned int)'o' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 237, (((unsigned int)'l' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'O' << 16) | ((unsigned int)'u' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 237, (((unsigned int)'t' << 0) | ((unsigned int)'p' << 8) | ((unsigned int)'u' << 16) | ((unsigned int)'t' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 16, 237, (((unsigned int)'C' << 0) | ((unsigned int)'P' << 8) | 0u | 0u)); \
    19u; \
})
#undef GetConsoleOutputCP
#define GetConsoleOutputCP(...) OBFH_API_CALL(2, GetConsoleOutputCP, __VA_ARGS__)

#define OBFH_GUI_ID_SetConsoleCP 238
#define OBFH_GUI_NAME_SetConsoleCP(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 238, (((unsigned int)'S' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'C' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 238, (((unsigned int)'o' << 0) | ((unsigned int)'n' << 8) | ((unsigned int)'s' << 16) | ((unsigned int)'o' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 238, (((unsigned int)'l' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'C' << 16) | ((unsigned int)'P' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 238, (0u | 0u | 0u | 0u)); \
    13u; \
})
#undef SetConsoleCP
#define SetConsoleCP(...) OBFH_API_CALL(2, SetConsoleCP, __VA_ARGS__)

#define OBFH_GUI_ID_SetConsoleOutputCP 239
#define OBFH_GUI_NAME_SetConsoleOutputCP(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 239, (((unsigned int)'S' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'C' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 239, (((unsigned int)'o' << 0) | ((unsigned int)'n' << 8) | ((unsigned int)'s' << 16) | ((unsigned int)'o' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 239, (((unsigned int)'l' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'O' << 16) | ((unsigned int)'u' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 239, (((unsigned int)'t' << 0) | ((unsigned int)'p' << 8) | ((unsigned int)'u' << 16) | ((unsigned int)'t' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 16, 239, (((unsigned int)'C' << 0) | ((unsigned int)'P' << 8) | 0u | 0u)); \
    19u; \
})
#undef SetConsoleOutputCP
#define SetConsoleOutputCP(...) OBFH_API_CALL(2, SetConsoleOutputCP, __VA_ARGS__)

#define OBFH_GUI_ID_GetSystemInfo 240
#define OBFH_GUI_NAME_GetSystemInfo(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 240, (((unsigned int)'G' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'S' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 240, (((unsigned int)'y' << 0) | ((unsigned int)'s' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'e' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 240, (((unsigned int)'m' << 0) | ((unsigned int)'I' << 8) | ((unsigned int)'n' << 16) | ((unsigned int)'f' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 240, (((unsigned int)'o' << 0) | 0u | 0u | 0u)); \
    14u; \
})
#undef GetSystemInfo
#define GetSystemInfo(...) OBFH_API_CALL(2, GetSystemInfo, __VA_ARGS__)

#define OBFH_GUI_ID_GetNativeSystemInfo 241
#define OBFH_GUI_NAME_GetNativeSystemInfo(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 241, (((unsigned int)'G' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'N' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 241, (((unsigned int)'a' << 0) | ((unsigned int)'t' << 8) | ((unsigned int)'i' << 16) | ((unsigned int)'v' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 241, (((unsigned int)'e' << 0) | ((unsigned int)'S' << 8) | ((unsigned int)'y' << 16) | ((unsigned int)'s' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 241, (((unsigned int)'t' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'m' << 16) | ((unsigned int)'I' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 16, 241, (((unsigned int)'n' << 0) | ((unsigned int)'f' << 8) | ((unsigned int)'o' << 16) | 0u)); \
    20u; \
})
#undef GetNativeSystemInfo
#define GetNativeSystemInfo(...) OBFH_API_CALL(2, GetNativeSystemInfo, __VA_ARGS__)

#define OBFH_GUI_ID_GetSystemTimeAsFileTime 242
#define OBFH_GUI_NAME_GetSystemTimeAsFileTime(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 242, (((unsigned int)'G' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'S' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 242, (((unsigned int)'y' << 0) | ((unsigned int)'s' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'e' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 242, (((unsigned int)'m' << 0) | ((unsigned int)'T' << 8) | ((unsigned int)'i' << 16) | ((unsigned int)'m' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 242, (((unsigned int)'e' << 0) | ((unsigned int)'A' << 8) | ((unsigned int)'s' << 16) | ((unsigned int)'F' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 16, 242, (((unsigned int)'i' << 0) | ((unsigned int)'l' << 8) | ((unsigned int)'e' << 16) | ((unsigned int)'T' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 20, 242, (((unsigned int)'i' << 0) | ((unsigned int)'m' << 8) | ((unsigned int)'e' << 16) | 0u)); \
    24u; \
})
#undef GetSystemTimeAsFileTime
#define GetSystemTimeAsFileTime(...) OBFH_API_CALL(2, GetSystemTimeAsFileTime, __VA_ARGS__)

#define OBFH_GUI_ID_CreateDialogParamA 243
#define OBFH_GUI_NAME_CreateDialogParamA(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 243, (((unsigned int)'C' << 0) | ((unsigned int)'r' << 8) | ((unsigned int)'e' << 16) | ((unsigned int)'a' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 243, (((unsigned int)'t' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'D' << 16) | ((unsigned int)'i' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 243, (((unsigned int)'a' << 0) | ((unsigned int)'l' << 8) | ((unsigned int)'o' << 16) | ((unsigned int)'g' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 243, (((unsigned int)'P' << 0) | ((unsigned int)'a' << 8) | ((unsigned int)'r' << 16) | ((unsigned int)'a' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 16, 243, (((unsigned int)'m' << 0) | ((unsigned int)'A' << 8) | 0u | 0u)); \
    19u; \
})
#undef CreateDialogParamA
#define CreateDialogParamA(...) OBFH_API_CALL(0, CreateDialogParamA, __VA_ARGS__)

#define OBFH_GUI_ID_CreateDialogParamW 244
#define OBFH_GUI_NAME_CreateDialogParamW(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 244, (((unsigned int)'C' << 0) | ((unsigned int)'r' << 8) | ((unsigned int)'e' << 16) | ((unsigned int)'a' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 244, (((unsigned int)'t' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'D' << 16) | ((unsigned int)'i' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 244, (((unsigned int)'a' << 0) | ((unsigned int)'l' << 8) | ((unsigned int)'o' << 16) | ((unsigned int)'g' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 244, (((unsigned int)'P' << 0) | ((unsigned int)'a' << 8) | ((unsigned int)'r' << 16) | ((unsigned int)'a' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 16, 244, (((unsigned int)'m' << 0) | ((unsigned int)'W' << 8) | 0u | 0u)); \
    19u; \
})
#undef CreateDialogParamW
#define CreateDialogParamW(...) OBFH_API_CALL(0, CreateDialogParamW, __VA_ARGS__)

#define OBFH_GUI_ID_DialogBoxParamA 245
#define OBFH_GUI_NAME_DialogBoxParamA(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 245, (((unsigned int)'D' << 0) | ((unsigned int)'i' << 8) | ((unsigned int)'a' << 16) | ((unsigned int)'l' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 245, (((unsigned int)'o' << 0) | ((unsigned int)'g' << 8) | ((unsigned int)'B' << 16) | ((unsigned int)'o' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 245, (((unsigned int)'x' << 0) | ((unsigned int)'P' << 8) | ((unsigned int)'a' << 16) | ((unsigned int)'r' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 245, (((unsigned int)'a' << 0) | ((unsigned int)'m' << 8) | ((unsigned int)'A' << 16) | 0u)); \
    16u; \
})
#undef DialogBoxParamA
#define DialogBoxParamA(...) OBFH_API_CALL(0, DialogBoxParamA, __VA_ARGS__)

#define OBFH_GUI_ID_DialogBoxParamW 246
#define OBFH_GUI_NAME_DialogBoxParamW(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 246, (((unsigned int)'D' << 0) | ((unsigned int)'i' << 8) | ((unsigned int)'a' << 16) | ((unsigned int)'l' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 246, (((unsigned int)'o' << 0) | ((unsigned int)'g' << 8) | ((unsigned int)'B' << 16) | ((unsigned int)'o' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 246, (((unsigned int)'x' << 0) | ((unsigned int)'P' << 8) | ((unsigned int)'a' << 16) | ((unsigned int)'r' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 246, (((unsigned int)'a' << 0) | ((unsigned int)'m' << 8) | ((unsigned int)'W' << 16) | 0u)); \
    16u; \
})
#undef DialogBoxParamW
#define DialogBoxParamW(...) OBFH_API_CALL(0, DialogBoxParamW, __VA_ARGS__)

#define OBFH_GUI_ID_GetDlgItemTextA 247
#define OBFH_GUI_NAME_GetDlgItemTextA(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 247, (((unsigned int)'G' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'D' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 247, (((unsigned int)'l' << 0) | ((unsigned int)'g' << 8) | ((unsigned int)'I' << 16) | ((unsigned int)'t' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 247, (((unsigned int)'e' << 0) | ((unsigned int)'m' << 8) | ((unsigned int)'T' << 16) | ((unsigned int)'e' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 247, (((unsigned int)'x' << 0) | ((unsigned int)'t' << 8) | ((unsigned int)'A' << 16) | 0u)); \
    16u; \
})
#undef GetDlgItemTextA
#define GetDlgItemTextA(...) OBFH_API_CALL(0, GetDlgItemTextA, __VA_ARGS__)

#define OBFH_GUI_ID_GetDlgItemTextW 248
#define OBFH_GUI_NAME_GetDlgItemTextW(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 248, (((unsigned int)'G' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'D' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 248, (((unsigned int)'l' << 0) | ((unsigned int)'g' << 8) | ((unsigned int)'I' << 16) | ((unsigned int)'t' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 248, (((unsigned int)'e' << 0) | ((unsigned int)'m' << 8) | ((unsigned int)'T' << 16) | ((unsigned int)'e' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 248, (((unsigned int)'x' << 0) | ((unsigned int)'t' << 8) | ((unsigned int)'W' << 16) | 0u)); \
    16u; \
})
#undef GetDlgItemTextW
#define GetDlgItemTextW(...) OBFH_API_CALL(0, GetDlgItemTextW, __VA_ARGS__)

#define OBFH_GUI_ID_SetDlgItemTextA 249
#define OBFH_GUI_NAME_SetDlgItemTextA(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 249, (((unsigned int)'S' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'D' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 249, (((unsigned int)'l' << 0) | ((unsigned int)'g' << 8) | ((unsigned int)'I' << 16) | ((unsigned int)'t' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 249, (((unsigned int)'e' << 0) | ((unsigned int)'m' << 8) | ((unsigned int)'T' << 16) | ((unsigned int)'e' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 249, (((unsigned int)'x' << 0) | ((unsigned int)'t' << 8) | ((unsigned int)'A' << 16) | 0u)); \
    16u; \
})
#undef SetDlgItemTextA
#define SetDlgItemTextA(...) OBFH_API_CALL(0, SetDlgItemTextA, __VA_ARGS__)

#define OBFH_GUI_ID_SetDlgItemTextW 250
#define OBFH_GUI_NAME_SetDlgItemTextW(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 250, (((unsigned int)'S' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'D' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 250, (((unsigned int)'l' << 0) | ((unsigned int)'g' << 8) | ((unsigned int)'I' << 16) | ((unsigned int)'t' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 250, (((unsigned int)'e' << 0) | ((unsigned int)'m' << 8) | ((unsigned int)'T' << 16) | ((unsigned int)'e' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 250, (((unsigned int)'x' << 0) | ((unsigned int)'t' << 8) | ((unsigned int)'W' << 16) | 0u)); \
    16u; \
})
#undef SetDlgItemTextW
#define SetDlgItemTextW(...) OBFH_API_CALL(0, SetDlgItemTextW, __VA_ARGS__)

#define OBFH_GUI_ID_PeekMessageA 251
#define OBFH_GUI_NAME_PeekMessageA(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 251, (((unsigned int)'P' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'e' << 16) | ((unsigned int)'k' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 251, (((unsigned int)'M' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'s' << 16) | ((unsigned int)'s' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 251, (((unsigned int)'a' << 0) | ((unsigned int)'g' << 8) | ((unsigned int)'e' << 16) | ((unsigned int)'A' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 251, (0u | 0u | 0u | 0u)); \
    13u; \
})
#undef PeekMessageA
#define PeekMessageA(...) OBFH_API_CALL(0, PeekMessageA, __VA_ARGS__)

#define OBFH_GUI_ID_PeekMessageW 252
#define OBFH_GUI_NAME_PeekMessageW(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 252, (((unsigned int)'P' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'e' << 16) | ((unsigned int)'k' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 252, (((unsigned int)'M' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'s' << 16) | ((unsigned int)'s' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 252, (((unsigned int)'a' << 0) | ((unsigned int)'g' << 8) | ((unsigned int)'e' << 16) | ((unsigned int)'W' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 252, (0u | 0u | 0u | 0u)); \
    13u; \
})
#undef PeekMessageW
#define PeekMessageW(...) OBFH_API_CALL(0, PeekMessageW, __VA_ARGS__)

#define OBFH_GUI_ID_EndDialog 253
#define OBFH_GUI_NAME_EndDialog(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 253, (((unsigned int)'E' << 0) | ((unsigned int)'n' << 8) | ((unsigned int)'d' << 16) | ((unsigned int)'D' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 253, (((unsigned int)'i' << 0) | ((unsigned int)'a' << 8) | ((unsigned int)'l' << 16) | ((unsigned int)'o' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 253, (((unsigned int)'g' << 0) | 0u | 0u | 0u)); \
    10u; \
})
#undef EndDialog
#define EndDialog(...) OBFH_API_CALL(0, EndDialog, __VA_ARGS__)

#define OBFH_GUI_ID_CheckDlgButton 254
#define OBFH_GUI_NAME_CheckDlgButton(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 254, (((unsigned int)'C' << 0) | ((unsigned int)'h' << 8) | ((unsigned int)'e' << 16) | ((unsigned int)'c' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 254, (((unsigned int)'k' << 0) | ((unsigned int)'D' << 8) | ((unsigned int)'l' << 16) | ((unsigned int)'g' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 254, (((unsigned int)'B' << 0) | ((unsigned int)'u' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'t' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 254, (((unsigned int)'o' << 0) | ((unsigned int)'n' << 8) | 0u | 0u)); \
    15u; \
})
#undef CheckDlgButton
#define CheckDlgButton(...) OBFH_API_CALL(0, CheckDlgButton, __VA_ARGS__)

#define OBFH_GUI_ID_IsDlgButtonChecked 255
#define OBFH_GUI_NAME_IsDlgButtonChecked(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 255, (((unsigned int)'I' << 0) | ((unsigned int)'s' << 8) | ((unsigned int)'D' << 16) | ((unsigned int)'l' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 255, (((unsigned int)'g' << 0) | ((unsigned int)'B' << 8) | ((unsigned int)'u' << 16) | ((unsigned int)'t' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 255, (((unsigned int)'t' << 0) | ((unsigned int)'o' << 8) | ((unsigned int)'n' << 16) | ((unsigned int)'C' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 255, (((unsigned int)'h' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'c' << 16) | ((unsigned int)'k' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 16, 255, (((unsigned int)'e' << 0) | ((unsigned int)'d' << 8) | 0u | 0u)); \
    19u; \
})
#undef IsDlgButtonChecked
#define IsDlgButtonChecked(...) OBFH_API_CALL(0, IsDlgButtonChecked, __VA_ARGS__)

#define OBFH_GUI_ID_SetMenu 256
#define OBFH_GUI_NAME_SetMenu(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 256, (((unsigned int)'S' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'M' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 256, (((unsigned int)'e' << 0) | ((unsigned int)'n' << 8) | ((unsigned int)'u' << 16) | 0u)); \
    8u; \
})
#undef SetMenu
#define SetMenu(...) OBFH_API_CALL(0, SetMenu, __VA_ARGS__)

#define OBFH_GUI_ID_DestroyMenu 257
#define OBFH_GUI_NAME_DestroyMenu(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 257, (((unsigned int)'D' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'s' << 16) | ((unsigned int)'t' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 257, (((unsigned int)'r' << 0) | ((unsigned int)'o' << 8) | ((unsigned int)'y' << 16) | ((unsigned int)'M' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 257, (((unsigned int)'e' << 0) | ((unsigned int)'n' << 8) | ((unsigned int)'u' << 16) | 0u)); \
    12u; \
})
#undef DestroyMenu
#define DestroyMenu(...) OBFH_API_CALL(0, DestroyMenu, __VA_ARGS__)

#define OBFH_GUI_ID_InvalidateRect 258
#define OBFH_GUI_NAME_InvalidateRect(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 258, (((unsigned int)'I' << 0) | ((unsigned int)'n' << 8) | ((unsigned int)'v' << 16) | ((unsigned int)'a' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 258, (((unsigned int)'l' << 0) | ((unsigned int)'i' << 8) | ((unsigned int)'d' << 16) | ((unsigned int)'a' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 258, (((unsigned int)'t' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'R' << 16) | ((unsigned int)'e' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 258, (((unsigned int)'c' << 0) | ((unsigned int)'t' << 8) | 0u | 0u)); \
    15u; \
})
#undef InvalidateRect
#define InvalidateRect(...) OBFH_API_CALL(0, InvalidateRect, __VA_ARGS__)

#define OBFH_GUI_ID_GetAsyncKeyState 259
#define OBFH_GUI_NAME_GetAsyncKeyState(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 259, (((unsigned int)'G' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'A' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 259, (((unsigned int)'s' << 0) | ((unsigned int)'y' << 8) | ((unsigned int)'n' << 16) | ((unsigned int)'c' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 259, (((unsigned int)'K' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'y' << 16) | ((unsigned int)'S' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 259, (((unsigned int)'t' << 0) | ((unsigned int)'a' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'e' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 16, 259, (0u | 0u | 0u | 0u)); \
    17u; \
})
#undef GetAsyncKeyState
#define GetAsyncKeyState(...) OBFH_API_CALL(0, GetAsyncKeyState, __VA_ARGS__)

#define OBFH_GUI_ID_ScreenToClient 260
#define OBFH_GUI_NAME_ScreenToClient(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 260, (((unsigned int)'S' << 0) | ((unsigned int)'c' << 8) | ((unsigned int)'r' << 16) | ((unsigned int)'e' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 260, (((unsigned int)'e' << 0) | ((unsigned int)'n' << 8) | ((unsigned int)'T' << 16) | ((unsigned int)'o' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 260, (((unsigned int)'C' << 0) | ((unsigned int)'l' << 8) | ((unsigned int)'i' << 16) | ((unsigned int)'e' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 260, (((unsigned int)'n' << 0) | ((unsigned int)'t' << 8) | 0u | 0u)); \
    15u; \
})
#undef ScreenToClient
#define ScreenToClient(...) OBFH_API_CALL(0, ScreenToClient, __VA_ARGS__)

#define OBFH_GUI_ID_ClientToScreen 261
#define OBFH_GUI_NAME_ClientToScreen(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 261, (((unsigned int)'C' << 0) | ((unsigned int)'l' << 8) | ((unsigned int)'i' << 16) | ((unsigned int)'e' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 261, (((unsigned int)'n' << 0) | ((unsigned int)'t' << 8) | ((unsigned int)'T' << 16) | ((unsigned int)'o' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 261, (((unsigned int)'S' << 0) | ((unsigned int)'c' << 8) | ((unsigned int)'r' << 16) | ((unsigned int)'e' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 261, (((unsigned int)'e' << 0) | ((unsigned int)'n' << 8) | 0u | 0u)); \
    15u; \
})
#undef ClientToScreen
#define ClientToScreen(...) OBFH_API_CALL(0, ClientToScreen, __VA_ARGS__)

#define OBFH_GUI_ID_GetCursorPos 262
#define OBFH_GUI_NAME_GetCursorPos(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 262, (((unsigned int)'G' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'C' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 262, (((unsigned int)'u' << 0) | ((unsigned int)'r' << 8) | ((unsigned int)'s' << 16) | ((unsigned int)'o' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 262, (((unsigned int)'r' << 0) | ((unsigned int)'P' << 8) | ((unsigned int)'o' << 16) | ((unsigned int)'s' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 262, (0u | 0u | 0u | 0u)); \
    13u; \
})
#undef GetCursorPos
#define GetCursorPos(...) OBFH_API_CALL(0, GetCursorPos, __VA_ARGS__)

#define OBFH_GUI_ID_GetWindow 263
#define OBFH_GUI_NAME_GetWindow(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 263, (((unsigned int)'G' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'W' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 263, (((unsigned int)'i' << 0) | ((unsigned int)'n' << 8) | ((unsigned int)'d' << 16) | ((unsigned int)'o' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 263, (((unsigned int)'w' << 0) | 0u | 0u | 0u)); \
    10u; \
})
#undef GetWindow
#define GetWindow(...) OBFH_API_CALL(0, GetWindow, __VA_ARGS__)

#define OBFH_GUI_ID_GetAncestor 264
#define OBFH_GUI_NAME_GetAncestor(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 264, (((unsigned int)'G' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'A' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 264, (((unsigned int)'n' << 0) | ((unsigned int)'c' << 8) | ((unsigned int)'e' << 16) | ((unsigned int)'s' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 264, (((unsigned int)'t' << 0) | ((unsigned int)'o' << 8) | ((unsigned int)'r' << 16) | 0u)); \
    12u; \
})
#undef GetAncestor
#define GetAncestor(...) OBFH_API_CALL(0, GetAncestor, __VA_ARGS__)

#define OBFH_GUI_ID_GetTextExtentPoint32A 265
#define OBFH_GUI_NAME_GetTextExtentPoint32A(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 265, (((unsigned int)'G' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'T' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 265, (((unsigned int)'e' << 0) | ((unsigned int)'x' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'E' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 265, (((unsigned int)'x' << 0) | ((unsigned int)'t' << 8) | ((unsigned int)'e' << 16) | ((unsigned int)'n' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 265, (((unsigned int)'t' << 0) | ((unsigned int)'P' << 8) | ((unsigned int)'o' << 16) | ((unsigned int)'i' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 16, 265, (((unsigned int)'n' << 0) | ((unsigned int)'t' << 8) | ((unsigned int)'3' << 16) | ((unsigned int)'2' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 20, 265, (((unsigned int)'A' << 0) | 0u | 0u | 0u)); \
    22u; \
})
#undef GetTextExtentPoint32A
#define GetTextExtentPoint32A(...) OBFH_API_CALL(1, GetTextExtentPoint32A, __VA_ARGS__)

#define OBFH_GUI_ID_GetTextExtentPoint32W 266
#define OBFH_GUI_NAME_GetTextExtentPoint32W(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 266, (((unsigned int)'G' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'T' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 266, (((unsigned int)'e' << 0) | ((unsigned int)'x' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'E' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 266, (((unsigned int)'x' << 0) | ((unsigned int)'t' << 8) | ((unsigned int)'e' << 16) | ((unsigned int)'n' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 266, (((unsigned int)'t' << 0) | ((unsigned int)'P' << 8) | ((unsigned int)'o' << 16) | ((unsigned int)'i' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 16, 266, (((unsigned int)'n' << 0) | ((unsigned int)'t' << 8) | ((unsigned int)'3' << 16) | ((unsigned int)'2' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 20, 266, (((unsigned int)'W' << 0) | 0u | 0u | 0u)); \
    22u; \
})
#undef GetTextExtentPoint32W
#define GetTextExtentPoint32W(...) OBFH_API_CALL(1, GetTextExtentPoint32W, __VA_ARGS__)

#define OBFH_GUI_ID_SetBkMode 267
#define OBFH_GUI_NAME_SetBkMode(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 267, (((unsigned int)'S' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'B' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 267, (((unsigned int)'k' << 0) | ((unsigned int)'M' << 8) | ((unsigned int)'o' << 16) | ((unsigned int)'d' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 267, (((unsigned int)'e' << 0) | 0u | 0u | 0u)); \
    10u; \
})
#undef SetBkMode
#define SetBkMode(...) OBFH_API_CALL(1, SetBkMode, __VA_ARGS__)

#define OBFH_GUI_ID_HeapDestroy 268
#define OBFH_GUI_NAME_HeapDestroy(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 268, (((unsigned int)'H' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'a' << 16) | ((unsigned int)'p' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 268, (((unsigned int)'D' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'s' << 16) | ((unsigned int)'t' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 268, (((unsigned int)'r' << 0) | ((unsigned int)'o' << 8) | ((unsigned int)'y' << 16) | 0u)); \
    12u; \
})
#undef HeapDestroy
#define HeapDestroy(...) OBFH_API_CALL(2, HeapDestroy, __VA_ARGS__)

#define OBFH_GUI_ID_GetFileSize 269
#define OBFH_GUI_NAME_GetFileSize(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 269, (((unsigned int)'G' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'F' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 269, (((unsigned int)'i' << 0) | ((unsigned int)'l' << 8) | ((unsigned int)'e' << 16) | ((unsigned int)'S' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 269, (((unsigned int)'i' << 0) | ((unsigned int)'z' << 8) | ((unsigned int)'e' << 16) | 0u)); \
    12u; \
})
#undef GetFileSize
#define GetFileSize(...) OBFH_API_CALL(2, GetFileSize, __VA_ARGS__)

#define OBFH_GUI_ID_SetFilePointer 270
#define OBFH_GUI_NAME_SetFilePointer(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 270, (((unsigned int)'S' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'F' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 270, (((unsigned int)'i' << 0) | ((unsigned int)'l' << 8) | ((unsigned int)'e' << 16) | ((unsigned int)'P' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 270, (((unsigned int)'o' << 0) | ((unsigned int)'i' << 8) | ((unsigned int)'n' << 16) | ((unsigned int)'t' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 270, (((unsigned int)'e' << 0) | ((unsigned int)'r' << 8) | 0u | 0u)); \
    15u; \
})
#undef SetFilePointer
#define SetFilePointer(...) OBFH_API_CALL(2, SetFilePointer, __VA_ARGS__)

#define OBFH_GUI_ID_GetFileAttributesExA 271
#define OBFH_GUI_NAME_GetFileAttributesExA(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 271, (((unsigned int)'G' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'F' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 271, (((unsigned int)'i' << 0) | ((unsigned int)'l' << 8) | ((unsigned int)'e' << 16) | ((unsigned int)'A' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 271, (((unsigned int)'t' << 0) | ((unsigned int)'t' << 8) | ((unsigned int)'r' << 16) | ((unsigned int)'i' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 271, (((unsigned int)'b' << 0) | ((unsigned int)'u' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'e' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 16, 271, (((unsigned int)'s' << 0) | ((unsigned int)'E' << 8) | ((unsigned int)'x' << 16) | ((unsigned int)'A' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 20, 271, (0u | 0u | 0u | 0u)); \
    21u; \
})
#undef GetFileAttributesExA
#define GetFileAttributesExA(...) OBFH_API_CALL(2, GetFileAttributesExA, __VA_ARGS__)

#define OBFH_GUI_ID_GetFileAttributesExW 272
#define OBFH_GUI_NAME_GetFileAttributesExW(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 272, (((unsigned int)'G' << 0) | ((unsigned int)'e' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'F' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 272, (((unsigned int)'i' << 0) | ((unsigned int)'l' << 8) | ((unsigned int)'e' << 16) | ((unsigned int)'A' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 272, (((unsigned int)'t' << 0) | ((unsigned int)'t' << 8) | ((unsigned int)'r' << 16) | ((unsigned int)'i' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 272, (((unsigned int)'b' << 0) | ((unsigned int)'u' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'e' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 16, 272, (((unsigned int)'s' << 0) | ((unsigned int)'E' << 8) | ((unsigned int)'x' << 16) | ((unsigned int)'W' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 20, 272, (0u | 0u | 0u | 0u)); \
    21u; \
})
#undef GetFileAttributesExW
#define GetFileAttributesExW(...) OBFH_API_CALL(2, GetFileAttributesExW, __VA_ARGS__)

#define OBFH_GUI_ID_FindFirstFileExA 273
#define OBFH_GUI_NAME_FindFirstFileExA(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 273, (((unsigned int)'F' << 0) | ((unsigned int)'i' << 8) | ((unsigned int)'n' << 16) | ((unsigned int)'d' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 273, (((unsigned int)'F' << 0) | ((unsigned int)'i' << 8) | ((unsigned int)'r' << 16) | ((unsigned int)'s' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 273, (((unsigned int)'t' << 0) | ((unsigned int)'F' << 8) | ((unsigned int)'i' << 16) | ((unsigned int)'l' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 273, (((unsigned int)'e' << 0) | ((unsigned int)'E' << 8) | ((unsigned int)'x' << 16) | ((unsigned int)'A' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 16, 273, (0u | 0u | 0u | 0u)); \
    17u; \
})
#undef FindFirstFileExA
#define FindFirstFileExA(...) OBFH_API_CALL(2, FindFirstFileExA, __VA_ARGS__)

#define OBFH_GUI_ID_FindFirstFileExW 274
#define OBFH_GUI_NAME_FindFirstFileExW(buffer) ({ \
    OBFH_GUI_NAME_WORD(buffer, 0, 274, (((unsigned int)'F' << 0) | ((unsigned int)'i' << 8) | ((unsigned int)'n' << 16) | ((unsigned int)'d' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 4, 274, (((unsigned int)'F' << 0) | ((unsigned int)'i' << 8) | ((unsigned int)'r' << 16) | ((unsigned int)'s' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 8, 274, (((unsigned int)'t' << 0) | ((unsigned int)'F' << 8) | ((unsigned int)'i' << 16) | ((unsigned int)'l' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 12, 274, (((unsigned int)'e' << 0) | ((unsigned int)'E' << 8) | ((unsigned int)'x' << 16) | ((unsigned int)'W' << 24))); \
    OBFH_GUI_NAME_WORD(buffer, 16, 274, (0u | 0u | 0u | 0u)); \
    17u; \
})
#undef FindFirstFileExW
#define FindFirstFileExW(...) OBFH_API_CALL(2, FindFirstFileExW, __VA_ARGS__)

// #endregion

// #region 22. Math aliases and typed operand transport
// ============================================================================
// 22. Math aliases and typed operand transport
// ============================================================================

static inline int obfh_abs_proxy(int value) {
    BREAK_STACK_CFLOW;
    PHANTOM_NOP;
    return value < (int)FALSE ? -value : value;
}
#define abs(x) obfh_abs_proxy(x)

#define OBFH_MATH_KEY ((ULONG_PTR)obfh_condition_proxy((float)_1, (float)obfh_int_proxy(SALT_SHIFT)))
// Mutate the typed value's address: arithmetic identities can change -0,
// rounding, NaN payloads or wide integer exponents before the math call.
#define OBFH_MATH_ADDRESS_ENCODE(address, key) \
    __builtin_choose_expr(__obfh_math_form == 0, (address) ^ (key), \
                          __builtin_choose_expr(__obfh_math_form == 1, (address) + (key), (address) - (key)))
#define OBFH_MATH_ADDRESS_DECODE(address, key) \
    __builtin_choose_expr(__obfh_math_form == 0, (address) ^ (key), \
                          __builtin_choose_expr(__obfh_math_form == 1, (address) - (key), (address) + (key)))
#define _MUTATE_MATH(value) ({ \
    enum { OBFH_DATA_FIELDS(__obfh_math_draw, __COUNTER__), \
           __obfh_math_form = __obfh_math_draw % 3u }; \
    BREAK_STACK_CFLOW; \
    __typeof__((value)) volatile __obfh_math_value = (value); \
    volatile ULONG_PTR __obfh_math_key = OBFH_MATH_KEY; \
    ULONG_PTR __obfh_math_address = obfh_uintptr_proxy(OBFH_MATH_ADDRESS_ENCODE((ULONG_PTR)&__obfh_math_value, __obfh_math_key)); \
    *(__typeof__(&__obfh_math_value))OBFH_MATH_ADDRESS_DECODE(__obfh_math_address, __obfh_math_key); \
})

// TCC math adapters without a guaranteed matching msvcrt export retain native calls.
// This preserves signatures and special-value behavior; operands still use typed transport.
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

// Known msvcrt math exports use the same name selector and typed target as other CRT calls.
// Keep the two store orders, typed signature and arguments explicit at each alias.
// This shares source scaffolding only: each call still constructs its own name.
#define OBFH_MATH_CRT_CALL(capacity, groups, forward, reverse, type, args) ({ \
    char __obfh_math_name[capacity]; \
    OBFH_NAME_ORDER_FOR(groups, forward, reverse); \
    OBFH_CRT_TARGET(type, __obfh_math_name) \
    args; \
})

#define acos(x) OBFH_MATH_CRT_CALL(5, OBFH_ALPHA_LOW | OBFH_ALPHA_HIGH, \
                                   (OBFH_NAME_PAIR(1, __obfh_math_name[OBFH_NAME_INDEX(0)] = _a, __obfh_math_name[OBFH_NAME_INDEX(1)] = _c), OBFH_NAME_PAIR(2, __obfh_math_name[OBFH_NAME_INDEX(2)] = _o, __obfh_math_name[OBFH_NAME_INDEX(3)] = _s), __obfh_math_name[OBFH_NAME_INDEX(4)] = _0), \
                                   (OBFH_NAME_PAIR(1, __obfh_math_name[OBFH_NAME_INDEX(4)] = _0, __obfh_math_name[OBFH_NAME_INDEX(3)] = _s), OBFH_NAME_PAIR(2, __obfh_math_name[OBFH_NAME_INDEX(2)] = _o, __obfh_math_name[OBFH_NAME_INDEX(1)] = _c), __obfh_math_name[OBFH_NAME_INDEX(0)] = _a), \
                                   double (*)(double), (x))

#define asin(x) OBFH_MATH_CRT_CALL(5, OBFH_ALPHA_LOW | OBFH_ALPHA_HIGH, \
                                   (OBFH_NAME_PAIR(1, __obfh_math_name[OBFH_NAME_INDEX(0)] = _a, __obfh_math_name[OBFH_NAME_INDEX(1)] = _s), OBFH_NAME_PAIR(2, __obfh_math_name[OBFH_NAME_INDEX(2)] = _i, __obfh_math_name[OBFH_NAME_INDEX(3)] = _n), __obfh_math_name[OBFH_NAME_INDEX(4)] = _0), \
                                   (OBFH_NAME_PAIR(1, __obfh_math_name[OBFH_NAME_INDEX(4)] = _0, __obfh_math_name[OBFH_NAME_INDEX(3)] = _n), OBFH_NAME_PAIR(2, __obfh_math_name[OBFH_NAME_INDEX(2)] = _i, __obfh_math_name[OBFH_NAME_INDEX(1)] = _s), __obfh_math_name[OBFH_NAME_INDEX(0)] = _a), \
                                   double (*)(double), (x))

#define atan(x) OBFH_MATH_CRT_CALL(5, OBFH_ALPHA_LOW | OBFH_ALPHA_HIGH, \
                                   (OBFH_NAME_PAIR(1, __obfh_math_name[OBFH_NAME_INDEX(0)] = _a, __obfh_math_name[OBFH_NAME_INDEX(1)] = _t), OBFH_NAME_PAIR(2, __obfh_math_name[OBFH_NAME_INDEX(2)] = _a, __obfh_math_name[OBFH_NAME_INDEX(3)] = _n), __obfh_math_name[OBFH_NAME_INDEX(4)] = _0), \
                                   (OBFH_NAME_PAIR(1, __obfh_math_name[OBFH_NAME_INDEX(4)] = _0, __obfh_math_name[OBFH_NAME_INDEX(3)] = _n), OBFH_NAME_PAIR(2, __obfh_math_name[OBFH_NAME_INDEX(2)] = _a, __obfh_math_name[OBFH_NAME_INDEX(1)] = _t), __obfh_math_name[OBFH_NAME_INDEX(0)] = _a), \
                                   double (*)(double), (x))

#define atan2(y, x) OBFH_MATH_CRT_CALL(6, OBFH_ALPHA_LOW | OBFH_ALPHA_HIGH, \
                                       (__obfh_math_name[OBFH_NAME_INDEX(0)] = _a, __obfh_math_name[OBFH_NAME_INDEX(1)] = _t, __obfh_math_name[OBFH_NAME_INDEX(2)] = _a, __obfh_math_name[OBFH_NAME_INDEX(3)] = _n, __obfh_math_name[4] = (_2 + '0'), __obfh_math_name[OBFH_NAME_INDEX(5)] = _0), \
                                       (__obfh_math_name[OBFH_NAME_INDEX(5)] = _0, __obfh_math_name[4] = (_2 + '0'), __obfh_math_name[OBFH_NAME_INDEX(3)] = _n, __obfh_math_name[OBFH_NAME_INDEX(2)] = _a, __obfh_math_name[OBFH_NAME_INDEX(1)] = _t, __obfh_math_name[OBFH_NAME_INDEX(0)] = _a), \
                                       double (*)(double, double), (y, x))

#define ceil(x) OBFH_MATH_CRT_CALL(5, OBFH_ALPHA_LOW, \
                                   (OBFH_NAME_PAIR(1, __obfh_math_name[OBFH_NAME_INDEX(0)] = _c, __obfh_math_name[OBFH_NAME_INDEX(1)] = _e), OBFH_NAME_PAIR(2, __obfh_math_name[OBFH_NAME_INDEX(2)] = _i, __obfh_math_name[OBFH_NAME_INDEX(3)] = _l), __obfh_math_name[OBFH_NAME_INDEX(4)] = _0), \
                                   (OBFH_NAME_PAIR(1, __obfh_math_name[OBFH_NAME_INDEX(4)] = _0, __obfh_math_name[OBFH_NAME_INDEX(3)] = _l), OBFH_NAME_PAIR(2, __obfh_math_name[OBFH_NAME_INDEX(2)] = _i, __obfh_math_name[OBFH_NAME_INDEX(1)] = _e), __obfh_math_name[OBFH_NAME_INDEX(0)] = _c), \
                                   double (*)(double), (x))

#define cos(x) OBFH_MATH_CRT_CALL(4, OBFH_ALPHA_LOW | OBFH_ALPHA_HIGH, \
                                  (OBFH_NAME_PAIR(1, __obfh_math_name[OBFH_NAME_INDEX(0)] = _c, __obfh_math_name[OBFH_NAME_INDEX(1)] = _o), OBFH_NAME_PAIR(2, __obfh_math_name[OBFH_NAME_INDEX(2)] = _s, __obfh_math_name[OBFH_NAME_INDEX(3)] = _0)), \
                                  (OBFH_NAME_PAIR(1, __obfh_math_name[OBFH_NAME_INDEX(3)] = _0, __obfh_math_name[OBFH_NAME_INDEX(2)] = _s), OBFH_NAME_PAIR(2, __obfh_math_name[OBFH_NAME_INDEX(1)] = _o, __obfh_math_name[OBFH_NAME_INDEX(0)] = _c)), \
                                  double (*)(double), (x))

#define cosh(x) OBFH_MATH_CRT_CALL(5, OBFH_ALPHA_LOW | OBFH_ALPHA_HIGH, \
                                   (OBFH_NAME_PAIR(1, __obfh_math_name[OBFH_NAME_INDEX(0)] = _c, __obfh_math_name[OBFH_NAME_INDEX(1)] = _o), OBFH_NAME_PAIR(2, __obfh_math_name[OBFH_NAME_INDEX(2)] = _s, __obfh_math_name[OBFH_NAME_INDEX(3)] = _h), __obfh_math_name[OBFH_NAME_INDEX(4)] = _0), \
                                   (OBFH_NAME_PAIR(1, __obfh_math_name[OBFH_NAME_INDEX(4)] = _0, __obfh_math_name[OBFH_NAME_INDEX(3)] = _h), OBFH_NAME_PAIR(2, __obfh_math_name[OBFH_NAME_INDEX(2)] = _s, __obfh_math_name[OBFH_NAME_INDEX(1)] = _o), __obfh_math_name[OBFH_NAME_INDEX(0)] = _c), \
                                   double (*)(double), (x))

#define exp(x) OBFH_MATH_CRT_CALL(4, OBFH_ALPHA_LOW | OBFH_ALPHA_HIGH, \
                                  (OBFH_NAME_PAIR(1, __obfh_math_name[OBFH_NAME_INDEX(0)] = _e, __obfh_math_name[OBFH_NAME_INDEX(1)] = _x), OBFH_NAME_PAIR(2, __obfh_math_name[OBFH_NAME_INDEX(2)] = _p, __obfh_math_name[OBFH_NAME_INDEX(3)] = _0)), \
                                  (OBFH_NAME_PAIR(1, __obfh_math_name[OBFH_NAME_INDEX(3)] = _0, __obfh_math_name[OBFH_NAME_INDEX(2)] = _p), OBFH_NAME_PAIR(2, __obfh_math_name[OBFH_NAME_INDEX(1)] = _x, __obfh_math_name[OBFH_NAME_INDEX(0)] = _e)), \
                                  double (*)(double), (x))

#define fabs(x) OBFH_MATH_CRT_CALL(5, OBFH_ALPHA_LOW | OBFH_ALPHA_HIGH, \
                                   (OBFH_NAME_PAIR(1, __obfh_math_name[OBFH_NAME_INDEX(0)] = _f, __obfh_math_name[OBFH_NAME_INDEX(1)] = _a), OBFH_NAME_PAIR(2, __obfh_math_name[OBFH_NAME_INDEX(2)] = _b, __obfh_math_name[OBFH_NAME_INDEX(3)] = _s), __obfh_math_name[OBFH_NAME_INDEX(4)] = _0), \
                                   (OBFH_NAME_PAIR(1, __obfh_math_name[OBFH_NAME_INDEX(4)] = _0, __obfh_math_name[OBFH_NAME_INDEX(3)] = _s), OBFH_NAME_PAIR(2, __obfh_math_name[OBFH_NAME_INDEX(2)] = _b, __obfh_math_name[OBFH_NAME_INDEX(1)] = _a), __obfh_math_name[OBFH_NAME_INDEX(0)] = _f), \
                                   double (*)(double), (x))

#define floor(x) OBFH_MATH_CRT_CALL(6, OBFH_ALPHA_LOW | OBFH_ALPHA_HIGH, \
                                    (OBFH_NAME_PAIR(1, __obfh_math_name[OBFH_NAME_INDEX(0)] = _f, __obfh_math_name[OBFH_NAME_INDEX(1)] = _l), OBFH_NAME_PAIR(2, __obfh_math_name[OBFH_NAME_INDEX(2)] = _o, __obfh_math_name[OBFH_NAME_INDEX(3)] = _o), OBFH_NAME_PAIR(3, __obfh_math_name[OBFH_NAME_INDEX(4)] = _r, __obfh_math_name[OBFH_NAME_INDEX(5)] = _0)), \
                                    (OBFH_NAME_PAIR(1, __obfh_math_name[OBFH_NAME_INDEX(5)] = _0, __obfh_math_name[OBFH_NAME_INDEX(4)] = _r), OBFH_NAME_PAIR(2, __obfh_math_name[OBFH_NAME_INDEX(3)] = _o, __obfh_math_name[OBFH_NAME_INDEX(2)] = _o), OBFH_NAME_PAIR(3, __obfh_math_name[OBFH_NAME_INDEX(1)] = _l, __obfh_math_name[OBFH_NAME_INDEX(0)] = _f)), \
                                    double (*)(double), (x))

#define fmod(x, y) OBFH_MATH_CRT_CALL(5, OBFH_ALPHA_LOW | OBFH_ALPHA_HIGH, \
                                      (OBFH_NAME_PAIR(1, __obfh_math_name[OBFH_NAME_INDEX(0)] = _f, __obfh_math_name[OBFH_NAME_INDEX(1)] = _m), OBFH_NAME_PAIR(2, __obfh_math_name[OBFH_NAME_INDEX(2)] = _o, __obfh_math_name[OBFH_NAME_INDEX(3)] = _d), __obfh_math_name[OBFH_NAME_INDEX(4)] = _0), \
                                      (OBFH_NAME_PAIR(1, __obfh_math_name[OBFH_NAME_INDEX(4)] = _0, __obfh_math_name[OBFH_NAME_INDEX(3)] = _d), OBFH_NAME_PAIR(2, __obfh_math_name[OBFH_NAME_INDEX(2)] = _o, __obfh_math_name[OBFH_NAME_INDEX(1)] = _m), __obfh_math_name[OBFH_NAME_INDEX(0)] = _f), \
                                      double (*)(double, double), (x, y))

#define frexp(x, y) OBFH_MATH_CRT_CALL(6, OBFH_ALPHA_LOW | OBFH_ALPHA_HIGH, \
                                       (OBFH_NAME_PAIR(1, __obfh_math_name[OBFH_NAME_INDEX(0)] = _f, __obfh_math_name[OBFH_NAME_INDEX(1)] = _r), OBFH_NAME_PAIR(2, __obfh_math_name[OBFH_NAME_INDEX(2)] = _e, __obfh_math_name[OBFH_NAME_INDEX(3)] = _x), OBFH_NAME_PAIR(3, __obfh_math_name[OBFH_NAME_INDEX(4)] = _p, __obfh_math_name[OBFH_NAME_INDEX(5)] = _0)), \
                                       (OBFH_NAME_PAIR(1, __obfh_math_name[OBFH_NAME_INDEX(5)] = _0, __obfh_math_name[OBFH_NAME_INDEX(4)] = _p), OBFH_NAME_PAIR(2, __obfh_math_name[OBFH_NAME_INDEX(3)] = _x, __obfh_math_name[OBFH_NAME_INDEX(2)] = _e), OBFH_NAME_PAIR(3, __obfh_math_name[OBFH_NAME_INDEX(1)] = _r, __obfh_math_name[OBFH_NAME_INDEX(0)] = _f)), \
                                       double (*)(double, int *), (x, y))

#define ldexp(x, y) OBFH_MATH_CRT_CALL(6, OBFH_ALPHA_LOW | OBFH_ALPHA_HIGH, \
                                       (OBFH_NAME_PAIR(1, __obfh_math_name[OBFH_NAME_INDEX(0)] = _l, __obfh_math_name[OBFH_NAME_INDEX(1)] = _d), OBFH_NAME_PAIR(2, __obfh_math_name[OBFH_NAME_INDEX(2)] = _e, __obfh_math_name[OBFH_NAME_INDEX(3)] = _x), OBFH_NAME_PAIR(3, __obfh_math_name[OBFH_NAME_INDEX(4)] = _p, __obfh_math_name[OBFH_NAME_INDEX(5)] = _0)), \
                                       (OBFH_NAME_PAIR(1, __obfh_math_name[OBFH_NAME_INDEX(5)] = _0, __obfh_math_name[OBFH_NAME_INDEX(4)] = _p), OBFH_NAME_PAIR(2, __obfh_math_name[OBFH_NAME_INDEX(3)] = _x, __obfh_math_name[OBFH_NAME_INDEX(2)] = _e), OBFH_NAME_PAIR(3, __obfh_math_name[OBFH_NAME_INDEX(1)] = _d, __obfh_math_name[OBFH_NAME_INDEX(0)] = _l)), \
                                       double (*)(double, int), (x, y))

#define log(x) OBFH_MATH_CRT_CALL(4, OBFH_ALPHA_LOW | OBFH_ALPHA_HIGH, \
                                  (OBFH_NAME_PAIR(1, __obfh_math_name[OBFH_NAME_INDEX(0)] = _l, __obfh_math_name[OBFH_NAME_INDEX(1)] = _o), OBFH_NAME_PAIR(2, __obfh_math_name[OBFH_NAME_INDEX(2)] = _g, __obfh_math_name[OBFH_NAME_INDEX(3)] = _0)), \
                                  (OBFH_NAME_PAIR(1, __obfh_math_name[OBFH_NAME_INDEX(3)] = _0, __obfh_math_name[OBFH_NAME_INDEX(2)] = _g), OBFH_NAME_PAIR(2, __obfh_math_name[OBFH_NAME_INDEX(1)] = _o, __obfh_math_name[OBFH_NAME_INDEX(0)] = _l)), \
                                  double (*)(double), (x))

#define log10(x) OBFH_MATH_CRT_CALL(6, OBFH_ALPHA_LOW | OBFH_ALPHA_HIGH, \
                                    (__obfh_math_name[OBFH_NAME_INDEX(0)] = _l, __obfh_math_name[OBFH_NAME_INDEX(1)] = _o, __obfh_math_name[OBFH_NAME_INDEX(2)] = _g, __obfh_math_name[3] = (_1 + '0'), __obfh_math_name[4] = (_0 + '0'), __obfh_math_name[OBFH_NAME_INDEX(5)] = _0), \
                                    (__obfh_math_name[OBFH_NAME_INDEX(5)] = _0, __obfh_math_name[4] = (_0 + '0'), __obfh_math_name[3] = (_1 + '0'), __obfh_math_name[OBFH_NAME_INDEX(2)] = _g, __obfh_math_name[OBFH_NAME_INDEX(1)] = _o, __obfh_math_name[OBFH_NAME_INDEX(0)] = _l), \
                                    double (*)(double), (x))

#define modf(x, y) OBFH_MATH_CRT_CALL(5, OBFH_ALPHA_LOW | OBFH_ALPHA_HIGH, \
                                      (OBFH_NAME_PAIR(1, __obfh_math_name[OBFH_NAME_INDEX(0)] = _m, __obfh_math_name[OBFH_NAME_INDEX(1)] = _o), OBFH_NAME_PAIR(2, __obfh_math_name[OBFH_NAME_INDEX(2)] = _d, __obfh_math_name[OBFH_NAME_INDEX(3)] = _f), __obfh_math_name[OBFH_NAME_INDEX(4)] = _0), \
                                      (OBFH_NAME_PAIR(1, __obfh_math_name[OBFH_NAME_INDEX(4)] = _0, __obfh_math_name[OBFH_NAME_INDEX(3)] = _f), OBFH_NAME_PAIR(2, __obfh_math_name[OBFH_NAME_INDEX(2)] = _d, __obfh_math_name[OBFH_NAME_INDEX(1)] = _o), __obfh_math_name[OBFH_NAME_INDEX(0)] = _m), \
                                      double (*)(double, double *), (x, y))

#define pow(x, y) OBFH_MATH_CRT_CALL(4, OBFH_ALPHA_HIGH, \
                                     (OBFH_NAME_PAIR(1, __obfh_math_name[OBFH_NAME_INDEX(0)] = _p, __obfh_math_name[OBFH_NAME_INDEX(1)] = _o), OBFH_NAME_PAIR(2, __obfh_math_name[OBFH_NAME_INDEX(2)] = _w, __obfh_math_name[OBFH_NAME_INDEX(3)] = _0)), \
                                     (OBFH_NAME_PAIR(1, __obfh_math_name[OBFH_NAME_INDEX(3)] = _0, __obfh_math_name[OBFH_NAME_INDEX(2)] = _w), OBFH_NAME_PAIR(2, __obfh_math_name[OBFH_NAME_INDEX(1)] = _o, __obfh_math_name[OBFH_NAME_INDEX(0)] = _p)), \
                                     double (*)(double, double), (x, y))

#define sin(x) OBFH_MATH_CRT_CALL(4, OBFH_ALPHA_LOW | OBFH_ALPHA_HIGH, \
                                  (OBFH_NAME_PAIR(1, __obfh_math_name[OBFH_NAME_INDEX(0)] = _s, __obfh_math_name[OBFH_NAME_INDEX(1)] = _i), OBFH_NAME_PAIR(2, __obfh_math_name[OBFH_NAME_INDEX(2)] = _n, __obfh_math_name[OBFH_NAME_INDEX(3)] = _0)), \
                                  (OBFH_NAME_PAIR(1, __obfh_math_name[OBFH_NAME_INDEX(3)] = _0, __obfh_math_name[OBFH_NAME_INDEX(2)] = _n), OBFH_NAME_PAIR(2, __obfh_math_name[OBFH_NAME_INDEX(1)] = _i, __obfh_math_name[OBFH_NAME_INDEX(0)] = _s)), \
                                  double (*)(double), (x))

#define sinh(x) OBFH_MATH_CRT_CALL(5, OBFH_ALPHA_LOW | OBFH_ALPHA_HIGH, \
                                   (OBFH_NAME_PAIR(1, __obfh_math_name[OBFH_NAME_INDEX(0)] = _s, __obfh_math_name[OBFH_NAME_INDEX(1)] = _i), OBFH_NAME_PAIR(2, __obfh_math_name[OBFH_NAME_INDEX(2)] = _n, __obfh_math_name[OBFH_NAME_INDEX(3)] = _h), __obfh_math_name[OBFH_NAME_INDEX(4)] = _0), \
                                   (OBFH_NAME_PAIR(1, __obfh_math_name[OBFH_NAME_INDEX(4)] = _0, __obfh_math_name[OBFH_NAME_INDEX(3)] = _h), OBFH_NAME_PAIR(2, __obfh_math_name[OBFH_NAME_INDEX(2)] = _n, __obfh_math_name[OBFH_NAME_INDEX(1)] = _i), __obfh_math_name[OBFH_NAME_INDEX(0)] = _s), \
                                   double (*)(double), (x))

#define sqrt(x) OBFH_MATH_CRT_CALL(5, OBFH_ALPHA_HIGH, \
                                   (OBFH_NAME_PAIR(1, __obfh_math_name[OBFH_NAME_INDEX(0)] = _s, __obfh_math_name[OBFH_NAME_INDEX(1)] = _q), OBFH_NAME_PAIR(2, __obfh_math_name[OBFH_NAME_INDEX(2)] = _r, __obfh_math_name[OBFH_NAME_INDEX(3)] = _t), __obfh_math_name[OBFH_NAME_INDEX(4)] = _0), \
                                   (OBFH_NAME_PAIR(1, __obfh_math_name[OBFH_NAME_INDEX(4)] = _0, __obfh_math_name[OBFH_NAME_INDEX(3)] = _t), OBFH_NAME_PAIR(2, __obfh_math_name[OBFH_NAME_INDEX(2)] = _r, __obfh_math_name[OBFH_NAME_INDEX(1)] = _q), __obfh_math_name[OBFH_NAME_INDEX(0)] = _s), \
                                   double (*)(double), (x))

#define tan(x) OBFH_MATH_CRT_CALL(4, OBFH_ALPHA_LOW | OBFH_ALPHA_HIGH, \
                                  (OBFH_NAME_PAIR(1, __obfh_math_name[OBFH_NAME_INDEX(0)] = _t, __obfh_math_name[OBFH_NAME_INDEX(1)] = _a), OBFH_NAME_PAIR(2, __obfh_math_name[OBFH_NAME_INDEX(2)] = _n, __obfh_math_name[OBFH_NAME_INDEX(3)] = _0)), \
                                  (OBFH_NAME_PAIR(1, __obfh_math_name[OBFH_NAME_INDEX(3)] = _0, __obfh_math_name[OBFH_NAME_INDEX(2)] = _n), OBFH_NAME_PAIR(2, __obfh_math_name[OBFH_NAME_INDEX(1)] = _a, __obfh_math_name[OBFH_NAME_INDEX(0)] = _t)), \
                                  double (*)(double), (x))

#define tanh(x) OBFH_MATH_CRT_CALL(5, OBFH_ALPHA_LOW | OBFH_ALPHA_HIGH, \
                                   (OBFH_NAME_PAIR(1, __obfh_math_name[OBFH_NAME_INDEX(0)] = _t, __obfh_math_name[OBFH_NAME_INDEX(1)] = _a), OBFH_NAME_PAIR(2, __obfh_math_name[OBFH_NAME_INDEX(2)] = _n, __obfh_math_name[OBFH_NAME_INDEX(3)] = _h), __obfh_math_name[OBFH_NAME_INDEX(4)] = _0), \
                                   (OBFH_NAME_PAIR(1, __obfh_math_name[OBFH_NAME_INDEX(4)] = _0, __obfh_math_name[OBFH_NAME_INDEX(3)] = _h), OBFH_NAME_PAIR(2, __obfh_math_name[OBFH_NAME_INDEX(2)] = _n, __obfh_math_name[OBFH_NAME_INDEX(1)] = _a), __obfh_math_name[OBFH_NAME_INDEX(0)] = _t), \
                                   double (*)(double), (x))

// #endregion

// #region 23. Library marker and disabled-mode diagnostic
// ============================================================================
// 23. Library marker and disabled-mode diagnostic
// ============================================================================

#if OBFH_DISABLE_CONTROL_FLOW != 1
#undef if
#define if(...) if (OBFH_PUBLIC_IF_CONDITION(__VA_ARGS__))
#undef OBFH_FLOW_ELSE_GUARD
#define OBFH_FLOW_ELSE_GUARD OBFH_PUBLIC_ELSE_GUARD
#endif

__declspec(dllexport) __attribute__((weak)) char *WhatSoundDoesACowMake() OBFH_CODE_SECTION_ATTRIBUTE {
    PHANTOM_NOP;
    return HIDE_STRING("Moo");
}

// The matched outer conditional preserves dangling else; intercepted else runs
// the transport once, rather than duplicating it for every value/void return.
#if OBFH_DISABLE_CONTROL_FLOW != 1
#define OBFH_FLOW_RETURN_GUARD (0)
#define return \
    if OBFH_FLOW_RETURN_GUARD { \
    } else \
        return
#endif

#elif !OBFH_EDITOR_VIEW
#warning Obfuscation disabled!
#endif
#endif

// ;)

// #endregion
