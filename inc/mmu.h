#ifndef __MMU_H__
#define __MMU_H__

#define PGSIZE		4096
#define PTXSHIFT	12
#define PDXSHIFT	22

#define KERNBASE	0x80000000

// page dir/table entry flags
#define PTE_P		0x001	// present
#define PTE_W		0x002	// writable
#define PTE_U		0x004	// user
#define PTE_A		0x020	// accessed
#define PTE_D		0x040	// dirty
#define PTE_PS		0x080	// page size, 0: 4K; 1: 4M

// page dir index
#define PTX(va)		(((uint32_t)(va) >> PTXSHIFT) & 0x3ff)
// page table index
#define PDX(va)		(((uint32_t)(va) >> PDXSHIFT) & 0x3ff)
// assemble page address from d, t and o
#define PGADDR(d, t, o)	((uint32_t)((d) <<PDXSHIFT | (t) << PTXSHIFT | (o)))
// Address in page table or page directory entry
#define PTE_ADDR(pte)	((uint32_t)(pte) & ~0xFFF)

#define V2P(a)		((a) - KERNBASE)
#define P2V(a)		((a) + KERNBASE)

#define PGROUNDUP(a)	(((a)+PGSIZE-1) & ~(PGSIZE-1))
#define PGROUNDDOWN(a)	(((a)) & ~(PGSIZE-1))

#endif