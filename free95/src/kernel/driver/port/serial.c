/*
 * Free 95
 *
 * NAME:
 *		serial.c
 *
 * DESCRIPTION:
 *		Serial port driver.
 *
 * Author: Rainy101112
 *
 */

#include "serial.h"
#include "common.h"

/* Initialize serial port */
void init_serial(int baud_rate)
{
	uint16_t divisor = 115200 / baud_rate;

	outb(SERIAL_PORT + 1, 0x00);
	outb(SERIAL_PORT + 3, 0x80);
	outb(SERIAL_PORT + 0, divisor & 0xFF);
	outb(SERIAL_PORT + 1, (divisor >> 8) & 0xFF);
	outb(SERIAL_PORT + 3, 0x03);
	outb(SERIAL_PORT + 2, 0xC7);
	outb(SERIAL_PORT + 4, 0x0B);
	outb(SERIAL_PORT + 4, 0x1E);
	outb(SERIAL_PORT + 0, 0xAE);

	if (inb(SERIAL_PORT + 0) != 0xAE)
	{
		return;
	}

	outb(SERIAL_PORT + 4, 0x0F);
}

int serial_received(void)
{
	return inb(SERIAL_PORT + 5) & 1;
}

int is_transmit_empty(void)
{
	return inb(SERIAL_PORT + 5) & 0x20;
}

char read_serial(void)
{
	while (serial_received() == 0)
		;
	return inb(SERIAL_PORT);
}

void write_serial(char a)
{
	while (is_transmit_empty() == 0)
		;
	outb(SERIAL_PORT, a);
}

void write_serial_string(const char *str)
{
	while (*str)
	{
		while (is_transmit_empty() == 0)
			;
		outb(SERIAL_PORT, *str);
		str++;
	}
}