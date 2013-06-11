// Faked hard disk.
// Should be useful for access initrd_ext2 fs loaded by grub.

#include <printf.h>
#include <types.h>
#include <buf.h>
#include <fs.h>


static uint32_t disksize = 0;
static uint8_t *hdstart = NULL;

void init_memhd(uint8_t *start, uint32_t size)
{
	hdstart = start;
	disksize = size/512;
	printk("memhd starts from %x, size = %d sectors\n", hdstart, disksize);
}

// interrupt handler
void memhd_intr(void)
{
	// need do nothing
}

void memhd_rw(struct buf *b)
{
	uint8_t *p;

	if(!(b->flags & BUF_BUSY))
		panic("memhd_rw: buf not busy");
	if((b->flags & (BUF_VALID | BUF_DIRTY)) == BUF_VALID)
		panic("memhd_rw: nothing to do");
	if(MAJOR(b->dev) != MEM_MAJOR)
		panic("memhd_rw: request not for memhd");
	if(b->num >= disksize)
		panic("memhd_rw: sector out of range");

	p = hdstart + b->num * 512;
	if(b->flags & BUF_DIRTY){
		b->flags &= ~BUF_DIRTY;
		memmove(p, b->data, 512);
	} else {
		memmove(b->data, p, 512);
	}

	b->flags |= BUF_VALID;
}

