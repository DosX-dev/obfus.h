#include <stdint.h>
#include <stdio.h>
#include <windows.h>

#include "../include/obfus.h"
#define SITE(n, macro)                               \
    __declspec(dllexport) int junk_site_##n(int x) { \
        macro;                                       \
        return x + n + 1;                            \
    }
SITE(0, OBFH_CFLOW_SELECT(0))
SITE(1, OBFH_CFLOW_SELECT(1))
SITE(2, OBFH_CFLOW_SELECT(2))
SITE(3, OBFH_CFLOW_SELECT(3))
SITE(4, OBFH_CFLOW_SELECT(4))
SITE(5, OBFH_CFLOW_SELECT(5))
SITE(6, OBFH_CFLOW_SELECT(6))
SITE(7, OBFH_CFLOW_SELECT(7))
SITE(8, OBFH_CFLOW_SELECT(8))
SITE(9, OBFH_CFLOW_SELECT(9))
SITE(10, OBFH_CFLOW_SELECT(10))
SITE(11, OBFH_CFLOW_SELECT(11))
SITE(12, OBFH_CFLOW_SELECT(12))
SITE(13, OBFH_CFLOW_SELECT(13))
SITE(14, OBFH_CFLOW_SELECT(14))
SITE(15, OBFH_CFLOW_SELECT(15))
SITE(16, OBFH_CFLOW_SELECT(16))
SITE(17, OBFH_CFLOW_SELECT(17))
SITE(18, OBFH_CFLOW_SELECT(18))
SITE(19, OBFH_CFLOW_SELECT(19))
SITE(20, OBFH_CFLOW_SELECT(20))
SITE(21, OBFH_CFLOW_SELECT(21))
SITE(22, OBFH_CFLOW_SELECT(22))
SITE(23, OBFH_CFLOW_SELECT(23))
SITE(24, OBFH_CFLOW_SELECT(24))
SITE(25, OBFH_CFLOW_SELECT(25))
SITE(26, OBFH_CFLOW_SELECT(26))
SITE(27, OBFH_CFLOW_SELECT(27))
SITE(28, OBFH_CFLOW_SELECT(28))
SITE(29, OBFH_CFLOW_SELECT(29))
SITE(30, OBFH_CFLOW_SELECT(30))
SITE(31, OBFH_CFLOW_SELECT(31))
SITE(32, OBFH_CFLOW_SELECT(32))
SITE(33, OBFH_CFLOW_SELECT(33))
SITE(34, OBFH_CFLOW_SELECT(34))
SITE(35, OBFH_CFLOW_SELECT(35))
SITE(36, OBFH_CFLOW_SELECT(36))
SITE(37, OBFH_CFLOW_SELECT(37))
SITE(38, OBFH_CFLOW_SELECT(38))
SITE(39, OBFH_CFLOW_SELECT(39))
SITE(40, OBFH_CFLOW_SELECT(40))
SITE(41, OBFH_CFLOW_SELECT(41))
SITE(42, OBFH_CFLOW_SELECT(42))
SITE(43, OBFH_CFLOW_SELECT(43))
SITE(44, OBFH_CFLOW_SELECT(44))
SITE(45, OBFH_CFLOW_SELECT(45))
SITE(46, OBFH_CFLOW_SELECT(46))
SITE(47, OBFH_CFLOW_SELECT(47))
SITE(48, OBFH_CFLOW_SELECT(48))
SITE(49, OBFH_CFLOW_SELECT(49))
SITE(50, OBFH_CFLOW_SELECT(50))
SITE(51, OBFH_CFLOW_SELECT(51))
SITE(52, OBFH_CFLOW_SELECT(52))
SITE(53, OBFH_CFLOW_SELECT(53))
SITE(54, OBFH_CFLOW_SELECT(54))
SITE(55, OBFH_CFLOW_SELECT(55))
SITE(56, OBFH_CFLOW_SELECT(56))
SITE(57, OBFH_CFLOW_SELECT(57))
SITE(58, OBFH_CFLOW_SELECT(58))
SITE(59, OBFH_CFLOW_SELECT(59))
SITE(60, OBFH_CFLOW_SELECT(60))
SITE(61, OBFH_CFLOW_SELECT(61))
SITE(62, OBFH_CFLOW_SELECT(62))
SITE(63, OBFH_CFLOW_SELECT(63))
SITE(64, OBFH_CFLOW_SELECT(64))
SITE(65, OBFH_CFLOW_SELECT(65))
SITE(66, OBFH_CFLOW_SELECT(66))
SITE(67, OBFH_CFLOW_SELECT(67))
SITE(68, OBFH_CFLOW_SELECT(68))
SITE(69, OBFH_CFLOW_SELECT(69))
SITE(70, OBFH_CFLOW_SELECT(70))
SITE(71, OBFH_CFLOW_SELECT(71))
SITE(72, OBFH_CFLOW_SELECT(72))
SITE(73, OBFH_CFLOW_SELECT(73))
SITE(74, OBFH_CFLOW_SELECT(74))
SITE(75, OBFH_CFLOW_SELECT(75))
SITE(76, OBFH_CFLOW_SELECT(76))
SITE(77, OBFH_CFLOW_SELECT(77))
SITE(78, OBFH_CFLOW_SELECT(78))
SITE(79, OBFH_CFLOW_SELECT(79))
SITE(80, OBFH_CFLOW_SELECT(80))
SITE(81, OBFH_CFLOW_SELECT(81))
SITE(82, OBFH_CFLOW_SELECT(82))
SITE(83, OBFH_CFLOW_SELECT(83))
SITE(84, OBFH_CFLOW_SELECT(84))
SITE(85, OBFH_CFLOW_SELECT(85))
SITE(86, OBFH_CFLOW_SELECT(86))
SITE(87, OBFH_CFLOW_SELECT(87))
SITE(88, OBFH_CFLOW_SELECT(88))
SITE(89, OBFH_CFLOW_SELECT(89))
SITE(90, OBFH_CFLOW_SELECT(90))
SITE(91, OBFH_CFLOW_SELECT(91))
SITE(92, OBFH_CFLOW_SELECT(92))
SITE(93, OBFH_CFLOW_SELECT(93))
SITE(94, OBFH_CFLOW_SELECT(94))
SITE(95, OBFH_CFLOW_SELECT(95))
SITE(96, OBFH_CFLOW_SELECT(96))
SITE(97, OBFH_CFLOW_SELECT(97))
SITE(98, OBFH_CFLOW_SELECT(98))
SITE(99, OBFH_CFLOW_SELECT(99))
SITE(100, OBFH_CFLOW_SELECT(100))
SITE(101, OBFH_CFLOW_SELECT(101))
SITE(102, OBFH_CFLOW_SELECT(102))
SITE(103, OBFH_CFLOW_SELECT(103))
SITE(104, OBFH_CFLOW_SELECT(104))
SITE(105, OBFH_CFLOW_SELECT(105))
SITE(106, OBFH_CFLOW_SELECT(106))
SITE(107, OBFH_CFLOW_SELECT(107))
SITE(108, OBFH_CFLOW_SELECT(108))
SITE(109, OBFH_CFLOW_SELECT(109))
SITE(110, OBFH_CFLOW_SELECT(110))
SITE(111, OBFH_CFLOW_SELECT(111))
SITE(112, OBFH_CFLOW_SELECT(112))
SITE(113, OBFH_CFLOW_SELECT(113))
SITE(114, OBFH_CFLOW_SELECT(114))
SITE(115, OBFH_CFLOW_SELECT(115))
SITE(116, OBFH_CFLOW_SELECT(116))
SITE(117, OBFH_CFLOW_SELECT(117))
SITE(118, OBFH_CFLOW_SELECT(118))
SITE(119, OBFH_CFLOW_SELECT(119))
SITE(120, OBFH_CFLOW_SELECT(120))
SITE(121, OBFH_CFLOW_SELECT(121))
SITE(122, OBFH_CFLOW_SELECT(122))
SITE(123, OBFH_CFLOW_SELECT(123))
SITE(124, OBFH_CFLOW_SELECT(124))
SITE(125, OBFH_CFLOW_SELECT(125))
SITE(126, OBFH_CFLOW_SELECT(126))
SITE(127, OBFH_CFLOW_SELECT(127))
__declspec(dllexport) int junk_anchor(int x) { return x; }
static int (*sites[])(int) = {junk_site_0, junk_site_1, junk_site_2, junk_site_3, junk_site_4, junk_site_5, junk_site_6, junk_site_7, junk_site_8, junk_site_9, junk_site_10, junk_site_11, junk_site_12, junk_site_13, junk_site_14, junk_site_15, junk_site_16, junk_site_17, junk_site_18, junk_site_19, junk_site_20, junk_site_21, junk_site_22, junk_site_23, junk_site_24, junk_site_25, junk_site_26, junk_site_27, junk_site_28, junk_site_29, junk_site_30, junk_site_31, junk_site_32, junk_site_33, junk_site_34, junk_site_35, junk_site_36, junk_site_37, junk_site_38, junk_site_39, junk_site_40, junk_site_41, junk_site_42, junk_site_43, junk_site_44, junk_site_45, junk_site_46, junk_site_47, junk_site_48, junk_site_49, junk_site_50, junk_site_51, junk_site_52, junk_site_53, junk_site_54, junk_site_55, junk_site_56, junk_site_57, junk_site_58, junk_site_59, junk_site_60, junk_site_61, junk_site_62, junk_site_63, junk_site_64, junk_site_65, junk_site_66, junk_site_67, junk_site_68, junk_site_69, junk_site_70, junk_site_71, junk_site_72, junk_site_73, junk_site_74, junk_site_75, junk_site_76, junk_site_77, junk_site_78, junk_site_79, junk_site_80, junk_site_81, junk_site_82, junk_site_83, junk_site_84, junk_site_85, junk_site_86, junk_site_87, junk_site_88, junk_site_89, junk_site_90, junk_site_91, junk_site_92, junk_site_93, junk_site_94, junk_site_95, junk_site_96, junk_site_97, junk_site_98, junk_site_99, junk_site_100, junk_site_101, junk_site_102, junk_site_103, junk_site_104, junk_site_105, junk_site_106, junk_site_107, junk_site_108, junk_site_109, junk_site_110, junk_site_111, junk_site_112, junk_site_113, junk_site_114, junk_site_115, junk_site_116, junk_site_117, junk_site_118, junk_site_119, junk_site_120, junk_site_121, junk_site_122, junk_site_123, junk_site_124, junk_site_125, junk_site_126, junk_site_127};
static DWORD WINAPI stress(void *unused) {
    unsigned int state = 0x12345678u;
    for (int i = 0; i < 1000; ++i) {
        state = state * 1664525u + 1013904223u;
        int x = (int)(state % 1000000u);
        volatile float f = x / 8.0f;
        volatile double d = x / 4.0;
        uint64_t wide = (uint64_t)state << 32 | (state ^ 0xa5a5a5a5u);
        int memory[4] = {x, ~x, i, 17};
        void *pointer = &memory[2];
        for (int j = 0; j < sizeof(sites) / sizeof(sites[0]); ++j) {
            if (sites[j](x) != x + j + 1 || f != x / 8.0f || d != x / 4.0 ||
                wide != (((uint64_t)state << 32) | (state ^ 0xa5a5a5a5u)) || pointer != &memory[2] || memory[0] != x || memory[1] != ~x || memory[2] != i || memory[3] != 17) {
                fprintf(stderr, "junk failure: site=%d iteration=%d\n", j, i);
                return 1;
            }
        }
        if (VM_ADD(x, 7) != x + 7 || VM_SUB(x, 7) != x - 7 || VM_MUL(x, 3) != x * 3) return 2;
        int calls = 0;
        VM_IF((++calls, x & 1)) {
            if (!(x & 1)) return 3;
        }
        VM_ELSE {
            if (x & 1) return 4;
        }
        if (calls != 1) return 5;
    }
    return 0;
}
int main(void) {
    if (stress(NULL)) return 1;
    HANDLE threads[4];
    for (int i = 0; i < 4; ++i) {
        threads[i] = CreateThread(NULL, 0, stress, NULL, 0, NULL);
        if (!threads[i]) return 6;
    }
    if (WaitForMultipleObjects(4, threads, TRUE, 10000) != WAIT_OBJECT_0) return 7;
    for (int i = 0; i < 4; ++i) {
        DWORD result;
        if (!GetExitCodeThread(threads[i], &result) || result) return 8;
        if (!CloseHandle(threads[i])) return 9;
    }
    puts("CFLOW_JUNK_PASS");
    return 0;
}
