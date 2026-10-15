#include <stdint.h>
#include <stdio.h>

#include "../include/obfus.h"
#undef if
#undef for
#undef while
#undef else
#undef return
#undef printf
#undef puts
#undef fflush

typedef unsigned (*NativeFunction)(unsigned, unsigned);
#if defined(__x86_64__)
#define POOL_ADDRESS "leaq 2%c[pool_layout]0f(%%rip),%%rax;jmp 9f;"
#define REFERENCE_ADDRESS "leaq 2%c[pool_layout]0f(%%rip),%0;jmp 9f;"
#define READ_SP(value) __asm__ volatile("movq %%rsp,%0" \
                                        : "=r"(value))
#else
#define POOL_ADDRESS "leal 2%c[pool_layout]0f,%%eax;jmp 9f;"
#define REFERENCE_ADDRESS "leal 2%c[pool_layout]0f,%0;jmp 9f;"
#define READ_SP(value) __asm__ volatile("movl %%esp,%0" \
                                        : "=r"(value))
#endif
#define POOL_START(guard) __asm__ volatile(OBFH_SF_ASM_CONSTANTS POOL_ADDRESS \
                                           : \
                                           : OBFH_SF_POOL_INPUTS \
                                           : "eax", "edx", "ecx", "cc", "memory")
#define POOL_END __asm__ volatile("9:" \
                                  : "=a"(entry))

/* Reference assembly emits each original layout in one ASM block. The tested
 * build selects its skeleton and inline bodies independently. Compare digests. */
#define REFERENCE_LAYOUT_0u(a, b, c, d) OBFH_SF_LAYOUT_0(a, b, c)
#define REFERENCE_LAYOUT_1u(a, b, c, d) OBFH_SF_LAYOUT_1(a, b, c)
#define REFERENCE_LAYOUT_2u(a, b, c, d) OBFH_SF_LAYOUT_2(a, b, c)
#define REFERENCE_LAYOUT_3u(a, b, c, d) OBFH_SF_LAYOUT_3(a, b, c)
#define REFERENCE_LAYOUT_4u(a, b, c, d) OBFH_SF_LAYOUT_4(a, b, c)
#define REFERENCE_LAYOUT_5u(a, b, c, d) OBFH_SF_LAYOUT_5(a, b, c)
#define REFERENCE_LAYOUT_6u(a, b, c, d) OBFH_SF_LAYOUT_6(a, b, c)
#define REFERENCE_LAYOUT_7u(a, b, c, d) OBFH_SF_LAYOUT_7(a, b, c)
#define REFERENCE_LAYOUT_8u(a, b, c, d) OBFH_SF_LAYOUT_8(a, b, c)
#define REFERENCE_LAYOUT_9u(a, b, c, d) OBFH_SF_LAYOUT_9(a, b, c)
#define REFERENCE_LAYOUT_22u(a, b, c, d) OBFH_SF_LAYOUT_22(a, b, c, d)
#define REFERENCE_LAYOUT_23u(a, b, c, d) OBFH_SF_LAYOUT_23(a, b, c, d)
#define REFERENCE_LAYOUT_24u(a, b, c, d) OBFH_SF_LAYOUT_24(a, b, c, d)
#define REFERENCE_LAYOUT_25u(a, b, c, d) OBFH_SF_LAYOUT_25(a, b, c, d)
#define REFERENCE_LAYOUT_26u(a, b, c, d) OBFH_SF_LAYOUT_26(a, b, c, d)
#define REFERENCE_LAYOUT_28u(a, b, c, d) OBFH_SF_LAYOUT_28(a, b, c, d, a)
#define REFERENCE_EMIT(g, l, a, b, c, d) \
    enum { __obfh_pool_id = __COUNTER__, \
           __obfh_pool_layout = l }; \
    __asm__ volatile(OBFH_SF_ASM_CONSTANTS REFERENCE_ADDRESS REFERENCE_LAYOUT_##l(OBFH_SF_##a, OBFH_SF_##b, OBFH_SF_##c, OBFH_SF_##d) "9:" \
                     : "=r"(entry) \
                     : [pool_layout] "i"(__obfh_pool_layout), OBFH_SF_INPUTS \
                     : "eax", "edx", "ecx", "cc", "memory")
#ifdef OBFH_TEST_POOL_REFERENCE
#define NATIVE_EMIT(index) OBFH_SF_SPEC_##index(REFERENCE_EMIT)
#else
#define NATIVE_EMIT(index) OBFH_SF_POOL_RUN(OBFH_SF_RECIPE_SELECT(index), POOL_START, POOL_END)
#endif
#define DEFINE_NATIVE(index) \
    static NativeFunction native_##index(void) { \
        NativeFunction entry; \
        OBFH_SF_CAPTURE; \
        NATIVE_EMIT(index); \
        return entry; \
    }
DEFINE_NATIVE(0)
DEFINE_NATIVE(1)
DEFINE_NATIVE(2)
DEFINE_NATIVE(3)
DEFINE_NATIVE(4)
DEFINE_NATIVE(5)
DEFINE_NATIVE(6)
DEFINE_NATIVE(7)
DEFINE_NATIVE(8)
DEFINE_NATIVE(9)
DEFINE_NATIVE(10)
DEFINE_NATIVE(11)
DEFINE_NATIVE(12)
DEFINE_NATIVE(13)
DEFINE_NATIVE(14)
DEFINE_NATIVE(15)
DEFINE_NATIVE(16)
DEFINE_NATIVE(17)
DEFINE_NATIVE(18)
DEFINE_NATIVE(19)
DEFINE_NATIVE(20)
DEFINE_NATIVE(21)
DEFINE_NATIVE(22)
DEFINE_NATIVE(23)
DEFINE_NATIVE(24)
DEFINE_NATIVE(25)
DEFINE_NATIVE(26)
DEFINE_NATIVE(27)
DEFINE_NATIVE(28)
DEFINE_NATIVE(29)
DEFINE_NATIVE(30)
DEFINE_NATIVE(31)
DEFINE_NATIVE(32)
DEFINE_NATIVE(33)
DEFINE_NATIVE(34)
DEFINE_NATIVE(35)
DEFINE_NATIVE(36)
DEFINE_NATIVE(37)
DEFINE_NATIVE(38)
DEFINE_NATIVE(39)
DEFINE_NATIVE(40)
DEFINE_NATIVE(41)
DEFINE_NATIVE(42)
DEFINE_NATIVE(43)
DEFINE_NATIVE(44)
DEFINE_NATIVE(45)
DEFINE_NATIVE(46)
DEFINE_NATIVE(47)
DEFINE_NATIVE(48)
DEFINE_NATIVE(49)
DEFINE_NATIVE(50)
DEFINE_NATIVE(51)
DEFINE_NATIVE(52)
DEFINE_NATIVE(53)
DEFINE_NATIVE(54)
DEFINE_NATIVE(55)
DEFINE_NATIVE(56)
DEFINE_NATIVE(57)
DEFINE_NATIVE(58)
DEFINE_NATIVE(59)
DEFINE_NATIVE(60)
DEFINE_NATIVE(61)
DEFINE_NATIVE(62)
DEFINE_NATIVE(63)
DEFINE_NATIVE(64)
DEFINE_NATIVE(65)
DEFINE_NATIVE(66)
DEFINE_NATIVE(67)
DEFINE_NATIVE(68)
DEFINE_NATIVE(69)
DEFINE_NATIVE(70)
DEFINE_NATIVE(71)
DEFINE_NATIVE(72)
DEFINE_NATIVE(73)
DEFINE_NATIVE(74)
DEFINE_NATIVE(75)
DEFINE_NATIVE(76)
DEFINE_NATIVE(77)
DEFINE_NATIVE(78)
DEFINE_NATIVE(79)
DEFINE_NATIVE(80)
DEFINE_NATIVE(81)
DEFINE_NATIVE(82)
DEFINE_NATIVE(83)
DEFINE_NATIVE(84)
DEFINE_NATIVE(85)
DEFINE_NATIVE(86)
DEFINE_NATIVE(87)
DEFINE_NATIVE(88)
DEFINE_NATIVE(89)
DEFINE_NATIVE(90)
DEFINE_NATIVE(91)
DEFINE_NATIVE(92)
DEFINE_NATIVE(93)
DEFINE_NATIVE(94)
DEFINE_NATIVE(95)
DEFINE_NATIVE(96)
DEFINE_NATIVE(97)
DEFINE_NATIVE(98)
DEFINE_NATIVE(99)
DEFINE_NATIVE(100)
DEFINE_NATIVE(101)
DEFINE_NATIVE(102)
DEFINE_NATIVE(103)
DEFINE_NATIVE(104)
DEFINE_NATIVE(105)
DEFINE_NATIVE(106)
DEFINE_NATIVE(107)
DEFINE_NATIVE(108)
DEFINE_NATIVE(109)
DEFINE_NATIVE(110)
DEFINE_NATIVE(111)
DEFINE_NATIVE(112)
DEFINE_NATIVE(113)
DEFINE_NATIVE(114)
DEFINE_NATIVE(115)
DEFINE_NATIVE(116)
DEFINE_NATIVE(117)
DEFINE_NATIVE(118)
DEFINE_NATIVE(119)
DEFINE_NATIVE(120)
DEFINE_NATIVE(121)
DEFINE_NATIVE(122)
DEFINE_NATIVE(123)
DEFINE_NATIVE(124)
DEFINE_NATIVE(125)
DEFINE_NATIVE(126)
DEFINE_NATIVE(127)
static NativeFunction (*getters[])(void) = {native_0, native_1, native_2, native_3, native_4, native_5, native_6, native_7, native_8, native_9, native_10, native_11, native_12, native_13, native_14, native_15, native_16, native_17, native_18, native_19, native_20, native_21, native_22, native_23, native_24, native_25, native_26, native_27, native_28, native_29, native_30, native_31, native_32, native_33, native_34, native_35, native_36, native_37, native_38, native_39, native_40, native_41, native_42, native_43, native_44, native_45, native_46, native_47, native_48, native_49, native_50, native_51, native_52, native_53, native_54, native_55, native_56, native_57, native_58, native_59, native_60, native_61, native_62, native_63, native_64, native_65, native_66, native_67, native_68, native_69, native_70, native_71, native_72, native_73, native_74, native_75, native_76, native_77, native_78, native_79, native_80, native_81, native_82, native_83, native_84, native_85, native_86, native_87, native_88, native_89, native_90, native_91, native_92, native_93, native_94, native_95, native_96, native_97, native_98, native_99, native_100, native_101, native_102, native_103, native_104, native_105, native_106, native_107, native_108, native_109, native_110, native_111, native_112, native_113, native_114, native_115, native_116, native_117, native_118, native_119, native_120, native_121, native_122, native_123, native_124, native_125, native_126, native_127};
int main(void) {
    volatile unsigned canaries[] = {0x12345678u, 0xabcdef01u, 0xfedcba98u};
    unsigned state = 1, digest = 0;
    for (unsigned i = 0; i < 128; i++) {
        NativeFunction f = getters[i]();
        for (unsigned j = 0; j < 128; j++) {
            uintptr_t before, after;
            state = state * 1664525u + 1013904223u;
            unsigned a = j == 0 ? 0 : j == 1 ? ~0u
                                             : state;
            READ_SP(before);
            digest = digest * 1664525u + f(a, ~state);
            READ_SP(after);
            if (before != after || canaries[0] != 0x12345678u || canaries[1] != 0xabcdef01u || canaries[2] != 0xfedcba98u) return 1;
        }
        printf("%u %08x\n", i, digest);
    }
    if (puts("STACK_PROXY_POOL_PASS") == EOF) return 2;
    return fflush(stdout) == EOF ? 2 : 0;
}
