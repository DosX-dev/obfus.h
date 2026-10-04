#include <stdio.h>
#include <windows.h>
static volatile LONG cold_calls;
void obfh_test_crt_cold(void) { InterlockedIncrement(&cold_calls); }
#include "../include/obfus.h"
#define CHECK(x)                                                          \
    do {                                                                  \
        if (!(x)) {                                                       \
            fprintf(stderr, "cache failure line %d: %s\n", __LINE__, #x); \
            return 1;                                                     \
        }                                                                 \
    } while (0)
static DWORD WINAPI worker(void *unused) {
    for (int i = 0; i < 500; ++i) {
        char source[32], target[32], moved[32];
        strcpy(source, "parallel cached CRT");
        memcpy(target, source, sizeof source);
        strcpy(moved, target);
        CHECK(strcmp(moved, "parallel cached CRT") == 0);
        void *memory = realloc(NULL, 37);
        CHECK(memory);
        free(memory);
    }
    return 0;
}
int main(void) {
    HANDLE threads[8];
    for (int i = 0; i < 8; ++i) {
        threads[i] = CreateThread(NULL, 0, worker, NULL, 0, NULL);
        CHECK(threads[i]);
    }
    CHECK(WaitForMultipleObjects(8, threads, TRUE, 10000) == WAIT_OBJECT_0);
    for (int i = 0; i < 8; ++i) {
        DWORD result;
        CHECK(GetExitCodeThread(threads[i], &result) && result == 0);
        CHECK(CloseHandle(threads[i]));
    }
    LONG before = cold_calls;
    CHECK(worker(NULL) == 0);
    CHECK(cold_calls == before);
    char name[32];
    strcpy(name, "abs");
    int (*absolute)(int) = (int (*)(int))obfh_crt_resolve(name);
    CHECK(absolute && absolute(-17) == 17);
    strcpy(name, "not_the_original_buffer");
    before = cold_calls;
    CHECK(obfh_crt_resolve("abs") == (FARPROC)absolute && cold_calls == before);
    CHECK(!obfh_crt_resolve("obfh_nonexistent_export"));
    CHECK(!obfh_crt_resolve("obfh_nonexistent_export"));
    CHECK(cold_calls == before + 2);
    // Saturating the bounded cache must preserve fallback resolution.
    const char *names[] = {"atoi", "atol", "atof", "malloc", "calloc", "free", "strcat", "strncat", "strncpy", "strncmp", "strlen", "strcmp", "strchr", "strrchr", "strstr", "strspn", "strcspn", "strpbrk", "strtok", "memcmp", "memset", "memmove", "fopen", "fclose", "fread", "fwrite", "fflush", "feof", "ferror", "clearerr", "rewind", "fseek", "ftell", "rand", "srand", "getenv", "system", "atexit", "exit", "abort"};
    for (int i = 0; i < sizeof(names) / sizeof(names[0]); ++i) {
        FARPROC first = obfh_crt_resolve(names[i]);
        CHECK(first && first == obfh_crt_resolve(names[i]));
    }
    CHECK(absolute(-91) == 91);
    puts("CACHE_PASS");
    return 0;
}
