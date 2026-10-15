#include <math.h>
#include <stdio.h>
#include <windows.h>

#include "../include/obfus.h"
int main(void) {
    // Exercise numeric transport; unused helpers no longer supply its opcodes.
    volatile double value = 7.25;
    if (round(value) != 7.0) return 1;
    puts(HIDE_STRING("obfh_unique_hidden_probe_8a39b17f"));
    return 0;
}
