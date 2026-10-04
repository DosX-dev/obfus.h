#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <windows.h>

/* Default protection: no VM, custom protection macros or extra options. */
#include "../include/obfus.h"

/* A fictional license format for this demonstration only. */
__declspec(dllexport) int demo_keygen(const char *name, char *output, unsigned capacity) {
    uint32_t a = 0x811c9dc5u, b = 0x9e3779b9u;
    unsigned count = 0;
    if (!name || !output || capacity < 36) return 0;
    for (const unsigned char *p = (const unsigned char *)name; *p; ++p) {
        unsigned ch = *p;
        if (ch >= 'a' && ch <= 'z') ch -= 'a' - 'A';
        if (ch == ' ' || ch == '-') continue;
        a = (a ^ ch) * 0x01000193u;
        b ^= a + ch + count;
        b = (b << 7) | (b >> 25);
        ++count;
    }
    if (!count) return 0;
    a ^= b >> 13;
    b ^= a * 0x85ebca6bu;
    uint32_t c = (a ^ 0xa5a5a5a5u) + count * 0x27d4eb2du;
    uint32_t d = (b ^ c) * 0xc2b2ae35u;
    return sprintf(output, "%08X-%08X-%08X-%08X", a, b, c, d) == 35;
}

int main(int argc, char **argv) {
    char key[36];
    if (argc == 2 && strcmp(argv[1], "--stress") == 0) {
        uint32_t digest = 2166136261u;
        for (unsigned i = 0; i < 10000; ++i) {
            char name[64];
            sprintf(name, "User-%u alpha %u", i, i * 2654435761u);
            if (!demo_keygen(name, key, sizeof key)) return 2;
            for (unsigned j = 0; key[j]; ++j) digest = (digest ^ (unsigned char)key[j]) * 16777619u;
        }
        if (printf("STRESS_PASS %08X\n", digest) < 0) return 3;
        return fflush(stdout) == 0 ? 0 : 3;
    }
    if (!demo_keygen(argc > 1 ? argv[1] : "Alice Example", key, sizeof key)) return 1;
    if (puts(key) < 0) return 3;
    return fflush(stdout) == 0 ? 0 : 3;
}
