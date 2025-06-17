#include <stdio.h>
#include <stdlib.h>  // For atoi()

#include "../include/obfus.h"

/**
 * Computes the nth Fibonacci number recursively.
 * For n <= 2, returns 1.
 * For n > 2, returns fib(n-1) + fib(n-2).
 *
 * @param n The position in the Fibonacci sequence (must be >= 1).
 * @return The nth Fibonacci number.
 */
int fib(int n) {
    if (n <= 2) {
        return 1;
    } else {
        return fib(n - 1) + fib(n - 2);
    }
}

/**
 * Entry point of the program.
 * Expects a single command-line argument specifying n.
 * Prints the nth Fibonacci number.
 *
 * Usage:
 *   fib n
 *
 * Example:
 *   fib 10
 *   Output: fib(10) = 55
 */
int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage: fib n\n");
        printf("Compute the nth Fibonacci number.\n");
        return 1;
    }

    int n = atoi(argv[1]);
    printf("fib(%d) = %d\n", n, fib(n));
    return 0;
}
