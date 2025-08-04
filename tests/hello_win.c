//+---------------------------------------------------------------------------
//
//  HELLO_WIN.C - Windows GUI 'Hello World!' Example
//
//+---------------------------------------------------------------------------

#include <windows.h>
#include "../include/obfus.h"

#define APPNAME "Turn around"

char szAppName[] = APPNAME;      // Application name
char szTitle[] = APPNAME;        // Title bar text
const char *pWindowText;         // Text to display in window

void CenterWindow(HWND hWnd);

//+---------------------------------------------------------------------------
//  WndProc - Handles window messages
//+---------------------------------------------------------------------------
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case WM_CREATE:
            CenterWindow(hwnd);
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        case WM_RBUTTONUP:           // Right mouse button releases
        case WM_KEYDOWN:
            if (message == WM_KEYDOWN && wParam != VK_ESCAPE)
                break;
            DestroyWindow(hwnd);
            break;

        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            RECT rc;
            GetClientRect(hwnd, &rc);
            SetTextColor(hdc, RGB(240, 240, 96));
            SetBkMode(hdc, TRANSPARENT);
            DrawText(hdc, pWindowText, -1, &rc, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
            EndPaint(hwnd, &ps);
            break;
        }

        default:
            return DefWindowProc(hwnd, message, wParam, lParam);
    }
    return 0;
}

//+---------------------------------------------------------------------------
//  WinMain - Program Entry Point
//+---------------------------------------------------------------------------
int APIENTRY WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nCmdShow
) {
    MSG msg;
    WNDCLASS wc = { 0 };
    HWND hwnd;

    // Use command line as window text, or default
    pWindowText = lpCmdLine[0] ? lpCmdLine : "DosX behind you!";

    // Set up window class
    wc.hInstance = hInstance;
    wc.lpszClassName = szAppName;
    wc.lpfnWndProc = WndProc;
    wc.style = CS_DBLCLKS | CS_VREDRAW | CS_HREDRAW;
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);

    if (!RegisterClass(&wc))
        return 0;

    // Create main window
    hwnd = CreateWindow(
        szAppName, szTitle,
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT,
        360, 240,         // Size: width, height
        NULL, NULL,
        hInstance, NULL
    );

    if (!hwnd)
        return 0;

    // Message loop
    while (GetMessage(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}

//+---------------------------------------------------------------------------
//  CenterWindow - Centers window on its parent or desktop
//+---------------------------------------------------------------------------
void CenterWindow(HWND hwnd_self) {
    HWND hwnd_parent = GetParent(hwnd_self);
    if (!hwnd_parent)
        hwnd_parent = GetDesktopWindow();

    RECT rw_self, rc_parent, rw_parent;
    GetWindowRect(hwnd_parent, &rw_parent);
    GetClientRect(hwnd_parent, &rc_parent);
    GetWindowRect(hwnd_self, &rw_self);

    int xpos = rw_parent.left + (rc_parent.right + rw_self.left - rw_self.right) / 2;
    int ypos = rw_parent.top + (rc_parent.bottom + rw_self.top - rw_self.bottom) / 2;

    SetWindowPos(
        hwnd_self, NULL,
        xpos, ypos, 0, 0,
        SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE
    );
}
