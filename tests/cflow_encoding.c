#include "../include/obfus.h"
#undef if
#undef else
#undef return
#undef printf

/* Check both MOV encodings for every destination used by CFLOW. */
#define CHECK_INPUT(name, reg, index, key) \
    static int name(void) { \
        unsigned int stack, actual; \
        __asm__ __volatile__("movl %%esp, %[stack]; " \
                             OBFH_CFLOW_INPUT(reg, index) \
                             "movl " reg ", %[actual];" \
                             : [stack] "=m"(stack), [actual] "=m"(actual) \
                             : [junk_key] "i"(key), \
                               [junk_salt] "i"(123), [junk_rotate] "i"(11) \
                             : "eax", "ecx", "edx", "cc"); \
        unsigned int value = stack ^ 123u; \
        return actual == ((value << 11) | (value >> 21)); \
    }

CHECK_INPUT(eax_store, "%%eax", "0", 0)
CHECK_INPUT(eax_load, "%%eax", "0", 32)
CHECK_INPUT(ecx_store, "%%ecx", "1", 0)
CHECK_INPUT(ecx_load, "%%ecx", "1", 32)
CHECK_INPUT(edx_store, "%%edx", "2", 0)
CHECK_INPUT(edx_load, "%%edx", "2", 32)

int main(void) {
    for (unsigned int i = 0; i < 256; ++i)
        if (!eax_store() || !eax_load() || !ecx_store() || !ecx_load() ||
            !edx_store() || !edx_load()) return 1;
    (printf)("CFLOW_ENCODING_PASS\n");
    return 0;
}
