//
// This file is part of ChuniX and is released under the terms of
// GNU GPLv3 License, see License for details.
//
// Copyright (C) 2014, Chunis Deng (<chunchengfh@gmail.com>)
//
#ifndef __STAT_H__
#define __STAT_H__

#include <types.h>

struct stat {
	dev_t st_dev;
	uint32_t st_mode;
	off_t st_size;
};

#endif
