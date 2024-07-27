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

*/

#ifndef OBFH
#define OBFH

#if !__TINYC__ && !__GNUC__ && !__MINGW32__
#define __attribute__(...)
#endif

// if virtualization disabled
#if NO_OBF == 1 || VIRT != 1
#define VM_ADD(num1, num2) num1 + num2
#define VM_SUB(num1, num2) num1 - num2
#define VM_MUL(num1, num2) num1 *num2
#define VM_DIV(num1, num2) num1 / num2
#define VM_MOD(num1, num2) num1 % num2
#define VM_EQU(num1, num2) num1 == num2
#define VM_NEQ(num1, num2) num1 != num2
#define VM_LSS(num1, num2) num1 < num2
#define VM_GTR(num1, num2) num1 > num2
#define VM_LEQ(num1, num2) num1 <= num2
#define VM_GEQ(num1, num2) num1 >= num2
#define VM_ADD_DBL(num1, num2) num1 + num2
#define VM_SUB_DBL(num1, num2) num1 - num2
#define VM_MUL_DBL(num1, num2) num1 *num2
#define VM_DIV_DBL(num1, num2) num1 / num2
#define VM_LSS_DBL(num1, num2) num1 < num2
#define VM_GTR_DBL(num1, num2) num1 > num2
#define VM_IF(condition) if (condition)
#define VM_ELSE_IF(condition) else if (condition)
#define VM_ELSE else
#endif

#if !NO_OBF

#include <conio.h>
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

// Fake signatures ;)
#if defined(FAKE_SIGNS) && (FAKE_SIGNS != 0)

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

#else

#define OBFH_SECTION_ATTRIBUTE SECTION_ATTRIBUTE(".obfh")  // OBFH section

#endif

// Thanks to @horsicq && @ac3ss0r
#define RND(min, max) (min + (((__COUNTER__ + (__LINE__ * __LINE__)) * 2654435761u) % (max - min + 1)))

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

#define __obfh_asm__(...) __asm__ __volatile(__VA_ARGS__)

#define BREAK_STACK_1       \
    __obfh_asm__(           \
        "xorl %eax, %eax\n" \
        "jz 1f\n"           \
        ".byte 0xE8\n"      \
        "1:\n")

#define BREAK_STACK_2 \
    if (_0) __obfh_asm__(".byte 0x00\n");

#define BREAK_STACK_3                     \
    switch (_0) {                         \
        case RND(1, 1000):                \
            __obfh_asm__(".byte 0x00\n"); \
    }

#define BREAK_STACK_4       \
    __obfh_asm__(           \
        "xorl %ebx, %ebx\n" \
        "xorl %edx, %edx\n" \
        "xorl %ebx, %edx\n" \
        "jz 1f\n"           \
        "mov %eax, 4\n"     \
        ".byte 0x00\n"      \
        "1:\n")

#define BREAK_STACK_5       \
    __obfh_asm__(           \
        "xorl %ebx, %ebx\n" \
        "xorl %eax, %eax\n" \
        "mov %eax, %ebx\n"  \
        "mov %edx, %ebx\n"  \
        "xorl %eax, %edx\n" \
        "jz 1f\n"           \
        ".byte 0x20\n"      \
        "1:\n")

#define BREAK_STACK_6       \
    __obfh_asm__(           \
        "xorl %edx, %edx\n" \
        "xorl %eax, %eax\n" \
        "mov %eax, %edx\n"  \
        "jz 1f\n"           \
        ".byte 0xE8\n"      \
        "1:\n")

#define BREAK_STACK_7       \
    __obfh_asm__(           \
        "xorl %edx, %edx\n" \
        "jz 1f\n"           \
        ".byte 0xE8\n"      \
        "1:\n")

#define BREAK_STACK_8       \
    __obfh_asm__(           \
        "xorl %eax, %eax\n" \
        "jz 1f\n"           \
        ".byte 0x50\n"      \
        "1:\n")

#define BREAK_STACK_9        \
    __obfh_asm__(            \
        "xorl %edx, %edx\n"  \
        "jz 1f\n"            \
        ".byte 0x00, 0x00\n" \
        "1:\n")

void junkFunc(int z, ...) OBFH_SECTION_ATTRIBUTE {
    __obfh_asm__("nop");
    return;
}

void junkFuncEmpty() {
    BREAK_STACK_5;
    __obfh_asm__("nop");
    return;
}

#define __CRASH                 \
    __obfh_asm__(".byte 0xED"); \
    __obfh_asm__("int $3");     \
    exit(1);

#define TRUE ((((_9 + _7 + (RND(0, 1000) * _0))) / _8) - _1)
#define FALSE (((_3 + _6 + (RND(0, 1000) * _0)) - _9) * RND(0, 255))

#define FAKE_CPUID __obfh_asm__("nop\ncpuid\nnop\n")

// Doesn't work
// #define sizeof(x) \
//    ((sizeof(x) * _1 * _2 * _4 / _8) + (sizeof(x) * _2 * _4 / _8) + (RND(0, 1000) * _0)) / 2

#define NOP_FLOOD                             \
    (RND(0, 1000)) + int_Proxy(RND(0, 1000)); \
    if (junkFunc) {                           \
        __obfh_asm__("nop");                  \
    }                                         \
    do {                                      \
        __obfh_asm__(                         \
            "nop\n"                           \
            "nop");                           \
    } while (RND(0, 200) * _0)

int malloc_Proxy(int *size) {
    BREAK_STACK_1;
    return malloc(size);
}
#define malloc(...) malloc_Proxy(__VA_ARGS__)

static char rndValueToProxy = RND(0, 10);

int int_Proxy(int value) OBFH_SECTION_ATTRIBUTE {
    BREAK_STACK_4;
    if (rndValueToProxy == value)
        return rndValueToProxy;

    junkFunc(RND(0, 100000), RND(0, 100000));

    FAKE_CPUID;

    return ((value * _1) + ((_4 * RND(0, 100000)) - _8) * _0);
}

double double_Proxy(double value) OBFH_SECTION_ATTRIBUTE {
    BREAK_STACK_1;
    junkFunc(RND(0, 1000), RND(0, 1000));
    FAKE_CPUID;
    return (value * _1);
}

int condition_True() OBFH_SECTION_ATTRIBUTE {
    BREAK_STACK_1;
    FAKE_CPUID;
    return _1 && TRUE;
}

int condition_Proxy(int junk, int condition) OBFH_SECTION_ATTRIBUTE {
    BREAK_STACK_4;

    int result = int_Proxy(condition * _1);
    if (result == (FALSE * junk)) {
        return _8 - (_4 * _2) && !condition_True();
    } else if (result == TRUE) {
        return (condition_True() || FALSE || TRUE) && (FALSE + int_Proxy(_1));
    }

    BREAK_STACK_1;
    return int_Proxy(condition);
}

// =============================================================
// Anti-Tamper for Control-Flow obfuscation (beta!)
#if antitamper == 1 && NO_CFLOW != 1
int obfhIsBlockValidated = 0;
int validateBlock() {  // returns false
    obfhIsBlockValidated = 1;
    return obfhIsBlockValidated;
}
int isBlockValidated() {  // returns true if validateBlock() executed
    if (obfhIsBlockValidated) {
        obfhIsBlockValidated = 0;
        return 1;
    }
    return 0;
}
#else
#define validateBlock() 1
#define isBlockValidated() 1
#endif
// =============================================================

// =============================================================
// Control Flow (global)
#if NO_CFLOW != 1

#if CFLOW_V2 == 1  // Control flow obfuscation for 'if' & 'for', V2 (strong!)

// if (V2)
#define if(condition)                                         \
    if (validateBlock() && int_Proxy(RND(1, 1000000)) < _0) { \
        __obfh_asm__(".byte 0x00");                           \
    } else if (int_Proxy((RND(0, 1000)) > _0 && (RND(2, 1000) > condition_True() && condition_Proxy(RND(0, 1000000), condition) && RND(1, 99999999) > _0 && (int_Proxy(RND(0, 1000)) < RND(1001, 100000000)))) * TRUE && isBlockValidated())

// for (V2)
#define for(data) for (data && int_Proxy(TRUE * (RND(0, 1000000))) + FALSE || TRUE)

// return
#define return \
    for (int _i = _0; _i < RND(1, 100); _i++) return

// break
#define break \
    if (int_Proxy(_1)) break

#else

// Control flow obfuscation for 'if' & 'for', V1

// if (V1)
#define if(condition) if (validateBlock() && (RND(0, 1000)) > _0 && (RND(2, 1000) > condition_True() && condition_Proxy(RND(0, 1000000), condition) && RND(1, 9999999) > _0 && (int_Proxy(RND(0, 1000)) < RND(1001, 100000000))) && isBlockValidated())

// for (V1)
#define for(data) for (data && int_Proxy(TRUE * (RND(0, 10000))) + FALSE || _1)

#endif

// else
#define else                                                                      \
    else if (_0 > RND(1, 1000)) {                                                 \
        junkFunc(RND(0, 1000));                                                   \
        __obfh_asm__(".byte 0x3C");                                               \
    }                                                                             \
    else if (RND(0, 10) == (RND(11, 100))) {                                      \
        BREAK_STACK_3;                                                            \
        int_Proxy(_3 - RND(0, 10000));                                            \
    }                                                                             \
    else if (FALSE * RND(0, 1000)) {                                              \
        BREAK_STACK_1;                                                            \
    }                                                                             \
    else if (FALSE * (int_Proxy(RND(0, 1000)) ? RND(1, 99999) : RND(1, 99999))) { \
        __obfh_asm__(".byte 0xEB");                                               \
    }                                                                             \
    else

// while
#define while(condition) while ((RND(0, 1000)) > _0 && _8 > _3 && condition_True() && RND(1, 9999999999) > _0 && condition_Proxy(RND(0, 1000), condition) && _5)

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
    OP__NOP = RND(11000, 11900) * __COUNTER__ * 5
} CMD;

typedef enum {
    SALT_CMD = RND(100, 900),
    SALT_NUM1 = RND(16, 48),
    SALT_NUM2 = RND(16, 48)
} SALT;

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

#define VM_ADD(num1, num2) (long)Obfh_VirtualMachine(_VM_DEMUTATOR_KEY, _ENC_OP__ADD, num1 * -1 + SALT_NUM1, RND(1, 500), num2 * -1 + SALT_NUM2, RND(1, 500))
#define VM_SUB(num1, num2) (long)Obfh_VirtualMachine(_VM_DEMUTATOR_KEY, _ENC_OP__SUB, num1 * -1 + SALT_NUM1, RND(1, 500), num2 * -1 + SALT_NUM2, RND(1, 500))
#define VM_MUL(num1, num2) (long)Obfh_VirtualMachine(_VM_DEMUTATOR_KEY, _ENC_OP__MUL, num1 * -1 + SALT_NUM1, RND(1, 500), num2 * -1 + SALT_NUM2, RND(1, 500))
#define VM_DIV(num1, num2) (long)Obfh_VirtualMachine(_VM_DEMUTATOR_KEY, _ENC_OP__DIV, num1 * -1 + SALT_NUM1, RND(1, 500), num2 * -1 + SALT_NUM2, RND(1, 500))
#define VM_MOD(num1, num2) (long)Obfh_VirtualMachine(_VM_DEMUTATOR_KEY, _ENC_OP__MOD, num1 * -1 + SALT_NUM1, RND(1, 500), num2 * -1 + SALT_NUM2, RND(1, 500))
#define VM_EQU(num1, num2) (long)Obfh_VirtualMachine(_VM_DEMUTATOR_KEY, _ENC_OP__EQU, num1 * -1 + SALT_NUM1, RND(1, 500), num2 * -1 + SALT_NUM2, RND(1, 500))
#define VM_NEQ(num1, num2) (long)Obfh_VirtualMachine(_VM_DEMUTATOR_KEY, _ENC_OP__NEQ, num1 * -1 + SALT_NUM1, RND(1, 500), num2 * -1 + SALT_NUM2, RND(1, 500))
#define VM_LSS(num1, num2) (long)Obfh_VirtualMachine(_VM_DEMUTATOR_KEY, _ENC_OP__LSS, num1 * -1 + SALT_NUM1, RND(1, 500), num2 * -1 + SALT_NUM2, RND(1, 500))
#define VM_GTR(num1, num2) (long)Obfh_VirtualMachine(_VM_DEMUTATOR_KEY, _ENC_OP__GTR, num1 * -1 + SALT_NUM1, RND(1, 500), num2 * -1 + SALT_NUM2, RND(1, 500))
#define VM_LEQ(num1, num2) (long)Obfh_VirtualMachine(_VM_DEMUTATOR_KEY, _ENC_OP__LEQ, num1 * -1 + SALT_NUM1, RND(1, 500), num2 * -1 + SALT_NUM2, RND(1, 500))
#define VM_GEQ(num1, num2) (long)Obfh_VirtualMachine(_VM_DEMUTATOR_KEY, _ENC_OP__GEQ, num1 * -1 + SALT_NUM1, RND(1, 500), num2 * -1 + SALT_NUM2, RND(1, 500))
#define VM_OBF_INT(num1) (VM_MUL(RND(1, 999), 0) ? RND(1, 9999) : (long)Obfh_VirtualMachine(_VM_DEMUTATOR_KEY, _ENC_OP__NOP, num1 * -1 + SALT_NUM1, RND(1, 500), RND(1, 99999999) * -1 + SALT_NUM2, RND(1, 500)))

#define VM_ADD_DBL(num1, num2) Obfh_VirtualMachine(_VM_DEMUTATOR_KEY, _ENC_OP__ADD, (double)num1 * -1 + SALT_NUM1, RND(1, 500), (double)num2 * -1 + SALT_NUM2, RND(1, 500))
#define VM_SUB_DBL(num1, num2) Obfh_VirtualMachine(_VM_DEMUTATOR_KEY, _ENC_OP__SUB, (double)num1 * -1 + SALT_NUM1, RND(1, 500), (double)num2 * -1 + SALT_NUM2, RND(1, 500))
#define VM_MUL_DBL(num1, num2) Obfh_VirtualMachine(_VM_DEMUTATOR_KEY, _ENC_OP__MUL, (double)num1 * -1 + SALT_NUM1, RND(1, 500), (double)num2 * -1 + SALT_NUM2, RND(1, 500))
#define VM_DIV_DBL(num1, num2) Obfh_VirtualMachine(_VM_DEMUTATOR_KEY, _ENC_OP__DIV, (double)num1 * -1 + SALT_NUM1, RND(1, 500), (double)num2 * -1 + SALT_NUM2, RND(1, 500))
#define VM_LSS_DBL(num1, num2) (long)Obfh_VirtualMachine(_VM_DEMUTATOR_KEY, _ENC_OP__LSS, (double)num1 * -1 + SALT_NUM1, RND(1, 500), (double)num2 * -1 + SALT_NUM2, RND(1, 500))
#define VM_GTR_DBL(num1, num2) (long)Obfh_VirtualMachine(_VM_DEMUTATOR_KEY, _ENC_OP__GTR, (double)num1 * -1 + SALT_NUM1, RND(1, 500), (double)num2 * -1 + SALT_NUM2, RND(1, 500))
#define VM_OBF_DBL(num1) (VM_MUL(RND(1, 999), 0) ? RND(1, 9999) : Obfh_VirtualMachine(_VM_DEMUTATOR_KEY, _ENC_OP__NOP, num1 * -1 + SALT_NUM1, RND(1, 500), RND(1, 99999999) * -1 + SALT_NUM2, RND(1, 500)))

#define VM_IF(condition) if (!VM_EQU((int)(condition), VM_MUL(RND(111111, 999999), 0)))
#define VM_ELSE_IF(condition) else if (!VM_EQU((int)(condition), VM_MUL(0, RND(111111, 999999))))
#define VM_ELSE else if (VM_EQU(1, _1))

long double obfhVmResult = 0;
long double Obfh_VirtualMachine(long double uni_key, int command, long double num1, long double junk_2, long double num2, long double junk_3) OBFH_SECTION_ATTRIBUTE {
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
        case -1:
            goto restoreCommand;
        case -2:
            goto firstFakePoint;
        case -3:
            return _0 * ~_1 + junk_2;
        case -4:
            goto restoreNum2;
        case -5:
            goto restoreNum1;
        case -6:
            __obfh_asm__(".byte 0xFF, 0x25");  // fake JMP
        case OP__ADD:                          // plus
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
            if (num2 != 0)
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
            // result = num1 >= num2;
            goto afterCalc;
        case OP__NOP:
            obfhVmResult = num1;
            goto afterCalc;
        default:
            // printf("ADD: %d, CMD: %d\n", OP__ADD, command);
            obfhVmResult = _0 * (uni_key * _5);
            __obfh_asm__(".byte 0x00");  // junk
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

    __obfh_asm__(".byte 0xFF, 0xE0");  // fake JMP EAX

secondFakePoint:
    BREAK_STACK_7;
    goto restoreCommand;
}
#endif
// =============================================================

char *getCharMask(int count) OBFH_SECTION_ATTRIBUTE {
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

    BREAK_STACK_8;

    FAKE_CPUID;

    return mask;
}

// WriteConsoleA
BOOL WriteConsoleA_Proxy(HANDLE hConsoleOutput, const void *lpBuffer, DWORD nNumberOfCharsToWrite, LPDWORD lpNumberOfCharsWritten, LPVOID lpReserved) OBFH_SECTION_ATTRIBUTE {
    BREAK_STACK_1;
    FAKE_CPUID;
    return WriteConsoleA(hConsoleOutput, lpBuffer, nNumberOfCharsToWrite, lpNumberOfCharsWritten, lpReserved);
}
#define WriteConsoleA(...) WriteConsoleA_Proxy(__VA_ARGS__)

// GetStdHandle
HANDLE GetStdHandle_Proxy(DWORD nStdHandle) OBFH_SECTION_ATTRIBUTE {
    BREAK_STACK_1;
    FAKE_CPUID;
    return GetStdHandle(int_Proxy(nStdHandle));
}
#define GetStdHandle(...) GetStdHandle_Proxy(__VA_ARGS__)

// GetProcAddress
FARPROC GetProcAddress_Proxy(HMODULE hModule, LPCSTR lpProcName) OBFH_SECTION_ATTRIBUTE {
    BREAK_STACK_1;
    FAKE_CPUID;
    return GetProcAddress(hModule, lpProcName);
}
#define GetProcAddress(...) GetProcAddress_Proxy(__VA_ARGS__)

HMODULE GetModuleHandleA_Proxy(LPCSTR lpModuleName) OBFH_SECTION_ATTRIBUTE {
    BREAK_STACK_9;
    FAKE_CPUID;
    return GetModuleHandleA(lpModuleName);
}
#define GetModuleHandleA(...) GetModuleHandleA_Proxy(__VA_ARGS__)

// strcmp
int strcmp_custom(const char *str1, const char *str2) OBFH_SECTION_ATTRIBUTE {
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
size_t strlen_custom(const char *str) OBFH_SECTION_ATTRIBUTE {
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

static char loadStr[5];
HMODULE LoadLibraryA_0(LPCSTR lpLibFileName) OBFH_SECTION_ATTRIBUTE {
    switch (_0) {
        case 1:
            __obfh_asm__(".byte 0x74");  // fake JE

            break;
        case 0:
            BREAK_STACK_3;
            // return LoadLibraryA(lpLibFileName);

            typedef HMODULE(WINAPI * LoadLibraryAFunc)(LPCSTR);
            static LoadLibraryAFunc loadLibraryA = NULL;
            if (loadLibraryA == NULL) {
                char libName[32];

                // kernel32
                sprintf(libName, strcat(getCharMask(_6), "%d"), _k, _e, _r, _n, _e, _l, (_4 * _8));

                HMODULE hKernel32 = GetModuleHandleA(libName);
                if (hKernel32 != NULL) {
                    FAKE_CPUID;
                    char _L_char = _L;
                    junkFunc(_0 + RND(1, 5));

                    if (loadStr[_3] != int_Proxy(_d)) {  // restore "Load"
                        loadStr[_4] = int_Proxy(_0);
                        loadStr[_3] = int_Proxy(_d);
                        loadStr[_2] = int_Proxy(_a);
                        BREAK_STACK_2;
                        loadStr[_1] = int_Proxy(_o);
                        loadStr[_0] = int_Proxy(_L);
                    }

                    char *funcName = malloc(32);

                    FAKE_CPUID;

                    sprintf(funcName, strcat("Library", "%c"), _A);  // _A = LoadLibrary{'A'}
                    loadLibraryA = (LoadLibraryAFunc)GetProcAddress(hKernel32, strcat(loadStr, funcName));
                    free(funcName);

#if MEM_CLEANER__JUST_FOR_FUN
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

WINAPI ThreadCompareDRs(void *p) {
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

int IsDebuggerPresent_Proxy() OBFH_SECTION_ATTRIBUTE {
    BREAK_STACK_1;
    NOP_FLOOD;
    BREAK_STACK_2;
#if ANTIDEBUG_V2 == 1

    // Registers validation
    HANDLE hMainThread;
    DWORD dwDummy, exitCode;

    DuplicateHandle(GetCurrentProcess(), GetCurrentThread(), GetCurrentProcess(),
                    &hMainThread, _0, FALSE, DUPLICATE_SAME_ACCESS);

    HANDLE hThread = CreateThread(NULL, _0, ThreadCompareDRs, hMainThread, _0, &dwDummy);
    if (hThread) {
        WaitForSingleObject(hThread, INFINITE);
        GetExitCodeThread(hThread, &exitCode);
        CloseHandle(hThread);
    }

    if (exitCode) return exitCode;

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

    // Standard antidebugger
    NOP_FLOOD;
    return IsDebuggerPresent();

#endif
}
// =============================================================

/*
void antiDebugMessage() {
    typedef int(WINAPI * MessageBoxAType)(HWND, LPCSTR, LPCSTR, UINT);
    MessageBoxAType MessageBoxA = (MessageBoxAType)GetProcAddress(LoadLibraryA("user32.dll"), "MessageBoxA");
    if (MessageBoxA != NULL) MessageBoxA(NULL, "Debugging prevented.", "", 0x10);
}
*/

void crash() {
    BREAK_STACK_1;
    __obfh_asm__("int $3");
    __obfh_asm__(".byte 0xED, 0x00");
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
        __obfh_asm__(".byte 0xED");                                                                \
        BREAK_STACK_1;                                                                             \
        __obfh_asm__(".byte 0x66, 0xC1, 0xE8, 0x05");                                              \
        __obfh_asm__(".byte 0x00");                                                                \
        __obfh_asm__("ret");                                                                       \
        crash();                                                                                   \
    } else {                                                                                       \
        0.0 / !IsDebuggerPresent();                                                                \
    };

#else
#define ANTI_DEBUG 0
#endif

char *getStdLibName() OBFH_SECTION_ATTRIBUTE {
    BREAK_STACK_1;
    NOP_FLOOD;
    junkFunc(_0 + _3);
    junkFunc(_3 - _2);

    // char *msvcrtName = malloc(_7);
    static char msvcrtName[7] = "\0\0\0\0\0\0\0";

    ANTI_DEBUG;

    msvcrtName[_3 + _2 + _1] = 0;
    BREAK_STACK_8;
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
    BREAK_STACK_9;
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
char *getReallocName_Proxy() OBFH_SECTION_ATTRIBUTE {
    BREAK_STACK_1;
    FAKE_CPUID;
    return "realloc";
}
#define realloc(...) ((void *(*)())GetProcAddress(LoadLibraryA_Proxy(getStdLibName_Proxy()), getReallocName_Proxy()))(__VA_ARGS__)

void *calloc_Proxy(size_t nmemb, size_t size) OBFH_SECTION_ATTRIBUTE {
    BREAK_STACK_1;
    return calloc(nmemb, size);
}
#define calloc(nmemb, size) calloc_Proxy(nmemb, size)

void *realloc_Proxy(void *ptr, size_t size) OBFH_SECTION_ATTRIBUTE {
    BREAK_STACK_1;
    return realloc(ptr, size);
}
#define realloc(ptr, size) realloc_Proxy(ptr, size)

char *gets_Proxy(char *s) OBFH_SECTION_ATTRIBUTE {
    BREAK_STACK_1;
    return gets(s);
}
#define gets(s) gets_Proxy(s)

int snprintf_Proxy(char *str, size_t size, const char *format, ...) OBFH_SECTION_ATTRIBUTE {
    BREAK_STACK_1;
    va_list args;
    va_start(args, format);
    int result = vsnprintf(str, size, format, args);
    va_end(args);
    return result;
}
#define snprintf(str, size, format, ...) snprintf_Proxy(str, size, format, __VA_ARGS__)

/*
#define printf(...) (([](...) -> int {                                                                        \
    static void (*printf_proxy)(const char *, ...) = NULL;                                                    \
    if (printf_proxy == NULL) {                                                                               \
        printf_proxy = (void (*)(const char *, ...))GetProcAddress(GetModuleHandleA("msvcrt.dll"), "printf"); \
    }                                                                                                         \
    return printf_proxy(__VA_ARGS__);                                                                         \
})(__VA_ARGS__))
*/

int vsprintf_Proxy(char *str, const char *format, va_list args) OBFH_SECTION_ATTRIBUTE {
    BREAK_STACK_1;
    return vsprintf(str, format, args);
}
#define vsprintf(str, format, args) vsprintf_Proxy(str, format, args)

int vsnprintf_Proxy(char *str, size_t size, const char *format, va_list args) OBFH_SECTION_ATTRIBUTE {
    BREAK_STACK_1;
    return vsnprintf(str, size, format, args);
}
#define vsnprintf(str, size, format, args) vsnprintf_Proxy(str, size, format, args)

char *getenv_Proxy(const char *name) OBFH_SECTION_ATTRIBUTE {
    BREAK_STACK_1;
    return getenv(name);
}
#define getenv(name) getenv_Proxy(name)

int system_Proxy(const char *command) OBFH_SECTION_ATTRIBUTE {
    BREAK_STACK_1;
    return system(command);
}
#define system(command) system_Proxy(command)

void abort_Proxy(void) OBFH_SECTION_ATTRIBUTE {
    BREAK_STACK_1;
    abort();
}
#define abort() abort_Proxy()

int atexit_Proxy(void (*func)(void)) OBFH_SECTION_ATTRIBUTE {
    BREAK_STACK_1;
    return atexit(func);
}
#define atexit(func) atexit_Proxy(func)

char *getcwd_Proxy(char *buf, size_t size) OBFH_SECTION_ATTRIBUTE {
    BREAK_STACK_1;
    return getcwd(buf, size);
}
#define getcwd(buf, size) ((char *)getcwd_Proxy(buf, size))

int tolower_Proxy(int c) OBFH_SECTION_ATTRIBUTE {
    BREAK_STACK_1;
    return tolower(c);
}
#define tolower(c) tolower_Proxy(c)

int toupper_Proxy(int c) OBFH_SECTION_ATTRIBUTE {
    BREAK_STACK_1;
    return toupper(c);
}
#define toupper(c) toupper_Proxy(c) * /

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
    CreateFileA(int_Proxy(lpFileName), int_Proxy(dwDesiredAccess), int_Proxy(dwShareMode), int_Proxy(lpSecurityAttributes), int_Proxy(dwCreationDisposition), int_Proxy(dwFlagsAndAttributes), int_Proxy(hTemplateFile))

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

#if virt_std == 1
#define _MUTATE_MATH(value) VM_MUL_DBL(VM_ADD_DBL(0, value), 1)
#else
#define _MUTATE_MATH(value) FALSE + value *TRUE
#endif

#define fma(x, y, z) fma(_MUTATE_MATH(x), _MUTATE_MATH(y), _MUTATE_MATH(z))
#define nexttoward(x, y) nexttoward(_MUTATE_MATH(x), _MUTATE_MATH(y))
#define nextafter(x, y) nextafter(_MUTATE_MATH(x), _MUTATE_MATH(y))
#define remainder(x, y) remainder(_MUTATE_MATH(x), _MUTATE_MATH(y))
#define copysign(x, y) copysign(_MUTATE_MATH(x), _MUTATE_MATH(y))
#define scalbln(x, y) scalbln(_MUTATE_MATH(x), _MUTATE_MATH(y))
#define remquo(x, y) remquo(_MUTATE_MATH(x), _MUTATE_MATH(y))
#define scalbn(x, y) scalbn(_MUTATE_MATH(x), _MUTATE_MATH(y))
#define atan2(y, x) atan2(_MUTATE_MATH(y), _MUTATE_MATH(x))
#define ldexp(x, y) ldexp(_MUTATE_MATH(x), _MUTATE_MATH(y))
#define frexp(x, y) frexp(_MUTATE_MATH(x), _MUTATE_MATH(y))
#define hypot(x, y) hypot(_MUTATE_MATH(x), _MUTATE_MATH(y))
#define fmod(x, y) fmod(_MUTATE_MATH(x), _MUTATE_MATH(y))
#define modf(x, y) modf(_MUTATE_MATH(x), _MUTATE_MATH(y))
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
#define nan(x) nan(_MUTATE_MATH(x))

__declspec(dllexport) char *WhatSoundDoesACowMake() OBFH_SECTION_ATTRIBUTE {
    return "Moo";
}

/*
#if __TINYC__
#define main(...) _start(__VA_ARGS__)
#endif
*/

#else
#define ANTI_DEBUG 0
#warning Obfuscation disabled!
#endif
#endif

// ;)
