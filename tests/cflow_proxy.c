#include <stdio.h>
#include <windows.h>

#include "../include/obfus.h"
#undef if
#undef else
#undef for
#undef while
#undef break
#if defined(__x86_64__)
#define SP(out) __asm__ __volatile__("movq %%rsp, %0" \
                                     : "=r"(out)      \
                                     :                \
                                     : "memory")
#else
#define SP(out) __asm__ __volatile__("movl %%esp, %0" \
                                     : "=r"(out)      \
                                     :                \
                                     : "memory")
#endif
#define DEFINE_PROXY(index)                                                                            \
    static int proxy_##index(unsigned int value, unsigned int truth) {                                 \
        unsigned int __obfh_flow_hash = 0x6c89d137u;                                                   \
        unsigned int __obfh_flow_state = value, __obfh_flow_tag = truth ? value : value ^ 0x13579bdfu; \
        ULONG_PTR __obfh_cookie = (ULONG_PTR)&value ^ value;                                           \
        ULONG_PTR before, after;                                                                       \
        unsigned long long live = ((unsigned long long)value << 32) | ~value;                          \
        SP(before);                                                                                    \
        {                                                                                              \
            OBFH_SF_CAPTURE;                                                                           \
            OBFH_SF_SPEC_##index(OBFH_SF_FLOW_EMIT);                                                   \
        }                                                                                              \
        SP(after);                                                                                     \
        return before != after || (unsigned int)(__obfh_flow_state == __obfh_flow_tag) != truth ||     \
               live != (((unsigned long long)value << 32) | ~value);                                   \
    }

DEFINE_PROXY(0)
DEFINE_PROXY(1)
DEFINE_PROXY(2)
DEFINE_PROXY(3)
DEFINE_PROXY(4)
DEFINE_PROXY(5)
DEFINE_PROXY(6)
DEFINE_PROXY(7)
DEFINE_PROXY(8)
DEFINE_PROXY(9)
DEFINE_PROXY(10)
DEFINE_PROXY(11)
DEFINE_PROXY(12)
DEFINE_PROXY(13)
DEFINE_PROXY(14)
DEFINE_PROXY(15)
DEFINE_PROXY(16)
DEFINE_PROXY(17)
DEFINE_PROXY(18)
DEFINE_PROXY(19)
DEFINE_PROXY(20)
DEFINE_PROXY(21)
DEFINE_PROXY(22)
DEFINE_PROXY(23)
DEFINE_PROXY(24)
DEFINE_PROXY(25)
DEFINE_PROXY(26)
DEFINE_PROXY(27)
DEFINE_PROXY(28)
DEFINE_PROXY(29)
DEFINE_PROXY(30)
DEFINE_PROXY(31)
DEFINE_PROXY(32)
DEFINE_PROXY(33)
DEFINE_PROXY(34)
DEFINE_PROXY(35)
DEFINE_PROXY(36)
DEFINE_PROXY(37)
DEFINE_PROXY(38)
DEFINE_PROXY(39)
DEFINE_PROXY(40)
DEFINE_PROXY(41)
DEFINE_PROXY(42)
DEFINE_PROXY(43)
DEFINE_PROXY(44)
DEFINE_PROXY(45)
DEFINE_PROXY(46)
DEFINE_PROXY(47)
DEFINE_PROXY(48)
DEFINE_PROXY(49)
DEFINE_PROXY(50)
DEFINE_PROXY(51)
DEFINE_PROXY(52)
DEFINE_PROXY(53)
DEFINE_PROXY(54)
DEFINE_PROXY(55)
DEFINE_PROXY(56)
DEFINE_PROXY(57)
DEFINE_PROXY(58)
DEFINE_PROXY(59)
DEFINE_PROXY(60)
DEFINE_PROXY(61)
DEFINE_PROXY(62)
DEFINE_PROXY(63)
DEFINE_PROXY(64)
DEFINE_PROXY(65)
DEFINE_PROXY(66)
DEFINE_PROXY(67)
DEFINE_PROXY(68)
DEFINE_PROXY(69)
DEFINE_PROXY(70)
DEFINE_PROXY(71)
DEFINE_PROXY(72)
DEFINE_PROXY(73)
DEFINE_PROXY(74)
DEFINE_PROXY(75)
DEFINE_PROXY(76)
DEFINE_PROXY(77)
DEFINE_PROXY(78)
DEFINE_PROXY(79)
DEFINE_PROXY(80)
DEFINE_PROXY(81)
DEFINE_PROXY(82)
DEFINE_PROXY(83)
DEFINE_PROXY(84)
DEFINE_PROXY(85)
DEFINE_PROXY(86)
DEFINE_PROXY(87)
DEFINE_PROXY(88)
DEFINE_PROXY(89)
DEFINE_PROXY(90)
DEFINE_PROXY(91)
DEFINE_PROXY(92)
DEFINE_PROXY(93)
DEFINE_PROXY(94)
DEFINE_PROXY(95)
DEFINE_PROXY(96)
DEFINE_PROXY(97)
DEFINE_PROXY(98)
DEFINE_PROXY(99)
DEFINE_PROXY(100)
DEFINE_PROXY(101)
DEFINE_PROXY(102)
DEFINE_PROXY(103)
DEFINE_PROXY(104)
DEFINE_PROXY(105)
DEFINE_PROXY(106)
DEFINE_PROXY(107)
DEFINE_PROXY(108)
DEFINE_PROXY(109)
DEFINE_PROXY(110)
DEFINE_PROXY(111)
DEFINE_PROXY(112)
DEFINE_PROXY(113)
DEFINE_PROXY(114)
DEFINE_PROXY(115)
DEFINE_PROXY(116)
DEFINE_PROXY(117)
DEFINE_PROXY(118)
DEFINE_PROXY(119)
DEFINE_PROXY(120)
DEFINE_PROXY(121)
DEFINE_PROXY(122)
DEFINE_PROXY(123)
DEFINE_PROXY(124)
DEFINE_PROXY(125)
DEFINE_PROXY(126)
DEFINE_PROXY(127)
static int (*proxies[])(unsigned int, unsigned int) = {proxy_0, proxy_1, proxy_2, proxy_3, proxy_4, proxy_5, proxy_6, proxy_7, proxy_8, proxy_9, proxy_10, proxy_11, proxy_12, proxy_13, proxy_14, proxy_15, proxy_16, proxy_17, proxy_18, proxy_19, proxy_20, proxy_21, proxy_22, proxy_23, proxy_24, proxy_25, proxy_26, proxy_27, proxy_28, proxy_29, proxy_30, proxy_31, proxy_32, proxy_33, proxy_34, proxy_35, proxy_36, proxy_37, proxy_38, proxy_39, proxy_40, proxy_41, proxy_42, proxy_43, proxy_44, proxy_45, proxy_46, proxy_47, proxy_48, proxy_49, proxy_50, proxy_51, proxy_52, proxy_53, proxy_54, proxy_55, proxy_56, proxy_57, proxy_58, proxy_59, proxy_60, proxy_61, proxy_62, proxy_63, proxy_64, proxy_65, proxy_66, proxy_67, proxy_68, proxy_69, proxy_70, proxy_71, proxy_72, proxy_73, proxy_74, proxy_75, proxy_76, proxy_77, proxy_78, proxy_79, proxy_80, proxy_81, proxy_82, proxy_83, proxy_84, proxy_85, proxy_86, proxy_87, proxy_88, proxy_89, proxy_90, proxy_91, proxy_92, proxy_93, proxy_94, proxy_95, proxy_96, proxy_97, proxy_98, proxy_99, proxy_100, proxy_101, proxy_102, proxy_103, proxy_104, proxy_105, proxy_106, proxy_107, proxy_108, proxy_109, proxy_110, proxy_111, proxy_112, proxy_113, proxy_114, proxy_115, proxy_116, proxy_117, proxy_118, proxy_119, proxy_120, proxy_121, proxy_122, proxy_123, proxy_124, proxy_125, proxy_126, proxy_127};
static DWORD WINAPI worker(void *parameter) {
    unsigned int value = (unsigned int)(ULONG_PTR)parameter;
    static const unsigned int edges[] = {0u, 1u, 2u, 3u, 0x7fffffffu, 0x80000000u, 0xffffffffu};
    for (unsigned int n = 0; n < 160; ++n) {
        value = value * 1664525u + 1013904223u;
        unsigned int input = n < sizeof(edges) / sizeof(edges[0]) ? edges[n] : value;
        for (unsigned int i = 0; i < sizeof(proxies) / sizeof(proxies[0]); ++i)
            for (unsigned int truth = 0; truth < 2; ++truth)
                if (proxies[i](input, truth)) return 1;
    }
    return 0;
}
int main(void) {
    if (worker((void *)17)) return 1;
    HANDLE threads[4];
    for (unsigned int i = 0; i < 4; ++i) {
        threads[i] = CreateThread(NULL, 0, worker, (void *)(ULONG_PTR)(i + 1), 0, NULL);
        if (!threads[i]) return 2;
    }
    if (WaitForMultipleObjects(4, threads, TRUE, 30000) != WAIT_OBJECT_0) return 3;
    for (unsigned int i = 0; i < 4; ++i) {
        DWORD result;
        if (!GetExitCodeThread(threads[i], &result) || result) return 4;
        CloseHandle(threads[i]);
    }
    puts("FLOW_PROXY_PASS");
    return 0;
}
