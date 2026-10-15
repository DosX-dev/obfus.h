#include <stdio.h>
#include <windows.h>
static void cache_observer(void *, unsigned int);
#define OBFH_GUI_CACHE_OBSERVER(slot, site) cache_observer(slot, site)
#define OBFH_DISABLE_ANTIDEBUG 1
#include "../include/obfus.h"
#undef if
#undef else
#undef case
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
static int decode_forms(void) {
    enum { __obfh_gui_key_lo = 0xfedcba98u,
           __obfh_gui_key_hi = 0x76543210u,
           __obfh_gui_bias = 0x89abcdefu,
           __obfh_gui_rotate = 17 };
    const ULONG_PTR values[] = {0, 1, (ULONG_PTR)-1, (ULONG_PTR)-2, (ULONG_PTR)1 << (sizeof(ULONG_PTR) * 8 - 1)};
    for (unsigned int i = 0; i < sizeof(values) / sizeof(values[0]); ++i) {
        for (unsigned int form = 0; form < 4; ++form) {
            ULONG_PTR x = values[i], key = OBFH_GUI_SITE_KEY, bias = (unsigned int)__obfh_gui_bias;
            ULONG_PTR __obfh_gui_value;
#define ROTATE(x) (((x) << 17) | ((x) >> (sizeof(ULONG_PTR) * 8 - 17)))
            if (form == 0) __obfh_gui_value = ROTATE(x ^ key) + bias;
            if (form == 1) __obfh_gui_value = ROTATE(x + bias) ^ key;
            if (form == 2) __obfh_gui_value = (ROTATE(x) ^ key) + bias;
            if (form == 3) __obfh_gui_value = ROTATE((x ^ key) + bias);
#undef ROTATE
            if (form == 0) OBFH_GUI_FORM_0(0);
            if (form == 1) OBFH_GUI_FORM_1(0);
            if (form == 2) OBFH_GUI_FORM_2(0);
            if (form == 3) OBFH_GUI_FORM_3(0);
            if (__obfh_gui_value != x) return 1;
        }
    }
    return 0;
}
static int cold_forms(void) {
    unsigned int forms = 0;
    const unsigned char name[] = "GetCurrentThreadId";
    for (unsigned int site = 0; site < 64; ++site) {
        OBFH_GUI_SLOT slot = {0};
        ULONG_PTR value = obfh_gui_cold(2, &slot, site, name, sizeof(name));
        ULONG_PTR key = OBFH_GUI_KEY(site), bias = OBFH_GUI_BIAS(site);
        unsigned int rotate = OBFH_GUI_ROTATE(site), form = OBFH_GUI_DRAW(site, 0x47554936u) & 3u;
        if (!slot.ready || (ULONG_PTR)slot.encoded != value) return 1;
        forms |= 1u << form;
        if (form == 0 || form == 2) value -= bias;
        if (form == 1 || form == 2) value ^= key;
        value = (value >> rotate) | (value << (sizeof(ULONG_PTR) * 8 - rotate));
        if (form == 1 || form == 3) value -= bias;
        if (form == 0 || form == 3) value ^= key;
        if (((DWORD(WINAPI *)(void))value)() != (GetCurrentThreadId)()) return 2;
    }
    return forms != 15u;
}
static DWORD WINAPI worker(void *unused) {
    for (unsigned int i = 0; i < 1000; ++i)
        if (thread_site() != (GetCurrentThreadId)()) return 1;
    return 0;
}
int main(void) {
    if (decode_forms() || cold_forms()) return 7;
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
