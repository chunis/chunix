#include <types.h>
#include <x86.h>
#include <hd.h>
#include <printf.h>
#include <fs_minix.h>
#include "../kern/sched.h"


static int dev = 0xe0 | (1 << 4);  // we always use the slave hd currently
static struct buf *hdqueue;  // hard disk queue, keep all requested buffers

// wait until hard disk is ready
void hdwait(void)
{
	int status;

	while( ((status = inb(HDR_STATUS)) & (0x80 | 0x40)) != 0x40)
		;
}

// hard disk do the real read/write work here
static void hd_dorw(struct buf *bp)
{
	if(!bp)
		panic("hd_dorw: bp is NULL");

	hdwait();

	outb(HDR_ALTER_STATUS, 0); // generate interrupt
	outb(HDR_NSECT, 1);   // 0x1f2
	outb(HDR_LBA_LOW, (bp->num & 0xff));  // 0x1f3
	outb(HDR_LBA_MID, (bp->num >> 8) & 0xff);  // 0x1f4
	outb(HDR_LBA_HI, (bp->num >> 16) & 0xff);   // 0x1f5
	outb(HDR_DEVICE, (dev | ((bp->num >> 24) & 0x0f)));   // 0x1f6

	if(bp->flag & BUF_DIRTY){  // write
		outb(HDR_STATUS, HD_WRITE);   // 0x1f7
		outsl(HDR_DATA, bp->data, SECT_SIZE/4);  // 0x1f0
	} else {
		outb(HDR_STATUS, HD_READ);   // 0x1f7
		// copy data happened after hd issues interrupt
	}
}

void dump_data(char *p, int n)
{
	int i;

	printk("---- dump_data begin ----\n");
	for(i=0; i < n; i++)
		printk("%x ", p[i]);
	printk("\n---- dump_data end ----\n");
}

// issue read/write request to hard disk
// just append bp to hdqueue, and call hd_dorw() if necessory
void hd_rw(struct buf *bp)
{
	struct buf **bpp;

	if(!(bp->flag & BUF_BUSY))
		panic("hd_rw: buf is not busy");
	if((bp->flag & (BUF_VALID|BUF_DIRTY)) == BUF_VALID)
		panic("hd_rw: need do nothing");

	// just append bp to hdqueue.
	bp->hdnext = 0;
	for(bpp=&hdqueue; *bpp; bpp=&(*bpp)->hdnext)
		;
	*bpp = bp;

	// Start disk if necessary.
	if(hdqueue == bp)
		hd_dorw(bp);

	// Wait for request to finish.
	while((bp->flag & (BUF_VALID|BUF_DIRTY)) != BUF_VALID){
		sleep_on(&bp->bwait);
	}

	// check if read is OK
	dump_data(bp->data, 24);
}

// hd interrupt handler
void hd_isr(void)
{
	struct buf *bp;

	if((bp = hdqueue) == 0)  // hdqueue is empty
		return;

	// remove bp from hdqueue and process bp
	hdqueue = bp->hdnext;

	if((bp->flag & BUF_DIRTY) == 0){
		printk("Read data out\n");
		hdwait();
		insl(HDR_DATA, bp->data, SECT_SIZE/4);  // 0x1f0
	}

	bp->flag |= BUF_VALID;
	bp->flag &= ~BUF_DIRTY;
	wakeup(&bp->bwait);

	// start next buffer in hdqueue
	if(hdqueue)
		hd_dorw(hdqueue);
}

// enable interrupt, and setup isr, then do something else
void init_hd(void)
{
	outb(0xa1, 0xbf);   // IRQ 14 (IDE) locates at slave 5259A
	printk("IRQ mask = %b, %b\n", inb(0xa1), inb(0x21));
}
