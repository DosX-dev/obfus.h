#include <stdio.h>
#include <windows.h>

#include "../include/obfus.h"
static int paints, creates, destroys;
static LRESULT CALLBACK procedure(HWND hwnd, UINT message, WPARAM w, LPARAM l) {
    switch (message) {
        case WM_CREATE:
            creates++;
            return 0;
        case WM_PAINT: {
            PAINTSTRUCT ps;
            RECT rect;
            HDC dc = BeginPaint(hwnd, &ps);
            GetClientRect(hwnd, &rect);
            DrawTextA(dc, "headless", -1, &rect, DT_CENTER);
            EndPaint(hwnd, &ps);
            paints++;
            return 0;
        }
        case WM_DESTROY:
            destroys++;
            return 0;
        default:
            return DefWindowProcA(hwnd, message, w, l);
    }
}
int main(void) {
    WNDCLASSA wc = {0};
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpfnWndProc = procedure;
    wc.lpszClassName = "obfh_headless_test";
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    if (!RegisterClassA(&wc)) return 1;
    HWND window = CreateWindowExA(0, wc.lpszClassName, "headless", WS_OVERLAPPEDWINDOW, 0, 0, 360, 240, NULL, NULL, wc.hInstance, NULL);
    if (!window || creates != 1) return 2;
    RECT rect;
    if (!GetWindowRect(window, &rect) || !GetClientRect(window, &rect)) return 3;
    if (!SetWindowPos(window, NULL, 100, 100, 320, 200, SWP_NOACTIVATE | SWP_NOZORDER)) return 4;
    SendMessageA(window, WM_PAINT, 0, 0);
    if (paints != 1 || GetParent(window) != NULL) return 5;
    SendMessageA(window, WM_CLOSE, 0, 0);
    if (destroys != 1) return 6;
    if (!UnregisterClassA(wc.lpszClassName, wc.hInstance)) return 7;
    puts("WINDOW_PASS");
    return 0;
}
