// Unit testing is very helpful in fixing virtualization bugs, but fixing them is still always a big headache!

#include <stdio.h>
#include <assert.h>

#define VIRT 1
#include "../include/obfus.h"

void test_int_operations() {
    // Test integer operations
    assert(VM_ADD(15, 15) == 30);
    assert(VM_SUB(30, 15) == 15);
    assert(VM_MUL(5, 6) == 30);
    assert(VM_DIV(20, 4) == 5);
    assert(VM_MOD(20, 3) == 2);
    assert(VM_EQU(10, 10) == 1);
    assert(VM_EQU(10, 20) == 0);
    assert(VM_NEQ(10, 20) == 1);
    assert(VM_NEQ(10, 10) == 0);
    assert(VM_LSS(10, 20) == 1);
    assert(VM_LSS(20, 10) == 0);
    assert(VM_GTR(20, 10) == 1);
    assert(VM_GTR(10, 20) == 0);
    assert(VM_LEQ(10, 10) == 1);
    assert(VM_LEQ(20, 10) == 0);
    assert(VM_GEQ(20, 10) == 1);
    assert(VM_GEQ(10, 20) == 0);

    printf("Integer operations: Passed\n");
}

void test_double_operations() {
    // Test double operations
    assert(VM_ADD_DBL(3.5, 4.5) == 8.0);
    assert(VM_SUB_DBL(10.0, 5.5) == 4.5);
    assert(VM_MUL_DBL(2.5, 4.0) == 10.0);
    assert(VM_DIV_DBL(20.0, 4.0) == 5.0);
    assert(VM_LSS_DBL(3.5, 4.5) == 1);
    assert(VM_LSS_DBL(4.5, 3.5) == 0);
    assert(VM_GTR_DBL(4.5, 3.5) == 1);
    assert(VM_GTR_DBL(3.5, 4.5) == 0);

    printf("Double operations: Passed\n");
}

void main() {
    // Call all the tests
    test_int_operations();
    test_double_operations();
}