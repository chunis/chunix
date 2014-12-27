//
// This file is part of ChuniX and is released under the terms of
// GNU GPLv3 License, see License for details.
//
// Copyright (C) 2014, Chunis Deng (<chunchengfh@gmail.com>)
//
#ifndef __MM_H__
#define __MM_H__

#define CMOS_EXTLO	0x17
#define CMOS_EXTHI	0x18

struct pglink {
	struct pglink *next;
};


uint32_t cmos_read(uint32_t r);
void kinit(void *vastart, void *vaend);
void kfree_range(void *start, void *end);
void region_alloc(pde_t *pgdir, void *va, uint32_t len, int perm);
char *kalloc_page(void);
void kfree_page(void *v);
pte_t *pgdir_walk(pde_t *pgdir, const void *va, int alloc);
void mem_init1(uint32_t *start, uint32_t size);
void mem_init2(void);
void setupkvm(void);
pde_t *mapkvm(void);

#endif
