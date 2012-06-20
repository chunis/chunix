#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#include <types.h>
#include "task.h"

#define GDT_NUM		128
#define IDT_NUM		256
#define ISR_NUM		0x30

#define KER_NULL	0x00
#define KER_CODE	0x08
#define KER_DATA	0x10
#define USR_CODE	0x18
#define USR_DATA	0x20
#define KER_TSS 	0x28

uint8_t gdt[GDT_NUM*8];
uint8_t idt[IDT_NUM*8];
uint8_t gdt_ptr[6];
uint8_t idt_ptr[6];
TSS_STRUCT tss;

#endif
