/*++

Free95

You may only use this code if you agree to the terms of the Free95 Source Code License agreement (GNU GPL v3) (see LICENSE).
If you do not agree to the terms, do not use the code.


	PROJECT: Free95 Userspace Components
	FILE: buildno.c
	DESCRIPTION: A Version Displayer Native NT Application.
	AUTHOR: @KapTheGuy

--*/

#include "appinclude/print.h"

void _start()
{
    unsigned int nBuild = 5414;
    unsigned int nBeta = 3;
    unsigned int nMajorVersion = 0;
    unsigned int nMiddleVersion = 3;
    unsigned int nMinorVersion = 0;

    char szMajorVersion[256];
    char szMiddleVersion[256];
    char szMinorVersion[256];
    char szBeta[256];
    char szBuild[256];

    RtlCreateStringFromUint(nMajorVersion, szMajorVersion);
    RtlCreateStringFromUint(nMiddleVersion, szMiddleVersion);
    RtlCreateStringFromUint(nMinorVersion, szMinorVersion);
    RtlCreateStringFromUint(nBeta, szBeta);
    RtlCreateStringFromUint(nBuild, szBuild);

	RtlCliDisplayString("Versoft Free95\nVersion ");
    RtlCliDisplayString(szMajorVersion);
    RtlCliDisplayString(".");
    RtlCliDisplayString(szMiddleVersion);
    RtlCliDisplayString(".");
    RtlCliDisplayString(szMinorVersion);
    RtlCliDisplayString(" Beta ");
    RtlCliDisplayString(szBeta);
    RtlCliDisplayString(" (Build ");
    RtlCliDisplayString(szBuild);
    RtlCliDisplayString(")\n");
}
