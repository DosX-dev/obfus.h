
#include <stdio.h>

// #define no_cflow      1
// #define no_antidebug  1
#include "../../include/obfus.h"
#include "example.h"  // test header import

void main() {
    int i = 0;
    for (i = 0; i < 5; i++) {
        char *out = malloc(256);

        strcpy(out, teststr);

        sprintf(out, out, (i + 1));

        if (out) {
            printf(out);
        } else {
            printf(errstr);
        }

        free(out);
    }

    getch();
}