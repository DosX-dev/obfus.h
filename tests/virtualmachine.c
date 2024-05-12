#include <stdio.h>
#include <string.h>
#include <windows.h>

#define VIRT 1
#include "../include/obfus.h"

void main() {
    //      VM_***(N1, N2) == RESULT == CORRECT
    printf("VM_ADD(15, 15) == %d == 30\n", VM_ADD(15, 15));
    printf("VM_SUB(30, 15) == %d == 15\n", VM_SUB(30, 15));
    printf("VM_MUL(5, 6) == %d == 30\n", VM_MUL(5, 6));
    printf("VM_DIV(20, 4) == %d == 5\n", VM_DIV(20, 4));
    printf("VM_MOD(20, 3) == %d == 2\n", VM_MOD(20, 3));
    printf("VM_EQU(10, 10) == %d == 1\n", VM_EQU(10, 10));
    printf("VM_EQU(10, 20) == %d == 0\n", VM_EQU(10, 20));
    printf("VM_NEQ(10, 20) == %d == 1\n", VM_NEQ(10, 20));
    printf("VM_NEQ(10, 20) == %d == 0\n", VM_NEQ(10, 10));
    printf("VM_LSS(10, 20) == %d == 1\n", VM_LSS(10, 20));
    printf("VM_LSS(20, 10) == %d == 0\n", VM_LSS(20, 10));
    printf("VM_GTR(20, 10) == %d == 1\n", VM_GTR(20, 10));
    printf("VM_GTR(10, 20) == %d == 0\n", VM_GTR(10, 20));
    printf("VM_LEQ(10, 10) == %d == 1\n", VM_LEQ(10, 10));
    printf("VM_LEQ(20, 10) == %d == 0\n", VM_LEQ(20, 10));
    printf("VM_GEQ(20, 10) == %d == 1\n", VM_GEQ(20, 10));
    printf("VM_GEQ(10, 20) == %d == 0\n", VM_GEQ(10, 20));

    printf("VM_ADD_DBL(3.5, 4.5) == %.1f == 8.0\n", VM_ADD_DBL(3.5, 4.5));
    printf("VM_SUB_DBL(10.0, 5.5) == %.1f == 4.5\n", VM_SUB_DBL(10.0, 5.5));
    printf("VM_MUL_DBL(2.5, 4.0) == %.1f == 10.0\n", VM_MUL_DBL(2.5, 4.0));
    printf("VM_DIV_DBL(20.0, 4.0) == %.1f == 5.0\n", VM_DIV_DBL(20.0, 4.0));
    printf("VM_LSS_DBL(3.5, 4.5) == %d == 1\n", VM_LSS_DBL(3.5, 4.5));
    printf("VM_LSS_DBL(4.5, 3.5) == %d == 0\n", VM_LSS_DBL(4.5, 3.5));
    printf("VM_GTR_DBL(4.5, 3.5) == %d == 1\n", VM_GTR_DBL(4.5, 3.5));
    printf("VM_GTR_DBL(3.5, 4.5) == %d == 0\n", VM_GTR_DBL(3.5, 4.5));

    getch();
}