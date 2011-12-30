#include <types.h>

#include "descriptor.h"
#include "printf.h"

// add an descriptor entry to GDT
void set_descriptor(DESCRIPTOR *desp, uint32_t base,
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
void set_gate(GATE *gatep, uint32_t offset, uint8_t attr, uint16_t sel)
{
	gatep->offset_low = offset & 0xffff;
	gatep->offset_high = (offset >> 16) & 0xffff;
	gatep->sel = sel;
	gatep->dcount = 0x00;	// be 0
	gatep->attr = attr; // 0x8e
}

void dump_descriptor(DESCRIPTOR *desp)
{
	uint32_t base, lim;
	uint16_t attr;

	base = (desp->base_high << 24) | (desp->base_mid << 16) | desp->base_low;
	lim = desp->lim_low | ((desp->attr2_lim_high & 0x0f) << 16);
	attr = desp->attr1 | ((desp->attr2_lim_high &0xf0) << 8);

	printf("base: %x, lim: %x, attr: %x\n", base, lim, attr);
}
