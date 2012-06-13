#include <x86.h>
#include <types.h>

#define CMOS_EXTLO	0x17
#define CMOS_EXTHI	0x18


uint32_t cmos_read(uint32_t r)
{
	outb(0x70, r);
	return inb(0x71);
}

// we won't bother to detect base memory (< 1M), but simply assume it is 640K.
// since if not, there won't have any extend memory for the kernel to be loaded.
static uint32_t detect_mem(void)
{
	return (cmos_read(CMOS_EXTLO) | cmos_read(CMOS_EXTHI) << 8);
}

// we won't use the memory < 1M. Just leave it there for future use. 
// e.g. the bios vectors can be used in vm86 for GUI.
// Note: We can't detect memory > 64M yet.
// it can be done using 'int 15' with eax=E820 in real mode, but it will make
// the bootloader too large to locate at a single sector.
// I'll fix this problem in the future. And now we'll limit the memory to no
// more than 64M in the coming days.
void mem_init(void)
{
	uint32_t extmem;

	extmem = detect_mem();
	printf("memory size: %d KB\n\n", extmem + 1024);
}
