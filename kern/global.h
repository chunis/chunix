#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#include <types.h>

#define GDT_NUM		128
#define IDT_NUM		256

uint8_t gdt[GDT_NUM*8];
uint8_t idt[IDT_NUM*8];
uint8_t gdt_ptr[6];
uint8_t idt_ptr[6];

#endif
