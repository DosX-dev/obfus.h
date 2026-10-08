#include <stdio.h>

#include "../include/obfus.h"
enum { data_counter_before = __COUNTER__ };
OBFH_DATA_JUNK(test_counter, OBFH_SECTION_ATTRIBUTE);
enum { data_counter_after = __COUNTER__ };
#undef if
#undef for
#undef while
#undef printf
#undef puts
#undef fflush
#define DUMP_DATA(name)                                                                           \
    do {                                                                                          \
        if (printf("JUNK %s %u ", #name, (unsigned int)sizeof(__obfh_data_##name)) < 0) return 6; \
        for (unsigned int n = 0; n < sizeof(__obfh_data_##name); ++n)                             \
            if (printf("%02x", (unsigned int)__obfh_data_##name[n]) < 0) return 6;                \
        if (puts("") < 0) return 6;                                                               \
    } while (0)
int main(void) {
    if (data_counter_after != data_counter_before + 1) return 7;
    if (_a != 'a' || sizeof(_a) != 1 || sizeof(_s_a) != 2 || _s_a[0] != 'a' || _s_a[1] != 0) return 1;
    if (_b != 'b' || sizeof(_b) != 1 || sizeof(_s_b) != 2 || _s_b[0] != 'b' || _s_b[1] != 0) return 1;
    if (_c != 'c' || sizeof(_c) != 1 || sizeof(_s_c) != 2 || _s_c[0] != 'c' || _s_c[1] != 0) return 1;
    if (_d != 'd' || sizeof(_d) != 1 || sizeof(_s_d) != 2 || _s_d[0] != 'd' || _s_d[1] != 0) return 1;
    if (_e != 'e' || sizeof(_e) != 1 || sizeof(_s_e) != 2 || _s_e[0] != 'e' || _s_e[1] != 0) return 1;
    if (_f != 'f' || sizeof(_f) != 1 || sizeof(_s_f) != 2 || _s_f[0] != 'f' || _s_f[1] != 0) return 1;
    if (_g != 'g' || sizeof(_g) != 1 || sizeof(_s_g) != 2 || _s_g[0] != 'g' || _s_g[1] != 0) return 1;
    if (_h != 'h' || sizeof(_h) != 1 || sizeof(_s_h) != 2 || _s_h[0] != 'h' || _s_h[1] != 0) return 1;
    if (_i != 'i' || sizeof(_i) != 1 || sizeof(_s_i) != 2 || _s_i[0] != 'i' || _s_i[1] != 0) return 1;
    if (_j != 'j' || sizeof(_j) != 1 || sizeof(_s_j) != 2 || _s_j[0] != 'j' || _s_j[1] != 0) return 1;
    if (_k != 'k' || sizeof(_k) != 1 || sizeof(_s_k) != 2 || _s_k[0] != 'k' || _s_k[1] != 0) return 1;
    if (_l != 'l' || sizeof(_l) != 1 || sizeof(_s_l) != 2 || _s_l[0] != 'l' || _s_l[1] != 0) return 1;
    if (_m != 'm' || sizeof(_m) != 1 || sizeof(_s_m) != 2 || _s_m[0] != 'm' || _s_m[1] != 0) return 1;
    if (_n != 'n' || sizeof(_n) != 1 || sizeof(_s_n) != 2 || _s_n[0] != 'n' || _s_n[1] != 0) return 1;
    if (_o != 'o' || sizeof(_o) != 1 || sizeof(_s_o) != 2 || _s_o[0] != 'o' || _s_o[1] != 0) return 1;
    if (_p != 'p' || sizeof(_p) != 1 || sizeof(_s_p) != 2 || _s_p[0] != 'p' || _s_p[1] != 0) return 1;
    if (_q != 'q' || sizeof(_q) != 1 || sizeof(_s_q) != 2 || _s_q[0] != 'q' || _s_q[1] != 0) return 1;
    if (_r != 'r' || sizeof(_r) != 1 || sizeof(_s_r) != 2 || _s_r[0] != 'r' || _s_r[1] != 0) return 1;
    if (_s != 's' || sizeof(_s) != 1 || sizeof(_s_s) != 2 || _s_s[0] != 's' || _s_s[1] != 0) return 1;
    if (_t != 't' || sizeof(_t) != 1 || sizeof(_s_t) != 2 || _s_t[0] != 't' || _s_t[1] != 0) return 1;
    if (_u != 'u' || sizeof(_u) != 1 || sizeof(_s_u) != 2 || _s_u[0] != 'u' || _s_u[1] != 0) return 1;
    if (_v != 'v' || sizeof(_v) != 1 || sizeof(_s_v) != 2 || _s_v[0] != 'v' || _s_v[1] != 0) return 1;
    if (_w != 'w' || sizeof(_w) != 1 || sizeof(_s_w) != 2 || _s_w[0] != 'w' || _s_w[1] != 0) return 1;
    if (_x != 'x' || sizeof(_x) != 1 || sizeof(_s_x) != 2 || _s_x[0] != 'x' || _s_x[1] != 0) return 1;
    if (_y != 'y' || sizeof(_y) != 1 || sizeof(_s_y) != 2 || _s_y[0] != 'y' || _s_y[1] != 0) return 1;
    if (_z != 'z' || sizeof(_z) != 1 || sizeof(_s_z) != 2 || _s_z[0] != 'z' || _s_z[1] != 0) return 1;
    if (_S != 'S' || sizeof(_S) != 1) return 2;
    if (_L != 'L' || sizeof(_L) != 1) return 2;
    if (_A != 'A' || sizeof(_A) != 1) return 2;
    if (_I != 'I' || sizeof(_I) != 1) return 2;
    if (_D != 'D' || sizeof(_D) != 1) return 2;
    if (_P != 'P' || sizeof(_P) != 1) return 2;
    if (_0 != 0 || sizeof(_0) != 1) return 3;
    if (_1 != 1 || sizeof(_1) != 1) return 3;
    if (_2 != 2 || sizeof(_2) != 1) return 3;
    if (_3 != 3 || sizeof(_3) != 1) return 3;
    if (_4 != 4 || sizeof(_4) != 1) return 3;
    if (_5 != 5 || sizeof(_5) != 1) return 3;
    if (_6 != 6 || sizeof(_6) != 1) return 3;
    if (_7 != 7 || sizeof(_7) != 1) return 3;
    if (_8 != 8 || sizeof(_8) != 1) return 3;
    if (_9 != 9 || sizeof(_9) != 1) return 3;
    DUMP_DATA(_s_a);
    DUMP_DATA(_s_b);
    DUMP_DATA(_s_c);
    DUMP_DATA(_s_d);
    DUMP_DATA(_s_e);
    DUMP_DATA(_s_f);
    DUMP_DATA(_s_g);
    DUMP_DATA(_s_h);
    DUMP_DATA(_s_i);
    DUMP_DATA(_s_j);
    DUMP_DATA(_s_k);
    DUMP_DATA(_s_l);
    DUMP_DATA(_s_m);
    DUMP_DATA(_s_n);
    DUMP_DATA(_s_o);
    DUMP_DATA(_s_p);
    DUMP_DATA(_s_q);
    DUMP_DATA(_s_r);
    DUMP_DATA(_s_s);
    DUMP_DATA(_s_t);
    DUMP_DATA(_s_u);
    DUMP_DATA(_s_v);
    DUMP_DATA(_s_w);
    DUMP_DATA(_s_x);
    DUMP_DATA(_s_y);
    DUMP_DATA(_s_z);
    DUMP_DATA(_a);
    DUMP_DATA(_b);
    DUMP_DATA(_c);
    DUMP_DATA(_d);
    DUMP_DATA(_e);
    DUMP_DATA(_f);
    DUMP_DATA(_g);
    DUMP_DATA(_h);
    DUMP_DATA(_i);
    DUMP_DATA(_j);
    DUMP_DATA(_k);
    DUMP_DATA(_l);
    DUMP_DATA(_m);
    DUMP_DATA(_n);
    DUMP_DATA(_o);
    DUMP_DATA(_p);
    DUMP_DATA(_q);
    DUMP_DATA(_r);
    DUMP_DATA(_s);
    DUMP_DATA(_t);
    DUMP_DATA(_u);
    DUMP_DATA(_v);
    DUMP_DATA(_w);
    DUMP_DATA(_x);
    DUMP_DATA(_y);
    DUMP_DATA(_z);
    DUMP_DATA(_S);
    DUMP_DATA(_L);
    DUMP_DATA(_A);
    DUMP_DATA(_I);
    DUMP_DATA(_D);
    DUMP_DATA(_P);
    DUMP_DATA(_0);
    DUMP_DATA(_1);
    DUMP_DATA(_2);
    DUMP_DATA(_3);
    DUMP_DATA(_4);
    DUMP_DATA(_5);
    DUMP_DATA(_6);
    DUMP_DATA(_7);
    DUMP_DATA(_8);
    DUMP_DATA(_9);
    if (puts("CONSTANT_DATA_PASS") < 0) return 4;
    return fflush(stdout) == 0 ? 0 : 5;
}
