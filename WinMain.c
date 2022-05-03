#include <windows.h>
#include <commctrl.h>
#include <stdlib.h>
#include "treectl.h"
#include "resource.h"

INT_PTR Dialog_OnInitDialog(HWND hDlg, WPARAM wParam, LPARAM lParam) {
    HINSTANCE hInstance;
    HWND      hCtl;

    hCtl = GetDlgItem(hDlg, IDC_OFFSET);
    SendMessage(hCtl, WM_UPDATEUISTATE, MAKEWPARAM(UIS_SET, UISF_HIDEFOCUS), 0);
    SendMessage(hCtl, TBM_SETRANGE, FALSE, MAKELPARAM(-10, 10));
    SendMessage(hCtl, TBM_SETPOS, FALSE, 1);
    SendMessage(hCtl, TBM_SETPOS, TRUE, 0);

    hCtl = GetDlgItem(hDlg, IDC_LENGTH);
    SendMessage(hCtl, WM_UPDATEUISTATE, MAKEWPARAM(UIS_SET, UISF_HIDEFOCUS), 0);
    SendMessage(hCtl, TBM_SETRANGE, FALSE, MAKELPARAM(5, 15));

    hCtl = GetDlgItem(hDlg, IDC_ANGLE);
    SendMessage(hCtl, WM_UPDATEUISTATE, MAKEWPARAM(UIS_SET, UISF_HIDEFOCUS), 0);
    SendMessage(hCtl, TBM_SETRANGE, FALSE, MAKELPARAM(-10, 10));
    SendMessage(hCtl, TBM_SETPOS, FALSE, 1);
    SendMessage(hCtl, TBM_SETPOS, TRUE, 0);

    hCtl = GetDlgItem(hDlg, IDC_CYAN);
    SendMessage(hCtl, WM_UPDATEUISTATE, MAKEWPARAM(UIS_SET, UISF_HIDEFOCUS), 0);

    hCtl = GetDlgItem(hDlg, IDC_MAGENTA);
    SendMessage(hCtl, WM_UPDATEUISTATE, MAKEWPARAM(UIS_SET, UISF_HIDEFOCUS), 0);

    hCtl = GetDlgItem(hDlg, IDC_YELLOW);
    SendMessage(hCtl, WM_UPDATEUISTATE, MAKEWPARAM(UIS_SET, UISF_HIDEFOCUS), 0);

    hCtl = GetDlgItem(hDlg, IDC_BLACK);
    SendMessage(hCtl, WM_UPDATEUISTATE, MAKEWPARAM(UIS_SET, UISF_HIDEFOCUS), 0);
    SendMessage(hCtl, BM_SETCHECK, BST_CHECKED, 0);

    hInstance = GetWindowLongPtr(hDlg, GWLP_HINSTANCE);
    hCtl = CreateWindowEx(WS_EX_CLIENTEDGE, TREE_CLASS, NULL, WS_CHILD | WS_VISIBLE, 197, 37, 297, 258, hDlg, NULL, hInstance, NULL);
    SetWindowLongPtr(hCtl, GWLP_ID, IDC_TREE);
    SendMessage(hCtl, TCM_SETPARAM, MAKEWPARAM(TC_OFFSET, 0), 0);
    SendMessage(hCtl, TCM_SETPARAM, MAKEWPARAM(TC_LENGTH, 0), 5);
    SendMessage(hCtl, TCM_SETPARAM, MAKEWPARAM(TC_ANGLE, 0), 0);

    return FALSE;
}

INT_PTR Dialog_OnHScroll(HWND hDlg, WPARAM wParam, LPARAM lParam) {
    LPARAM nParam;
    switch (GetDlgCtrlID(lParam)) {
    case IDC_OFFSET:
        nParam = SendMessage(lParam, TBM_GETPOS, 0, 0);
        SendMessage(GetDlgItem(hDlg, IDC_TREE), TCM_SETPARAM, MAKEWPARAM(TC_OFFSET, 0), nParam);
        break;
    case IDC_LENGTH:
        nParam = SendMessage(lParam, TBM_GETPOS, 0, 0);
        SendMessage(GetDlgItem(hDlg, IDC_TREE), TCM_SETPARAM, MAKEWPARAM(TC_LENGTH, 0), nParam);
        break;
    case IDC_ANGLE:
        nParam = SendMessage(lParam, TBM_GETPOS, 0, 0);
        SendMessage(GetDlgItem(hDlg, IDC_TREE), TCM_SETPARAM, MAKEWPARAM(TC_ANGLE, 0), nParam);
        break;
    }
    return 0;
}

INT_PTR Dialog_OnCommand(HWND hDlg, WPARAM wParam, LPARAM lParam) {
    COLORREF color;
    if (HIWORD(wParam) == BN_CLICKED) {
        switch (LOWORD(wParam)) {
        case IDC_CYAN:
            color = RGB(0, 255, 255);
            SendMessage(GetDlgItem(hDlg, IDC_TREE), TCM_SETCOLOR, 0, color);
            break;
        case IDC_MAGENTA:
            color = RGB(255, 0, 255);
            SendMessage(GetDlgItem(hDlg, IDC_TREE), TCM_SETCOLOR, 0, color);
            break;
        case IDC_YELLOW:
            color = RGB(255, 255, 0);
            SendMessage(GetDlgItem(hDlg, IDC_TREE), TCM_SETCOLOR, 0, color);
            break;
        case IDC_BLACK:
            color = RGB(0, 0, 0);
            SendMessage(GetDlgItem(hDlg, IDC_TREE), TCM_SETCOLOR, 0, color);
            break;
        }
    } else if (HIWORD(wParam) == 1) {
        switch (LOWORD(wParam)) {
        case IDA_COPY:
            SendMessage(GetDlgItem(hDlg, IDC_TREE), TCM_CLIPCOPY, 0, 0);
            break;
        case IDA_EXIT:
            SendMessage(hDlg, WM_CLOSE, 0, 0);
            break;
        }
    }
    return 0;
}

INT_PTR Dialog_OnClose(HWND hDlg, WPARAM wParam, LPARAM lParam) {
    DestroyWindow(hDlg);
    PostQuitMessage(0);
    return 0;
}

INT_PTR Dialog_OnCtlColorDlg(HWND hDlg, WPARAM wParam, LPARAM lParam) {
    return GetStockObject(WHITE_BRUSH);
}

INT_PTR Dialog_OnCtlColorStatic(HWND hDlg, WPARAM wParam, LPARAM lParam) {
    return GetStockObject(WHITE_BRUSH);
}

INT_PTR CALLBACK DlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_INITDIALOG:
        return Dialog_OnInitDialog(hDlg, wParam, lParam);
    case WM_HSCROLL:
        return Dialog_OnHScroll(hDlg, wParam, lParam);
    case WM_COMMAND:
        return Dialog_OnCommand(hDlg, wParam, lParam);
    case WM_CLOSE:
        return Dialog_OnClose(hDlg, wParam, lParam);
    case WM_CTLCOLORDLG:
        return Dialog_OnCtlColorDlg(hDlg, wParam, lParam);
    case WM_CTLCOLORSTATIC:
        return Dialog_OnCtlColorStatic(hDlg, wParam, lParam);
    default:
        return 0;
    }
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int nCmdShow) {
    HACCEL hAccel;
    HICON  hIcon;
    HWND   hDlg;
    MSG    msg;

    if (!RegisterTree(hInstance)) {
        MessageBox(NULL, L"Failed to register the window class", L"Error", MB_OK | MB_ICONERROR);
        return 0;
    }

    hAccel = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_ACCEL));
    hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TREE));
    hDlg = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_WINDOW), NULL, DlgProc);
    SendMessage(hDlg, WM_SETICON, ICON_SMALL, hIcon);
    SendMessage(hDlg, WM_SETICON, ICON_BIG, hIcon);
    ShowWindow(hDlg, nCmdShow);

    while (GetMessage(&msg, NULL, 0, 0)) {
        if (!IsDialogMessage(hDlg, &msg) || !TranslateAccelerator(hDlg, hAccel, &msg)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return 0;
}