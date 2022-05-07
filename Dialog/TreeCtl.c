#include "treectl.h"
#include <math.h>
#include <stdlib.h>

#define PI 3.14159265359

typedef struct {
    LRESULT nOffset;
    LRESULT nLength;
    LRESULT nAngle;
    HDC     hMemDC;
    HBITMAP hBitmap;
    HPEN    hPen;
    int     nWidth;
    int     nHeight;
} TREEINFO, *PTREEINFO;

void DrawTree(PTREEINFO pInfo, int x, int y, double angle, int step) {
    double length = pInfo->nLength / 5.0;
    if (step > 0) {
        int nx = x + (int)(cos(angle * PI / 180.0) * step * length);
        int ny = y - (int)(sin(angle * PI / 180.0) * step * length);
        MoveToEx(pInfo->hMemDC, x, y, NULL);
        LineTo(pInfo->hMemDC, nx, ny);
        MoveToEx(pInfo->hMemDC, x, y, NULL);
        DrawTree(pInfo, nx, ny, angle - 20.0 - pInfo->nAngle * 2.0, step - 1);
        DrawTree(pInfo, nx, ny, angle + 20.0 - pInfo->nAngle * 2.0, step - 1);
    }
}

LRESULT Tree_OnCreate(HWND hWnd, WPARAM wParam, LPARAM lParam) {
    PTREEINFO pInfo;
    HDC       hWinDC;
    RECT      rect;
    if (pInfo = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(TREEINFO))) {
        SetWindowLongPtr(hWnd, 0, pInfo);
        GetClientRect(hWnd, &rect);
        hWinDC = GetDC(hWnd);
            pInfo->hMemDC = CreateCompatibleDC(hWinDC);
            pInfo->hBitmap = CreateCompatibleBitmap(hWinDC, rect.right - rect.left, rect.bottom - rect.top);
            pInfo->hPen = GetStockObject(BLACK_PEN);
        ReleaseDC(hWnd, hWinDC);
        return 0;
    } else {
        return -1;
    }
}

LRESULT Tree_OnDestroy(HWND hWnd, WPARAM wParam, LPARAM lParam) {
    PTREEINFO pInfo = GetWindowLongPtr(hWnd, 0);
    DeleteObject(pInfo->hPen);
    DeleteObject(pInfo->hBitmap);
    DeleteDC(pInfo->hMemDC);
    HeapFree(GetProcessHeap(), 0, pInfo);
    return 0;
}

LRESULT Tree_OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam) {
    PTREEINFO pInfo = GetWindowLongPtr(hWnd, 0);
    pInfo->nWidth = LOWORD(lParam);
    pInfo->nHeight = HIWORD(lParam);
    return 0;
}

LRESULT Tree_OnPaint(HWND hWnd, WPARAM wParam, LPARAM lParam) {
    PTREEINFO   pInfo = GetWindowLongPtr(hWnd, 0);
    HDC         hWinDC;
    PAINTSTRUCT paint;

    hWinDC = BeginPaint(hWnd, &paint);
    SaveDC(pInfo->hMemDC);
        SelectObject(pInfo->hMemDC, pInfo->hBitmap);
        SelectObject(pInfo->hMemDC, pInfo->hPen);
        BitBlt(pInfo->hMemDC, 0, 0, pInfo->nWidth, pInfo->nHeight, pInfo->hMemDC, 0, 0, WHITENESS);
        DrawTree(pInfo, pInfo->nWidth / 2 + pInfo->nOffset * 4, pInfo->nHeight, 90.0, 12);
        BitBlt(hWinDC, 0, 0, pInfo->nWidth, pInfo->nHeight, pInfo->hMemDC, 0, 0, SRCCOPY);
    RestoreDC(pInfo->hMemDC, -1);
    EndPaint(hWnd, &paint);

    return 0;
}

LRESULT Tree_OnSetParam(HWND hWnd, WPARAM wParam, LPARAM lParam) {
    PTREEINFO pInfo = GetWindowLongPtr(hWnd, 0);
    switch (LOWORD(wParam)) {
    case TC_OFFSET:
        pInfo->nOffset = lParam;
        break;
    case TC_LENGTH:
        pInfo->nLength = lParam;
        break;
    case TC_ANGLE:
        pInfo->nAngle = lParam;
        break;
    }
    InvalidateRect(hWnd, NULL, FALSE);
    return 0;
}

LRESULT Tree_OnSetColor(HWND hWnd, WPARAM wParam, LPARAM lParam) {
    PTREEINFO pInfo = GetWindowLongPtr(hWnd, 0);
    DeleteObject(pInfo->hPen);
    pInfo->hPen = CreatePen(PS_SOLID, 0, lParam);
    InvalidateRect(hWnd, NULL, FALSE);
    return 0;
}

LRESULT Tree_OnClipCopy(HWND hWnd, WPARAM wParam, LPARAM lParam) {
    PTREEINFO pInfo = GetWindowLongPtr(hWnd, 0);
    HDC       hMemDC, hWinDC;
    HBITMAP   hBitmap;

    OpenClipboard(hWnd);

    hWinDC = GetDC(hWnd);
    hMemDC = CreateCompatibleDC(hWinDC);
    hBitmap = CreateCompatibleBitmap(hWinDC, pInfo->nWidth, pInfo->nHeight);
        SelectObject(hMemDC, hBitmap);
        BitBlt(hMemDC, 0, 0, pInfo->nWidth, pInfo->nHeight, hWinDC, 0, 0, SRCCOPY);
    DeleteDC(hMemDC);
    ReleaseDC(hWnd, hWinDC);

    EmptyClipboard();
    SetClipboardData(CF_BITMAP, hBitmap);
    CloseClipboard();
    return 0;
}

LRESULT CALLBACK Tree_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_CREATE:
        return Tree_OnCreate(hWnd, wParam, lParam);
    case WM_DESTROY:
        return Tree_OnDestroy(hWnd, wParam, lParam);
    case WM_PAINT:
        return Tree_OnPaint(hWnd, wParam, lParam);
    case WM_SIZE:
        return Tree_OnSize(hWnd, wParam, lParam);
    case TCM_SETPARAM:
        return Tree_OnSetParam(hWnd, wParam, lParam);
    case TCM_SETCOLOR:
        return Tree_OnSetColor(hWnd, wParam, lParam);
    case TCM_CLIPCOPY:
        return Tree_OnClipCopy(hWnd, wParam, lParam);
    default:
        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }
}

ATOM RegisterTree(HINSTANCE hInstance) {
    const WNDCLASS wndclass = {
        .style         = CS_HREDRAW | CS_VREDRAW,
        .lpfnWndProc   = Tree_WndProc,
        .cbClsExtra    = 0,
        .cbWndExtra    = sizeof(INT_PTR),
        .hInstance     = hInstance,
        .hIcon         = NULL,
        .hCursor       = LoadCursor(NULL, IDC_CROSS),
        .hbrBackground = GetStockObject(WHITE_BRUSH),
        .lpszMenuName  = NULL,
        .lpszClassName = TREE_CLASS
    };
    return RegisterClass(&wndclass);
}
