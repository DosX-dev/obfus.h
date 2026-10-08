// Full-header integration stress test. Unsafe modes MUST run as child processes.
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
static FARPROC reference_export(HMODULE module, const char *name) { return GetProcAddress(module, name); }
static void test_phase(const char *phase, int progress) {
    fprintf(stderr, "TEST_PHASE: %s %d\n", phase, progress);
    fflush(stderr);
}
#include "../include/obfus.h"
#define CHECK(x)                                                           \
    do {                                                                   \
        if (!(x)) {                                                        \
            fprintf(stderr, "failed %s:%d: %s\n", __FILE__, __LINE__, #x); \
            return 1;                                                      \
        }                                                                  \
    } while (0)

#if !NO_OBF
static int export_boundaries(void) {
    BYTE *image = VirtualAlloc(NULL, 4096, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    CHECK(image);
    PIMAGE_DOS_HEADER dos = (PIMAGE_DOS_HEADER)image;
    dos->e_magic = IMAGE_DOS_SIGNATURE;
    dos->e_lfanew = 128;
    PIMAGE_NT_HEADERS nt = (PIMAGE_NT_HEADERS)(image + 128);
    nt->Signature = IMAGE_NT_SIGNATURE;
    nt->OptionalHeader.SizeOfImage = 4096;
    nt->OptionalHeader.NumberOfRvaAndSizes = 16;
    nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress = 512;
    nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size = 512;
    PIMAGE_EXPORT_DIRECTORY exports = (PIMAGE_EXPORT_DIRECTORY)(image + 512);
    exports->Base = 1;
    exports->NumberOfFunctions = 1;
    exports->NumberOfNames = 1;
    exports->AddressOfFunctions = 600;
    exports->AddressOfNames = 604;
    exports->AddressOfNameOrdinals = 608;
    *(DWORD *)(image + 600) = 2048;
    *(DWORD *)(image + 604) = 700;
    strcpy((char *)image + 700, "entry");
    CHECK(GetProcAddress((HMODULE)image, "entry") == (FARPROC)(image + 2048));
    CHECK(GetProcAddress((HMODULE)image, (LPCSTR)1) == (FARPROC)(image + 2048));
    CHECK(GetProcAddress((HMODULE)image, (LPCSTR)2) == NULL);
    CHECK(obfh_find_export((HMODULE)image, "entry", 32) == NULL);
    *(DWORD *)(image + 600) = 800;
    strcpy((char *)image + 800, "KERNEL32.GetCurrentProcess");
    CHECK(GetProcAddress((HMODULE)image, "entry") == reference_export(GetModuleHandleA("kernel32.dll"), "GetCurrentProcess"));
    strcpy((char *)image + 800, "KERNEL32.#999999");
    CHECK(GetProcAddress((HMODULE)image, "entry") == NULL);
    *(WORD *)(image + 608) = 1;
    CHECK(GetProcAddress((HMODULE)image, "entry") == NULL);
    *(WORD *)(image + 608) = 0;
    exports->AddressOfFunctions = 4095;
    CHECK(GetProcAddress((HMODULE)image, "entry") == NULL);
    exports->AddressOfFunctions = 600;
    *(DWORD *)(image + 604) = 4095;
    image[4095] = 'x';
    CHECK(GetProcAddress((HMODULE)image, "entry") == NULL);
    CHECK(VirtualFree(image, 0, MEM_RELEASE));
    return 0;
}
#endif

#if VIRT && !NO_OBF
static volatile LONG vm_calls;
static long double observed_vm(const unsigned int *program, unsigned int length, unsigned int key, OBFH_VM_VALUE a, OBFH_VM_VALUE b) {
    InterlockedIncrement(&vm_calls);
    return (Obfh_VirtualMachine)(program, length, key, a, b);
}
#define Obfh_VirtualMachine(...) observed_vm(__VA_ARGS__)
#endif
static int exercise(int report) {
    for (int i = 0; i < 300; ++i) {
        if (report && i % 50 == 0) test_phase("scalar stress /300", i);
        char *p = malloc(8192);
        CHECK(p);
        memset(p, 'x', 8191);
        p[8191] = 0;
        CHECK(strlen(p) == 8191);
        p = realloc(p, 16384);
        CHECK(p);
        CHECK(p[8190] == 'x');
        free(p);
        p = calloc(128, 64);
        CHECK(p);
        CHECK(p[8191] == 0);
        free(p);
        CHECK(strcmp("\xff", "\x01") > 0);
        CHECK(strcmp("a", "aa") < 0);
        CHECK(strcmp(HIDE_STRING("hidden"), "hidden") == 0);
        char *first = HIDE_STRING("first");
        char *second = HIDE_STRING("second");
        char stack_churn[8192];
        memset(stack_churn, 'z', sizeof stack_churn);
        CHECK(strcmp(first, "first") == 0 && strcmp(second, "second") == 0);
        char out[32];
        CHECK(sprintf(out, "%s:%d", "ok", i) > 0);
        CHECK(snprintf(out, sizeof out, "plain") == 5);
        struct {
            char text[4];
            unsigned char guard[8];
        } tiny;
        memset(&tiny, 0x5a, sizeof tiny);
        snprintf(tiny.text, sizeof tiny.text, "%s", "0123456789");
        for (int k = 0; k < 8; k++) CHECK(tiny.guard[k] == 0x5a);
        CHECK(strchr("hello", 'e') != NULL && strrchr("hello", 'l') != NULL);
        char copy[16];
        CHECK(strcpy(copy, "hello") == copy);
        CHECK(memcpy(copy, "world", 6) == copy && strcmp(copy, "world") == 0);
        int side_effect = -3;
        CHECK(abs(side_effect++) == 3 && side_effect == -2);
        double whole;
        int exponent, quotient;
        CHECK(modf(3.5, &whole) == 0.5 && whole == 3.0);
        CHECK(frexp(8.0, &exponent) == 0.5 && exponent == 4);

#if VIRT && !NO_OBF
        LONG calls_before = vm_calls;
#endif
        CHECK(2 * VM_ADD(3, 4) == 14);
#if VIRT && !NO_OBF
        CHECK(vm_calls > calls_before);
#endif
        CHECK(sin(0.0) == 0.0 && pow(2.0 + 1.0, 2.0) == 9.0);
        CHECK(VM_ADD(5, 7) == 12 && VM_MUL(8, 9) == 72);
        CHECK(VM_SUB(5, 8) == -3 && VM_DIV(81, 9) == 9);
        CHECK(VM_ADD(INT_MIN, 0) == INT_MIN && VM_SUB(INT_MAX, 0) == INT_MAX);
        CHECK(VM_MOD(INT_MIN, 3) == INT_MIN % 3);
#if VIRT && !NO_OBF
        CHECK(VM_MOD(INT_MIN, -1) == 0);
#endif
        CHECK(VM_EQU(42, 42) && VM_NEQ(4, 5));
        CHECK(GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA") == reference_export(GetModuleHandleA("kernel32.dll"), "LoadLibraryA"));
        CHECK(GetProcAddress(GetModuleHandleA("kernel32.dll"), "GetCurrentProcess"));
#if NO_OBF != 1
        HMODULE error_module = GetModuleHandleA("kernel32.dll");
        SetLastError(0x13572468u);
        CHECK(GetProcAddress(error_module, "GetCurrentProcess"));
        CHECK(GetLastError() == 0x13572468u);
        CHECK(GetProcAddress(error_module, "obfh_nonexistent") == NULL);
        CHECK(GetLastError() == ERROR_PROC_NOT_FOUND);
#endif
        CHECK(GetProcAddress(GetModuleHandleA("kernel32.dll"), "obfh_nonexistent") == NULL);
#if !NO_OBF
        CHECK(GetProcAddress(NULL, "anything") == NULL);
        struct {
            char name[18];
            unsigned char guard[8];
        } masks;
        memset(&masks, 0x5a, sizeof masks);
        CHECK(getKernel32Name_proxy(masks.name) == masks.name);
        CHECK(strcmp(masks.name, "kernel32") == 0);
        CHECK(getLoaderName_proxy(masks.name) == masks.name);
        CHECK(strcmp(masks.name, "LoadLibraryA") == 0);
        CHECK(getDebuggerName_proxy(masks.name) == masks.name);
        CHECK(strcmp(masks.name, "IsDebuggerPresent") == 0);
        for (int k = 0; k < 8; k++) CHECK(masks.guard[k] == 0x5a);
        CHECK(obfh_double_proxy(3.5) == 3.5);
        CHECK(obfh_condition_proxy(0, -1) == -1);
        CHECK((ULONG_PTR)GetCurrentProcess() == (ULONG_PTR)-1);
#endif
        HMODULE module = LoadLibraryA("user32.dll");
        CHECK(module);
        CHECK(FreeLibrary(module));
    }
    return 0;
}
static DWORD WINAPI run_worker(void *unused) { return exercise(0); }

int main(void) {
    test_phase("startup", 0);
    SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX);
#if !NO_OBF
    CHECK(export_boundaries() == 0);
#endif
    CHECK(exercise(1) == 0);
    test_phase("four concurrent stress workers", 0);
    HANDLE threads[4];
    for (int i = 0; i < 4; i++) {
        threads[i] = CreateThread(NULL, 0, run_worker, NULL, 0, NULL);
        CHECK(threads[i]);
    }
    CHECK(WaitForMultipleObjects(4, threads, TRUE, 30000) == WAIT_OBJECT_0);
    for (int i = 0; i < 4; i++) {
        DWORD code;
        CHECK(GetExitCodeThread(threads[i], &code) && code == 0);
        CHECK(CloseHandle(threads[i]));
    }
    test_phase("long output", 0);
    char large[4097];
    memset(large, 'A', 4096);
    large[4096] = 0;
    CHECK(printf("%s", large) == 4096);
    CHECK(printf("\n") == 1);
#if !NO_OBF && NO_ANTIDEBUG != 1
    CHECK(IsDebuggerPresent_proxy() == IsDebuggerPresent());
    test_phase("anti-debug check", 0);
    ANTI_DEBUG;
#endif
    puts("Full-header stress passed");
    return 0;
}
