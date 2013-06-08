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
char *kalloc(void);
void kfree_page(void *v);
pte_t *pgdir_walk(pde_t *pgdir, const void *va, int alloc);
void mem_init(void);
void setupkvm(void);
pde_t *mapkvm(void);

#endif
