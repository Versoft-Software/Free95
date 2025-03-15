/*
 * Free95
 *
 * Module name:
 *      hello.c
 *
 * Description:
 *      Windows API Hello World Application
 *
 * Author: Kap Petrov
 *
*/

#include "hello.h"

int once = 0;

LRESULT WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_CLOSE:
            PostQuitMessage(0);
            return 0;
        case WM_PAINT:
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            SetTextColor(hdc, RGB(0, 0, 255));
            SetBkMode(hdc, TRANSPARENT);
            TextOut(hdc, 10, 10, "Welcome to Free95!\nPress F3 for a tutorial on\nhow to use Free95", 13);

            EndPaint(hwnd, &ps);
            return 0;
        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}

void WinMain()
{
    const char *CLASS_NAME = "SampleClass";

    WNDCLASS wc = {};
    wc.hInstance = 0;
    wc.lpszClassName = CLASS_NAME;
    wc.lpfnWndProc = WindowProc;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    if(!RegisterClass(&wc))
    {
        MessageBox(0, "FAILED TO CREATE WINDOW!", "ERROR", MB_OK);
        return;
    }

    HWND hwnd = CreateWindowEx(0, CLASS_NAME, "Welcome", WS_OVERLAPPEDWINDOW, 330, 100, 300, 300, 0, 0, 0, 0);

    ShowWindow(hwnd, SW_SHOWNORMAL);

    MSG msg = {};
    if (GetMessage(&msg, 0, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    else
    {
        PsKillSystemThread(1);
        FillRectangle(0, 0, 640, 480, 0x00000000);
    }

    free(hwnd, sizeof(hwnd));
}
