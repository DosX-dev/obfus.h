#include <stdio.h>
#define FAST_NAME native_tokens
#include "fast_tokens_body.h"
#undef FAST_NAME
#include "../include/obfus.h"
#define FAST_NAME protected_tokens
#include "fast_tokens_body.h"
#undef FAST_NAME

int main(void) {
    for (unsigned i = 0; i < 256; ++i) {
        unsigned expected = native_tokens(i), actual = protected_tokens(i);
        if (expected != actual) {
            fprintf(stderr, "fast token failure: %u %u %u\n", i, expected, actual);
            return 1;
        }
    }
    puts("FAST_TOKENS_PASS");
    return 0;
}
