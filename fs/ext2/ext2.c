//
// This file is part of ChuniX and is released under the terms of
// GNU GPLv3 License, see License for details.
//
// Copyright (C) 2014, Chunis Deng (<chunchengfh@gmail.com>)
//
#include <types.h>
#include <const.h>
#include <hd.h>
#include <fs.h>
#include <ext2.h>
#include <printf.h>
#include <console.h>
#include <x86.h>

#include "../kern/task.h"


// *** BLOCK PART ***

#define EXT2_DEV MKDEV(MEM_MAJOR, 0)
//#define EXT2_DEV MKDEV(HD_MAJOR, 0)

static void read_block(uint32_t sec, void *buf)
{
	struct buf *bp;

	bp = bread(EXT2_DEV, sec);
	memmove(buf, bp->data, 512);
	brelse(bp);
}

// read n blocks from hd. make sure buf is no smaller than SECT_SIZE*n.
static void read_nblocks(uint32_t sec, void *buf, int n)
{
	struct buf *bp;
	char *p = buf;

	while(n-- > 0){
		bp = bread(EXT2_DEV, sec);
		memmove(p, bp->data, 512);
		brelse(bp);
		p += SECT_SIZE;
	}
}

static void write_block(uint32_t sec, void *buf)
{
	return;
}

static void write_nblocks(uint32_t sec, void *buf, int n)
{
	return;
}


// *** INODE PART ***

#define NIBUF	32
static struct ext2_inode ibuf[NIBUF];  // static variable contains all zeros

static void ext2_iupdate(void)
{
}

static void ext2_ialloc(void)
{
}

// *** EXT2 PART ***

#define SB_BLOCK ((sizeof (struct ext2_super_block) -1) / SECT_SIZE + 1)
#define SB_BUF_SIZE (SB_BLOCK * SECT_SIZE)

struct fs_node *ext2_root;	// root directory node

static char buf[SECT_SIZE];
static char sbb[SB_BUF_SIZE];	// super_block buffer
static struct ext2_super_block *sbp = sbb;

void ext2_read_sb(void)
{
	read_nblocks(2, sbp, SB_BLOCK);
}

void ext2_write_sb(void)
{
}

void dump_ext2_superblock(struct ext2_super_block *sb_ptr)
{
	int block_size;

	settextcolor(12, 0);
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
	resettextcolor();
}

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
	dump_ext2_superblock(sb_ptr);

	block_size = 1024 << sb_ptr->log_block_size;
	printk("a block = %d bytes, ", block_size);

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


// obtain superblock info
static void ext2_obtain_sb_info(void)
{
	ext2_read_sb();
	dump_ext2_superblock(sbp);

#if 0
	nblk = sb->total_blk;
	reserved_blk = sb->resv_blk;

	settextcolor(11, 0);
	printk("data area blocks: %d\n", da_blocks);
	printk("index area items: %d\n", ia_num);
	printk("block_size: %d (2 = 512, 3 = 1024 bytes)\n", block_size);
	resettextcolor();
#endif
}


void ext2_cat_file(const char *file)
{
	settextcolor(15, 0);
	printk("From function ext2_cat_file(): file = %s\n", file);
	resettextcolor();
}

void _ext2_open(struct fs_node *node, int flag)
{
	// for test
	ext2_cat_file("etc/motd");
}

struct fs_node *init_ext2(void)
{
	// TODO: Below will panic, since we are not running for any task
	// ext2_root = (struct fs_node *)kmalloc(sizeof(struct fs_node));
	// so use kalloc_page() instead, which is wasteful.
	// Maybe we need a kmalloc_kernel() besides kmalloc()?
	ext2_root = (struct fs_node *)kalloc_page(sizeof(struct fs_node));

	strcpy(ext2_root->name, "ext2");
	ext2_root->mask = 0;
	ext2_root->uid = 0;
	ext2_root->gid = 0;
	ext2_root->flags = FS_DIR;
	ext2_root->inode = 0;
	ext2_root->length = 0;
	ext2_root->impl = 0;

	ext2_root->open = _ext2_open;
	ext2_root->close = 0;
	ext2_root->read = 0;
	ext2_root->write = 0;
	ext2_root->readdir = 0;
	ext2_root->finddir = 0;
	ext2_root->ptr = 0;

	// cache super block info
	ext2_obtain_sb_info();

	// just for test
	ext2_cat_file("README");

	return ext2_root;
}
