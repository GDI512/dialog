#ifndef WINDOW_TREECTL_H
#define WINDOW_TREECTL_H

#include <windows.h>

#define TREE_CLASS  L"Tree Control"

#define TCM_SETPARAM  (WM_USER + 0x0001)
#define TCM_SETCOLOR  (WM_USER + 0x0002)
#define TCM_CLIPCOPY  (WM_USER + 0x0003)

#define TC_OFFSET  0x0
#define TC_LENGTH  0x1
#define TC_ANGLE   0x2

ATOM RegisterTreeClass(HINSTANCE hInstance);

#endif