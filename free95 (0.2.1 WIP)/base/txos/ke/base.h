#ifndef BASE_H
#define BASE_H

typedef unsigned short ATOM;
/* Changed from BOOL to WINBOOL to avoid Objective-C conflict */
typedef int WINBOOL;
typedef unsigned char BOOLEAN;
typedef unsigned char BYTE;
typedef unsigned long CALTYPE;
typedef unsigned long CALID;
typedef char CCHAR;
typedef unsigned long COLORREF;
#define CONST const

/* Check VOID before defining CHAR, SHORT, and LONG */
#ifndef VOID
#define VOID void
typedef char CHAR;
typedef short SHORT;
typedef long LONG;
#endif

/*
typedef CTRYID;
typedef DLGPROC;
*/
typedef unsigned int DWORD; /* was unsigned long */
typedef double DWORDLONG, *PDWORDLONG;
/*
typedef EDITWORDBREAKPROC;
typedef ENHMFENUMPROC;
typedef ENUMRESLANGPROC;
typedef ENUMRESNAMEPROC;
typedef ENUMRESTYPEPROC;
*/
typedef float FLOAT;
/* typedef GLOBALHANDLE; */
typedef void *HANDLE;
typedef HANDLE HACCEL;
typedef HANDLE HBITMAP;
typedef HANDLE HBRUSH;
typedef HANDLE HCOLORSPACE;
typedef HANDLE HCONV;
typedef HANDLE HCONVLIST;
typedef HANDLE HCURSOR;
typedef HANDLE HDBC;
typedef HANDLE HDC;
typedef HANDLE HDDEDATA;
typedef HANDLE HDESK;
typedef HANDLE HDROP;
typedef HANDLE HDWP;
typedef HANDLE HENHMETAFILE;
typedef HANDLE HENV;
typedef int HFILE;
typedef HANDLE HFONT;
typedef HANDLE HGDIOBJ;
typedef HANDLE HGLOBAL;
typedef HANDLE HGLRC;
typedef HANDLE HHOOK;
typedef HANDLE HICON;
typedef HANDLE HIMAGELIST;
typedef HANDLE HINSTANCE;
typedef HANDLE HKEY, *PHKEY;
typedef HANDLE HKL;
typedef HANDLE HLOCAL;
typedef HANDLE HMENU;
typedef HANDLE HMETAFILE;
typedef HANDLE HMODULE;
typedef HANDLE HPALETTE;
typedef HANDLE HPEN;
typedef HANDLE HRASCONN;
typedef long HRESULT;
typedef HANDLE HRGN;
typedef HANDLE HRSRC;
typedef HANDLE HSTMT;
typedef HANDLE HSZ;
typedef HANDLE HWINSTA;
typedef HANDLE HWND;
typedef int INT;
typedef unsigned short LANGID;
typedef DWORD LCID;
typedef DWORD LCTYPE;
/* typedef LOCALHANDLE */
typedef double LONGLONG, *PLONGLONG;
typedef unsigned short *LP;
typedef long LPARAM;
typedef WINBOOL *LPBOOL;
typedef BYTE *LPBYTE;
typedef CONST CHAR *LPCCH;
typedef CHAR *LPCH;
typedef COLORREF *LPCOLORREF;
typedef const char *LPCSTR;
typedef char *PCSZ;

#ifdef UNICODE
typedef const unsigned short *LPCTSTR;
#else
typedef const char *LPCTSTR;
#endif /* UNICODE */

typedef const unsigned short *LPCWCH;
typedef const unsigned short *LPCWSTR;
typedef DWORD *LPDWORD;
/* typedef LPFRHOOKPROC; */
typedef HANDLE *LPHANDLE;
/* typedef LPHANDLER_FUNCTION; */
typedef int *LPINT;
typedef long *LPLONG;
typedef char *LPSTR;

#ifdef UNICODE
typedef unsigned short *LPTCH;
typedef unsigned short *LPTSTR;
#else
typedef char *LPTCH;
typedef char *LPTSTR;
#endif /* UNICODE */

typedef long LRESULT;
typedef void *LPVOID;
typedef const void *LPCVOID;
typedef unsigned short *LPWCH;
typedef unsigned short *LPWORD;
typedef unsigned short *LPWSTR;
typedef unsigned short *PWSTR;
/* typedef NPSTR; */
typedef unsigned short *NWPSTR;
typedef WINBOOL *PWINBOOL;
typedef BYTE *PBOOLEAN;
typedef BYTE *PBYTE;
typedef const CHAR *PCCH;
typedef CHAR *PCH;
typedef CHAR *PCHAR;
typedef const char *PCSTR;
typedef const unsigned short *PCWCH;
typedef const unsigned short *PCWSTR;
typedef DWORD *PDWORD;
typedef float *PFLOAT;
typedef HANDLE *PHANDLE;
/* typedef PHKEY; */
typedef int *PINT;
/* typedef LCID *PLCID; */
typedef long *PLONG;
typedef short *PSHORT;
/* typedef PSID; */
typedef char *PSTR;
typedef char *PSZ;

#ifdef UNICODE
typedef unsigned short *PTBYTE;
typedef unsigned short *PTCH;
typedef unsigned short *PTCHAR;
typedef unsigned short *PTSTR;
#else
typedef unsigned char *PTBYTE;
typedef char *PTCH;
typedef char *PTCHAR;
typedef char *PTSTR;
#endif /* UNICODE */

typedef unsigned char *PUCHAR;
typedef unsigned int *PUINT;
typedef unsigned long *PULONG;
typedef unsigned short *PUSHORT;
typedef void *PVOID;
typedef unsigned short *PWCH;
typedef unsigned short *PWCHAR;
typedef unsigned short *PWORD;
/*
typedef PWSTR;
typedef REGSAM;
*/

typedef short RETCODE;

typedef HANDLE SC_HANDLE;
typedef LPVOID SC_LOCK;
typedef SC_HANDLE *LPSC_HANDLE;
typedef DWORD SERVICE_STATUS_HANDLE;
/* typedef SPHANDLE; */

#ifdef UNICODE
typedef unsigned short TBYTE;
typedef unsigned short TCHAR;
typedef unsigned short BCHAR;
#else
typedef unsigned char TBYTE;
typedef char TCHAR;
typedef BYTE BCHAR;
#endif /* UNICODE */

typedef unsigned char UCHAR;
typedef unsigned int UINT;
typedef unsigned long ULONG;
typedef unsigned short USHORT;
typedef unsigned short WCHAR;
typedef unsigned short WORD;
typedef unsigned int WPARAM;

#define TRUE 1
#define FALSE 0
#define true 1
#define false 0

#endif
