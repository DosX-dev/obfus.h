/*
[Same operations on obfus.h with gcc compatibility]
~ Compile with: gcc -O2 -masm=intel your_file.c -o output.exe -w
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <stdarg.h>

#if defined _MSC_VER
#warning obfus.h optimized for GCC/MinGW. MSVC may have issues.
#endif

#if !defined __COUNTER__
#error You are using too old a compiler version!
#endif

#ifndef __asm__
#define __asm__(...)
#endif

#define SECTION_ATTRIBUTE(NAME) __attribute__((section(NAME)))

#define DATA_SECTION_ATTRIBUTE SECTION_ATTRIBUTE(".data")

// Fake signatures
#if FAKE_SIGNS == 1

static const char FAKE_ENIGMAVM_1[] SECTION_ATTRIBUTE(".enigma1") = {0};
static const char FAKE_ENIGMAVM_2[] SECTION_ATTRIBUTE(".enigma2") = {0};
static const char FAKE_VMPROTECT_1[] SECTION_ATTRIBUTE(".vmp0") = {0};
static const char FAKE_VMPROTECT_2[] SECTION_ATTRIBUTE(".vmp1") = {0};
static const char FAKE_VMPROTECT_3[] SECTION_ATTRIBUTE(".vmp2") = {0};

#define OBFH_SECTION_ATTRIBUTE SECTION_ATTRIBUTE("UPX0")
static const char FAKE_UPX[] OBFH_SECTION_ATTRIBUTE = {0};

static const char FAKE_THEMIDA[] SECTION_ATTRIBUTE(".winlice") = {0};
static const char FAKE_PETITE[] SECTION_ATTRIBUTE(".petite") = {0};
static const char FAKE_ENIGMA[] SECTION_ATTRIBUTE(".data") = {0x45, 0x6e, 0x69, 0x67,
                                                              0x6d, 0x61, 0x20, 0x70,
                                                              0x72, 0x6f, 0x74, 0x65,
                                                              0x63, 0x74, 0x6f, 0x72,
                                                              0x20, 0x76, 0x01};

#else

#define OBFH_SECTION_ATTRIBUTE  // Empty for GCC compatibility

#endif

// Compile-time random number generator
#define RND(min, max) \
    (min + (((__COUNTER__ + (__LINE__ * __LINE__)) * 2654435761u) % (max - min + 1)))

#define STACK_STRING(str) ((char[]){str})

// Forward declarations
int obfh_int_proxy(int value);
float obfh_condition_true(void);
long double __s_rdtsc(float junk, ...);
char *obfh_process_hidden_string(char *string, ...);

#define HIDE_STRING(str) \
    (_0 < RND(1, 255) ? obfh_process_hidden_string(STACK_STRING("\0" str "\0"), (float)__s_rdtsc(RND(0, 255)) != 0.1) : (char*)str)

typedef enum {
    SALT_SHIFT = RND(0xBAD, 0xBEEF)
} VAR_ADDR_SHIFT;

#define RET_BY_VAR(value)                                   \
    {                                                       \
        int _obfh_ret_val_shift = SALT_SHIFT,               \
            *_obfh_ret_val_addr = ((int*)&value + SALT_SHIFT);    \
        return *(_obfh_ret_val_addr - _obfh_ret_val_shift); \
    }

// Obfuscation data variables (without section attributes for GCC compatibility)
volatile static char _s_a[] = "a", _s_b[] = "b", _s_c[] = "c", _s_d[] = "d",
                     _s_e[] = "e", _s_f[] = "f", _s_g[] = "g", _s_h[] = "h",
                     _s_i[] = "i", _s_j[] = "j", _s_k[] = "k", _s_l[] = "l",
                     _s_m[] = "m", _s_n[] = "n", _s_o[] = "o", _s_p[] = "p",
                     _s_q[] = "q", _s_r[] = "r", _s_s[] = "s", _s_t[] = "t",
                     _s_u[] = "u", _s_v[] = "v", _s_w[] = "w", _s_x[] = "x",
                     _s_y[] = "y", _s_z[] = "z";

volatile static char _a = 'a', _b = 'b', _c = 'c', _d = 'd',
                     _e = 'e', _f = 'f', _g = 'g', _h = 'h',
                     _i = 'i', _j = 'j', _k = 'k', _l = 'l',
                     _m = 'm', _n = 'n', _o = 'o', _p = 'p',
                     _q = 'q', _r = 'r', _s = 's', _t = 't',
                     _u = 'u', _v = 'v', _w = 'w', _x = 'x',
                     _y = 'y', _z = 'z',
                     _S = 'S', _L = 'L', _A = 'A', _I = 'I',
                     _D = 'D', _P = 'P';

volatile static char _0 = 0, _1 = 1, _2 = 2, _3 = 3, _4 = 4,
                     _5 = 5, _6 = 6, _7 = 7, _8 = 8, _9 = 9;

#define __obfh_asm__(...) __asm__ __volatile__(__VA_ARGS__)

// Stack breaking macros (x86/x64 compatible)
#define BREAK_STACK_1      \
    __obfh_asm__(          \
        "xor eax, eax\n\t" \
        "jz 1f\n\t"           \
        ".byte 0xE8\n\t"      \
        "1:\n\t"               \
        ::: "eax")

#define BREAK_STACK_2 \
    if (_0) __obfh_asm__(".byte 0x00")

#define BREAK_STACK_3                          \
    switch (_0) {                              \
        case RND(1, 1000):                     \
            __obfh_asm__(".byte 0x00, 0x00"); \
    }

#define BREAK_STACK_4      \
    __obfh_asm__(          \
        "xor ebx, ebx\n\t" \
        "xor edx, edx\n\t" \
        "jz 1f\n\t"           \
        ".byte 0x00\n\t"      \
        "1:\n\t"               \
        ::: "ebx", "edx")

#define FAKE_CPUID __obfh_asm__("nop\n\t" ::: )

#if defined(__x86_64__) || defined(_M_X64)
#define BAD_JMP __obfh_asm__(".byte 0xFF, 0x25, 0xF1, 0xF2, 0xF3, 0xF4")
#else
#define BAD_JMP __obfh_asm__(".byte 0xEB, 0xE1")
#endif

#define BAD_CALL __obfh_asm__(".byte 0xB8")

// Function declarations
void obfh_junk_func_args(int z, ...) {
    BREAK_STACK_1;
    __obfh_asm__("nop");
    return;
}

void obfh_junk_func(void) {
    __obfh_asm__("nop");
    return;
}

#define __CRASH       \
    __obfh_asm__(     \
        ".byte 0xED\n\t" \
        "int3");   \
    exit(1)

#define TRUE ((((_9 + _7 + (RND(0, 1000) * _0))) / _8) - _1)
#define FALSE (((_3 + _6 + (RND(0, 1000) * _0)) - _9) * RND(0, 255))

#define NOP_FLOOD                                  \
    (RND(0, 1000)) + obfh_int_proxy(RND(0, 1000)); \
    if (obfh_junk_func_args) {                     \
        __obfh_asm__("nop");                      \
    }                                              \
    do {                                           \
        __obfh_asm__("nop\n\t" "nop");            \
    } while (RND(0, 200) * _0)

static float rndValueToProxy = RND(0, 10);

int obfh_int_proxy(int value) {
    volatile int temp = value;
    return temp;
}

double obfh_double_proxy(double value) {
    volatile double temp = value;
    return temp;
}

float obfh_condition_true(void) {
    BREAK_STACK_1;
    return _1 && TRUE;
}

int obfh_condition_proxy(float junk, float condition, ...) {
    volatile int temp = (int)condition;
    return temp;
}

char *obfh_process_hidden_string(char *string, ...) {
    BREAK_STACK_1;
    
    if (!obfh_condition_true() || _0) {
        BAD_JMP;
    }

    static char string_to_return[4096];
    strcpy(string_to_return, string + 1);
    return string_to_return;
}

long double __s_rdtsc(float junk, ...) {
    unsigned int lo, hi;
    __obfh_asm__(
        "rdtsc\n\t"
        : "=a"(lo), "=d"(hi)
    );
    
    unsigned long long time = ((unsigned long long)hi << 32) | lo;
    return (long double)time;
}

// =============================================================
// Control Flow Obfuscation
#if NO_CFLOW != 1

#define if(cond)                                                                         \
    if ((float)__s_rdtsc(RND(0, 255)) == (float)((RND(1, 255) * -1)) * (float)1.0) {     \
        BAD_CALL;                                                                        \
    } else if (&__s_rdtsc && (cond ? ((float)__s_rdtsc(RND(0, 255), RND(0, 255)) != 0.1) \
                                   : (float)__s_rdtsc(RND(0, 255)) == 0.1))

#define else      \
    else if (0) { \
        BAD_CALL; \
    }             \
    else

#if CFLOW_V2
#define OBFUS_CONDITION_BLOCK(...) \
    (obfh_condition_proxy(RND(0, 255), (__VA_ARGS__) ? !!obfh_int_proxy(!!obfh_condition_true()) : !!!obfh_condition_true(), RND(0, 255)) ? !!obfh_condition_true() : obfh_int_proxy(!obfh_condition_true()))
#else
#define OBFUS_CONDITION_BLOCK(...) \
    (obfh_condition_proxy(RND(0, 255), (__VA_ARGS__), RND(0, 255)) ? !!obfh_condition_true() : !obfh_condition_true())
#endif

#define break                                                  \
    {                                                          \
        if (OBFUS_CONDITION_BLOCK(RND(1, 255))) BREAK_STACK_1; \
        break;                                                 \
    }

#define switch(...)                         \
    if (OBFUS_CONDITION_BLOCK(RND(1, 255))) \
        switch (__VA_ARGS__)

#define while(...) \
    while ((float)__s_rdtsc(RND(0, 255)) != 0.1 && (&__s_rdtsc != (void*)!&__s_rdtsc) && (__VA_ARGS__))

#define for(...)                            \
    if (OBFUS_CONDITION_BLOCK(RND(1, 255))) \
        for (__VA_ARGS__)

#endif
// =============================================================

// =============================================================
// Virtual Machine
#if VIRT == 1

typedef enum {
    OP__ADD = RND(0, 900) * 5,
    OP__SUB = RND(1000, 1900) * 5,
    OP__MUL = RND(2000, 2900) * 5,
    OP__DIV = RND(3000, 3900) * 5,
    OP__MOD = RND(4000, 4900) * 5,
    OP__EQU = RND(5000, 5900) * 5,
    OP__NEQ = RND(6000, 6900) * 5,
    OP__GTR = RND(7000, 7900) * 5,
    OP__LSS = RND(8000, 8900) * 5,
    OP__LEQ = RND(9000, 9900) * 5,
    OP__GEQ = RND(10000, 10900) * 5,
    OP__NOP = RND(11000, 11900) * 5
} CMD;

typedef enum {
    SALT_CMD = RND(100, 900),
    SALT_NUM1 = RND(16, 48),
    SALT_NUM2 = RND(16, 48)
} VM_SALT;

static int _salt = SALT_CMD;

#define _VM_DEMUTATOR_KEY (1)
#define _VM_MUTATOR_KEY (1)

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

long double Obfh_VirtualMachine(long double uni_key, int command, long double num1, long double junk_2, long double num2, long double junk_3);

#define VM_ADD(num1, num2) ((long)Obfh_VirtualMachine(_VM_DEMUTATOR_KEY, _ENC_OP__ADD, (num1) * -1 + SALT_NUM1, RND(1, 500), (num2) * -1 + SALT_NUM2, RND(1, 500)))
#define VM_SUB(num1, num2) ((long)Obfh_VirtualMachine(_VM_DEMUTATOR_KEY, _ENC_OP__SUB, (num1) * -1 + SALT_NUM1, RND(1, 500), (num2) * -1 + SALT_NUM2, RND(1, 500)))
#define VM_MUL(num1, num2) ((long)Obfh_VirtualMachine(_VM_DEMUTATOR_KEY, _ENC_OP__MUL, (num1) * -1 + SALT_NUM1, RND(1, 500), (num2) * -1 + SALT_NUM2, RND(1, 500)))
#define VM_DIV(num1, num2) ((long)Obfh_VirtualMachine(_VM_DEMUTATOR_KEY, _ENC_OP__DIV, (num1) * -1 + SALT_NUM1, RND(1, 500), (num2) * -1 + SALT_NUM2, RND(1, 500)))
#define VM_MOD(num1, num2) ((long)Obfh_VirtualMachine(_VM_DEMUTATOR_KEY, _ENC_OP__MOD, (num1) * -1 + SALT_NUM1, RND(1, 500), (num2) * -1 + SALT_NUM2, RND(1, 500)))
#define VM_EQU(num1, num2) ((long)Obfh_VirtualMachine(_VM_DEMUTATOR_KEY, _ENC_OP__EQU, (num1) * -1 + SALT_NUM1, RND(1, 500), (num2) * -1 + SALT_NUM2, RND(1, 500)))
#define VM_NEQ(num1, num2) ((long)Obfh_VirtualMachine(_VM_DEMUTATOR_KEY, _ENC_OP__NEQ, (num1) * -1 + SALT_NUM1, RND(1, 500), (num2) * -1 + SALT_NUM2, RND(1, 500)))
#define VM_LSS(num1, num2) ((long)Obfh_VirtualMachine(_VM_DEMUTATOR_KEY, _ENC_OP__LSS, (num1) * -1 + SALT_NUM1, RND(1, 500), (num2) * -1 + SALT_NUM2, RND(1, 500)))
#define VM_GTR(num1, num2) ((long)Obfh_VirtualMachine(_VM_DEMUTATOR_KEY, _ENC_OP__GTR, (num1) * -1 + SALT_NUM1, RND(1, 500), (num2) * -1 + SALT_NUM2, RND(1, 500)))
#define VM_LEQ(num1, num2) ((long)Obfh_VirtualMachine(_VM_DEMUTATOR_KEY, _ENC_OP__LEQ, (num1) * -1 + SALT_NUM1, RND(1, 500), (num2) * -1 + SALT_NUM2, RND(1, 500)))
#define VM_GEQ(num1, num2) ((long)Obfh_VirtualMachine(_VM_DEMUTATOR_KEY, _ENC_OP__GEQ, (num1) * -1 + SALT_NUM1, RND(1, 500), (num2) * -1 + SALT_NUM2, RND(1, 500)))
#define VM_OBF_INT(num1) ((long)Obfh_VirtualMachine(_VM_DEMUTATOR_KEY, _ENC_OP__NOP, (num1) * -1 + SALT_NUM1, RND(1, 500), 0, RND(1, 500)))

#define VM_ADD_DBL(num1, num2) (Obfh_VirtualMachine(_VM_DEMUTATOR_KEY, _ENC_OP__ADD, (double)(num1) * -1 + SALT_NUM1, RND(1, 500), (double)(num2) * -1 + SALT_NUM2, RND(1, 500)))
#define VM_SUB_DBL(num1, num2) (Obfh_VirtualMachine(_VM_DEMUTATOR_KEY, _ENC_OP__SUB, (double)(num1) * -1 + SALT_NUM1, RND(1, 500), (double)(num2) * -1 + SALT_NUM2, RND(1, 500)))
#define VM_MUL_DBL(num1, num2) (Obfh_VirtualMachine(_VM_DEMUTATOR_KEY, _ENC_OP__MUL, (double)(num1) * -1 + SALT_NUM1, RND(1, 500), (double)(num2) * -1 + SALT_NUM2, RND(1, 500)))
#define VM_DIV_DBL(num1, num2) (Obfh_VirtualMachine(_VM_DEMUTATOR_KEY, _ENC_OP__DIV, (double)(num1) * -1 + SALT_NUM1, RND(1, 500), (double)(num2) * -1 + SALT_NUM2, RND(1, 500)))
#define VM_LSS_DBL(num1, num2) ((long)Obfh_VirtualMachine(_VM_DEMUTATOR_KEY, _ENC_OP__LSS, (double)(num1) * -1 + SALT_NUM1, RND(1, 500), (double)(num2) * -1 + SALT_NUM2, RND(1, 500)))
#define VM_GTR_DBL(num1, num2) ((long)Obfh_VirtualMachine(_VM_DEMUTATOR_KEY, _ENC_OP__GTR, (double)(num1) * -1 + SALT_NUM1, RND(1, 500), (double)(num2) * -1 + SALT_NUM2, RND(1, 500)))
#define VM_OBF_DBL(num1) (Obfh_VirtualMachine(_VM_DEMUTATOR_KEY, _ENC_OP__NOP, (num1) * -1 + SALT_NUM1, RND(1, 500), 0, RND(1, 500)))

#define VM_IF(condition) if (!VM_EQU((int)(condition), VM_MUL(RND(111111, 999999), 0)))
#define VM_ELSE_IF(condition) else if (!VM_EQU((int)(condition), VM_MUL(0, RND(111111, 999999))))
#define VM_ELSE else if (VM_EQU(1, _1))

long double obfhVmResult = 0;

long double Obfh_VirtualMachine(long double uni_key, int command, long double num1, long double junk_2, long double num2, long double junk_3) {
    BREAK_STACK_1;
    
    // Restore values
    command /= ~_salt;
    command += (int)uni_key;
    
    num2 -= SALT_NUM2;
    num2 *= (-1 * _1);
    
    num1 -= SALT_NUM1;
    num1 *= (-1 * _1);

    switch (command) {
        case OP__ADD:
            obfhVmResult = (num1 + num2);
            break;
        case OP__SUB:
            obfhVmResult = (num1 - num2);
            break;
        case OP__MUL:
            if (num1 == _0 || num2 == _0)
                obfhVmResult = _0;
            else
                obfhVmResult = num1 * num2;
            break;
        case OP__DIV:
            if (num2 != _0)
                obfhVmResult = num1 / num2;
            else
                obfhVmResult = _0;
            break;
        case OP__MOD:
            if (num2 != 0)
                obfhVmResult = (int)num1 % (int)num2;
            else
                obfhVmResult = _0;
            break;
        case OP__EQU:
            obfhVmResult = (num1 == num2) ? _1 : _0;
            break;
        case OP__NEQ:
            obfhVmResult = (num1 != num2) ? _1 : _0;
            break;
        case OP__LSS:
            obfhVmResult = (num1 < num2) ? _1 : _0;
            break;
        case OP__GTR:
            obfhVmResult = (num1 > num2) ? _1 : _0;
            break;
        case OP__LEQ:
            obfhVmResult = (num1 <= num2) ? _1 : _0;
            break;
        case OP__GEQ:
            obfhVmResult = (num1 >= num2) ? _1 : _0;
            break;
        case OP__NOP:
            obfhVmResult = num1;
            break;
        default:
            obfhVmResult = _0;
            BAD_JMP;
    }

    return obfhVmResult;
}
#endif
// =============================================================

// Custom standard library wrappers
int strcmp_custom(const char *str1, const char *str2) {
    BREAK_STACK_1;
    while (*str1 != '\0' || *str2 != '\0') {
        if (*str1 < *str2) {
            return -1;
        } else if (*str1 > *str2) {
            return 1;
        }
        str1++;
        str2++;
    }
    FAKE_CPUID;
    return 0;
}
#define strcmp(...) strcmp_custom(__VA_ARGS__)

size_t strlen_custom(const char *str) {
    BREAK_STACK_1;
    size_t length = 0;
    while (*str != '\0') {
        length++;
        str++;
    }
    FAKE_CPUID;
    return length;
}
#define strlen(...) strlen_custom(__VA_ARGS__)

// =============================================================
// Anti-Debug Protection
#if NO_ANTIDEBUG != 1

#if ANTIDEBUG_V2 == 1

void ad_ZeroDRs(PCONTEXT pCtx) {
    BREAK_STACK_1;
    pCtx->Dr0 = 0;
    pCtx->Dr1 = 0;
    pCtx->Dr2 = 0;
    pCtx->Dr3 = 0;
    pCtx->Dr6 = 0;
    pCtx->Dr7 = 0;
}

int ad_CompareDRs(PCONTEXT pCtx) {
    BREAK_STACK_1;
    if (pCtx->Dr7 != 0) {
        ad_ZeroDRs(pCtx);
        return 1;
    } else {
        if (0 == (pCtx->Dr0 | pCtx->Dr1 | pCtx->Dr2 | pCtx->Dr3)) {
            ad_ZeroDRs(pCtx);
        }
        ad_ZeroDRs(pCtx);
    }
    return 0;
}

DWORD WINAPI ThreadCompareDRs(void *p) {
    BREAK_STACK_1;
    DWORD dwRet = 0;
    HANDLE hMainThread = (HANDLE)p;
    if (-1 != SuspendThread(hMainThread)) {
        CONTEXT context;
        context.ContextFlags = CONTEXT_DEBUG_REGISTERS;
        if (GetThreadContext(hMainThread, &context)) {
            if (ad_CompareDRs(&context))
                dwRet = 1;
        }
        ResumeThread(hMainThread);
    }
    CloseHandle(hMainThread);
    return dwRet;
}

int IsDebuggerPresent_proxy(void) {
    BREAK_STACK_1;
    NOP_FLOOD;
    
    // Register validation
    HANDLE hMainThread;
    DWORD dwDummy, exitCode = 0;

    DuplicateHandle(GetCurrentProcess(), GetCurrentThread(), GetCurrentProcess(),
                    &hMainThread, 0, FALSE, DUPLICATE_SAME_ACCESS);

    HANDLE hThread = CreateThread(NULL, 0, ThreadCompareDRs, hMainThread, 0, &dwDummy);
    if (hThread) {
        WaitForSingleObject(hThread, INFINITE);
        GetExitCodeThread(hThread, &exitCode);
        CloseHandle(hThread);
    }

    if (exitCode) return exitCode;

    // Dynamic antidebugger
    return IsDebuggerPresent();
}

#else

int IsDebuggerPresent_proxy(void) {
    BREAK_STACK_1;
    NOP_FLOOD;
    return IsDebuggerPresent();
}

#endif

void crash(void) {
    BREAK_STACK_1;
    __obfh_asm__("int3\n\t" ".byte 0xED, 0x00");
}

void loop(void) {
    while (1) {
        Sleep(1000);
    }
}

#define ANTI_DEBUG                                                              \
    if (IsDebuggerPresent() || IsDebuggerPresent_proxy()) {                    \
        loop();                                                                 \
        while (1) {};                                                           \
        __obfh_asm__(".byte 0xED");                                            \
        BREAK_STACK_1;                                                          \
        crash();                                                                \
    } else {                                                                    \
        (void)0;                                                                \
    }

#else
#define ANTI_DEBUG ((void)0)
#endif
// =============================================================

// Custom printf implementation
void printf_custom(const char *format, ...) {
    BREAK_STACK_1;
    char buffer[4096];
    va_list args;
    
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD written;
    WriteConsoleA(hConsole, buffer, (DWORD)strlen(buffer), &written, NULL);
}

#define printf(...) do { \
    BREAK_STACK_1; \
    printf_custom(__VA_ARGS__); \
} while (0)

// Memory management wrappers
void *memset_proxy(void *ptr, int value, size_t num) {
    BREAK_STACK_1;
    return memset(ptr, value, num);
}
#define memset(...) memset_proxy(__VA_ARGS__)

void *calloc_proxy(size_t nmemb, size_t size) {
    BREAK_STACK_1;
    return calloc(nmemb, size);
}
#define calloc(nmemb, size) calloc_proxy(nmemb, size)

void *realloc_proxy(void *ptr, size_t size) {
    BREAK_STACK_1;
    return realloc(ptr, size);
}
#define realloc(ptr, size) realloc_proxy(ptr, size)

// String manipulation wrappers
char *strcpy_proxy(char *dest, const char *src) {
    BREAK_STACK_1;
    return strcpy(dest, src);
}
#define strcpy(dest, src) strcpy_proxy(dest, src)

char *strcat_proxy(char *dest, const char *src) {
    BREAK_STACK_1;
    return strcat(dest, src);
}
#define strcat(dest, src) strcat_proxy(dest, src)

char *strchr_proxy(const char *str, int c) {
    BREAK_STACK_1;
    return strchr(str, c);
}
#define strchr(str, c) strchr_proxy(str, c)

char *strrchr_proxy(const char *str, int c) {
    BREAK_STACK_1;
    return strrchr(str, c);
}
#define strrchr(str, c) strrchr_proxy(str, c)

char *strtok_proxy(char *str, const char *delim) {
    BREAK_STACK_1;
    return strtok(str, delim);
}
#define strtok(str, delim) strtok_proxy(str, delim)

// File operations wrappers
FILE *fopen_proxy(const char *filename, const char *mode) {
    BREAK_STACK_1;
    return fopen(filename, mode);
}
#define fopen(filename, mode) fopen_proxy(filename, mode)

int fclose_proxy(FILE *stream) {
    BREAK_STACK_1;
    return fclose(stream);
}
#define fclose(stream) fclose_proxy(stream)

size_t fread_proxy(void *ptr, size_t size, size_t count, FILE *stream) {
    BREAK_STACK_1;
    return fread(ptr, size, count, stream);
}
#define fread(ptr, size, count, stream) fread_proxy(ptr, size, count, stream)

size_t fwrite_proxy(const void *ptr, size_t size, size_t count, FILE *stream) {
    BREAK_STACK_1;
    return fwrite(ptr, size, count, stream);
}
#define fwrite(ptr, size, count, stream) fwrite_proxy(ptr, size, count, stream)

// System functions wrappers
int system_proxy(const char *command) {
    BREAK_STACK_1;
    return system(command);
}
#define system(command) system_proxy(command)

void exit_proxy(int status) {
    BREAK_STACK_1;
    exit(status);
}
#define exit(status) exit_proxy(status)

void abort_proxy(void) {
    BREAK_STACK_1;
    abort();
}
#define abort() abort_proxy()

int atexit_proxy(void (*func)(void)) {
    BREAK_STACK_1;
    return atexit(func);
}
#define atexit(func) atexit_proxy(func)

char *getenv_proxy(const char *name) {
    BREAK_STACK_1;
    return getenv(name);
}
#define getenv(name) getenv_proxy(name)

// Character classification wrappers
int tolower_proxy(int c) {
    BREAK_STACK_1;
    return tolower(c);
}
#define tolower(c) tolower_proxy(c)

int toupper_proxy(int c) {
    BREAK_STACK_1;
    return toupper(c);
}
#define toupper(c) toupper_proxy(c)

// Math function wrappers
#define _MUTATE_MATH(value) (FALSE + (value) * TRUE)

#define sqrt(x) sqrt(_MUTATE_MATH(x))
#define sin(x) sin(_MUTATE_MATH(x))
#define cos(x) cos(_MUTATE_MATH(x))
#define tan(x) tan(_MUTATE_MATH(x))
#define asin(x) asin(_MUTATE_MATH(x))
#define acos(x) acos(_MUTATE_MATH(x))
#define atan(x) atan(_MUTATE_MATH(x))
#define exp(x) exp(_MUTATE_MATH(x))
#define log(x) log(_MUTATE_MATH(x))
#define log10(x) log10(_MUTATE_MATH(x))
#define pow(x, y) pow(_MUTATE_MATH(x), _MUTATE_MATH(y))
#define ceil(x) ceil(_MUTATE_MATH(x))
#define floor(x) floor(_MUTATE_MATH(x))
#define fabs(x) fabs(_MUTATE_MATH(x))
#define abs(x) ((x) < FALSE ? -(x) : (x))

// Windows API wrappers
#define Sleep(x) Sleep(obfh_int_proxy(x * TRUE))

#define GetStdHandle(nStdHandle) GetStdHandle(obfh_int_proxy(nStdHandle))

#define WriteConsoleA(hConsoleOutput, lpBuffer, nNumberOfCharsToWrite, lpNumberOfCharsWritten, lpReserved) \
    WriteConsoleA(hConsoleOutput, lpBuffer, obfh_int_proxy(nNumberOfCharsToWrite), lpNumberOfCharsWritten, lpReserved)

#define GetModuleHandleA(lpModuleName) GetModuleHandleA(lpModuleName)

#define GetCurrentProcess() GetCurrentProcess()

#define VirtualAlloc(lpAddress, dwSize, flAllocationType, flProtect) \
    VirtualAlloc(lpAddress, obfh_int_proxy(dwSize), obfh_int_proxy(flAllocationType), obfh_int_proxy(flProtect))

#define VirtualFree(lpAddress, dwSize, dwFreeType) \
    VirtualFree(lpAddress, obfh_int_proxy(dwSize), obfh_int_proxy(dwFreeType))

#define CreateThread(lpThreadAttributes, dwStackSize, lpStartAddress, lpParameter, dwCreationFlags, lpThreadId) \
    CreateThread(lpThreadAttributes, obfh_int_proxy(dwStackSize), lpStartAddress, lpParameter, obfh_int_proxy(dwCreationFlags), lpThreadId)

#define WaitForSingleObject(hHandle, dwMilliseconds) \
    WaitForSingleObject(hHandle, obfh_int_proxy(dwMilliseconds))

#define CloseHandle(hObject) CloseHandle(hObject)

#define CreateFileA(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile) \
    CreateFileA(lpFileName, obfh_int_proxy(dwDesiredAccess), obfh_int_proxy(dwShareMode), lpSecurityAttributes, obfh_int_proxy(dwCreationDisposition), obfh_int_proxy(dwFlagsAndAttributes), hTemplateFile)

#define ReadFile(hFile, lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead, lpOverlapped) \
    ReadFile(hFile, lpBuffer, obfh_int_proxy(nNumberOfBytesToRead), lpNumberOfBytesRead, lpOverlapped)

#define WriteFile(hFile, lpBuffer, nNumberOfBytesToWrite, lpNumberOfBytesWritten, lpOverlapped) \
    WriteFile(hFile, lpBuffer, obfh_int_proxy(nNumberOfBytesToWrite), lpNumberOfBytesWritten, lpOverlapped)

#define GetModuleFileNameA(hModule, lpFilename, nSize) \
    GetModuleFileNameA(hModule, lpFilename, obfh_int_proxy(nSize))

#define ExitProcess(uExitCode) ExitProcess(obfh_int_proxy(uExitCode))

// Utility functions
#define memmove(dest, src, n) memmove(dest, src, obfh_int_proxy((n) * TRUE))

#define memcpy(dest, src, n) memcpy(dest, src, obfh_int_proxy((n) * TRUE))

// Export function example
__declspec(dllexport) char *WhatSoundDoesACowMake(void) {
    return HIDE_STRING("Moo");
}

#else
#warning Obfuscation disabled!
#endif

#endif // OBFH

// End of obfus.h
