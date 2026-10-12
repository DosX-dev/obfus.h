#include <stdio.h>
#define CASE_NAME native_cases
#include "case_tokens_body.h"
#undef CASE_NAME
#include "../include/obfus.h"
#define CASE_NAME protected_cases
#include "case_tokens_body.h"
#undef CASE_NAME
int main(void) {
    for (int input = -64; input < 256; ++input)
        for (int condition = 0; condition < 2; ++condition) {
            unsigned expected = native_cases(input, condition), actual = protected_cases(input, condition);
            if (actual != expected) {
                fprintf(stderr, "case failure: %d %d expected %u actual %u\n", input, condition, expected, actual);
                return 1;
            }
        }
    puts("CASE_TOKENS_PASS");
    return 0;
}
