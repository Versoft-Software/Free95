/*
 * Free95
 *
 * Module name:
 *      win32.c
 *
 * Description:
 *      Win32 API Implementation
 *
 * Author: Kap Petrov
 *
*/

#include "win32.h"

void FsWrite(FileNode *file, const char *con)
{
    if (strcmp(file->Name, "CON") == 0)
    {
        KiPutString(con, 0, 0);
    }
    else
    {
        strcpy(file->Content, con);
    }
}

char *FsRead(FileNode *file)
{
    if (strcmp(file->Name, "CON") == 0)
    {
        const char *kbBuf = getInpA();
        return kbBuf;
    }
    else
    {
        return file->Content;
    }
}

void NtWriteFile(HANDLE FileHandle, PVOID Buffer, ULONG Length)
{
    FsWrite(FileHandle, Buffer);
}

char *NtReadFile(HANDLE FileHandle)
{
    return FsRead(FileHandle);
}

UINT lenstr(const char *str) {
    const char *s = str; // Pointer to traverse the string
    while (*s) {         // Continue until null character is found
        s++;
    }
    return s - str;      // Length is the difference between pointers
}

WINBOOL WriteFile(HANDLE hFile, LPCVOID lpBuffer)
{
    ULONG length = (ULONG)lenstr(lpBuffer);

    asm volatile (
        "mov %0, %%edi\n"       // Syscall number (176)
        "mov %1, %%ebx\n"       // File handle
        "mov %2, %%esi\n"       // Buffer
        "mov %3, %%edx\n"       // Length
        "int $0x2E\n"           // Trigger syscall
        :
        : "g"(176), "g"(hFile), "g"((PVOID)lpBuffer), "g"(length)
        : "edi", "ebx", "esi", "edx"
    );
    return TRUE;
}

UINT wcslen(const char* str)
{
    UINT length = 0;
    while (str[length] != '\0')
    {
        length++;
    }
    return length;
}

HANDLE GetStdHandle(DWORD nStdHandle)
{
    HANDLE hnd;

    if (nStdHandle == STD_INPUT_HANDLE || STD_OUTPUT_HANDLE)
    {
        hnd = getCon();
    }
    else
    {
        hnd = 0;
    }

    return hnd;
}

WINBOOL WriteConsole(HANDLE hConsoleOutput, const VOID *lpBuffer, DWORD nNumberOfCharsToWrite, LPDWORD lpNumberOfCharsWritten, LPVOID lpReserved)
{
    WriteFile(hConsoleOutput, lpBuffer);

    lpNumberOfCharsWritten = nNumberOfCharsToWrite;
    return TRUE;
}

WINBOOL ReadConsole(HANDLE hConsoleInput, LPVOID lpBuffer, DWORD nNumberOfCharsToRead, LPDWORD lpNumbersOfCharRead, LPVOID pInputControl)
{
    const char *buffer =  NtReadFile(hConsoleInput);
    strcpy(lpBuffer, buffer);
    lpNumbersOfCharRead = nNumberOfCharsToRead;

    return TRUE;
}

WNDCLASS registered;
int winW = 0;
int winH = 0;
int nMsgInc = 0;

HMODULE GetModuleHandle(LPCSTR lpModuleName)
{
    return 0;
}

ATOM RegisterClass(WNDCLASS *lpWndClass)
{
    registered = *lpWndClass;
    return 1;
}

HWND CreateWindowEx(DWORD dwExStyle, LPCTSTR lpClassName, LPCTSTR lpWindowName, DWORD dwStyle, int x, int y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
{
    WINDOW *sample = (WINDOW *)malloc(sizeof(WINDOW));

    sample->x = x;
    sample->y = y;
    sample->w = nWidth;
    sample->h = nHeight;
    strncpy(sample->name, lpWindowName, 25);
    sample->color = registered.hbrBackground;
    registered.reserved = 0;

    winW = nWidth;
    winH = nHeight;

    return (HWND)sample;
}

WINBOOL ShowWindow(HWND hWnd, int nCmdShow)
{
    WINDOW *win = (WINDOW*)hWnd;
    FillRectangle(win->x, win->y, win->w, win->h, win->color);

    FillRectangle(win->x, win->y , win->w, 20, RGB(0, 0, 255));

    KiPutStringC(win->name, win->x, win->y + 2, RGB(255, 255, 255));
    FillRectangle(win->x + (win->w - 20), win->y, 20, 20, 0xFFFF0000);

    winW = win->x;
    winH = win->y;

    LPARAM p = 0;
    POINT clickedPoint = {mouse_getx(), mouse_gety()};
    WORD hi = mouse_gety();
    WORD lo = mouse_getx();
    p = MAKELPARAM(lo, hi);

    PostMessage(0, WM_PAINT, 0, 0, clickedPoint);

    if (mouse_button_left())
    {
        PostMessage(0, WM_LBUTTONDOWN, 0, p, clickedPoint);
        setClicked(0);
    }

    return TRUE;
}

WINBOOL MessageBox(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType)
{
    // Screen resolution
    int screenWidth = 640;
    int screenHeight = 480;

    // Message box dimensions
    int msgBoxWidth = 300;
    int msgBoxHeight = 200;

    // Calculate the top-left position to center the message box
    int msgBoxX = (screenWidth - msgBoxWidth) / 2;
    int msgBoxY = (screenHeight - msgBoxHeight) / 2;

    // Title bar height
    int titleBarHeight = 20;

    // Body
    FillRectangle(msgBoxX, msgBoxY, msgBoxWidth, msgBoxHeight, RGB(255, 255, 255));

    // Title bar
    FillRectangle(msgBoxX, msgBoxY, msgBoxWidth, titleBarHeight, RGB(0, 0, 255));

    // Title
    KiPutStringC(lpCaption, msgBoxX + 5, msgBoxY + 2, RGB(255, 255, 255));

    // Text
    KiPutStringC(lpText, msgBoxX + (msgBoxWidth / 2) - (4 * strlen(lpText)), msgBoxY + (msgBoxHeight / 2) - 10, RGB(0, 0, 0));

    // Type configs
    if (uType == MB_OK)
    {
        // OK button dimensions
        int okButtonWidth = 50;
        int okButtonHeight = 22;

        // OK button position
        int okButtonX = msgBoxX + (msgBoxWidth / 2) - (okButtonWidth / 2);
        int okButtonY = msgBoxY + msgBoxHeight - okButtonHeight - 10;

        FillRectangle(okButtonX, okButtonY, okButtonWidth, okButtonHeight, RGB(192, 192, 192));
        KiPutStringC("OK", okButtonX + (okButtonWidth / 2) - 8, okButtonY + 5, RGB(0, 0, 0));
    }

    return TRUE;
}


LRESULT DefWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    return 100;
}

typedef struct {
    MSG messages[MAX_MESSAGES];
    int head;
    int tail;
    int size;
} MessageArray;

MessageArray messageArray = { .head = 0, .tail = 0, .size = 0 };

void PostMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, POINT pt)
{
    if (messageArray.size >= MAX_MESSAGES)
    {
        // Well, shit.
        MessageBox(0, "       A fatal error has occured\n       System halted.", "Error", MB_OK);
        HaltKbdDrv();
        while(1);
        return;
    }

    MSG newMsg = {
        .hwnd = hWnd,
        .message = message,
        .wParam = wParam,
        .lParam = lParam,
        .time = 0,
        .pt = pt
    };

    messageArray.messages[messageArray.tail] = newMsg;
    messageArray.tail = (messageArray.tail + 1) % MAX_MESSAGES;
    messageArray.size++;
}

WINBOOL GetMessage(MSG *lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax)
{
    if (messageArray.size == 0)
    {
        // No messages to retrieve, so go fuck yourself.
        return FALSE;
    }

    *lpMsg = messageArray.messages[messageArray.head];
    messageArray.head = (messageArray.head + 1) % MAX_MESSAGES;
    messageArray.size--;

    return TRUE;
}

WINBOOL TranslateMessage(const MSG *lpMsg)
{
    return FALSE;
}

LRESULT DispatchMessage(const MSG *lpMsg)
{
    registered.lpfnWndProc(lpMsg->hwnd, lpMsg->message, lpMsg->wParam, lpMsg->lParam);
    return 1;
}

void PostQuitMessage(int nExitCode)
{
    registered.reserved = 10;
    POINT pExample = {0, 0};
    PostMessage(0, WM_CLOSE, 0, 0, pExample);
}

UINT uColor = 0x00000000;

RECT BeginPaint(HANDLE hWnd, PAINTSTRUCT *psStruct)
{
    psStruct->rcPaint.left = 0;
    psStruct->rcPaint.top = 0;
    psStruct->rcPaint.right = winW;
    psStruct->rcPaint.bottom = winH;

    return (RECT)psStruct->rcPaint;
}

WINBOOL SetTextColor(HDC hDc, UINT color)
{
    uColor = color;
}

int SetBkMode(HDC hDc, int mode)
{
    return 0;
}

WINBOOL TextOut(HDC h, int x, int y, LPCSTR lpString, int c)
{
    RECT *hDc;
    hDc = (RECT*)(h);

    KiPutStringC(lpString, hDc->right + x, hDc->bottom + y + 20, uColor);
}

HDC EndPaint(HANDLE hWnd, PAINTSTRUCT *psStruct)
{
    return 0;
}

HBRUSH CreateSolidBrush(UINT nBrush)
{
    return nBrush;
}

WINBOOL DeleteObject(HBRUSH hBrush)
{
    hBrush = 0;
    return TRUE;
}
