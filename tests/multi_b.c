#include "../include/obfus.h"
int multi_b(int x) {
    int result = 0;
    for (int i = 0; i < x; ++i) result = VM_ADD(result, 1);
    return result;
}
