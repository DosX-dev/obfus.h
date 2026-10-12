/* Shared player UI. Native controls keep keyboard, paste and accessibility semantics. */
typedef int(WINAPI *GdipStartupFn)(ULONG_PTR *, void *, void *);
typedef void(WINAPI *GdipShutdownFn)(ULONG_PTR);
typedef int(WINAPI *GdipFromStreamFn)(IStream *, void **);
typedef int(WINAPI *GdipFromHdcFn)(HDC, void **);
typedef int(WINAPI *GdipDrawFn)(void *, void *, int, int, int, int);
typedef int(WINAPI *GdipDisposeFn)(void *);
static HWND inputbox, statusbox, checkbutton, aboutbutton, mainwindow;
static HFONT bodyfont, titlefont, codefont, smallfont;
static HBRUSH background, editbackground;
static int verdict, hover, focused, animating;
static int hover_mix, focus_mix, result_mix, about_hover, about_mix;
static int paste_attempts;
static WNDPROC edit_proc, button_proc, about_proc;
static HMODULE gdiplus;
static ULONG_PTR gdiptoken;
static IStream *logostream;
static void *logoimage;
static GdipFromHdcFn graphics_from_hdc;
static GdipDrawFn draw_image;
static GdipDisposeFn dispose_image, dispose_graphics;
static GdipShutdownFn shutdown_gdip;
static COLORREF accent = DAILY_DIFFICULTY_COLOR;
#define UI_BG RGB(12, 15, 20)
#define UI_FIELD RGB(21, 26, 34)
static COLORREF blend(COLORREF a, COLORREF b, int amount) {
    return RGB((GetRValue(a) * (256 - amount) + GetRValue(b) * amount) / 256, (GetGValue(a) * (256 - amount) + GetGValue(b) * amount) / 256, (GetBValue(a) * (256 - amount) + GetBValue(b) * amount) / 256);
}
static void label(HDC dc, int x, int y, const char *s, COLORREF color, HFONT font) {
    SelectObject(dc, font);
    SetTextColor(dc, color);
    TextOutA(dc, x, y, s, lstrlenA(s));
}
static void rounded(HDC dc, int x, int y, int w, int h, int radius, COLORREF fill, COLORREF stroke) {
    HBRUSH b = CreateSolidBrush(fill);
    HPEN p = CreatePen(PS_SOLID, 1, stroke);
    HGDIOBJ ob = SelectObject(dc, b), op = SelectObject(dc, p);
    RoundRect(dc, x, y, x + w, y + h, radius, radius);
    SelectObject(dc, ob);
    SelectObject(dc, op);
    DeleteObject(b);
    DeleteObject(p);
}
static void wake_animation(void) {
    if (!animating) {
        animating = 1;
        SetTimer(mainwindow, 1, 16, 0);
    }
}
static int ease(int value, int target) {
    int delta = target - value;
    if (delta > -3 && delta < 3) return target;
    return value + delta / 4 + (delta > 0 ? 1 : -1);
}
static void paste_input(HWND window) {
    /* Clipboard viewers can briefly hold the global clipboard. Retry without blocking the UI. */
    if (!OpenClipboard(window)) {
        if (++paste_attempts < 20) SetTimer(window, 2, 15, 0);
        return;
    }
    KillTimer(window, 2);
    {
        HANDLE data = GetClipboardData(CF_UNICODETEXT);
        if (data) {
            const WCHAR *text = (const WCHAR *)GlobalLock(data);
            if (text) {
                SIZE_T capacity = GlobalSize(data) / sizeof(WCHAR), length = 0;
                while (length < capacity && text[length]) length++;
                if (length < capacity) SendMessageW(window, EM_REPLACESEL, TRUE, (LPARAM)text);
                GlobalUnlock(data);
            }
        }
    }
    CloseClipboard();
}
static LRESULT CALLBACK input_proc(HWND window, UINT message, WPARAM wp, LPARAM lp) {
    if (message == WM_CHAR || message == WM_SETTEXT) KillTimer(window, 2);
    if (message == WM_PASTE) {
        paste_attempts = 0;
        paste_input(window);
        return 0;
    }
    if (message == WM_TIMER && wp == 2) {
        KillTimer(window, 2);
        paste_input(window);
        return 0;
    }
    if (message == WM_SETFOCUS || message == WM_KILLFOCUS) {
        focused = message == WM_SETFOCUS;
        wake_animation();
    }
    return CallWindowProcW(edit_proc, window, message, wp, lp);
}
static LRESULT CALLBACK check_proc(HWND window, UINT message, WPARAM wp, LPARAM lp) {
    int is_about = window == aboutbutton;
    if (message == WM_ERASEBKGND) return 1;
    if (message == WM_MOUSEMOVE && !(is_about ? about_hover : hover)) {
        TRACKMOUSEEVENT track = {sizeof(track), TME_LEAVE, window, 0};
        if (is_about)
            about_hover = 1;
        else
            hover = 1;
        TrackMouseEvent(&track);
        wake_animation();
    }
    if (message == WM_MOUSELEAVE) {
        if (is_about)
            about_hover = 0;
        else
            hover = 0;
        wake_animation();
    }
    if (message == WM_SETFOCUS || message == WM_KILLFOCUS || message == WM_LBUTTONDOWN || message == WM_LBUTTONUP) InvalidateRect(window, 0, FALSE);
    return CallWindowProcA(is_about ? about_proc : button_proc, window, message, wp, lp);
}
static void load_logo(HINSTANCE instance) {
    struct {
        UINT version;
        void *callback;
        BOOL suppress_thread;
        BOOL suppress_codecs;
    } startup = {1, 0, 0, 0};
    HRSRC resource = FindResourceA(instance, MAKEINTRESOURCE(101), RT_RCDATA);
    DWORD size;
    HGLOBAL memory;
    void *data, *locked;
    GdipStartupFn start;
    GdipFromStreamFn from_stream;
    if (!resource) return;
    size = SizeofResource(instance, resource);
    data = LockResource(LoadResource(instance, resource));
    if (!data || !size) return;
    gdiplus = LoadLibraryA(HIDE_STRING("gdiplus"));
    if (!gdiplus) return;
    start = (GdipStartupFn)GetProcAddress(gdiplus, "GdiplusStartup");
    shutdown_gdip = (GdipShutdownFn)GetProcAddress(gdiplus, "GdiplusShutdown");
    from_stream = (GdipFromStreamFn)GetProcAddress(gdiplus, "GdipCreateBitmapFromStream");
    graphics_from_hdc = (GdipFromHdcFn)GetProcAddress(gdiplus, "GdipCreateFromHDC");
    draw_image = (GdipDrawFn)GetProcAddress(gdiplus, "GdipDrawImageRectI");
    dispose_image = (GdipDisposeFn)GetProcAddress(gdiplus, "GdipDisposeImage");
    dispose_graphics = (GdipDisposeFn)GetProcAddress(gdiplus, "GdipDeleteGraphics");
    if (!start || !shutdown_gdip || !from_stream || !graphics_from_hdc || !draw_image || !dispose_image || !dispose_graphics) return;
    if (start(&gdiptoken, &startup, 0)) {
        gdiptoken = 0;
        return;
    }
    memory = GlobalAlloc(GMEM_MOVEABLE, size);
    if (!memory) return;
    locked = GlobalLock(memory);
    if (!locked) {
        GlobalFree(memory);
        return;
    }
    memcpy(locked, data, size);
    GlobalUnlock(memory);
    if (CreateStreamOnHGlobal(memory, TRUE, &logostream)) {
        GlobalFree(memory);
        return;
    }
    from_stream(logostream, &logoimage);
}
static void check_password(HWND window) {
    char input[1024];
    WCHAR unicode_input[1024];
    int length = GetWindowTextLengthW(inputbox);
    if (length != PASSWORD_LENGTH)
        verdict = 2;
    else {
        int i, valid = GetWindowTextW(inputbox, unicode_input, 1024) == length;
        for (i = 0; valid && i < length; i++) {
            if (unicode_input[i] < 33 || unicode_input[i] > 126) valid = 0;
            input[i] = (char)unicode_input[i];
        }
        input[length] = 0;
        ANTI_DEBUG;
        verdict = valid && verify(input) ? 1 : 2;
    }
    {
        volatile char *wipe = input;
        unsigned i;
        for (i = 0; i < sizeof(input); i++) wipe[i] = 0;
        wipe = (volatile char *)unicode_input;
        for (i = 0; i < sizeof(unicode_input); i++) wipe[i] = 0;
    }
    SetWindowTextA(statusbox, verdict == 1 ? HIDE_STRING("Accepted.") : HIDE_STRING("Incorrect password."));
    result_mix = 0;
    wake_animation();
    InvalidateRect(window, 0, FALSE);
}
static LRESULT CALLBACK window_proc(HWND window, UINT message, WPARAM wp, LPARAM lp) {
    switch (message) {
        case WM_CREATE: {
            HINSTANCE instance = ((CREATESTRUCT *)lp)->hInstance;
            mainwindow = window;
            bodyfont = CreateFontA(-15, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, 0, 0, CLEARTYPE_QUALITY, 0, "Segoe UI");
            smallfont = CreateFontA(-12, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, 0, 0, CLEARTYPE_QUALITY, 0, "Segoe UI");
            titlefont = CreateFontA(-32, 0, 0, 0, FW_SEMIBOLD, 0, 0, 0, DEFAULT_CHARSET, 0, 0, CLEARTYPE_QUALITY, 0, "Segoe UI");
            codefont = CreateFontA(-19, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, 0, 0, CLEARTYPE_QUALITY, 0, "Consolas");
            background = CreateSolidBrush(UI_BG);
            editbackground = CreateSolidBrush(UI_FIELD);
            inputbox = CreateWindowExW(0, L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_PASSWORD | ES_AUTOHSCROLL, 364, 211, 330, 25, window, (HMENU)100, instance, 0);
            SendMessageA(inputbox, WM_SETFONT, (WPARAM)codefont, TRUE);
            SendMessageA(inputbox, EM_SETLIMITTEXT, 1023, 0);
            SendMessageA(inputbox, EM_SETMARGINS, EC_LEFTMARGIN | EC_RIGHTMARGIN, MAKELPARAM(0, 0));
            edit_proc = (WNDPROC)SetWindowLongPtrW(inputbox, GWLP_WNDPROC, (LONG_PTR)input_proc);
            checkbutton = CreateWindowExA(0, "BUTTON", "Check", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_OWNERDRAW, 346, 260, 366, 48, window, (HMENU)101, instance, 0);
            button_proc = (WNDPROC)SetWindowLongPtrA(checkbutton, GWLP_WNDPROC, (LONG_PTR)check_proc);
            /* A native status control retains its accessible text; its presentation is painted below. */
            statusbox = CreateWindowExA(0, "STATIC", "", WS_CHILD, 346, 308, 366, 24, window, (HMENU)102, instance, 0);
            aboutbutton = CreateWindowExA(0, "BUTTON", "About", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_OWNERDRAW, 648, 414, 64, 28, window, (HMENU)103, instance, 0);
            about_proc = (WNDPROC)SetWindowLongPtrA(aboutbutton, GWLP_WNDPROC, (LONG_PTR)check_proc);
            load_logo(instance);
            SetFocus(inputbox);
            return 0;
        }
        case WM_TIMER:
            if (wp == 1) {
                hover_mix = ease(hover_mix, hover ? 256 : 0);
                focus_mix = ease(focus_mix, focused ? 256 : 0);
                result_mix = ease(result_mix, verdict ? 256 : 0);
                about_mix = ease(about_mix, about_hover ? 256 : 0);
                {
                    RECT r = {342, 194, 718, 351};
                    InvalidateRect(window, &r, FALSE);
                }
                InvalidateRect(checkbutton, 0, FALSE);
                InvalidateRect(aboutbutton, 0, FALSE);
                if (hover_mix == (hover ? 256 : 0) && focus_mix == (focused ? 256 : 0) && result_mix == (verdict ? 256 : 0) && about_mix == (about_hover ? 256 : 0)) {
                    KillTimer(window, 1);
                    animating = 0;
                }
            }
            return 0;
        case WM_CTLCOLOREDIT:
            SetTextColor((HDC)wp, RGB(238, 242, 247));
            SetBkColor((HDC)wp, UI_FIELD);
            return (LRESULT)editbackground;
        case WM_COMMAND:
            if (LOWORD(wp) == 103 && HIWORD(wp) == BN_CLICKED) {
                if (MessageBoxExA(window, HIDE_STRING("Part of the obfus.h reverse-engineering gamification program.\n\nRecover the original password. Patching the success message does not count.\n\nNew rounds daily at 13:00 Moscow time (UTC+3). Publication may be delayed.\n\nWould you like to visit https://github.com/DosX-dev/obfus.h?"), HIDE_STRING("About Daily Crackme"), MB_YESNO | MB_ICONINFORMATION | MB_DEFBUTTON2, MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US)) == IDYES) {
                    ShellExecuteA(window, HIDE_STRING("open"), HIDE_STRING("https://github.com/DosX-dev/obfus.h"), 0, 0, SW_SHOWNORMAL);
                }
                return 0;
            }
            if (LOWORD(wp) == 101 && HIWORD(wp) == BN_CLICKED) {
                check_password(window);
                return 0;
            }
            if (LOWORD(wp) == 100 && HIWORD(wp) == EN_CHANGE && verdict) {
                verdict = 0;
                SetWindowTextA(statusbox, "");
                wake_animation();
                InvalidateRect(window, 0, FALSE);
            }
            break;
        case WM_DRAWITEM: {
            if (((DRAWITEMSTRUCT *)lp)->CtlID == 103) {
                DRAWITEMSTRUCT *about = (DRAWITEMSTRUCT *)lp;
                HDC dc = CreateCompatibleDC(about->hDC);
                HBITMAP bitmap = CreateCompatibleBitmap(about->hDC, 64, 28);
                HGDIOBJ old = SelectObject(dc, bitmap);
                HBRUSH brush = CreateSolidBrush(RGB(15, 18, 24));
                COLORREF fill = blend(RGB(27, 32, 42), RGB(41, 46, 61), about_mix);
                FillRect(dc, &about->rcItem, brush);
                DeleteObject(brush);
                if (about->itemState & ODS_SELECTED) fill = RGB(23, 27, 37);
                rounded(dc, 0, 0, 64, 28, 12, fill, (about->itemState & ODS_FOCUS) ? RGB(126, 120, 215) : blend(RGB(42, 48, 61), RGB(65, 71, 90), about_mix));
                SetBkMode(dc, TRANSPARENT);
                SetTextColor(dc, blend(RGB(166, 176, 192), RGB(229, 233, 242), about_mix));
                SelectObject(dc, smallfont);
                DrawTextA(dc, "About", -1, &about->rcItem, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
                BitBlt(about->hDC, 0, 0, 64, 28, dc, 0, 0, SRCCOPY);
                SelectObject(dc, old);
                DeleteObject(bitmap);
                DeleteDC(dc);
                return TRUE;
            }
            /* Paint the complete button offscreen. Parent paints exclude child windows. */
            DRAWITEMSTRUCT *d = (DRAWITEMSTRUCT *)lp;
            HDC dc = CreateCompatibleDC(d->hDC);
            HBITMAP bitmap = CreateCompatibleBitmap(d->hDC, 366, 48);
            HGDIOBJ old = SelectObject(dc, bitmap);
            COLORREF fill = blend(RGB(115, 108, 220), RGB(137, 131, 237), hover_mix);
            if (d->itemState & ODS_SELECTED) fill = RGB(96, 90, 193);
            FillRect(dc, &d->rcItem, background);
            rounded(dc, 0, 0, 366, 48, 20, fill, fill);
            SetBkMode(dc, TRANSPARENT);
            SetTextColor(dc, RGB(255, 255, 255));
            SelectObject(dc, bodyfont);
            DrawTextA(dc, HIDE_STRING("Check password"), -1, &d->rcItem, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
            if (d->itemState & ODS_FOCUS) {
                HPEN p = CreatePen(PS_SOLID, 1, RGB(199, 195, 255));
                HGDIOBJ op = SelectObject(dc, p), ob = SelectObject(dc, GetStockObject(NULL_BRUSH));
                RoundRect(dc, 3, 3, 363, 45, 16, 16);
                SelectObject(dc, op);
                SelectObject(dc, ob);
                DeleteObject(p);
            }
            BitBlt(d->hDC, 0, 0, 366, 48, dc, 0, 0, SRCCOPY);
            SelectObject(dc, old);
            DeleteObject(bitmap);
            DeleteDC(dc);
            return TRUE;
        }
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC screen = BeginPaint(window, &ps), dc = CreateCompatibleDC(screen);
            HBITMAP bitmap = CreateCompatibleBitmap(screen, 760, 460);
            HGDIOBJ old = SelectObject(dc, bitmap);
            RECT area = {0, 0, 760, 460};
            void *graphics = 0;
            COLORREF ink = RGB(233, 237, 243), muted = RGB(137, 148, 165);
            FillRect(dc, &area, background);
            SetBkMode(dc, TRANSPARENT);
            if (logoimage && !graphics_from_hdc(dc, &graphics)) {
                draw_image(graphics, logoimage, 8, 42, 320, 320);
                dispose_graphics(graphics);
            }
            {
                const char *difficulty = HIDE_STRING("@LEVEL@");
                SIZE size;
                HGDIOBJ previous = SelectObject(dc, smallfont);
                GetTextExtentPoint32A(dc, difficulty, (int)strlen(difficulty), &size);
                SelectObject(dc, previous);
                rounded(dc, 346, 54, size.cx + 16, 26, 12, blend(UI_BG, accent, 18), blend(UI_BG, accent, 65));
                label(dc, 354, 54 + (26 - size.cy) / 2, difficulty, accent, smallfont);
            }
            label(dc, 346, 87, HIDE_STRING("Daily Crackme"), ink, titlefont);
            label(dc, 346, 136, HIDE_STRING("Recover the original password."), muted, bodyfont);
            label(dc, 346, 179, HIDE_STRING("Password"), RGB(175, 185, 199), smallfont);
            {
                char lengthtext[64];
                sprintf(lengthtext, HIDE_STRING("%d ASCII characters"), PASSWORD_LENGTH);
                label(dc, 604, 179, lengthtext, RGB(103, 116, 134), smallfont);
            }
            rounded(dc, 346, 199, 366, 48, 20, UI_FIELD, blend(RGB(38, 45, 57), RGB(126, 120, 215), focus_mix));
            if (verdict) {
                char status[64];
                COLORREF result = verdict == 1 ? RGB(103, 217, 164) : RGB(239, 130, 137);
                GetWindowTextA(statusbox, status, sizeof(status));
                label(dc, 346, 324, status, blend(UI_BG, result, result_mix), bodyfont);
            }
            {
                RECT footer = {0, 404, 760, 460};
                HBRUSH brush = CreateSolidBrush(RGB(15, 18, 24));
                FillRect(dc, &footer, brush);
                DeleteObject(brush);
            }
            {
                HPEN pen = CreatePen(PS_SOLID, 1, RGB(32, 37, 47));
                HGDIOBJ oldpen = SelectObject(dc, pen);
                MoveToEx(dc, 0, 404, 0);
                LineTo(dc, 760, 404);
                SelectObject(dc, oldpen);
                DeleteObject(pen);
            }
            label(dc, 32, 422, HIDE_STRING("Round @ROUND@  /  @DATE@"), RGB(115, 126, 144), smallfont);
            label(dc, 346, 422, HIDE_STRING("Daily at 13:00 Moscow (UTC+3)"), RGB(115, 126, 144), smallfont);
            BitBlt(screen, 0, 0, 760, 460, dc, 0, 0, SRCCOPY);
            SelectObject(dc, old);
            DeleteObject(bitmap);
            DeleteDC(dc);
            EndPaint(window, &ps);
            return 0;
        }
        case WM_ERASEBKGND:
            return 1;
        case WM_DESTROY:
            KillTimer(window, 1);
            if (logoimage) dispose_image(logoimage);
            if (logostream) logostream->lpVtbl->Release(logostream);
            if (gdiptoken) shutdown_gdip(gdiptoken);
            if (gdiplus) FreeLibrary(gdiplus);
            DeleteObject(bodyfont);
            DeleteObject(titlefont);
            DeleteObject(codefont);
            DeleteObject(smallfont);
            DeleteObject(background);
            DeleteObject(editbackground);
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProcA(window, message, wp, lp);
}
int WINAPI WinMain(HINSTANCE instance, HINSTANCE previous, LPSTR command, int show) {
    WNDCLASSA cls = {0};
    HWND window;
    MSG msg;
    RECT area = {0, 0, 760, 460};
    DWORD style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_CLIPCHILDREN;
    (void)previous;
    (void)command;
    SetProcessDPIAware();
    ANTI_DEBUG;
    cls.lpfnWndProc = window_proc;
    cls.hInstance = instance;
    cls.hCursor = LoadCursorA(0, IDC_ARROW);
    cls.lpszClassName = "ObfusDailyCrackme";
    if (!RegisterClassA(&cls)) return 1;
    AdjustWindowRectEx(&area, style, FALSE, WS_EX_DLGMODALFRAME);
    {
        POINT cursor = {0, 0};
        MONITORINFO monitor = {sizeof(MONITORINFO)};
        RECT work = {0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN)};
        int width = area.right - area.left, height = area.bottom - area.top, x, y;
        GetCursorPos(&cursor);
        if (GetMonitorInfoA(MonitorFromPoint(cursor, MONITOR_DEFAULTTOPRIMARY), &monitor))
            work = monitor.rcWork;
        else
            SystemParametersInfoA(SPI_GETWORKAREA, 0, &work, 0);
        x = work.left + (work.right - work.left - width) / 2;
        y = work.top + (work.bottom - work.top - height) / 2;
        if (x < work.left) x = work.left;
        if (y < work.top) y = work.top;
        window = CreateWindowExA(WS_EX_DLGMODALFRAME, cls.lpszClassName, HIDE_STRING("Daily Crackme"), style, x, y, width, height, 0, 0, instance, 0);
    }
    if (!window) return 1;
    {
        WCHAR title[80];
        MultiByteToWideChar(CP_UTF8, 0, HIDE_STRING("Daily Crackme \xE2\x80\x94 protected with obfus.h"), -1, title, 80);
        SetWindowTextW(window, title);
    }
    /* Optional system decoration; unsupported attributes leave the native caption intact. */
    {
        HMODULE dwm = LoadLibraryA("dwmapi");
        if (dwm) {
            typedef HRESULT(WINAPI * DwmAttributeFn)(HWND, DWORD, const void *, DWORD);
            DwmAttributeFn attribute = (DwmAttributeFn)GetProcAddress(dwm, "DwmSetWindowAttribute");
            if (attribute) {
                BOOL dark = TRUE;
                COLORREF caption = UI_BG, text = RGB(233, 237, 243);
                if (attribute(window, 20, &dark, sizeof(dark)) < 0) attribute(window, 19, &dark, sizeof(dark));
                attribute(window, 35, &caption, sizeof(caption));
                attribute(window, 36, &text, sizeof(text));
            }
            FreeLibrary(dwm);
        }
    }
    ShowWindow(window, show);
    UpdateWindow(window);
    while (GetMessageW(&msg, 0, 0, 0) > 0) {
        if (msg.message == WM_KEYDOWN && msg.wParam == VK_RETURN) {
            if (msg.hwnd == aboutbutton)
                SendMessageA(window, WM_COMMAND, MAKEWPARAM(103, BN_CLICKED), (LPARAM)aboutbutton);
            else
                check_password(window);
            continue;
        }
        if (!IsDialogMessageW(window, &msg)) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }
    return (int)msg.wParam;
}
