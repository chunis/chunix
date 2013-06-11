#include <const.h>
#include <printf.h>
#include <hd.h>
#include <buf.h>


struct buf fsbuf[NBUF];
struct buf bhead;

// init buffer cache
void init_buffer(void)
{
	struct buf *bp;

	bhead.next = &bhead;
	bhead.prev = &bhead;

	for(bp = fsbuf; bp < fsbuf+NBUF; bp++){
		bp->dev = -1;
		bp->next = bhead.next;
		bp->prev = &bhead;
		bhead.next->prev = bp;
		bhead.next = bp;
	}
}

void buf_ll_rw(struct buf *bp)
{
	if(MAJOR(bp->dev) == MEM_MAJOR){
		memhd_rw(bp);
	} else if(MAJOR(bp->dev) == HD_MAJOR){
		hd_rw(bp);
	} else {
		printk("buf_ll_rw: unknown dev %d\n", MAJOR(bp->dev));
		panic("buf_ll_rw");
	}
}

// Look through buffer cache for the nblk-th sector on device dev
// If not found, allocate a fresh block.
// In either case, return BUF_BUSY buffer
struct buf *getblk(uint32_t dev, uint32_t nblk)
{
	struct buf *bp;

repeat:
	for(bp = bhead.next; bp != &bhead; bp = bp->next){
		if(bp->dev == dev && bp->num == nblk){
			if((bp->flags & BUF_BUSY) == 0){
				bp->flags |= BUF_BUSY;
				return bp;
			}
			sleep_on(&bp->bwait);
			goto repeat;
		}
	}

	// block not cached yet
	for(bp = bhead.prev; bp != &bhead; bp = bp->prev){
		if((bp->flags & (BUF_BUSY | BUF_DIRTY)) == 0){
			bp->flags = BUF_BUSY;
			bp->dev = dev;
			bp->num = nblk;
			return bp;
		}
	}

	// no cache available
	panic("getblk: no buffers available");
}

struct buf *bread(uint32_t dev, uint32_t nblk)
{
	struct buf *bp;

	bp = getblk(dev, nblk);
	if((bp->flags & BUF_VALID) == 0)	// data invalid
		buf_ll_rw(bp);

	printk("bread() is done\n");
	return bp;
}

void bwrite(struct buf *bp)
{
	if((bp->flags & BUF_BUSY) == 0)
		panic("bwrite: buffer isn't busy");

	bp->flags |= BUF_DIRTY;
	buf_ll_rw(bp);
}

// release a BUF_BUSY buffer, move it to the start of buffer list
void brelse(struct buf *bp)
{
	if((bp->flags & BUF_BUSY) == 0)
		panic("bwrite: buffer isn't busy");

	bp->next->prev = bp->prev;
	bp->prev->next = bp->next;
	bp->next = bhead.next;
	bp->prev = &bhead;
	bhead.next->prev = bp;
	bhead.next = bp;

	bp->flags &= ~BUF_BUSY;
	wakeup(&bp->bwait);
}
