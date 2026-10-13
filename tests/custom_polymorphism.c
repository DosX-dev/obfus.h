#include <stdio.h>
#include <string.h>
#include <windows.h>
static int bytes_equal(const unsigned char *a, const unsigned char *b, unsigned n) {
    for (unsigned i = 0; i < n; ++i)
        if (a[i] != b[i]) return 0;
    return 1;
}
/* All eight registers, including the Windows x64 callee-preserved XMM6/7. */
static void load_xmm(const void *p) {
    __asm__ __volatile__(
        ".byte 0xf3,0x0f,0x6f,0x02; .byte 0xf3,0x0f,0x6f,0x4a,16;"
        ".byte 0xf3,0x0f,0x6f,0x52,32; .byte 0xf3,0x0f,0x6f,0x5a,48;"
        ".byte 0xf3,0x0f,0x6f,0x62,64; .byte 0xf3,0x0f,0x6f,0x6a,80;"
        ".byte 0xf3,0x0f,0x6f,0x72,96; .byte 0xf3,0x0f,0x6f,0x7a,112;"
        :
        : "d"(p)
        : "memory");
}
static void store_xmm(void *p) {
    __asm__ __volatile__(
        ".byte 0xf3,0x0f,0x7f,0x02; .byte 0xf3,0x0f,0x7f,0x4a,16;"
        ".byte 0xf3,0x0f,0x7f,0x52,32; .byte 0xf3,0x0f,0x7f,0x5a,48;"
        ".byte 0xf3,0x0f,0x7f,0x62,64; .byte 0xf3,0x0f,0x7f,0x6a,80;"
        ".byte 0xf3,0x0f,0x7f,0x72,96; .byte 0xf3,0x0f,0x7f,0x7a,112;"
        :
        : "d"(p)
        : "memory");
}
#include "../include/obfus.h"
static void *move(void *a, const void *b, size_t n) { return memmove(a, b, n); }
#if OBFH_DISABLE_ANY_OBFUSCATION != 1 && !defined(__INTELLISENSE__)
// Both formulas must expand at the same call-site line, even after formatting.
#define CHECK_CUSTOM_DRAW() ({ \
    OBFH_CUSTOM_BEGIN; \
    (unsigned int)__obfh_custom_draw != OBFH_PHANTOM_DRAW(__obfh_custom_site); \
})
#endif
int main(void) {
    unsigned char before[128], after[128], data[400];
    unsigned fail = 0;
    for (unsigned i = 0; i < 128; ++i) before[i] = (unsigned char)(i * 19 + 7);
    for (unsigned n = 1; n <= 255; ++n) {
        for (unsigned i = 0; i < sizeof data; ++i) data[i] = (unsigned char)i;
#if OBFH_DISABLE_ANY_OBFUSCATION != 1 && !defined(__INTELLISENSE__)
        load_xmm(before);
#endif
        fail |= move(data + 3, data, n) != data + 3;
#if OBFH_DISABLE_ANY_OBFUSCATION != 1 && !defined(__INTELLISENSE__)
        store_xmm(after);
        fail |= !bytes_equal(before, after, 128);
#endif
        for (unsigned i = 0; i < n; ++i) fail |= data[i + 3] != (unsigned char)i;
        for (unsigned i = 0; i < 3; ++i) fail |= data[i] != (unsigned char)i;
        for (unsigned i = n + 3; i < sizeof data; ++i) fail |= data[i] != (unsigned char)i;
    }
#if OBFH_DISABLE_ANY_OBFUSCATION != 1 && !defined(__INTELLISENSE__)
    {
        __label__ selected;
        fail |= CHECK_CUSTOM_DRAW();
        OBFH_CUSTOM_BEGIN;
        int count = 0, a = 3, b = 7;
        int *pointer = &a;
        fail |= OBFH_CUSTOM_SELECT(++count == 1, pointer, (int *)NULL) != &a;
        fail |= count != 1;
        OBFH_CUSTOM_PAIR(++a, --b);
        fail |= a != 4 || b != 6;
        OBFH_CUSTOM_EXIT(++count == 2, selected);
        fail |= 1;
    selected:;
        fail |= count != 2;
    }
#endif
    if (fail) {
        (puts)("CUSTOM_POLYMORPHISM_FAIL");
        return 1;
    }
    (puts)("CUSTOM_POLYMORPHISM_PASS");
    return 0;
}
