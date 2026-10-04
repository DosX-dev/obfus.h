#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#include "../include/obfus.h"
#define CHECK(x)                                                   \
    do {                                                           \
        if (!(x)) {                                                \
            fprintf(stderr, "stream failure line %d\n", __LINE__); \
            return 1;                                              \
        }                                                          \
    } while (0)
int main(int argc, char **argv) {
    CHECK(argc == 2);
    int number;
    char word[32];
    CHECK(scanf("%d %31s", &number, word) == 2);
    CHECK(number == 42 && strcmp(word, "automated") == 0);
    FILE *file = fopen(argv[1], "wb");
    CHECK(file);
    char buffer[8192], output[8192];
    for (int i = 0; i < 8192; i++) buffer[i] = (char)(i % 251);
    CHECK(fwrite(buffer, 1, sizeof buffer, file) == sizeof buffer);
    CHECK(fclose(file) == 0);
    file = fopen(argv[1], "rb");
    CHECK(file);
    CHECK(fread(output, 1, sizeof output, file) == sizeof output);
    CHECK(memcmp(buffer, output, sizeof buffer) == 0);
    CHECK(fclose(file) == 0);
    CHECK(remove(argv[1]) == 0);
    char tokens[] = "a,b,c";
    CHECK(strcmp(strtok(tokens, ","), "a") == 0);
    CHECK(strcmp(strtok(NULL, ","), "b") == 0);
    CHECK(strcmp(strtok(NULL, ","), "c") == 0);
    CHECK(strtok(NULL, ",") == NULL);
    CHECK(tolower('A') == 'a' && toupper('b') == 'B' && tolower(EOF) == EOF);
    char directory[4096];
    CHECK(getcwd(directory, sizeof directory));
    CHECK(strlen(directory) > 0);
    srand(1234);
    int a = rand();
    srand(1234);
    CHECK(rand() == a);
    void *zero = malloc(0);
    free(zero);
    CHECK(calloc((size_t)-1, 2) == NULL);
    CHECK(printf("stream:%s:%d\n", word, number) > 0);
    int result = 17;
    CHECK(printf("%d", result) == 2);
    CHECK(result == 17);
    puts("STREAMS_PASS");
    return 0;
}
