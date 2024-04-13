
#include <stdio.h>

// #define no_cflow      1
// #define no_antidebug  1
#include "../include/obfus.h"

void main() {
    int i = 0;
    for (i = 0; i < 5; i++) {
        char *out = malloc(256);

        strcpy(out, "%d) Hello, world!\n");

        sprintf(out, out, (i + 1));

        if (out) {
            printf(out);
        } else {
            printf("Error!");
        }

        free(out);
    }

    getch();
}