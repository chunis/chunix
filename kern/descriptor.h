//
// This file is part of ChuniX and is released under the terms of
// GNU GPLv3 License, see License for details.
//
// Copyright (C) 2014, Chunis Deng (<chunchengfh@gmail.com>)
//
#ifndef __DESCRIPTOR_H__
#define __DESCRIPTOR_H__

#include <types.h>

#define ISR_NUM		0x30
#define GDT_NUM		128
#define IDT_NUM		256

struct descriptor {
	uint16_t lim_low;
	uint16_t base_low;
	uint8_t base_mid;
	uint8_t attr1;
	uint8_t attr2_lim_high;
	uint8_t base_high;
};

struct gate {
	uint16_t offset_low;
	uint16_t sel;
	uint8_t dcount;		// be 0
	uint8_t attr;
	uint16_t offset_high;
};

uint8_t gdt[GDT_NUM*8];
struct gate idt[IDT_NUM];
uint8_t gdt_ptr[6];
uint8_t idt_ptr[6];

void set_descriptor(struct descriptor *desp, uint32_t base, uint32_t lim, uint16_t attr);
void set_gate(struct gate *gatep, uint32_t offset, uint8_t attr, uint16_t sel);
void dump_descriptor(struct descriptor *desp);
void init_gdt(void);
void init_idt(void);
void dump_gdt(void);

#endif
