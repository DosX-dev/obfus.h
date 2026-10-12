#include <stdio.h>

#include "../include/obfus.h"
#undef if
#undef for
#undef while
#undef printf
#undef puts
#undef fflush
#undef OBFH_CFLOW_INPUT
#define OBFH_CFLOW_INPUT(reg, index) "movl %[input], " reg ";"
static int pred_0(unsigned int input) {
    unsigned char valid;
    __obfh_asm__(OBFH_CFLOW_PRED_ADD_CARRY "sete %[valid];"
                 : [valid] "=m"(valid)
                 : OBFH_CFLOW_NAMED_INPUTS, [input] "m"(input)
                 : OBFH_CFLOW_CLOBBERS);
    return valid;
}
static int pred_1(unsigned int input) {
    unsigned char valid;
    __obfh_asm__(OBFH_CFLOW_PRED_OR_AND_SUM "sete %[valid];"
                 : [valid] "=m"(valid)
                 : OBFH_CFLOW_NAMED_INPUTS, [input] "m"(input)
                 : OBFH_CFLOW_CLOBBERS);
    return valid;
}
static int pred_2(unsigned int input) {
    unsigned char valid;
    __obfh_asm__(OBFH_CFLOW_PRED_SUB_BORROW "sete %[valid];"
                 : [valid] "=m"(valid)
                 : OBFH_CFLOW_NAMED_INPUTS, [input] "m"(input)
                 : OBFH_CFLOW_CLOBBERS);
    return valid;
}
static int pred_3(unsigned int input) {
    unsigned char valid;
    __obfh_asm__(OBFH_CFLOW_PRED_DEMORGAN "sete %[valid];"
                 : [valid] "=m"(valid)
                 : OBFH_CFLOW_NAMED_INPUTS, [input] "m"(input)
                 : OBFH_CFLOW_CLOBBERS);
    return valid;
}
static int pred_4(unsigned int input) {
    unsigned char valid;
    __obfh_asm__(OBFH_CFLOW_PRED_OR_DISTRIBUTE "sete %[valid];"
                 : [valid] "=m"(valid)
                 : OBFH_CFLOW_NAMED_INPUTS, [input] "m"(input)
                 : OBFH_CFLOW_CLOBBERS);
    return valid;
}
static int pred_5(unsigned int input) {
    unsigned char valid;
    __obfh_asm__(OBFH_CFLOW_PRED_AND_PARTITION "sete %[valid];"
                 : [valid] "=m"(valid)
                 : OBFH_CFLOW_NAMED_INPUTS, [input] "m"(input)
                 : OBFH_CFLOW_CLOBBERS);
    return valid;
}
static int pred_6(unsigned int input) {
    unsigned char valid;
    __obfh_asm__(OBFH_CFLOW_PRED_XOR_CANCEL "sete %[valid];"
                 : [valid] "=m"(valid)
                 : OBFH_CFLOW_NAMED_INPUTS, [input] "m"(input)
                 : OBFH_CFLOW_CLOBBERS);
    return valid;
}
static int pred_7(unsigned int input) {
    unsigned char valid;
    __obfh_asm__(OBFH_CFLOW_PRED_COMPLEMENT_CARRY "setnc %[valid];"
                 : [valid] "=m"(valid)
                 : OBFH_CFLOW_NAMED_INPUTS, [input] "m"(input)
                 : OBFH_CFLOW_CLOBBERS);
    return valid;
}
static int pred_8(unsigned int input) {
    unsigned char valid;
    __obfh_asm__(OBFH_CFLOW_PRED_COMPLEMENT_WRAP "setc %[valid];"
                 : [valid] "=m"(valid)
                 : OBFH_CFLOW_NAMED_INPUTS, [input] "m"(input)
                 : OBFH_CFLOW_CLOBBERS);
    return valid;
}
static int pred_9(unsigned int input) {
    unsigned char valid;
    __obfh_asm__(OBFH_CFLOW_PRED_MASK_SUBTRACT "setnc %[valid];"
                 : [valid] "=m"(valid)
                 : OBFH_CFLOW_NAMED_INPUTS, [input] "m"(input)
                 : OBFH_CFLOW_CLOBBERS);
    return valid;
}
static int pred_10(unsigned int input) {
    unsigned char valid;
    __obfh_asm__(OBFH_CFLOW_PRED_MASK_OR_ORDER "setae %[valid];"
                 : [valid] "=m"(valid)
                 : OBFH_CFLOW_NAMED_INPUTS, [input] "m"(input)
                 : OBFH_CFLOW_CLOBBERS);
    return valid;
}
static int pred_11(unsigned int input) {
    unsigned char valid;
    __obfh_asm__(OBFH_CFLOW_PRED_ROTATE_XOR "sete %[valid];"
                 : [valid] "=m"(valid)
                 : OBFH_CFLOW_NAMED_INPUTS, [input] "m"(input)
                 : OBFH_CFLOW_CLOBBERS);
    return valid;
}
static int pred_12(unsigned int input) {
    unsigned char valid;
    __obfh_asm__(OBFH_CFLOW_PRED_ROTATE_RESTORE "sete %[valid];"
                 : [valid] "=m"(valid)
                 : OBFH_CFLOW_NAMED_INPUTS, [input] "m"(input)
                 : OBFH_CFLOW_CLOBBERS);
    return valid;
}
static int pred_13(unsigned int input) {
    unsigned char valid;
    __obfh_asm__(OBFH_CFLOW_PRED_BSWAP_XOR "sete %[valid];"
                 : [valid] "=m"(valid)
                 : OBFH_CFLOW_NAMED_INPUTS, [input] "m"(input)
                 : OBFH_CFLOW_CLOBBERS);
    return valid;
}
static int pred_14(unsigned int input) {
    unsigned char valid;
    __obfh_asm__(OBFH_CFLOW_PRED_WORD_PARTITION "sete %[valid];"
                 : [valid] "=m"(valid)
                 : OBFH_CFLOW_NAMED_INPUTS, [input] "m"(input)
                 : OBFH_CFLOW_CLOBBERS);
    return valid;
}
static int pred_15(unsigned int input) {
    unsigned char valid;
    __obfh_asm__(OBFH_CFLOW_PRED_BYTE_PARITY "sete %[valid];"
                 : [valid] "=m"(valid)
                 : OBFH_CFLOW_NAMED_INPUTS, [input] "m"(input)
                 : OBFH_CFLOW_CLOBBERS);
    return valid;
}
static int pred_16(unsigned int input) {
    unsigned char valid;
    __obfh_asm__(OBFH_CFLOW_PRED_BYTE_ROTATE "sete %[valid];"
                 : [valid] "=m"(valid)
                 : OBFH_CFLOW_NAMED_INPUTS, [input] "m"(input)
                 : OBFH_CFLOW_CLOBBERS);
    return valid;
}
static int pred_17(unsigned int input) {
    unsigned char valid;
    __obfh_asm__(OBFH_CFLOW_PRED_WORD_ROTATE "sete %[valid];"
                 : [valid] "=m"(valid)
                 : OBFH_CFLOW_NAMED_INPUTS, [input] "m"(input)
                 : OBFH_CFLOW_CLOBBERS);
    return valid;
}
static int pred_18(unsigned int input) {
    unsigned char valid;
    __obfh_asm__(OBFH_CFLOW_PRED_NEG_COMPLEMENT "sete %[valid];"
                 : [valid] "=m"(valid)
                 : OBFH_CFLOW_NAMED_INPUTS, [input] "m"(input)
                 : OBFH_CFLOW_CLOBBERS);
    return valid;
}
static int pred_19(unsigned int input) {
    unsigned char valid;
    __obfh_asm__(OBFH_CFLOW_PRED_MUL_DISTRIBUTE "sete %[valid];"
                 : [valid] "=m"(valid)
                 : OBFH_CFLOW_NAMED_INPUTS, [input] "m"(input)
                 : OBFH_CFLOW_CLOBBERS);
    return valid;
}
static int pred_20(unsigned int input) {
    unsigned char valid;
    __obfh_asm__(OBFH_CFLOW_PRED_SQUARE_EXPAND "sete %[valid];"
                 : [valid] "=m"(valid)
                 : OBFH_CFLOW_NAMED_INPUTS, [input] "m"(input)
                 : OBFH_CFLOW_CLOBBERS);
    return valid;
}
static int pred_21(unsigned int input) {
    unsigned char valid;
    __obfh_asm__(OBFH_CFLOW_PRED_NEG_SQUARE "sete %[valid];"
                 : [valid] "=m"(valid)
                 : OBFH_CFLOW_NAMED_INPUTS, [input] "m"(input)
                 : OBFH_CFLOW_CLOBBERS);
    return valid;
}
static int pred_22(unsigned int input) {
    unsigned char valid;
    __obfh_asm__(OBFH_CFLOW_PRED_BSWAP_NOT "sete %[valid];"
                 : [valid] "=m"(valid)
                 : OBFH_CFLOW_NAMED_INPUTS, [input] "m"(input)
                 : OBFH_CFLOW_CLOBBERS);
    return valid;
}
static int pred_23(unsigned int input) {
    unsigned char valid;
    __obfh_asm__(OBFH_CFLOW_PRED_SUB_MASK "sete %[valid];"
                 : [valid] "=m"(valid)
                 : OBFH_CFLOW_NAMED_INPUTS, [input] "m"(input)
                 : OBFH_CFLOW_CLOBBERS);
    return valid;
}
static int pred_24(unsigned int input) {
    unsigned char valid;
    __obfh_asm__(OBFH_CFLOW_PRED_MASK_ORDER "setbe %[valid];"
                 : [valid] "=m"(valid)
                 : OBFH_CFLOW_NAMED_INPUTS, [input] "m"(input)
                 : OBFH_CFLOW_CLOBBERS);
    return valid;
}
static int pred_25(unsigned int input) {
    unsigned char valid;
    __obfh_asm__(OBFH_CFLOW_PRED_COMPLEMENT_TRANSLATE "sete %[valid];"
                 : [valid] "=m"(valid)
                 : OBFH_CFLOW_NAMED_INPUTS, [input] "m"(input)
                 : OBFH_CFLOW_CLOBBERS);
    return valid;
}
static int pred_26(unsigned int input) {
    unsigned char valid;
    __obfh_asm__(OBFH_CFLOW_PRED_SQUARE_RESIDUE "sete %[valid];"
                 : [valid] "=m"(valid)
                 : OBFH_CFLOW_NAMED_INPUTS, [input] "m"(input)
                 : OBFH_CFLOW_CLOBBERS);
    return valid;
}
static int pred_27(unsigned int input) {
    unsigned char valid;
    __obfh_asm__(OBFH_CFLOW_PRED_FOURTH_RESIDUE "sete %[valid];"
                 : [valid] "=m"(valid)
                 : OBFH_CFLOW_NAMED_INPUTS, [input] "m"(input)
                 : OBFH_CFLOW_CLOBBERS);
    return valid;
}
static int pred_28(unsigned int input) {
    unsigned char valid;
    __obfh_asm__(OBFH_CFLOW_PRED_ISOLATE_BIT "sete %[valid];"
                 : [valid] "=m"(valid)
                 : OBFH_CFLOW_NAMED_INPUTS, [input] "m"(input)
                 : OBFH_CFLOW_CLOBBERS);
    return valid;
}
static int (*preds[])(unsigned int) = {pred_0, pred_1, pred_2, pred_3, pred_4, pred_5, pred_6, pred_7, pred_8, pred_9, pred_10, pred_11, pred_12, pred_13, pred_14, pred_15, pred_16, pred_17, pred_18, pred_19, pred_20, pred_21, pred_22, pred_23, pred_24, pred_25, pred_26, pred_27, pred_28};
int main(void) {
    unsigned int value = 0xA5A5A5A5u;
    for (int p = 0; p < 29; ++p) {
        for (int n = 0; n < 10000; ++n) {
            value ^= value << 13;
            value ^= value >> 17;
            value ^= value << 5;
            if (!preds[p](value)) return p + 1;
        }
        for (unsigned int n = 0; n < 256; ++n)
            if (!preds[p](n) || !preds[p](~n) || !preds[p](0x80000000u + n)) return p + 1;
        for (int bit = 0; bit < 32; ++bit) {
            unsigned int n = 1u << bit;
            if (!preds[p](n) || !preds[p](n - 1u) || !preds[p](n + 1u)) return p + 1;
        }
    }
    if (puts("BREAK_PREDICATES_PASS") < 0) return 40;
    return fflush(stdout) == 0 ? 0 : 41;
}
