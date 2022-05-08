#include "TreeCtl.h"

#include <math.h>

#define PI 3.14159265359

typedef struct {
    LRESULT nOffset ;
    LRESULT nLength ;
    LRESULT nAngle ;
    HDC     hMemDC ;
    HBITMAP hBitmap ;
    HPEN    hPen ;
    int     nWidth ;
    int     nHeight ;
} USERDATA ;

static void DrawTree (USERDATA* pUser, int xPos, int yPos, double angle, int step) {
    double length = pUser->nLength / 5.0 ;
    if (step > 0) {
        int nx = xPos + (int) (cos (angle * PI / 180.0) * step * length) ;
        int ny = yPos - (int) (sin (angle * PI / 180.0) * step * length) ;
        MoveToEx (pUser->hMemDC, xPos, yPos, NULL) ;
        LineTo (pUser->hMemDC, nx, ny) ;
        MoveToEx (pUser->hMemDC, xPos, yPos, NULL) ;
        DrawTree (pUser, nx, ny, angle - 20.0 - pUser->nAngle * 2.0, step - 1) ;
        DrawTree (pUser, nx, ny, angle + 20.0 - pUser->nAngle * 2.0, step - 1) ;
    }
}

static LRESULT OnCreate (HWND hwnd, WPARAM wParam, LPARAM lParam) {
    USERDATA* pUser ;
    HDC       hWinDC ;
    pUser = HeapAlloc (GetProcessHeap (), HEAP_ZERO_MEMORY, sizeof (USERDATA)) ;
    SetWindowLongPtr (hwnd, 0, pUser) ;
    hWinDC        = GetDC (hwnd) ;
    pUser->hMemDC = CreateCompatibleDC (hWinDC) ;
    pUser->hPen   = GetStockObject (BLACK_PEN) ;
    ReleaseDC (hwnd, hWinDC) ;
    return 0 ;
}

static LRESULT OnDestroy (HWND hwnd, WPARAM wParam, LPARAM lParam) {
    USERDATA* pUser = GetWindowLongPtr (hwnd, 0) ;
    DeleteObject (pUser->hPen) ;
    DeleteObject (pUser->hBitmap) ;
    DeleteDC (pUser->hMemDC) ;
    HeapFree (GetProcessHeap (), 0, pUser) ;
    return 0 ;
}

static LRESULT OnSize (HWND hwnd, WPARAM wParam, LPARAM lParam) {
    USERDATA* pUser = GetWindowLongPtr (hwnd, 0) ;
    HDC       hWinDC ;
    pUser->nWidth  = LOWORD (lParam) ;
    pUser->nHeight = HIWORD (lParam) ;
    hWinDC = GetDC (hwnd) ;
    DeleteObject (pUser->hBitmap) ;
    pUser->hBitmap = CreateCompatibleBitmap (hWinDC, pUser->nWidth, pUser->nHeight) ;
    ReleaseDC (hwnd, hWinDC) ;
    return 0 ;
}

static LRESULT OnPaint (HWND hwnd, WPARAM wParam, LPARAM lParam) {
    USERDATA*   pUser = GetWindowLongPtr (hwnd, 0) ;
    HDC         hWinDC ;
    PAINTSTRUCT paint ;
    hWinDC = BeginPaint (hwnd, &paint) ;
    SaveDC (pUser->hMemDC) ;
    SelectObject (pUser->hMemDC, pUser->hBitmap) ;
    SelectObject (pUser->hMemDC, pUser->hPen) ;
    BitBlt (pUser->hMemDC, 0, 0, pUser->nWidth, pUser->nHeight, pUser->hMemDC, 0, 0, WHITENESS) ;
    DrawTree (pUser, pUser->nWidth / 2 + pUser->nOffset * 4, pUser->nHeight, 90.0, 12) ;
    BitBlt (hWinDC, 0, 0, pUser->nWidth, pUser->nHeight, pUser->hMemDC, 0, 0, SRCCOPY) ;
    RestoreDC (pUser->hMemDC, -1) ;
    EndPaint (hwnd, &paint) ;
    return 0 ;
}

static LRESULT OnSetParam (HWND hwnd, WPARAM wParam, LPARAM lParam) {
    USERDATA* pUser = GetWindowLongPtr (hwnd, 0) ;
    switch (LOWORD (wParam)) {
    case TC_OFFSET:
        pUser->nOffset = lParam ;
        break ;
    case TC_LENGTH:
        pUser->nLength = lParam ;
        break ;
    case TC_ANGLE:
        pUser->nAngle = lParam ;
        break ;
    }
    InvalidateRect (hwnd, NULL, FALSE) ;
    return 0 ;
}

static LRESULT OnSetColor (HWND hwnd, WPARAM wParam, LPARAM lParam) {
    USERDATA* pUser = GetWindowLongPtr (hwnd, 0) ;
    DeleteObject (pUser->hPen) ;
    pUser->hPen = CreatePen (PS_SOLID, 0, lParam) ;
    InvalidateRect (hwnd, NULL, FALSE) ;
    return 0 ;
}

static LRESULT OnClipCopy (HWND hwnd, WPARAM wParam, LPARAM lParam) {
    USERDATA* pUser = GetWindowLongPtr (hwnd, 0) ;
    HDC       hMemDC, hWinDC ;
    HBITMAP   hBitmap ;

    OpenClipboard (hwnd) ;

    hWinDC  = GetDC (hwnd) ;
    hMemDC  = CreateCompatibleDC (hWinDC) ;
    hBitmap = CreateCompatibleBitmap (hWinDC, pUser->nWidth, pUser->nHeight) ;
    SelectObject (hMemDC, hBitmap) ;
    BitBlt (hMemDC, 0, 0, pUser->nWidth, pUser->nHeight, hWinDC, 0, 0, SRCCOPY) ;
    DeleteDC (hMemDC) ;
    ReleaseDC (hwnd, hWinDC) ;

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
        .cbWndExtra    = sizeof (INT_PTR),
        .hInstance     = hInstance,
        .hIcon         = NULL,
        .hCursor       = LoadCursor (NULL, IDC_CROSS),
        .hbrBackground = GetStockObject (WHITE_BRUSH),
        .lpszMenuName  = NULL,
        .lpszClassName = TREE_CLASS
    };
    return RegisterClass (&wndclass) ;
}
