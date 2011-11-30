#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#include <types.h>

#define GDT_NUM		256

uint8_t gdt[GDT_NUM];
uint8_t gdt_ptr[6];

#endif
