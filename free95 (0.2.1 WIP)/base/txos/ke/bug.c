/*++

Free95 20x/TX Kernel

You may only use this code if you agree to the terms of the Free95 Source Code License agreement (GNU GPL v3) (see LICENSE).
If you do not agree to the terms, do not use the code.


Module Name:

    bug.c

Abstract:

    This module implements functions to bring the system down in a mannered way.
    Inspired by NT BugCheck

--*/

#include "bug.h"
#include "string/string.h"
#include "../init/kernel.h"

#define MAX_BUFFER 256

void KeBugCheck(uint32_t BugCheckCode)
{
	char Message[MAX_BUFFER];

	switch(BugCheckCode)
	{
		case KMODE_DIV_ZERO:
			strcpy(Message, "Division Error");
			break;
		case KMODE_PAGE_FAULT:
			strcpy(Message, "Page Fault");
			break;
		case KMODE_GPF:
			strcpy(Message, "General Protection Fault");
			break;
		case KMODE_INVOP:
			strcpy(Message, "Invalid Opcode");
			break;
		case KMODE_DF:
			strcpy(Message, "Double Fault");
			break;
		case KMODE_SNP:
			strcpy(Message, "Segment Not Present");
			break;
		case IMP_CRASH:
			strcpy(Message, "..., How did we get here?");
			break;
		case KMODE_GDTFAIL:
			strcpy(Message, "Invalid argument to encoding GDT");
			break;
		default:
			strcpy(Message, "Exception was not handled");
			break;
	}

	DbgPrint("\n\rFatal system error: ");
	DbgPrint(Message);
	DbgPrint("\n\r");

	terminal_initialize();

	print("\n*** STOP: ");
	print(Message);
	print("\n\nIf this is the first time you have seen this, restart your computer.\nIf this screen appears again, follow these steps:\n\nCheck for any faulty hardware, or if software is properly installed.\nDisable BIOS memory options such as caching or shadowing.\nCheck if your Free95 Installation is corrupted.\n");

	while(1);
}
