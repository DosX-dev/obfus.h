#define VIRT 1
#include "../include/obfus.h"
#undef if
#undef else
#undef printf
#undef puts

#if EXPECT_EDITOR != OBFH_EDITOR_VIEW
#error Editor view leaked into a real compiler or was not selected for the editor
#endif

static int value;
static int next(void) { return ++value; }

int main(void) {
    typedef char IntegerResult[__builtin_types_compatible_p(__typeof__(VM_ADD(1, 2)), long) ? 1 : -1];
    typedef char FloatingResult[__builtin_types_compatible_p(__typeof__(VM_ADD_DBL(1, 2)), long double) ? 1 : -1];
    typedef char IdentityResult[__builtin_types_compatible_p(__typeof__(VM_OBF_DBL(1.0L)), long double) ? 1 : -1];
    if (VM_ADD(next(), 2) != 3 || value != 1) return 1;
    if (memcmp_custom("abc", "abc", 3) || strcmp_custom("abc", "abc")) return 2;
    if (strlen_custom(HIDE_STRING("editor")) != 6) return 3;
    if (VM_NOT(0) != 0xffffffffu || VM_SHL(1, 31) != 0x80000000u || VM_SHR(-1, 31) != 1u) return 4;
    BREAK_STACK_CFLOW;
    STACK_PROXY_FUNCTIONS;
    PHANTOM_NOP;
    puts("EDITOR_VIEW_PASS");
    return 0;
}
