#include "TreeCtl.h"

#include <math.h>

#define PI      3.14159265359
#define RAD(X)  X * PI / 180.0

typedef struct tagUSERDATA {
    HDC     hMemDC ;
    HBITMAP hBitmap ;
    HPEN    hPen ;
    int     cxClient ;
    int     cyClient ;
    double  offset ;
    double  length ;
    double  angle ;
} USERDATA ;

static void DrawTree (USERDATA* pUser, double xPos, double yPos, double angle, int step) {
    if (step > 0) {
        double xNew = round (xPos + cos (angle) * step * pUser->length) ;
        double yNew = round (yPos - sin (angle) * step * pUser->length) ;
        MoveToEx (pUser->hMemDC, xPos, yPos, NULL) ;
        LineTo (pUser->hMemDC, xNew, yNew) ;
        MoveToEx (pUser->hMemDC, xPos, yPos, NULL) ;
        DrawTree (pUser, xNew, yNew, angle - 0.349066 - pUser->angle, step - 1) ;
        DrawTree (pUser, xNew, yNew, angle + 0.349066 - pUser->angle, step - 1) ;
    }
}

static HBITMAP SaveClientToBitmap (HWND hwnd) {
    USERDATA* pUser = GetWindowLongPtr (hwnd, GWLP_USERDATA) ;
    HDC       hWinDC ;
    HBITMAP   hBitmap, hOldBmp ;
    hWinDC  = GetDC (hwnd) ;
    hBitmap = CreateCompatibleBitmap (hWinDC, pUser->cxClient, pUser->cyClient) ;
    hOldBmp = SelectObject (pUser->hMemDC, hBitmap) ;
    BitBlt (pUser->hMemDC, 0, 0, pUser->cxClient, pUser->cyClient, hWinDC, 0, 0, SRCCOPY) ;
    SelectObject (pUser->hMemDC, hOldBmp) ;
    ReleaseDC (hwnd, hWinDC) ;
    return hBitmap ;
}

static LRESULT OnCreate (HWND hwnd) {
    USERDATA* pUser ;
    HDC       hWinDC ;
    RECT      rect ;

    pUser  = HeapAlloc (GetProcessHeap (), HEAP_GENERATE_EXCEPTIONS, sizeof (USERDATA)) ;
    hWinDC = GetDC (hwnd) ;
    GetClientRect (hwnd, &rect) ;
    
    pUser->cxClient = rect.right - rect.left ;
    pUser->cyClient = rect.bottom - rect.top ;
    pUser->hMemDC   = CreateCompatibleDC (hWinDC) ;
    pUser->hBitmap  = CreateCompatibleBitmap (hWinDC, pUser->cxClient, pUser->cyClient) ;
    pUser->hPen     = GetStockObject (BLACK_PEN) ;
    pUser->offset   = 0.0 ;
    pUser->length   = 10.0 / 4.5 ;
    pUser->angle    = 0.0 ;

    ReleaseDC (hwnd, hWinDC) ;
    SetWindowLongPtr (hwnd, GWLP_USERDATA, pUser) ;

    return 0 ;
}

static LRESULT OnDestroy (HWND hwnd) {
    USERDATA* pUser = GetWindowLongPtr (hwnd, GWLP_USERDATA) ;
    DeleteObject (pUser->hPen) ;
    DeleteObject (pUser->hBitmap) ;
    DeleteDC (pUser->hMemDC) ;
    HeapFree (GetProcessHeap (), 0, pUser) ;
    return 0 ;
}

static LRESULT OnPaint (HWND hwnd) {
    USERDATA*   pUser = GetWindowLongPtr (hwnd, GWLP_USERDATA) ;
    HDC         hWinDC ;
    HGDIOBJ     hOldBmp, hOldPen ;
    PAINTSTRUCT paint ;

    hWinDC  = BeginPaint (hwnd, &paint) ;
    hOldBmp = SelectObject (pUser->hMemDC, pUser->hBitmap) ;
    hOldPen = SelectObject (pUser->hMemDC, pUser->hPen) ;

    BitBlt (pUser->hMemDC, 0, 0, pUser->cxClient, pUser->cyClient, pUser->hMemDC, 0, 0, WHITENESS) ;
    DrawTree (pUser, pUser->cxClient / 2.0 + pUser->offset, pUser->cyClient, RAD (90.0), 12) ;
    BitBlt (hWinDC, 0, 0, pUser->cxClient, pUser->cyClient, pUser->hMemDC, 0, 0, SRCCOPY) ;

    SelectObject (pUser->hMemDC, hOldPen) ;
    SelectObject (pUser->hMemDC, hOldBmp) ;
    EndPaint (hwnd, &paint) ;

    return 0 ;
}

static LRESULT OnSetParam (HWND hwnd, WPARAM wParam, LPARAM lParam) {
    USERDATA* pUser = GetWindowLongPtr (hwnd, GWLP_USERDATA) ;
    switch (LOWORD (wParam)) {
    case TC_OFFSET:
        pUser->offset = lParam * 4.5 ;
        InvalidateRect (hwnd, NULL, FALSE) ;
        return 0 ;
    case TC_LENGTH:
        pUser->length = lParam / 4.5 ;
        InvalidateRect (hwnd, NULL, FALSE) ;
        return 0 ;
    case TC_ANGLE:
        pUser->angle = RAD (lParam * 2.0) ;
        InvalidateRect (hwnd, NULL, FALSE) ;
        return 0 ;
    }
}

static LRESULT OnSetColor (HWND hwnd, LPARAM lParam) {
    USERDATA* pUser = GetWindowLongPtr (hwnd, GWLP_USERDATA) ;
    DeleteObject (pUser->hPen) ;
    pUser->hPen = CreatePen (PS_SOLID, 0, lParam) ;
    InvalidateRect (hwnd, NULL, FALSE) ;
    return 0 ;
}

static LRESULT OnClipboard (HWND hwnd) {
    HBITMAP hBitmap = SaveClientToBitmap (hwnd) ;
    OpenClipboard (hwnd) ;
    EmptyClipboard () ;
    SetClipboardData (CF_BITMAP, hBitmap) ;
    CloseClipboard () ;
    return 0 ;
}

static LRESULT CALLBACK WndProc (HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_CREATE:
        return OnCreate (hwnd) ;
    case WM_DESTROY:
        return OnDestroy (hwnd) ;
    case WM_PAINT:
        return OnPaint (hwnd) ;
    case TCM_SETPARAM:
        return OnSetParam (hwnd, wParam, lParam) ;
    case TCM_SETCOLOR:
        return OnSetColor (hwnd, lParam) ;
    case TCM_CLIPBOARD:
        return OnClipboard (hwnd) ;
    default:
        return DefWindowProc (hwnd, uMsg, wParam, lParam) ;
    }
}

ATOM RegisterTreeClass (HINSTANCE hInstance) {
    const WNDCLASS wndclass = {
        .style         = 0,
        .lpfnWndProc   = WndProc,
        .cbClsExtra    = 0,
        .cbWndExtra    = 0,
        .hInstance     = hInstance,
        .hIcon         = NULL,
        .hCursor       = LoadCursor (NULL, IDC_CROSS),
        .hbrBackground = NULL,
        .lpszMenuName  = NULL,
        .lpszClassName = TREE_CLASS
    };
    return RegisterClass (&wndclass) ;
}
