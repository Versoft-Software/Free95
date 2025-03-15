/*
 * Free95
 *
 * Module name:
 *      tutorial.c
 *
 * Description:
 *      Free95 Tutorial
 *
 * Author: Kap Petrov
 *
*/

#include "tutorial.h"

LRESULT TutProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_CLOSE:
            PostQuitMessage(0);
            return 0;
        case WM_PAINT:
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            SetTextColor(hdc, RGB(0, 0, 0));
            SetBkMode(hdc, TRANSPARENT);
            TextOut(hdc, 10, 10, "Keyboard Shortcuts\nF7: Kill all processes\nF6: Secret :)\nCTRL.ALT.DEL: Secret :)\n\nHappy Learning!", 13);

            EndPaint(hwnd, &ps);
            return 0;
        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}

void TutMain()
{
    const char *CLASS_NAME = "TutorialApp";

    WNDCLASS wc = {};
    wc.hInstance = 0;
    wc.lpszClassName = CLASS_NAME;
    wc.lpfnWndProc = TutProc;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    if(!RegisterClass(&wc))
    {
        MessageBox(0, "FAILED TO CREATE WINDOW!", "ERROR", MB_OK);
        return;
    }

    HWND hwnd = CreateWindowEx(0, CLASS_NAME, "Free95 Tutorial", WS_OVERLAPPEDWINDOW, 0, 0, 640, 480, 0, 0, 0, 0);

    ShowWindow(hwnd, SW_SHOWNORMAL);

    MSG msg = {};
    if (GetMessage(&msg, 0, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    else
    {
        PsKillSystemThread(2);
        FillRectangle(0, 0, 640, 480, 0x00000000);
    }

    free(hwnd, sizeof(hwnd));
    while (1);
}
