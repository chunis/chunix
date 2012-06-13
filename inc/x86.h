#ifndef __X86_H__
#define __X86_H__

#include <types.h>

static __inline void
insl(int port, void *addr, int count) __attribute__((always_inline));

static __inline uint8_t
inb(int port)
{
	uint8_t val;
	__asm__ __volatile__("inb %w1,%0" : "=a"(val) : "d"(port));
	return val;
}

static __inline void
insl(int port, void *addr, int count)
{
	__asm__ __volatile__("cld\n\trepne\n\tinsl" : "=D"(addr), "=c"(count)
				: "d"(port), "0"(addr), "1"(count)
				: "memory", "cc");
}

static __inline uint8_t
outb(int port, uint8_t val)
{
	__asm__ __volatile__("outb %0, %w1" : : "a"(val), "d"(port));
}

static __inline void
outsl(int port, void const *addr, int count)
{
	__asm__ __volatile__("cld\n\trepne\n\toutsl" : "=S"(addr), "=c"(count)
				: "d"(port), "0"(addr), "1"(count)
				: "cc");
}

static __inline void
stosb(void *addr, int val, int count)
{
	__asm__ __volatile__("cld; rep stosb" : "=D" (addr), "=c" (count)
				: "0" (addr), "1" (count), "a" (val)
				: "memory", "cc");
}

static __inline void
stosl(void *addr, int val, int count)
{
	__asm__ __volatile__("cld; rep stosl" : "=D" (addr), "=c" (count)
				: "0" (addr), "1" (count), "a" (val)
				: "memory", "cc");
}

#endif
