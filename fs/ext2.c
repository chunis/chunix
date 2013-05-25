#include <types.h>
#include <ext2.h>
#include "../kern/printf.h"

void dump_ext2(uint32_t start_addr)
{
	struct ext2_super_block *sb_ptr;
	struct ext2_bg_descriptor *bg_ptr;
	struct ext2_inode *inodetable, *inode, *inode_f;
	struct ext2_dir_entry *dirent;
	uint32_t dir_offset;
	uint8_t *file_ptr;
	char name[256];
	int block_size;
	int i, j;


	sb_ptr = (struct ext2_super_block *)(start_addr + EXT2_SB_OFFSET);

	printf("start_addr = %x, ", start_addr);
	printf("superblock Magic = %x\n", (int)sb_ptr->magic);
	assert(sb_ptr->magic == EXT2_SUPER_MAGIC);

	printf("inodes count: %d, ", sb_ptr->inodes_count);
	printf("block count: %d, ", sb_ptr->blocks_count);
	printf("blocks reserved for root: %d\n", sb_ptr->r_blocks_count);

	printf("free blocks: %d, ", sb_ptr->free_blocks_count);
	printf("free inodes: %d\n", sb_ptr->free_inodes_count);

	printf("first block start: %d, ", sb_ptr->first_data_block);
	block_size = 1024 << sb_ptr->log_block_size;
	printf("a block = %d bytes, ", block_size);
	printf("a frag = %d bytes\n", 1024 << sb_ptr->log_frag_size);

	printf("each group has %d blocks, %d fragments or %d inodes\n",
			sb_ptr->blocks_per_group, sb_ptr->frags_per_group,
			sb_ptr->inodes_per_group);

	printf("last mount: %x, last write: %x\n", sb_ptr->mtime, sb_ptr->wtime);
	printf("mounts %d times since verified, ", sb_ptr->mnt_count);
	printf("and need to be verified after %d mounts\n",
			sb_ptr->max_mnt_count - sb_ptr->mnt_count);

	printf("inodes are %d bytes\n", (int)sb_ptr->inode_size);

	bg_ptr = (struct ext2_bg_descriptor *)(start_addr + EXT2_SB_OFFSET + EXT2_SB_SIZE);
	printf("\nNow come to the first block group...\n");
	printf("It has %d free blocks, %d free inodes and %d used dirs\n",
			bg_ptr->free_blocks_count, bg_ptr->free_inodes_count,
			 bg_ptr->used_dirs_count);

	inodetable = (struct ext2_inode *)(start_addr + block_size * bg_ptr->inode_table);
	for(i = 0; i < sb_ptr->inodes_per_group; i++){
		inode = (struct ext2_inode *)((int)inodetable + (int)sb_ptr->inode_size * i);
		if(inode->block[0] == 0)
			continue;
		printf("Inode %d starts from block %d, = %d bytes (%d blocks)\n",
				i, inode->block[0], inode->size, inode->blocks);
		if(inode->mode & EXT2_S_IFDIR){
			printf("It is a directory. Files listing:\n");

			dir_offset = 0;
			while(dir_offset < inode->size){
				dirent = (struct ext2_inode *)(start_addr + block_size * inode->block[0] + dir_offset);
				if(dirent->inode == 0)
					break;

				printf("[offset: %d] name: %s [inode: %d]\n", dir_offset, dirent->name, dirent->inode);
				if(! strncmp(dirent->name, "motd", dirent->name_len)){
					settextcolor(14, 0);
					printf("Find file 'motd'. ");
					inode_f = (struct ext2_inode *)((uint32_t)inodetable + sb_ptr->inode_size * (dirent->inode - 1));
					printf("file size: %d, start from block %d.\n", inode_f->size, inode_f->block[0]);

					printf("Its content:\n");
					settextcolor(12, 0);
					file_ptr = (uint8_t *)(start_addr + block_size * inode_f->block[0]);
					for(j = 0; j < inode_f->size; j++)
						printf("%c", file_ptr[j]);
					settextcolor(13, 0);
				}

				dir_offset += dirent->rec_len;
			}
		}
	}
}
