#include <windows.h>
static HANDLE delay_entered, delay_finish;
static BOOL delayed_signal(HANDLE event) {
    if (delay_entered) {
        SetEvent(delay_entered);
        if (WaitForSingleObject(delay_finish, 10000) != WAIT_OBJECT_0) return FALSE;
    }
    return SetEvent(event);
}
static DWORD short_wait(HANDLE event, DWORD timeout) {
    return WaitForSingleObject(event, timeout == 5000 ? 50 : timeout);
}
#define SetEvent delayed_signal
#define WaitForSingleObject short_wait
#include "../include/obfus.h"
__declspec(dllexport) int pool_probe(void) { return obfh_ad_register_probe(); }
__declspec(dllexport) void pool_delay(HANDLE entered, HANDLE finish) {
    delay_entered = entered;
    delay_finish = finish;
}
