#include "windows.h"
#include "../ke/string/string.h"

/* Returns OS Information:

	Version 4.0
	Build 5120
	NT-Based Operating System
	Service Pack 0

 for now. */
WINBOOL GetVersionExA(LPOSVERSIONINFOA lpVersionInformation)
{
	lpVersionInformation->dwOSVersionInfoSize = 148;
	lpVersionInformation->dwMajorVersion = 4;
	lpVersionInformation->dwMinorVersion = 0;
	lpVersionInformation->dwBuildNumber = 5120;
	lpVersionInformation->dwPlatformId = 2; // NT-Based OS
	strcpy(lpVersionInformation->szCSDVersion, "Service Pack 0\0");

	return TRUE;	
}

/* TODO */
WINBOOL VerifyVersionInfoA(LPOSVERSIONINFOEXA lpVersionInformation, DWORD dwTypeMask, DWORDLONG dwlConditionMask)
{
	return FALSE;
}
