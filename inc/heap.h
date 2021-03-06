//
// This file is part of ChuniX and is released under the terms of
// GNU GPLv3 License, see License for details.
//
// Copyright (C) 2014, Chunis Deng (<chunchengfh@gmail.com>)
//
#ifndef __HEAP_H__
#define __HEAP_H__

#include <types.h>


#define HEAP_START	0xD0000000
#define HEAP_END	0xFFBFF000

struct heap_head {
	struct heap_head *prev, *next;
	uint32_t free: 1;
	uint32_t len: 31;
};

void heap_init(void);
void *kmalloc(uint32_t n);
void kfree(void *p);

#endif
