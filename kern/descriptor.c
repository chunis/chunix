//
// This file is part of ChuniX and is released under the terms of
// GNU GPLv3 License, see License for details.
//
// Copyright (C) 2014, Chunis Deng (<chunchengfh@gmail.com>)
//
#include <types.h>
#include <const.h>
#include <trap.h>
#include <printf.h>
#include <string.h>

#include "task.h"
#include "descriptor.h"

extern uint32_t isr_table[ISR_NUM];

// add an descriptor entry to GDT
void set_descriptor(struct descriptor *desp, uint32_t base,
		uint32_t lim, uint16_t attr)
{
	desp->base_low = base & 0xffff;
	desp->base_mid = (base >> 16) & 0xff;
	desp->base_high = base >> 24;
	desp->lim_low = lim & 0xffff;
	desp->attr1 = attr & 0xff;
	desp->attr2_lim_high = ((attr >> 8) & 0xf0) | ((lim >> 16) & 0x0f);

}

// add an gate entry to GDT
void set_gate(struct gate *gatep, uint32_t offset, uint8_t attr, uint16_t sel)
{
	gatep->offset_low = offset & 0xffff;
	gatep->offset_high = (offset >> 16) & 0xffff;
	gatep->sel = sel;
	gatep->dcount = 0x00;	// be 0
	gatep->attr = attr; // 0x8e
}

void dump_descriptor(struct descriptor *desp)
{
	uint32_t base, lim;
	uint16_t attr;

	base = (desp->base_high << 24) | (desp->base_mid << 16) | desp->base_low;
	lim = desp->lim_low | ((desp->attr2_lim_high & 0x0f) << 16);
	attr = desp->attr1 | ((desp->attr2_lim_high &0xf0) << 8);

	printk("base: %x, lim: %x, attr: %x\n", base, lim, attr);
}

// add items to gdt, include kernel/user code/data segment and tss
void init_gdt(void)
{
	uint32_t *gdt_base;
	uint16_t *gdt_lim;

	memset(gdt, 0, sizeof(gdt));
	set_descriptor((struct descriptor *)&gdt[KER_CODE], 0,
			0xfffff, 0xC0<<8 | DA_CR);
	set_descriptor((struct descriptor *)&gdt[KER_DATA], 0,
			0xfffff, 0xC0<<8 | DA_DRW);
	set_descriptor((struct descriptor *)&gdt[USR_CODE], 0,
			0xfffff, (0xC0<<8 | DA_DPL3 | DA_CR));
	set_descriptor((struct descriptor *)&gdt[USR_DATA], 0,
			0xfffff, (0xC0<<8 | DA_DPL3 | DA_DRW));
	// for tss
	memset(&tss, 0, sizeof(tss));
	tss.ss0 = KER_DATA;
	set_descriptor((struct descriptor *)&gdt[KER_TSS], (uint32_t)&tss,
			sizeof(tss)-1, DA_386TSS);

	// load gdt
	gdt_base = (uint32_t *)(&gdt_ptr[2]);
	gdt_lim = (uint16_t *)(&gdt_ptr[0]);
	*gdt_base = (uint32_t)&gdt;
	*gdt_lim = GDT_NUM * 8 - 1;
	__asm__ __volatile__("lgdt gdt_ptr");
}

// all isrs are Interrupt Gate
void init_idt(void)
{
	uint32_t *idt_base;
	uint16_t *idt_lim;
	int i;

	memset(idt, 0, sizeof(idt));

	for(i=0; i<ISR_NUM; i++){
		set_gate(&idt[i], isr_table[i], 0x8e, KER_CODE);
	}

	// syscall, DPL=3
	set_gate(&idt[T_SYSCALL], isr_table[T_SYSCALL], 0xee, KER_CODE);

	idt_base = (uint32_t *)(&idt_ptr[2]);
	idt_lim = (uint16_t *)(&idt_ptr[0]);
	*idt_base = (uint32_t)&idt;
	*idt_lim = IDT_NUM * 8 - 1;
	__asm__ __volatile__("lidt idt_ptr");
}

void dump_gdt(void)
{
	int i;
	uint32_t *gp = (uint32_t *)&gdt;

	printk("\n------- dump_gdt() start ------\n");
	for(i=0; i<6; i++) {
		dump_descriptor((struct descriptor *)gp);
		gp += 2;
	}
	printk("------- dump_gdt() end --------\n");
}
