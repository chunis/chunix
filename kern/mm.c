#include <x86.h>
#include <types.h>
#include <mmu.h>
#include <printf.h>
#include "mm.h"
#include "task.h"

struct pglink *free_list;	// free page link
static uint32_t memsz;		// memory size by KB
extern char data[], end[];	// defined in kernel.ld
static uint32_t mem_start;
pde_t *kpgdir;

pte_t *pgdir_walk(pde_t *pgdir, const void *va, int alloc);

// create PTEs for va which refer to physical address pa.
// Note: va and size might not be page-aligned.
// return 0 when succeed, and -1 if fails
int mappages(pde_t *pgdir, void *va, uint32_t pa, int sz, int perm)
{
	char *_va = (char *)PGROUNDDOWN((uint32_t)va);
	char *end = (char *)PGROUNDDOWN(((uint32_t)va) + sz - 1);
	pte_t *pte;

	while(_va <= end){
		if((pte = pgdir_walk(pgdir, _va, 1)) == 0)
			return -1;
		if(*pte & PTE_P)
			panic("mappages: re-map");
		*pte = PTE_ADDR(pa) | perm | PTE_P;

		_va += PGSIZE;
		pa += PGSIZE;
	}
	return 0;
}

// unmap PTE for va. va must be page-aligned.
void unmap_page(pde_t *pgdir, void *va)
{
	pte_t *pte;
	uint32_t pa;

	if((pte = pgdir_walk(pgdir, va, 0)) == 0)
		panic("unmap_page: not mapped");
	if(!(*pte & PTE_P))
		panic("unmap_page: not present");

	pa = V2P(PTE_ADDR(*pte));
	*pte = 0;
	kfree_page(pa);
}

pde_t *mapkvm(void)
{
	pde_t *pgdir;

	if(!(pgdir = kalloc_page()))
		return 0;

	memset(pgdir, 0, PGSIZE);
	if(mappages(pgdir, (void *)KERNBASE, 0, EXTMEM, PTE_W) < 0)
		return 0;
	if(mappages(pgdir, (void *)KERNLINK, (uint32_t)V2P(KERNLINK),
			((uint32_t)data - (uint32_t)KERNLINK), 0) < 0)
		return 0;

	if(mappages(pgdir, (void *)data, (uint32_t)V2P(data),
			(memsz*1024 - V2P((uint32_t)data)), PTE_W) < 0)
		return 0;

	return pgdir;
}

void setupkvm(void)
{
	if((kpgdir = mapkvm()) == 0)
		panic("kpgdir is NULL!");
	lcr3(V2P((uint32_t)kpgdir));
}

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
		kfree_page(p);
}

// allocate len bytes of physical memory for task tp,
// and map it at virtual address va.
void region_alloc(pde_t *pgdir, void *va, uint32_t len, int perm)
{
	pte_t *pte;
	char *p;
	uint32_t _va = PGROUNDDOWN((uint32_t)va);
	uint32_t _va_end = PGROUNDUP((uint32_t)va+len);

	while(_va < _va_end){
		pte = pgdir_walk(pgdir, (void *)_va, 1);
		if(!pte)
			panic("region_alloc failed!");
		if(*pte & PTE_P){
			printk("WARNING!! %x is already mapped\n", _va);
		} else {
			p = kalloc_page();
			if(!p)
				panic("region_alloc failed!");
			*pte = PTE_ADDR(V2P((uint32_t)p)) | perm | PTE_P;
		}
		_va += PGSIZE;
	}
}

// free page pointed by v. v must aligned to PGSIZE
void kfree_page(void *v)
{
	struct pglink *fp;

	if((uint32_t)v % PGSIZE || (uint32_t)v < mem_start || (uint32_t)v > V2P(memsz*1024)){
		printk("kfree_page: v = %x\n", v);
		panic("kfree_page");
	}

	memset(v, 1, PGSIZE);

	fp = (struct pglink *)v;
	fp->next = free_list;
	free_list = fp;
}

// Allocate one 4096-bytes Page.
// Return pointer to the page if success, or NULL if out of memory
char *kalloc_page(void)
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
pte_t *pgdir_walk(pde_t *pgdir, const void *va, int alloc)
{
	pde_t *pde;
	pte_t *pte;

	pde = &pgdir[PDX(va)];

	if(*pde & PTE_P) {	// page table exists
		pte = (pte_t *)P2V(PTE_ADDR(*pde));
		return (pte_t *)(pte + PTX(va));
	}

	if(!alloc || (pte = (pte_t *)kalloc_page()) == 0)
		return 0;

	memset(pte, 0, PGSIZE);
	*pde = PTE_ADDR(V2P((uint32_t)pte)) | PTE_U | PTE_W | PTE_P;

	return (pte_t *)(pte + PTX(va));
}

int dealloc_uvm(pde_t *pgdir, uint32_t oldsz, uint32_t newsz)
{
	// TODO
	return 0;
}

// Free a page table and all the physical memory pages
// in the user part.
void free_vm(pde_t *pgdir)
{
	int i;

	if(pgdir == NULL)
		panic("free_vm: pgdir is NULL");

	dealloc_uvm(pgdir, KERNBASE, 0);
	for(i = 0; i < 1024; i++){
		if(pgdir[i] & PTE_P){
			char *v = P2V(PTE_ADDR(pgdir[i]));
			kfree_page(v);
		}
	}
	kfree_page((char*)pgdir);
}

// copy a page from current task to task 'tp'.
// alloc a new page for tp and map it, then copy the content from 'addr'.
// Note: addr should be page-aligned.
pde_t* copy_page(pde_t *pgdir, uint32_t addr)
{
	pte_t *pte;
	uint32_t pa, flags;
	char *mem;

	if((pte = pgdir_walk(current->pgdir, (void *)addr, 0)) == 0)
		panic("copy_page: pte doesn't exist");
	if(!(*pte & PTE_P))
		panic("copy_page: page doesn't present");
	pa = PTE_ADDR(*pte);
	flags = PTE_FLAGS(*pte);
	if((mem = kalloc_page()) == 0){
		return NULL;
	}

	memmove(mem, (char*)P2V(pa), PGSIZE);
	if(mappages(pgdir, (void*)addr, V2P(mem), PGSIZE, flags) < 0){
		kfree_page(mem);
		return NULL;
	}
	return pgdir;
}

// copy a parent process's page table for child
pde_t* copy_uvm(pde_t *pgdir, uint32_t sz)
{
	pde_t *pde;
	pte_t *pte;
	uint32_t pa, i, flags;
	char *mem;

	if((pde = mapkvm()) == 0)
		return 0;
	for(i = 0; i < sz; i += PGSIZE){
		if((pte = pgdir_walk(pgdir, (void *)i, 0)) == 0)
			panic("copy_uvm: pte doesn't exist");
		if(!(*pte & PTE_P))
			panic("copy_uvm: page doesn't present");
		pa = PTE_ADDR(*pte);
		flags = PTE_FLAGS(*pte);
		if((mem = kalloc_page()) == 0){
			free_vm(pde);
			return NULL;
		}

		memmove(mem, (char*)P2V(pa), PGSIZE);
		if(mappages(pde, (void*)i, V2P(mem), PGSIZE, flags) < 0){
			free_vm(pde);
			return NULL;
		}
	}
	return pde;
}

char* uva2ka(pde_t *pgdir, char *uva)
{
	pte_t *pte;

	pte = pgdir_walk(pgdir, uva, 0);
	if((*pte & PTE_P) == 0)
		return 0;
	if((*pte & PTE_U) == 0)
		return 0;
	return (char*)P2V(PTE_ADDR(*pte));
}

// Copy len bytes from p to user address va in page table pgdir.
// Most useful when pgdir is not the current page table.
// uva2ka ensures this only works for PTE_U pages.
int copyout(pde_t *pgdir, uint32_t va, void *p, uint32_t len)
{
	char *buf, *pa0;
	uint32_t n, va0;

	buf = (char*)p;
	while(len > 0){
		va0 = (uint32_t)PGROUNDDOWN(va);
		pa0 = uva2ka(pgdir, (char*)va0);
		if(pa0 == 0)
			return -1;
		n = PGSIZE - (va - va0);
		if(n > len)
			n = len;
		memmove(pa0 + (va - va0), buf, n);
		len -= n;
		buf += n;
		va = va0 + PGSIZE;
	}
	return 0;
}


// we won't bother to detect base memory (< 1M), but simply assume it is 640K.
// since if not, there won't have any extend memory for the kernel to be loaded.
static uint32_t detect_mem(void)
{
	return (cmos_read(CMOS_EXTLO) | cmos_read(CMOS_EXTHI) << 8);
}

// we won't use the memory < 1M. Just leave it there for future use. 
// e.g. the bios vectors can be used in vm86 for GUI.
// If caller doesn't give a positive size, we'll have to detect it.
// Note: We can't detect memory > 64M yet.
// it can be done using 'int 15' with eax=E820 in real mode, but it will make
// the bootloader too large to locate at a single sector.
void mem_init1(uint32_t *start, uint32_t size)
{
	mem_start = start;
	memsz = size ? size : (detect_mem() + 1024);
	printk("memory size: %d KB\n\n", memsz);

	kinit(start, P2V(4 * 1024 * 1024));
}

// init memory [4M, memsz]
void mem_init2(void)
{
	kinit(P2V(4 * 1024 * 1024), P2V(memsz * 1024));
}

void page_fault(struct stack_frame *tf)
{
	int present  = tf->err & 0x1 ? 1 : 0;
	int rw       = tf->err & 0x2 ? 1 : 0;
	int user     = tf->err & 0x4 ? 1 : 0;
	int reserved = tf->err & 0x8 ? 1 : 0;
	uint32_t fault_addr;

	__asm__ __volatile__("mov %%cr2, %0" : "=r" (fault_addr));
	printk("\npage fault! (flags: p:%d, rw:%d, user:%d, res:%d)\n",
			present, rw, user, reserved);
	printk("eip = %x, faulting address %x\n", tf->eip, fault_addr);
}
