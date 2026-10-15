// Independent unsigned arithmetic oracle; it does not reuse guard VALUE macros.
int main(void) {
    const unsigned boundary[] = {0, 1, 2, 3, 0x7fffffffu, 0x80000000u, 0xfffffffeu, 0xffffffffu};
    for (unsigned n = 0; n < sizeof(raw_cases) / sizeof(raw_cases[0]); ++n) {
        unsigned random = 0x12345678u, first = 0, varied = 0;
        for (unsigned i = 0; i < 264; ++i) {
            unsigned x = i < 8 ? boundary[i] : (random = random * 1664525u + 1013904223u);
            unsigned key = raw_cases[n].key;
            unsigned expected = raw_cases[n].kind == 8 ? x ^ key : raw_cases[n].kind == 10 ? x & (0u - x)
                                                                                           : (x + key) * (x + key) - x * x;
            unsigned actual = raw_cases[n].run(x);
            if (actual != expected) {
                printf("SF_LIVE_GUARDS_FAIL %u %u\n", n, i);
                return 1;
            }
            if (!i)
                first = actual;
            else
                varied |= actual != first;
        }
        // Guard13 is algebraically constant only for the two degenerate keys.
        if (!varied && (raw_cases[n].kind != 13 || 2u * raw_cases[n].key != 0)) return 2;
    }
    puts("SF_LIVE_GUARDS_PASS");
    return 0;
}
