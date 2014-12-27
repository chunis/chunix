//
// This file is part of ChuniX and is released under the terms of
// GNU GPLv3 License, see License for details.
//
// Copyright (C) 2014, Chunis Deng (<chunchengfh@gmail.com>)
//
#ifndef __EXT2_H__
#define __EXT2_H__


#include <types.h>

#define EXT2_SUPER_MAGIC 0xEF53
#define EXT2_SB_OFFSET	1024	// offset of superblock from partition start
#define EXT2_SB_SIZE	1024	// superblock size
#define EXT2_BLK_SIZE	1024	// block size

/* File Types */
#define EXT2_S_IFSOCK   0xC000
#define EXT2_S_IFLNK    0xA000
#define EXT2_S_IFREG    0x8000
#define EXT2_S_IFBLK    0x6000
#define EXT2_S_IFDIR    0x4000
#define EXT2_S_IFCHR    0x2000
#define EXT2_S_IFIFO    0x1000

/* setuid, etc. */
#define EXT2_S_ISUID    0x0800
#define EXT2_S_ISGID    0x0400
#define EXT2_S_ISVTX    0x0200

/* rights */
#define EXT2_S_IRUSR    0x0100
#define EXT2_S_IWUSR    0x0080
#define EXT2_S_IXUSR    0x0040
#define EXT2_S_IRGRP    0x0020
#define EXT2_S_IWGRP    0x0010
#define EXT2_S_IXGRP    0x0008
#define EXT2_S_IROTH    0x0004
#define EXT2_S_IWOTH    0x0002
#define EXT2_S_IXOTH    0x0001


// the super block
struct ext2_super_block {
	uint32_t inodes_count;
	uint32_t blocks_count;
	uint32_t r_blocks_count;
	uint32_t free_blocks_count;
	uint32_t free_inodes_count;
	uint32_t first_data_block;
	uint32_t log_block_size;
	uint32_t log_frag_size;
	uint32_t blocks_per_group;
	uint32_t frags_per_group;
	uint32_t inodes_per_group;
	uint32_t mtime;
	uint32_t wtime;
	uint16_t mnt_count;
	uint16_t max_mnt_count;
	uint16_t magic;
	uint16_t state;
	uint16_t errors;
	uint16_t minor_rev_level;
	uint32_t lastcheck;
	uint32_t checkinterval;
	uint32_t creator_os;
	uint32_t rev_level;
	uint16_t def_resuid;
	uint16_t def_resgid;

	// Dynamic inode sizes
	uint32_t first_ino;
	uint16_t inode_size;
	uint16_t block_group_nr;
	uint32_t feature_compat;
	uint32_t feature_incompat;
	uint32_t feature_ro_compat;
	uint8_t  uuid[16];
	int8_t   volume_name[16];
	int8_t   last_mounted[64];
	uint32_t algorithm_usage_bitmap;

	// Performance hints
	uint8_t  prealloc_blocks;
	uint8_t  prealloc_dir_blocks;
	uint16_t padding1;

	// Journaling support
	uint8_t  journal_uuid[16];
	uint32_t journal_inum;
	uint32_t journal_dev;
	uint32_t last_orphan;
	uint32_t hash_seed[4];
	uint8_t  def_hash_version;
	uint8_t  reserved_char_pad;
	uint16_t reserved_word_pad;
	uint32_t default_mount_opts;
	uint32_t first_meta_bg;
	uint32_t reserved[190];
} __attribute__((packed));

// blocks group descriptor
struct ext2_bg_descriptor
{
	int32_t  block_bitmap;
	int32_t  inode_bitmap;
	int32_t  inode_table;
	int16_t  free_blocks_count;
	int16_t  free_inodes_count;
	int16_t  used_dirs_count;
	int16_t  pad;
	int32_t  reserved[3];
} __attribute__((packed));

struct ext2_inode {
	int16_t	mode;
	int16_t	uid;
	int32_t	size;
	int32_t	atime;
	int32_t	ctime;
	int32_t	mtime;
	int32_t	dtime;
	int16_t	gid;
	int16_t	links_count;
	int32_t	blocks;
	int32_t	flags;
	int32_t osd1;
	int32_t	block[15];
	int32_t	generation;
	int32_t	file_acl;
	int32_t	dir_acl;
	int32_t	faddr;
	int8_t  osd2[12];
} __attribute__((packed));

struct ext2_dir_entry {
	uint32_t inode;
	uint16_t rec_len;
	uint8_t  name_len;
	uint8_t  file_type;
	char     name[255];
} __attribute__((packed));


void dump_ext2_superblock(struct ext2_super_block *sb_ptr);
void dump_ext2(uint32_t start_addr);

#endif
