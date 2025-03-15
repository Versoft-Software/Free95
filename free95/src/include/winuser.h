#ifndef WINUSER_H
#define WINUSER_H

#include "win32.h"

WINBOOL FillRect(HDC hDc, RECT *r, HBRUSH hBrush)
{
    RECT rObject;
    rObject = *r;

    FillRectangle(rObject.left, rObject.top + 20, rObject.right, rObject.bottom, RGB(0, 128, 255));
    return TRUE;
}

WINBOOL PtInRect(const RECT *rect, POINT pt)
{
    return (pt.x >= rect->left &&
            pt.x <= rect->right &&
            pt.y >= rect->top + 20 &&
            pt.y <= rect->bottom);
}

#endif
