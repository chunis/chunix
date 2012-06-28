#ifndef __FS_H__
#define __FS_H__

#include <types.h>

struct buf {
	int flag;
	uint32_t dev;
	uint32_t num;
	uint32_t data[512];

	struct buf *prev;
	struct buf *next;
	struct buf *fnext;
};

#define BUF_BUSY  0x1	// buffer is locked
#define BUF_VALID 0x2	// buffer data is valid
#define BUF_DIRTY 0x4	// buffer data needs to be written to disk

#endif
