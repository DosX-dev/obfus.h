#include <stdio.h>
#include <windows.h>
static volatile LONG cold_calls;
void obfh_test_crt_cold(void) { InterlockedIncrement(&cold_calls); }
#include "../include/obfus.h"
static FARPROC resolve_at_one_site(const char *name) { return obfh_crt_resolve(name); }
static FARPROC resolve_at_another_site(const char *name) { return obfh_crt_resolve(name); }
#define CHECK(x) \
    do { \
        if (!(x)) { \
            fprintf(stderr, "cache failure line %d: %s\n", __LINE__, #x); \
            return 1; \
        } \
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
    int (*absolute)(int) = (int (*)(int))resolve_at_one_site(name);
    CHECK(absolute && absolute(-17) == 17);
    strcpy(name, "not_the_original_buffer");
    before = cold_calls;
    CHECK(resolve_at_one_site("abs") == (FARPROC)absolute && cold_calls == before);
    CHECK(!resolve_at_one_site("obfh_nonexistent_export"));
    CHECK(!resolve_at_one_site("obfh_nonexistent_export"));
    CHECK(cold_calls == before + 2);
    before = cold_calls;
    CHECK(resolve_at_another_site("abs") == (FARPROC)absolute && cold_calls == before + 1);
    CHECK(resolve_at_another_site("abs") == (FARPROC)absolute && cold_calls == before + 1);
    // A different name at an initialized site must resolve without replacing its entry.
    const char *names[] = {"atoi", "atol", "atof", "malloc", "calloc", "free", "strcat", "strncat", "strncpy", "strncmp", "strlen", "strcmp", "strchr", "strrchr", "strstr", "strspn", "strcspn", "strpbrk", "strtok", "memcmp", "memset", "memmove", "fopen", "fclose", "fread", "fwrite", "fflush", "feof", "ferror", "clearerr", "rewind", "fseek", "ftell", "rand", "srand", "getenv", "system", "atexit", "exit", "abort"};
    for (int i = 0; i < sizeof(names) / sizeof(names[0]); ++i) {
        FARPROC first = resolve_at_one_site(names[i]);
        CHECK(first && first == resolve_at_one_site(names[i]));
    }
    before = cold_calls;
    CHECK(resolve_at_one_site("abs") == (FARPROC)absolute && cold_calls == before);
    CHECK(absolute(-91) == 91);
    puts("CACHE_PASS");
    return 0;
}
