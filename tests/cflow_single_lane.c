#include <stdio.h>
#define __obfh_asm__(...) __asm__ __volatile__(__VA_ARGS__)
#include "single-lane.h"
static unsigned probe_0(unsigned input, unsigned *target) {
    enum { __obfh_point0 = 0u,
           __obfh_k00 = 2005604357u,
           __obfh_k01 = 2202282422u,
           __obfh_a00 = 579804268u,
           __obfh_a01 = 3879332085u,
           __obfh_r00 = 2u,
           __obfh_r01 = 13u };
    OBFH_P_PREPARE(0, 0)
    OBFH_P_PREPARE(0, 1) unsigned __obfh_flow_state = input;
    OBFH_P_STEP(0, 0);
    *target = __obfh_point1;
    return __obfh_flow_state;
}
static unsigned probe_1(unsigned input, unsigned *target) {
    enum { __obfh_point0 = 4294967295u,
           __obfh_k00 = 3525347642u,
           __obfh_k01 = 4184041675u,
           __obfh_a00 = 2090436262u,
           __obfh_a01 = 1377771315u,
           __obfh_r00 = 11u,
           __obfh_r01 = 13u };
    OBFH_P_PREPARE(0, 0)
    OBFH_P_PREPARE(0, 1) unsigned __obfh_flow_state = input;
    OBFH_P_STEP(0, 0);
    *target = __obfh_point1;
    return __obfh_flow_state;
}
static unsigned probe_2(unsigned input, unsigned *target) {
    enum { __obfh_point0 = 2147483648u,
           __obfh_k00 = 4141658641u,
           __obfh_k01 = 1060859567u,
           __obfh_a00 = 3208292583u,
           __obfh_a01 = 665115765u,
           __obfh_r00 = 5u,
           __obfh_r01 = 13u };
    OBFH_P_PREPARE(0, 0)
    OBFH_P_PREPARE(0, 1) unsigned __obfh_flow_state = input;
    OBFH_P_STEP(0, 0);
    *target = __obfh_point1;
    return __obfh_flow_state;
}
static unsigned probe_3(unsigned input, unsigned *target) {
    enum { __obfh_point0 = 249145528u,
           __obfh_k00 = 2299485176u,
           __obfh_k01 = 245994736u,
           __obfh_a00 = 3759439342u,
           __obfh_a01 = 2060456055u,
           __obfh_r00 = 3u,
           __obfh_r01 = 13u };
    OBFH_P_PREPARE(0, 0)
    OBFH_P_PREPARE(0, 1) unsigned __obfh_flow_state = input;
    OBFH_P_STEP(0, 0);
    *target = __obfh_point1;
    return __obfh_flow_state;
}
static unsigned probe_4(unsigned input, unsigned *target) {
    enum { __obfh_point0 = 0u,
           __obfh_k00 = 3657020524u,
           __obfh_k01 = 395389609u,
           __obfh_a00 = 1744736512u,
           __obfh_a01 = 579908965u,
           __obfh_r00 = 20u,
           __obfh_r01 = 13u };
    OBFH_P_PREPARE(0, 0)
    OBFH_P_PREPARE(0, 1) unsigned __obfh_flow_state = input;
    OBFH_P_STEP(0, 0);
    *target = __obfh_point1;
    return __obfh_flow_state;
}
static unsigned probe_5(unsigned input, unsigned *target) {
    enum { __obfh_point0 = 4294967295u,
           __obfh_k00 = 329405719u,
           __obfh_k01 = 4187340137u,
           __obfh_a00 = 2956674308u,
           __obfh_a01 = 1185421746u,
           __obfh_r00 = 29u,
           __obfh_r01 = 13u };
    OBFH_P_PREPARE(0, 0)
    OBFH_P_PREPARE(0, 1) unsigned __obfh_flow_state = input;
    OBFH_P_STEP(0, 0);
    *target = __obfh_point1;
    return __obfh_flow_state;
}
static unsigned probe_6(unsigned input, unsigned *target) {
    enum { __obfh_point0 = 2147483648u,
           __obfh_k00 = 4168638176u,
           __obfh_k01 = 3390361393u,
           __obfh_a00 = 1166259910u,
           __obfh_a01 = 2142745658u,
           __obfh_r00 = 20u,
           __obfh_r01 = 13u };
    OBFH_P_PREPARE(0, 0)
    OBFH_P_PREPARE(0, 1) unsigned __obfh_flow_state = input;
    OBFH_P_STEP(0, 0);
    *target = __obfh_point1;
    return __obfh_flow_state;
}
static unsigned probe_7(unsigned input, unsigned *target) {
    enum { __obfh_point0 = 836707006u,
           __obfh_k00 = 3098077131u,
           __obfh_k01 = 4025862562u,
           __obfh_a00 = 447038502u,
           __obfh_a01 = 1263372091u,
           __obfh_r00 = 13u,
           __obfh_r01 = 13u };
    OBFH_P_PREPARE(0, 0)
    OBFH_P_PREPARE(0, 1) unsigned __obfh_flow_state = input;
    OBFH_P_STEP(0, 0);
    *target = __obfh_point1;
    return __obfh_flow_state;
}
static unsigned probe_8(unsigned input, unsigned *target) {
    enum { __obfh_point0 = 0u,
           __obfh_k00 = 1219702812u,
           __obfh_k01 = 260901760u,
           __obfh_a00 = 3167009257u,
           __obfh_a01 = 1690155407u,
           __obfh_r00 = 28u,
           __obfh_r01 = 13u };
    OBFH_P_PREPARE(0, 0)
    OBFH_P_PREPARE(0, 1) unsigned __obfh_flow_state = input;
    OBFH_P_STEP(0, 0);
    *target = __obfh_point1;
    return __obfh_flow_state;
}
static unsigned probe_9(unsigned input, unsigned *target) {
    enum { __obfh_point0 = 4294967295u,
           __obfh_k00 = 2268004784u,
           __obfh_k01 = 2031578450u,
           __obfh_a00 = 615173369u,
           __obfh_a01 = 284135095u,
           __obfh_r00 = 24u,
           __obfh_r01 = 13u };
    OBFH_P_PREPARE(0, 0)
    OBFH_P_PREPARE(0, 1) unsigned __obfh_flow_state = input;
    OBFH_P_STEP(0, 0);
    *target = __obfh_point1;
    return __obfh_flow_state;
}
static unsigned probe_10(unsigned input, unsigned *target) {
    enum { __obfh_point0 = 2147483648u,
           __obfh_k00 = 2690319061u,
           __obfh_k01 = 3462000667u,
           __obfh_a00 = 479732850u,
           __obfh_a01 = 3968037787u,
           __obfh_r00 = 8u,
           __obfh_r01 = 13u };
    OBFH_P_PREPARE(0, 0)
    OBFH_P_PREPARE(0, 1) unsigned __obfh_flow_state = input;
    OBFH_P_STEP(0, 0);
    *target = __obfh_point1;
    return __obfh_flow_state;
}
static unsigned probe_11(unsigned input, unsigned *target) {
    enum { __obfh_point0 = 2564280131u,
           __obfh_k00 = 581692326u,
           __obfh_k01 = 456472459u,
           __obfh_a00 = 182837969u,
           __obfh_a01 = 1013666369u,
           __obfh_r00 = 19u,
           __obfh_r01 = 13u };
    OBFH_P_PREPARE(0, 0)
    OBFH_P_PREPARE(0, 1) unsigned __obfh_flow_state = input;
    OBFH_P_STEP(0, 0);
    *target = __obfh_point1;
    return __obfh_flow_state;
}
static unsigned probe_12(unsigned input, unsigned *target) {
    enum { __obfh_point0 = 0u,
           __obfh_k00 = 708541675u,
           __obfh_k01 = 1936259167u,
           __obfh_a00 = 349456312u,
           __obfh_a01 = 365315957u,
           __obfh_r00 = 23u,
           __obfh_r01 = 13u };
    OBFH_P_PREPARE(0, 0)
    OBFH_P_PREPARE(0, 1) unsigned __obfh_flow_state = input;
    OBFH_P_STEP(0, 0);
    *target = __obfh_point1;
    return __obfh_flow_state;
}
static unsigned probe_13(unsigned input, unsigned *target) {
    enum { __obfh_point0 = 4294967295u,
           __obfh_k00 = 1542958556u,
           __obfh_k01 = 1932662827u,
           __obfh_a00 = 1795462821u,
           __obfh_a01 = 2691520548u,
           __obfh_r00 = 18u,
           __obfh_r01 = 13u };
    OBFH_P_PREPARE(0, 0)
    OBFH_P_PREPARE(0, 1) unsigned __obfh_flow_state = input;
    OBFH_P_STEP(0, 0);
    *target = __obfh_point1;
    return __obfh_flow_state;
}
static unsigned probe_14(unsigned input, unsigned *target) {
    enum { __obfh_point0 = 2147483648u,
           __obfh_k00 = 2307057290u,
           __obfh_k01 = 1635860468u,
           __obfh_a00 = 3061500773u,
           __obfh_a01 = 894783985u,
           __obfh_r00 = 31u,
           __obfh_r01 = 13u };
    OBFH_P_PREPARE(0, 0)
    OBFH_P_PREPARE(0, 1) unsigned __obfh_flow_state = input;
    OBFH_P_STEP(0, 0);
    *target = __obfh_point1;
    return __obfh_flow_state;
}
static unsigned probe_15(unsigned input, unsigned *target) {
    enum { __obfh_point0 = 1014814354u,
           __obfh_k00 = 961171307u,
           __obfh_k01 = 4084482740u,
           __obfh_a00 = 2293634157u,
           __obfh_a01 = 3526357352u,
           __obfh_r00 = 28u,
           __obfh_r01 = 13u };
    OBFH_P_PREPARE(0, 0)
    OBFH_P_PREPARE(0, 1) unsigned __obfh_flow_state = input;
    OBFH_P_STEP(0, 0);
    *target = __obfh_point1;
    return __obfh_flow_state;
}
static unsigned probe_16(unsigned input, unsigned *target) {
    enum { __obfh_point0 = 0u,
           __obfh_k00 = 3006767218u,
           __obfh_k01 = 3082220027u,
           __obfh_a00 = 308654393u,
           __obfh_a01 = 1464596596u,
           __obfh_r00 = 24u,
           __obfh_r01 = 13u };
    OBFH_P_PREPARE(0, 0)
    OBFH_P_PREPARE(0, 1) unsigned __obfh_flow_state = input;
    OBFH_P_STEP(0, 0);
    *target = __obfh_point1;
    return __obfh_flow_state;
}
static unsigned probe_17(unsigned input, unsigned *target) {
    enum { __obfh_point0 = 4294967295u,
           __obfh_k00 = 3605586256u,
           __obfh_k01 = 3011123579u,
           __obfh_a00 = 2322828536u,
           __obfh_a01 = 2766264671u,
           __obfh_r00 = 17u,
           __obfh_r01 = 13u };
    OBFH_P_PREPARE(0, 0)
    OBFH_P_PREPARE(0, 1) unsigned __obfh_flow_state = input;
    OBFH_P_STEP(0, 0);
    *target = __obfh_point1;
    return __obfh_flow_state;
}
static unsigned probe_18(unsigned input, unsigned *target) {
    enum { __obfh_point0 = 2147483648u,
           __obfh_k00 = 3371444880u,
           __obfh_k01 = 4100765793u,
           __obfh_a00 = 744483585u,
           __obfh_a01 = 1242344534u,
           __obfh_r00 = 22u,
           __obfh_r01 = 13u };
    OBFH_P_PREPARE(0, 0)
    OBFH_P_PREPARE(0, 1) unsigned __obfh_flow_state = input;
    OBFH_P_STEP(0, 0);
    *target = __obfh_point1;
    return __obfh_flow_state;
}
static unsigned probe_19(unsigned input, unsigned *target) {
    enum { __obfh_point0 = 3364281477u,
           __obfh_k00 = 4188468196u,
           __obfh_k01 = 146316746u,
           __obfh_a00 = 3348415573u,
           __obfh_a01 = 3804426586u,
           __obfh_r00 = 12u,
           __obfh_r01 = 13u };
    OBFH_P_PREPARE(0, 0)
    OBFH_P_PREPARE(0, 1) unsigned __obfh_flow_state = input;
    OBFH_P_STEP(0, 0);
    *target = __obfh_point1;
    return __obfh_flow_state;
}
static unsigned probe_20(unsigned input, unsigned *target) {
    enum { __obfh_point0 = 0u,
           __obfh_k00 = 176288811u,
           __obfh_k01 = 4276231746u,
           __obfh_a00 = 3308988241u,
           __obfh_a01 = 1261911020u,
           __obfh_r00 = 7u,
           __obfh_r01 = 13u };
    OBFH_P_PREPARE(0, 0)
    OBFH_P_PREPARE(0, 1) unsigned __obfh_flow_state = input;
    OBFH_P_STEP(0, 0);
    *target = __obfh_point1;
    return __obfh_flow_state;
}
static unsigned probe_21(unsigned input, unsigned *target) {
    enum { __obfh_point0 = 4294967295u,
           __obfh_k00 = 2100417862u,
           __obfh_k01 = 2281428906u,
           __obfh_a00 = 2252450623u,
           __obfh_a01 = 3910497898u,
           __obfh_r00 = 15u,
           __obfh_r01 = 13u };
    OBFH_P_PREPARE(0, 0)
    OBFH_P_PREPARE(0, 1) unsigned __obfh_flow_state = input;
    OBFH_P_STEP(0, 0);
    *target = __obfh_point1;
    return __obfh_flow_state;
}
static unsigned probe_22(unsigned input, unsigned *target) {
    enum { __obfh_point0 = 2147483648u,
           __obfh_k00 = 1991546613u,
           __obfh_k01 = 4077981386u,
           __obfh_a00 = 1768242359u,
           __obfh_a01 = 2583576759u,
           __obfh_r00 = 11u,
           __obfh_r01 = 13u };
    OBFH_P_PREPARE(0, 0)
    OBFH_P_PREPARE(0, 1) unsigned __obfh_flow_state = input;
    OBFH_P_STEP(0, 0);
    *target = __obfh_point1;
    return __obfh_flow_state;
}
static unsigned probe_23(unsigned input, unsigned *target) {
    enum { __obfh_point0 = 3565982804u,
           __obfh_k00 = 1639348000u,
           __obfh_k01 = 781101359u,
           __obfh_a00 = 3382771659u,
           __obfh_a01 = 2509036059u,
           __obfh_r00 = 18u,
           __obfh_r01 = 13u };
    OBFH_P_PREPARE(0, 0)
    OBFH_P_PREPARE(0, 1) unsigned __obfh_flow_state = input;
    OBFH_P_STEP(0, 0);
    *target = __obfh_point1;
    return __obfh_flow_state;
}
int main(void) {
    {
        unsigned target;
        unsigned out = probe_0(0u, &target);
        if (out != (unsigned)(858282924u + target - 858282924u)) return 1;
    }
    {
        unsigned target;
        unsigned out = probe_0(4294967295u, &target);
        if (out != (unsigned)(858282929u + target - 858282924u)) return 1;
    }
    {
        unsigned target;
        unsigned out = probe_0(0u, &target);
        if (out != (unsigned)(858282924u + target - 858282924u)) return 1;
    }
    {
        unsigned target;
        unsigned out = probe_0(1172168220u, &target);
        if (out != (unsigned)(3587376416u + target - 858282924u)) return 1;
    }
    {
        unsigned target;
        unsigned out = probe_1(0u, &target);
        if (out != (unsigned)(2281606904u + target - 986507453u)) return 2;
    }
    {
        unsigned target;
        unsigned out = probe_1(4294967295u, &target);
        if (out != (unsigned)(986507453u + target - 986507453u)) return 2;
    }
    {
        unsigned target;
        unsigned out = probe_1(4294967295u, &target);
        if (out != (unsigned)(986507453u + target - 986507453u)) return 2;
    }
    {
        unsigned target;
        unsigned out = probe_1(2268975613u, &target);
        if (out != (unsigned)(2724037575u + target - 986507453u)) return 2;
    }
    {
        unsigned target;
        unsigned out = probe_2(0u, &target);
        if (out != (unsigned)(4141212253u + target - 4141212237u)) return 3;
    }
    {
        unsigned target;
        unsigned out = probe_2(4294967295u, &target);
        if (out != (unsigned)(153754658u + target - 4141212237u)) return 3;
    }
    {
        unsigned target;
        unsigned out = probe_2(2147483648u, &target);
        if (out != (unsigned)(4141212237u + target - 4141212237u)) return 3;
    }
    {
        unsigned target;
        unsigned out = probe_2(2356965140u, &target);
        if (out != (unsigned)(464554955u + target - 4141212237u)) return 3;
    }
    {
        unsigned target;
        unsigned out = probe_3(0u, &target);
        if (out != (unsigned)(4051025362u + target - 1808715610u)) return 4;
    }
    {
        unsigned target;
        unsigned out = probe_3(4294967295u, &target);
        if (out != (unsigned)(4051025371u + target - 1808715610u)) return 4;
    }
    {
        unsigned target;
        unsigned out = probe_3(249145528u, &target);
        if (out != (unsigned)(1808715610u + target - 1808715610u)) return 4;
    }
    {
        unsigned target;
        unsigned out = probe_3(3028795107u, &target);
        if (out != (unsigned)(2561673175u + target - 1808715610u)) return 4;
    }
    {
        unsigned target;
        unsigned out = probe_4(0u, &target);
        if (out != (unsigned)(1001519968u + target - 1001519968u)) return 5;
    }
    {
        unsigned target;
        unsigned out = probe_4(4294967295u, &target);
        if (out != (unsigned)(1001540448u + target - 1001519968u)) return 5;
    }
    {
        unsigned target;
        unsigned out = probe_4(0u, &target);
        if (out != (unsigned)(1001519968u + target - 1001519968u)) return 5;
    }
    {
        unsigned target;
        unsigned out = probe_4(655141877u, &target);
        if (out != (unsigned)(3413053483u + target - 1001519968u)) return 5;
    }
    {
        unsigned target;
        unsigned out = probe_5(0u, &target);
        if (out != (unsigned)(490050819u + target - 490050828u)) return 6;
    }
    {
        unsigned target;
        unsigned out = probe_5(4294967295u, &target);
        if (out != (unsigned)(490050828u + target - 490050828u)) return 6;
    }
    {
        unsigned target;
        unsigned out = probe_5(4294967295u, &target);
        if (out != (unsigned)(490050828u + target - 490050828u)) return 6;
    }
    {
        unsigned target;
        unsigned out = probe_5(2253245108u, &target);
        if (out != (unsigned)(1685681327u + target - 490050828u)) return 6;
    }
    {
        unsigned target;
        unsigned out = probe_6(0u, &target);
        if (out != (unsigned)(1545247268u + target - 1546820132u)) return 7;
    }
    {
        unsigned target;
        unsigned out = probe_6(4294967295u, &target);
        if (out != (unsigned)(1545247265u + target - 1546820132u)) return 7;
    }
    {
        unsigned target;
        unsigned out = probe_6(2147483648u, &target);
        if (out != (unsigned)(1546820132u + target - 1546820132u)) return 7;
    }
    {
        unsigned target;
        unsigned out = probe_6(2514420416u, &target);
        if (out != (unsigned)(3761681396u + target - 1546820132u)) return 7;
    }
    {
        unsigned target;
        unsigned out = probe_7(0u, &target);
        if (out != (unsigned)(1841269634u + target - 1960149909u)) return 8;
    }
    {
        unsigned target;
        unsigned out = probe_7(4294967295u, &target);
        if (out != (unsigned)(1841269625u + target - 1960149909u)) return 8;
    }
    {
        unsigned target;
        unsigned out = probe_7(836707006u, &target);
        if (out != (unsigned)(1960149909u + target - 1960149909u)) return 8;
    }
    {
        unsigned target;
        unsigned out = probe_7(2520545260u, &target);
        if (out != (unsigned)(1957278849u + target - 1960149909u)) return 8;
    }
    {
        unsigned target;
        unsigned out = probe_8(0u, &target);
        if (out != (unsigned)(4203222529u + target - 4203222529u)) return 9;
    }
    {
        unsigned target;
        unsigned out = probe_8(4294967295u, &target);
        if (out != (unsigned)(2347660852u + target - 4203222529u)) return 9;
    }
    {
        unsigned target;
        unsigned out = probe_8(0u, &target);
        if (out != (unsigned)(4203222529u + target - 4203222529u)) return 9;
    }
    {
        unsigned target;
        unsigned out = probe_8(1280241465u, &target);
        if (out != (unsigned)(1556475138u + target - 4203222529u)) return 9;
    }
    {
        unsigned target;
        unsigned out = probe_9(0u, &target);
        if (out != (unsigned)(2743455048u + target - 2743454899u)) return 10;
    }
    {
        unsigned target;
        unsigned out = probe_9(4294967295u, &target);
        if (out != (unsigned)(2743454899u + target - 2743454899u)) return 10;
    }
    {
        unsigned target;
        unsigned out = probe_9(4294967295u, &target);
        if (out != (unsigned)(2743454899u + target - 2743454899u)) return 10;
    }
    {
        unsigned target;
        unsigned out = probe_9(2299172151u, &target);
        if (out != (unsigned)(3563360187u + target - 2743454899u)) return 10;
    }
    {
        unsigned target;
        unsigned out = probe_10(0u, &target);
        if (out != (unsigned)(920221141u + target - 3067704789u)) return 11;
    }
    {
        unsigned target;
        unsigned out = probe_10(4294967295u, &target);
        if (out != (unsigned)(920221144u + target - 3067704789u)) return 11;
    }
    {
        unsigned target;
        unsigned out = probe_10(2147483648u, &target);
        if (out != (unsigned)(3067704789u + target - 3067704789u)) return 11;
    }
    {
        unsigned target;
        unsigned out = probe_10(3285603088u, &target);
        if (out != (unsigned)(3948313765u + target - 3067704789u)) return 11;
    }
    {
        unsigned target;
        unsigned out = probe_11(0u, &target);
        if (out != (unsigned)(2227234048u + target - 623549349u)) return 12;
    }
    {
        unsigned target;
        unsigned out = probe_11(4294967295u, &target);
        if (out != (unsigned)(2227234057u + target - 623549349u)) return 12;
    }
    {
        unsigned target;
        unsigned out = probe_11(2564280131u, &target);
        if (out != (unsigned)(623549349u + target - 623549349u)) return 12;
    }
    {
        unsigned target;
        unsigned out = probe_11(1889644860u, &target);
        if (out != (unsigned)(2400299492u + target - 623549349u)) return 12;
    }
    {
        unsigned target;
        unsigned out = probe_12(0u, &target);
        if (out != (unsigned)(1057997989u + target - 1057997989u)) return 13;
    }
    {
        unsigned target;
        unsigned out = probe_12(4294967295u, &target);
        if (out != (unsigned)(3935881934u + target - 1057997989u)) return 13;
    }
    {
        unsigned target;
        unsigned out = probe_12(0u, &target);
        if (out != (unsigned)(1057997989u + target - 1057997989u)) return 13;
    }
    {
        unsigned target;
        unsigned out = probe_12(3904453113u, &target);
        if (out != (unsigned)(3612773580u + target - 1057997989u)) return 13;
    }
    {
        unsigned target;
        unsigned out = probe_13(0u, &target);
        if (out != (unsigned)(3473720456u + target - 3473720451u)) return 14;
    }
    {
        unsigned target;
        unsigned out = probe_13(4294967295u, &target);
        if (out != (unsigned)(3473720451u + target - 3473720451u)) return 14;
    }
    {
        unsigned target;
        unsigned out = probe_13(4294967295u, &target);
        if (out != (unsigned)(3473720451u + target - 3473720451u)) return 14;
    }
    {
        unsigned target;
        unsigned out = probe_13(2727126301u, &target);
        if (out != (unsigned)(474844725u + target - 3473720451u)) return 14;
    }
    {
        unsigned target;
        unsigned out = probe_14(0u, &target);
        if (out != (unsigned)(3221376528u + target - 1073892880u)) return 15;
    }
    {
        unsigned target;
        unsigned out = probe_14(4294967295u, &target);
        if (out != (unsigned)(3221376527u + target - 1073892880u)) return 15;
    }
    {
        unsigned target;
        unsigned out = probe_14(2147483648u, &target);
        if (out != (unsigned)(1073892880u + target - 1073892880u)) return 15;
    }
    {
        unsigned target;
        unsigned out = probe_14(2359162563u, &target);
        if (out != (unsigned)(1285571795u + target - 1073892880u)) return 15;
    }
    {
        unsigned target;
        unsigned out = probe_15(0u, &target);
        if (out != (unsigned)(1308861689u + target - 1905522131u)) return 16;
    }
    {
        unsigned target;
        unsigned out = probe_15(4294967295u, &target);
        if (out != (unsigned)(1308861692u + target - 1905522131u)) return 16;
    }
    {
        unsigned target;
        unsigned out = probe_15(1014814354u, &target);
        if (out != (unsigned)(1905522131u + target - 1905522131u)) return 16;
    }
    {
        unsigned target;
        unsigned out = probe_15(4248236202u, &target);
        if (out != (unsigned)(3162339115u + target - 1905522131u)) return 16;
    }
    {
        unsigned target;
        unsigned out = probe_16(0u, &target);
        if (out != (unsigned)(3315421613u + target - 3315421613u)) return 17;
    }
    {
        unsigned target;
        unsigned out = probe_16(4294967295u, &target);
        if (out != (unsigned)(3315421612u + target - 3315421613u)) return 17;
    }
    {
        unsigned target;
        unsigned out = probe_16(0u, &target);
        if (out != (unsigned)(3315421613u + target - 3315421613u)) return 17;
    }
    {
        unsigned target;
        unsigned out = probe_16(3039620518u, &target);
        if (out != (unsigned)(2060074835u + target - 3315421613u)) return 17;
    }
    {
        unsigned target;
        unsigned out = probe_17(0u, &target);
        if (out != (unsigned)(4040947256u + target - 3169963113u)) return 18;
    }
    {
        unsigned target;
        unsigned out = probe_17(4294967295u, &target);
        if (out != (unsigned)(3169963113u + target - 3169963113u)) return 18;
    }
    {
        unsigned target;
        unsigned out = probe_17(4294967295u, &target);
        if (out != (unsigned)(3169963113u + target - 3169963113u)) return 18;
    }
    {
        unsigned target;
        unsigned out = probe_17(3486460362u, &target);
        if (out != (unsigned)(3124276110u + target - 3169963113u)) return 18;
    }
    {
        unsigned target;
        unsigned out = probe_18(0u, &target);
        if (out != (unsigned)(2006450890u + target - 2025325258u)) return 19;
    }
    {
        unsigned target;
        unsigned out = probe_18(4294967295u, &target);
        if (out != (unsigned)(1968702154u + target - 2025325258u)) return 19;
    }
    {
        unsigned target;
        unsigned out = probe_18(2147483648u, &target);
        if (out != (unsigned)(2025325258u + target - 2025325258u)) return 19;
    }
    {
        unsigned target;
        unsigned out = probe_18(994989302u, &target);
        if (out != (unsigned)(2711450375u + target - 2025325258u)) return 19;
    }
    {
        unsigned target;
        unsigned out = probe_19(0u, &target);
        if (out != (unsigned)(922424953u + target - 559375574u)) return 20;
    }
    {
        unsigned target;
        unsigned out = probe_19(4294967295u, &target);
        if (out != (unsigned)(922424948u + target - 559375574u)) return 20;
    }
    {
        unsigned target;
        unsigned out = probe_19(3364281477u, &target);
        if (out != (unsigned)(559375574u + target - 559375574u)) return 20;
    }
    {
        unsigned target;
        unsigned out = probe_19(2423032847u, &target);
        if (out != (unsigned)(147454500u + target - 559375574u)) return 20;
    }
    {
        unsigned target;
        unsigned out = probe_20(0u, &target);
        if (out != (unsigned)(170213227u + target - 170213227u)) return 21;
    }
    {
        unsigned target;
        unsigned out = probe_20(4294967295u, &target);
        if (out != (unsigned)(887519636u + target - 170213227u)) return 21;
    }
    {
        unsigned target;
        unsigned out = probe_20(0u, &target);
        if (out != (unsigned)(170213227u + target - 170213227u)) return 21;
    }
    {
        unsigned target;
        unsigned out = probe_20(3309686308u, &target);
        if (out != (unsigned)(3716732407u + target - 170213227u)) return 21;
    }
    {
        unsigned target;
        unsigned out = probe_21(0u, &target);
        if (out != (unsigned)(2020129301u + target - 2018949653u)) return 22;
    }
    {
        unsigned target;
        unsigned out = probe_21(4294967295u, &target);
        if (out != (unsigned)(2018949653u + target - 2018949653u)) return 22;
    }
    {
        unsigned target;
        unsigned out = probe_21(4294967295u, &target);
        if (out != (unsigned)(2018949653u + target - 2018949653u)) return 22;
    }
    {
        unsigned target;
        unsigned out = probe_21(2766677422u, &target);
        if (out != (unsigned)(513692261u + target - 2018949653u)) return 22;
    }
    {
        unsigned target;
        unsigned out = probe_22(0u, &target);
        if (out != (unsigned)(577295767u + target - 577294743u)) return 23;
    }
    {
        unsigned target;
        unsigned out = probe_22(4294967295u, &target);
        if (out != (unsigned)(577289623u + target - 577294743u)) return 23;
    }
    {
        unsigned target;
        unsigned out = probe_22(2147483648u, &target);
        if (out != (unsigned)(577294743u + target - 577294743u)) return 23;
    }
    {
        unsigned target;
        unsigned out = probe_22(886018847u, &target);
        if (out != (unsigned)(2553526922u + target - 577294743u)) return 23;
    }
    {
        unsigned target;
        unsigned out = probe_23(0u, &target);
        if (out != (unsigned)(80967615u + target - 2267921091u)) return 24;
    }
    {
        unsigned target;
        unsigned out = probe_23(4294967295u, &target);
        if (out != (unsigned)(80967618u + target - 2267921091u)) return 24;
    }
    {
        unsigned target;
        unsigned out = probe_23(3565982804u, &target);
        if (out != (unsigned)(2267921091u + target - 2267921091u)) return 24;
    }
    {
        unsigned target;
        unsigned out = probe_23(3217151191u, &target);
        if (out != (unsigned)(3314415930u + target - 2267921091u)) return 24;
    }
    puts("MACHINE_PRIMITIVES_PASS");
    return 0;
}
