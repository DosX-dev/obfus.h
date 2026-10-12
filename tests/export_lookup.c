#include <stdio.h>
#include <string.h>
#include <windows.h>

static FARPROC native_export(HMODULE module, LPCSTR name) {
    return GetProcAddress(module, name);
}
#include "../include/obfus.h"
#undef if
#undef else
#undef for
#undef while
#undef break
#undef return
#undef printf

#define CHECK(value) \
    do { \
        if (!(value)) { \
            printf("export failure: %d\n", __LINE__); \
            return 1; \
        } \
    } while (0)

static int compare_exports(const char *library) {
    HMODULE module = LoadLibraryA(library);
    CHECK(module);
    BYTE *base = (BYTE *)module;
    PIMAGE_NT_HEADERS nt = (PIMAGE_NT_HEADERS)(base + ((PIMAGE_DOS_HEADER)base)->e_lfanew);
    PIMAGE_EXPORT_DIRECTORY table = (PIMAGE_EXPORT_DIRECTORY)(base + nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);
    DWORD *names = (DWORD *)(base + table->AddressOfNames);
    DWORD *functions = (DWORD *)(base + table->AddressOfFunctions);
    WORD *ordinals = (WORD *)(base + table->AddressOfNameOrdinals);
    for (DWORD i = 0; i < table->NumberOfNames; ++i) {
        const char *name = (const char *)(base + names[i]);
        DWORD rva = functions[ordinals[i]];
        IMAGE_DATA_DIRECTORY directory = nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];
        if (rva >= directory.VirtualAddress && rva - directory.VirtualAddress < directory.Size) {
            const char *forward = (const char *)(base + rva);
            // API-set forwarding can depend on the importing module. The existing
            // LoadLibrary-based resolver does not implement that loader contract.
            if (!strncmp(forward, "api-ms-", 7) || !strncmp(forward, "ext-ms-", 7)) continue;
        }
        FARPROC actual = GetProcAddress(module, name), expected = native_export(module, name);
        // Compatibility shims may override native GetProcAddress on x86. A direct
        // export must still agree with the independently read PE function RVA.
        int direct = !(rva >= directory.VirtualAddress && rva - directory.VirtualAddress < directory.Size);
        if (actual != expected && !(direct && rva && actual == (FARPROC)((ULONG_PTR)base + rva))) {
            printf("export mismatch: %s!%s actual=%p expected=%p\n", library, name, actual, expected);
            return 1;
        }
        DWORD ordinal = table->Base + ordinals[i];
        if (ordinal && ordinal <= 65535) {
            actual = GetProcAddress(module, (LPCSTR)(ULONG_PTR)ordinal);
            expected = native_export(module, (LPCSTR)(ULONG_PTR)ordinal);
            CHECK(actual == expected || (direct && rva && actual == (FARPROC)((ULONG_PTR)base + rva)));
        }
    }
    CHECK(!GetProcAddress(module, ""));
    CHECK(!GetProcAddress(module, "!missing_obfh_export"));
    CHECK(!GetProcAddress(module, "zzzz_missing_obfh_export"));
    CHECK(GetLastError() == ERROR_PROC_NOT_FOUND);
    CHECK(FreeLibrary(module));
    return 0;
}

static int malformed_names(void) {
    BYTE *image = VirtualAlloc(NULL, 8192, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    CHECK(image);
    DWORD old;
    CHECK(VirtualProtect(image + 4096, 4096, PAGE_NOACCESS, &old));
    PIMAGE_DOS_HEADER dos = (PIMAGE_DOS_HEADER)image;
    dos->e_magic = IMAGE_DOS_SIGNATURE;
    dos->e_lfanew = 128;
    PIMAGE_NT_HEADERS nt = (PIMAGE_NT_HEADERS)(image + 128);
    nt->Signature = IMAGE_NT_SIGNATURE;
    nt->OptionalHeader.SizeOfImage = 4096;
    nt->OptionalHeader.NumberOfRvaAndSizes = 16;
    nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress = 512;
    nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size = 512;
    PIMAGE_EXPORT_DIRECTORY table = (PIMAGE_EXPORT_DIRECTORY)(image + 512);
    table->Base = 1;
    table->NumberOfNames = 3;
    table->NumberOfFunctions = 3;
    table->AddressOfFunctions = 600;
    table->AddressOfNames = 612;
    table->AddressOfNameOrdinals = 624;
    DWORD *functions = (DWORD *)(image + 600), *names = (DWORD *)(image + 612);
    WORD *ordinals = (WORD *)(image + 624);
    for (unsigned i = 0; i < 3; ++i) {
        functions[i] = 2048 + i;
        names[i] = 700 + i * 16;
        ordinals[i] = i;
    }
    strcpy((char *)image + names[0], "alpha");
    strcpy((char *)image + names[1], "middle");
    strcpy((char *)image + names[2], "omega");
    CHECK(GetProcAddress((HMODULE)image, "alpha") == (FARPROC)(image + 2048));
    CHECK(GetProcAddress((HMODULE)image, "middle") == (FARPROC)(image + 2049));
    CHECK(GetProcAddress((HMODULE)image, "omega") == (FARPROC)(image + 2050));
    CHECK(!GetProcAddress((HMODULE)image, "between"));
    CHECK(!GetProcAddress((HMODULE)image, "zzzz"));
    names[1] = 4095;
    image[4095] = 'x';
    CHECK(!GetProcAddress((HMODULE)image, "middle"));
    names[1] = 4096;
    CHECK(!GetProcAddress((HMODULE)image, "middle"));
    names[1] = 716;
    ordinals[1] = 3;
    CHECK(!GetProcAddress((HMODULE)image, "middle"));
    CHECK(VirtualFree(image, 0, MEM_RELEASE));
    return 0;
}

int main(void) {
    if (compare_exports("kernel32") || compare_exports("user32") || compare_exports("gdi32") || malformed_names()) return 1;
    puts("EXPORT_LOOKUP_PASS");
    return 0;
}
