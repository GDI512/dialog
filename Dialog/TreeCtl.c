#include "TreeCtl.h"

#include <math.h>

#define PI      3.14159265359f
#define RAD(X)  X * PI / 180.0f

typedef struct tagUSERDATA {
    HDC     hMemDC ;
    HBITMAP hBitmap ;
    HPEN    hPen ;
    int     cxClient ;
    int     cyClient ;
    float   offset ;
    float   length ;
    float   angle ;
} USERDATA ;

static void DrawTree (USERDATA* pUser, float xPos, float yPos, float angle, int step) {
    if (step > 0) {
        float xNew = round (xPos + cos (angle) * step * pUser->length) ;
        float yNew = round (yPos - sin (angle) * step * pUser->length) ;
        MoveToEx (pUser->hMemDC, xPos, yPos, NULL) ;
        LineTo (pUser->hMemDC, xNew, yNew) ;
        MoveToEx (pUser->hMemDC, xPos, yPos, NULL) ;
        DrawTree (pUser, xNew, yNew, angle - 0.349066f - pUser->angle, step - 1) ;
        DrawTree (pUser, xNew, yNew, angle + 0.349066f - pUser->angle, step - 1) ;
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

static LRESULT OnCreate (HWND hwnd, WPARAM wParam, LPARAM lParam) {
    USERDATA* pUser ;
    HDC       hWinDC ;
    pUser = HeapAlloc (GetProcessHeap (), HEAP_ZERO_MEMORY, sizeof (USERDATA)) ;
    SetWindowLongPtr (hwnd, GWLP_USERDATA, pUser) ;
    hWinDC = GetDC (hwnd) ;
    pUser->hMemDC = CreateCompatibleDC (hWinDC) ;
    pUser->hPen = GetStockObject (BLACK_PEN) ;
    ReleaseDC (hwnd, hWinDC) ;
    return 0 ;
}

static LRESULT OnDestroy (HWND hwnd, WPARAM wParam, LPARAM lParam) {
    USERDATA* pUser = GetWindowLongPtr (hwnd, GWLP_USERDATA) ;
    DeleteObject (pUser->hPen) ;
    DeleteObject (pUser->hBitmap) ;
    DeleteDC (pUser->hMemDC) ;
    HeapFree (GetProcessHeap (), 0, pUser) ;
    return 0 ;
}

static LRESULT OnSize (HWND hwnd, WPARAM wParam, LPARAM lParam) {
    USERDATA* pUser = GetWindowLongPtr (hwnd, GWLP_USERDATA) ;
    HDC       hWinDC ;
    pUser->cxClient = LOWORD (lParam) ;
    pUser->cyClient = HIWORD (lParam) ;
    hWinDC = GetDC (hwnd) ;
    DeleteObject (pUser->hBitmap) ;
    pUser->hBitmap = CreateCompatibleBitmap (hWinDC, pUser->cxClient, pUser->cyClient) ;
    ReleaseDC (hwnd, hWinDC) ;
    return 0 ;
}

static LRESULT OnPaint (HWND hwnd, WPARAM wParam, LPARAM lParam) {
    USERDATA*   pUser = GetWindowLongPtr (hwnd, GWLP_USERDATA) ;
    HDC         hWinDC ;
    HGDIOBJ     hOldBmp, hOldPen ;
    PAINTSTRUCT paint ;
    hWinDC  = BeginPaint (hwnd, &paint) ;
    hOldBmp = SelectObject (pUser->hMemDC, pUser->hBitmap) ;
    hOldPen = SelectObject (pUser->hMemDC, pUser->hPen) ;
    BitBlt (pUser->hMemDC, 0, 0, pUser->cxClient, pUser->cyClient, pUser->hMemDC, 0, 0, WHITENESS) ;
    DrawTree (pUser, pUser->cxClient / 2.0f + pUser->offset, pUser->cyClient, RAD (90.0f), 12) ;
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
        pUser->offset = lParam * 4.5f ;
        break ;
    case TC_LENGTH:
        pUser->length = lParam / 4.5f ;
        break ;
    case TC_ANGLE:
        pUser->angle = RAD (lParam * 2.0f) ;
        break ;
    }
    InvalidateRect (hwnd, NULL, FALSE) ;
    return 0 ;
}

static LRESULT OnSetColor (HWND hwnd, WPARAM wParam, LPARAM lParam) {
    USERDATA* pUser = GetWindowLongPtr (hwnd, GWLP_USERDATA) ;
    DeleteObject (pUser->hPen) ;
    pUser->hPen = CreatePen (PS_SOLID, 0, lParam) ;
    InvalidateRect (hwnd, NULL, FALSE) ;
    return 0 ;
}

static LRESULT OnClipCopy (HWND hwnd, WPARAM wParam, LPARAM lParam) {
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
        return OnCreate (hwnd, wParam, lParam) ;
    case WM_DESTROY:
        return OnDestroy (hwnd, wParam, lParam) ;
    case WM_PAINT:
        return OnPaint (hwnd, wParam, lParam) ;
    case WM_SIZE:
        return OnSize (hwnd, wParam, lParam) ;
    case TCM_SETPARAM:
        return OnSetParam (hwnd, wParam, lParam) ;
    case TCM_SETCOLOR:
        return OnSetColor (hwnd, wParam, lParam) ;
    case TCM_CLIPCOPY:
        return OnClipCopy (hwnd, wParam, lParam) ;
    default:
        return DefWindowProc (hwnd, uMsg, wParam, lParam) ;
    }
}

ATOM RegisterTreeClass (HINSTANCE hInstance) {
    const WNDCLASS wndclass = {
        .style         = CS_HREDRAW | CS_VREDRAW,
        .lpfnWndProc   = WndProc,
        .cbClsExtra    = 0,
        .cbWndExtra    = 0,
        .hInstance     = hInstance,
        .hIcon         = NULL,
        .hCursor       = LoadCursor (NULL, IDC_CROSS),
        .hbrBackground = GetStockObject (WHITE_BRUSH),
        .lpszMenuName  = NULL,
        .lpszClassName = TREE_CLASS
    };
    return RegisterClass (&wndclass) ;
}
