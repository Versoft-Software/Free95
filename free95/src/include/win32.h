#ifndef WIN32_H
#define WIN32_H

#include "base.h"
#include <stdbool.h>

#define STD_INPUT_HANDLE  ((DWORD)-10)
#define STD_OUTPUT_HANDLE ((DWORD)-11)
#define STD_ERROR_HANDLE  ((DWORD)-12)

#define WS_OVERLAPPEDWINDOW 0x00CF0000
#define WM_DESTROY 0x0002
#define WM_CLOSE 0x0010
#define WM_PAINT 0x000F
#define WM_LBUTTONDOWN 0x0201
#define SW_SHOWNORMAL 1
#define COLOR_WINDOW 0xFFFFFFFE
#define TRANSPARENT 0
#define OPAQUE 1

#define MB_OK 0
#define MB_OKCANCEL 1
#define MB_YESNO 4
#define MB_YESNOCANCEL 3

typedef unsigned long DWORD_PTR;

#define RGB(r, g, b) (((UINT)(0) << 24) | ((UINT)(r) << 16) | ((UINT)(g) << 8) | (UINT)(b))
#define LOWORD(l) ((WORD)((DWORD_PTR)(l) & 0xFFFF))
#define HIWORD(l) ((WORD)((DWORD_PTR)(l) >> 16))
#define MAKELPARAM(l, h) ((LPARAM)((DWORD)(((WORD)(l)) | ((DWORD)((WORD)(h))) << 16)))

#define MAX_MESSAGES 4096

typedef struct
{
    void (*Task)(void);
    bool IsActive;
    char Name[100];
} TaskControlBlock;

typedef struct FileNode
{
    char Name[100];
    char Content[1024];
} FileNode;

typedef struct WNDCLASS
{
    UINT style;
    LRESULT (*lpfnWndProc)(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    HINSTANCE hInstance;
    HICON hIcon;
    HCURSOR hCursor;
    HBRUSH hbrBackground;
    LPCSTR lpszMenuName;
    LPCSTR lpszClassName;
    int reserved;
} WNDCLASS;

typedef struct WINDOW
{
    int x;
    int y;
    int w;
    int h;
    LPCTSTR name;
    UINT color;
} WINDOW;

typedef struct POINT
{
    int x;
    int y;
} POINT;

typedef struct tagMSG
{
    HWND   hwnd;    // Handle to the window receiving the message
    UINT   message; // The message identifier (e.g., WM_PAINT, WM_CLOSE)
    WPARAM wParam;  // Additional message-specific information
    LPARAM lParam;  // Additional message-specific information
    DWORD  time;    // Time the message was posted
    POINT  pt;      // Cursor position when the message was posted
} MSG, *PMSG;

typedef struct Node
{
    MSG msg;
    struct Node *next;
} Node;

typedef struct tagRECT {
    UINT left;    // x-coordinate of the upper-left corner
    UINT top;     // y-coordinate of the upper-left corner
    UINT right;   // x-coordinate of the lower-right corner
    UINT bottom;  // y-coordinate of the lower-right corner
} RECT, *PRECT;

typedef struct tagPAINTSTRUCT
{
    HDC  hdc;         // Handle to the device context for painting
    WINBOOL fErase;      // Indicates whether the background needs to be erased
    RECT rcPaint;     // Rectangle defining the area to be painted
    WINBOOL fRestore;    // Reserved; typically FALSE
    WINBOOL fIncUpdate;  // Reserved; typically FALSE
    BYTE rgbReserved[32]; // Reserved for future use
} PAINTSTRUCT, *PPAINTSTRUCT;

void NtWriteFile(HANDLE FileHandle, PVOID Buffer, ULONG Length);
WINBOOL WriteFile(HANDLE hFile, LPCVOID lpBuffer);
UINT wcslen(const char* str);
HANDLE GetStdHandle(DWORD nStdHandle);
WINBOOL WriteConsole(HANDLE hConsoleOutput, const VOID *lpBuffer, DWORD nNumberOfCharsToWrite, LPDWORD lpNumberOfCharsWritten, LPVOID lpReserved);
WINBOOL ReadConsole(HANDLE hConsoleInput, LPVOID lpBuffer, DWORD nNumberOfCharsToRead, LPDWORD lpNumbersOfCharRead, LPVOID pInputControl);

#endif
