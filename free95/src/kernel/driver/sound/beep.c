/*
 * Free 95
 *
 * NAME:
 *		beep.c
 *
 * DESCRIPTION:
 *		PC Speaker on motherboard driver.
 *
 * Author: Rainy101112
 *
 */

#include "beep.h"
#include "common.h"

void system_beep(int hertz)
{
	int i;
	if (hertz == 0)
	{
		i = inb(0x61);
		outb(0x61, i & 0x0d);
	}
	else
	{
		i = hertz;
		outb(0x43, 0xb6);
		outb(0x42, i & 0xff);
		outb(0x42, i >> 8);
		i = inb(0x61);
		outb(0x61, (i | 0x03) & 0x0f);
	}
}