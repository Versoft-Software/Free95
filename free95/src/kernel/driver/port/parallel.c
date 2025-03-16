/*
 * Free 95
 *
 * NAME:
 *		parallel.c
 *
 * DESCRIPTION:
 *		Parallel port driver.
 *
 * Author: Rainy101112
 *
 */

#include "timer.h"
#include "common.h"
#include "parallel.h"

void wait_parallel_ready(void)
{
	while ((!inb(LPT1_PORT_STATUS)) & 0x80)
	{
		KiWait(1000);
	}
}

void parallel_write(unsigned char c)
{
	unsigned char lControl;
	wait_parallel_ready();

	outb(LPT1_PORT_BASE, c);
	lControl = inb(LPT1_PORT_CONTROL);
	outb(LPT1_PORT_CONTROL, lControl | 1);
	KiWait(1000);
	outb(LPT1_PORT_CONTROL, lControl);

	wait_parallel_ready();
}

void parallel_write_string(const char *str)
{
	while (*str)
	{
		unsigned char lControl;
		wait_parallel_ready();

		outb(LPT1_PORT_BASE, *str);
		lControl = inb(LPT1_PORT_CONTROL);
		outb(LPT1_PORT_CONTROL, lControl | 1);
		KiWait(1000);
		outb(LPT1_PORT_CONTROL, lControl);

		wait_parallel_ready();
		str++;
	}
}
