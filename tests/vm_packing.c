/* Independent canonical-word oracle. Keep packing separate from field construction. */
static unsigned reference_opcode(unsigned op, unsigned key, unsigned seed) {
    unsigned m = ((key >> 8) & 255u) | 1u, k = key & 255u;
    switch ((seed >> 24) % 6u) {
        case 0:
            return (op * m + k) & 255u;
        case 1:
            return (op * m ^ k) & 255u;
        case 2:
            return ((op ^ k) * m) & 255u;
        case 3:
            return ((op + k) * m) & 255u;
        case 4:
            return (op * m - k) & 255u;
        default:
            return ((op - k) * m) & 255u;
    }
}
static unsigned reference_word(unsigned word, unsigned key, unsigned pc, unsigned seed) {
    unsigned packed, rotate = key % 31u + 1u, mode = ((seed >> 6) & 15u) % 9u;
    unsigned op = word & 255u, x = (word >> 8) & 3u, y = (word >> 10) & 3u, z = (word >> 12) & 3u, imm = word >> 14;
    switch ((key >> 29) & 3u) {
        case 0:
            packed = word;
            break;
        case 1:
            packed = (op << 24) | (x << 18) | (y << 20) | (z << 22) | imm;
            break;
        case 2:
            packed = (op << 6) | x | (y << 2) | (z << 4) | (imm << 14);
            break;
        default:
            packed = op | (x << 12) | (y << 8) | (z << 10) | (imm << 14);
            break;
    }
    packed = (packed >> rotate) | (packed << (32u - rotate));
    unsigned mask = mode % 3u == 0 ? key + pc * 0x9e3779b9u : mode % 3u == 1 ? key ^ (pc * 0x9e3779b9u)
                                                                             : key - pc * 0x9e3779b9u;
    return mode / 3u == 0 ? packed ^ mask : mode / 3u == 1 ? packed + mask
                                                           : packed - mask;
}
static int reference_programs(const unsigned **actual, unsigned key, unsigned operation, unsigned ra, unsigned rb, unsigned rc, unsigned rd, unsigned seed) {
    static const unsigned fields[6][3] = {{8, 10, 12}, {8, 12, 10}, {10, 12, 8}, {10, 8, 12}, {12, 8, 10}, {12, 10, 8}};
    static const unsigned lengths[8] = {4, 6, 11, 10, 12, 11, 8, 15};
    static const unsigned operations[19] = {OBFH_V_ADD, OBFH_V_SUB, OBFH_V_MUL, OBFH_V_DIV, OBFH_V_MOD, OBFH_V_COMPARE, OBFH_V_COMPARE, OBFH_V_COMPARE, OBFH_V_COMPARE, OBFH_V_COMPARE, OBFH_V_COMPARE, OBFH_V_MOVE, OBFH_V_TEST, OBFH_V_BAND, OBFH_V_BOR, OBFH_V_BXOR, OBFH_V_BNOT, OBFH_V_BSHL, OBFH_V_BSHR};
    const unsigned *f = fields[(seed >> 14) % 6u];
#define REF_INSTRUCTION(op, d, a, b, imm) (reference_opcode(op, key, seed) | ((unsigned)(d) << f[0]) | ((unsigned)(a) << f[1]) | ((unsigned)(b) << f[2]) | (((unsigned)(imm)&0x3ffffu) << 14))
#define REF_OPERATION_0(d, a, b) REF_INSTRUCTION(operations[operation], d, a, b, 0)
#define REF_OPERATION_1(d, a) REF_INSTRUCTION((operation >= 5 && operation <= 10) || operation == 12 ? OBFH_V_BOOLEAN : OBFH_V_CXOR, d, a, 0, operation == 12 ? 6 : operation >= 5 ? operation - 5 \
                                                                                                                                                                                   : 0)
    unsigned load0 = key & 32u ? REF_INSTRUCTION(OBFH_V_LOAD_A, ra, 0, 0, 0) : REF_INSTRUCTION(OBFH_V_LOAD_B, rb, 0, 0, 0);
    unsigned load1 = key & 32u ? REF_INSTRUCTION(OBFH_V_LOAD_B, rb, 0, 0, 0) : REF_INSTRUCTION(OBFH_V_LOAD_A, ra, 0, 0, 0);
    unsigned prep0 = key & 64u ? REF_INSTRUCTION(OBFH_V_CROL, 1, 0, 0, 7) : REF_INSTRUCTION(OBFH_V_MOVE, rd, ra, 0, 0);
    unsigned prep1 = key & 64u ? REF_INSTRUCTION(OBFH_V_MOVE, rd, ra, 0, 0) : REF_INSTRUCTION(OBFH_V_CROL, 1, 0, 0, 7);
    unsigned expected[8][15] = {
        {REF_INSTRUCTION(OBFH_V_LOAD_PAIR, ra, rb, 0, 0),
         REF_OPERATION_0(rc, ra, rb),
         REF_OPERATION_1(rc, ra),
         REF_INSTRUCTION(OBFH_V_RETURN, 0, rc, 0, 0)},
        {load0,
         load1,
         REF_INSTRUCTION(OBFH_V_MOVE, rd, ra, 0, 0),
         REF_OPERATION_0(rc, rd, rb),
         REF_OPERATION_1(rc, rd),
         REF_INSTRUCTION(OBFH_V_COPY_RETURN, ra, rc, 0, 0)},
        {REF_INSTRUCTION(OBFH_V_LOAD_PAIR, ra, rb, 0, 0),
         REF_INSTRUCTION(OBFH_V_TEST, 0, ra, 0, 0),
         REF_INSTRUCTION(OBFH_V_JFLAG, 0, 1, 0, 7),
         REF_OPERATION_0(rc, ra, rb),
         REF_OPERATION_1(rc, ra),
         REF_INSTRUCTION(OBFH_V_JUMP, 0, 0, 0, 10),
         REF_INSTRUCTION(OBFH_V_CROL, 0, 0, 0, 3),
         REF_OPERATION_0(rd, ra, rb),
         REF_OPERATION_1(rd, ra),
         REF_INSTRUCTION(OBFH_V_MOVE, rc, rd, 0, 0),
         REF_INSTRUCTION(OBFH_V_RETURN, 0, rc, 0, 0)},
        {REF_INSTRUCTION(OBFH_V_LOAD_PAIR, ra, rb, 0, 0),
         REF_INSTRUCTION(OBFH_V_TEST, 0, ra, 0, 0),
         REF_INSTRUCTION(OBFH_V_JFLAG, 0, 1, 0, 7),
         REF_OPERATION_0(rc, ra, rb),
         REF_OPERATION_1(rc, ra),
         REF_INSTRUCTION(OBFH_V_RETURN, 0, rc, 0, 0),
         REF_INSTRUCTION(OBFH_V_CXOR, 0, 0, 0, 5),
         REF_OPERATION_0(rd, ra, rb),
         REF_OPERATION_1(rd, ra),
         REF_INSTRUCTION(OBFH_V_RETURN, 0, rd, 0, 0)},
        {REF_INSTRUCTION(OBFH_V_JUMP, 0, 0, 0, 4),
         REF_OPERATION_0(rc, ra, rb),
         REF_OPERATION_1(rc, ra),
         REF_INSTRUCTION(OBFH_V_RETURN, 0, rc, 0, 0),
         REF_INSTRUCTION(OBFH_V_LOAD_PAIR, ra, rb, 0, 0),
         REF_INSTRUCTION(OBFH_V_TEST, 0, ra, 0, 0),
         REF_INSTRUCTION(OBFH_V_JFLAG, 0, 1, 0, 9),
         REF_INSTRUCTION(OBFH_V_MOVE, rd, ra, 0, 0),
         REF_INSTRUCTION(OBFH_V_JUMP, 0, 0, 0, 1),
         REF_INSTRUCTION(OBFH_V_SWAP, 0, ra, rb, 0),
         REF_INSTRUCTION(OBFH_V_SWAP, 0, ra, rb, 0),
         REF_INSTRUCTION(OBFH_V_JUMP, 0, 0, 0, 1)},
        {REF_INSTRUCTION(OBFH_V_LOAD_PAIR, ra, rb, 0, 0),
         REF_INSTRUCTION(OBFH_V_SWAP, 0, ra, rb, 0),
         REF_INSTRUCTION(OBFH_V_TEST, 0, rb, 0, 0),
         REF_INSTRUCTION(OBFH_V_JFLAG, 0, 1, 0, 7),
         REF_OPERATION_0(rc, rb, ra),
         REF_OPERATION_1(rc, rb),
         REF_INSTRUCTION(OBFH_V_JUMP, 0, 0, 0, 10),
         REF_OPERATION_0(rd, rb, ra),
         REF_OPERATION_1(rd, rb),
         REF_INSTRUCTION(OBFH_V_MOVE, rc, rd, 0, 0),
         REF_INSTRUCTION(OBFH_V_RETURN, 0, rc, 0, 0)},
        {REF_INSTRUCTION(OBFH_V_LOAD_PAIR, ra, rb, 0, 0),
         REF_INSTRUCTION(OBFH_V_CADD, 0, 0, 0, 2),
         REF_INSTRUCTION(OBFH_V_CADD, 0, 0, 0, 262143),
         REF_INSTRUCTION(OBFH_V_JFLAG, 1, 1, 0, 2),
         REF_INSTRUCTION(OBFH_V_MOVE, rd, ra, 0, 0),
         REF_OPERATION_0(rc, rd, rb),
         REF_OPERATION_1(rc, rd),
         REF_INSTRUCTION(OBFH_V_RETURN, 0, rc, 0, 0)},
        {REF_INSTRUCTION(OBFH_V_LOAD_PAIR, ra, rb, 0, 0),
         REF_INSTRUCTION(OBFH_V_TEST, 0, ra, 0, 0),
         REF_INSTRUCTION(OBFH_V_JFLAG, 0, 1, 0, 6),
         prep0,
         prep1,
         REF_INSTRUCTION(OBFH_V_JUMP, 0, 0, 0, 8),
         REF_INSTRUCTION(OBFH_V_MOVE, rd, ra, 0, 0),
         REF_INSTRUCTION(OBFH_V_CXOR, 1, 0, 0, 57),
         REF_OPERATION_0(rc, rd, rb),
         REF_OPERATION_1(rc, rd),
         REF_INSTRUCTION(OBFH_V_TEST, 0, rc, 0, 0),
         REF_INSTRUCTION(OBFH_V_JFLAG, 0, 1, 0, 14),
         REF_INSTRUCTION(OBFH_V_MOVE, ra, rc, 0, 0),
         REF_INSTRUCTION(OBFH_V_RETURN, 0, ra, 0, 0),
         REF_INSTRUCTION(OBFH_V_RETURN, 0, rc, 0, 0)}};
#undef REF_INSTRUCTION
#undef REF_OPERATION_0
#undef REF_OPERATION_1
    for (unsigned form = 0; form < 8; ++form)
        for (unsigned pc = 0; pc < lengths[form]; ++pc)
            if (actual[form][pc] != reference_word(expected[form][pc], key, pc, seed)) {
                printf("VM_PACKING_FAIL seed=%u key=%u operation=%u form=%u pc=%u\n", seed, key, operation, form, pc);
                return 1;
            }
    return 0;
}
