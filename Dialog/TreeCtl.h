#ifndef WINDOW_TREECTL_H
#define WINDOW_TREECTL_H

#include <windows.h>

#define TREE_CLASS     L"Tree Control"

#define TCM_SETPARAM   (WM_USER + 0x0000)
#define TCM_SETCOLOR   (WM_USER + 0x0001)
#define TCM_CLIPBOARD  (WM_USER + 0x0002)

#define TC_OFFSET      0
#define TC_LENGTH      1
#define TC_ANGLE       2

ATOM RegisterTreeClass (HINSTANCE hInstance) ;

#endif