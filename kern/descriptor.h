#ifndef __DESCRIPTOR_H__
#define __DESCRIPTOR_H__

#include <types.h>

#define GDT_NUM		128
#define IDT_NUM		256

typedef struct {
	uint16_t lim_low;
	uint16_t base_low;
	uint8_t base_mid;
	uint8_t attr1;
	uint8_t attr2_lim_high;
	uint8_t base_high;
} DESCRIPTOR;

typedef struct {
	uint16_t offset_low;
	uint16_t sel;
	uint8_t dcount;		// be 0
	uint8_t attr;
	uint16_t offset_high;
} GATE;

uint8_t gdt[GDT_NUM*8];
GATE idt[IDT_NUM];
uint8_t gdt_ptr[6];
uint8_t idt_ptr[6];

void set_descriptor(DESCRIPTOR *desp, uint32_t base, uint32_t lim, uint16_t attr);
void set_gate(GATE *gatep, uint32_t offset, uint8_t attr, uint16_t sel);
void dump_descriptor(DESCRIPTOR *desp);
void init_gdt(void);
void init_idt(void);
void dump_gdt(void);

#endif
