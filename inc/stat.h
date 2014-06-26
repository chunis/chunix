#ifndef __STAT_H__
#define __STAT_H__

#include <types.h>

struct stat {
	dev_t st_dev;
	uint32_t st_mode;
	off_t st_size;
};

#endif
