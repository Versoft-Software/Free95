/*
 * Free95
 *
 * Module name:
 *      freever.c
 *
 * Description:
 *      Version Checker for Free95
 *
 * Author: Kap Petrov
 *
*/

#include "freever.h"

LRESULT VerProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_CLOSE:
            PostQuitMessage(0);
            return 0;
        case WM_PAINT:
            PAINTSTRUCT ps;
            HANDLE hdc = BeginPaint(hwnd, &ps);

            SetTextColor(hdc, RGB(0, 0, 0));
            SetBkMode(hdc, TRANSPARENT);
            TextOut(hdc, 10, 10, "Versoft Free95\nVersion 0.2.0\n\nAlpha testing version\n", 6);

            EndPaint(hwnd, &ps);
            return 0;
        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}

void VerMain()
{
    const char *CLASS_NAME = "VerApp";

    WNDCLASS wc = {};
    wc.hInstance = 0;
    wc.lpszClassName = CLASS_NAME;
    wc.lpfnWndProc = VerProc;
    wc.hbrBackground = (HBRUSH)(RGB(127, 127, 127));

    if(!RegisterClass(&wc))
    {
        MessageBox(0, "FAILED TO CREATE WINDOW!", "ERROR", MB_OK);
        return;
    }

    HWND hwnd1 = CreateWindowEx(0, CLASS_NAME, "Free95 Version", WS_OVERLAPPEDWINDOW, 200, 100, 220, 200, 0, 0, 0, 0);

    ShowWindow(hwnd1, SW_SHOWNORMAL);

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

    free(hwnd1, sizeof(hwnd1));
}
