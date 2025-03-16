#ifndef WINUSER_H
#define WINUSER_H

#include "win32.h"

extern VOID FillRectangle(UINT32 x, UINT32 y, UINT32 width, UINT32 height, UINT32 color);

WINBOOL FillRect(RECT *r)
{
    RECT rObject;
    rObject = *r;

    FillRectangle(rObject.left, rObject.top + 20, rObject.right, rObject.bottom, RGB(0, 128, 255));
    return TRUE;
}

WINBOOL PtInRect(const RECT *rect, POINT pt)
{
    return ((UINT)pt.x >= rect->left &&
            (UINT)pt.x <= rect->right &&
            (UINT)pt.y >= rect->top + 20 &&
            (UINT)pt.y <= rect->bottom);
}

#endif
