#include <types.h>
#include <x86.h>
#include "hd.h"

// wait until hard disk is ready
void hdwait(void)
{
	int status;

	while( (status = inb(HDR_STATUS)) & (0x80 | 0x40) != 0x40)
		;
}

/* read/write to hard disk
 * cmd: HD_READ/HD_WRITE
 * nb: number of blocks
 * offset: operate start location
 * buf: buffer for read to or write from
 */
void hd_rw(int cmd, int nb, int offset, char *buf)
{
	int rw;
	int dev = 0xe0 | (1 << 4);	// we use the slave hd currently

	if(cmd != HD_READ && cmd != HD_WRITE){
		printf("ERROR! cmd '%d' doesn't support!\n", cmd);
		return;
	}

	hdwait();

	outb(HDR_ALTER_STATUS, 0); // generate interrupt

	outb(HDR_NSECT, nb);   // 0x1f2
	outb(HDR_LBA_LOW, (offset & 0xff));  // 0x1f3
	outb(HDR_LBA_MID, (offset >> 8) & 0xff);  // 0x1f4
	outb(HDR_LBA_HI, (offset >> 16) & 0xff);   // 0x1f5
	outb(HDR_DEVICE, (dev | ((offset >> 24) & 0x0f)));   // 0x1f6
	outb(HDR_STATUS, cmd);   // 0x1f7

	if(cmd == HD_READ)
		insl(HDR_DATA, buf, SECT_SIZE*nb/4);  // 0x1f0
	else
		outsl(HDR_DATA, buf, SECT_SIZE*nb/4);  // 0x1f0
}

// test basic hard disk read and write
static void hd_identify(int drv)
{
	char buf[512];
	char str[512] = "Hello, chunix";

	printf("Hello, in hd_identify now\n");

	hd_rw(HD_READ, 1, 0, buf);
	printf("before write: %s\n", buf);

	hd_rw(HD_WRITE, 1, 0, str);
	hd_rw(HD_READ, 1, 0, buf);
	printf("after write: %s\n", buf);
}

// enable interrupt, and setup isr, then do something else
void init_hd(void)
{
	outb(0xa1, 0xbf);   // IRQ 14 (IDE) locates at slave 5259A
	printf("IRQ mask = %b, %b\n", inb(0xa1), inb(0x21));

	hd_identify(0);
}

void hd_isr(void)
{
	uint8_t sta = inb(HDR_STATUS);
}
