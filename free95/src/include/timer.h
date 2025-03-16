/*
 * Free 95
 *
 * NAME:
 *		timer.h
 *
 * DESCRIPTION:
 *		Timer.
 *
 * Author: Rainy101112 ...
 *
 */

#ifndef TIMER_H
#define TIMER_H

#include "base.h"
#include "stdint.h"

#define PIT_COMMAND_PORT 0x43
#define PIT_CHANNEL0_PORT 0x40
#define PIT_FREQUENCY 1193182

void set_pit_frequency(uint32_t frequency);
void timer_handler();
void init_timer(uint32_t frequency);
void KiWait(uint32_t milliseconds);

#endif