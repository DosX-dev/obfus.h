/* Shared colors, shape rendering and button motion. Injected after GUI state declarations. */
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
/* Use the already loaded GDI+ renderer for continuous curves, without raster resampling. */
typedef int(WINAPI *UiPathCreate)(int, void **);
typedef int(WINAPI *UiPathArc)(void *, float, float, float, float, float, float);
typedef int(WINAPI *UiPathClose)(void *);
typedef int(WINAPI *UiSolidCreate)(unsigned, void **);
typedef int(WINAPI *UiPathFill)(void *, void *, void *);
typedef int(WINAPI *UiGraphicsMode)(void *, int);
static UiPathCreate ui_path_create;
static UiPathArc ui_path_arc;
static UiPathClose ui_path_close;
static UiSolidCreate ui_solid_create;
static UiPathFill ui_path_fill;
static UiGraphicsMode ui_smoothing, ui_pixel_offset;
static GdipDisposeFn ui_path_delete, ui_brush_delete;
static int ui_vector_ready;
static int ui_vector_init(void) {
    if (ui_vector_ready) return ui_vector_ready > 0;
    if (!gdiplus) return 0;
    ui_path_create = (UiPathCreate)GetProcAddress(gdiplus, "GdipCreatePath");
    ui_path_arc = (UiPathArc)GetProcAddress(gdiplus, "GdipAddPathArc");
    ui_path_close = (UiPathClose)GetProcAddress(gdiplus, "GdipClosePathFigure");
    ui_solid_create = (UiSolidCreate)GetProcAddress(gdiplus, "GdipCreateSolidFill");
    ui_path_fill = (UiPathFill)GetProcAddress(gdiplus, "GdipFillPath");
    ui_smoothing = (UiGraphicsMode)GetProcAddress(gdiplus, "GdipSetSmoothingMode");
    ui_pixel_offset = (UiGraphicsMode)GetProcAddress(gdiplus, "GdipSetPixelOffsetMode");
    ui_path_delete = (GdipDisposeFn)GetProcAddress(gdiplus, "GdipDeletePath");
    ui_brush_delete = (GdipDisposeFn)GetProcAddress(gdiplus, "GdipDeleteBrush");
    ui_vector_ready = ui_path_create && ui_path_arc && ui_path_close && ui_solid_create && ui_path_fill && ui_smoothing && ui_pixel_offset && ui_path_delete && ui_brush_delete && graphics_from_hdc && dispose_graphics && gdiptoken ? 1 : -1;
    return ui_vector_ready > 0;
}
static void ui_contour(void *graphics, float x, float y, float w, float h, float diameter, COLORREF color) {
    void *path = 0, *brush = 0;
    unsigned argb = 0xff000000u | ((unsigned)GetRValue(color) << 16) | ((unsigned)GetGValue(color) << 8) | GetBValue(color);
    if (ui_path_create(0, &path)) return;
    ui_path_arc(path, x + w - diameter, y, diameter, diameter, -90.0f, 90.0f);
    ui_path_arc(path, x + w - diameter, y + h - diameter, diameter, diameter, 0.0f, 90.0f);
    ui_path_arc(path, x, y + h - diameter, diameter, diameter, 90.0f, 90.0f);
    ui_path_arc(path, x, y, diameter, diameter, 180.0f, 90.0f);
    ui_path_close(path);
    if (!ui_solid_create(argb, &brush)) {
        ui_path_fill(graphics, brush, path);
        ui_brush_delete(brush);
    }
    ui_path_delete(path);
}
static void rounded(HDC dc, int x, int y, int w, int h, int radius, COLORREF fill, COLORREF stroke) {
    void *graphics = 0;
    if (ui_vector_init() && !graphics_from_hdc(dc, &graphics)) {
        ui_smoothing(graphics, 4);
        ui_pixel_offset(graphics, 4);
        ui_contour(graphics, (float)x, (float)y, (float)w, (float)h, (float)radius, stroke);
        ui_contour(graphics, (float)x + 1.0f, (float)y + 1.0f, (float)w - 2.0f, (float)h - 2.0f, (float)radius - 2.0f, fill);
        dispose_graphics(graphics);
    } else {
        HBRUSH b = CreateSolidBrush(fill);
        HPEN p = CreatePen(PS_SOLID, 1, stroke);
        HGDIOBJ ob = SelectObject(dc, b), op = SelectObject(dc, p);
        RoundRect(dc, x, y, x + w, y + h, radius, radius);
        SelectObject(dc, ob);
        SelectObject(dc, op);
        DeleteObject(b);
        DeleteObject(p);
    }
}
static int check_press_mix, about_press_mix;
static void ui_press(HWND button) {
    int *mix = button == aboutbutton ? &about_press_mix : &check_press_mix;
    if (*mix < 96) *mix = 96;
}
static int ui_press_ease(int value, int target) {
    int delta = target - value;
    if (delta > -4 && delta < 4) return target;
    return value + delta * 3 / 4;
}
static int ui_button_tick(void) {
    int check_target = SendMessageA(checkbutton, BM_GETSTATE, 0, 0) & BST_PUSHED ? 256 : 0;
    int about_target = SendMessageA(aboutbutton, BM_GETSTATE, 0, 0) & BST_PUSHED ? 256 : 0;
    check_press_mix = ui_press_ease(check_press_mix, check_target);
    about_press_mix = ui_press_ease(about_press_mix, about_target);
    return check_press_mix == check_target && about_press_mix == about_target;
}
static LRESULT ui_draw_button(DRAWITEMSTRUCT *item) {
    int about = item->CtlID == 103, width = about ? 64 : 366, height = about ? 28 : 48;
    int press = about ? about_press_mix : check_press_mix;
    int focus = (item->itemState & ODS_FOCUS) && !(item->itemState & ODS_NOFOCUSRECT);
    HDC dc = CreateCompatibleDC(item->hDC);
    HBITMAP bitmap = CreateCompatibleBitmap(item->hDC, width, height);
    HGDIOBJ old = SelectObject(dc, bitmap);
    HBRUSH brush = CreateSolidBrush(about ? RGB(15, 18, 24) : UI_BG);
    COLORREF fill = about ? blend(RGB(27, 32, 42), RGB(41, 46, 61), about_mix) : blend(RGB(115, 108, 220), RGB(137, 131, 237), hover_mix);
    COLORREF stroke = about ? blend(RGB(42, 48, 61), RGB(65, 71, 90), about_mix) : blend(RGB(145, 138, 230), RGB(166, 160, 250), hover_mix);
    fill = blend(fill, about ? RGB(23, 27, 37) : RGB(96, 90, 193), press);
    stroke = blend(stroke, about ? RGB(57, 62, 79) : RGB(125, 118, 214), press);
    FillRect(dc, &item->rcItem, brush);
    DeleteObject(brush);
    rounded(dc, 0, 0, width, height, about ? 12 : 20, fill, about && focus ? RGB(126, 120, 215) : stroke);
    if (!about && focus) rounded(dc, 3, 3, width - 6, height - 6, 16, fill, RGB(199, 195, 255));
    SetBkMode(dc, TRANSPARENT);
    SetTextColor(dc, about ? blend(RGB(166, 176, 192), RGB(229, 233, 242), about_mix) : RGB(255, 255, 255));
    SelectObject(dc, about ? smallfont : bodyfont);
    DrawTextA(dc, about ? "About" : HIDE_STRING("Check password"), -1, &item->rcItem, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    BitBlt(item->hDC, 0, 0, width, height, dc, 0, 0, SRCCOPY);
    SelectObject(dc, old);
    DeleteObject(bitmap);
    DeleteDC(dc);
    return TRUE;
}

static LRESULT ui_draw_window(HWND window) {
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
