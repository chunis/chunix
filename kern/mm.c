#include <x86.h>
#include <types.h>
#include <mmu.h>
#include "mm.h"

struct pglink *free_list;	// free page link
static uint32_t memsz;		// memory size by KB
extern char end[];	// defined in kernel.ld

uint32_t cmos_read(uint32_t r)
{
	outb(0x70, r);
	return inb(0x71);
}

void kinit(void *vastart, void *vaend)
{
	kfree_range(vastart, vaend);
}

void kfree_range(void *start, void *end)
{
	uint8_t *p = (uint8_t *)PGROUNDUP((uint32_t)start);

	for(; p + PGSIZE <= end; p += PGSIZE)
		kfree(p);
}

// free page pointed by v. v must aligned to PGSIZE
void kfree(void *v)
{
	struct pglink *fp;

	if((uint32_t)v % PGSIZE || v < end || v > V2P(memsz*1024)){
		printf("kfree: v = %x\n", v);
		panic("kfree");
	}

	memset(v, 1, PGSIZE);

	fp = (struct pglink *)v;
	fp->next = free_list;
	free_list = fp;
}

// Allocate one 4096-bytes Page.
// Return pointer to the page if success, or NULL if out of memory
char *kalloc(void)
{
	struct pglink *fp;

	fp = free_list;
	if(fp)
		free_list = fp->next;
	return (char *)fp;
}

// return the address of PTE in pgdir that corresponds to virtual address va.
// if alloc != 0, alloc pages needed to create page table.
// if fails, return 0 (= NULL).
static pte_t *pgdir_walk(pde_t *pgdir, const void *va, int alloc)
{
	pde_t *pde;
	pte_t *pte;
	struct Page *pp = 0;

	pde = &pgdir[PDX(va)];

	if(*pde & PTE_P) {	// page table exists
		pte = (pte_t *)P2V(PTE_ADDR(*pde));
		return (pte_t *)(pte + PTX(va));
	}

	if(!alloc || (pte = (pte_t *)kalloc()) == 0)
		return 0;

	memset(pte, 0, PGSIZE);
	*pde = PTE_ADDR(V2P(pte)) | PTE_U | PTE_W | PTE_P;

	return (pte_t *)(pte + PTX(va));
}

// we won't bother to detect base memory (< 1M), but simply assume it is 640K.
// since if not, there won't have any extend memory for the kernel to be loaded.
static uint32_t detect_mem(void)
{
	return (cmos_read(CMOS_EXTLO) | cmos_read(CMOS_EXTHI) << 8);
}

// we won't use the memory < 1M. Just leave it there for future use. 
// e.g. the bios vectors can be used in vm86 for GUI.
// Note: We can't detect memory > 64M yet.
// it can be done using 'int 15' with eax=E820 in real mode, but it will make
// the bootloader too large to locate at a single sector.
// I'll fix this problem in the future. And now we'll limit the memory to no
// more than 64M in the coming days.
void mem_init(void)
{
	memsz = detect_mem() + 1024;
	printf("memory size: %d KB\n\n", memsz);

	kinit(end, P2V(4*1024*1024));
}
