#ifndef WINDOWS_H
#define WINDOWS_H

/* Include all Win32 API Related headers here */
#include "console.h"
#include "windef.h"

typedef struct _OSVERSIONINFOA
{
    DWORD dwOSVersionInfoSize;  // Size of this structure, in bytes.
    DWORD dwMajorVersion;       // Major version number of the OS.
    DWORD dwMinorVersion;       // Minor version number of the OS.
    DWORD dwBuildNumber;        // Build number of the OS.
    DWORD dwPlatformId;         // Platform identifier.
    CHAR  szCSDVersion[128];    // Null-terminated string for service pack info.
} OSVERSIONINFOA, *POSVERSIONINFOA, *LPOSVERSIONINFOA;

typedef struct _OSVERSIONINFOW
{
    DWORD dwOSVersionInfoSize;
    DWORD dwMajorVersion;
    DWORD dwMinorVersion;
    DWORD dwBuildNumber;
    DWORD dwPlatformId;
    WCHAR szCSDVersion[128];
} OSVERSIONINFOW, *POSVERSIONINFOW, *LPOSVERSIONINFOW;

#ifdef UNICODE
typedef OSVERSIONINFOW OSVERSIONINFO;
typedef LPOSVERSIONINFOW LPOSVERSIONINFO;
#else
typedef OSVERSIONINFOA OSVERSIONINFO;
typedef LPOSVERSIONINFOA LPOSVERSIONINFO;
#endif

typedef struct _OSVERSIONINFOEXA
{
    DWORD dwOSVersionInfoSize;  // Size of this structure, in bytes.
    DWORD dwMajorVersion;       // Major version number of the OS.
    DWORD dwMinorVersion;       // Minor version number of the OS.
    DWORD dwBuildNumber;        // Build number of the OS.
    DWORD dwPlatformId;         // Platform identifier.
    CHAR  szCSDVersion[128];    // Null-terminated string for service pack info.
    WORD  wServicePackMajor;    // Major version number of the service pack.
    WORD  wServicePackMinor;    // Minor version number of the service pack.
    WORD  wSuiteMask;           // Bitmask for product suites available on the system.
    BYTE  wProductType;         // Additional information about the system type.
    BYTE  wReserved;            // Reserved for future use.
} OSVERSIONINFOEXA, *POSVERSIONINFOEXA, *LPOSVERSIONINFOEXA;

typedef struct _OSVERSIONINFOEXW
{
    DWORD dwOSVersionInfoSize;
    DWORD dwMajorVersion;
    DWORD dwMinorVersion;
    DWORD dwBuildNumber;
    DWORD dwPlatformId;
    WCHAR szCSDVersion[128];
    WORD  wServicePackMajor;
    WORD  wServicePackMinor;
    WORD  wSuiteMask;
    BYTE  wProductType;
    BYTE  wReserved;
} OSVERSIONINFOEXW, *POSVERSIONINFOEXW, *LPOSVERSIONINFOEXW;

#ifdef UNICODE
typedef OSVERSIONINFOEXW OSVERSIONINFOEX;
typedef LPOSVERSIONINFOEXW LPOSVERSIONINFOEX;
#else
typedef OSVERSIONINFOEXA OSVERSIONINFOEX;
typedef LPOSVERSIONINFOEXA LPOSVERSIONINFOEX;
#endif

WINBOOL GetVersionExA(LPOSVERSIONINFOA lpVersionInformation);

WINBOOL VerifyVersionInfoA(LPOSVERSIONINFOEXA lpVersionInformation, DWORD dwTypeMask, DWORDLONG dwlConditionMask);

#define VerifyVersionInfo VerifyVersionInfoA

#endif
