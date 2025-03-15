/*
 * Free95
 *
 * Module name:
 *      reshell.c
 *
 * Description:
 *      Program Manager for Free95
 *
 * Author: Kap Petrov
 *
*/

#include "reshell.h"
#include "winuser.h"

LRESULT ReshellProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    RECT target = {20, 15, 48, 48};

    switch (uMsg)
    {
        case WM_CLOSE:
            PostQuitMessage(0);
            return 0;
        case WM_LBUTTONDOWN:
            int x = LOWORD(lParam);
            int y = HIWORD(lParam);
            POINT pt = {x, y};
            if (PtInRect(&target, pt))
            {
                launch();
            }
            return 0;
        case WM_PAINT:
            PAINTSTRUCT ps;
            HANDLE hdc = BeginPaint(hwnd, &ps);

            SetTextColor(hdc, RGB(0, 0, 0));
            SetBkMode(hdc, TRANSPARENT);
            TextOut(hdc, 8, 58, "Freever", 6);

            RECT rect = {20, 15, 48, 48};

            HBRUSH hBrush = CreateSolidBrush(RGB(0, 128, 255));

            FillRect(hdc, &rect, hBrush);

            DeleteObject(hBrush);

            EndPaint(hwnd, &ps);
            return 0;
        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}

void ReshellMain()
{
    const char *CLASS_NAME = "ReshellApp";

    WNDCLASS wc = {};
    wc.hInstance = 0;
    wc.lpszClassName = CLASS_NAME;
    wc.lpfnWndProc = ReshellProc;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    if(!RegisterClass(&wc))
    {
        MessageBox(0, "FAILED TO CREATE WINDOW!", "ERROR", MB_OK);
        return;
    }

    HWND hwnd = CreateWindowEx(0, CLASS_NAME, "Program Manager", WS_OVERLAPPEDWINDOW, 10, 10, 300, 300, 0, 0, 0, 0);

    ShowWindow(hwnd, SW_SHOWNORMAL);

    MSG msg = {};
    if (GetMessage(&msg, 0, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    else
    {
        PsKillSystemThread(0);
        FillRectangle(0, 0, 640, 480, 0x00000000);
    }

    free(hwnd, sizeof(hwnd));
}
