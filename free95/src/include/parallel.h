/*
 * Free 95
 *
 * NAME:
 *		parallel.h
 *
 * DESCRIPTION:
 *		Parallel port driver.
 *
 * Author: Rainy101112
 *
 */

#ifndef PARALLEL_H
#define PARALLEL_H

#define LPT1_PORT_BASE 0x378
#define LPT1_PORT_DATA LPT1_PORT_BASE
#define LPT1_PORT_STATUS LPT1_PORT_BASE + 1
#define LPT1_PORT_CONTROL LPT1_PORT_BASE + 2

void wait_parallel_ready(void);

void parallel_write(unsigned char c);

void parallel_write_string(const char *str);

#endif // PARALLEL_H