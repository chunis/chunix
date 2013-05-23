#include <types.h>
#include <ext2.h>

void dump_ext2(uint32_t start_addr)
{
	struct ext2_super_block *sb_ptr;

	sb_ptr = (struct ext2_super_block *)start_addr;

	printf("start_addr = %x\n", start_addr);
	printf("superblock Magic = %x\n", (int)sb_ptr->magic);
}
