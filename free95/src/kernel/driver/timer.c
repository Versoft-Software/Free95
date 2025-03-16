/*
 * Free 95
 *
 * NAME:
 *		timer.c
 *
 * DESCRIPTION:
 *		Timer.
 *
 * Author: Rainy101112 ...
 *
 */

#include "stdint.h"
#include "common.h"
#include "timer.h"
#include "isr.h"

volatile uint32_t timer_ticks = 0;
uint32_t current_frequency = 0;

void set_pit_frequency(uint32_t frequency)
{
	current_frequency = frequency; // Store the frequency globally
	uint16_t divisor = PIT_FREQUENCY / frequency;

	outb(PIT_COMMAND_PORT, 0x36);							   // Channel 0, LSB/MSB, Mode 3 (square wave)
	outb(PIT_CHANNEL0_PORT, (uint8_t)(divisor & 0xFF));		   // Low byte
	outb(PIT_CHANNEL0_PORT, (uint8_t)((divisor >> 8) & 0xFF)); // High byte
}

// Timer interrupt handler
void timer_handler()
{
	timer_ticks++;	  // Increment the tick counter
	outb(0x20, 0x20); // Send End of Interrupt (EOI) to PIC
}

// Function to initialize the timer
void init_timer(uint32_t frequency)
{
	set_pit_frequency(frequency);

	// Register the timer interrupt handler for IRQ0
	register_interrupt_handler(IRQ0, timer_handler);
}

void KiWait(uint32_t milliseconds)
{
	uint32_t start_ticks = timer_ticks;
	uint32_t ticks_to_wait = milliseconds * (PIT_FREQUENCY / 1000) / current_frequency;

	while ((timer_ticks - start_ticks) < ticks_to_wait)
	{
		// Busy wait
		asm volatile("sti\nhlt"); // Optionally halt CPU to reduce power consumption
	}
}