/*
 * Free 95
 *
 * NAME:
 *		common.h
 *
 * DESCRIPTION:
 *		Common driver.
 *
 * Author: Rainy101112
 *
 */

#ifndef INCLUDE_COMMON_H_
#define INCLUDE_COMMON_H_

#include "base.h"
#include "string.h"

void outb(uint16_t port, uint8_t value);
void outw(uint16_t port, uint16_t value);
void outl(uint16_t port, uint32_t value);

uint8_t inb(uint16_t port);
uint16_t inw(uint16_t port);
uint32_t inl(uint16_t port);

void insw(uint16_t port, void *buf, unsigned long n);

void outsw(uint16_t port, const void *buf, unsigned long n);

void insl(uint32_t port, void *addr, int cnt);

void outsl(uint32_t port, const void *addr, int cnt);

uint32_t load_eflags(void);

void store_eflags(uint32_t eflags);

uint32_t get_cr0(void);

void set_cr0(uint32_t cr0);

void cpu_get_msr(uint32_t msr, uint32_t *lo, uint32_t *hi);

void cpu_set_msr(uint32_t msr, uint32_t lo, uint32_t hi);

void enable_intr(void);
void disable_intr(void);
void krn_halt(void);

#endif // INCLUDE_COMMON_H_