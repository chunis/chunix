// Faked hard disk.
// Should be useful for access initrd_ext2 fs loaded by grub.

#include <printf.h>
#include <types.h>


void init_memhd(void)
{
	printk("mem hd inited\n");
}

// interrupt handler
void memhd_intr(void)
{
	// need do nothing
}

