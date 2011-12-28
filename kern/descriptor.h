#ifndef __DESCRIPTOR_H__
#define __DESCRIPTOR_H__

#include <types.h>

enum TI_TYPE { TI_GDT, TI_LDT };

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

void set_descriptor(DESCRIPTOR *desp, uint32_t base, uint32_t lim, uint16_t attr);
void set_gate(GATE *gatep, uint32_t offset, uint8_t attr, uint16_t sel);

#endif
