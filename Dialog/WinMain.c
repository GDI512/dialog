#include "resource.h"
#include "TreeCtl.h"

#include <Windows.h>
#include <CommCtrl.h>

static INT_PTR OnInitDialog (HWND hDlg) {
    HWND hCtl = GetDlgItem (hDlg, IDC_OFFSET) ;
    SendMessage (hCtl, TBM_SETRANGE, FALSE, MAKELPARAM (-10, 10)) ;
    SendMessage (hCtl, TBM_SETPOS, FALSE, 1) ;
    SendMessage (hCtl, TBM_SETPOS, TRUE, 0) ;

    hCtl = GetDlgItem (hDlg, IDC_LENGTH) ;
    SendMessage (hCtl, TBM_SETRANGE, FALSE, MAKELPARAM (5, 15)) ;
    SendMessage (hCtl, TBM_SETPOS, TRUE, 10) ;

    hCtl = GetDlgItem (hDlg, IDC_ANGLE) ;
    SendMessage (hCtl, TBM_SETRANGE, FALSE, MAKELPARAM (-10, 10)) ;
    SendMessage (hCtl, TBM_SETPOS, FALSE, 1) ;
    SendMessage (hCtl, TBM_SETPOS, TRUE, 0) ;

    hCtl = GetDlgItem (hDlg, IDC_BLACK) ;
    SendMessage (hCtl, BM_SETCHECK, BST_CHECKED, 0) ;

    hCtl = CreateWindow (TREE_CLASS, NULL, WS_CHILD | WS_VISIBLE, 178, 23, 334, 290, 
        hDlg, NULL, GetWindowLongPtr (hDlg, GWLP_HINSTANCE), NULL) ;
    SetWindowLongPtr (hCtl, GWLP_ID, IDC_TREE) ;
    return TRUE ;
}

static INT_PTR OnScroll (HWND hDlg, LPARAM lParam) {
    switch (GetDlgCtrlID (lParam)) {
    case IDC_OFFSET:
        lParam = SendMessage (lParam, TBM_GETPOS, 0, 0) ;
        SendMessage (GetDlgItem (hDlg, IDC_TREE), TCM_SETPARAM, TC_OFFSET, lParam) ;
        break ;
    case IDC_LENGTH:
        lParam = SendMessage (lParam, TBM_GETPOS, 0, 0) ;
        SendMessage (GetDlgItem (hDlg, IDC_TREE), TCM_SETPARAM, TC_LENGTH, lParam) ;
        break ;
    case IDC_ANGLE:
        lParam = SendMessage (lParam, TBM_GETPOS, 0, 0) ;
        SendMessage (GetDlgItem (hDlg, IDC_TREE), TCM_SETPARAM, TC_ANGLE, lParam) ;
        break ;
    }
    return SetWindowLongPtr (hDlg, DWLP_MSGRESULT, 0), TRUE ;
}

static INT_PTR OnCommand (HWND hDlg, WPARAM wParam) {
    COLORREF color ;
    if (HIWORD (wParam) == BN_CLICKED) {
        switch (LOWORD (wParam)) {
        case IDC_CYAN:
            color = RGB (0, 255, 255) ;
            SendMessage (GetDlgItem (hDlg, IDC_TREE), TCM_SETCOLOR, 0, color) ;
            break ;
        case IDC_MAGENTA:
            color = RGB (255, 0, 255) ;
            SendMessage (GetDlgItem (hDlg, IDC_TREE), TCM_SETCOLOR, 0, color) ;
            break ;
        case IDC_YELLOW:
            color = RGB (255, 255, 0) ;
            SendMessage (GetDlgItem (hDlg, IDC_TREE), TCM_SETCOLOR, 0, color) ;
            break ;
        case IDC_BLACK:
            color = RGB (0, 0, 0) ;
            SendMessage (GetDlgItem (hDlg, IDC_TREE), TCM_SETCOLOR, 0, color) ;
            break ;
        }
    } else if (HIWORD (wParam) == 1) {
        switch (LOWORD (wParam)) {
        case IDA_COPY:
            SendMessage (GetDlgItem (hDlg, IDC_TREE), TCM_CLIPBOARD, 0, 0) ;
            break ;
        case IDA_EXIT:
            SendMessage (hDlg, WM_CLOSE, 0, 0) ;
            break ;
        }
    }
    return SetWindowLongPtr (hDlg, DWLP_MSGRESULT, 0), TRUE ;
}

static INT_PTR OnClose (HWND hDlg) {
    DestroyWindow (hDlg) ;
    PostQuitMessage (0) ;
    return SetWindowLongPtr (hDlg, DWLP_MSGRESULT, 0), TRUE ;
}

INT_PTR CALLBACK DlgProc (HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_INITDIALOG:
        return OnInitDialog (hDlg) ;
    case WM_HSCROLL:
        return OnScroll (hDlg, lParam) ;
    case WM_COMMAND:
        return OnCommand (hDlg, wParam) ;
    case WM_CLOSE:
        return OnClose (hDlg) ;
    case WM_CTLCOLORDLG:
    case WM_CTLCOLORSTATIC:
        return GetStockObject (WHITE_BRUSH) ;
    default:
        return FALSE ;
    }
}

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int nCmdShow) {
    HWND   hDlg ;
    HACCEL hAccel ;
    HICON  hIcon ;
    MSG    msg ;

    if (!RegisterTreeClass (hInstance)) {
        MessageBox (NULL, L"Failed to register the window class", L"Error", MB_ICONERROR) ;
        return 1 ;
    }

    hDlg   = CreateDialog (hInstance, MAKEINTRESOURCE (IDD_DIALOG), NULL, DlgProc) ;
    hAccel = LoadAccelerators (hInstance, MAKEINTRESOURCE (IDR_ACCEL)) ;
    hIcon  = LoadIcon (hInstance, MAKEINTRESOURCE (IDI_TREE)) ;
    SendMessage (hDlg, WM_SETICON, ICON_SMALL, hIcon) ;
    SendMessage (hDlg, WM_SETICON, ICON_BIG, hIcon) ;
    ShowWindow (hDlg, nCmdShow) ;

    while (GetMessage (&msg, NULL, 0, 0)) {
        if (!IsDialogMessage (hDlg, &msg) & !TranslateAccelerator (hDlg, hAccel, &msg)) {
            TranslateMessage (&msg) ;
            DispatchMessage (&msg) ;
        }
    }
    return 0 ;
}