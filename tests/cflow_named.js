'use strict';

// Original contiguous predicates and layouts are kept independent of the
// production family mapping, flag metadata and fragment selectors.
const reference = String.raw`#define NAMED_REFERENCE_PRED_ADD_CARRY OBFH_CFLOW_INPUT("%%eax", "0") "movl %%eax, %%edx; xorl $.Lcfk, %%eax; andl $.Lcfk, %%edx; " OBFH_ASM_BINARY32("0x01", "2", "2", ".Lcfk", "19") OBFH_ASM_BINARY32("0x01", "0", "2", ".Lcfk", "20") OBFH_CFLOW_INPUT("%%edx", "2") "addl $.Lcfk, %%edx; " OBFH_ASM_BINARY32("0x39", "0", "2", ".Lcfk", "21") ""
#define NAMED_REFERENCE_PRED_OR_AND_SUM OBFH_CFLOW_INPUT("%%eax", "0") "movl %%eax, %%edx; orl $.Lcfk, %%eax; andl $.Lcfk, %%edx; " OBFH_ASM_BINARY32("0x01", "0", "2", ".Lcfk", "22") OBFH_CFLOW_INPUT("%%edx", "2") "addl $.Lcfk, %%edx; " OBFH_ASM_BINARY32("0x39", "0", "2", ".Lcfk", "23") ""
#define NAMED_REFERENCE_PRED_SUB_BORROW OBFH_CFLOW_INPUT("%%eax", "0") "movl %%eax, %%edx; xorl $.Lcfk, %%eax; notl %%edx; andl $.Lcfk, %%edx; " OBFH_ASM_BINARY32("0x01", "2", "2", ".Lcfk", "24") OBFH_ASM_BINARY32("0x29", "0", "2", ".Lcfk", "25") OBFH_CFLOW_INPUT("%%edx", "2") "subl $.Lcfk, %%edx; " OBFH_ASM_BINARY32("0x39", "0", "2", ".Lcfk", "26") ""
#define NAMED_REFERENCE_PRED_DEMORGAN OBFH_CFLOW_INPUT("%%eax", "0") "orl $.Lcfk, %%eax; notl %%eax; " OBFH_CFLOW_INPUT("%%edx", "2") "notl %%edx; movl $.Lcfk, %%ecx; notl %%ecx; " OBFH_ASM_BINARY32("0x21", "2", "1", ".Lcfk", "27") OBFH_ASM_BINARY32("0x39", "0", "2", ".Lcfk", "28") ""
#define NAMED_REFERENCE_PRED_OR_DISTRIBUTE OBFH_CFLOW_INPUT("%%eax", "0") "movl %%eax, %%edx; orl $.Lcfk, %%eax; movl $.Lcfk, %%ecx; notl %%ecx; " OBFH_ASM_BINARY32("0x09", "2", "1", ".Lcfk", "29") OBFH_ASM_BINARY32("0x21", "0", "2", ".Lcfk", "18") OBFH_CFLOW_INPUT("%%edx", "2") OBFH_ASM_BINARY32("0x39", "0", "2", ".Lcfk", "19") ""
#define NAMED_REFERENCE_PRED_AND_PARTITION OBFH_CFLOW_INPUT("%%eax", "0") "movl %%eax, %%edx; andl $.Lcfk, %%eax; movl $.Lcfk, %%ecx; notl %%ecx; " OBFH_ASM_BINARY32("0x21", "2", "1", ".Lcfk", "20") OBFH_ASM_BINARY32("0x09", "0", "2", ".Lcfk", "21") OBFH_CFLOW_INPUT("%%edx", "2") OBFH_ASM_BINARY32("0x39", "0", "2", ".Lcfk", "22") ""
#define NAMED_REFERENCE_PRED_XOR_CANCEL OBFH_CFLOW_INPUT("%%eax", "0") "movl %%eax, %%edx; xorl $.Lcfk, %%eax; " OBFH_ASM_BINARY32("0x31", "0", "2", ".Lcfk", "23") " cmpl $.Lcfk, %%eax;"
#define NAMED_REFERENCE_PRED_COMPLEMENT_CARRY OBFH_CFLOW_INPUT("%%eax", "0") "movl %%eax, %%edx; notl %%edx; " OBFH_ASM_BINARY32("0x01", "0", "2", ".Lcfk", "24") ""
#define NAMED_REFERENCE_PRED_COMPLEMENT_WRAP OBFH_CFLOW_INPUT("%%eax", "0") "movl %%eax, %%edx; notl %%edx; " OBFH_ASM_BINARY32("0x01", "0", "2", ".Lcfk", "25") " addl $1, %%eax;"
#define NAMED_REFERENCE_PRED_MASK_SUBTRACT OBFH_CFLOW_INPUT("%%eax", "0") "movl %%eax, %%edx; andl $.Lcfk, %%edx; " OBFH_ASM_BINARY32("0x29", "0", "2", ".Lcfk", "26") ""
#define NAMED_REFERENCE_PRED_MASK_OR_ORDER OBFH_CFLOW_INPUT("%%eax", "0") "movl %%eax, %%edx; orl $.Lcfk, %%eax; " OBFH_ASM_BINARY32("0x39", "0", "2", ".Lcfk", "27") ""
#define NAMED_REFERENCE_PRED_ROTATE_XOR OBFH_CFLOW_INPUT("%%eax", "0") "xorl $.Lcfk, %%eax; roll $.Lcfr, %%eax; " OBFH_CFLOW_INPUT("%%edx", "2") "roll $.Lcfr, %%edx; movl $.Lcfk, %%ecx; roll $.Lcfr, %%ecx; " OBFH_ASM_BINARY32("0x31", "2", "1", ".Lcfk", "28") OBFH_ASM_BINARY32("0x39", "0", "2", ".Lcfk", "29") ""
#define NAMED_REFERENCE_PRED_ROTATE_RESTORE OBFH_CFLOW_INPUT("%%eax", "0") OBFH_ASM_ROTATE_PAIR("0", ".Lcfr", ".Lcfk", "0") OBFH_ASM_ROTATE_PAIR("0", ".Lcfr", ".Lcfk", "1") OBFH_CFLOW_INPUT("%%edx", "2") OBFH_ASM_EQ32("0", "2", ".Lcfk", "16") ""
#define NAMED_REFERENCE_PRED_BSWAP_XOR OBFH_CFLOW_INPUT("%%eax", "0") "xorl $.Lcfk, %%eax; bswap %%eax; " OBFH_CFLOW_INPUT("%%edx", "2") "bswap %%edx; movl $.Lcfk, %%ecx; bswap %%ecx; " OBFH_ASM_BINARY32("0x31", "2", "1", ".Lcfk", "18") OBFH_ASM_EQ32("0", "2", ".Lcfk", "16") ""
#define NAMED_REFERENCE_PRED_WORD_PARTITION OBFH_CFLOW_INPUT("%%eax", "0") "movl %%eax, %%edx; andl $65535, %%eax; andl $-65536, %%edx; " OBFH_ASM_BINARY32("0x09", "0", "2", ".Lcfk", "19") OBFH_CFLOW_INPUT("%%edx", "2") OBFH_ASM_BINARY32("0x39", "0", "2", ".Lcfk", "20") ""
#define NAMED_REFERENCE_PRED_BYTE_PARITY OBFH_CFLOW_INPUT("%%eax", "0") "movzbl %%al, %%edx; imull %%eax, %%eax; " OBFH_ASM_BINARY32("0x01", "0", "2", ".Lcfk", "21") " testb $1, %%al;"
#define NAMED_REFERENCE_PRED_BYTE_ROTATE OBFH_CFLOW_INPUT("%%eax", "0") "movl %%eax, %%edx; rolb $.Lcfr, %%al; rorb $.Lcfr, %%al; cmpb %%dl, %%al;"
#define NAMED_REFERENCE_PRED_WORD_ROTATE OBFH_CFLOW_INPUT("%%eax", "0") "movl %%eax, %%edx; rolw $.Lcfr, %%ax; rorw $.Lcfr, %%ax; cmpw %%dx, %%ax;"
#define NAMED_REFERENCE_PRED_NEG_COMPLEMENT OBFH_CFLOW_INPUT("%%eax", "0") "notl %%eax; addl $1, %%eax; " OBFH_CFLOW_INPUT("%%edx", "2") "negl %%edx; " OBFH_ASM_EQ32("0", "2", ".Lcfk", "16") ""
#define NAMED_REFERENCE_PRED_MUL_DISTRIBUTE OBFH_CFLOW_INPUT("%%eax", "0") "addl $.Lcfk, %%eax; imull $.Lcfs, %%eax; " OBFH_CFLOW_INPUT("%%edx", "2") "imull $.Lcfs, %%edx; movl $.Lcfk, %%ecx; imull $.Lcfs, %%ecx; " OBFH_ASM_BINARY32("0x01", "2", "1", ".Lcfk", "22") OBFH_ASM_EQ32("0", "2", ".Lcfk", "16") ""
#define NAMED_REFERENCE_PRED_SQUARE_EXPAND OBFH_CFLOW_INPUT("%%eax", "0") "movl %%eax, %%edx; imull %%edx, %%edx; movl %%eax, %%ecx; imull $.Lcfk, %%ecx; " OBFH_ASM_BINARY32("0x01", "1", "1", ".Lcfk", "23") " addl $.Lcfk, %%eax; imull %%eax, %%eax; " OBFH_ASM_BINARY32("0x29", "0", "2", ".Lcfk", "24") OBFH_ASM_BINARY32("0x29", "0", "1", ".Lcfk", "25") " movl $.Lcfk, %%edx; imull %%edx, %%edx; " OBFH_ASM_BINARY32("0x39", "0", "2", ".Lcfk", "26") ""
#define NAMED_REFERENCE_PRED_NEG_SQUARE OBFH_CFLOW_INPUT("%%eax", "0") "negl %%eax; imull %%eax, %%eax; " OBFH_CFLOW_INPUT("%%edx", "2") "imull %%edx, %%edx; " OBFH_ASM_BINARY32("0x39", "0", "2", ".Lcfk", "27") ""
#define NAMED_REFERENCE_PRED_BSWAP_NOT OBFH_CFLOW_INPUT("%%eax", "0") "notl %%eax; bswap %%eax; " OBFH_CFLOW_INPUT("%%edx", "2") "bswap %%edx; notl %%edx; " OBFH_ASM_EQ32("0", "2", ".Lcfk", "16") ""
#define NAMED_REFERENCE_PRED_SUB_MASK OBFH_CFLOW_INPUT("%%eax", "0") "movl %%eax, %%edx; andl $.Lcfk, %%edx; " OBFH_ASM_BINARY32("0x29", "0", "2", ".Lcfk", "28") OBFH_CFLOW_INPUT("%%edx", "2") "movl $.Lcfk, %%ecx; notl %%ecx; " OBFH_ASM_BINARY32("0x21", "2", "1", ".Lcfk", "29") OBFH_ASM_BINARY32("0x39", "0", "2", ".Lcfk", "18") ""
#define NAMED_REFERENCE_PRED_MASK_ORDER OBFH_CFLOW_INPUT("%%eax", "0") "movl %%eax, %%edx; andl $.Lcfk, %%eax; orl $.Lcfk, %%edx; " OBFH_ASM_BINARY32("0x39", "0", "2", ".Lcfk", "19") ""
#define NAMED_REFERENCE_PRED_COMPLEMENT_TRANSLATE OBFH_CFLOW_INPUT("%%eax", "0") "notl %%eax; addl $.Lcfk, %%eax; " OBFH_CFLOW_INPUT("%%edx", "2") "negl %%edx; addl $.Lcfk, %%edx; subl $1, %%edx; " OBFH_ASM_BINARY32("0x39", "0", "2", ".Lcfk", "20") ""
#define NAMED_REFERENCE_PRED_SQUARE_RESIDUE OBFH_CFLOW_INPUT("%%eax", "0") "imull %%eax, %%eax; andl $7, %%eax; leal -1(%%eax), %%edx; leal -4(%%eax), %%ecx; imull %%edx, %%eax; imull %%ecx, %%eax; testl %%eax, %%eax;"
#define NAMED_REFERENCE_PRED_FOURTH_RESIDUE OBFH_CFLOW_INPUT("%%eax", "0") "imull %%eax, %%eax; imull %%eax, %%eax; andl $15, %%eax; leal -1(%%eax), %%edx; imull %%edx, %%eax; testl %%eax, %%eax;"
#define NAMED_REFERENCE_PRED_ISOLATE_BIT OBFH_CFLOW_INPUT("%%eax", "0") "movl %%eax, %%edx; negl %%edx; " OBFH_ASM_BINARY32("0x21", "0", "2", ".Lcfk", "21") " leal -1(%%eax), %%edx; " OBFH_ASM_BINARY32("0x21", "0", "2", ".Lcfk", "22") " testl %%eax, %%eax;"
#define NAMED_REFERENCE_TEMPLATE_99 \
    OBFH_CFLOW_LOCAL_NAMED_ASM(NAMED_REFERENCE_PRED_ADD_CARRY "je 1f;" OBFH_CFLOW_DATA_CALL "1:" NAMED_REFERENCE_PRED_ROTATE_XOR "je 2f;" OBFH_CFLOW_DATA_INDIRECT "2:")

#define NAMED_REFERENCE_TEMPLATE_100 \
    OBFH_CFLOW_LOCAL_NAMED_ASM(NAMED_REFERENCE_PRED_OR_AND_SUM "jne 1f; jmp 2f; 1:" OBFH_CFLOW_DATA_STACK "2:" NAMED_REFERENCE_PRED_ROTATE_RESTORE "sete %%cl; movzbl %%cl, %%ecx; testl %%ecx, %%ecx; jnz 3f;" OBFH_CFLOW_DATA_TRAP "3:")

#define NAMED_REFERENCE_TEMPLATE_101 \
    OBFH_CFLOW_LOCAL_NAMED_ASM(NAMED_REFERENCE_PRED_SUB_BORROW "sete %%cl; movzbl %%cl, %%ecx; negl %%ecx; testl %%ecx, %%ecx; js 1f;" OBFH_CFLOW_DATA_FRAME "1:" NAMED_REFERENCE_PRED_BSWAP_XOR "jne 2f; jmp 3f; 2:" OBFH_CFLOW_DATA_RETURN "3:")

#define NAMED_REFERENCE_TEMPLATE_102 \
    OBFH_CFLOW_LOCAL_NAMED_ASM(NAMED_REFERENCE_PRED_DEMORGAN "je 2f; 1:" OBFH_CFLOW_DATA_INDIRECT "2:" NAMED_REFERENCE_PRED_WORD_PARTITION "jne 1b; jmp 3f;" OBFH_CFLOW_DATA_CALL "3:")

#define NAMED_REFERENCE_TEMPLATE_103 \
    OBFH_CFLOW_LOCAL_NAMED_ASM(NAMED_REFERENCE_PRED_OR_DISTRIBUTE "je 1f;" OBFH_CFLOW_DATA_TRAP "1:" NAMED_REFERENCE_PRED_BYTE_PARITY "je 2f;" OBFH_CFLOW_DATA_STACK "2:")

#define NAMED_REFERENCE_TEMPLATE_104 \
    OBFH_CFLOW_LOCAL_NAMED_ASM(NAMED_REFERENCE_PRED_AND_PARTITION "jne 1f; jmp 2f; 1:" OBFH_CFLOW_DATA_RETURN "2:" NAMED_REFERENCE_PRED_BYTE_ROTATE "sete %%cl; movzbl %%cl, %%ecx; testl %%ecx, %%ecx; jnz 3f;" OBFH_CFLOW_DATA_FRAME "3:")

#define NAMED_REFERENCE_TEMPLATE_105 \
    OBFH_CFLOW_LOCAL_NAMED_ASM(NAMED_REFERENCE_PRED_XOR_CANCEL "sete %%cl; movzbl %%cl, %%ecx; negl %%ecx; testl %%ecx, %%ecx; js 1f;" OBFH_CFLOW_DATA_CALL "1:" NAMED_REFERENCE_PRED_WORD_ROTATE "jne 2f; jmp 3f; 2:" OBFH_CFLOW_DATA_INDIRECT "3:")

#define NAMED_REFERENCE_TEMPLATE_106 \
    OBFH_CFLOW_LOCAL_NAMED_ASM(NAMED_REFERENCE_PRED_COMPLEMENT_CARRY "jnc 2f; 1:" OBFH_CFLOW_DATA_STACK "2:" NAMED_REFERENCE_PRED_NEG_COMPLEMENT "jne 1b; jmp 3f;" OBFH_CFLOW_DATA_TRAP "3:")

#define NAMED_REFERENCE_TEMPLATE_107 \
    OBFH_CFLOW_LOCAL_NAMED_ASM(NAMED_REFERENCE_PRED_COMPLEMENT_WRAP "jc 1f;" OBFH_CFLOW_DATA_FRAME "1:" NAMED_REFERENCE_PRED_MUL_DISTRIBUTE "je 2f;" OBFH_CFLOW_DATA_RETURN "2:")

#define NAMED_REFERENCE_TEMPLATE_108 \
    OBFH_CFLOW_LOCAL_NAMED_ASM(NAMED_REFERENCE_PRED_MASK_SUBTRACT "jc 1f; jmp 2f; 1:" OBFH_CFLOW_DATA_INDIRECT "2:" NAMED_REFERENCE_PRED_SQUARE_EXPAND "sete %%cl; movzbl %%cl, %%ecx; testl %%ecx, %%ecx; jnz 3f;" OBFH_CFLOW_DATA_CALL "3:")

#define NAMED_REFERENCE_TEMPLATE_109 \
    OBFH_CFLOW_LOCAL_NAMED_ASM(NAMED_REFERENCE_PRED_MASK_OR_ORDER "setae %%cl; movzbl %%cl, %%ecx; negl %%ecx; testl %%ecx, %%ecx; js 1f;" OBFH_CFLOW_DATA_TRAP "1:" NAMED_REFERENCE_PRED_NEG_SQUARE "jne 2f; jmp 3f; 2:" OBFH_CFLOW_DATA_STACK "3:")

#define NAMED_REFERENCE_TEMPLATE_110 \
    OBFH_CFLOW_LOCAL_NAMED_ASM(NAMED_REFERENCE_PRED_ROTATE_XOR "je 2f; 1:" OBFH_CFLOW_DATA_RETURN "2:" NAMED_REFERENCE_PRED_BSWAP_NOT "jne 1b; jmp 3f;" OBFH_CFLOW_DATA_FRAME "3:")

#define NAMED_REFERENCE_TEMPLATE_111 \
    OBFH_CFLOW_LOCAL_NAMED_ASM(NAMED_REFERENCE_PRED_ROTATE_RESTORE "je 1f;" OBFH_CFLOW_DATA_CALL "1:" NAMED_REFERENCE_PRED_SUB_MASK "je 2f;" OBFH_CFLOW_DATA_INDIRECT "2:")

#define NAMED_REFERENCE_TEMPLATE_112 \
    OBFH_CFLOW_LOCAL_NAMED_ASM(NAMED_REFERENCE_PRED_BSWAP_XOR "jne 1f; jmp 2f; 1:" OBFH_CFLOW_DATA_STACK "2:" NAMED_REFERENCE_PRED_MASK_ORDER "setbe %%cl; movzbl %%cl, %%ecx; testl %%ecx, %%ecx; jnz 3f;" OBFH_CFLOW_DATA_TRAP "3:")

#define NAMED_REFERENCE_TEMPLATE_113 \
    OBFH_CFLOW_LOCAL_NAMED_ASM(NAMED_REFERENCE_PRED_WORD_PARTITION "sete %%cl; movzbl %%cl, %%ecx; negl %%ecx; testl %%ecx, %%ecx; js 1f;" OBFH_CFLOW_DATA_FRAME "1:" NAMED_REFERENCE_PRED_COMPLEMENT_TRANSLATE "jne 2f; jmp 3f; 2:" OBFH_CFLOW_DATA_RETURN "3:")

#define NAMED_REFERENCE_TEMPLATE_114 \
    OBFH_CFLOW_LOCAL_NAMED_ASM(NAMED_REFERENCE_PRED_BYTE_PARITY "je 2f; 1:" OBFH_CFLOW_DATA_INDIRECT "2:" NAMED_REFERENCE_PRED_SQUARE_RESIDUE "jne 1b; jmp 3f;" OBFH_CFLOW_DATA_CALL "3:")

#define NAMED_REFERENCE_TEMPLATE_115 \
    OBFH_CFLOW_LOCAL_NAMED_ASM(NAMED_REFERENCE_PRED_BYTE_ROTATE "je 1f;" OBFH_CFLOW_DATA_TRAP "1:" NAMED_REFERENCE_PRED_FOURTH_RESIDUE "je 2f;" OBFH_CFLOW_DATA_STACK "2:")

#define NAMED_REFERENCE_TEMPLATE_116 \
    OBFH_CFLOW_LOCAL_NAMED_ASM(NAMED_REFERENCE_PRED_WORD_ROTATE "jne 1f; jmp 2f; 1:" OBFH_CFLOW_DATA_RETURN "2:" NAMED_REFERENCE_PRED_ISOLATE_BIT "sete %%cl; movzbl %%cl, %%ecx; testl %%ecx, %%ecx; jnz 3f;" OBFH_CFLOW_DATA_FRAME "3:")

#define NAMED_REFERENCE_TEMPLATE_117 \
    OBFH_CFLOW_LOCAL_NAMED_ASM(NAMED_REFERENCE_PRED_NEG_COMPLEMENT "sete %%cl; movzbl %%cl, %%ecx; negl %%ecx; testl %%ecx, %%ecx; js 1f;" OBFH_CFLOW_DATA_CALL "1:" NAMED_REFERENCE_PRED_ADD_CARRY "jne 2f; jmp 3f; 2:" OBFH_CFLOW_DATA_INDIRECT "3:")

#define NAMED_REFERENCE_TEMPLATE_118 \
    OBFH_CFLOW_LOCAL_NAMED_ASM(NAMED_REFERENCE_PRED_MUL_DISTRIBUTE "je 2f; 1:" OBFH_CFLOW_DATA_STACK "2:" NAMED_REFERENCE_PRED_OR_AND_SUM "jne 1b; jmp 3f;" OBFH_CFLOW_DATA_TRAP "3:")

#define NAMED_REFERENCE_TEMPLATE_119 \
    OBFH_CFLOW_LOCAL_NAMED_ASM(NAMED_REFERENCE_PRED_SQUARE_EXPAND "je 1f;" OBFH_CFLOW_DATA_FRAME "1:" NAMED_REFERENCE_PRED_SUB_BORROW "je 2f;" OBFH_CFLOW_DATA_RETURN "2:")

#define NAMED_REFERENCE_TEMPLATE_120 \
    OBFH_CFLOW_LOCAL_NAMED_ASM(NAMED_REFERENCE_PRED_NEG_SQUARE "jne 1f; jmp 2f; 1:" OBFH_CFLOW_DATA_INDIRECT "2:" NAMED_REFERENCE_PRED_DEMORGAN "sete %%cl; movzbl %%cl, %%ecx; testl %%ecx, %%ecx; jnz 3f;" OBFH_CFLOW_DATA_CALL "3:")

#define NAMED_REFERENCE_TEMPLATE_121 \
    OBFH_CFLOW_LOCAL_NAMED_ASM(NAMED_REFERENCE_PRED_BSWAP_NOT "sete %%cl; movzbl %%cl, %%ecx; negl %%ecx; testl %%ecx, %%ecx; js 1f;" OBFH_CFLOW_DATA_TRAP "1:" NAMED_REFERENCE_PRED_OR_DISTRIBUTE "jne 2f; jmp 3f; 2:" OBFH_CFLOW_DATA_STACK "3:")

#define NAMED_REFERENCE_TEMPLATE_122 \
    OBFH_CFLOW_LOCAL_NAMED_ASM(NAMED_REFERENCE_PRED_SUB_MASK "je 2f; 1:" OBFH_CFLOW_DATA_RETURN "2:" NAMED_REFERENCE_PRED_AND_PARTITION "jne 1b; jmp 3f;" OBFH_CFLOW_DATA_FRAME "3:")

#define NAMED_REFERENCE_TEMPLATE_123 \
    OBFH_CFLOW_LOCAL_NAMED_ASM(NAMED_REFERENCE_PRED_MASK_ORDER "jbe 1f;" OBFH_CFLOW_DATA_CALL "1:" NAMED_REFERENCE_PRED_XOR_CANCEL "je 2f;" OBFH_CFLOW_DATA_INDIRECT "2:")

#define NAMED_REFERENCE_TEMPLATE_124 \
    OBFH_CFLOW_LOCAL_NAMED_ASM(NAMED_REFERENCE_PRED_COMPLEMENT_TRANSLATE "jne 1f; jmp 2f; 1:" OBFH_CFLOW_DATA_STACK "2:" NAMED_REFERENCE_PRED_COMPLEMENT_CARRY "setnc %%cl; movzbl %%cl, %%ecx; testl %%ecx, %%ecx; jnz 3f;" OBFH_CFLOW_DATA_TRAP "3:")

#define NAMED_REFERENCE_TEMPLATE_125 \
    OBFH_CFLOW_LOCAL_NAMED_ASM(NAMED_REFERENCE_PRED_SQUARE_RESIDUE "sete %%cl; movzbl %%cl, %%ecx; negl %%ecx; testl %%ecx, %%ecx; js 1f;" OBFH_CFLOW_DATA_FRAME "1:" NAMED_REFERENCE_PRED_COMPLEMENT_WRAP "jnc 2f; jmp 3f; 2:" OBFH_CFLOW_DATA_RETURN "3:")

#define NAMED_REFERENCE_TEMPLATE_126 \
    OBFH_CFLOW_LOCAL_NAMED_ASM(NAMED_REFERENCE_PRED_FOURTH_RESIDUE "je 2f; 1:" OBFH_CFLOW_DATA_INDIRECT "2:" NAMED_REFERENCE_PRED_MASK_SUBTRACT "jc 1b; jmp 3f;" OBFH_CFLOW_DATA_CALL "3:")

#define NAMED_REFERENCE_TEMPLATE_127 \
    OBFH_CFLOW_LOCAL_NAMED_ASM(NAMED_REFERENCE_PRED_ISOLATE_BIT "je 1f;" OBFH_CFLOW_DATA_TRAP "1:" NAMED_REFERENCE_PRED_MASK_OR_ORDER "jae 2f;" OBFH_CFLOW_DATA_STACK "2:")

`;

function fixture(header) {
    const start = header.indexOf('#define OBFH_CFLOW_FILL ');
    const end = header.indexOf('#define OBFH_CFLOW_VARIANT_COUNT ', start);
    if (start < 0 || end < start || !header.includes('#define OBFH_CFLOW_NAMED_EMIT(')) throw new Error('Missing factored named CFLOW emitter');
    let code = '#include <stdint.h>\n#include <stdio.h>\n#define __obfh_asm__ __asm__\n#define OBFH_CFLOW_CLOBBERS "eax","ecx","edx","cc","memory"\n' + header.slice(start, end) + '\n' + reference;
    code += `
typedef struct { const unsigned char *start, *end; } NamedSpan;
#if defined(__x86_64__)
#define NAMED_ADDRESS "leaq 1f(%%rip),%%rax;movq %%rax,%[start];leaq 9f(%%rip),%%rax;movq %%rax,%[end];jmp 9f;1:"
#else
#define NAMED_ADDRESS "leal 1f,%%eax;movl %%eax,%[start];leal 9f,%%eax;movl %%eax,%[end];jmp 9f;1:"
#endif
#define NAMED_CARRIER(name, body, salt, pad, byte, key, rotate) static NamedSpan name(void) { NamedSpan span;\
enum { __obfh_cf_salt=(salt), __obfh_cf_pad=(pad), __obfh_cf_byte=(byte), __obfh_cf_word=(key), __obfh_cf_rotate=(rotate),\
       __obfh_cf_byte0=0, __obfh_cf_byte1=255, __obfh_cf_byte2=128, __obfh_cf_byte3=85, __obfh_cf_payload=0x89abcdefu };\
OBFH_INPUT_CORE(__ci,__obfh_cf_salt,__obfh_cf_rotate,__obfh_cf_word);\
OBFH_INPUT_EAX(__ci,__obfh_cf_salt); OBFH_INPUT_OTHER(__ci,__obfh_cf_salt,1); OBFH_INPUT_OTHER(__ci,__obfh_cf_salt,2);\
enum { __ct=__ci_o ? (unsigned int)__ci_x>>24 : (unsigned int)__ci_r>>16 };\
__asm__ __volatile__(OBFH_CFLOW_PRED_CONSTANTS OBFH_CFLOW_INPUT_CONSTANTS OBFH_CFLOW_DATA_CONSTANTS : : OBFH_CFLOW_LOCAL_NAMED_INPUTS);\
__asm__ __volatile__(NAMED_ADDRESS : [start]"=m"(span.start),[end]"=m"(span.end) : : OBFH_CFLOW_CLOBBERS);\
body; __asm__ __volatile__("9:"); return span; }
`;
    const actual = [], expected = [];
    let config = 0;
    for (let form = 0; form < 16; ++form) {
        for (const rotate of [1, 2, 31]) {
            const key = (([0, 0x7fffffff, 0x80000000, 0xffffffff][config % 4] & ~((3 << 17) | (1 << 12) | (1 << 13))) | ((form >> 2) << 17) | ((form & 1) << 12) | (((form >> 1) & 1) << 13)) >>> 0;
            const salt = [1, 127, 128, 32767][config % 4];
            const args = [salt, config % 7 + 1, [0, 255, 128][config % 3], key, rotate].map(value => value + 'u').join(',');
            for (let index = 99; index < 128; ++index) {
                const id = config * 29 + index - 99;
                code += `NAMED_CARRIER(named_actual_${id},OBFH_CFLOW_NAMED_EMIT(${index}u),${args})\n`;
                code += `NAMED_CARRIER(named_reference_${id},NAMED_REFERENCE_TEMPLATE_${index},${args})\n`;
                actual.push('named_actual_' + id);
                expected.push('named_reference_' + id);
            }
            ++config;
        }
    }
    code += `int main(void) {
NamedSpan (*actual[])(void)={${actual.join(',')}};
NamedSpan (*expected[])(void)={${expected.join(',')}};
for (unsigned i=0; i<sizeof(actual)/sizeof(actual[0]); ++i) {
    NamedSpan a=actual[i](), b=expected[i](); uintptr_t length=(uintptr_t)a.end-(uintptr_t)a.start;
    if (length!=(uintptr_t)b.end-(uintptr_t)b.start || length>512) { fprintf(stderr,"named CFLOW length: config %u template %u\\n",i/29,i%29+99); return 1; }
    for (uintptr_t n=0; n<length; ++n) if (a.start[n]!=b.start[n]) { fprintf(stderr,"named CFLOW byte: config %u template %u offset %u\\n",i/29,i%29+99,(unsigned)n); return 2; }
}
if (puts("CFLOW_NAMED_PASS")==EOF || fflush(stdout)) return 3; return 0;
}\n`;
    return code;
}
module.exports = { fixture };
