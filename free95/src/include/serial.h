/*
 * Free 95
 *
 * NAME:
 *		serial.h
 *
 * DESCRIPTION:
 *		Serial port driver.
 *
 * Author: Rainy101112
 *
 */

#ifndef SERIAL_H
#define SERIAL_H

#define SERIAL_PORT 0x3f8

void init_serial(int baud_rate);
int serial_received(void);
int is_transmit_empty(void);
char read_serial(void);
void write_serial(char a);
void write_serial_string(const char *str);

#endif // SERIAL_H
