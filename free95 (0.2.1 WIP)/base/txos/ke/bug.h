#ifndef BUG_H
#define BUG_H

#include <stdint.h>

/* Bug codes */
#define KMODE_DIV_ZERO                			0x00000000
#define KMODE_PAGE_FAULT                		0x0000001E
#define KMODE_GPF								0x000001EF
#define KMODE_INVOP								0x00000FF0
#define KMODE_DF								0x000000EE
#define KMODE_SNP								0x00000001
#define IMP_CRASH								0xFFFFFFFF
#define KMODE_GDTFAIL							0x0000FF00

void KeBugCheck(uint32_t BugCheckCode);

#endif
