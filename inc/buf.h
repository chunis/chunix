#ifndef __BUF_H__
#define __BUF_H__

struct buf {
	int flag;
	uint32_t dev;
	uint32_t num;
	uint8_t data[512];

	void *bwait;
	struct buf *prev;
	struct buf *next;
	struct buf *hdnext;
};

#endif
