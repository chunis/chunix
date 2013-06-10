#include <types.h>
#include <heap.h>
#include <mmu.h>
#include <printf.h>

uint32_t heap_top = HEAP_START;
uint32_t heap_bot = HEAP_START;
struct heap_head *heap_start = NULL;
extern pde_t *kpgdir;

static void glue_chunk (struct heap_head *chunk);
static void split_chunk (struct heap_head *chunk, uint32_t len);
static void free_chunk (struct heap_head *chunk);
static void alloc_chunk (uint32_t start, uint32_t len);

void heap_init(void)
{
	// nothing needed to do here
}

// return a pointer to a chunk of >= n bytes memery
void *kmalloc(uint32_t n)
{
	uint32_t chunk_start;
	struct heap_head *curp = heap_start, *prevp = NULL;

	n += sizeof(struct heap_head);
	while(curp){
		if(curp->free == 1 && curp->len >= n){
			split_chunk(curp, n);
			curp->free = 0;
			return (void *)((uint32_t)curp + sizeof(struct heap_head));
		}
		prevp = curp;
		curp = curp->next;
	}

	// no big enough hole found
	if(prevp){
		chunk_start = (uint32_t)prevp + prevp->len;
	} else {
		chunk_start = HEAP_START;
		heap_start = (struct heap_head *)chunk_start;
	}

	alloc_chunk(chunk_start, n);
	curp = (struct heap_head *)chunk_start;
	curp->prev = prevp;
	curp->next = NULL;
	curp->free = 0;
	curp->len = n;
	prevp->next = curp;
	return (void *)(chunk_start + sizeof(struct heap_head));
}

// free a chunk of memory allocated with kmalloc for usable again
void kfree(void *p)
{
	struct heap_head *head;

	head = (struct heap_head *)((uint32_t)p - sizeof(struct heap_head));
	head->free = 1;
	glue_chunk(head);
}

static void alloc_chunk (uint32_t start, uint32_t len)
{
	pte_t *page;

	while(start + len > heap_top){
		page = kalloc_page();
		mappages(kpgdir, heap_top, page, PGSIZE, PTE_P | PTE_W);
		heap_top += PGSIZE;
	}
}

static void free_chunk (struct heap_head *chunk)
{
	pte_t *page;

	chunk->prev->next = NULL;
	if(chunk->prev == NULL)
		heap_start = NULL;

	while((uint32_t)chunk <= (heap_top - PGSIZE)){
		heap_top -= PGSIZE;
		page = pgdir_walk(kpgdir, heap_top, 0);
		assert(page);
		kfree_page(page);
		unmap_page(kpgdir, heap_top);
	}
}

// after split, the left space size need to >= (sizeof(heap_head) + 1) bytes.
// otherwise we won't bother to split it
static void split_chunk (struct heap_head *chunk, uint32_t len)
{
	struct heap_head *new;
	uint32_t len_left = chunk->len - len;

	if(len_left <= sizeof(struct heap_head))
		return;

	new = (struct heap_head *)((uint32_t)chunk + len);
	new->free = 1;
	new->len = len_left;
	chunk->len = len;

	new->prev = chunk;
	new->next = chunk->next;
	chunk->next->prev = new;
	chunk->next = new;
}

static void glue_chunk (struct heap_head *chunk)
{
	// try to glue right side first
	if(chunk->next && chunk->next->free == 1){
		chunk->len += chunk->next->len;
		chunk->next->next->prev = chunk;
		chunk->next = chunk->next->next;
	}

	// then try to glue left side
	if(chunk->prev && chunk->prev->free == 1){
		chunk->prev->len += chunk->len;
		chunk->prev->next = chunk->next;
		chunk->next->prev = chunk->prev;
		chunk = chunk->prev;
	}

	// check if chunk is on the top of heap. if so, try to free it.
	if(chunk->next == NULL)
		free_chunk(chunk);
}
