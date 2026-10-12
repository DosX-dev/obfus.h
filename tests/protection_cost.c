/* Timing loops stay native: measure each protected operation, not the timer. */
#include <stdio.h>
#include <windows.h>
static volatile ULONG_PTR cost_sink;
static void measure(const char *name, void (*operation)(void)) {
    LARGE_INTEGER frequency, begin, end;
    unsigned count = 1;
    double samples[5];
    QueryPerformanceFrequency(&frequency);
    operation();
    for (;;) {
        QueryPerformanceCounter(&begin);
        for (unsigned i = 0; i < count; ++i) operation();
        QueryPerformanceCounter(&end);
        if ((double)(end.QuadPart - begin.QuadPart) / frequency.QuadPart >= 0.02 || count >= 1048576) break;
        count *= 2;
    }
    for (unsigned sample = 0; sample < 5; ++sample) {
        QueryPerformanceCounter(&begin);
        for (unsigned i = 0; i < count; ++i) operation();
        QueryPerformanceCounter(&end);
        samples[sample] = 1e9 * (end.QuadPart - begin.QuadPart) / frequency.QuadPart / count;
    }
    for (unsigned i = 0; i < 5; ++i)
        for (unsigned j = i + 1; j < 5; ++j) {
            if (samples[j] < samples[i]) {
                double t = samples[i];
                samples[i] = samples[j];
                samples[j] = t;
            }
        }
    printf("COST %s %.3f %u\n", name, samples[2], count);
}
#define VIRT 1
#include "obfus.h"
static void protected_if_operation(void) {
    if (cost_sink & 1u)
        cost_sink += 3;
    else
        cost_sink ^= 5;
}
static void protected_do_operation(void) {
    do {
        cost_sink++;
    } while (0);
}
/* Header function bodies have already been compiled with their normal guards. */
#undef if
#undef else
#undef for
#undef while
#undef do
#undef break
#undef continue
#undef return
#undef switch
#undef default
static void empty_operation(void) { cost_sink++; }
static void flow_operation(void) {
    BREAK_STACK_CFLOW;
    cost_sink++;
}
static void stack_operation(void) {
    STACK_PROXY_FUNCTIONS;
    cost_sink++;
}
static void proxy_operation(void) { cost_sink = obfh_uintptr_proxy(cost_sink); }
static OBFH_CRT_ENTRY cache;
static void cache_operation(void) { cost_sink = (ULONG_PTR)obfh_crt_site("abs", &cache, 0x1571u); }
static void transport_operation(void) {
    OBFH_VM_VALUE encoded = obfh_vm_encode(37.25L, 119, 1);
    cost_sink = (ULONG_PTR)obfh_vm_decode(encoded, 119);
}
static void vm_operation(void) { cost_sink = VM_ADD(37, 19); }
static void string_operation(void) {
    char *text = HIDE_STRING("protected string");
    cost_sink = text[0];
}
static void length_operation(void) { cost_sink = strlen("short string"); }
static void debug_operation(void) { cost_sink = IsDebuggerPresent_proxy(); }
int main(void) {
    measure("call-baseline", empty_operation);
    measure("cflow", flow_operation);
    measure("stack-proxy", stack_operation);
    measure("automatic-if", protected_if_operation);
    measure("automatic-do-once", protected_do_operation);
    measure("pointer-transport", proxy_operation);
    measure("crt-cache-hit", cache_operation);
    measure("vm-value-roundtrip", transport_operation);
    measure("vm-add", vm_operation);
    measure("hidden-string", string_operation);
    measure("custom-strlen-12", length_operation);
    measure("antidebug-probe", debug_operation);
    return 0;
}
