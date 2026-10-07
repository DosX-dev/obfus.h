#include <stdio.h>
#include <windows.h>
static void cache_observer(void *, unsigned int);
#define OBFH_GUI_CACHE_OBSERVER(slot, site) cache_observer(slot, site)
#define NO_ANTIDEBUG 1
#include "../include/obfus.h"
#undef if
#undef else
#undef for
#undef while
#undef break
#undef switch
#undef printf
#undef puts
static void *seen[8];
static unsigned int keys[8], count;
static int record = 1, failed;
static void cache_observer(void *slot, unsigned int site) {
    if (!record) return;
    for (unsigned int i = 0; i < count; ++i)
        if (seen[i] == slot) {
            if (keys[i] != site) failed = 1;
            return;
        }
    if (count == 8) {
        failed = 1;
        return;
    }
    seen[count] = slot;
    keys[count++] = site;
}
static DWORD first_site(void) { return GetCurrentThreadId(); }
static DWORD second_site(void) { return GetCurrentThreadId(); }
static DWORD thread_site(void) { return GetCurrentThreadId(); }
static DWORD WINAPI worker(void *unused) {
    for (unsigned int i = 0; i < 1000; ++i)
        if (thread_site() != (GetCurrentThreadId)()) return 1;
    return 0;
}
int main(void) {
    DWORD expected = (GetCurrentThreadId)();
    if (first_site() != expected || first_site() != expected || second_site() != expected || second_site() != expected) return 1;
    if (failed || count != 2 || seen[0] == seen[1] || keys[0] == keys[1]) return 2;
    OBFH_GUI_SLOT *a = seen[0], *b = seen[1];
    if (!a->ready || !b->ready || a->encoded == b->encoded || ((ULONG_PTR)a & 15u) || ((ULONG_PTR)b & 15u)) return 3;
    record = 0;
    HANDLE threads[8];
    for (unsigned int i = 0; i < 8; ++i) {
        threads[i] = (CreateThread)(NULL, 0, worker, NULL, 0, NULL);
        if (!threads[i]) return 4;
    }
    if ((WaitForMultipleObjects)(8, threads, TRUE, 15000) != WAIT_OBJECT_0) return 5;
    for (unsigned int i = 0; i < 8; ++i) {
        DWORD result;
        if (!(GetExitCodeThread)(threads[i], &result) || result) return 6;
        (CloseHandle)(threads[i]);
    }
    puts("SITE_CACHE_PASS");
    return 0;
}
