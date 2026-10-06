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
#define BREAK_STACK_CFLOW ((void)0)
#define STACK_PROXY_FUNCTIONS ((void)0)
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
#define TEXT_SECTION_ATTRIBUTE SECTION_ATTRIBUTE(".text")  // Text section

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

// TCC encodes custom-section function RVAs relative to .text. Keep code in
// .text when publishing unwind-backed decoys; protected data stays separate.
#if defined(__TINYC__) && defined(__x86_64__) && defined(_WIN32) && !NO_PDATA_DECOYS
#define OBFH_CODE_SECTION_ATTRIBUTE TEXT_SECTION_ATTRIBUTE
#define OBFH_DATA_CODE_SECTION_ATTRIBUTE TEXT_SECTION_ATTRIBUTE
#else
#define OBFH_CODE_SECTION_ATTRIBUTE OBFH_SECTION_ATTRIBUTE
#define OBFH_DATA_CODE_SECTION_ATTRIBUTE DATA_SECTION_ATTRIBUTE
#endif

// A fixed seed makes builds repeatable; override it to vary builds as well as sites.
#ifndef OBFH_BUILD_SEED
#define OBFH_BUILD_SEED 0u
#endif
// Thanks to @horsicq && @ac3ss0r
#define RND(min, max) \
    ((min) + (((__COUNTER__ + (__LINE__ * __LINE__) + (unsigned int)OBFH_BUILD_SEED) * 2654435761u) % ((max) - (min) + 1)))

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
#define OBFH_DATA_DRAW(salt) OBFH_MIX_B(OBFH_MIX_A((unsigned int)__LINE__ ^ (unsigned int)OBFH_BUILD_SEED ^ ((unsigned int)(salt)*2654435761u)))

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
#define OBFH_PD_ASM(body)                                                                 \
    __obfh_asm__(                                                                         \
        "pushq %%rbp; movq %%rsp, %%rbp; .byte 0x48, 0x81, 0xec; .long %c[frame]; "       \
        "movl %%ecx, %%eax; movl %%eax, -%c[slot](%%rbp); " body                          \
        ".byte 0x48, 0x81, 0xc4; .long %c[frame]; popq %%rbp; ret;"                       \
        :                                                                                 \
        : [frame] "i"(__obfh_pd_frame), [slot] "i"(__obfh_pd_slot),                       \
          [key] "i"(__obfh_pd_key), [key2] "i"(__obfh_pd_key2), [mul] "i"(__obfh_pd_mul), \
          [rotate] "i"(__obfh_pd_rotate), [loops] "i"(__obfh_pd_loops)                    \
        : "rax", "rcx", "rdx", "cc", "memory")
#define OBFH_PD_DEFINE(site)                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            \
    static void __obfh_pdata_decoy_##site(void) __attribute__((noinline, used));                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        \
    static void __obfh_pdata_decoy_##site(void) {                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       \
        enum { __obfh_pd_kind = OBFH_PD_DRAW(site, 1u) & 15u,                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           \
               __obfh_pd_frame = 48u + 16u * (OBFH_PD_DRAW(site, 2u) % 14u),                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            \
               __obfh_pd_slot = 8u + 8u * (OBFH_PD_DRAW(site, 3u) % 5u),                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                \
               __obfh_pd_key = OBFH_PD_DRAW(site, 4u),                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  \
               __obfh_pd_key2 = OBFH_PD_DRAW(site, 5u),                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 \
               __obfh_pd_mul = OBFH_PD_DRAW(site, 6u) | 1u,                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             \
               __obfh_pd_rotate = 1u + (OBFH_PD_DRAW(site, 7u) % 31u),                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  \
               __obfh_pd_loops = 2u + (OBFH_PD_DRAW(site, 8u) % 6u) };                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  \
        __builtin_choose_expr(__obfh_pd_kind < 8u, __builtin_choose_expr(__obfh_pd_kind < 4u, __builtin_choose_expr(__obfh_pd_kind < 2u, __builtin_choose_expr(__obfh_pd_kind < 1u, ({ OBFH_PD_ASM(OBFH_PD_BODY_0); }), ({ OBFH_PD_ASM(OBFH_PD_BODY_1); })), __builtin_choose_expr(__obfh_pd_kind < 3u, ({ OBFH_PD_ASM(OBFH_PD_BODY_2); }), ({ OBFH_PD_ASM(OBFH_PD_BODY_3); }))), __builtin_choose_expr(__obfh_pd_kind < 6u, __builtin_choose_expr(__obfh_pd_kind < 5u, ({ OBFH_PD_ASM(OBFH_PD_BODY_4); }), ({ OBFH_PD_ASM(OBFH_PD_BODY_5); })), __builtin_choose_expr(__obfh_pd_kind < 7u, ({ OBFH_PD_ASM(OBFH_PD_BODY_6); }), ({ OBFH_PD_ASM(OBFH_PD_BODY_7); })))), __builtin_choose_expr(__obfh_pd_kind < 12u, __builtin_choose_expr(__obfh_pd_kind < 10u, __builtin_choose_expr(__obfh_pd_kind < 9u, ({ OBFH_PD_ASM(OBFH_PD_BODY_8); }), ({ OBFH_PD_ASM(OBFH_PD_BODY_9); })), __builtin_choose_expr(__obfh_pd_kind < 11u, ({ OBFH_PD_ASM(OBFH_PD_BODY_10); }), ({ OBFH_PD_ASM(OBFH_PD_BODY_11); }))), __builtin_choose_expr(__obfh_pd_kind < 14u, __builtin_choose_expr(__obfh_pd_kind < 13u, ({ OBFH_PD_ASM(OBFH_PD_BODY_12); }), ({ OBFH_PD_ASM(OBFH_PD_BODY_13); })), __builtin_choose_expr(__obfh_pd_kind < 15u, ({ OBFH_PD_ASM(OBFH_PD_BODY_14); }), ({ OBFH_PD_ASM(OBFH_PD_BODY_15); }))))); \
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

// All 128 templates are available; selection emits only the chosen asm.
#define OBFH_CFLOW_VARIANT_COUNT 128u
#define OBFH_CFLOW_GROUP_0(index) \
    __builtin_choose_expr((index) < 4u, __builtin_choose_expr((index) < 2u, __builtin_choose_expr((index) < 1u, ({ BREAK_STACK_CFLOW_0; }), ({ BREAK_STACK_CFLOW_1; })), __builtin_choose_expr((index) < 3u, ({ BREAK_STACK_CFLOW_2; }), ({ BREAK_STACK_CFLOW_3; }))), __builtin_choose_expr((index) < 6u, __builtin_choose_expr((index) < 5u, ({ BREAK_STACK_CFLOW_4; }), ({ BREAK_STACK_CFLOW_5; })), __builtin_choose_expr((index) < 7u, ({ BREAK_STACK_CFLOW_6; }), ({ BREAK_STACK_CFLOW_7; }))))
#define OBFH_CFLOW_GROUP_1(index) \
    __builtin_choose_expr((index) < 12u, __builtin_choose_expr((index) < 10u, __builtin_choose_expr((index) < 9u, ({ BREAK_STACK_CFLOW_8; }), ({ BREAK_STACK_CFLOW_9; })), __builtin_choose_expr((index) < 11u, ({ BREAK_STACK_CFLOW_10; }), ({ BREAK_STACK_CFLOW_11; }))), __builtin_choose_expr((index) < 14u, __builtin_choose_expr((index) < 13u, ({ BREAK_STACK_CFLOW_12; }), ({ BREAK_STACK_CFLOW_13; })), __builtin_choose_expr((index) < 15u, ({ BREAK_STACK_CFLOW_14; }), ({ BREAK_STACK_CFLOW_15; }))))
#define OBFH_CFLOW_GROUP_2(index) \
    __builtin_choose_expr((index) < 20u, __builtin_choose_expr((index) < 18u, __builtin_choose_expr((index) < 17u, ({ BREAK_STACK_CFLOW_16; }), ({ BREAK_STACK_CFLOW_17; })), __builtin_choose_expr((index) < 19u, ({ BREAK_STACK_CFLOW_18; }), ({ BREAK_STACK_CFLOW_19; }))), __builtin_choose_expr((index) < 22u, __builtin_choose_expr((index) < 21u, ({ BREAK_STACK_CFLOW_20; }), ({ BREAK_STACK_CFLOW_21; })), __builtin_choose_expr((index) < 23u, ({ BREAK_STACK_CFLOW_22; }), ({ BREAK_STACK_CFLOW_23; }))))
#define OBFH_CFLOW_GROUP_3(index) \
    __builtin_choose_expr((index) < 28u, __builtin_choose_expr((index) < 26u, __builtin_choose_expr((index) < 25u, ({ BREAK_STACK_CFLOW_24; }), ({ BREAK_STACK_CFLOW_25; })), __builtin_choose_expr((index) < 27u, ({ BREAK_STACK_CFLOW_26; }), ({ BREAK_STACK_CFLOW_27; }))), __builtin_choose_expr((index) < 30u, __builtin_choose_expr((index) < 29u, ({ BREAK_STACK_CFLOW_28; }), ({ BREAK_STACK_CFLOW_29; })), __builtin_choose_expr((index) < 31u, ({ BREAK_STACK_CFLOW_30; }), ({ BREAK_STACK_CFLOW_31; }))))
#define OBFH_CFLOW_GROUP_4(index) \
    __builtin_choose_expr((index) < 36u, __builtin_choose_expr((index) < 34u, __builtin_choose_expr((index) < 33u, ({ BREAK_STACK_CFLOW_32; }), ({ BREAK_STACK_CFLOW_33; })), __builtin_choose_expr((index) < 35u, ({ BREAK_STACK_CFLOW_34; }), ({ BREAK_STACK_CFLOW_35; }))), __builtin_choose_expr((index) < 38u, __builtin_choose_expr((index) < 37u, ({ BREAK_STACK_CFLOW_36; }), ({ BREAK_STACK_CFLOW_37; })), __builtin_choose_expr((index) < 39u, ({ BREAK_STACK_CFLOW_38; }), ({ BREAK_STACK_CFLOW_39; }))))
#define OBFH_CFLOW_GROUP_5(index) \
    __builtin_choose_expr((index) < 44u, __builtin_choose_expr((index) < 42u, __builtin_choose_expr((index) < 41u, ({ BREAK_STACK_CFLOW_40; }), ({ BREAK_STACK_CFLOW_41; })), __builtin_choose_expr((index) < 43u, ({ BREAK_STACK_CFLOW_42; }), ({ BREAK_STACK_CFLOW_43; }))), __builtin_choose_expr((index) < 46u, __builtin_choose_expr((index) < 45u, ({ BREAK_STACK_CFLOW_44; }), ({ BREAK_STACK_CFLOW_45; })), __builtin_choose_expr((index) < 47u, ({ BREAK_STACK_CFLOW_46; }), ({ BREAK_STACK_CFLOW_47; }))))
#define OBFH_CFLOW_GROUP_6(index) \
    __builtin_choose_expr((index) < 52u, __builtin_choose_expr((index) < 50u, __builtin_choose_expr((index) < 49u, ({ BREAK_STACK_CFLOW_48; }), ({ BREAK_STACK_CFLOW_49; })), __builtin_choose_expr((index) < 51u, ({ BREAK_STACK_CFLOW_50; }), ({ BREAK_STACK_CFLOW_51; }))), __builtin_choose_expr((index) < 54u, __builtin_choose_expr((index) < 53u, ({ BREAK_STACK_CFLOW_52; }), ({ BREAK_STACK_CFLOW_53; })), __builtin_choose_expr((index) < 55u, ({ BREAK_STACK_CFLOW_54; }), ({ BREAK_STACK_CFLOW_55; }))))
#define OBFH_CFLOW_GROUP_7(index) \
    __builtin_choose_expr((index) < 60u, __builtin_choose_expr((index) < 58u, __builtin_choose_expr((index) < 57u, ({ BREAK_STACK_CFLOW_56; }), ({ BREAK_STACK_CFLOW_57; })), __builtin_choose_expr((index) < 59u, ({ BREAK_STACK_CFLOW_58; }), ({ BREAK_STACK_CFLOW_59; }))), __builtin_choose_expr((index) < 62u, __builtin_choose_expr((index) < 61u, ({ BREAK_STACK_CFLOW_60; }), ({ BREAK_STACK_CFLOW_61; })), __builtin_choose_expr((index) < 63u, ({ BREAK_STACK_CFLOW_62; }), ({ BREAK_STACK_CFLOW_63; }))))
#define OBFH_CFLOW_GROUP_8(index) \
    __builtin_choose_expr((index) < 68u, __builtin_choose_expr((index) < 66u, __builtin_choose_expr((index) < 65u, ({ BREAK_STACK_CFLOW_64; }), ({ BREAK_STACK_CFLOW_65; })), __builtin_choose_expr((index) < 67u, ({ BREAK_STACK_CFLOW_66; }), ({ BREAK_STACK_CFLOW_67; }))), __builtin_choose_expr((index) < 70u, __builtin_choose_expr((index) < 69u, ({ BREAK_STACK_CFLOW_68; }), ({ BREAK_STACK_CFLOW_69; })), __builtin_choose_expr((index) < 71u, ({ BREAK_STACK_CFLOW_70; }), ({ BREAK_STACK_CFLOW_71; }))))
#define OBFH_CFLOW_GROUP_9(index) \
    __builtin_choose_expr((index) < 76u, __builtin_choose_expr((index) < 74u, __builtin_choose_expr((index) < 73u, ({ BREAK_STACK_CFLOW_72; }), ({ BREAK_STACK_CFLOW_73; })), __builtin_choose_expr((index) < 75u, ({ BREAK_STACK_CFLOW_74; }), ({ BREAK_STACK_CFLOW_75; }))), __builtin_choose_expr((index) < 78u, __builtin_choose_expr((index) < 77u, ({ BREAK_STACK_CFLOW_76; }), ({ BREAK_STACK_CFLOW_77; })), __builtin_choose_expr((index) < 79u, ({ BREAK_STACK_CFLOW_78; }), ({ BREAK_STACK_CFLOW_79; }))))
#define OBFH_CFLOW_GROUP_10(index) \
    __builtin_choose_expr((index) < 84u, __builtin_choose_expr((index) < 82u, __builtin_choose_expr((index) < 81u, ({ BREAK_STACK_CFLOW_80; }), ({ BREAK_STACK_CFLOW_81; })), __builtin_choose_expr((index) < 83u, ({ BREAK_STACK_CFLOW_82; }), ({ BREAK_STACK_CFLOW_83; }))), __builtin_choose_expr((index) < 86u, __builtin_choose_expr((index) < 85u, ({ BREAK_STACK_CFLOW_84; }), ({ BREAK_STACK_CFLOW_85; })), __builtin_choose_expr((index) < 87u, ({ BREAK_STACK_CFLOW_86; }), ({ BREAK_STACK_CFLOW_87; }))))
#define OBFH_CFLOW_GROUP_11(index) \
    __builtin_choose_expr((index) < 92u, __builtin_choose_expr((index) < 90u, __builtin_choose_expr((index) < 89u, ({ BREAK_STACK_CFLOW_88; }), ({ BREAK_STACK_CFLOW_89; })), __builtin_choose_expr((index) < 91u, ({ BREAK_STACK_CFLOW_90; }), ({ BREAK_STACK_CFLOW_91; }))), __builtin_choose_expr((index) < 94u, __builtin_choose_expr((index) < 93u, ({ BREAK_STACK_CFLOW_92; }), ({ BREAK_STACK_CFLOW_93; })), __builtin_choose_expr((index) < 95u, ({ BREAK_STACK_CFLOW_94; }), ({ BREAK_STACK_CFLOW_95; }))))
#define OBFH_CFLOW_GROUP_12(index) \
    __builtin_choose_expr((index) < 100u, __builtin_choose_expr((index) < 98u, __builtin_choose_expr((index) < 97u, ({ BREAK_STACK_CFLOW_96; }), ({ BREAK_STACK_CFLOW_97; })), __builtin_choose_expr((index) < 99u, ({ BREAK_STACK_CFLOW_98; }), ({ BREAK_STACK_CFLOW_99; }))), __builtin_choose_expr((index) < 102u, __builtin_choose_expr((index) < 101u, ({ BREAK_STACK_CFLOW_100; }), ({ BREAK_STACK_CFLOW_101; })), __builtin_choose_expr((index) < 103u, ({ BREAK_STACK_CFLOW_102; }), ({ BREAK_STACK_CFLOW_103; }))))
#define OBFH_CFLOW_GROUP_13(index) \
    __builtin_choose_expr((index) < 108u, __builtin_choose_expr((index) < 106u, __builtin_choose_expr((index) < 105u, ({ BREAK_STACK_CFLOW_104; }), ({ BREAK_STACK_CFLOW_105; })), __builtin_choose_expr((index) < 107u, ({ BREAK_STACK_CFLOW_106; }), ({ BREAK_STACK_CFLOW_107; }))), __builtin_choose_expr((index) < 110u, __builtin_choose_expr((index) < 109u, ({ BREAK_STACK_CFLOW_108; }), ({ BREAK_STACK_CFLOW_109; })), __builtin_choose_expr((index) < 111u, ({ BREAK_STACK_CFLOW_110; }), ({ BREAK_STACK_CFLOW_111; }))))
#define OBFH_CFLOW_GROUP_14(index) \
    __builtin_choose_expr((index) < 116u, __builtin_choose_expr((index) < 114u, __builtin_choose_expr((index) < 113u, ({ BREAK_STACK_CFLOW_112; }), ({ BREAK_STACK_CFLOW_113; })), __builtin_choose_expr((index) < 115u, ({ BREAK_STACK_CFLOW_114; }), ({ BREAK_STACK_CFLOW_115; }))), __builtin_choose_expr((index) < 118u, __builtin_choose_expr((index) < 117u, ({ BREAK_STACK_CFLOW_116; }), ({ BREAK_STACK_CFLOW_117; })), __builtin_choose_expr((index) < 119u, ({ BREAK_STACK_CFLOW_118; }), ({ BREAK_STACK_CFLOW_119; }))))
#define OBFH_CFLOW_GROUP_15(index) \
    __builtin_choose_expr((index) < 124u, __builtin_choose_expr((index) < 122u, __builtin_choose_expr((index) < 121u, ({ BREAK_STACK_CFLOW_120; }), ({ BREAK_STACK_CFLOW_121; })), __builtin_choose_expr((index) < 123u, ({ BREAK_STACK_CFLOW_122; }), ({ BREAK_STACK_CFLOW_123; }))), __builtin_choose_expr((index) < 126u, __builtin_choose_expr((index) < 125u, ({ BREAK_STACK_CFLOW_124; }), ({ BREAK_STACK_CFLOW_125; })), __builtin_choose_expr((index) < 127u, ({ BREAK_STACK_CFLOW_126; }), ({ BREAK_STACK_CFLOW_127; }))))
#define OBFH_CFLOW_SELECT(index) \
    __builtin_choose_expr((index) < 64u, __builtin_choose_expr((index) < 32u, __builtin_choose_expr((index) < 16u, __builtin_choose_expr((index) < 8u, OBFH_CFLOW_GROUP_0(index), OBFH_CFLOW_GROUP_1(index)), __builtin_choose_expr((index) < 24u, OBFH_CFLOW_GROUP_2(index), OBFH_CFLOW_GROUP_3(index))), __builtin_choose_expr((index) < 48u, __builtin_choose_expr((index) < 40u, OBFH_CFLOW_GROUP_4(index), OBFH_CFLOW_GROUP_5(index)), __builtin_choose_expr((index) < 56u, OBFH_CFLOW_GROUP_6(index), OBFH_CFLOW_GROUP_7(index)))), __builtin_choose_expr((index) < 96u, __builtin_choose_expr((index) < 80u, __builtin_choose_expr((index) < 72u, OBFH_CFLOW_GROUP_8(index), OBFH_CFLOW_GROUP_9(index)), __builtin_choose_expr((index) < 88u, OBFH_CFLOW_GROUP_10(index), OBFH_CFLOW_GROUP_11(index))), __builtin_choose_expr((index) < 112u, __builtin_choose_expr((index) < 104u, OBFH_CFLOW_GROUP_12(index), OBFH_CFLOW_GROUP_13(index)), __builtin_choose_expr((index) < 120u, OBFH_CFLOW_GROUP_14(index), OBFH_CFLOW_GROUP_15(index)))))

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
#define OBFH_CFLOW_EXTRA(index) OBFH_CFLOW_SELECT(OBFH_CFLOW_EXTRA_INDEX(index))
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

#define BREAK_STACK_CFLOW_0 \
    OBFH_CFLOW_ASM("movl %%esp, %%eax; addl %0, %%eax; " OBFH_CFLOW_PRED_ADJACENT_PRODUCT("%%eax", "%%edx") " testl $1, %%eax; jz 1f; " OBFH_CFLOW_PAYLOAD_CALL " 1:")

#define BREAK_STACK_CFLOW_1 \
    OBFH_CFLOW_ASM("movl %%esp, %%eax; addl %0, %%eax; " OBFH_CFLOW_PRED_SQUARE("%%eax") " testl $2, %%eax; jz 1f; " OBFH_CFLOW_PAYLOAD_INDIRECT " 1:")

#define BREAK_STACK_CFLOW_2 \
    OBFH_CFLOW_ASM("movl %%esp, %%eax; addl %0, %%eax; " OBFH_CFLOW_PRED_SQUARE_XOR("%%eax", "%%edx") " testl $1, %%eax; jz 1f; " OBFH_CFLOW_PAYLOAD_MOV " 1:")

#define BREAK_STACK_CFLOW_3 \
    OBFH_CFLOW_ASM("movl %%esp, %%eax; addl %0, %%eax; " OBFH_CFLOW_PRED_ODD_PRODUCT("%%eax", "%%edx") " testl $1, %%eax; jnz 1f; " OBFH_CFLOW_PAYLOAD_JUMP " 1:")

#define BREAK_STACK_CFLOW_4 \
    OBFH_CFLOW_ASM("movl %%esp, %%edx; addl %0, %%edx; " OBFH_CFLOW_PRED_CUBE_MINUS("%%edx", "%%ecx") " testl $1, %%edx; jz 1f; " OBFH_CFLOW_PAYLOAD_CALL " 1:")

#define BREAK_STACK_CFLOW_5 \
    OBFH_CFLOW_ASM("movl %%esp, %%edx; addl %0, %%edx; " OBFH_CFLOW_PRED_THREE_PRODUCT("%%edx", "%%ecx", "%%eax") " testl $1, %%edx; jz 1f; " OBFH_CFLOW_PAYLOAD_INDIRECT " 1:")

#define BREAK_STACK_CFLOW_6 \
    OBFH_CFLOW_ASM("movl %%esp, %%edx; addl %0, %%edx; " OBFH_CFLOW_PRED_FOUR_PRODUCT("%%edx", "%%ecx", "%%eax") " testl $7, %%edx; jz 1f; " OBFH_CFLOW_PAYLOAD_MOV " 1:")

#define BREAK_STACK_CFLOW_7 \
    OBFH_CFLOW_ASM("movl %%esp, %%edx; addl %0, %%edx; " OBFH_CFLOW_PRED_SQUARE_PRODUCT("%%edx", "%%ecx") " testl $3, %%edx; jz 1f; " OBFH_CFLOW_PAYLOAD_JUMP " 1:")

#define BREAK_STACK_CFLOW_8 \
    OBFH_CFLOW_ASM("movl %%esp, %%ecx; addl %0, %%ecx; " OBFH_CFLOW_PRED_FOURTH("%%ecx") " testl $14, %%ecx; jz 1f; " OBFH_CFLOW_PAYLOAD_CALL " 1:")

#define BREAK_STACK_CFLOW_9 \
    OBFH_CFLOW_ASM("movl %%esp, %%ecx; addl %0, %%ecx; " OBFH_CFLOW_PRED_FIFTH_MINUS("%%ecx", "%%eax") " testl $1, %%ecx; jz 1f; " OBFH_CFLOW_PAYLOAD_INDIRECT " 1:")

#define BREAK_STACK_CFLOW_10 \
    OBFH_CFLOW_ASM("movl %%esp, %%ecx; addl %0, %%ecx; " OBFH_CFLOW_PRED_ADJACENT_OR("%%ecx", "%%eax") " testl $1, %%ecx; jnz 1f; " OBFH_CFLOW_PAYLOAD_MOV " 1:")

#define BREAK_STACK_CFLOW_11 \
    OBFH_CFLOW_ASM("movl %%esp, %%ecx; addl %0, %%ecx; " OBFH_CFLOW_PRED_ADJACENT_AND("%%ecx", "%%eax") " testl $1, %%ecx; jz 1f; " OBFH_CFLOW_PAYLOAD_JUMP " 1:")

#define BREAK_STACK_CFLOW_12 \
    OBFH_CFLOW_ASM("movl %%esp, %%eax; addl %0, %%eax; " OBFH_CFLOW_PRED_PREVIOUS_PRODUCT("%%eax", "%%ecx") " testl $1, %%eax; jz 1f; " OBFH_CFLOW_PAYLOAD_CALL " 1:")

#define BREAK_STACK_CFLOW_13 \
    OBFH_CFLOW_ASM("movl %%esp, %%eax; addl %0, %%eax; " OBFH_CFLOW_PRED_SQUARE_PLUS("%%eax", "%%ecx") " testl $1, %%eax; jz 1f; " OBFH_CFLOW_PAYLOAD_INDIRECT " 1:")

#define BREAK_STACK_CFLOW_14 \
    OBFH_CFLOW_ASM("movl %%esp, %%eax; addl %0, %%eax; " OBFH_CFLOW_PRED_CUBE_PLUS("%%eax", "%%ecx") " testl $1, %%eax; jz 1f; " OBFH_CFLOW_PAYLOAD_MOV " 1:")

#define BREAK_STACK_CFLOW_15 \
    OBFH_CFLOW_ASM("movl %%esp, %%eax; addl %0, %%eax; " OBFH_CFLOW_PRED_ODD_SQUARE("%%eax") " testl $2, %%eax; jz 1f; " OBFH_CFLOW_PAYLOAD_JUMP " 1:")

#define BREAK_STACK_CFLOW_16 \
    OBFH_CFLOW_ASM("movl %%esp, %%eax; xorl %0, %%eax; " OBFH_CFLOW_PRED_ADJACENT_PRODUCT("%%eax", "%%edx") " testl $1, %%eax; jnz 1f; jmp 2f; 1: " OBFH_CFLOW_PAYLOAD_CALL " 2:")

#define BREAK_STACK_CFLOW_17 \
    OBFH_CFLOW_ASM("movl %%esp, %%eax; xorl %0, %%eax; " OBFH_CFLOW_PRED_SQUARE("%%eax") " testl $2, %%eax; jnz 1f; jmp 2f; 1: " OBFH_CFLOW_PAYLOAD_INDIRECT " 2:")

#define BREAK_STACK_CFLOW_18 \
    OBFH_CFLOW_ASM("movl %%esp, %%eax; xorl %0, %%eax; " OBFH_CFLOW_PRED_SQUARE_XOR("%%eax", "%%edx") " testl $1, %%eax; jnz 1f; jmp 2f; 1: " OBFH_CFLOW_PAYLOAD_MOV " 2:")

#define BREAK_STACK_CFLOW_19 \
    OBFH_CFLOW_ASM("movl %%esp, %%eax; xorl %0, %%eax; " OBFH_CFLOW_PRED_ODD_PRODUCT("%%eax", "%%edx") " testl $1, %%eax; jz 1f; jmp 2f; 1: " OBFH_CFLOW_PAYLOAD_JUMP " 2:")

#define BREAK_STACK_CFLOW_20 \
    OBFH_CFLOW_ASM("movl %%esp, %%edx; xorl %0, %%edx; " OBFH_CFLOW_PRED_CUBE_MINUS("%%edx", "%%ecx") " testl $1, %%edx; jnz 1f; jmp 2f; 1: " OBFH_CFLOW_PAYLOAD_CALL " 2:")

#define BREAK_STACK_CFLOW_21 \
    OBFH_CFLOW_ASM("movl %%esp, %%edx; xorl %0, %%edx; " OBFH_CFLOW_PRED_THREE_PRODUCT("%%edx", "%%ecx", "%%eax") " testl $1, %%edx; jnz 1f; jmp 2f; 1: " OBFH_CFLOW_PAYLOAD_INDIRECT " 2:")

#define BREAK_STACK_CFLOW_22 \
    OBFH_CFLOW_ASM("movl %%esp, %%edx; xorl %0, %%edx; " OBFH_CFLOW_PRED_FOUR_PRODUCT("%%edx", "%%ecx", "%%eax") " testl $7, %%edx; jnz 1f; jmp 2f; 1: " OBFH_CFLOW_PAYLOAD_MOV " 2:")

#define BREAK_STACK_CFLOW_23 \
    OBFH_CFLOW_ASM("movl %%esp, %%edx; xorl %0, %%edx; " OBFH_CFLOW_PRED_SQUARE_PRODUCT("%%edx", "%%ecx") " testl $3, %%edx; jnz 1f; jmp 2f; 1: " OBFH_CFLOW_PAYLOAD_JUMP " 2:")

#define BREAK_STACK_CFLOW_24 \
    OBFH_CFLOW_ASM("movl %%esp, %%ecx; xorl %0, %%ecx; " OBFH_CFLOW_PRED_FOURTH("%%ecx") " testl $14, %%ecx; jnz 1f; jmp 2f; 1: " OBFH_CFLOW_PAYLOAD_CALL " 2:")

#define BREAK_STACK_CFLOW_25 \
    OBFH_CFLOW_ASM("movl %%esp, %%ecx; xorl %0, %%ecx; " OBFH_CFLOW_PRED_FIFTH_MINUS("%%ecx", "%%eax") " testl $1, %%ecx; jnz 1f; jmp 2f; 1: " OBFH_CFLOW_PAYLOAD_INDIRECT " 2:")

#define BREAK_STACK_CFLOW_26 \
    OBFH_CFLOW_ASM("movl %%esp, %%ecx; xorl %0, %%ecx; " OBFH_CFLOW_PRED_ADJACENT_OR("%%ecx", "%%eax") " testl $1, %%ecx; jz 1f; jmp 2f; 1: " OBFH_CFLOW_PAYLOAD_MOV " 2:")

#define BREAK_STACK_CFLOW_27 \
    OBFH_CFLOW_ASM("movl %%esp, %%ecx; xorl %0, %%ecx; " OBFH_CFLOW_PRED_ADJACENT_AND("%%ecx", "%%eax") " testl $1, %%ecx; jnz 1f; jmp 2f; 1: " OBFH_CFLOW_PAYLOAD_JUMP " 2:")

#define BREAK_STACK_CFLOW_28 \
    OBFH_CFLOW_ASM("movl %%esp, %%eax; xorl %0, %%eax; " OBFH_CFLOW_PRED_PREVIOUS_PRODUCT("%%eax", "%%ecx") " testl $1, %%eax; jnz 1f; jmp 2f; 1: " OBFH_CFLOW_PAYLOAD_CALL " 2:")

#define BREAK_STACK_CFLOW_29 \
    OBFH_CFLOW_ASM("movl %%esp, %%eax; xorl %0, %%eax; " OBFH_CFLOW_PRED_SQUARE_PLUS("%%eax", "%%ecx") " testl $1, %%eax; jnz 1f; jmp 2f; 1: " OBFH_CFLOW_PAYLOAD_INDIRECT " 2:")

#define BREAK_STACK_CFLOW_30 \
    OBFH_CFLOW_ASM("movl %%esp, %%eax; xorl %0, %%eax; " OBFH_CFLOW_PRED_CUBE_PLUS("%%eax", "%%ecx") " testl $1, %%eax; jnz 1f; jmp 2f; 1: " OBFH_CFLOW_PAYLOAD_MOV " 2:")

#define BREAK_STACK_CFLOW_31 \
    OBFH_CFLOW_ASM("movl %%esp, %%eax; xorl %0, %%eax; " OBFH_CFLOW_PRED_ODD_SQUARE("%%eax") " testl $2, %%eax; jnz 1f; jmp 2f; 1: " OBFH_CFLOW_PAYLOAD_JUMP " 2:")

#define BREAK_STACK_CFLOW_32 \
    OBFH_CFLOW_ASM("movl %%esp, %%eax; subl %0, %%eax; " OBFH_CFLOW_PRED_ADJACENT_PRODUCT("%%eax", "%%edx") " testl $1, %%eax; jz 1f; " OBFH_CFLOW_PAYLOAD_CALL " 1: testl $1, %%eax; jz 2f; .byte 0x0F, 0x0B, 0xE8; .fill %c1, 1, %c2; 2:")

#define BREAK_STACK_CFLOW_33 \
    OBFH_CFLOW_ASM("movl %%esp, %%eax; subl %0, %%eax; " OBFH_CFLOW_PRED_SQUARE("%%eax") " testl $2, %%eax; jz 1f; " OBFH_CFLOW_PAYLOAD_INDIRECT " 1: testl $2, %%eax; jz 2f; .byte 0x0F, 0x0B, 0xE8; .fill %c1, 1, %c2; 2:")

#define BREAK_STACK_CFLOW_34 \
    OBFH_CFLOW_ASM("movl %%esp, %%eax; subl %0, %%eax; " OBFH_CFLOW_PRED_SQUARE_XOR("%%eax", "%%edx") " testl $1, %%eax; jz 1f; " OBFH_CFLOW_PAYLOAD_MOV " 1: testl $1, %%eax; jz 2f; .byte 0x0F, 0x0B, 0xE8; .fill %c1, 1, %c2; 2:")

#define BREAK_STACK_CFLOW_35 \
    OBFH_CFLOW_ASM("movl %%esp, %%eax; subl %0, %%eax; " OBFH_CFLOW_PRED_ODD_PRODUCT("%%eax", "%%edx") " testl $1, %%eax; jnz 1f; " OBFH_CFLOW_PAYLOAD_JUMP " 1: testl $1, %%eax; jnz 2f; .byte 0x0F, 0x0B, 0xE8; .fill %c1, 1, %c2; 2:")

#define BREAK_STACK_CFLOW_36 \
    OBFH_CFLOW_ASM("movl %%esp, %%edx; subl %0, %%edx; " OBFH_CFLOW_PRED_CUBE_MINUS("%%edx", "%%ecx") " testl $1, %%edx; jz 1f; " OBFH_CFLOW_PAYLOAD_CALL " 1: testl $1, %%edx; jz 2f; .byte 0x0F, 0x0B, 0xE8; .fill %c1, 1, %c2; 2:")

#define BREAK_STACK_CFLOW_37 \
    OBFH_CFLOW_ASM("movl %%esp, %%edx; subl %0, %%edx; " OBFH_CFLOW_PRED_THREE_PRODUCT("%%edx", "%%ecx", "%%eax") " testl $1, %%edx; jz 1f; " OBFH_CFLOW_PAYLOAD_INDIRECT " 1: testl $1, %%edx; jz 2f; .byte 0x0F, 0x0B, 0xE8; .fill %c1, 1, %c2; 2:")

#define BREAK_STACK_CFLOW_38 \
    OBFH_CFLOW_ASM("movl %%esp, %%edx; subl %0, %%edx; " OBFH_CFLOW_PRED_FOUR_PRODUCT("%%edx", "%%ecx", "%%eax") " testl $7, %%edx; jz 1f; " OBFH_CFLOW_PAYLOAD_MOV " 1: testl $7, %%edx; jz 2f; .byte 0x0F, 0x0B, 0xE8; .fill %c1, 1, %c2; 2:")

#define BREAK_STACK_CFLOW_39 \
    OBFH_CFLOW_ASM("movl %%esp, %%edx; subl %0, %%edx; " OBFH_CFLOW_PRED_SQUARE_PRODUCT("%%edx", "%%ecx") " testl $3, %%edx; jz 1f; " OBFH_CFLOW_PAYLOAD_JUMP " 1: testl $3, %%edx; jz 2f; .byte 0x0F, 0x0B, 0xE8; .fill %c1, 1, %c2; 2:")

#define BREAK_STACK_CFLOW_40 \
    OBFH_CFLOW_ASM("movl %%esp, %%ecx; subl %0, %%ecx; " OBFH_CFLOW_PRED_FOURTH("%%ecx") " testl $14, %%ecx; jz 1f; " OBFH_CFLOW_PAYLOAD_CALL " 1: testl $14, %%ecx; jz 2f; .byte 0x0F, 0x0B, 0xE8; .fill %c1, 1, %c2; 2:")

#define BREAK_STACK_CFLOW_41 \
    OBFH_CFLOW_ASM("movl %%esp, %%ecx; subl %0, %%ecx; " OBFH_CFLOW_PRED_FIFTH_MINUS("%%ecx", "%%eax") " testl $1, %%ecx; jz 1f; " OBFH_CFLOW_PAYLOAD_INDIRECT " 1: testl $1, %%ecx; jz 2f; .byte 0x0F, 0x0B, 0xE8; .fill %c1, 1, %c2; 2:")

#define BREAK_STACK_CFLOW_42 \
    OBFH_CFLOW_ASM("movl %%esp, %%ecx; subl %0, %%ecx; " OBFH_CFLOW_PRED_ADJACENT_OR("%%ecx", "%%eax") " testl $1, %%ecx; jnz 1f; " OBFH_CFLOW_PAYLOAD_MOV " 1: testl $1, %%ecx; jnz 2f; .byte 0x0F, 0x0B, 0xE8; .fill %c1, 1, %c2; 2:")

#define BREAK_STACK_CFLOW_43 \
    OBFH_CFLOW_ASM("movl %%esp, %%ecx; subl %0, %%ecx; " OBFH_CFLOW_PRED_ADJACENT_AND("%%ecx", "%%eax") " testl $1, %%ecx; jz 1f; " OBFH_CFLOW_PAYLOAD_JUMP " 1: testl $1, %%ecx; jz 2f; .byte 0x0F, 0x0B, 0xE8; .fill %c1, 1, %c2; 2:")

#define BREAK_STACK_CFLOW_44 \
    OBFH_CFLOW_ASM("movl %%esp, %%eax; subl %0, %%eax; " OBFH_CFLOW_PRED_PREVIOUS_PRODUCT("%%eax", "%%ecx") " testl $1, %%eax; jz 1f; " OBFH_CFLOW_PAYLOAD_CALL " 1: testl $1, %%eax; jz 2f; .byte 0x0F, 0x0B, 0xE8; .fill %c1, 1, %c2; 2:")

#define BREAK_STACK_CFLOW_45 \
    OBFH_CFLOW_ASM("movl %%esp, %%eax; subl %0, %%eax; " OBFH_CFLOW_PRED_SQUARE_PLUS("%%eax", "%%ecx") " testl $1, %%eax; jz 1f; " OBFH_CFLOW_PAYLOAD_INDIRECT " 1: testl $1, %%eax; jz 2f; .byte 0x0F, 0x0B, 0xE8; .fill %c1, 1, %c2; 2:")

#define BREAK_STACK_CFLOW_46 \
    OBFH_CFLOW_ASM("movl %%esp, %%eax; subl %0, %%eax; " OBFH_CFLOW_PRED_CUBE_PLUS("%%eax", "%%ecx") " testl $1, %%eax; jz 1f; " OBFH_CFLOW_PAYLOAD_MOV " 1: testl $1, %%eax; jz 2f; .byte 0x0F, 0x0B, 0xE8; .fill %c1, 1, %c2; 2:")

#define BREAK_STACK_CFLOW_47 \
    OBFH_CFLOW_ASM("movl %%esp, %%eax; subl %0, %%eax; " OBFH_CFLOW_PRED_ODD_SQUARE("%%eax") " testl $2, %%eax; jz 1f; " OBFH_CFLOW_PAYLOAD_JUMP " 1: testl $2, %%eax; jz 2f; .byte 0x0F, 0x0B, 0xE8; .fill %c1, 1, %c2; 2:")

#define BREAK_STACK_CFLOW_48 \
    OBFH_CFLOW_ASM("movl %%esp, %%eax; roll $7, %%eax; addl %0, %%eax; " OBFH_CFLOW_PRED_ADJACENT_PRODUCT("%%eax", "%%edx") " testl $1, %%eax; jnz 1f; jmp 2f; 1: .byte 0xC3; " OBFH_CFLOW_PAYLOAD_CALL " 2: testl $1, %%eax; jz 3f; .byte 0xFF, 0x25; .long %c3; 3:")

#define BREAK_STACK_CFLOW_49 \
    OBFH_CFLOW_ASM("movl %%esp, %%eax; roll $7, %%eax; addl %0, %%eax; " OBFH_CFLOW_PRED_SQUARE("%%eax") " testl $2, %%eax; jnz 1f; jmp 2f; 1: .byte 0xC3; " OBFH_CFLOW_PAYLOAD_INDIRECT " 2: testl $2, %%eax; jz 3f; .byte 0xFF, 0x25; .long %c3; 3:")

#define BREAK_STACK_CFLOW_50 \
    OBFH_CFLOW_ASM("movl %%esp, %%eax; roll $7, %%eax; addl %0, %%eax; " OBFH_CFLOW_PRED_SQUARE_XOR("%%eax", "%%edx") " testl $1, %%eax; jnz 1f; jmp 2f; 1: .byte 0xC3; " OBFH_CFLOW_PAYLOAD_MOV " 2: testl $1, %%eax; jz 3f; .byte 0xFF, 0x25; .long %c3; 3:")

#define BREAK_STACK_CFLOW_51 \
    OBFH_CFLOW_ASM("movl %%esp, %%eax; roll $7, %%eax; addl %0, %%eax; " OBFH_CFLOW_PRED_ODD_PRODUCT("%%eax", "%%edx") " testl $1, %%eax; jz 1f; jmp 2f; 1: .byte 0xC3; " OBFH_CFLOW_PAYLOAD_JUMP " 2: testl $1, %%eax; jnz 3f; .byte 0xFF, 0x25; .long %c3; 3:")

#define BREAK_STACK_CFLOW_52 \
    OBFH_CFLOW_ASM("movl %%esp, %%edx; roll $7, %%edx; addl %0, %%edx; " OBFH_CFLOW_PRED_CUBE_MINUS("%%edx", "%%ecx") " testl $1, %%edx; jnz 1f; jmp 2f; 1: .byte 0xC3; " OBFH_CFLOW_PAYLOAD_CALL " 2: testl $1, %%edx; jz 3f; .byte 0xFF, 0x25; .long %c3; 3:")

#define BREAK_STACK_CFLOW_53 \
    OBFH_CFLOW_ASM("movl %%esp, %%edx; roll $7, %%edx; addl %0, %%edx; " OBFH_CFLOW_PRED_THREE_PRODUCT("%%edx", "%%ecx", "%%eax") " testl $1, %%edx; jnz 1f; jmp 2f; 1: .byte 0xC3; " OBFH_CFLOW_PAYLOAD_INDIRECT " 2: testl $1, %%edx; jz 3f; .byte 0xFF, 0x25; .long %c3; 3:")

#define BREAK_STACK_CFLOW_54 \
    OBFH_CFLOW_ASM("movl %%esp, %%edx; roll $7, %%edx; addl %0, %%edx; " OBFH_CFLOW_PRED_FOUR_PRODUCT("%%edx", "%%ecx", "%%eax") " testl $7, %%edx; jnz 1f; jmp 2f; 1: .byte 0xC3; " OBFH_CFLOW_PAYLOAD_MOV " 2: testl $7, %%edx; jz 3f; .byte 0xFF, 0x25; .long %c3; 3:")

#define BREAK_STACK_CFLOW_55 \
    OBFH_CFLOW_ASM("movl %%esp, %%edx; roll $7, %%edx; addl %0, %%edx; " OBFH_CFLOW_PRED_SQUARE_PRODUCT("%%edx", "%%ecx") " testl $3, %%edx; jnz 1f; jmp 2f; 1: .byte 0xC3; " OBFH_CFLOW_PAYLOAD_JUMP " 2: testl $3, %%edx; jz 3f; .byte 0xFF, 0x25; .long %c3; 3:")

#define BREAK_STACK_CFLOW_56 \
    OBFH_CFLOW_ASM("movl %%esp, %%ecx; roll $7, %%ecx; addl %0, %%ecx; " OBFH_CFLOW_PRED_FOURTH("%%ecx") " testl $14, %%ecx; jnz 1f; jmp 2f; 1: .byte 0xC3; " OBFH_CFLOW_PAYLOAD_CALL " 2: testl $14, %%ecx; jz 3f; .byte 0xFF, 0x25; .long %c3; 3:")

#define BREAK_STACK_CFLOW_57 \
    OBFH_CFLOW_ASM("movl %%esp, %%ecx; roll $7, %%ecx; addl %0, %%ecx; " OBFH_CFLOW_PRED_FIFTH_MINUS("%%ecx", "%%eax") " testl $1, %%ecx; jnz 1f; jmp 2f; 1: .byte 0xC3; " OBFH_CFLOW_PAYLOAD_INDIRECT " 2: testl $1, %%ecx; jz 3f; .byte 0xFF, 0x25; .long %c3; 3:")

#define BREAK_STACK_CFLOW_58 \
    OBFH_CFLOW_ASM("movl %%esp, %%ecx; roll $7, %%ecx; addl %0, %%ecx; " OBFH_CFLOW_PRED_ADJACENT_OR("%%ecx", "%%eax") " testl $1, %%ecx; jz 1f; jmp 2f; 1: .byte 0xC3; " OBFH_CFLOW_PAYLOAD_MOV " 2: testl $1, %%ecx; jnz 3f; .byte 0xFF, 0x25; .long %c3; 3:")

#define BREAK_STACK_CFLOW_59 \
    OBFH_CFLOW_ASM("movl %%esp, %%ecx; roll $7, %%ecx; addl %0, %%ecx; " OBFH_CFLOW_PRED_ADJACENT_AND("%%ecx", "%%eax") " testl $1, %%ecx; jnz 1f; jmp 2f; 1: .byte 0xC3; " OBFH_CFLOW_PAYLOAD_JUMP " 2: testl $1, %%ecx; jz 3f; .byte 0xFF, 0x25; .long %c3; 3:")

#define BREAK_STACK_CFLOW_60 \
    OBFH_CFLOW_ASM("movl %%esp, %%eax; roll $7, %%eax; addl %0, %%eax; " OBFH_CFLOW_PRED_PREVIOUS_PRODUCT("%%eax", "%%ecx") " testl $1, %%eax; jnz 1f; jmp 2f; 1: .byte 0xC3; " OBFH_CFLOW_PAYLOAD_CALL " 2: testl $1, %%eax; jz 3f; .byte 0xFF, 0x25; .long %c3; 3:")

#define BREAK_STACK_CFLOW_61 \
    OBFH_CFLOW_ASM("movl %%esp, %%eax; roll $7, %%eax; addl %0, %%eax; " OBFH_CFLOW_PRED_SQUARE_PLUS("%%eax", "%%ecx") " testl $1, %%eax; jnz 1f; jmp 2f; 1: .byte 0xC3; " OBFH_CFLOW_PAYLOAD_INDIRECT " 2: testl $1, %%eax; jz 3f; .byte 0xFF, 0x25; .long %c3; 3:")

#define BREAK_STACK_CFLOW_62 \
    OBFH_CFLOW_ASM("movl %%esp, %%eax; roll $7, %%eax; addl %0, %%eax; " OBFH_CFLOW_PRED_CUBE_PLUS("%%eax", "%%ecx") " testl $1, %%eax; jnz 1f; jmp 2f; 1: .byte 0xC3; " OBFH_CFLOW_PAYLOAD_MOV " 2: testl $1, %%eax; jz 3f; .byte 0xFF, 0x25; .long %c3; 3:")

#define BREAK_STACK_CFLOW_63 \
    OBFH_CFLOW_ASM("movl %%esp, %%eax; roll $7, %%eax; addl %0, %%eax; " OBFH_CFLOW_PRED_ODD_SQUARE("%%eax") " testl $2, %%eax; jnz 1f; jmp 2f; 1: .byte 0xC3; " OBFH_CFLOW_PAYLOAD_JUMP " 2: testl $2, %%eax; jz 3f; .byte 0xFF, 0x25; .long %c3; 3:")

#define BREAK_STACK_CFLOW_64 \
    OBFH_CFLOW_ASM("movl %%esp, %%eax; addl %0, %%eax; " OBFH_CFLOW_PRED_ADJACENT_PRODUCT("%%eax", "%%edx") "  testl $1, %%eax; jz 1f; " OBFH_CFLOW_PAYLOAD_CALL_DATA " 1: movl %%esp, %%edx; addl %0, %%edx; " OBFH_CFLOW_PRED_SQUARE_PRODUCT("%%edx", "%%ecx") "  testl $3, %%edx; jz 2f; .byte 0xFF, 0x25; .long %c3; .byte %c2, 0xE9; 2:")

#define BREAK_STACK_CFLOW_65 \
    OBFH_CFLOW_ASM("movl %%esp, %%eax; addl %0, %%eax; " OBFH_CFLOW_PRED_SQUARE("%%eax") "  testl $2, %%eax; jz 1f; " OBFH_CFLOW_PAYLOAD_CALL_DATA " 1: movl %%esp, %%ecx; addl %0, %%ecx; " OBFH_CFLOW_PRED_FOURTH("%%ecx") "  testl $14, %%ecx; jz 2f; .byte 0xFF, 0x25; .long %c3; .byte %c2, 0xE9; 2:")

#define BREAK_STACK_CFLOW_66 \
    OBFH_CFLOW_ASM("movl %%esp, %%eax; addl %0, %%eax; " OBFH_CFLOW_PRED_SQUARE_XOR("%%eax", "%%edx") "  testl $1, %%eax; jz 1f; " OBFH_CFLOW_PAYLOAD_CALL_DATA " 1: movl %%esp, %%ecx; addl %0, %%ecx; " OBFH_CFLOW_PRED_FIFTH_MINUS("%%ecx", "%%eax") "  testl $1, %%ecx; jz 2f; .byte 0xFF, 0x25; .long %c3; .byte %c2, 0xE9; 2:")

#define BREAK_STACK_CFLOW_67 \
    OBFH_CFLOW_ASM("movl %%esp, %%eax; addl %0, %%eax; " OBFH_CFLOW_PRED_ODD_PRODUCT("%%eax", "%%edx") "  testl $1, %%eax; jnz 1f; " OBFH_CFLOW_PAYLOAD_CALL_DATA " 1: movl %%esp, %%ecx; addl %0, %%ecx; " OBFH_CFLOW_PRED_ADJACENT_OR("%%ecx", "%%eax") "  testl $1, %%ecx; jnz 2f; .byte 0xFF, 0x25; .long %c3; .byte %c2, 0xE9; 2:")

#define BREAK_STACK_CFLOW_68 \
    OBFH_CFLOW_ASM("movl %%esp, %%edx; addl %0, %%edx; " OBFH_CFLOW_PRED_CUBE_MINUS("%%edx", "%%ecx") "  testl $1, %%edx; jz 1f; " OBFH_CFLOW_PAYLOAD_CALL_DATA " 1: movl %%esp, %%ecx; addl %0, %%ecx; " OBFH_CFLOW_PRED_ADJACENT_AND("%%ecx", "%%eax") "  testl $1, %%ecx; jz 2f; .byte 0xFF, 0x25; .long %c3; .byte %c2, 0xE9; 2:")

#define BREAK_STACK_CFLOW_69 \
    OBFH_CFLOW_ASM("movl %%esp, %%edx; addl %0, %%edx; " OBFH_CFLOW_PRED_THREE_PRODUCT("%%edx", "%%ecx", "%%eax") "  testl $1, %%edx; jz 1f; " OBFH_CFLOW_PAYLOAD_CALL_DATA " 1: movl %%esp, %%eax; addl %0, %%eax; " OBFH_CFLOW_PRED_PREVIOUS_PRODUCT("%%eax", "%%ecx") "  testl $1, %%eax; jz 2f; .byte 0xFF, 0x25; .long %c3; .byte %c2, 0xE9; 2:")

#define BREAK_STACK_CFLOW_70 \
    OBFH_CFLOW_ASM("movl %%esp, %%edx; addl %0, %%edx; " OBFH_CFLOW_PRED_FOUR_PRODUCT("%%edx", "%%ecx", "%%eax") "  testl $7, %%edx; jz 1f; " OBFH_CFLOW_PAYLOAD_CALL_DATA " 1: movl %%esp, %%eax; addl %0, %%eax; " OBFH_CFLOW_PRED_SQUARE_PLUS("%%eax", "%%ecx") "  testl $1, %%eax; jz 2f; .byte 0xFF, 0x25; .long %c3; .byte %c2, 0xE9; 2:")

#define BREAK_STACK_CFLOW_71 \
    OBFH_CFLOW_ASM("movl %%esp, %%edx; addl %0, %%edx; " OBFH_CFLOW_PRED_SQUARE_PRODUCT("%%edx", "%%ecx") "  testl $3, %%edx; jz 1f; " OBFH_CFLOW_PAYLOAD_CALL_DATA " 1: movl %%esp, %%eax; addl %0, %%eax; " OBFH_CFLOW_PRED_CUBE_PLUS("%%eax", "%%ecx") "  testl $1, %%eax; jz 2f; .byte 0xFF, 0x25; .long %c3; .byte %c2, 0xE9; 2:")

#define BREAK_STACK_CFLOW_72 \
    OBFH_CFLOW_ASM("movl %%esp, %%ecx; addl %0, %%ecx; " OBFH_CFLOW_PRED_FOURTH("%%ecx") "  testl $14, %%ecx; jz 1f; " OBFH_CFLOW_PAYLOAD_CALL_DATA " 1: movl %%esp, %%eax; addl %0, %%eax; " OBFH_CFLOW_PRED_ODD_SQUARE("%%eax") "  testl $2, %%eax; jz 2f; .byte 0xFF, 0x25; .long %c3; .byte %c2, 0xE9; 2:")

#define BREAK_STACK_CFLOW_73 \
    OBFH_CFLOW_ASM("movl %%esp, %%ecx; addl %0, %%ecx; " OBFH_CFLOW_PRED_FIFTH_MINUS("%%ecx", "%%eax") "  testl $1, %%ecx; jz 1f; " OBFH_CFLOW_PAYLOAD_CALL_DATA " 1: movl %%esp, %%eax; addl %0, %%eax; " OBFH_CFLOW_PRED_ADJACENT_PRODUCT("%%eax", "%%edx") "  testl $1, %%eax; jz 2f; .byte 0xFF, 0x25; .long %c3; .byte %c2, 0xE9; 2:")

#define BREAK_STACK_CFLOW_74 \
    OBFH_CFLOW_ASM("movl %%esp, %%ecx; addl %0, %%ecx; " OBFH_CFLOW_PRED_ADJACENT_OR("%%ecx", "%%eax") "  testl $1, %%ecx; jnz 1f; " OBFH_CFLOW_PAYLOAD_CALL_DATA " 1: movl %%esp, %%eax; addl %0, %%eax; " OBFH_CFLOW_PRED_SQUARE("%%eax") "  testl $2, %%eax; jz 2f; .byte 0xFF, 0x25; .long %c3; .byte %c2, 0xE9; 2:")

#define BREAK_STACK_CFLOW_75 \
    OBFH_CFLOW_ASM("movl %%esp, %%ecx; addl %0, %%ecx; " OBFH_CFLOW_PRED_ADJACENT_AND("%%ecx", "%%eax") "  testl $1, %%ecx; jz 1f; " OBFH_CFLOW_PAYLOAD_CALL_DATA " 1: movl %%esp, %%eax; addl %0, %%eax; " OBFH_CFLOW_PRED_SQUARE_XOR("%%eax", "%%edx") "  testl $1, %%eax; jz 2f; .byte 0xFF, 0x25; .long %c3; .byte %c2, 0xE9; 2:")

#define BREAK_STACK_CFLOW_76 \
    OBFH_CFLOW_ASM("movl %%esp, %%eax; addl %0, %%eax; " OBFH_CFLOW_PRED_PREVIOUS_PRODUCT("%%eax", "%%ecx") "  testl $1, %%eax; jz 1f; " OBFH_CFLOW_PAYLOAD_CALL_DATA " 1: movl %%esp, %%eax; addl %0, %%eax; " OBFH_CFLOW_PRED_ODD_PRODUCT("%%eax", "%%edx") "  testl $1, %%eax; jnz 2f; .byte 0xFF, 0x25; .long %c3; .byte %c2, 0xE9; 2:")

#define BREAK_STACK_CFLOW_77 \
    OBFH_CFLOW_ASM("movl %%esp, %%eax; addl %0, %%eax; " OBFH_CFLOW_PRED_SQUARE_PLUS("%%eax", "%%ecx") "  testl $1, %%eax; jz 1f; " OBFH_CFLOW_PAYLOAD_CALL_DATA " 1: movl %%esp, %%edx; addl %0, %%edx; " OBFH_CFLOW_PRED_CUBE_MINUS("%%edx", "%%ecx") "  testl $1, %%edx; jz 2f; .byte 0xFF, 0x25; .long %c3; .byte %c2, 0xE9; 2:")

#define BREAK_STACK_CFLOW_78 \
    OBFH_CFLOW_ASM("movl %%esp, %%eax; addl %0, %%eax; " OBFH_CFLOW_PRED_CUBE_PLUS("%%eax", "%%ecx") "  testl $1, %%eax; jz 1f; " OBFH_CFLOW_PAYLOAD_CALL_DATA " 1: movl %%esp, %%edx; addl %0, %%edx; " OBFH_CFLOW_PRED_THREE_PRODUCT("%%edx", "%%ecx", "%%eax") "  testl $1, %%edx; jz 2f; .byte 0xFF, 0x25; .long %c3; .byte %c2, 0xE9; 2:")

#define BREAK_STACK_CFLOW_79 \
    OBFH_CFLOW_ASM("movl %%esp, %%eax; addl %0, %%eax; " OBFH_CFLOW_PRED_ODD_SQUARE("%%eax") "  testl $2, %%eax; jz 1f; " OBFH_CFLOW_PAYLOAD_CALL_DATA " 1: movl %%esp, %%edx; addl %0, %%edx; " OBFH_CFLOW_PRED_FOUR_PRODUCT("%%edx", "%%ecx", "%%eax") "  testl $7, %%edx; jz 2f; .byte 0xFF, 0x25; .long %c3; .byte %c2, 0xE9; 2:")

#define BREAK_STACK_CFLOW_80 \
    OBFH_CFLOW_ASM("movl %%esp, %%eax; xorl %0, %%eax; " OBFH_CFLOW_PRED_ADJACENT_PRODUCT("%%eax", "%%edx") "  testl $1, %%eax; jnz 1f; jmp 2f; 1: " OBFH_CFLOW_PAYLOAD_CALL_DATA " 2: movl %%esp, %%edx; addl %0, %%edx; " OBFH_CFLOW_PRED_SQUARE_PRODUCT("%%edx", "%%ecx") "  testl $3, %%edx; jz 3f; " OBFH_CFLOW_PAYLOAD_TRAP_INDIRECT " 3:")

#define BREAK_STACK_CFLOW_81 \
    OBFH_CFLOW_ASM("movl %%esp, %%eax; xorl %0, %%eax; " OBFH_CFLOW_PRED_SQUARE("%%eax") "  testl $2, %%eax; jnz 1f; jmp 2f; 1: " OBFH_CFLOW_PAYLOAD_CALL_DATA " 2: movl %%esp, %%ecx; addl %0, %%ecx; " OBFH_CFLOW_PRED_FOURTH("%%ecx") "  testl $14, %%ecx; jz 3f; " OBFH_CFLOW_PAYLOAD_TRAP_INDIRECT " 3:")

#define BREAK_STACK_CFLOW_82 \
    OBFH_CFLOW_ASM("movl %%esp, %%eax; xorl %0, %%eax; " OBFH_CFLOW_PRED_SQUARE_XOR("%%eax", "%%edx") "  testl $1, %%eax; jnz 1f; jmp 2f; 1: " OBFH_CFLOW_PAYLOAD_CALL_DATA " 2: movl %%esp, %%ecx; addl %0, %%ecx; " OBFH_CFLOW_PRED_FIFTH_MINUS("%%ecx", "%%eax") "  testl $1, %%ecx; jz 3f; " OBFH_CFLOW_PAYLOAD_TRAP_INDIRECT " 3:")

#define BREAK_STACK_CFLOW_83 \
    OBFH_CFLOW_ASM("movl %%esp, %%eax; xorl %0, %%eax; " OBFH_CFLOW_PRED_ODD_PRODUCT("%%eax", "%%edx") "  testl $1, %%eax; jz 1f; jmp 2f; 1: " OBFH_CFLOW_PAYLOAD_CALL_DATA " 2: movl %%esp, %%ecx; addl %0, %%ecx; " OBFH_CFLOW_PRED_ADJACENT_OR("%%ecx", "%%eax") "  testl $1, %%ecx; jnz 3f; " OBFH_CFLOW_PAYLOAD_TRAP_INDIRECT " 3:")

#define BREAK_STACK_CFLOW_84 \
    OBFH_CFLOW_ASM("movl %%esp, %%edx; xorl %0, %%edx; " OBFH_CFLOW_PRED_CUBE_MINUS("%%edx", "%%ecx") "  testl $1, %%edx; jnz 1f; jmp 2f; 1: " OBFH_CFLOW_PAYLOAD_CALL_DATA " 2: movl %%esp, %%ecx; addl %0, %%ecx; " OBFH_CFLOW_PRED_ADJACENT_AND("%%ecx", "%%eax") "  testl $1, %%ecx; jz 3f; " OBFH_CFLOW_PAYLOAD_TRAP_INDIRECT " 3:")

#define BREAK_STACK_CFLOW_85 \
    OBFH_CFLOW_ASM("movl %%esp, %%edx; xorl %0, %%edx; " OBFH_CFLOW_PRED_THREE_PRODUCT("%%edx", "%%ecx", "%%eax") "  testl $1, %%edx; jnz 1f; jmp 2f; 1: " OBFH_CFLOW_PAYLOAD_CALL_DATA " 2: movl %%esp, %%eax; addl %0, %%eax; " OBFH_CFLOW_PRED_PREVIOUS_PRODUCT("%%eax", "%%ecx") "  testl $1, %%eax; jz 3f; " OBFH_CFLOW_PAYLOAD_TRAP_INDIRECT " 3:")

#define BREAK_STACK_CFLOW_86 \
    OBFH_JUNK_ASM("xorl %%eax, %%eax; jz 1f; .byte 0xE8; " OBFH_JUNK_PAYLOAD " 1: cpuid;")

#define BREAK_STACK_CFLOW_87 ({                                                                                         \
    enum { __obfh_break_salt = RND(1, 32767),                                                                           \
           __obfh_break_gap = RND(1, 255) };                                                                            \
    __obfh_asm__("movzbl %7, %%eax; xorl %8, %%eax; cmpl %9, %%eax; jne 1f; .byte 0x00, 0xE8; " OBFH_JUNK_PAYLOAD " 1:" \
                 :                                                                                                      \
                 : OBFH_JUNK_INPUTS, "m"(_0), "i"(__obfh_break_salt), "i"(__obfh_break_salt + __obfh_break_gap)         \
                 : OBFH_CPUID_CLOBBERS);                                                                                \
})

#define BREAK_STACK_CFLOW_88 ({                                                                                                                                \
    enum { __obfh_break_salt = RND(1, 32767),                                                                                                                  \
           __obfh_break_case1 = RND(129, 191),                                                                                                                 \
           __obfh_break_case2 = RND(257, 319) };                                                                                                               \
    __obfh_asm__("movzbl %7, %%eax; addl %8, %%eax; cmpl %9, %%eax; jne 1f; .byte 0x00, 0x00; " OBFH_JUNK_PAYLOAD                                              \
                 " 1: cmpl %10, %%eax; jne 2f; .byte 0xFF, 0x25; " OBFH_JUNK_PAYLOAD " 2:"                                                                     \
                 :                                                                                                                                             \
                 : OBFH_JUNK_INPUTS, "m"(_0), "i"(__obfh_break_salt), "i"(__obfh_break_salt + __obfh_break_case1), "i"(__obfh_break_salt + __obfh_break_case2) \
                 : OBFH_CPUID_CLOBBERS);                                                                                                                       \
})

#define BREAK_STACK_CFLOW_89 \
    OBFH_JUNK_ASM("xorl %%ebx, %%ebx; xorl %%edx, %%edx; xorl %%ebx, %%edx; jz 1f; mov $4, %%eax; .byte 0x00; " OBFH_JUNK_PAYLOAD " 1: cpuid;")

#define BREAK_STACK_CFLOW_90 \
    OBFH_JUNK_ASM("xorl %%ebx, %%ebx; xorl %%eax, %%eax; mov %%eax, %%ebx; mov %%edx, %%ebx; xorl %%ebx, %%edx; jz 1f; .byte 0x20; " OBFH_JUNK_PAYLOAD " 1: cpuid;")

#define BREAK_STACK_CFLOW_91 \
    OBFH_JUNK_ASM("xorl %%edx, %%edx; xorl %%eax, %%eax; mov %%eax, %%edx; jz 1f; .byte 0xE8; " OBFH_JUNK_PAYLOAD " 1: cpuid;")

#define BREAK_STACK_CFLOW_92 \
    OBFH_JUNK_ASM("xorl %%edx, %%edx; jz 1f; .byte 0xE8; " OBFH_JUNK_PAYLOAD " 1: cpuid;")

#define BREAK_STACK_CFLOW_93 \
    OBFH_JUNK_ASM("xorl %%eax, %%eax; jz 1f; .byte 0x50; " OBFH_JUNK_PAYLOAD " 1: cpuid;")

#define BREAK_STACK_CFLOW_94 \
    OBFH_JUNK_ASM("xorl %%edx, %%edx; jz 1f; .byte 0x00, 0x00; " OBFH_JUNK_PAYLOAD " 1: cpuid;")

#define BREAK_STACK_CFLOW_95 \
    OBFH_STACK_JUNK_ASM("movl %%esp, %%eax; addl %0, %%eax; leal 1(%%eax), %%edx; imull %%edx, %%eax; testl $1, %%eax; jz 1f; .byte 0xE8; " OBFH_STACK_JUNK_PAYLOAD " 1:", "eax", "edx")

#define BREAK_STACK_CFLOW_96 \
    OBFH_STACK_JUNK_ASM("movl %%esp, %%edx; addl %0, %%edx; imull %%edx, %%edx; testl $2, %%edx; jz 1f; .byte 0xFF, 0x25; " OBFH_STACK_JUNK_PAYLOAD " 1:", "edx")

#define BREAK_STACK_CFLOW_97 \
    OBFH_STACK_JUNK_ASM("movl %%esp, %%eax; addl %0, %%eax; movl %%eax, %%edx; imull %%eax, %%eax; xorl %%edx, %%eax; testl $1, %%eax; jz 1f; .byte 0x0F, 0x0B, 0xE8; " OBFH_STACK_JUNK_PAYLOAD " 1:", "eax", "edx")

#define BREAK_STACK_CFLOW_98 \
    OBFH_STACK_JUNK_ASM("movl %%esp, %%eax; addl %0, %%eax; leal 1(%%eax), %%edx; imull %%edx, %%eax; addl $1, %%eax; testl $1, %%eax; jnz 1f; .byte 0xC3, 0xE8; " OBFH_STACK_JUNK_PAYLOAD " 1:", "eax", "edx")

#define BREAK_STACK_CFLOW_99 \
    OBFH_CFLOW_NAMED_ASM(OBFH_CFLOW_PRED_ADD_CARRY "je 1f;" OBFH_CFLOW_DATA_CALL "1:" OBFH_CFLOW_PRED_ROTATE_XOR "je 2f;" OBFH_CFLOW_DATA_INDIRECT "2:")

#define BREAK_STACK_CFLOW_100 \
    OBFH_CFLOW_NAMED_ASM(OBFH_CFLOW_PRED_OR_AND_SUM "jne 1f; jmp 2f; 1:" OBFH_CFLOW_DATA_STACK "2:" OBFH_CFLOW_PRED_ROTATE_RESTORE "sete %%cl; movzbl %%cl, %%ecx; testl %%ecx, %%ecx; jnz 3f;" OBFH_CFLOW_DATA_TRAP "3:")

#define BREAK_STACK_CFLOW_101 \
    OBFH_CFLOW_NAMED_ASM(OBFH_CFLOW_PRED_SUB_BORROW "sete %%cl; movzbl %%cl, %%ecx; negl %%ecx; testl %%ecx, %%ecx; js 1f;" OBFH_CFLOW_DATA_FRAME "1:" OBFH_CFLOW_PRED_BSWAP_XOR "jne 2f; jmp 3f; 2:" OBFH_CFLOW_DATA_RETURN "3:")

#define BREAK_STACK_CFLOW_102 \
    OBFH_CFLOW_NAMED_ASM(OBFH_CFLOW_PRED_DEMORGAN "je 2f; 1:" OBFH_CFLOW_DATA_INDIRECT "2:" OBFH_CFLOW_PRED_WORD_PARTITION "jne 1b; jmp 3f;" OBFH_CFLOW_DATA_CALL "3:")

#define BREAK_STACK_CFLOW_103 \
    OBFH_CFLOW_NAMED_ASM(OBFH_CFLOW_PRED_OR_DISTRIBUTE "je 1f;" OBFH_CFLOW_DATA_TRAP "1:" OBFH_CFLOW_PRED_BYTE_PARITY "je 2f;" OBFH_CFLOW_DATA_STACK "2:")

#define BREAK_STACK_CFLOW_104 \
    OBFH_CFLOW_NAMED_ASM(OBFH_CFLOW_PRED_AND_PARTITION "jne 1f; jmp 2f; 1:" OBFH_CFLOW_DATA_RETURN "2:" OBFH_CFLOW_PRED_BYTE_ROTATE "sete %%cl; movzbl %%cl, %%ecx; testl %%ecx, %%ecx; jnz 3f;" OBFH_CFLOW_DATA_FRAME "3:")

#define BREAK_STACK_CFLOW_105 \
    OBFH_CFLOW_NAMED_ASM(OBFH_CFLOW_PRED_XOR_CANCEL "sete %%cl; movzbl %%cl, %%ecx; negl %%ecx; testl %%ecx, %%ecx; js 1f;" OBFH_CFLOW_DATA_CALL "1:" OBFH_CFLOW_PRED_WORD_ROTATE "jne 2f; jmp 3f; 2:" OBFH_CFLOW_DATA_INDIRECT "3:")

#define BREAK_STACK_CFLOW_106 \
    OBFH_CFLOW_NAMED_ASM(OBFH_CFLOW_PRED_COMPLEMENT_CARRY "jnc 2f; 1:" OBFH_CFLOW_DATA_STACK "2:" OBFH_CFLOW_PRED_NEG_COMPLEMENT "jne 1b; jmp 3f;" OBFH_CFLOW_DATA_TRAP "3:")

#define BREAK_STACK_CFLOW_107 \
    OBFH_CFLOW_NAMED_ASM(OBFH_CFLOW_PRED_COMPLEMENT_WRAP "jc 1f;" OBFH_CFLOW_DATA_FRAME "1:" OBFH_CFLOW_PRED_MUL_DISTRIBUTE "je 2f;" OBFH_CFLOW_DATA_RETURN "2:")

#define BREAK_STACK_CFLOW_108 \
    OBFH_CFLOW_NAMED_ASM(OBFH_CFLOW_PRED_MASK_SUBTRACT "jc 1f; jmp 2f; 1:" OBFH_CFLOW_DATA_INDIRECT "2:" OBFH_CFLOW_PRED_SQUARE_EXPAND "sete %%cl; movzbl %%cl, %%ecx; testl %%ecx, %%ecx; jnz 3f;" OBFH_CFLOW_DATA_CALL "3:")

#define BREAK_STACK_CFLOW_109 \
    OBFH_CFLOW_NAMED_ASM(OBFH_CFLOW_PRED_MASK_OR_ORDER "setae %%cl; movzbl %%cl, %%ecx; negl %%ecx; testl %%ecx, %%ecx; js 1f;" OBFH_CFLOW_DATA_TRAP "1:" OBFH_CFLOW_PRED_NEG_SQUARE "jne 2f; jmp 3f; 2:" OBFH_CFLOW_DATA_STACK "3:")

#define BREAK_STACK_CFLOW_110 \
    OBFH_CFLOW_NAMED_ASM(OBFH_CFLOW_PRED_ROTATE_XOR "je 2f; 1:" OBFH_CFLOW_DATA_RETURN "2:" OBFH_CFLOW_PRED_BSWAP_NOT "jne 1b; jmp 3f;" OBFH_CFLOW_DATA_FRAME "3:")

#define BREAK_STACK_CFLOW_111 \
    OBFH_CFLOW_NAMED_ASM(OBFH_CFLOW_PRED_ROTATE_RESTORE "je 1f;" OBFH_CFLOW_DATA_CALL "1:" OBFH_CFLOW_PRED_SUB_MASK "je 2f;" OBFH_CFLOW_DATA_INDIRECT "2:")

#define BREAK_STACK_CFLOW_112 \
    OBFH_CFLOW_NAMED_ASM(OBFH_CFLOW_PRED_BSWAP_XOR "jne 1f; jmp 2f; 1:" OBFH_CFLOW_DATA_STACK "2:" OBFH_CFLOW_PRED_MASK_ORDER "setbe %%cl; movzbl %%cl, %%ecx; testl %%ecx, %%ecx; jnz 3f;" OBFH_CFLOW_DATA_TRAP "3:")

#define BREAK_STACK_CFLOW_113 \
    OBFH_CFLOW_NAMED_ASM(OBFH_CFLOW_PRED_WORD_PARTITION "sete %%cl; movzbl %%cl, %%ecx; negl %%ecx; testl %%ecx, %%ecx; js 1f;" OBFH_CFLOW_DATA_FRAME "1:" OBFH_CFLOW_PRED_COMPLEMENT_TRANSLATE "jne 2f; jmp 3f; 2:" OBFH_CFLOW_DATA_RETURN "3:")

#define BREAK_STACK_CFLOW_114 \
    OBFH_CFLOW_NAMED_ASM(OBFH_CFLOW_PRED_BYTE_PARITY "je 2f; 1:" OBFH_CFLOW_DATA_INDIRECT "2:" OBFH_CFLOW_PRED_SQUARE_RESIDUE "jne 1b; jmp 3f;" OBFH_CFLOW_DATA_CALL "3:")

#define BREAK_STACK_CFLOW_115 \
    OBFH_CFLOW_NAMED_ASM(OBFH_CFLOW_PRED_BYTE_ROTATE "je 1f;" OBFH_CFLOW_DATA_TRAP "1:" OBFH_CFLOW_PRED_FOURTH_RESIDUE "je 2f;" OBFH_CFLOW_DATA_STACK "2:")

#define BREAK_STACK_CFLOW_116 \
    OBFH_CFLOW_NAMED_ASM(OBFH_CFLOW_PRED_WORD_ROTATE "jne 1f; jmp 2f; 1:" OBFH_CFLOW_DATA_RETURN "2:" OBFH_CFLOW_PRED_ISOLATE_BIT "sete %%cl; movzbl %%cl, %%ecx; testl %%ecx, %%ecx; jnz 3f;" OBFH_CFLOW_DATA_FRAME "3:")

#define BREAK_STACK_CFLOW_117 \
    OBFH_CFLOW_NAMED_ASM(OBFH_CFLOW_PRED_NEG_COMPLEMENT "sete %%cl; movzbl %%cl, %%ecx; negl %%ecx; testl %%ecx, %%ecx; js 1f;" OBFH_CFLOW_DATA_CALL "1:" OBFH_CFLOW_PRED_ADD_CARRY "jne 2f; jmp 3f; 2:" OBFH_CFLOW_DATA_INDIRECT "3:")

#define BREAK_STACK_CFLOW_118 \
    OBFH_CFLOW_NAMED_ASM(OBFH_CFLOW_PRED_MUL_DISTRIBUTE "je 2f; 1:" OBFH_CFLOW_DATA_STACK "2:" OBFH_CFLOW_PRED_OR_AND_SUM "jne 1b; jmp 3f;" OBFH_CFLOW_DATA_TRAP "3:")

#define BREAK_STACK_CFLOW_119 \
    OBFH_CFLOW_NAMED_ASM(OBFH_CFLOW_PRED_SQUARE_EXPAND "je 1f;" OBFH_CFLOW_DATA_FRAME "1:" OBFH_CFLOW_PRED_SUB_BORROW "je 2f;" OBFH_CFLOW_DATA_RETURN "2:")

#define BREAK_STACK_CFLOW_120 \
    OBFH_CFLOW_NAMED_ASM(OBFH_CFLOW_PRED_NEG_SQUARE "jne 1f; jmp 2f; 1:" OBFH_CFLOW_DATA_INDIRECT "2:" OBFH_CFLOW_PRED_DEMORGAN "sete %%cl; movzbl %%cl, %%ecx; testl %%ecx, %%ecx; jnz 3f;" OBFH_CFLOW_DATA_CALL "3:")

#define BREAK_STACK_CFLOW_121 \
    OBFH_CFLOW_NAMED_ASM(OBFH_CFLOW_PRED_BSWAP_NOT "sete %%cl; movzbl %%cl, %%ecx; negl %%ecx; testl %%ecx, %%ecx; js 1f;" OBFH_CFLOW_DATA_TRAP "1:" OBFH_CFLOW_PRED_OR_DISTRIBUTE "jne 2f; jmp 3f; 2:" OBFH_CFLOW_DATA_STACK "3:")

#define BREAK_STACK_CFLOW_122 \
    OBFH_CFLOW_NAMED_ASM(OBFH_CFLOW_PRED_SUB_MASK "je 2f; 1:" OBFH_CFLOW_DATA_RETURN "2:" OBFH_CFLOW_PRED_AND_PARTITION "jne 1b; jmp 3f;" OBFH_CFLOW_DATA_FRAME "3:")

#define BREAK_STACK_CFLOW_123 \
    OBFH_CFLOW_NAMED_ASM(OBFH_CFLOW_PRED_MASK_ORDER "jbe 1f;" OBFH_CFLOW_DATA_CALL "1:" OBFH_CFLOW_PRED_XOR_CANCEL "je 2f;" OBFH_CFLOW_DATA_INDIRECT "2:")

#define BREAK_STACK_CFLOW_124 \
    OBFH_CFLOW_NAMED_ASM(OBFH_CFLOW_PRED_COMPLEMENT_TRANSLATE "jne 1f; jmp 2f; 1:" OBFH_CFLOW_DATA_STACK "2:" OBFH_CFLOW_PRED_COMPLEMENT_CARRY "setnc %%cl; movzbl %%cl, %%ecx; testl %%ecx, %%ecx; jnz 3f;" OBFH_CFLOW_DATA_TRAP "3:")

#define BREAK_STACK_CFLOW_125 \
    OBFH_CFLOW_NAMED_ASM(OBFH_CFLOW_PRED_SQUARE_RESIDUE "sete %%cl; movzbl %%cl, %%ecx; negl %%ecx; testl %%ecx, %%ecx; js 1f;" OBFH_CFLOW_DATA_FRAME "1:" OBFH_CFLOW_PRED_COMPLEMENT_WRAP "jnc 2f; jmp 3f; 2:" OBFH_CFLOW_DATA_RETURN "3:")

#define BREAK_STACK_CFLOW_126 \
    OBFH_CFLOW_NAMED_ASM(OBFH_CFLOW_PRED_FOURTH_RESIDUE "je 2f; 1:" OBFH_CFLOW_DATA_INDIRECT "2:" OBFH_CFLOW_PRED_MASK_SUBTRACT "jc 1b; jmp 3f;" OBFH_CFLOW_DATA_CALL "3:")

#define BREAK_STACK_CFLOW_127 \
    OBFH_CFLOW_NAMED_ASM(OBFH_CFLOW_PRED_ISOLATE_BIT "je 1f;" OBFH_CFLOW_DATA_TRAP "1:" OBFH_CFLOW_PRED_MASK_OR_ORDER "jae 2f;" OBFH_CFLOW_DATA_STACK "2:")

#define BREAK_STACK_CFLOW OBFH_CFLOW_EMIT(__COUNTER__, OBFH_CFLOW_EXTRA)

// Self-contained fake functions: live guards skip all calls and stack edits.
// Frames and calling conventions follow Windows TCC x86/x64. Numeric labels
// stay inside each ASM expansion; no insertion refers to another call site.
#define OBFH_SF_INPUTS                                                                                                                                                                                                                                                                                                                           \
    [sf_salt] "i"(RND(1, 32767)), [sf_rotate] "i"(RND(1, 31)),                                                                                                                                                                                                                                                                                   \
        [sf_frame_a] "i"(RND(2, 12) * 16u), [sf_frame_b] "i"(RND(2, 12) * 16u), [sf_frame_c] "i"(RND(2, 12) * 16u),                                                                                                                                                                                                                              \
        [sf_arg_a] "i"(RND(1, 65535)), [sf_arg_b] "i"(RND(1, 65535)),                                                                                                                                                                                                                                                                            \
        [sf_key_a] "i"(OBFH_JUNK_WORD), [sf_key_b] "i"(OBFH_JUNK_WORD), [sf_factor] "i"(RND(1, 32767) * 2u + 1u),                                                                                                                                                                                                                                \
        [sf_pad] "i"(RND(0, 7)), [sf_loops] "i"(RND(1, 7)), [sf_mask] "i"(__obfh_sf_mask),                                                                                                                                                                                                                                                       \
        [sf_not_mask] "i"(~(unsigned int)__obfh_sf_mask), [sf_frame_d] "i"(RND(2, 12) * 16u), [sf_pad_b] "i"(RND(0, 15)), [sf_noise_a] "i"(OBFH_JUNK_BYTE), [sf_noise_b] "i"(OBFH_JUNK_BYTE), [sf_local_a] "i"(RND(1, 3) * 4u), [sf_local_b] "i"(RND(4, 7) * 4u), [sf_leaf_frame] "i"(RND(2, 6) * 16u + 8u), [sf_leaf_slot] "i"(RND(0, 6) * 4u), \
        [sf_leaf_alu_a] "i"(__obfh_sf_alu_a == 0 ? 0x05 : __obfh_sf_alu_a == 1 ? 0x0d                                                                                                                                                                                                                                                            \
                                                      : __obfh_sf_alu_a == 2   ? 0x25                                                                                                                                                                                                                                                            \
                                                      : __obfh_sf_alu_a == 3   ? 0x2d                                                                                                                                                                                                                                                            \
                                                                               : 0x35),                                                                                                                                                                                                                                                            \
        [sf_leaf_alu_b] "i"(0xc2 + __obfh_sf_alu_b * 8),                                                                                                                                                                                                                                                                                         \
        [sf_leaf_shift] "i"(__obfh_sf_shift == 0 ? 0xc0 : __obfh_sf_shift == 1 ? 0xc8                                                                                                                                                                                                                                                            \
                                                      : __obfh_sf_shift == 2   ? 0xe0                                                                                                                                                                                                                                                            \
                                                      : __obfh_sf_shift == 3   ? 0xe8                                                                                                                                                                                                                                                            \
                                                                               : 0xf8)

#if defined(__x86_64__)
#define OBFH_SF_INPUT "movl %%esp, %%eax;"
#define OBFH_SF_FRAME(size) "pushq %%rbp; movq %%rsp, %%rbp; subq $%c[" size "], %%rsp;"
#define OBFH_SF_FRAME_ALT(size) "pushq %%rbp; movq %%rsp, %%rbp; leaq -%c[" size "](%%rsp), %%rsp;"
#define OBFH_SF_EPILOGUE_ALT "movq %%rbp, %%rsp; popq %%rbp; ret;"
#define OBFH_SF_ARGS "movl %%ecx, %%eax;"
#define OBFH_SF_CALL(target) "subq $32, %%rsp; movl $%c[sf_arg_a], %%ecx; movl $%c[sf_arg_b], %%edx; call " target "; addq $32, %%rsp;"
#else
#define OBFH_SF_INPUT "movl %%esp, %%eax;"
#define OBFH_SF_FRAME(size) "pushl %%ebp; movl %%esp, %%ebp; subl $%c[" size "], %%esp; nop;"
#define OBFH_SF_FRAME_ALT(size) "pushl %%ebp; movl %%esp, %%ebp; leal -%c[" size "](%%esp), %%esp;"
#define OBFH_SF_EPILOGUE_ALT "movl %%ebp, %%esp; popl %%ebp; ret;"
#define OBFH_SF_ARGS "movl 12(%%ebp), %%edx; movl 8(%%ebp), %%eax;"
#define OBFH_SF_CALL(target) "pushl $%c[sf_arg_b]; pushl $%c[sf_arg_a]; call " target "; addl $8, %%esp;"
#endif
#define OBFH_SF_EPILOGUE "leave; ret;"
#define OBFH_SF_PADDING ".fill %c[sf_pad], 1, 0x90;"
#define OBFH_SF_GAP ".fill %c[sf_pad_b], 1, 0x90; .byte %c[sf_noise_a], %c[sf_noise_b];"
#define OBFH_SF_LOCAL "movl %%eax, -%c[sf_local_a](%%ebp); movl %%edx, -%c[sf_local_b](%%ebp);"
// Use pointer-width addressing in fake x64 bodies as well as their prologues.
#if defined(__x86_64__)
#undef OBFH_SF_LOCAL
#define OBFH_SF_LOCAL "movl %%eax, -%c[sf_local_a](%%rbp); movl %%edx, -%c[sf_local_b](%%rbp);"
#endif
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

#define OBFH_SF_ENTRY_ALT(label, frame, body) label ": " OBFH_SF_FRAME_ALT(frame) body
#define OBFH_SF_ENTRY(label, frame, body) label ": " OBFH_SF_FRAME(frame) body
#define OBFH_SF_LAYOUT_0(a, b, c)                                               \
    OBFH_SF_CALL("1f")                                                          \
    OBFH_SF_EPILOGUE OBFH_SF_PADDING                                            \
        OBFH_SF_ENTRY("1", "sf_frame_a", a) OBFH_SF_CALL("2f") OBFH_SF_EPILOGUE \
            OBFH_SF_ENTRY("2", "sf_frame_b", b) "leave; jmp 1b;"
#define OBFH_SF_LAYOUT_1(a, b, c)                                                                              \
    OBFH_SF_CALL("1f")                                                                                         \
    OBFH_SF_EPILOGUE OBFH_SF_PADDING                                                                           \
        OBFH_SF_ENTRY("1", "sf_frame_a", a) "testl $1, %%eax; jz 4f;" OBFH_SF_CALL("2f") "4:" OBFH_SF_EPILOGUE \
            OBFH_SF_ENTRY("2", "sf_frame_b", b) OBFH_SF_EPILOGUE
#define OBFH_SF_LAYOUT_2(a, b, c)                                                                  \
    OBFH_SF_CALL("1f")                                                                             \
    OBFH_SF_EPILOGUE OBFH_SF_PADDING                                                               \
        OBFH_SF_ENTRY("1", "sf_frame_a", a) OBFH_SF_CALL("2f") OBFH_SF_CALL("3f") OBFH_SF_EPILOGUE \
            OBFH_SF_ENTRY("2", "sf_frame_b", b) OBFH_SF_CALL("3f") OBFH_SF_EPILOGUE                \
                OBFH_SF_ENTRY("3", "sf_frame_c", c) "leave; jmp 1b;"
#define OBFH_SF_LAYOUT_3(a, b, c)                                                                                                               \
    OBFH_SF_CALL("1f")                                                                                                                          \
    OBFH_SF_EPILOGUE OBFH_SF_PADDING                                                                                                            \
        OBFH_SF_ENTRY("1", "sf_frame_a", a) "testl $1, %%eax; jnz 4f;" OBFH_SF_CALL("2f") "jmp 5f; 4:" OBFH_SF_CALL("3f") "5:" OBFH_SF_EPILOGUE \
            OBFH_SF_ENTRY("2", "sf_frame_b", b) OBFH_SF_CALL("3f") OBFH_SF_EPILOGUE                                                             \
                OBFH_SF_ENTRY("3", "sf_frame_c", c) "leave; jmp 1b;"
#define OBFH_SF_LAYOUT_4(a, b, c)                                                                                                                                   \
    OBFH_SF_CALL("1f")                                                                                                                                              \
    OBFH_SF_EPILOGUE OBFH_SF_PADDING OBFH_SF_ENTRY("1", "sf_frame_a", a) OBFH_SF_CALL("2f") OBFH_SF_CALL("3f") OBFH_SF_EPILOGUE OBFH_SF_ENTRY("2", "sf_frame_b", b) \
    OBFH_SF_EPILOGUE OBFH_SF_ENTRY("3", "sf_frame_c", c) "testl $1, %%eax; jz 4f;" OBFH_SF_CALL("2b") "4:" OBFH_SF_EPILOGUE

#define OBFH_SF_LAYOUT_5(a, b, c) \
    OBFH_SF_CALL("1f")            \
    OBFH_SF_EPILOGUE OBFH_SF_PADDING OBFH_SF_ENTRY("1", "sf_frame_a", a) OBFH_SF_CALL("2f") OBFH_SF_EPILOGUE OBFH_SF_ENTRY("2", "sf_frame_b", b) OBFH_SF_CALL("3f") OBFH_SF_EPILOGUE OBFH_SF_ENTRY("3", "sf_frame_c", c) OBFH_SF_EPILOGUE

#define OBFH_SF_LAYOUT_6(a, b, c) \
    OBFH_SF_CALL("1f")            \
    OBFH_SF_EPILOGUE OBFH_SF_PADDING OBFH_SF_ENTRY("1", "sf_frame_a", a) "leave; jmp 2f;" OBFH_SF_ENTRY("2", "sf_frame_b", b) "leave; jmp 3f;" OBFH_SF_ENTRY("3", "sf_frame_c", c) OBFH_SF_EPILOGUE

#define OBFH_SF_LAYOUT_7(a, b, c) \
    OBFH_SF_CALL("1f")            \
    OBFH_SF_EPILOGUE OBFH_SF_PADDING OBFH_SF_ENTRY("1", "sf_frame_a", a) "testl %%edx, %%edx; jz 4f;" OBFH_SF_CALL("1b") "4:" OBFH_SF_CALL("2f") OBFH_SF_EPILOGUE OBFH_SF_ENTRY("2", "sf_frame_b", b) OBFH_SF_CALL("3f") OBFH_SF_EPILOGUE OBFH_SF_ENTRY("3", "sf_frame_c", c) OBFH_SF_EPILOGUE

#define OBFH_SF_LAYOUT_8(a, b, c) \
    "testl $1, %%eax; jz 4f;" OBFH_SF_CALL("1f") "jmp 5f; 4:" OBFH_SF_CALL("2f") "5:" OBFH_SF_EPILOGUE OBFH_SF_PADDING OBFH_SF_ENTRY("1", "sf_frame_a", a) OBFH_SF_CALL("3f") OBFH_SF_EPILOGUE OBFH_SF_ENTRY("2", "sf_frame_b", b) OBFH_SF_CALL("3f") OBFH_SF_EPILOGUE OBFH_SF_ENTRY("3", "sf_frame_c", c) OBFH_SF_EPILOGUE

#define OBFH_SF_LAYOUT_9(a, b, c) \
    OBFH_SF_CALL("1f")            \
    OBFH_SF_EPILOGUE OBFH_SF_PADDING OBFH_SF_ENTRY("1", "sf_frame_a", a) OBFH_SF_CALL("2f") OBFH_SF_EPILOGUE OBFH_SF_ENTRY("2", "sf_frame_b", b) "testl $1, %%eax; jnz 4f;" OBFH_SF_CALL("1b") "4:" OBFH_SF_CALL("3f") OBFH_SF_EPILOGUE OBFH_SF_ENTRY("3", "sf_frame_c", c) "leave; jmp 2b;"

#define OBFH_SF_LAYOUT_10(a, b, c, d)       \
    OBFH_SF_CALL("1f")                      \
    OBFH_SF_EPILOGUE                        \
    OBFH_SF_GAP                             \
    OBFH_SF_ENTRY("3", "sf_frame_c", c)     \
    OBFH_SF_EPILOGUE                        \
    OBFH_SF_GAP                             \
    OBFH_SF_ENTRY_ALT("2", "sf_frame_b", b) \
    OBFH_SF_CALL("3b")                      \
    OBFH_SF_EPILOGUE_ALT                    \
    OBFH_SF_GAP                             \
    OBFH_SF_ENTRY("1", "sf_frame_a", a)     \
    OBFH_SF_CALL("2b")                      \
    OBFH_SF_EPILOGUE

#define OBFH_SF_LAYOUT_11(a, b, c, d)       \
    OBFH_SF_CALL("1f")                      \
    OBFH_SF_EPILOGUE                        \
    OBFH_SF_GAP                             \
    OBFH_SF_ENTRY("2", "sf_frame_b", b)     \
    OBFH_SF_CALL("3f")                      \
    OBFH_SF_EPILOGUE                        \
    OBFH_SF_GAP                             \
    OBFH_SF_ENTRY_ALT("1", "sf_frame_a", a) \
    OBFH_SF_CALL("2b")                      \
    OBFH_SF_CALL("3f")                      \
    OBFH_SF_EPILOGUE_ALT                    \
    OBFH_SF_GAP                             \
    OBFH_SF_ENTRY("3", "sf_frame_c", c)     \
    OBFH_SF_EPILOGUE

#define OBFH_SF_LAYOUT_12(a, b, c, d)       \
    OBFH_SF_CALL("1f")                      \
    OBFH_SF_EPILOGUE                        \
    OBFH_SF_GAP                             \
    OBFH_SF_ENTRY("1", "sf_frame_a", a)     \
    OBFH_SF_CALL("2f")                      \
    OBFH_SF_CALL("8f")                      \
    OBFH_SF_EPILOGUE                        \
    OBFH_SF_GAP                             \
    OBFH_SF_ENTRY("2", "sf_frame_b", b)     \
    OBFH_SF_CALL("3f")                      \
    OBFH_SF_EPILOGUE                        \
    OBFH_SF_GAP                             \
    OBFH_SF_ENTRY_ALT("3", "sf_frame_c", c) \
    OBFH_SF_EPILOGUE_ALT                    \
    OBFH_SF_GAP                             \
    OBFH_SF_ENTRY("8", "sf_frame_d", d)     \
    OBFH_SF_EPILOGUE

#define OBFH_SF_LAYOUT_13(a, b, c, d)       \
    OBFH_SF_CALL("1f")                      \
    OBFH_SF_EPILOGUE                        \
    OBFH_SF_GAP                             \
    OBFH_SF_ENTRY("1", "sf_frame_a", a)     \
    OBFH_SF_CALL("2f")                      \
    OBFH_SF_EPILOGUE                        \
    OBFH_SF_GAP                             \
    OBFH_SF_ENTRY_ALT("2", "sf_frame_b", b) \
    OBFH_SF_CALL("3f")                      \
    OBFH_SF_EPILOGUE_ALT                    \
    OBFH_SF_GAP                             \
    OBFH_SF_ENTRY("3", "sf_frame_c", c)     \
    "leave; jmp 8f;" OBFH_SF_GAP            \
        OBFH_SF_ENTRY("8", "sf_frame_d", d) \
            OBFH_SF_EPILOGUE

#define OBFH_SF_LAYOUT_14(a, b, c, d)       \
    OBFH_SF_CALL("1f")                      \
    OBFH_SF_EPILOGUE                        \
    OBFH_SF_GAP                             \
    OBFH_SF_ENTRY("8", "sf_frame_d", d)     \
    OBFH_SF_CALL("1f")                      \
    OBFH_SF_EPILOGUE                        \
    OBFH_SF_GAP                             \
    OBFH_SF_ENTRY_ALT("3", "sf_frame_c", c) \
    OBFH_SF_EPILOGUE_ALT                    \
    OBFH_SF_GAP                             \
    OBFH_SF_ENTRY("1", "sf_frame_a", a)     \
    OBFH_SF_CALL("2f")                      \
    OBFH_SF_EPILOGUE                        \
    OBFH_SF_GAP                             \
    OBFH_SF_ENTRY("2", "sf_frame_b", b)     \
    OBFH_SF_CALL("3b")                      \
    OBFH_SF_CALL("8b")                      \
    OBFH_SF_EPILOGUE

#define OBFH_SF_LAYOUT_15(a, b, c, d)                                                                      \
    OBFH_SF_CALL("1f")                                                                                     \
    OBFH_SF_EPILOGUE                                                                                       \
    OBFH_SF_GAP                                                                                            \
    OBFH_SF_ENTRY_ALT("1", "sf_frame_a", a)                                                                \
    "testl $1, %%eax; jz 4f;" OBFH_SF_CALL("2f") "jmp 5f; 4:" OBFH_SF_CALL("8f") "5:" OBFH_SF_EPILOGUE_ALT \
        OBFH_SF_GAP                                                                                        \
            OBFH_SF_ENTRY("2", "sf_frame_b", b)                                                            \
                OBFH_SF_CALL("3f")                                                                         \
                    OBFH_SF_EPILOGUE                                                                       \
                        OBFH_SF_GAP                                                                        \
                            OBFH_SF_ENTRY("8", "sf_frame_d", d)                                            \
                                OBFH_SF_CALL("3f")                                                         \
                                    OBFH_SF_EPILOGUE                                                       \
                                        OBFH_SF_GAP                                                        \
                                            OBFH_SF_ENTRY("3", "sf_frame_c", c)                            \
                                                OBFH_SF_EPILOGUE

#define OBFH_SF_LAYOUT_16(a, b, c, d)                                       \
    OBFH_SF_CALL("1f")                                                      \
    OBFH_SF_EPILOGUE                                                        \
    OBFH_SF_GAP                                                             \
    OBFH_SF_ENTRY("1", "sf_frame_a", a)                                     \
    OBFH_SF_CALL("2f")                                                      \
    OBFH_SF_EPILOGUE                                                        \
    OBFH_SF_GAP                                                             \
    OBFH_SF_ENTRY("2", "sf_frame_b", b)                                     \
    "testl %%edx, %%edx; jz 4f;" OBFH_SF_CALL("2b") "4:" OBFH_SF_CALL("3f") \
        OBFH_SF_EPILOGUE                                                    \
            OBFH_SF_GAP                                                     \
                OBFH_SF_ENTRY_ALT("3", "sf_frame_c", c)                     \
                    OBFH_SF_EPILOGUE_ALT

#define OBFH_SF_LAYOUT_17(a, b, c, d)                                                                    \
    OBFH_SF_CALL("1f")                                                                                   \
    OBFH_SF_EPILOGUE                                                                                     \
    OBFH_SF_GAP                                                                                          \
    OBFH_SF_ENTRY("1", "sf_frame_a", a)                                                                  \
    "cmpl %%edx, %%eax; jb 4f;" OBFH_SF_CALL("2f") "jmp 5f; 4:" OBFH_SF_CALL("3f") "5:" OBFH_SF_EPILOGUE \
        OBFH_SF_GAP                                                                                      \
            OBFH_SF_ENTRY("2", "sf_frame_b", b) "leave; jmp 3f;" OBFH_SF_GAP                             \
                OBFH_SF_ENTRY_ALT("3", "sf_frame_c", c)                                                  \
                    OBFH_SF_CALL("1b")                                                                   \
                        OBFH_SF_EPILOGUE_ALT

#define OBFH_SF_LAYOUT_18(a, b, c, d)       \
    OBFH_SF_CALL("3f")                      \
    OBFH_SF_EPILOGUE                        \
    OBFH_SF_GAP                             \
    OBFH_SF_ENTRY("1", "sf_frame_a", a)     \
    OBFH_SF_EPILOGUE                        \
    OBFH_SF_GAP                             \
    OBFH_SF_ENTRY_ALT("2", "sf_frame_b", b) \
    OBFH_SF_CALL("1b")                      \
    OBFH_SF_EPILOGUE_ALT                    \
    OBFH_SF_GAP                             \
    OBFH_SF_ENTRY("3", "sf_frame_c", c)     \
    OBFH_SF_CALL("2b")                      \
    OBFH_SF_EPILOGUE

#define OBFH_SF_LAYOUT_19(a, b, c, d)                                                                  \
    "testl $1, %%eax; jz 4f;" OBFH_SF_CALL("1f") "jmp 5f; 4:" OBFH_SF_CALL("3f") "5:" OBFH_SF_EPILOGUE \
        OBFH_SF_GAP                                                                                    \
            OBFH_SF_ENTRY("1", "sf_frame_a", a)                                                        \
                OBFH_SF_CALL("2f")                                                                     \
                    OBFH_SF_EPILOGUE                                                                   \
                        OBFH_SF_GAP                                                                    \
                            OBFH_SF_ENTRY_ALT("2", "sf_frame_b", b)                                    \
    OBFH_SF_EPILOGUE_ALT                                                                               \
    OBFH_SF_GAP                                                                                        \
    OBFH_SF_ENTRY("3", "sf_frame_c", c)                                                                \
    OBFH_SF_CALL("2b")                                                                                 \
    OBFH_SF_EPILOGUE

#define OBFH_SF_LAYOUT_20(a, b, c, d)                                                                  \
    OBFH_SF_CALL("1f")                                                                                 \
    OBFH_SF_EPILOGUE                                                                                   \
    OBFH_SF_GAP                                                                                        \
    OBFH_SF_ENTRY("2", "sf_frame_b", b)                                                                \
    OBFH_SF_CALL("8f")                                                                                 \
    OBFH_SF_EPILOGUE                                                                                   \
    OBFH_SF_GAP                                                                                        \
    OBFH_SF_ENTRY_ALT("8", "sf_frame_d", d)                                                            \
    OBFH_SF_CALL("3f")                                                                                 \
    OBFH_SF_EPILOGUE_ALT                                                                               \
    OBFH_SF_GAP                                                                                        \
    OBFH_SF_ENTRY("1", "sf_frame_a", a)                                                                \
    "testl $1, %%eax; jz 4f;" OBFH_SF_CALL("2b") "jmp 5f; 4:" OBFH_SF_CALL("3f") "5:" OBFH_SF_EPILOGUE \
        OBFH_SF_GAP                                                                                    \
            OBFH_SF_ENTRY("3", "sf_frame_c", c)                                                        \
                OBFH_SF_CALL("1b")                                                                     \
                    OBFH_SF_EPILOGUE

#define OBFH_SF_LAYOUT_21(a, b, c, d)       \
    OBFH_SF_CALL("1f")                      \
    OBFH_SF_EPILOGUE                        \
    OBFH_SF_GAP                             \
    OBFH_SF_ENTRY("3", "sf_frame_c", c)     \
    OBFH_SF_EPILOGUE                        \
    OBFH_SF_GAP                             \
    OBFH_SF_ENTRY_ALT("8", "sf_frame_d", d) \
    OBFH_SF_CALL("3b")                      \
    OBFH_SF_EPILOGUE_ALT                    \
    OBFH_SF_GAP                             \
    OBFH_SF_ENTRY("2", "sf_frame_b", b)     \
    OBFH_SF_CALL("8b")                      \
    OBFH_SF_EPILOGUE                        \
    OBFH_SF_GAP                             \
    OBFH_SF_ENTRY("1", "sf_frame_a", a)     \
    OBFH_SF_CALL("2b")                      \
    OBFH_SF_CALL("3b")                      \
    OBFH_SF_EPILOGUE

// Skipped native helpers: register-only and scratch-stack forms share no RBP frame.
// Their arithmetic/opcodes vary per expansion; all stack edits follow the live guard.
#if defined(__x86_64__)
#define OBFH_SF_LEAF_ARGS "movl %%ecx, %%eax;"
#define OBFH_SF_LEAF_ENTER "subq $%c[sf_leaf_frame], %%rsp;"
#define OBFH_SF_LEAF_STORE "movl %%eax, %c[sf_leaf_slot](%%rsp);"
#define OBFH_SF_LEAF_LOAD "movl %c[sf_leaf_slot](%%rsp), %%ecx;"
#define OBFH_SF_LEAF_EXIT "addq $%c[sf_leaf_frame], %%rsp; ret;"
#else
#define OBFH_SF_LEAF_ARGS "movl 4(%%esp), %%eax; movl 8(%%esp), %%edx;"
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
    OBFH_SF_CALL("1f")                  \
    OBFH_SF_EPILOGUE OBFH_SF_GAP        \
        OBFH_SF_LEAF_ENTRY("2", c)      \
    OBFH_SF_GAP                         \
    OBFH_SF_ENTRY("1", "sf_frame_a", a) \
    OBFH_SF_CALL("2b")                  \
    OBFH_SF_EPILOGUE_ALT
#define OBFH_SF_LAYOUT_23(a, b, c, d) \
    OBFH_SF_CALL("1f")                \
    OBFH_SF_EPILOGUE_ALT OBFH_SF_GAP  \
        OBFH_SF_LEAF_ENTRY("1", c)    \
    OBFH_SF_GAP OBFH_SF_LEAF_ENTRY("2", d)
#define OBFH_SF_LAYOUT_24(a, b, c, d)                                                                                  \
    OBFH_SF_CALL("1f")                                                                                                 \
    OBFH_SF_EPILOGUE OBFH_SF_GAP                                                                                       \
        OBFH_SF_ENTRY_ALT("1", "sf_frame_a", a) OBFH_SF_CALL("3f") OBFH_SF_CALL("2f") OBFH_SF_EPILOGUE_ALT OBFH_SF_GAP \
            OBFH_SF_LEAF_ENTRY("2", c)                                                                                 \
    OBFH_SF_GAP OBFH_SF_LEAF_ENTRY("3", d)
#define OBFH_SF_LAYOUT_25(a, b, c, d)   \
    OBFH_SF_CALL("1f")                  \
    OBFH_SF_EPILOGUE_ALT OBFH_SF_GAP    \
        OBFH_SF_LEAF_ENTRY("3", d)      \
    OBFH_SF_GAP                         \
    OBFH_SF_ENTRY("1", "sf_frame_a", a) \
    OBFH_SF_CALL("2f")                  \
    OBFH_SF_EPILOGUE OBFH_SF_GAP        \
        OBFH_SF_ENTRY_ALT("2", "sf_frame_b", b) OBFH_SF_CALL("3b") OBFH_SF_EPILOGUE_ALT
#define OBFH_SF_LAYOUT_26(a, b, c, d)                                                     \
    OBFH_SF_CALL("1f")                                                                    \
    OBFH_SF_EPILOGUE OBFH_SF_GAP                                                          \
        OBFH_SF_LEAF_ENTRY("2", c)                                                        \
    OBFH_SF_GAP                                                                           \
    OBFH_SF_ENTRY_ALT("1", "sf_frame_a", a)                                               \
    "testl %%eax, %%eax; jz 4f;" OBFH_SF_CALL("2b") "4:" OBFH_SF_EPILOGUE_ALT OBFH_SF_GAP \
        OBFH_SF_LEAF_ENTRY("3", d)
#define OBFH_SF_LAYOUT_27(a, b, c, d)      \
    OBFH_SF_CALL("2f")                     \
    OBFH_SF_EPILOGUE_ALT OBFH_SF_GAP       \
        OBFH_SF_LEAF_ENTRY("1", c)         \
    OBFH_SF_GAP OBFH_SF_LEAF_ENTRY("3", d) \
    OBFH_SF_GAP                            \
    OBFH_SF_ENTRY("2", "sf_frame_b", b)    \
    OBFH_SF_CALL("1b")                     \
    OBFH_SF_CALL("3b")                     \
    OBFH_SF_EPILOGUE

#define OBFH_SF_ASM(guard, layout) \
    ({ enum { __obfh_sf_mask = OBFH_JUNK_WORD, __obfh_sf_alu_a = RND(0, 4), __obfh_sf_alu_b = RND(0, 6), __obfh_sf_shift = RND(0, 4) }; \
    __obfh_asm__(OBFH_SF_INPUT "xorl $%c[sf_salt], %%eax; roll $%c[sf_rotate], %%eax;" guard layout "9:" \
                 :                                                                                       \
                 : OBFH_SF_INPUTS                                                                        \
                 : "eax", "edx", "ecx", "cc", "memory"); })
#define OBFH_SF_VARIANT_COUNT 128u
#define OBFH_SF_GROUP_0(index) __builtin_choose_expr((index) <= 3u, __builtin_choose_expr((index) <= 1u, __builtin_choose_expr((index) <= 0u, ({ OBFH_SF_VARIANT_0; }), ({ OBFH_SF_VARIANT_1; })), __builtin_choose_expr((index) <= 2u, ({ OBFH_SF_VARIANT_2; }), ({ OBFH_SF_VARIANT_3; }))), __builtin_choose_expr((index) <= 5u, __builtin_choose_expr((index) <= 4u, ({ OBFH_SF_VARIANT_4; }), ({ OBFH_SF_VARIANT_5; })), __builtin_choose_expr((index) <= 6u, ({ OBFH_SF_VARIANT_6; }), ({ OBFH_SF_VARIANT_7; }))))
#define OBFH_SF_GROUP_1(index) __builtin_choose_expr((index) <= 11u, __builtin_choose_expr((index) <= 9u, __builtin_choose_expr((index) <= 8u, ({ OBFH_SF_VARIANT_8; }), ({ OBFH_SF_VARIANT_9; })), __builtin_choose_expr((index) <= 10u, ({ OBFH_SF_VARIANT_10; }), ({ OBFH_SF_VARIANT_11; }))), __builtin_choose_expr((index) <= 13u, __builtin_choose_expr((index) <= 12u, ({ OBFH_SF_VARIANT_12; }), ({ OBFH_SF_VARIANT_13; })), __builtin_choose_expr((index) <= 14u, ({ OBFH_SF_VARIANT_14; }), ({ OBFH_SF_VARIANT_15; }))))
#define OBFH_SF_GROUP_2(index) __builtin_choose_expr((index) <= 19u, __builtin_choose_expr((index) <= 17u, __builtin_choose_expr((index) <= 16u, ({ OBFH_SF_VARIANT_16; }), ({ OBFH_SF_VARIANT_17; })), __builtin_choose_expr((index) <= 18u, ({ OBFH_SF_VARIANT_18; }), ({ OBFH_SF_VARIANT_19; }))), __builtin_choose_expr((index) <= 21u, __builtin_choose_expr((index) <= 20u, ({ OBFH_SF_VARIANT_20; }), ({ OBFH_SF_VARIANT_21; })), __builtin_choose_expr((index) <= 22u, ({ OBFH_SF_VARIANT_22; }), ({ OBFH_SF_VARIANT_23; }))))
#define OBFH_SF_GROUP_3(index) __builtin_choose_expr((index) <= 27u, __builtin_choose_expr((index) <= 25u, __builtin_choose_expr((index) <= 24u, ({ OBFH_SF_VARIANT_24; }), ({ OBFH_SF_VARIANT_25; })), __builtin_choose_expr((index) <= 26u, ({ OBFH_SF_VARIANT_26; }), ({ OBFH_SF_VARIANT_27; }))), __builtin_choose_expr((index) <= 29u, __builtin_choose_expr((index) <= 28u, ({ OBFH_SF_VARIANT_28; }), ({ OBFH_SF_VARIANT_29; })), __builtin_choose_expr((index) <= 30u, ({ OBFH_SF_VARIANT_30; }), ({ OBFH_SF_VARIANT_31; }))))
#define OBFH_SF_GROUP_4(index) __builtin_choose_expr((index) <= 35u, __builtin_choose_expr((index) <= 33u, __builtin_choose_expr((index) <= 32u, ({ OBFH_SF_VARIANT_32; }), ({ OBFH_SF_VARIANT_33; })), __builtin_choose_expr((index) <= 34u, ({ OBFH_SF_VARIANT_34; }), ({ OBFH_SF_VARIANT_35; }))), __builtin_choose_expr((index) <= 37u, __builtin_choose_expr((index) <= 36u, ({ OBFH_SF_VARIANT_36; }), ({ OBFH_SF_VARIANT_37; })), __builtin_choose_expr((index) <= 38u, ({ OBFH_SF_VARIANT_38; }), ({ OBFH_SF_VARIANT_39; }))))
#define OBFH_SF_GROUP_5(index) __builtin_choose_expr((index) <= 43u, __builtin_choose_expr((index) <= 41u, __builtin_choose_expr((index) <= 40u, ({ OBFH_SF_VARIANT_40; }), ({ OBFH_SF_VARIANT_41; })), __builtin_choose_expr((index) <= 42u, ({ OBFH_SF_VARIANT_42; }), ({ OBFH_SF_VARIANT_43; }))), __builtin_choose_expr((index) <= 45u, __builtin_choose_expr((index) <= 44u, ({ OBFH_SF_VARIANT_44; }), ({ OBFH_SF_VARIANT_45; })), __builtin_choose_expr((index) <= 46u, ({ OBFH_SF_VARIANT_46; }), ({ OBFH_SF_VARIANT_47; }))))
#define OBFH_SF_GROUP_6(index) __builtin_choose_expr((index) <= 51u, __builtin_choose_expr((index) <= 49u, __builtin_choose_expr((index) <= 48u, ({ OBFH_SF_VARIANT_48; }), ({ OBFH_SF_VARIANT_49; })), __builtin_choose_expr((index) <= 50u, ({ OBFH_SF_VARIANT_50; }), ({ OBFH_SF_VARIANT_51; }))), __builtin_choose_expr((index) <= 53u, __builtin_choose_expr((index) <= 52u, ({ OBFH_SF_VARIANT_52; }), ({ OBFH_SF_VARIANT_53; })), __builtin_choose_expr((index) <= 54u, ({ OBFH_SF_VARIANT_54; }), ({ OBFH_SF_VARIANT_55; }))))
#define OBFH_SF_GROUP_7(index) __builtin_choose_expr((index) <= 59u, __builtin_choose_expr((index) <= 57u, __builtin_choose_expr((index) <= 56u, ({ OBFH_SF_VARIANT_56; }), ({ OBFH_SF_VARIANT_57; })), __builtin_choose_expr((index) <= 58u, ({ OBFH_SF_VARIANT_58; }), ({ OBFH_SF_VARIANT_59; }))), __builtin_choose_expr((index) <= 61u, __builtin_choose_expr((index) <= 60u, ({ OBFH_SF_VARIANT_60; }), ({ OBFH_SF_VARIANT_61; })), __builtin_choose_expr((index) <= 62u, ({ OBFH_SF_VARIANT_62; }), ({ OBFH_SF_VARIANT_63; }))))
#define OBFH_SF_GROUP_8(index) __builtin_choose_expr((index) <= 67u, __builtin_choose_expr((index) <= 65u, __builtin_choose_expr((index) <= 64u, ({ OBFH_SF_VARIANT_64; }), ({ OBFH_SF_VARIANT_65; })), __builtin_choose_expr((index) <= 66u, ({ OBFH_SF_VARIANT_66; }), ({ OBFH_SF_VARIANT_67; }))), __builtin_choose_expr((index) <= 69u, __builtin_choose_expr((index) <= 68u, ({ OBFH_SF_VARIANT_68; }), ({ OBFH_SF_VARIANT_69; })), __builtin_choose_expr((index) <= 70u, ({ OBFH_SF_VARIANT_70; }), ({ OBFH_SF_VARIANT_71; }))))
#define OBFH_SF_GROUP_9(index) __builtin_choose_expr((index) <= 75u, __builtin_choose_expr((index) <= 73u, __builtin_choose_expr((index) <= 72u, ({ OBFH_SF_VARIANT_72; }), ({ OBFH_SF_VARIANT_73; })), __builtin_choose_expr((index) <= 74u, ({ OBFH_SF_VARIANT_74; }), ({ OBFH_SF_VARIANT_75; }))), __builtin_choose_expr((index) <= 77u, __builtin_choose_expr((index) <= 76u, ({ OBFH_SF_VARIANT_76; }), ({ OBFH_SF_VARIANT_77; })), __builtin_choose_expr((index) <= 78u, ({ OBFH_SF_VARIANT_78; }), ({ OBFH_SF_VARIANT_79; }))))
#define OBFH_SF_GROUP_10(index) __builtin_choose_expr((index) <= 83u, __builtin_choose_expr((index) <= 81u, __builtin_choose_expr((index) <= 80u, ({ OBFH_SF_VARIANT_80; }), ({ OBFH_SF_VARIANT_81; })), __builtin_choose_expr((index) <= 82u, ({ OBFH_SF_VARIANT_82; }), ({ OBFH_SF_VARIANT_83; }))), __builtin_choose_expr((index) <= 85u, __builtin_choose_expr((index) <= 84u, ({ OBFH_SF_VARIANT_84; }), ({ OBFH_SF_VARIANT_85; })), __builtin_choose_expr((index) <= 86u, ({ OBFH_SF_VARIANT_86; }), ({ OBFH_SF_VARIANT_87; }))))
#define OBFH_SF_GROUP_11(index) __builtin_choose_expr((index) <= 91u, __builtin_choose_expr((index) <= 89u, __builtin_choose_expr((index) <= 88u, ({ OBFH_SF_VARIANT_88; }), ({ OBFH_SF_VARIANT_89; })), __builtin_choose_expr((index) <= 90u, ({ OBFH_SF_VARIANT_90; }), ({ OBFH_SF_VARIANT_91; }))), __builtin_choose_expr((index) <= 93u, __builtin_choose_expr((index) <= 92u, ({ OBFH_SF_VARIANT_92; }), ({ OBFH_SF_VARIANT_93; })), __builtin_choose_expr((index) <= 94u, ({ OBFH_SF_VARIANT_94; }), ({ OBFH_SF_VARIANT_95; }))))
#define OBFH_SF_GROUP_12(index) __builtin_choose_expr((index) <= 99u, __builtin_choose_expr((index) <= 97u, __builtin_choose_expr((index) <= 96u, ({ OBFH_SF_VARIANT_96; }), ({ OBFH_SF_VARIANT_97; })), __builtin_choose_expr((index) <= 98u, ({ OBFH_SF_VARIANT_98; }), ({ OBFH_SF_VARIANT_99; }))), __builtin_choose_expr((index) <= 101u, __builtin_choose_expr((index) <= 100u, ({ OBFH_SF_VARIANT_100; }), ({ OBFH_SF_VARIANT_101; })), __builtin_choose_expr((index) <= 102u, ({ OBFH_SF_VARIANT_102; }), ({ OBFH_SF_VARIANT_103; }))))
#define OBFH_SF_GROUP_13(index) __builtin_choose_expr((index) <= 107u, __builtin_choose_expr((index) <= 105u, __builtin_choose_expr((index) <= 104u, ({ OBFH_SF_VARIANT_104; }), ({ OBFH_SF_VARIANT_105; })), __builtin_choose_expr((index) <= 106u, ({ OBFH_SF_VARIANT_106; }), ({ OBFH_SF_VARIANT_107; }))), __builtin_choose_expr((index) <= 109u, __builtin_choose_expr((index) <= 108u, ({ OBFH_SF_VARIANT_108; }), ({ OBFH_SF_VARIANT_109; })), __builtin_choose_expr((index) <= 110u, ({ OBFH_SF_VARIANT_110; }), ({ OBFH_SF_VARIANT_111; }))))
#define OBFH_SF_GROUP_14(index) __builtin_choose_expr((index) <= 115u, __builtin_choose_expr((index) <= 113u, __builtin_choose_expr((index) <= 112u, ({ OBFH_SF_VARIANT_112; }), ({ OBFH_SF_VARIANT_113; })), __builtin_choose_expr((index) <= 114u, ({ OBFH_SF_VARIANT_114; }), ({ OBFH_SF_VARIANT_115; }))), __builtin_choose_expr((index) <= 117u, __builtin_choose_expr((index) <= 116u, ({ OBFH_SF_VARIANT_116; }), ({ OBFH_SF_VARIANT_117; })), __builtin_choose_expr((index) <= 118u, ({ OBFH_SF_VARIANT_118; }), ({ OBFH_SF_VARIANT_119; }))))
#define OBFH_SF_GROUP_15(index) __builtin_choose_expr((index) <= 123u, __builtin_choose_expr((index) <= 121u, __builtin_choose_expr((index) <= 120u, ({ OBFH_SF_VARIANT_120; }), ({ OBFH_SF_VARIANT_121; })), __builtin_choose_expr((index) <= 122u, ({ OBFH_SF_VARIANT_122; }), ({ OBFH_SF_VARIANT_123; }))), __builtin_choose_expr((index) <= 125u, __builtin_choose_expr((index) <= 124u, ({ OBFH_SF_VARIANT_124; }), ({ OBFH_SF_VARIANT_125; })), __builtin_choose_expr((index) <= 126u, ({ OBFH_SF_VARIANT_126; }), ({ OBFH_SF_VARIANT_127; }))))
#define OBFH_SF_SELECT(index) __builtin_choose_expr((index) <= 63u, __builtin_choose_expr((index) <= 31u, __builtin_choose_expr((index) <= 15u, __builtin_choose_expr((index) <= 7u, OBFH_SF_GROUP_0(index), OBFH_SF_GROUP_1(index)), __builtin_choose_expr((index) <= 23u, OBFH_SF_GROUP_2(index), OBFH_SF_GROUP_3(index))), __builtin_choose_expr((index) <= 47u, __builtin_choose_expr((index) <= 39u, OBFH_SF_GROUP_4(index), OBFH_SF_GROUP_5(index)), __builtin_choose_expr((index) <= 55u, OBFH_SF_GROUP_6(index), OBFH_SF_GROUP_7(index)))), __builtin_choose_expr((index) <= 95u, __builtin_choose_expr((index) <= 79u, __builtin_choose_expr((index) <= 71u, OBFH_SF_GROUP_8(index), OBFH_SF_GROUP_9(index)), __builtin_choose_expr((index) <= 87u, OBFH_SF_GROUP_10(index), OBFH_SF_GROUP_11(index))), __builtin_choose_expr((index) <= 111u, __builtin_choose_expr((index) <= 103u, OBFH_SF_GROUP_12(index), OBFH_SF_GROUP_13(index)), __builtin_choose_expr((index) <= 119u, OBFH_SF_GROUP_14(index), OBFH_SF_GROUP_15(index)))))

#define OBFH_SF_VARIANT_0 OBFH_SF_ASM(OBFH_SF_GUARD_0, OBFH_SF_LAYOUT_0(OBFH_SF_BODY_A, OBFH_SF_BODY_B, OBFH_SF_BODY_C))
#define OBFH_SF_VARIANT_1 OBFH_SF_ASM(OBFH_SF_GUARD_0, OBFH_SF_LAYOUT_1(OBFH_SF_BODY_B, OBFH_SF_BODY_C, OBFH_SF_BODY_D))
#define OBFH_SF_VARIANT_2 OBFH_SF_ASM(OBFH_SF_GUARD_0, OBFH_SF_LAYOUT_2(OBFH_SF_BODY_C, OBFH_SF_BODY_D, OBFH_SF_BODY_A))
#define OBFH_SF_VARIANT_3 OBFH_SF_ASM(OBFH_SF_GUARD_0, OBFH_SF_LAYOUT_3(OBFH_SF_BODY_D, OBFH_SF_BODY_A, OBFH_SF_BODY_B))
#define OBFH_SF_VARIANT_4 OBFH_SF_ASM(OBFH_SF_GUARD_1, OBFH_SF_LAYOUT_0(OBFH_SF_BODY_A, OBFH_SF_BODY_B, OBFH_SF_BODY_C))
#define OBFH_SF_VARIANT_5 OBFH_SF_ASM(OBFH_SF_GUARD_1, OBFH_SF_LAYOUT_1(OBFH_SF_BODY_B, OBFH_SF_BODY_C, OBFH_SF_BODY_D))
#define OBFH_SF_VARIANT_6 OBFH_SF_ASM(OBFH_SF_GUARD_1, OBFH_SF_LAYOUT_2(OBFH_SF_BODY_C, OBFH_SF_BODY_D, OBFH_SF_BODY_A))
#define OBFH_SF_VARIANT_7 OBFH_SF_ASM(OBFH_SF_GUARD_1, OBFH_SF_LAYOUT_3(OBFH_SF_BODY_D, OBFH_SF_BODY_A, OBFH_SF_BODY_B))
#define OBFH_SF_VARIANT_8 OBFH_SF_ASM(OBFH_SF_GUARD_2, OBFH_SF_LAYOUT_0(OBFH_SF_BODY_A, OBFH_SF_BODY_B, OBFH_SF_BODY_C))
#define OBFH_SF_VARIANT_9 OBFH_SF_ASM(OBFH_SF_GUARD_2, OBFH_SF_LAYOUT_1(OBFH_SF_BODY_B, OBFH_SF_BODY_C, OBFH_SF_BODY_D))
#define OBFH_SF_VARIANT_10 OBFH_SF_ASM(OBFH_SF_GUARD_2, OBFH_SF_LAYOUT_2(OBFH_SF_BODY_C, OBFH_SF_BODY_D, OBFH_SF_BODY_A))
#define OBFH_SF_VARIANT_11 OBFH_SF_ASM(OBFH_SF_GUARD_2, OBFH_SF_LAYOUT_3(OBFH_SF_BODY_D, OBFH_SF_BODY_A, OBFH_SF_BODY_B))
#define OBFH_SF_VARIANT_12 OBFH_SF_ASM(OBFH_SF_GUARD_3, OBFH_SF_LAYOUT_0(OBFH_SF_BODY_A, OBFH_SF_BODY_B, OBFH_SF_BODY_C))
#define OBFH_SF_VARIANT_13 OBFH_SF_ASM(OBFH_SF_GUARD_3, OBFH_SF_LAYOUT_1(OBFH_SF_BODY_B, OBFH_SF_BODY_C, OBFH_SF_BODY_D))
#define OBFH_SF_VARIANT_14 OBFH_SF_ASM(OBFH_SF_GUARD_3, OBFH_SF_LAYOUT_2(OBFH_SF_BODY_C, OBFH_SF_BODY_D, OBFH_SF_BODY_A))
#define OBFH_SF_VARIANT_15 OBFH_SF_ASM(OBFH_SF_GUARD_3, OBFH_SF_LAYOUT_3(OBFH_SF_BODY_D, OBFH_SF_BODY_A, OBFH_SF_BODY_B))
#define OBFH_SF_VARIANT_16 OBFH_SF_ASM(OBFH_SF_GUARD_4, OBFH_SF_LAYOUT_4(OBFH_SF_BODY_E, OBFH_SF_BODY_J, OBFH_SF_BODY_B))
#define OBFH_SF_VARIANT_17 OBFH_SF_ASM(OBFH_SF_GUARD_4, OBFH_SF_LAYOUT_5(OBFH_SF_BODY_F, OBFH_SF_BODY_K, OBFH_SF_BODY_C))
#define OBFH_SF_VARIANT_18 OBFH_SF_ASM(OBFH_SF_GUARD_4, OBFH_SF_LAYOUT_6(OBFH_SF_BODY_G, OBFH_SF_BODY_L, OBFH_SF_BODY_D))
#define OBFH_SF_VARIANT_19 OBFH_SF_ASM(OBFH_SF_GUARD_4, OBFH_SF_LAYOUT_7(OBFH_SF_BODY_H, OBFH_SF_BODY_A, OBFH_SF_BODY_E))
#define OBFH_SF_VARIANT_20 OBFH_SF_ASM(OBFH_SF_GUARD_4, OBFH_SF_LAYOUT_8(OBFH_SF_BODY_I, OBFH_SF_BODY_B, OBFH_SF_BODY_F))
#define OBFH_SF_VARIANT_21 OBFH_SF_ASM(OBFH_SF_GUARD_4, OBFH_SF_LAYOUT_9(OBFH_SF_BODY_J, OBFH_SF_BODY_C, OBFH_SF_BODY_G))
#define OBFH_SF_VARIANT_22 OBFH_SF_ASM(OBFH_SF_GUARD_5, OBFH_SF_LAYOUT_4(OBFH_SF_BODY_K, OBFH_SF_BODY_D, OBFH_SF_BODY_H))
#define OBFH_SF_VARIANT_23 OBFH_SF_ASM(OBFH_SF_GUARD_5, OBFH_SF_LAYOUT_5(OBFH_SF_BODY_L, OBFH_SF_BODY_E, OBFH_SF_BODY_I))
#define OBFH_SF_VARIANT_24 OBFH_SF_ASM(OBFH_SF_GUARD_5, OBFH_SF_LAYOUT_6(OBFH_SF_BODY_A, OBFH_SF_BODY_F, OBFH_SF_BODY_J))
#define OBFH_SF_VARIANT_25 OBFH_SF_ASM(OBFH_SF_GUARD_5, OBFH_SF_LAYOUT_7(OBFH_SF_BODY_B, OBFH_SF_BODY_G, OBFH_SF_BODY_K))
#define OBFH_SF_VARIANT_26 OBFH_SF_ASM(OBFH_SF_GUARD_5, OBFH_SF_LAYOUT_8(OBFH_SF_BODY_C, OBFH_SF_BODY_H, OBFH_SF_BODY_L))
#define OBFH_SF_VARIANT_27 OBFH_SF_ASM(OBFH_SF_GUARD_5, OBFH_SF_LAYOUT_9(OBFH_SF_BODY_D, OBFH_SF_BODY_I, OBFH_SF_BODY_A))
#define OBFH_SF_VARIANT_28 OBFH_SF_ASM(OBFH_SF_GUARD_6, OBFH_SF_LAYOUT_4(OBFH_SF_BODY_E, OBFH_SF_BODY_J, OBFH_SF_BODY_B))
#define OBFH_SF_VARIANT_29 OBFH_SF_ASM(OBFH_SF_GUARD_6, OBFH_SF_LAYOUT_5(OBFH_SF_BODY_F, OBFH_SF_BODY_K, OBFH_SF_BODY_C))
#define OBFH_SF_VARIANT_30 OBFH_SF_ASM(OBFH_SF_GUARD_6, OBFH_SF_LAYOUT_6(OBFH_SF_BODY_G, OBFH_SF_BODY_L, OBFH_SF_BODY_D))
#define OBFH_SF_VARIANT_31 OBFH_SF_ASM(OBFH_SF_GUARD_6, OBFH_SF_LAYOUT_7(OBFH_SF_BODY_H, OBFH_SF_BODY_A, OBFH_SF_BODY_E))
#define OBFH_SF_VARIANT_32 OBFH_SF_ASM(OBFH_SF_GUARD_6, OBFH_SF_LAYOUT_8(OBFH_SF_BODY_I, OBFH_SF_BODY_B, OBFH_SF_BODY_F))
#define OBFH_SF_VARIANT_33 OBFH_SF_ASM(OBFH_SF_GUARD_6, OBFH_SF_LAYOUT_9(OBFH_SF_BODY_J, OBFH_SF_BODY_C, OBFH_SF_BODY_G))
#define OBFH_SF_VARIANT_34 OBFH_SF_ASM(OBFH_SF_GUARD_7, OBFH_SF_LAYOUT_4(OBFH_SF_BODY_K, OBFH_SF_BODY_D, OBFH_SF_BODY_H))
#define OBFH_SF_VARIANT_35 OBFH_SF_ASM(OBFH_SF_GUARD_7, OBFH_SF_LAYOUT_5(OBFH_SF_BODY_L, OBFH_SF_BODY_E, OBFH_SF_BODY_I))
#define OBFH_SF_VARIANT_36 OBFH_SF_ASM(OBFH_SF_GUARD_7, OBFH_SF_LAYOUT_6(OBFH_SF_BODY_A, OBFH_SF_BODY_F, OBFH_SF_BODY_J))
#define OBFH_SF_VARIANT_37 OBFH_SF_ASM(OBFH_SF_GUARD_7, OBFH_SF_LAYOUT_7(OBFH_SF_BODY_B, OBFH_SF_BODY_G, OBFH_SF_BODY_K))
#define OBFH_SF_VARIANT_38 OBFH_SF_ASM(OBFH_SF_GUARD_7, OBFH_SF_LAYOUT_8(OBFH_SF_BODY_C, OBFH_SF_BODY_H, OBFH_SF_BODY_L))
#define OBFH_SF_VARIANT_39 OBFH_SF_ASM(OBFH_SF_GUARD_7, OBFH_SF_LAYOUT_9(OBFH_SF_BODY_D, OBFH_SF_BODY_I, OBFH_SF_BODY_A))
#define OBFH_SF_VARIANT_40 OBFH_SF_ASM(OBFH_SF_GUARD_8, OBFH_SF_LAYOUT_4(OBFH_SF_BODY_E, OBFH_SF_BODY_J, OBFH_SF_BODY_B))
#define OBFH_SF_VARIANT_41 OBFH_SF_ASM(OBFH_SF_GUARD_8, OBFH_SF_LAYOUT_5(OBFH_SF_BODY_F, OBFH_SF_BODY_K, OBFH_SF_BODY_C))
#define OBFH_SF_VARIANT_42 OBFH_SF_ASM(OBFH_SF_GUARD_8, OBFH_SF_LAYOUT_6(OBFH_SF_BODY_G, OBFH_SF_BODY_L, OBFH_SF_BODY_D))
#define OBFH_SF_VARIANT_43 OBFH_SF_ASM(OBFH_SF_GUARD_8, OBFH_SF_LAYOUT_7(OBFH_SF_BODY_H, OBFH_SF_BODY_A, OBFH_SF_BODY_E))
#define OBFH_SF_VARIANT_44 OBFH_SF_ASM(OBFH_SF_GUARD_8, OBFH_SF_LAYOUT_8(OBFH_SF_BODY_I, OBFH_SF_BODY_B, OBFH_SF_BODY_F))
#define OBFH_SF_VARIANT_45 OBFH_SF_ASM(OBFH_SF_GUARD_8, OBFH_SF_LAYOUT_9(OBFH_SF_BODY_J, OBFH_SF_BODY_C, OBFH_SF_BODY_G))
#define OBFH_SF_VARIANT_46 OBFH_SF_ASM(OBFH_SF_GUARD_0, OBFH_SF_LAYOUT_22(OBFH_SF_BODY_A, OBFH_SF_BODY_H, OBFH_SF_LEAF_A, OBFH_SF_LEAF_E))
#define OBFH_SF_VARIANT_47 OBFH_SF_ASM(OBFH_SF_GUARD_0, OBFH_SF_LAYOUT_23(OBFH_SF_BODY_D, OBFH_SF_BODY_K, OBFH_SF_LEAF_F, OBFH_SF_LEAF_L))
#define OBFH_SF_VARIANT_48 OBFH_SF_ASM(OBFH_SF_GUARD_0, OBFH_SF_LAYOUT_24(OBFH_SF_BODY_G, OBFH_SF_BODY_N, OBFH_SF_LEAF_K, OBFH_SF_LEAF_G))
#define OBFH_SF_VARIANT_49 OBFH_SF_ASM(OBFH_SF_GUARD_0, OBFH_SF_LAYOUT_25(OBFH_SF_BODY_J, OBFH_SF_BODY_Q, OBFH_SF_LEAF_D, OBFH_SF_LEAF_C))
#define OBFH_SF_VARIANT_50 OBFH_SF_ASM(OBFH_SF_GUARD_0, OBFH_SF_LAYOUT_26(OBFH_SF_BODY_M, OBFH_SF_BODY_T, OBFH_SF_LEAF_I, OBFH_SF_LEAF_J))
#define OBFH_SF_VARIANT_51 OBFH_SF_ASM(OBFH_SF_GUARD_0, OBFH_SF_LAYOUT_27(OBFH_SF_BODY_P, OBFH_SF_BODY_C, OBFH_SF_LEAF_B, OBFH_SF_LEAF_E))
#define OBFH_SF_VARIANT_52 OBFH_SF_ASM(OBFH_SF_GUARD_1, OBFH_SF_LAYOUT_22(OBFH_SF_BODY_S, OBFH_SF_BODY_F, OBFH_SF_LEAF_H, OBFH_SF_LEAF_A))
#define OBFH_SF_VARIANT_53 OBFH_SF_ASM(OBFH_SF_GUARD_1, OBFH_SF_LAYOUT_23(OBFH_SF_BODY_B, OBFH_SF_BODY_I, OBFH_SF_LEAF_A, OBFH_SF_LEAF_H))
#define OBFH_SF_VARIANT_54 OBFH_SF_ASM(OBFH_SF_GUARD_1, OBFH_SF_LAYOUT_24(OBFH_SF_BODY_E, OBFH_SF_BODY_L, OBFH_SF_LEAF_F, OBFH_SF_LEAF_C))
#define OBFH_SF_VARIANT_55 OBFH_SF_ASM(OBFH_SF_GUARD_1, OBFH_SF_LAYOUT_25(OBFH_SF_BODY_H, OBFH_SF_BODY_O, OBFH_SF_LEAF_K, OBFH_SF_LEAF_K))
#define OBFH_SF_VARIANT_56 OBFH_SF_ASM(OBFH_SF_GUARD_1, OBFH_SF_LAYOUT_26(OBFH_SF_BODY_K, OBFH_SF_BODY_R, OBFH_SF_LEAF_D, OBFH_SF_LEAF_F))
#define OBFH_SF_VARIANT_57 OBFH_SF_ASM(OBFH_SF_GUARD_1, OBFH_SF_LAYOUT_27(OBFH_SF_BODY_N, OBFH_SF_BODY_A, OBFH_SF_LEAF_I, OBFH_SF_LEAF_A))
#define OBFH_SF_VARIANT_58 OBFH_SF_ASM(OBFH_SF_GUARD_2, OBFH_SF_LAYOUT_22(OBFH_SF_BODY_Q, OBFH_SF_BODY_D, OBFH_SF_LEAF_C, OBFH_SF_LEAF_I))
#define OBFH_SF_VARIANT_59 OBFH_SF_ASM(OBFH_SF_GUARD_2, OBFH_SF_LAYOUT_23(OBFH_SF_BODY_T, OBFH_SF_BODY_G, OBFH_SF_LEAF_H, OBFH_SF_LEAF_D))
#define OBFH_SF_VARIANT_60 OBFH_SF_ASM(OBFH_SF_GUARD_2, OBFH_SF_LAYOUT_24(OBFH_SF_BODY_C, OBFH_SF_BODY_J, OBFH_SF_LEAF_A, OBFH_SF_LEAF_K))
#define OBFH_SF_VARIANT_61 OBFH_SF_ASM(OBFH_SF_GUARD_2, OBFH_SF_LAYOUT_25(OBFH_SF_BODY_F, OBFH_SF_BODY_M, OBFH_SF_LEAF_F, OBFH_SF_LEAF_G))
#define OBFH_SF_VARIANT_62 OBFH_SF_ASM(OBFH_SF_GUARD_2, OBFH_SF_LAYOUT_26(OBFH_SF_BODY_I, OBFH_SF_BODY_P, OBFH_SF_LEAF_K, OBFH_SF_LEAF_B))
#define OBFH_SF_VARIANT_63 OBFH_SF_ASM(OBFH_SF_GUARD_2, OBFH_SF_LAYOUT_27(OBFH_SF_BODY_L, OBFH_SF_BODY_S, OBFH_SF_LEAF_D, OBFH_SF_LEAF_I))
#define OBFH_SF_VARIANT_64 OBFH_SF_ASM(OBFH_SF_GUARD_3, OBFH_SF_LAYOUT_22(OBFH_SF_BODY_O, OBFH_SF_BODY_B, OBFH_SF_LEAF_J, OBFH_SF_LEAF_E))
#define OBFH_SF_VARIANT_65 OBFH_SF_ASM(OBFH_SF_GUARD_3, OBFH_SF_LAYOUT_23(OBFH_SF_BODY_R, OBFH_SF_BODY_E, OBFH_SF_LEAF_C, OBFH_SF_LEAF_L))
#define OBFH_SF_VARIANT_66 OBFH_SF_ASM(OBFH_SF_GUARD_3, OBFH_SF_LAYOUT_24(OBFH_SF_BODY_A, OBFH_SF_BODY_H, OBFH_SF_LEAF_H, OBFH_SF_LEAF_G))
#define OBFH_SF_VARIANT_67 OBFH_SF_ASM(OBFH_SF_GUARD_3, OBFH_SF_LAYOUT_25(OBFH_SF_BODY_D, OBFH_SF_BODY_K, OBFH_SF_LEAF_A, OBFH_SF_LEAF_C))
#define OBFH_SF_VARIANT_68 OBFH_SF_ASM(OBFH_SF_GUARD_3, OBFH_SF_LAYOUT_26(OBFH_SF_BODY_G, OBFH_SF_BODY_N, OBFH_SF_LEAF_F, OBFH_SF_LEAF_J))
#define OBFH_SF_VARIANT_69 OBFH_SF_ASM(OBFH_SF_GUARD_3, OBFH_SF_LAYOUT_27(OBFH_SF_BODY_J, OBFH_SF_BODY_Q, OBFH_SF_LEAF_K, OBFH_SF_LEAF_E))
#define OBFH_SF_VARIANT_70 OBFH_SF_ASM(OBFH_SF_GUARD_4, OBFH_SF_LAYOUT_22(OBFH_SF_BODY_M, OBFH_SF_BODY_T, OBFH_SF_LEAF_E, OBFH_SF_LEAF_A))
#define OBFH_SF_VARIANT_71 OBFH_SF_ASM(OBFH_SF_GUARD_4, OBFH_SF_LAYOUT_23(OBFH_SF_BODY_P, OBFH_SF_BODY_C, OBFH_SF_LEAF_J, OBFH_SF_LEAF_H))
#define OBFH_SF_VARIANT_72 OBFH_SF_ASM(OBFH_SF_GUARD_4, OBFH_SF_LAYOUT_24(OBFH_SF_BODY_S, OBFH_SF_BODY_F, OBFH_SF_LEAF_C, OBFH_SF_LEAF_C))
#define OBFH_SF_VARIANT_73 OBFH_SF_ASM(OBFH_SF_GUARD_4, OBFH_SF_LAYOUT_25(OBFH_SF_BODY_B, OBFH_SF_BODY_I, OBFH_SF_LEAF_H, OBFH_SF_LEAF_K))
#define OBFH_SF_VARIANT_74 OBFH_SF_ASM(OBFH_SF_GUARD_4, OBFH_SF_LAYOUT_26(OBFH_SF_BODY_E, OBFH_SF_BODY_L, OBFH_SF_LEAF_A, OBFH_SF_LEAF_F))
#define OBFH_SF_VARIANT_75 OBFH_SF_ASM(OBFH_SF_GUARD_4, OBFH_SF_LAYOUT_27(OBFH_SF_BODY_H, OBFH_SF_BODY_O, OBFH_SF_LEAF_F, OBFH_SF_LEAF_A))
#define OBFH_SF_VARIANT_76 OBFH_SF_ASM(OBFH_SF_GUARD_5, OBFH_SF_LAYOUT_22(OBFH_SF_BODY_K, OBFH_SF_BODY_R, OBFH_SF_LEAF_L, OBFH_SF_LEAF_I))
#define OBFH_SF_VARIANT_77 OBFH_SF_ASM(OBFH_SF_GUARD_5, OBFH_SF_LAYOUT_23(OBFH_SF_BODY_N, OBFH_SF_BODY_A, OBFH_SF_LEAF_E, OBFH_SF_LEAF_D))
#define OBFH_SF_VARIANT_78 OBFH_SF_ASM(OBFH_SF_GUARD_5, OBFH_SF_LAYOUT_24(OBFH_SF_BODY_Q, OBFH_SF_BODY_D, OBFH_SF_LEAF_J, OBFH_SF_LEAF_K))
#define OBFH_SF_VARIANT_79 OBFH_SF_ASM(OBFH_SF_GUARD_5, OBFH_SF_LAYOUT_25(OBFH_SF_BODY_T, OBFH_SF_BODY_G, OBFH_SF_LEAF_C, OBFH_SF_LEAF_G))
#define OBFH_SF_VARIANT_80 OBFH_SF_ASM(OBFH_SF_GUARD_5, OBFH_SF_LAYOUT_26(OBFH_SF_BODY_C, OBFH_SF_BODY_J, OBFH_SF_LEAF_H, OBFH_SF_LEAF_B))
#define OBFH_SF_VARIANT_81 OBFH_SF_ASM(OBFH_SF_GUARD_5, OBFH_SF_LAYOUT_27(OBFH_SF_BODY_F, OBFH_SF_BODY_M, OBFH_SF_LEAF_A, OBFH_SF_LEAF_I))
#define OBFH_SF_VARIANT_82 OBFH_SF_ASM(OBFH_SF_GUARD_6, OBFH_SF_LAYOUT_22(OBFH_SF_BODY_I, OBFH_SF_BODY_P, OBFH_SF_LEAF_G, OBFH_SF_LEAF_E))
#define OBFH_SF_VARIANT_83 OBFH_SF_ASM(OBFH_SF_GUARD_6, OBFH_SF_LAYOUT_23(OBFH_SF_BODY_L, OBFH_SF_BODY_S, OBFH_SF_LEAF_L, OBFH_SF_LEAF_L))
#define OBFH_SF_VARIANT_84 OBFH_SF_ASM(OBFH_SF_GUARD_6, OBFH_SF_LAYOUT_24(OBFH_SF_BODY_O, OBFH_SF_BODY_B, OBFH_SF_LEAF_E, OBFH_SF_LEAF_G))
#define OBFH_SF_VARIANT_85 OBFH_SF_ASM(OBFH_SF_GUARD_6, OBFH_SF_LAYOUT_25(OBFH_SF_BODY_R, OBFH_SF_BODY_E, OBFH_SF_LEAF_J, OBFH_SF_LEAF_C))
#define OBFH_SF_VARIANT_86 OBFH_SF_ASM(OBFH_SF_GUARD_6, OBFH_SF_LAYOUT_26(OBFH_SF_BODY_A, OBFH_SF_BODY_H, OBFH_SF_LEAF_C, OBFH_SF_LEAF_J))
#define OBFH_SF_VARIANT_87 OBFH_SF_ASM(OBFH_SF_GUARD_6, OBFH_SF_LAYOUT_27(OBFH_SF_BODY_D, OBFH_SF_BODY_K, OBFH_SF_LEAF_H, OBFH_SF_LEAF_E))
#define OBFH_SF_VARIANT_88 OBFH_SF_ASM(OBFH_SF_GUARD_7, OBFH_SF_LAYOUT_22(OBFH_SF_BODY_G, OBFH_SF_BODY_N, OBFH_SF_LEAF_B, OBFH_SF_LEAF_A))
#define OBFH_SF_VARIANT_89 OBFH_SF_ASM(OBFH_SF_GUARD_7, OBFH_SF_LAYOUT_23(OBFH_SF_BODY_J, OBFH_SF_BODY_Q, OBFH_SF_LEAF_G, OBFH_SF_LEAF_H))
#define OBFH_SF_VARIANT_90 OBFH_SF_ASM(OBFH_SF_GUARD_7, OBFH_SF_LAYOUT_24(OBFH_SF_BODY_M, OBFH_SF_BODY_T, OBFH_SF_LEAF_L, OBFH_SF_LEAF_C))
#define OBFH_SF_VARIANT_91 OBFH_SF_ASM(OBFH_SF_GUARD_7, OBFH_SF_LAYOUT_25(OBFH_SF_BODY_P, OBFH_SF_BODY_C, OBFH_SF_LEAF_E, OBFH_SF_LEAF_K))
#define OBFH_SF_VARIANT_92 OBFH_SF_ASM(OBFH_SF_GUARD_7, OBFH_SF_LAYOUT_26(OBFH_SF_BODY_S, OBFH_SF_BODY_F, OBFH_SF_LEAF_J, OBFH_SF_LEAF_F))
#define OBFH_SF_VARIANT_93 OBFH_SF_ASM(OBFH_SF_GUARD_7, OBFH_SF_LAYOUT_27(OBFH_SF_BODY_B, OBFH_SF_BODY_I, OBFH_SF_LEAF_C, OBFH_SF_LEAF_A))
#define OBFH_SF_VARIANT_94 OBFH_SF_ASM(OBFH_SF_GUARD_8, OBFH_SF_LAYOUT_22(OBFH_SF_BODY_E, OBFH_SF_BODY_L, OBFH_SF_LEAF_I, OBFH_SF_LEAF_I))
#define OBFH_SF_VARIANT_95 OBFH_SF_ASM(OBFH_SF_GUARD_8, OBFH_SF_LAYOUT_23(OBFH_SF_BODY_H, OBFH_SF_BODY_O, OBFH_SF_LEAF_B, OBFH_SF_LEAF_D))
#define OBFH_SF_VARIANT_96 OBFH_SF_ASM(OBFH_SF_GUARD_8, OBFH_SF_LAYOUT_24(OBFH_SF_BODY_K, OBFH_SF_BODY_R, OBFH_SF_LEAF_G, OBFH_SF_LEAF_K))
#define OBFH_SF_VARIANT_97 OBFH_SF_ASM(OBFH_SF_GUARD_8, OBFH_SF_LAYOUT_25(OBFH_SF_BODY_N, OBFH_SF_BODY_A, OBFH_SF_LEAF_L, OBFH_SF_LEAF_G))
#define OBFH_SF_VARIANT_98 OBFH_SF_ASM(OBFH_SF_GUARD_8, OBFH_SF_LAYOUT_26(OBFH_SF_BODY_Q, OBFH_SF_BODY_D, OBFH_SF_LEAF_E, OBFH_SF_LEAF_B))
#define OBFH_SF_VARIANT_99 OBFH_SF_ASM(OBFH_SF_GUARD_8, OBFH_SF_LAYOUT_27(OBFH_SF_BODY_T, OBFH_SF_BODY_G, OBFH_SF_LEAF_J, OBFH_SF_LEAF_I))
#define OBFH_SF_VARIANT_100 OBFH_SF_ASM(OBFH_SF_GUARD_9, OBFH_SF_LAYOUT_22(OBFH_SF_BODY_C, OBFH_SF_BODY_J, OBFH_SF_LEAF_D, OBFH_SF_LEAF_E))
#define OBFH_SF_VARIANT_101 OBFH_SF_ASM(OBFH_SF_GUARD_9, OBFH_SF_LAYOUT_23(OBFH_SF_BODY_F, OBFH_SF_BODY_M, OBFH_SF_LEAF_I, OBFH_SF_LEAF_L))
#define OBFH_SF_VARIANT_102 OBFH_SF_ASM(OBFH_SF_GUARD_9, OBFH_SF_LAYOUT_24(OBFH_SF_BODY_I, OBFH_SF_BODY_P, OBFH_SF_LEAF_B, OBFH_SF_LEAF_G))
#define OBFH_SF_VARIANT_103 OBFH_SF_ASM(OBFH_SF_GUARD_9, OBFH_SF_LAYOUT_25(OBFH_SF_BODY_L, OBFH_SF_BODY_S, OBFH_SF_LEAF_G, OBFH_SF_LEAF_C))
#define OBFH_SF_VARIANT_104 OBFH_SF_ASM(OBFH_SF_GUARD_9, OBFH_SF_LAYOUT_26(OBFH_SF_BODY_O, OBFH_SF_BODY_B, OBFH_SF_LEAF_L, OBFH_SF_LEAF_J))
#define OBFH_SF_VARIANT_105 OBFH_SF_ASM(OBFH_SF_GUARD_9, OBFH_SF_LAYOUT_27(OBFH_SF_BODY_R, OBFH_SF_BODY_E, OBFH_SF_LEAF_E, OBFH_SF_LEAF_E))
#define OBFH_SF_VARIANT_106 OBFH_SF_ASM(OBFH_SF_GUARD_10, OBFH_SF_LAYOUT_22(OBFH_SF_BODY_A, OBFH_SF_BODY_H, OBFH_SF_LEAF_K, OBFH_SF_LEAF_A))
#define OBFH_SF_VARIANT_107 OBFH_SF_ASM(OBFH_SF_GUARD_10, OBFH_SF_LAYOUT_23(OBFH_SF_BODY_D, OBFH_SF_BODY_K, OBFH_SF_LEAF_D, OBFH_SF_LEAF_H))
#define OBFH_SF_VARIANT_108 OBFH_SF_ASM(OBFH_SF_GUARD_10, OBFH_SF_LAYOUT_24(OBFH_SF_BODY_G, OBFH_SF_BODY_N, OBFH_SF_LEAF_I, OBFH_SF_LEAF_C))
#define OBFH_SF_VARIANT_109 OBFH_SF_ASM(OBFH_SF_GUARD_10, OBFH_SF_LAYOUT_25(OBFH_SF_BODY_J, OBFH_SF_BODY_Q, OBFH_SF_LEAF_B, OBFH_SF_LEAF_K))
#define OBFH_SF_VARIANT_110 OBFH_SF_ASM(OBFH_SF_GUARD_10, OBFH_SF_LAYOUT_26(OBFH_SF_BODY_M, OBFH_SF_BODY_T, OBFH_SF_LEAF_G, OBFH_SF_LEAF_F))
#define OBFH_SF_VARIANT_111 OBFH_SF_ASM(OBFH_SF_GUARD_10, OBFH_SF_LAYOUT_27(OBFH_SF_BODY_P, OBFH_SF_BODY_C, OBFH_SF_LEAF_L, OBFH_SF_LEAF_A))
#define OBFH_SF_VARIANT_112 OBFH_SF_ASM(OBFH_SF_GUARD_11, OBFH_SF_LAYOUT_22(OBFH_SF_BODY_S, OBFH_SF_BODY_F, OBFH_SF_LEAF_F, OBFH_SF_LEAF_I))
#define OBFH_SF_VARIANT_113 OBFH_SF_ASM(OBFH_SF_GUARD_11, OBFH_SF_LAYOUT_23(OBFH_SF_BODY_B, OBFH_SF_BODY_I, OBFH_SF_LEAF_K, OBFH_SF_LEAF_D))
#define OBFH_SF_VARIANT_114 OBFH_SF_ASM(OBFH_SF_GUARD_11, OBFH_SF_LAYOUT_24(OBFH_SF_BODY_E, OBFH_SF_BODY_L, OBFH_SF_LEAF_D, OBFH_SF_LEAF_K))
#define OBFH_SF_VARIANT_115 OBFH_SF_ASM(OBFH_SF_GUARD_11, OBFH_SF_LAYOUT_25(OBFH_SF_BODY_H, OBFH_SF_BODY_O, OBFH_SF_LEAF_I, OBFH_SF_LEAF_G))
#define OBFH_SF_VARIANT_116 OBFH_SF_ASM(OBFH_SF_GUARD_11, OBFH_SF_LAYOUT_26(OBFH_SF_BODY_K, OBFH_SF_BODY_R, OBFH_SF_LEAF_B, OBFH_SF_LEAF_B))
#define OBFH_SF_VARIANT_117 OBFH_SF_ASM(OBFH_SF_GUARD_11, OBFH_SF_LAYOUT_27(OBFH_SF_BODY_N, OBFH_SF_BODY_A, OBFH_SF_LEAF_G, OBFH_SF_LEAF_I))
#define OBFH_SF_VARIANT_118 OBFH_SF_ASM(OBFH_SF_GUARD_12, OBFH_SF_LAYOUT_22(OBFH_SF_BODY_Q, OBFH_SF_BODY_D, OBFH_SF_LEAF_A, OBFH_SF_LEAF_E))
#define OBFH_SF_VARIANT_119 OBFH_SF_ASM(OBFH_SF_GUARD_12, OBFH_SF_LAYOUT_23(OBFH_SF_BODY_T, OBFH_SF_BODY_G, OBFH_SF_LEAF_F, OBFH_SF_LEAF_L))
#define OBFH_SF_VARIANT_120 OBFH_SF_ASM(OBFH_SF_GUARD_12, OBFH_SF_LAYOUT_24(OBFH_SF_BODY_C, OBFH_SF_BODY_J, OBFH_SF_LEAF_K, OBFH_SF_LEAF_G))
#define OBFH_SF_VARIANT_121 OBFH_SF_ASM(OBFH_SF_GUARD_12, OBFH_SF_LAYOUT_25(OBFH_SF_BODY_F, OBFH_SF_BODY_M, OBFH_SF_LEAF_D, OBFH_SF_LEAF_C))
#define OBFH_SF_VARIANT_122 OBFH_SF_ASM(OBFH_SF_GUARD_12, OBFH_SF_LAYOUT_26(OBFH_SF_BODY_I, OBFH_SF_BODY_P, OBFH_SF_LEAF_I, OBFH_SF_LEAF_J))
#define OBFH_SF_VARIANT_123 OBFH_SF_ASM(OBFH_SF_GUARD_12, OBFH_SF_LAYOUT_27(OBFH_SF_BODY_L, OBFH_SF_BODY_S, OBFH_SF_LEAF_B, OBFH_SF_LEAF_E))
#define OBFH_SF_VARIANT_124 OBFH_SF_ASM(OBFH_SF_GUARD_13, OBFH_SF_LAYOUT_22(OBFH_SF_BODY_O, OBFH_SF_BODY_B, OBFH_SF_LEAF_H, OBFH_SF_LEAF_A))
#define OBFH_SF_VARIANT_125 OBFH_SF_ASM(OBFH_SF_GUARD_13, OBFH_SF_LAYOUT_23(OBFH_SF_BODY_R, OBFH_SF_BODY_E, OBFH_SF_LEAF_A, OBFH_SF_LEAF_H))
#define OBFH_SF_VARIANT_126 OBFH_SF_ASM(OBFH_SF_GUARD_13, OBFH_SF_LAYOUT_24(OBFH_SF_BODY_A, OBFH_SF_BODY_H, OBFH_SF_LEAF_F, OBFH_SF_LEAF_C))
#define OBFH_SF_VARIANT_127 OBFH_SF_ASM(OBFH_SF_GUARD_13, OBFH_SF_LAYOUT_25(OBFH_SF_BODY_D, OBFH_SF_BODY_K, OBFH_SF_LEAF_K, OBFH_SF_LEAF_K))

#define STACK_PROXY_FUNCTIONS ({                                                                                                                                                    \
    enum { __obfh_sf_id = __COUNTER__,                                                                                                                                              \
           __obfh_sf_variant = OBFH_MIX_B(OBFH_MIX_A((unsigned int)__obfh_sf_id ^ (unsigned int)OBFH_BUILD_SEED ^ (unsigned int)__LINE__ ^ 0x53504631u)) % OBFH_SF_VARIANT_COUNT }; \
    OBFH_SF_SELECT(__obfh_sf_variant);                                                                                                                                              \
    (void)0;                                                                                                                                                                        \
})

#if defined(__x86_64__)
#define BAD_JMP __obfh_asm__("cpuid; mov %eax, %rax; mov %ebx, %edx; .byte 0xFF, 0x25, 0xF1, 0xF2, 0xF3, 0xF4;")
#else
#define BAD_JMP __obfh_asm__(".byte 0xEB, 0xE1;")
#endif

#define BAD_CALL __obfh_asm__(".byte 0xB8;")

static void obfh_junk_func_args(int z, ...) OBFH_CODE_SECTION_ATTRIBUTE {
    BREAK_STACK_CFLOW;
    __obfh_asm__("nop;");
    return;
}

static void obfh_junk_func() OBFH_DATA_CODE_SECTION_ATTRIBUTE {
    BREAK_STACK_CFLOW;
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
    : OBFH_CPUID_CLOBBERS)

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

static FARPROC obfh_crt_resolve(const char *name);
static void *malloc_proxy(size_t size) {
    STACK_PROXY_FUNCTIONS;
    BREAK_STACK_CFLOW;
    char name[] = {_m, _a, _l, _l, _o, _c, _0};
    void *result = ((void *(*)(size_t))obfh_crt_resolve(name))(size);
    STACK_PROXY_FUNCTIONS;
    RET_BY_VAR(result);
}
#define malloc(...) malloc_proxy(__VA_ARGS__)

static float rndValueToProxy = RND(0, 10);

static int obfh_int_proxy(int value) OBFH_CODE_SECTION_ATTRIBUTE {
    BREAK_STACK_CFLOW;
    RET_BY_VAR(value);
}

// Preserve pointer and SIZE_T width on both Windows targets.
static ULONG_PTR obfh_uintptr_proxy(ULONG_PTR value) OBFH_CODE_SECTION_ATTRIBUTE {
    BREAK_STACK_CFLOW;
    RET_BY_VAR(value);
}

#define OBFH_PTR(type, value) ((type)obfh_uintptr_proxy((ULONG_PTR)(value)))

static double obfh_double_proxy(double value) OBFH_CODE_SECTION_ATTRIBUTE {
    BREAK_STACK_CFLOW;
    RET_BY_VAR(value);
}

static float obfh_condition_true();

// Hidden string access
static char *obfh_process_hidden_string(char *string, ...) OBFH_CODE_SECTION_ATTRIBUTE {
    BREAK_STACK_CFLOW;

    if (!obfh_condition_true() || _0) {
        BAD_JMP;
    }

    // ['\0', 's', 't', 'r', 'i', 'n', 'g'] => "string"
    // STACK_STRING storage belongs to the HIDE_STRING call site.
    return string + 1;
}

static float obfh_condition_true() OBFH_CODE_SECTION_ATTRIBUTE {
    BREAK_STACK_CFLOW;
    return _1 && TRUE;
}

static int obfh_condition_proxy(float junk, float condition, ...) OBFH_CODE_SECTION_ATTRIBUTE {
    BREAK_STACK_CFLOW;
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
        if (InterlockedCompareExchange(&entry->state, 2, 2) != 2) continue;
        unsigned int i = 0;
        for (; i < sizeof(entry->name); ++i) {
            unsigned char byte = entry->name[i] ^ (unsigned char)(SALT_SHIFT + i * 13u);
            if (byte != (unsigned char)name[i]) break;
            if (!byte) return (FARPROC)obfh_uintptr_proxy(entry->address ^ SALT_SHIFT);
        }
    }
    return NULL;
}

static void obfh_crt_publish(const char *name, FARPROC function) {
    BREAK_STACK_CFLOW;
    if (!function || obfh_crt_cached(name)) return;
    unsigned int length = 0;
    while (name[length] && length < 31) ++length;
    if (name[length]) return;
    for (unsigned int slot = 0; slot < 32; ++slot) {
        OBFH_CRT_ENTRY *entry = &obfh_crt_entries[slot];
        if (InterlockedCompareExchange(&entry->state, 1, 0) != 0) continue;
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

// =============================================================
// Control Flow (global)
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
        unsigned int __obfh_flow_state =                                                                              \
            __builtin_choose_expr(__obfh_flow_hash & 1u, (condition) ? __obfh_true_tag : __obfh_false_tag,            \
                                  !(condition) ? __obfh_false_tag : __obfh_true_tag);                                 \
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
/* Private VM kernel, emitted before keyword interception. */
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
    x ^= x >> 13;
    return x;
}
/* Encodings are local to an execution; only decoded uint32 values enter ALU operations. */
static unsigned int obfh_v_control_read(const OBFH_V_CONTEXT *c, unsigned int r) {
    return c->controls[r] ^ obfh_v_mix(c->key ^ (r * 3266489917u));
}
static void obfh_v_control_write(OBFH_V_CONTEXT *c, unsigned int r, unsigned int value) {
    c->controls[r] = value ^ obfh_v_mix(c->key ^ (r * 3266489917u));
}
static unsigned int obfh_v_flags(const OBFH_V_CONTEXT *c) {
    return c->flags ^ c->flag_key;
}
static void obfh_v_set_flags(OBFH_V_CONTEXT *c, unsigned int flags) {
    c->flags = flags ^ c->flag_key;
}
static void obfh_v_write(OBFH_V_CONTEXT *c, unsigned int r, long double v) {
    unsigned int m = obfh_v_mix(c->key ^ (r * 3266489917u) ^ c->steps);
    const unsigned char *p = (const unsigned char *)&v;
    c->registers[r].mask = m;
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
    for (unsigned int i = 0; i < sizeof(v); ++i) {
        m = obfh_v_mix(m + i + 1u);
        p[i] = c->registers[r].bytes[i] ^ (unsigned char)m;
    }
    return v;
}
static void obfh_v_copy(OBFH_V_CONTEXT *c, unsigned int d, unsigned int a) {
    /* Encoded copy preserves numeric object bytes without an FP operation. */
    c->registers[d] = c->registers[a];
    c->initialized |= 1u << d;
}
static void obfh_v_failure(OBFH_V_CONTEXT *c, unsigned int status) {
    c->status = status;
    ExitProcess(0xE0BF0000u | status);
}
static unsigned int obfh_v_decode(unsigned int word, unsigned int key, unsigned int pc) {
    unsigned int rotate = (key % 31u) + 1u;
    word ^= key + pc * 0x9e3779b9u;
    return (word << rotate) | (word >> (32u - rotate));
}
static unsigned int obfh_v_opcode(unsigned int word, unsigned int key) {
    unsigned int multiplier = ((key >> 8) & 255u) | 1u, inverse = 1u;
    inverse *= 2u - multiplier * inverse;
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

#if NO_CFLOW != 1
// Each intercepted if inserts its selected template between condition transport stages.
// if
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
#define OBFH_FLOW_BREAK_GUARD()                                    \
    (({                                                            \
        if (OBFUS_CONDITION_BLOCK(RND(1, 255))) BREAK_STACK_CFLOW; \
        1;                                                         \
    }))

// A matched conditional consumes the caller's semicolon and preserves dangling else.
#define break                   \
    if OBFH_FLOW_BREAK_GUARD () \
        break;                  \
    else                        \
        (void)0

// switch
#define switch(...)                                                                                                  \
    switch (({                                                                                                       \
        __typeof__((__VA_ARGS__)) __obfh_switch_value = (__VA_ARGS__);                                               \
        volatile ULONG_PTR __obfh_switch_shift = (ULONG_PTR)OBFUS_CONDITION_BLOCK(RND(1, 255)) * SALT_SHIFT;         \
        ULONG_PTR __obfh_switch_address = obfh_uintptr_proxy((ULONG_PTR)&__obfh_switch_value ^ __obfh_switch_shift); \
        *(__typeof__(&__obfh_switch_value))(__obfh_switch_address ^ __obfh_switch_shift);                            \
    }))

// while
#define while(...) while (OBFUS_CONDITION_BLOCK((__VA_ARGS__)))

// Protect every iteration and retain randomized bytes in the never-taken arm.
#define for(...)                    \
    for (__VA_ARGS__)               \
        if (0) {                    \
            OBFH_FLOW_DEAD_BYTES(); \
        } else

#endif
// =============================================================

// =============================================================
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
// =============================================================
// Caller-owned storage keeps the mask valid and avoids shared-buffer races.
static char *getCharMask(int count, char *mask, size_t capacity) OBFH_CODE_SECTION_ATTRIBUTE {
    BREAK_STACK_CFLOW;
    if (!mask || !capacity || count < 0 || (size_t)count > (capacity - 1) / 2) return NULL;
    int i = (((_1 * _5) - _4) + _1) - _2;
    BREAK_STACK_CFLOW;
    char *ptr = mask;
    for (i = _0; i < count; ++i) {
        *ptr++ = '%';
        *ptr++ = _c;
    }
    *ptr = _0;
    BREAK_STACK_CFLOW;
    FAKE_CPUID;
    return mask;
}

// WriteConsoleA
static BOOL WriteConsoleA_proxy(HANDLE hConsoleOutput, const void *lpBuffer, DWORD nNumberOfCharsToWrite, LPDWORD lpNumberOfCharsWritten, LPVOID lpReserved) OBFH_CODE_SECTION_ATTRIBUTE {
    BREAK_STACK_CFLOW;
    FAKE_CPUID;
    return WriteConsoleA(hConsoleOutput, lpBuffer, nNumberOfCharsToWrite, lpNumberOfCharsWritten, lpReserved);
}
#define WriteConsoleA(...) WriteConsoleA_proxy(__VA_ARGS__)

// GetStdHandle
static HANDLE GetStdHandle_proxy(DWORD nStdHandle) OBFH_CODE_SECTION_ATTRIBUTE {
    BREAK_STACK_CFLOW;
    FAKE_CPUID;
    return GetStdHandle(obfh_int_proxy(nStdHandle));
}
#define GetStdHandle(...) GetStdHandle_proxy(__VA_ARGS__)

static HMODULE GetModuleHandleA_proxy(LPCSTR lpModuleName) OBFH_CODE_SECTION_ATTRIBUTE {
    BREAK_STACK_CFLOW;
    FAKE_CPUID;
    return GetModuleHandleA(lpModuleName);
}
#define GetModuleHandleA(...) GetModuleHandleA_proxy(__VA_ARGS__)

// strcmp
static int strcmp_custom(const char *str1, const char *str2) OBFH_CODE_SECTION_ATTRIBUTE {
    BREAK_STACK_CFLOW;
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
static size_t strlen_custom(const char *str) OBFH_CODE_SECTION_ATTRIBUTE {
    BREAK_STACK_CFLOW;
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
static HMODULE LoadLibraryA_proxy(LPCSTR lpLibFileName);

// Bounded string scan used by the export parser.
static const char *obfh_find_zero(const void *buffer, size_t count) {
    BREAK_STACK_CFLOW;
    const char *bytes = buffer;
    for (size_t i = _0; i < count; ++i)
        if (bytes[i] == _0) return bytes + i;
    BREAK_STACK_CFLOW;
    return NULL;
}
// Check an RVA range against the loaded image size.
static int obfh_image_range(DWORD size, DWORD rva, size_t length) {
    return rva <= size && length <= (size_t)(size - rva);
}
// GetProcAddress: custom PE export lookup, including ordinals and forwarders.
static FARPROC obfh_find_export(HMODULE hModule, LPCSTR lpProcName, unsigned int depth) OBFH_CODE_SECTION_ATTRIBUTE {
    BREAK_STACK_CFLOW;
    BREAK_STACK_CFLOW;
    obfh_junk_func_args(RND(0, 885));
    BREAK_STACK_CFLOW;
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
    BREAK_STACK_CFLOW;
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
static FARPROC GetProcAddress_custom(HMODULE hModule, LPCSTR lpProcName) OBFH_CODE_SECTION_ATTRIBUTE {
    FARPROC result = obfh_find_export(hModule, lpProcName, 0);
    BREAK_STACK_CFLOW;
    return result;
}
#define GetProcAddress(...) GetProcAddress_custom(__VA_ARGS__)

// LoadLibraryA: dynamic loader resolution and proxy chain.
static HMODULE LoadLibraryA_0(LPCSTR lpLibFileName) OBFH_CODE_SECTION_ATTRIBUTE {
    BREAK_STACK_CFLOW;
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
                if (!kernel) return NULL;
                FAKE_CPUID;
                char charL = _L;
                obfh_junk_func_args(_0 + RND(1, 5));
                BREAK_STACK_CFLOW;
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
    return LoadLibraryA_0((LPCSTR)lpLibFileName);
}

static HMODULE LoadLibraryA_2(LPCSTR lpLibFileName) {
    BREAK_STACK_CFLOW;
    return LoadLibraryA_1((LPCSTR)lpLibFileName);
}

static HMODULE LoadLibraryA_3(LPCSTR lpLibFileName) OBFH_CODE_SECTION_ATTRIBUTE {
    BREAK_STACK_CFLOW;
    return LoadLibraryA_2((LPCSTR)lpLibFileName);
}

static HMODULE LoadLibraryA_4(LPCSTR lpLibFileName) {
    BREAK_STACK_CFLOW;
    return LoadLibraryA_3((LPCSTR)lpLibFileName);
}

static HMODULE LoadLibraryA_5(LPCSTR lpLibFileName) OBFH_CODE_SECTION_ATTRIBUTE {
    BREAK_STACK_CFLOW;
    return LoadLibraryA_4((LPCSTR)lpLibFileName);
}

static HMODULE LoadLibraryA_proxy(LPCSTR lpLibFileName) {
    BREAK_STACK_CFLOW;
    return LoadLibraryA_5((LPCSTR)lpLibFileName);
}
#define LoadLibraryA(...) LoadLibraryA_proxy(__VA_ARGS__)

// =============================================================
// Anti-Debug: probes return evidence; the response lives outside the call site.
#if NO_ANTIDEBUG != 1

#if ANTIDEBUG_V2 == 1
// The worker owns the duplicated handle, including after a failed/timed-out wait.
static DWORD WINAPI obfh_ad_register_worker(void *argument) {
    HANDLE thread = (HANDLE)argument;
    DWORD detected = 0;
    BREAK_STACK_CFLOW;
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
                         &target, 0, FALSE, DUPLICATE_SAME_ACCESS)) return 0;
    HANDLE worker = CreateThread(NULL, 0, obfh_ad_register_worker, target, 0, NULL);
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
    if (check) return check() != FALSE;
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
#if ANTIDEBUG_V2 == 1
    if (!detected) detected = obfh_ad_register_probe();
#endif
    return detected;
}

// Live paths use defined unsigned arithmetic and leave SP untouched. Random
// instructions come from the shared pool and reside on its skipped paths.
static void obfh_ad_sink_a(unsigned int initial) OBFH_CODE_SECTION_ATTRIBUTE {
    volatile unsigned int state = initial | 1u;
    for (;;) {
        BREAK_STACK_CFLOW;
        unsigned int next = state ^ RND(1, 2147483646);
        state = ((next << 7) | (next >> 25)) + RND(1, 65535);
    }
}

static void obfh_ad_sink_b(unsigned int initial) OBFH_CODE_SECTION_ATTRIBUTE {
    volatile unsigned int state = initial;
    for (;;) {
        state = state * (RND(1, 32767) * 2u + 1u) + RND(1, 65535);
        BREAK_STACK_CFLOW;
        state ^= state >> 13;
    }
}

static void obfh_ad_react(unsigned int nonce, unsigned int route) OBFH_CODE_SECTION_ATTRIBUTE {
    typedef void (*ObfhResponse)(unsigned int);
    ObfhResponse volatile response = (route & 1u) ? obfh_ad_sink_a : obfh_ad_sink_b;
    BREAK_STACK_CFLOW;
    response(nonce ^ RND(1, 2147483646));
}

#define ANTI_DEBUG                                                                   \
    do {                                                                             \
        BREAK_STACK_CFLOW;                                                           \
        if (IsDebuggerPresent_proxy()) obfh_ad_react(RND(1, 2147483646), RND(0, 1)); \
    } while (0)
#else
#define ANTI_DEBUG \
    do {           \
    } while (0)
#endif

// CRT module name, copied within the hidden string's lifetime.
static char *getStdLibName_proxy(char *name, size_t capacity) {
    BREAK_STACK_CFLOW;
    if (!name || capacity < sizeof("msvcrt")) return NULL;
    const char *hidden = HIDE_STRING("msvcrt");
    for (int i = _0; i < sizeof("msvcrt"); ++i) name[i] = hidden[i];
    FAKE_CPUID;
    return name;
}

// Resolve through the custom loader/export chain once, then keep its DLL alive.
static FARPROC obfh_crt_resolve(const char *name) {
    BREAK_STACK_CFLOW;
    FARPROC cached = obfh_crt_cached(name);
    if (cached) return cached;
    static PVOID volatile cachedModule;
    HMODULE module = (HMODULE)InterlockedCompareExchangePointer(&cachedModule, NULL, NULL);
    char moduleName[11];
    if (!module) {
        HMODULE loaded = LoadLibraryA_proxy(getStdLibName_proxy(moduleName, sizeof moduleName));
        if (!loaded) return NULL;
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
static int printf_custom(int junk, const char *format, ...) {
    BREAK_STACK_CFLOW;
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
            if (result >= 0 && (!WriteConsoleA(console, buffer, (DWORD)obfh_uintptr_proxy((ULONG_PTR)result), &written, NULL) || written != (DWORD)result)) result = -1;
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
            BREAK_STACK_CFLOW;                                                              \
            obfh_junk_func_args((RND(0, 1000) * 3) < _0);                                   \
            __obfh_printf_result = printf_custom(RND(0, 1000), __VA_ARGS__);                \
        } while (_0 > ((unsigned long long)RND(0, 100000000000) * (unsigned char)_2) + 82); \
        __obfh_printf_result;                                                               \
    })

// scanf
static char *getScanfName_proxy(char *name) {
    BREAK_STACK_CFLOW;
    FAKE_CPUID;
    name[0] = _s;
    name[1] = _c;
    name[2] = _a;
    name[3] = _n;
    name[4] = _f;
    name[5] = _0;
#if CFLOW_V2
    BREAK_STACK_CFLOW;
#endif
    return name;
}
// Build the name in caller-owned storage, then invoke the resolved typed function.
#define OBFH_CRT_CALL(name_builder, function_type, ...) ({                         \
    BREAK_STACK_CFLOW;                                                             \
    char __obfh_crt_name[32];                                                      \
    STACK_PROXY_FUNCTIONS;                                                         \
    ((function_type)obfh_crt_resolve(name_builder(__obfh_crt_name)))(__VA_ARGS__); \
})

#define scanf(...) OBFH_CRT_CALL(getScanfName_proxy, int (*)(const char *, ...), __VA_ARGS__)

// sprintf
static char *getSprintfName_proxy(char *name) {
    BREAK_STACK_CFLOW;
    FAKE_CPUID;
    name[0] = _s;
    name[1] = _p;
    name[2] = _r;
    name[3] = _i;
    name[4] = _n;
    name[5] = _t;
    name[6] = _f;
    name[7] = _0;
#if CFLOW_V2
    BREAK_STACK_CFLOW;
#endif
    return name;
}
#define sprintf(...) OBFH_CRT_CALL(getSprintfName_proxy, int (*)(char *, const char *, ...), __VA_ARGS__)

// fclose
static char *getFcloseName_proxy(char *name) {
    BREAK_STACK_CFLOW;
    FAKE_CPUID;
    name[0] = _f;
    name[1] = _c;
    name[2] = _l;
    name[3] = _o;
    name[4] = _s;
    name[5] = _e;
    name[6] = _0;
#if CFLOW_V2
    BREAK_STACK_CFLOW;
#endif
    return name;
}
#define fclose(...) OBFH_CRT_CALL(getFcloseName_proxy, int (*)(FILE *), __VA_ARGS__)

// fopen
static char *getFopenName_proxy(char *name) {
    BREAK_STACK_CFLOW;
    FAKE_CPUID;
    name[0] = _f;
    name[1] = _o;
    name[2] = _p;
    name[3] = _e;
    name[4] = _n;
    name[5] = _0;
#if CFLOW_V2
    BREAK_STACK_CFLOW;
#endif
    return name;
}
#define fopen(...) OBFH_CRT_CALL(getFopenName_proxy, FILE *(*)(const char *, const char *), __VA_ARGS__)

// fread
static char *getFreadName_proxy(char *name) {
    BREAK_STACK_CFLOW;
    FAKE_CPUID;
    name[0] = _f;
    name[1] = _r;
    name[2] = _e;
    name[3] = _a;
    name[4] = _d;
    name[5] = _0;
#if CFLOW_V2
    BREAK_STACK_CFLOW;
#endif
    return name;
}
#define fread(...) OBFH_CRT_CALL(getFreadName_proxy, size_t (*)(void *, size_t, size_t, FILE *), __VA_ARGS__)

// fwrite
static char *getFwriteName_proxy(char *name) {
    BREAK_STACK_CFLOW;
    FAKE_CPUID;
    name[0] = _f;
    name[1] = _w;
    name[2] = _r;
    name[3] = _i;
    name[4] = _t;
    name[5] = _e;
    name[6] = _0;
#if CFLOW_V2
    BREAK_STACK_CFLOW;
#endif
    return name;
}
#define fwrite(...) OBFH_CRT_CALL(getFwriteName_proxy, size_t (*)(const void *, size_t, size_t, FILE *), __VA_ARGS__)

// exit
static char *getExitName_proxy(char *name) {
    BREAK_STACK_CFLOW;
    FAKE_CPUID;
    name[0] = _e;
    name[1] = _x;
    name[2] = _i;
    name[3] = _t;
    name[4] = _0;
#if CFLOW_V2
    BREAK_STACK_CFLOW;
#endif
    return name;
}
#define exit(...) OBFH_CRT_CALL(getExitName_proxy, void (*)(int), __VA_ARGS__)

// strcpy
static char *getStrcpyName_proxy(char *name) {
    BREAK_STACK_CFLOW;
    FAKE_CPUID;
    name[0] = _s;
    name[1] = _t;
    name[2] = _r;
    name[3] = _c;
    name[4] = _p;
    name[5] = _y;
    name[6] = _0;
#if CFLOW_V2
    BREAK_STACK_CFLOW;
#endif
    return name;
}
#define strcpy(...) OBFH_CRT_CALL(getStrcpyName_proxy, char *(*)(char *, const char *), __VA_ARGS__)

// strtok
static char *getStrtokName_proxy(char *name) {
    BREAK_STACK_CFLOW;
    FAKE_CPUID;
    name[0] = _s;
    name[1] = _t;
    name[2] = _r;
    name[3] = _t;
    name[4] = _o;
    name[5] = _k;
    name[6] = _0;
#if CFLOW_V2
    BREAK_STACK_CFLOW;
#endif
    return name;
}
#define strtok(...) OBFH_CRT_CALL(getStrtokName_proxy, char *(*)(char *, const char *), __VA_ARGS__)

// memset
static void *memset_proxy(void *ptr, int value, size_t num) {
    STACK_PROXY_FUNCTIONS;
    BREAK_STACK_CFLOW;
    void *result = ((void *(*)(void *, int, size_t))obfh_crt_resolve(HIDE_STRING("memset")))(ptr, value * _1, num);
    STACK_PROXY_FUNCTIONS;
    RET_BY_VAR(result);
}
#define memset(...) memset_proxy(__VA_ARGS__)

// memcpy
static char *getMemcpyName_proxy(char *name) {
    BREAK_STACK_CFLOW;
    FAKE_CPUID;
    name[0] = _m;
    name[1] = _e;
    name[2] = _m;
    name[3] = _c;
    name[4] = _p;
    name[5] = _y;
    name[6] = _0;
#if CFLOW_V2
    BREAK_STACK_CFLOW;
#endif
    return name;
}
#define memcpy(...) OBFH_CRT_CALL(getMemcpyName_proxy, void *(*)(void *, const void *, size_t), __VA_ARGS__)

// strchr
static char *getStrchrName_proxy(char *name) {
    BREAK_STACK_CFLOW;
    FAKE_CPUID;
    name[0] = _s;
    name[1] = _t;
    name[2] = _r;
    name[3] = _c;
    name[4] = _h;
    name[5] = _r;
    name[6] = _0;
#if CFLOW_V2
    BREAK_STACK_CFLOW;
#endif
    return name;
}
#define strchr(...) OBFH_CRT_CALL(getStrchrName_proxy, char *(*)(const char *, int), __VA_ARGS__)

// strrchr
static char *getStrrchrName_proxy(char *name) {
    BREAK_STACK_CFLOW;
    FAKE_CPUID;
    name[0] = _s;
    name[1] = _t;
    name[2] = _r;
    name[3] = _r;
    name[4] = _c;
    name[5] = _h;
    name[6] = _r;
    name[7] = _0;
#if CFLOW_V2
    BREAK_STACK_CFLOW;
#endif
    return name;
}
#define strrchr(...) OBFH_CRT_CALL(getStrrchrName_proxy, char *(*)(const char *, int), __VA_ARGS__)

// rand
static char *getRandName_proxy(char *name) {
    BREAK_STACK_CFLOW;
    FAKE_CPUID;
    name[0] = _r;
    name[1] = _a;
    name[2] = _n;
    name[3] = _d;
    name[4] = _0;
#if CFLOW_V2
    BREAK_STACK_CFLOW;
#endif
    return name;
}
#define rand(...) OBFH_CRT_CALL(getRandName_proxy, int (*)(void), __VA_ARGS__)

// realloc
static char *getReallocName_proxy(char *name) OBFH_CODE_SECTION_ATTRIBUTE {
    BREAK_STACK_CFLOW;
    FAKE_CPUID;
    name[0] = _r;
    name[1] = _e;
    name[2] = _a;
    name[3] = _l;
    name[4] = _l;
    name[5] = _o;
    name[6] = _c;
    name[7] = _0;
#if CFLOW_V2
    BREAK_STACK_CFLOW;
#endif
    return name;
}
#define realloc(...) OBFH_CRT_CALL(getReallocName_proxy, void *(*)(void *, size_t), __VA_ARGS__)

static void *calloc_proxy(size_t nmemb, size_t size) OBFH_CODE_SECTION_ATTRIBUTE {
    STACK_PROXY_FUNCTIONS;
    BREAK_STACK_CFLOW;
    void *result = ((void *(*)(size_t, size_t))obfh_crt_resolve(HIDE_STRING("calloc")))(nmemb, size);
    STACK_PROXY_FUNCTIONS;
    RET_BY_VAR(result);
}
#define calloc(nmemb, size) calloc_proxy(nmemb, size)

#undef realloc
static void *realloc_proxy(void *ptr, size_t size) OBFH_CODE_SECTION_ATTRIBUTE {
    STACK_PROXY_FUNCTIONS;
    BREAK_STACK_CFLOW;
    char name[32];
    void *result = ((void *(*)(void *, size_t))obfh_crt_resolve(getReallocName_proxy(name)))(ptr, size);
    STACK_PROXY_FUNCTIONS;
    RET_BY_VAR(result);
}
#define realloc(ptr, size) realloc_proxy(ptr, size)

static char *gets_proxy(char *s) OBFH_CODE_SECTION_ATTRIBUTE {
    STACK_PROXY_FUNCTIONS;
    BREAK_STACK_CFLOW;
    char *result = ((char *(*)(char *))obfh_crt_resolve(HIDE_STRING("gets")))(s);
    STACK_PROXY_FUNCTIONS;
    RET_BY_VAR(result);
}
#define gets(s) gets_proxy(s)

static int snprintf_proxy(char *str, size_t size, const char *format, ...) OBFH_CODE_SECTION_ATTRIBUTE {
    STACK_PROXY_FUNCTIONS;
    BREAK_STACK_CFLOW;
    va_list args;
    va_start(args, format);
    int result = vsnprintf(str, size, format, args);
    va_end(args);
    STACK_PROXY_FUNCTIONS;
    RET_BY_VAR(result);
}
#define snprintf(...) snprintf_proxy(__VA_ARGS__)

static int vsprintf_proxy(char *str, const char *format, va_list args) OBFH_CODE_SECTION_ATTRIBUTE {
    STACK_PROXY_FUNCTIONS;
    BREAK_STACK_CFLOW;
    int result = ((int (*)(char *, const char *, va_list))obfh_crt_resolve(HIDE_STRING("vsprintf")))(str, format, args);
    STACK_PROXY_FUNCTIONS;
    RET_BY_VAR(result);
}
#define vsprintf(str, format, args) vsprintf_proxy(str, format, args)

static int vsnprintf_proxy(char *str, size_t size, const char *format, va_list args) OBFH_CODE_SECTION_ATTRIBUTE {
    STACK_PROXY_FUNCTIONS;
    BREAK_STACK_CFLOW;
    // Keep TCC's formatting adapter: it need not be an msvcrt export.
    int result = vsnprintf(str, size, format, args);
    STACK_PROXY_FUNCTIONS;
    RET_BY_VAR(result);
}
#define vsnprintf(str, size, format, args) vsnprintf_proxy(str, size, format, args)

static char *getenv_proxy(const char *name) OBFH_CODE_SECTION_ATTRIBUTE {
    STACK_PROXY_FUNCTIONS;
    BREAK_STACK_CFLOW;
    char *result = ((char *(*)(const char *))obfh_crt_resolve(HIDE_STRING("getenv")))(name);
    STACK_PROXY_FUNCTIONS;
    RET_BY_VAR(result);
}
#define getenv(name) getenv_proxy(name)

static int system_proxy(const char *command) OBFH_CODE_SECTION_ATTRIBUTE {
    STACK_PROXY_FUNCTIONS;
    BREAK_STACK_CFLOW;
    int result = ((int (*)(const char *))obfh_crt_resolve(HIDE_STRING("system")))(command);
    STACK_PROXY_FUNCTIONS;
    RET_BY_VAR(result);
}
#define system(command) system_proxy(command)

static void abort_proxy(void) OBFH_CODE_SECTION_ATTRIBUTE {
    STACK_PROXY_FUNCTIONS;
    BREAK_STACK_CFLOW;
    ((void (*)(void))obfh_crt_resolve(HIDE_STRING("abort")))();
}
#define abort() abort_proxy()

static int atexit_proxy(void (*func)(void)) OBFH_CODE_SECTION_ATTRIBUTE {
    STACK_PROXY_FUNCTIONS;
    BREAK_STACK_CFLOW;
    int result = ((int (*)(void (*)(void)))obfh_crt_resolve(HIDE_STRING("atexit")))(func);
    STACK_PROXY_FUNCTIONS;
    RET_BY_VAR(result);
}
#define atexit(func) atexit_proxy(func)

static char *getcwd_proxy(char *buf, size_t size) OBFH_CODE_SECTION_ATTRIBUTE {
    STACK_PROXY_FUNCTIONS;
    BREAK_STACK_CFLOW;
    char *result = ((char *(*)(char *, int))obfh_crt_resolve(HIDE_STRING("_getcwd")))(buf, (int)size);
    STACK_PROXY_FUNCTIONS;
    RET_BY_VAR(result);
}
#define getcwd(buf, size) ((char *)getcwd_proxy(buf, size))

static int tolower_proxy(int c) OBFH_CODE_SECTION_ATTRIBUTE {
    STACK_PROXY_FUNCTIONS;
    BREAK_STACK_CFLOW;
    int result = ((int (*)(int))obfh_crt_resolve(HIDE_STRING("tolower")))(c);
    STACK_PROXY_FUNCTIONS;
    RET_BY_VAR(result);
}
#define tolower(c) tolower_proxy(c)

static int toupper_proxy(int c) OBFH_CODE_SECTION_ATTRIBUTE {
    STACK_PROXY_FUNCTIONS;
    BREAK_STACK_CFLOW;
    int result = ((int (*)(int))obfh_crt_resolve(HIDE_STRING("toupper")))(c);
    STACK_PROXY_FUNCTIONS;
    RET_BY_VAR(result);
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
    OBFH_WINAPI(CreateFile)                                                                                                                    \
    (OBFH_PTR(LPCTSTR, lpFileName), obfh_int_proxy(dwDesiredAccess), obfh_int_proxy(dwShareMode), OBFH_PTR(LPSECURITY_ATTRIBUTES, lpSecurityAttributes), obfh_int_proxy(dwCreationDisposition), obfh_int_proxy(dwFlagsAndAttributes), OBFH_PTR(HANDLE, hTemplateFile))

#define ReadFile(hFile, lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead, lpOverlapped) \
    ReadFile(OBFH_PTR(HANDLE, hFile), OBFH_PTR(LPVOID, lpBuffer), obfh_int_proxy(nNumberOfBytesToRead), OBFH_PTR(LPDWORD, lpNumberOfBytesRead), OBFH_PTR(LPOVERLAPPED, lpOverlapped))

#define WriteFile(hFile, lpBuffer, nNumberOfBytesToWrite, lpNumberOfBytesWritten, lpOverlapped) \
    WriteFile(OBFH_PTR(HANDLE, hFile), OBFH_PTR(LPCVOID, lpBuffer), obfh_int_proxy(nNumberOfBytesToWrite), OBFH_PTR(LPDWORD, lpNumberOfBytesWritten), OBFH_PTR(LPOVERLAPPED, lpOverlapped))

#define CloseHandle(hObject) \
    CloseHandle(OBFH_PTR(HANDLE, hObject))

#define GetModuleHandle(lpModuleName) \
    OBFH_WINAPI(GetModuleHandle)      \
    (OBFH_PTR(LPCTSTR, lpModuleName))

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

static int obfh_abs_proxy(int value) {
    BREAK_STACK_CFLOW;
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

#define fma(x, y, z) fma(_MUTATE_MATH(x), _MUTATE_MATH(y), _MUTATE_MATH(z))
#define nexttoward(x, y) nexttoward(_MUTATE_MATH(x), _MUTATE_MATH(y))
#define remquo(x, y, z) remquo(_MUTATE_MATH(x), _MUTATE_MATH(y), (z))
#define nextafter(x, y) nextafter(_MUTATE_MATH(x), _MUTATE_MATH(y))
#define remainder(x, y) remainder(_MUTATE_MATH(x), _MUTATE_MATH(y))
#define scalbn(x, y) scalbn(_MUTATE_MATH(x), _MUTATE_MATH(y))
#define copysign(x, y) copysign(_MUTATE_MATH(x), _MUTATE_MATH(y))
#define atan2(y, x) ({ enum { __obfh_math_order = OBFH_MIX_B(OBFH_JUNK_WORD) & 1u }; char __obfh_math_name[6]; __builtin_choose_expr(__obfh_math_order, (__obfh_math_name[0] = _a, __obfh_math_name[1] = _t, __obfh_math_name[2] = _a, __obfh_math_name[3] = _n, __obfh_math_name[4] = (_2 + '0'), __obfh_math_name[5] = _0), (__obfh_math_name[5] = _0, __obfh_math_name[4] = (_2 + '0'), __obfh_math_name[3] = _n, __obfh_math_name[2] = _a, __obfh_math_name[1] = _t, __obfh_math_name[0] = _a)); ((double (*)(double, double))obfh_crt_resolve(__obfh_math_name))(y, x); })
#define ldexp(x, y) ({ enum { __obfh_math_order = OBFH_MIX_B(OBFH_JUNK_WORD) & 1u }; char __obfh_math_name[6]; __builtin_choose_expr(__obfh_math_order, (__obfh_math_name[0] = _l, __obfh_math_name[1] = _d, __obfh_math_name[2] = _e, __obfh_math_name[3] = _x, __obfh_math_name[4] = _p, __obfh_math_name[5] = _0), (__obfh_math_name[5] = _0, __obfh_math_name[4] = _p, __obfh_math_name[3] = _x, __obfh_math_name[2] = _e, __obfh_math_name[1] = _d, __obfh_math_name[0] = _l)); ((double (*)(double, int))obfh_crt_resolve(__obfh_math_name))(x, y); })
#define scalbln(x, y) scalbln(_MUTATE_MATH(x), _MUTATE_MATH(y))
#define hypot(x, y) hypot(_MUTATE_MATH(x), _MUTATE_MATH(y))
#define fmod(x, y) ({ enum { __obfh_math_order = OBFH_MIX_B(OBFH_JUNK_WORD) & 1u }; char __obfh_math_name[5]; __builtin_choose_expr(__obfh_math_order, (__obfh_math_name[0] = _f, __obfh_math_name[1] = _m, __obfh_math_name[2] = _o, __obfh_math_name[3] = _d, __obfh_math_name[4] = _0), (__obfh_math_name[4] = _0, __obfh_math_name[3] = _d, __obfh_math_name[2] = _o, __obfh_math_name[1] = _m, __obfh_math_name[0] = _f)); ((double (*)(double, double))obfh_crt_resolve(__obfh_math_name))(x, y); })
#define fdim(x, y) fdim(_MUTATE_MATH(x), _MUTATE_MATH(y))
#define fmax(x, y) fmax(_MUTATE_MATH(x), _MUTATE_MATH(y))
#define fmin(x, y) fmin(_MUTATE_MATH(x), _MUTATE_MATH(y))
#define pow(x, y) ({ enum { __obfh_math_order = OBFH_MIX_B(OBFH_JUNK_WORD) & 1u }; char __obfh_math_name[4]; __builtin_choose_expr(__obfh_math_order, (__obfh_math_name[0] = _p, __obfh_math_name[1] = _o, __obfh_math_name[2] = _w, __obfh_math_name[3] = _0), (__obfh_math_name[3] = _0, __obfh_math_name[2] = _w, __obfh_math_name[1] = _o, __obfh_math_name[0] = _p)); ((double (*)(double, double))obfh_crt_resolve(__obfh_math_name))(x, y); })
#define frexp(x, y) ({ enum { __obfh_math_order = OBFH_MIX_B(OBFH_JUNK_WORD) & 1u }; char __obfh_math_name[6]; __builtin_choose_expr(__obfh_math_order, (__obfh_math_name[0] = _f, __obfh_math_name[1] = _r, __obfh_math_name[2] = _e, __obfh_math_name[3] = _x, __obfh_math_name[4] = _p, __obfh_math_name[5] = _0), (__obfh_math_name[5] = _0, __obfh_math_name[4] = _p, __obfh_math_name[3] = _x, __obfh_math_name[2] = _e, __obfh_math_name[1] = _r, __obfh_math_name[0] = _f)); ((double (*)(double, int *))obfh_crt_resolve(__obfh_math_name))(x, y); })
#define modf(x, y) ({ enum { __obfh_math_order = OBFH_MIX_B(OBFH_JUNK_WORD) & 1u }; char __obfh_math_name[5]; __builtin_choose_expr(__obfh_math_order, (__obfh_math_name[0] = _m, __obfh_math_name[1] = _o, __obfh_math_name[2] = _d, __obfh_math_name[3] = _f, __obfh_math_name[4] = _0), (__obfh_math_name[4] = _0, __obfh_math_name[3] = _f, __obfh_math_name[2] = _d, __obfh_math_name[1] = _o, __obfh_math_name[0] = _m)); ((double (*)(double, double *))obfh_crt_resolve(__obfh_math_name))(x, y); })
#define nearbyint(x) nearbyint(_MUTATE_MATH(x))
#define lgamma(x) lgamma(_MUTATE_MATH(x))
#define tgamma(x) tgamma(_MUTATE_MATH(x))
#define log10(x) ({ enum { __obfh_math_order = OBFH_MIX_B(OBFH_JUNK_WORD) & 1u }; char __obfh_math_name[6]; __builtin_choose_expr(__obfh_math_order, (__obfh_math_name[0] = _l, __obfh_math_name[1] = _o, __obfh_math_name[2] = _g, __obfh_math_name[3] = (_1 + '0'), __obfh_math_name[4] = (_0 + '0'), __obfh_math_name[5] = _0), (__obfh_math_name[5] = _0, __obfh_math_name[4] = (_0 + '0'), __obfh_math_name[3] = (_1 + '0'), __obfh_math_name[2] = _g, __obfh_math_name[1] = _o, __obfh_math_name[0] = _l)); ((double (*)(double))obfh_crt_resolve(__obfh_math_name))(x); })
#define floor(x) ({ enum { __obfh_math_order = OBFH_MIX_B(OBFH_JUNK_WORD) & 1u }; char __obfh_math_name[6]; __builtin_choose_expr(__obfh_math_order, (__obfh_math_name[0] = _f, __obfh_math_name[1] = _l, __obfh_math_name[2] = _o, __obfh_math_name[3] = _o, __obfh_math_name[4] = _r, __obfh_math_name[5] = _0), (__obfh_math_name[5] = _0, __obfh_math_name[4] = _r, __obfh_math_name[3] = _o, __obfh_math_name[2] = _o, __obfh_math_name[1] = _l, __obfh_math_name[0] = _f)); ((double (*)(double))obfh_crt_resolve(__obfh_math_name))(x); })
#define expm1(x) expm1(_MUTATE_MATH(x))
#define log1p(x) log1p(_MUTATE_MATH(x))
#define acosh(x) acosh(_MUTATE_MATH(x))
#define asinh(x) asinh(_MUTATE_MATH(x))
#define atanh(x) atanh(_MUTATE_MATH(x))
#define ilogb(x) ilogb(_MUTATE_MATH(x))
#define round(x) round(_MUTATE_MATH(x))
#define trunc(x) trunc(_MUTATE_MATH(x))
#define ceil(x) ({ enum { __obfh_math_order = OBFH_MIX_B(OBFH_JUNK_WORD) & 1u }; char __obfh_math_name[5]; __builtin_choose_expr(__obfh_math_order, (__obfh_math_name[0] = _c, __obfh_math_name[1] = _e, __obfh_math_name[2] = _i, __obfh_math_name[3] = _l, __obfh_math_name[4] = _0), (__obfh_math_name[4] = _0, __obfh_math_name[3] = _l, __obfh_math_name[2] = _i, __obfh_math_name[1] = _e, __obfh_math_name[0] = _c)); ((double (*)(double))obfh_crt_resolve(__obfh_math_name))(x); })
#define fabs(x) ({ enum { __obfh_math_order = OBFH_MIX_B(OBFH_JUNK_WORD) & 1u }; char __obfh_math_name[5]; __builtin_choose_expr(__obfh_math_order, (__obfh_math_name[0] = _f, __obfh_math_name[1] = _a, __obfh_math_name[2] = _b, __obfh_math_name[3] = _s, __obfh_math_name[4] = _0), (__obfh_math_name[4] = _0, __obfh_math_name[3] = _s, __obfh_math_name[2] = _b, __obfh_math_name[1] = _a, __obfh_math_name[0] = _f)); ((double (*)(double))obfh_crt_resolve(__obfh_math_name))(x); })
#define sqrt(x) ({ enum { __obfh_math_order = OBFH_MIX_B(OBFH_JUNK_WORD) & 1u }; char __obfh_math_name[5]; __builtin_choose_expr(__obfh_math_order, (__obfh_math_name[0] = _s, __obfh_math_name[1] = _q, __obfh_math_name[2] = _r, __obfh_math_name[3] = _t, __obfh_math_name[4] = _0), (__obfh_math_name[4] = _0, __obfh_math_name[3] = _t, __obfh_math_name[2] = _r, __obfh_math_name[1] = _q, __obfh_math_name[0] = _s)); ((double (*)(double))obfh_crt_resolve(__obfh_math_name))(x); })
#define asin(x) ({ enum { __obfh_math_order = OBFH_MIX_B(OBFH_JUNK_WORD) & 1u }; char __obfh_math_name[5]; __builtin_choose_expr(__obfh_math_order, (__obfh_math_name[0] = _a, __obfh_math_name[1] = _s, __obfh_math_name[2] = _i, __obfh_math_name[3] = _n, __obfh_math_name[4] = _0), (__obfh_math_name[4] = _0, __obfh_math_name[3] = _n, __obfh_math_name[2] = _i, __obfh_math_name[1] = _s, __obfh_math_name[0] = _a)); ((double (*)(double))obfh_crt_resolve(__obfh_math_name))(x); })
#define acos(x) ({ enum { __obfh_math_order = OBFH_MIX_B(OBFH_JUNK_WORD) & 1u }; char __obfh_math_name[5]; __builtin_choose_expr(__obfh_math_order, (__obfh_math_name[0] = _a, __obfh_math_name[1] = _c, __obfh_math_name[2] = _o, __obfh_math_name[3] = _s, __obfh_math_name[4] = _0), (__obfh_math_name[4] = _0, __obfh_math_name[3] = _s, __obfh_math_name[2] = _o, __obfh_math_name[1] = _c, __obfh_math_name[0] = _a)); ((double (*)(double))obfh_crt_resolve(__obfh_math_name))(x); })
#define atan(x) ({ enum { __obfh_math_order = OBFH_MIX_B(OBFH_JUNK_WORD) & 1u }; char __obfh_math_name[5]; __builtin_choose_expr(__obfh_math_order, (__obfh_math_name[0] = _a, __obfh_math_name[1] = _t, __obfh_math_name[2] = _a, __obfh_math_name[3] = _n, __obfh_math_name[4] = _0), (__obfh_math_name[4] = _0, __obfh_math_name[3] = _n, __obfh_math_name[2] = _a, __obfh_math_name[1] = _t, __obfh_math_name[0] = _a)); ((double (*)(double))obfh_crt_resolve(__obfh_math_name))(x); })
#define sinh(x) ({ enum { __obfh_math_order = OBFH_MIX_B(OBFH_JUNK_WORD) & 1u }; char __obfh_math_name[5]; __builtin_choose_expr(__obfh_math_order, (__obfh_math_name[0] = _s, __obfh_math_name[1] = _i, __obfh_math_name[2] = _n, __obfh_math_name[3] = _h, __obfh_math_name[4] = _0), (__obfh_math_name[4] = _0, __obfh_math_name[3] = _h, __obfh_math_name[2] = _n, __obfh_math_name[1] = _i, __obfh_math_name[0] = _s)); ((double (*)(double))obfh_crt_resolve(__obfh_math_name))(x); })
#define cosh(x) ({ enum { __obfh_math_order = OBFH_MIX_B(OBFH_JUNK_WORD) & 1u }; char __obfh_math_name[5]; __builtin_choose_expr(__obfh_math_order, (__obfh_math_name[0] = _c, __obfh_math_name[1] = _o, __obfh_math_name[2] = _s, __obfh_math_name[3] = _h, __obfh_math_name[4] = _0), (__obfh_math_name[4] = _0, __obfh_math_name[3] = _h, __obfh_math_name[2] = _s, __obfh_math_name[1] = _o, __obfh_math_name[0] = _c)); ((double (*)(double))obfh_crt_resolve(__obfh_math_name))(x); })
#define tanh(x) ({ enum { __obfh_math_order = OBFH_MIX_B(OBFH_JUNK_WORD) & 1u }; char __obfh_math_name[5]; __builtin_choose_expr(__obfh_math_order, (__obfh_math_name[0] = _t, __obfh_math_name[1] = _a, __obfh_math_name[2] = _n, __obfh_math_name[3] = _h, __obfh_math_name[4] = _0), (__obfh_math_name[4] = _0, __obfh_math_name[3] = _h, __obfh_math_name[2] = _n, __obfh_math_name[1] = _a, __obfh_math_name[0] = _t)); ((double (*)(double))obfh_crt_resolve(__obfh_math_name))(x); })
#define ceil(x) ({ enum { __obfh_math_order = OBFH_MIX_B(OBFH_JUNK_WORD) & 1u }; char __obfh_math_name[5]; __builtin_choose_expr(__obfh_math_order, (__obfh_math_name[0] = _c, __obfh_math_name[1] = _e, __obfh_math_name[2] = _i, __obfh_math_name[3] = _l, __obfh_math_name[4] = _0), (__obfh_math_name[4] = _0, __obfh_math_name[3] = _l, __obfh_math_name[2] = _i, __obfh_math_name[1] = _e, __obfh_math_name[0] = _c)); ((double (*)(double))obfh_crt_resolve(__obfh_math_name))(x); })
#define fabs(x) ({ enum { __obfh_math_order = OBFH_MIX_B(OBFH_JUNK_WORD) & 1u }; char __obfh_math_name[5]; __builtin_choose_expr(__obfh_math_order, (__obfh_math_name[0] = _f, __obfh_math_name[1] = _a, __obfh_math_name[2] = _b, __obfh_math_name[3] = _s, __obfh_math_name[4] = _0), (__obfh_math_name[4] = _0, __obfh_math_name[3] = _s, __obfh_math_name[2] = _b, __obfh_math_name[1] = _a, __obfh_math_name[0] = _f)); ((double (*)(double))obfh_crt_resolve(__obfh_math_name))(x); })
#define erfc(x) erfc(_MUTATE_MATH(x))
#define log2(x) log2(_MUTATE_MATH(x))
#define cbrt(x) cbrt(_MUTATE_MATH(x))
#define exp2(x) exp2(_MUTATE_MATH(x))
#define logb(x) logb(_MUTATE_MATH(x))
#define rint(x) rint(_MUTATE_MATH(x))
#define exp(x) ({ enum { __obfh_math_order = OBFH_MIX_B(OBFH_JUNK_WORD) & 1u }; char __obfh_math_name[4]; __builtin_choose_expr(__obfh_math_order, (__obfh_math_name[0] = _e, __obfh_math_name[1] = _x, __obfh_math_name[2] = _p, __obfh_math_name[3] = _0), (__obfh_math_name[3] = _0, __obfh_math_name[2] = _p, __obfh_math_name[1] = _x, __obfh_math_name[0] = _e)); ((double (*)(double))obfh_crt_resolve(__obfh_math_name))(x); })
#define log(x) ({ enum { __obfh_math_order = OBFH_MIX_B(OBFH_JUNK_WORD) & 1u }; char __obfh_math_name[4]; __builtin_choose_expr(__obfh_math_order, (__obfh_math_name[0] = _l, __obfh_math_name[1] = _o, __obfh_math_name[2] = _g, __obfh_math_name[3] = _0), (__obfh_math_name[3] = _0, __obfh_math_name[2] = _g, __obfh_math_name[1] = _o, __obfh_math_name[0] = _l)); ((double (*)(double))obfh_crt_resolve(__obfh_math_name))(x); })
#define sin(x) ({ enum { __obfh_math_order = OBFH_MIX_B(OBFH_JUNK_WORD) & 1u }; char __obfh_math_name[4]; __builtin_choose_expr(__obfh_math_order, (__obfh_math_name[0] = _s, __obfh_math_name[1] = _i, __obfh_math_name[2] = _n, __obfh_math_name[3] = _0), (__obfh_math_name[3] = _0, __obfh_math_name[2] = _n, __obfh_math_name[1] = _i, __obfh_math_name[0] = _s)); ((double (*)(double))obfh_crt_resolve(__obfh_math_name))(x); })
#define cos(x) ({ enum { __obfh_math_order = OBFH_MIX_B(OBFH_JUNK_WORD) & 1u }; char __obfh_math_name[4]; __builtin_choose_expr(__obfh_math_order, (__obfh_math_name[0] = _c, __obfh_math_name[1] = _o, __obfh_math_name[2] = _s, __obfh_math_name[3] = _0), (__obfh_math_name[3] = _0, __obfh_math_name[2] = _s, __obfh_math_name[1] = _o, __obfh_math_name[0] = _c)); ((double (*)(double))obfh_crt_resolve(__obfh_math_name))(x); })
#define tan(x) ({ enum { __obfh_math_order = OBFH_MIX_B(OBFH_JUNK_WORD) & 1u }; char __obfh_math_name[4]; __builtin_choose_expr(__obfh_math_order, (__obfh_math_name[0] = _t, __obfh_math_name[1] = _a, __obfh_math_name[2] = _n, __obfh_math_name[3] = _0), (__obfh_math_name[3] = _0, __obfh_math_name[2] = _n, __obfh_math_name[1] = _a, __obfh_math_name[0] = _t)); ((double (*)(double))obfh_crt_resolve(__obfh_math_name))(x); })
#define erf(x) erf(_MUTATE_MATH(x))

__declspec(dllexport) __attribute__((weak)) char *WhatSoundDoesACowMake() OBFH_CODE_SECTION_ATTRIBUTE {
    return HIDE_STRING("Moo");
}

#else
#warning Obfuscation disabled!
#endif
#endif

// ;)
