/*
 * Free 95
 *
 * NAME:
 *		common.c
 *
 * DESCRIPTION:
 *		Common driver.
 *
 * Author: Rainy101112
 *
 */

#include "common.h"
#include "base.h"

inline void outb(uint16_t port, uint8_t value)
{
	__asm__ __volatile__("outb %1, %0" : : "dN"(port), "a"(value));
}

inline uint8_t inb(uint16_t port)
{
	uint8_t ret;
	__asm__ __volatile__("inb %1, %0" : "=a"(ret) : "dN"(port));
	return ret;
}

inline void outw(uint16_t port, uint16_t value)
{
	__asm__ __volatile__("outw %1, %0" : : "dN"(port), "a"(value));
}

inline uint16_t inw(uint16_t port)
{
	uint16_t ret;
	__asm__ __volatile__("inw %1, %0" : "=a"(ret) : "dN"(port));
	return ret;
}

inline void outl(uint16_t port, uint32_t value)
{
	__asm__ __volatile__("outl %1, %0" : : "dN"(port), "a"(value));
}

inline uint32_t inl(uint16_t port)
{
	uint32_t ret;
	__asm__ __volatile__("inl %1, %0" : "=a"(ret) : "dN"(port));
	return ret;
}

inline void insw(uint16_t port, void *buf, unsigned long n)
{
	__asm__ __volatile__("cld; rep; insw" : "+D"(buf), "+c"(n) : "d"(port));
}

inline void outsw(uint16_t port, const void *buf, unsigned long n)
{
	__asm__ __volatile__("cld; rep; outsw" : "+S"(buf), "+c"(n) : "d"(port));
}

inline void insl(uint32_t port, void *addr, int cnt)
{
	__asm__ __volatile__("cld;"
						 "repne; insl;"
						 : "=D"(addr), "=c"(cnt)
						 : "d"(port), "0"(addr), "1"(cnt)
						 : "memory", "cc");
}

inline void outsl(uint32_t port, const void *addr, int cnt)
{
	__asm__ __volatile__("cld;"
						 "repne; outsl;"
						 : "=S"(addr), "=c"(cnt)
						 : "d"(port), "0"(addr), "1"(cnt)
						 : "memory", "cc");
}

inline uint32_t load_eflags(void)
{
	uint32_t eflags;
	__asm__ __volatile__("pushf\n\t"
						 "pop %0"
						 : "=g"(eflags)
						 :
						 : "memory");
	return eflags;
}

inline void store_eflags(uint32_t eflags)
{
	__asm__ __volatile__("push %0\n\t"
						 "popf"
						 :
						 : "r"(eflags)
						 : "memory", "cc");
}

uint32_t get_cr0(void)
{
	uint32_t cr0;
	__asm__ __volatile__("mov %%cr0, %0" : "=r"(cr0));
	return cr0;
}

void set_cr0(uint32_t cr0)
{
	__asm__ __volatile__("mov %0, %%cr0" : : "r"(cr0));
}

void cpu_get_msr(uint32_t msr, uint32_t *lo, uint32_t *hi)
{
	__asm__ __volatile__("rdmsr" : "=a"(*lo), "=d"(*hi) : "c"(msr));
}

void cpu_set_msr(uint32_t msr, uint32_t lo, uint32_t hi)
{
	__asm__ __volatile__("wrmsr" : : "a"(lo), "d"(hi), "c"(msr));
}

void enable_intr(void) { __asm__ __volatile__("sti"); }

void disable_intr(void) { __asm__ __volatile__("cli" ::: "memory"); }

void krn_halt(void)
{
	disable_intr();
	while (1)
	{
		__asm__("hlt");
	}
}