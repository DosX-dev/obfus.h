#include <stdio.h>
#include <string.h>
#include <windows.h>

#include "../include/obfus.h"
int main(void) {
    int accepted = 0, rejected = 0, oversized = 0;
    for (int n = 220; n <= 260; n++) {
        char path[300];
        memset(path, 'a', n);
        path[0] = 'C';
        path[1] = ':';
        path[2] = 92;
        path[n - 1] = 92;
        path[n] = 0;
        if (!SetEnvironmentVariableA("TMP", path)) return 2;
        struct {
            char name[MAX_PATH];
            unsigned char guard[32];
        } data;
        memset(&data, 0x5a, sizeof data);
        DWORD length = GetTempPath(MAX_PATH, data.name);
        int fits = length > 0 && length < MAX_PATH;
        if (fits) fits = length + sizeof("obfh-wrapper-regression.tmp") <= sizeof data.name;
        if (fits) {
            strcat(data.name, "obfh-wrapper-regression.tmp");
            accepted++;
        } else {
            rejected++;
            if (length >= MAX_PATH) oversized++;
        }
        for (int i = 0; i < 32; i++)
            if (data.guard[i] != 0x5a) return 3;
        if (fits != (n + sizeof("obfh-wrapper-regression.tmp") <= MAX_PATH)) return 4;
    }
    if (accepted != 13 || rejected != 28 || oversized != 1) return 5;
    puts("PATHS_PASS");
    return 0;
}
