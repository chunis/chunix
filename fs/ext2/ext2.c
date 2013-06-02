#include <types.h>
#include <const.h>
#include <fs.h>
#include <ext2.h>
#include <printf.h>


extern enum fs_type root_fs_type;

static struct superblock *ext2_read_super(struct super_block *sb)
{
	printk("In ext2_read_super\n");

	if(root_fs_type != EXT2)
		return NULL;

	printk("Enter ext2_read_super now\n");
	return sb;
}

struct fs_node ext2_fs = {
	ext2_read_super,
	"ext2",
	1,
	NULL,
};

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

	printk("start_addr = %x, ", start_addr);
	printk("superblock Magic = %x\n", (int)sb_ptr->magic);
	assert(sb_ptr->magic == EXT2_SUPER_MAGIC);

	printk("inodes count: %d, ", sb_ptr->inodes_count);
	printk("block count: %d, ", sb_ptr->blocks_count);
	printk("blocks reserved for root: %d\n", sb_ptr->r_blocks_count);

	printk("free blocks: %d, ", sb_ptr->free_blocks_count);
	printk("free inodes: %d\n", sb_ptr->free_inodes_count);

	printk("first block start: %d, ", sb_ptr->first_data_block);
	block_size = 1024 << sb_ptr->log_block_size;
	printk("a block = %d bytes, ", block_size);
	printk("a frag = %d bytes\n", 1024 << sb_ptr->log_frag_size);

	printk("each group has %d blocks, %d fragments or %d inodes\n",
			sb_ptr->blocks_per_group, sb_ptr->frags_per_group,
			sb_ptr->inodes_per_group);

	printk("last mount: %x, last write: %x\n", sb_ptr->mtime, sb_ptr->wtime);
	printk("mounts %d times since verified, ", sb_ptr->mnt_count);
	printk("and need to be verified after %d mounts\n",
			sb_ptr->max_mnt_count - sb_ptr->mnt_count);

	printk("inodes are %d bytes\n", (int)sb_ptr->inode_size);

	bg_ptr = (struct ext2_bg_descriptor *)(start_addr + EXT2_SB_OFFSET + EXT2_SB_SIZE);
	printk("\nNow come to the first block group...\n");
	printk("It has %d free blocks, %d free inodes and %d used dirs\n",
			bg_ptr->free_blocks_count, bg_ptr->free_inodes_count,
			 bg_ptr->used_dirs_count);

	inodetable = (struct ext2_inode *)(start_addr + block_size * bg_ptr->inode_table);
	for(i = 0; i < sb_ptr->inodes_per_group; i++){
		inode = (struct ext2_inode *)((int)inodetable + (int)sb_ptr->inode_size * i);
		if(inode->block[0] == 0)
			continue;
		printk("Inode %d starts from block %d, = %d bytes (%d blocks)\n",
				i, inode->block[0], inode->size, inode->blocks);
		if(inode->mode & EXT2_S_IFDIR){
			printk("It is a directory. Files listing:\n");

			dir_offset = 0;
			while(dir_offset < inode->size){
				dirent = (struct ext2_inode *)(start_addr + block_size * inode->block[0] + dir_offset);
				if(dirent->inode == 0)
					break;

				printk("[offset: %d] name: %s [inode: %d]\n", dir_offset, dirent->name, dirent->inode);
				if(! strncmp(dirent->name, "motd", dirent->name_len)){
					settextcolor(14, 0);
					printk("Find file 'motd'. ");
					inode_f = (struct ext2_inode *)((uint32_t)inodetable + sb_ptr->inode_size * (dirent->inode - 1));
					printk("file size: %d, start from block %d.\n", inode_f->size, inode_f->block[0]);

					printk("Its content:\n");
					settextcolor(12, 0);
					file_ptr = (uint8_t *)(start_addr + block_size * inode_f->block[0]);
					for(j = 0; j < inode_f->size; j++)
						printk("%c", file_ptr[j]);
					settextcolor(13, 0);
				}

				dir_offset += dirent->rec_len;
			}
		}
	}
}

void add_ext2_fs(void)
{
	register_filesystem(&ext2_fs);
}

void del_ext2_fs(void)
{
	unregister_filesystem(&ext2_fs);
}
