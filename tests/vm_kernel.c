#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "../include/obfus.h"
#undef if
#undef else
#undef for
#undef while
#undef break
#undef switch
#undef printf
static unsigned int visited[OBFH_V_RETURN + 1], paths[8][32], current_variant, current_operation, arithmetic, trace[128], trace_count;
static unsigned int operation_paths[8][OBFH_VOP_BSHR + 1][32];
void study_step(unsigned int op, unsigned int pc) {
    if (op > OBFH_V_RETURN || pc >= 32)
        ExitProcess(90);
    if (trace_count >= 128)
        ExitProcess(90);
    trace[trace_count++] = pc;
    visited[op]++;
    paths[current_variant][pc]++;
    operation_paths[current_variant][current_operation][pc]++;
    if (op >= OBFH_V_ADD && op <= OBFH_V_MOD)
        arithmetic++;
}
#define __obfh_voperation operation
static long double variant_0(unsigned int operation, long double a, long double b) {
    enum {
        __obfh_vkey = (0x5d721300u ^ (unsigned int)OBFH_BUILD_SEED) + 0u * 3266489917u,
        __obfh_va = __obfh_vkey & 3u,
        __obfh_vb = (__obfh_va + ((__obfh_vkey & 4u) ? 3u : 1u)) & 3u,
        __obfh_vc = (__obfh_va + 2u) & 3u,
        __obfh_vd = (__obfh_va + ((__obfh_vkey & 4u) ? 1u : 3u)) & 3u
    };
    current_variant = 0;
    const unsigned int *p = OBFH_V_PROGRAM_0;
    return Obfh_VirtualMachine(p, 4, __obfh_vkey, obfh_vm_encode(a, SALT_NUM1, 1), obfh_vm_encode(b, SALT_NUM2, 1));
}
static long double variant_1(unsigned int operation, long double a, long double b) {
    enum {
        __obfh_vkey = (0x5d721300u ^ (unsigned int)OBFH_BUILD_SEED) + 1u * 3266489917u,
        __obfh_va = __obfh_vkey & 3u,
        __obfh_vb = (__obfh_va + ((__obfh_vkey & 4u) ? 3u : 1u)) & 3u,
        __obfh_vc = (__obfh_va + 2u) & 3u,
        __obfh_vd = (__obfh_va + ((__obfh_vkey & 4u) ? 1u : 3u)) & 3u
    };
    current_variant = 1;
    const unsigned int *p = OBFH_V_PROGRAM_1;
    return Obfh_VirtualMachine(p, 6, __obfh_vkey, obfh_vm_encode(a, SALT_NUM1, 1), obfh_vm_encode(b, SALT_NUM2, 1));
}
static long double variant_2(unsigned int operation, long double a, long double b) {
    enum {
        __obfh_vkey = (0x5d721300u ^ (unsigned int)OBFH_BUILD_SEED) + 2u * 3266489917u,
        __obfh_va = __obfh_vkey & 3u,
        __obfh_vb = (__obfh_va + ((__obfh_vkey & 4u) ? 3u : 1u)) & 3u,
        __obfh_vc = (__obfh_va + 2u) & 3u,
        __obfh_vd = (__obfh_va + ((__obfh_vkey & 4u) ? 1u : 3u)) & 3u
    };
    current_variant = 2;
    const unsigned int *p = OBFH_V_PROGRAM_2;
    return Obfh_VirtualMachine(p, 11, __obfh_vkey, obfh_vm_encode(a, SALT_NUM1, 1), obfh_vm_encode(b, SALT_NUM2, 1));
}
static long double variant_3(unsigned int operation, long double a, long double b) {
    enum {
        __obfh_vkey = (0x5d721300u ^ (unsigned int)OBFH_BUILD_SEED) + 3u * 3266489917u,
        __obfh_va = __obfh_vkey & 3u,
        __obfh_vb = (__obfh_va + ((__obfh_vkey & 4u) ? 3u : 1u)) & 3u,
        __obfh_vc = (__obfh_va + 2u) & 3u,
        __obfh_vd = (__obfh_va + ((__obfh_vkey & 4u) ? 1u : 3u)) & 3u
    };
    current_variant = 3;
    const unsigned int *p = OBFH_V_PROGRAM_3;
    return Obfh_VirtualMachine(p, 10, __obfh_vkey, obfh_vm_encode(a, SALT_NUM1, 1), obfh_vm_encode(b, SALT_NUM2, 1));
}
static long double variant_4(unsigned int operation, long double a, long double b) {
    enum {
        __obfh_vkey = (0x5d721300u ^ (unsigned int)OBFH_BUILD_SEED) + 4u * 3266489917u,
        __obfh_va = __obfh_vkey & 3u,
        __obfh_vb = (__obfh_va + ((__obfh_vkey & 4u) ? 3u : 1u)) & 3u,
        __obfh_vc = (__obfh_va + 2u) & 3u,
        __obfh_vd = (__obfh_va + ((__obfh_vkey & 4u) ? 1u : 3u)) & 3u
    };
    current_variant = 4;
    const unsigned int *p = OBFH_V_PROGRAM_4;
    return Obfh_VirtualMachine(p, 12, __obfh_vkey, obfh_vm_encode(a, SALT_NUM1, 1), obfh_vm_encode(b, SALT_NUM2, 1));
}
static long double variant_5(unsigned int operation, long double a, long double b) {
    enum {
        __obfh_vkey = (0x5d721300u ^ (unsigned int)OBFH_BUILD_SEED) + 5u * 3266489917u,
        __obfh_va = __obfh_vkey & 3u,
        __obfh_vb = (__obfh_va + ((__obfh_vkey & 4u) ? 3u : 1u)) & 3u,
        __obfh_vc = (__obfh_va + 2u) & 3u,
        __obfh_vd = (__obfh_va + ((__obfh_vkey & 4u) ? 1u : 3u)) & 3u
    };
    current_variant = 5;
    const unsigned int *p = OBFH_V_PROGRAM_5;
    return Obfh_VirtualMachine(p, 11, __obfh_vkey, obfh_vm_encode(a, SALT_NUM1, 1), obfh_vm_encode(b, SALT_NUM2, 1));
}
static long double variant_6(unsigned int operation, long double a, long double b) {
    enum {
        __obfh_vkey = (0x5d721300u ^ (unsigned int)OBFH_BUILD_SEED) + 6u * 3266489917u,
        __obfh_va = __obfh_vkey & 3u,
        __obfh_vb = (__obfh_va + ((__obfh_vkey & 4u) ? 3u : 1u)) & 3u,
        __obfh_vc = (__obfh_va + 2u) & 3u,
        __obfh_vd = (__obfh_va + ((__obfh_vkey & 4u) ? 1u : 3u)) & 3u
    };
    current_variant = 6;
    const unsigned int *p = OBFH_V_PROGRAM_6;
    return Obfh_VirtualMachine(p, 8, __obfh_vkey, obfh_vm_encode(a, SALT_NUM1, 1), obfh_vm_encode(b, SALT_NUM2, 1));
}
static long double variant_7(unsigned int operation, long double a, long double b) {
    enum {
        __obfh_vkey = (0x5d721300u ^ (unsigned int)OBFH_BUILD_SEED) + 7u * 3266489917u,
        __obfh_va = __obfh_vkey & 3u,
        __obfh_vb = (__obfh_va + ((__obfh_vkey & 4u) ? 3u : 1u)) & 3u,
        __obfh_vc = (__obfh_va + 2u) & 3u,
        __obfh_vd = (__obfh_va + ((__obfh_vkey & 4u) ? 1u : 3u)) & 3u
    };
    current_variant = 7;
    const unsigned int *p = OBFH_V_PROGRAM_7;
    return Obfh_VirtualMachine(p, 15, __obfh_vkey, obfh_vm_encode(a, SALT_NUM1, 1), obfh_vm_encode(b, SALT_NUM2, 1));
}
#undef __obfh_voperation
static long double (*variants[])(unsigned int, long double, long double) = {variant_0, variant_1, variant_2, variant_3,
                                                                            variant_4, variant_5, variant_6, variant_7};
static long double expected(unsigned int op, long double a, long double b) {
    switch (op) {
        case 0:
            return a + b;
        case 1:
            return a - b;
        case 2:
            return a * b;
        case 3:
            return a / b;
        case 4:
            return (int)b && !((int)a == INT_MIN && (int)b == -1) ? (int)a % (int)b : 0;
        case 5:
            return a == b;
        case 6:
            return a != b;
        case 7:
            return a < b;
        case 8:
            return a > b;
        case 9:
            return a <= b;
        case 10:
            return a >= b;
        case 11:
            return a;
        default:
            return !!a;
    }
}
static int same(long double a, long double b) {
    return (a != a && b != b) || (a == b && (a != 0 || signbit(a) == signbit(b)));
}
static int failure(unsigned int mode) {
    enum {
        __obfh_vkey = 0x12345678u
    };
    unsigned int p[3], len = 1;
    switch (mode) {
        case 1:
            p[0] = OBFH_V_ENCODE(OBFH_V_INSTRUCTION(OBFH_V_RETURN + 1, 0, 0, 0, 0), 0);
            break;
        case 2:
            p[0] = OBFH_V_ENCODE(OBFH_V_INSTRUCTION(OBFH_V_RETURN, 0, 0, 0, 0), 0);
            break;
        case 3:
            p[0] = OBFH_V_ENCODE(OBFH_V_INSTRUCTION(OBFH_V_JUMP, 0, 0, 0, 2), 0);
            break;
        case 4:
            p[0] = OBFH_V_ENCODE(OBFH_V_INSTRUCTION(OBFH_V_JUMP, 0, 0, 0, 0), 0);
            break;
        case 5:
            p[0] = OBFH_V_ENCODE(OBFH_V_INSTRUCTION(OBFH_V_LOAD_A, 0, 0, 0, 0), 0);
            break;
        case 6:
            p[0] = OBFH_V_ENCODE(OBFH_V_INSTRUCTION(OBFH_V_LOAD_A, 0, 0, 0, 0), 0);
            p[1] = OBFH_V_ENCODE(OBFH_V_INSTRUCTION(OBFH_V_BOOLEAN, 0, 0, 0, 7), 1);
            len = 2;
            break;
        case 7:
            len = 33;
            break;
        case 8:
            p[0] = OBFH_V_ENCODE(OBFH_V_INSTRUCTION(OBFH_V_BOOLEAN, 0, 0, 0, 0), 0);
            break;
        case 9:
            p[0] = OBFH_V_ENCODE(OBFH_V_INSTRUCTION(OBFH_V_JFLAG, 0, 1, 0, 0), 0);
            break;
        case 10:
            p[0] = OBFH_V_ENCODE(OBFH_V_INSTRUCTION(OBFH_V_CADD, 0, 0, 0, 0), 0);
            p[1] = OBFH_V_ENCODE(OBFH_V_INSTRUCTION(OBFH_V_JFLAG, 2, 1, 0, 0), 1);
            len = 2;
            break;
        case 11:
            Obfh_VirtualMachine(NULL, 1, __obfh_vkey, obfh_vm_encode(0, SALT_NUM1, 0), obfh_vm_encode(0, SALT_NUM2, 0));
            return 91;
        case 12:
            len = 0;
            break;
        case 13:
            p[0] = OBFH_V_ENCODE(OBFH_V_INSTRUCTION(OBFH_V_LOAD_PAIR, 0, 0, 0, 0), 0);
            break;
        case 14:
            p[0] = OBFH_V_ENCODE(OBFH_V_INSTRUCTION(OBFH_V_COPY_RETURN, 1, 0, 0, 0), 0);
            break;
        default:
            return 92;
    }
    Obfh_VirtualMachine(p, len, __obfh_vkey, obfh_vm_encode(0, SALT_NUM1, 0), obfh_vm_encode(0, SALT_NUM2, 0));
    return 91;
}
static int route(unsigned int v, long double a, long double b, long double result) {
    unsigned int expected[32], count = 0;
#define PC(n) expected[count++] = (n)
    switch (v) {
        case 0:
            for (unsigned int p = 0; p < 4; ++p)
                PC(p);
            break;
        case 1:
            for (unsigned int p = 0; p < 6; ++p)
                PC(p);
            break;
        case 2:
        case 3:
            for (unsigned int p = 0; p < 4; ++p)
                PC(p);
            {
                unsigned int first = a != 0 ? 8 : 4;
                PC(first);
                PC(first + 1);
                PC(first + 2);
            }
            if (v == 2)
                PC(11);
            break;
        case 4:
            PC(0);
            PC(4);
            PC(5);
            PC(6);
            PC(7);
            if (a != 0) {
                PC(10);
                PC(11);
                PC(12);
            } else {
                PC(8);
                PC(9);
            }
            PC(1);
            PC(2);
            PC(3);
            break;
        case 5:
            for (unsigned int p = 0; p < 5; ++p)
                PC(p);
            {
                unsigned int first = a != 0 ? 8 : 5;
                PC(first);
                PC(first + 1);
                PC(first + 2);
            }
            PC(11);
            break;
        case 6:
            PC(0);
            PC(1);
            PC(2);
            PC(3);
            PC(4);
            PC(3);
            PC(4);
            PC(5);
            PC(6);
            PC(7);
            PC(8);
            break;
        default:
            PC(0);
            PC(1);
            PC(2);
            PC(3);
            if (a != 0) {
                PC(7);
                PC(8);
            } else {
                PC(4);
                PC(5);
                PC(6);
            }
            PC(9);
            PC(10);
            PC(11);
            PC(12);
            if (result != 0) {
                PC(15);
            } else {
                PC(13);
                PC(14);
            }
    }
#undef PC
    if (v >= 2) {
        unsigned int removed = v == 4 ? 5 : 1, out = 0;
        for (unsigned int i = 0; i < count; ++i)
            if (expected[i] != removed) expected[out++] = expected[i] > removed ? expected[i] - 1 : expected[i];
        count = out;
    }
    if (count != trace_count)
        return 0;
    for (unsigned int i = 0; i < count; ++i)
        if (expected[i] != trace[i])
            return 0;
    return 1;
}
int main(int argc, char **argv) {
    if (argc > 1)
        return failure(atoi(argv[1]));
    const long double values[] = {0.0L,
                                  -0.0L,
                                  1,
                                  -1,
                                  2.25L,
                                  -2,
                                  (long double)INT_MIN,
                                  (long double)INT_MAX,
                                  DBL_MIN,
                                  (long double)HUGE_VAL,
                                  -(long double)HUGE_VAL,
                                  (long double)NAN};
    const unsigned int count = sizeof(values) / sizeof(values[0]);
    for (unsigned int v = 0; v < 8; v++)
        for (unsigned int op = 0; op < 13; op++)
            for (unsigned int i = 0; i < count; i++)
                for (unsigned int j = 0; j < count; j++) {
                    if (op >= OBFH_VOP_ID && j != 0)
                        continue;
                    current_operation = op;
                    if (op == OBFH_VOP_MOD &&
                        (!(values[i] >= INT_MIN && values[i] <= INT_MAX) || !(values[j] >= INT_MIN && values[j] <= INT_MAX)))
                        continue;
                    arithmetic = 0;
                    trace_count = 0;
                    long double actual = variants[v](op, values[i], values[j]), ref = expected(op, values[i], values[j]);
                    if (!same(actual, ref) || arithmetic != (op < 5 ? 1 : 0) || !route(v, values[i], values[j], ref)) {
                        printf("FAIL variant %u operation %u operands %u/%u arithmetic %u\n", v, op, i, j, arithmetic);
                        return 1;
                    }
                }
    const unsigned int bits[] = {0, 1, 31, 32, 0x55555555u, 0x80000000u, 0xffffffffu};
    for (unsigned int v = 0; v < 8; ++v)
        for (unsigned int op = OBFH_VOP_BAND; op <= OBFH_VOP_BSHR; ++op)
            for (unsigned int i = 0; i < 7; ++i)
                for (unsigned int j = 0; j < 7; ++j) {
                    unsigned int a = bits[i], b = bits[j], ref = 0;
                    switch (op) {
                        case OBFH_VOP_BAND:
                            ref = a & b;
                            break;
                        case OBFH_VOP_BOR:
                            ref = a | b;
                            break;
                        case OBFH_VOP_BXOR:
                            ref = a ^ b;
                            break;
                        case OBFH_VOP_BNOT:
                            ref = ~a;
                            break;
                        case OBFH_VOP_BSHL:
                            ref = a << (b & 31u);
                            break;
                        case OBFH_VOP_BSHR:
                            ref = a >> (b & 31u);
                            break;
                    }
                    current_operation = op;
                    trace_count = 0;
                    unsigned int opcode = OBFH_V_BAND + op - OBFH_VOP_BAND, before = visited[opcode];
                    long double actual = variants[v](op, (long double)a, (long double)b);
                    if (actual != (long double)ref || visited[opcode] != before + 1 || !route(v, a, b, ref)) return 8;
                }
    for (unsigned int op = 1; op <= OBFH_V_RETURN; op++)
        if (!visited[op]) {
            printf("Unvisited opcode %u\n", op);
            return 2;
        }
    for (unsigned int v = 0; v < 8; v++)
        for (unsigned int pc = 0; pc < 16; pc++) {
            const unsigned int lengths[] = {4, 6, 11, 10, 12, 11, 8, 15};
            if (pc >= lengths[v] || (pc == 6 && (v == 2 || v == 3)))
                continue;
            for (unsigned int op = 0; op < 13; ++op)
                if (!operation_paths[v][op][pc]) {
                    printf("Unvisited variant %u pc %u\n", v, pc);
                    return 3;
                }
        }
    /* Exercise all opcode residues and odd multipliers, not just generated commands. */
    for (unsigned int multiplier = 1; multiplier < 256; multiplier += 2) {
        unsigned int key = (multiplier << 8) | 0xa5u;
        for (unsigned int opcode = 0; opcode < 256; ++opcode) {
            unsigned int word = (opcode * multiplier + (key & 255u)) & 255u;
            if (obfh_v_opcode(word, key) != opcode)
                return 4;
        }
    }
    OBFH_V_CONTEXT context;
    context.key = (unsigned int)OBFH_BUILD_SEED;
    context.flag_key = obfh_v_mix(context.key ^ 0x85ebca6bu);
    const unsigned int controls[] = {0, 1, 0x7fffffffu, 0x80000000u, 0xffffffffu};
    for (unsigned int r = 0; r < 4; ++r)
        for (unsigned int i = 0; i < 5; ++i) {
            obfh_v_control_write(&context, r, controls[i]);
            if (obfh_v_control_read(&context, r) != controls[i])
                return 5;
        }
    for (unsigned int f = 0; f < 64; ++f) {
        obfh_v_set_flags(&context, f);
        if (obfh_v_flags(&context) != f)
            return 6;
    }
    trace_count = 0;
    /* BOOLEAN consumes flags only: no value register must be initialized beforehand. */
    {
        enum {
            __obfh_vkey = 0u
        };
        const unsigned int program[] = {OBFH_V_ENCODE(OBFH_V_INSTRUCTION(OBFH_V_CADD, 0, 0, 0, 0), 0),
                                        OBFH_V_ENCODE(OBFH_V_INSTRUCTION(OBFH_V_BOOLEAN, 0, 3, 0, 0), 1),
                                        OBFH_V_ENCODE(OBFH_V_INSTRUCTION(OBFH_V_RETURN, 0, 0, 0, 0), 2)};
        if (Obfh_VirtualMachine(program, 3, __obfh_vkey, obfh_vm_encode(0, SALT_NUM1, 0), obfh_vm_encode(0, SALT_NUM2, 0)) != 0)
            return 7;
    }
    /* Force every physical format independently of the seeded template selection. */
#define FORMAT_CASE(layout) \
    { \
        enum { __obfh_vkey = 0x11223344u | ((layout) << 29) }; \
        const unsigned int program[] = { \
            OBFH_V_ENCODE(OBFH_V_INSTRUCTION(OBFH_V_LOAD_PAIR, 0, 1, 0, 0), 0), \
            OBFH_V_ENCODE(OBFH_V_INSTRUCTION(OBFH_V_ADD, 2, 0, 1, 0), 1), \
            OBFH_V_ENCODE(OBFH_V_INSTRUCTION(OBFH_V_COPY_RETURN, 3, 2, 0, 0), 2)}; \
        trace_count = 0; \
        if (Obfh_VirtualMachine(program, 3, __obfh_vkey, obfh_vm_encode(-1.25L, SALT_NUM1, 1), \
                                obfh_vm_encode(3.75L, SALT_NUM2, 1)) != 2.5L || \
            trace_count != 3) return 9; \
    }
    FORMAT_CASE(0u);
    FORMAT_CASE(1u);
    FORMAT_CASE(2u);
    FORMAT_CASE(3u);
#undef FORMAT_CASE
    puts("KERNEL_PASS");
    return 0;
}
