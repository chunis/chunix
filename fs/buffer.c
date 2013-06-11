#include <const.h>
#include <printf.h>
#include <hd.h>
#include <buf.h>
//#include "fs_minix.h"

struct buf fsbuf[NBUF];
struct buf bhead;

// init buffer cache
void init_buffer(void)
{
	struct buf *bp;

	bhead.next = &bhead;
	bhead.prev = &bhead;

	for(bp = fsbuf; bp < fsbuf+NBUF; bp++){
		bp->next = bhead.next;
		bp->prev = &bhead;
		bhead.next->prev = bp;
		bhead.next = bp;
	}
}

struct buf *getblk(uint32_t dev, uint32_t nblk)
{
	struct buf *bp;

repeat:
	for(bp = bhead.next; bp != &bhead; bp = bp->next){
		if(bp->dev == dev && bp->num == nblk){
			if((bp->flag & BUF_BUSY) == 0){
				bp->flag |= BUF_BUSY;
				return bp;
			}
			sleep_on(&bp->bwait);
			goto repeat;
		}
	}

	// block not cached yet
	for(bp = bhead.prev; bp != &bhead; bp = bp->prev){
		if((bp->flag & (BUF_BUSY | BUF_DIRTY)) == 0){
			bp->flag |= BUF_BUSY;
			bp->dev = dev;
			bp->num = nblk;
			return bp;
		}
	}

	// not cache available
	panic("getblk: no buffers available");
}

struct buf *bread(uint32_t dev, uint32_t nblk)
{
	struct buf *bp;

	bp = getblk(dev, nblk);
	if((bp->flag & BUF_VALID) == 0)	// data invalid
		hd_rw(bp);

	printk("bread() is done\n");
	return bp;
}

void bwrite(struct buf *bp)
{
	if((bp->flag & BUF_BUSY) == 0)
		panic("bwrite: buffer isn't busy");

	bp->flag |= BUF_DIRTY;
	hd_rw(bp);
	bp->flag &= ~(BUF_BUSY | BUF_DIRTY);
}

// release a BUF_BUSY buffer, move it to the start of buffer list
void brelse(struct buf *bp)
{
	if((bp->flag & BUF_BUSY) == 0)
		panic("bwrite: buffer isn't busy");

	bp->next->prev = bp->prev;
	bp->prev->next = bp->next;
	bp->next = bhead.next;
	bp->prev = &bhead;
	bhead.next->prev = bp;
	bhead.next = bp;

	bp->flag &= ~BUF_BUSY;
	wakeup(&bp->bwait);
}
