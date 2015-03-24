//
// This file is part of ChuniX and is released under the terms of
// GNU GPLv3 License, see License for details.
//
// Copyright (C) 2014, Chunis Deng (<chunchengfh@gmail.com>)
//
/*
  A trival implement for SFS (Simple File System).
  Refer to: http://www.d-rift.nl/combuster/vdisk/sfs.html
*/

#ifndef __FS_SFS_H__
#define __FS_SFS_H__

#include <types.h>
#include <fs.h>

struct sfs_stat {
	dev_t st_dev;
	uint32_t st_mode;
	off_t st_size;
};

int sfs_open(const char *pathname, int flags);
int sfs_read(int fd, void *buf, int n);
int sfs_read_file(const char *file, char *buf, int n);
int sfs_stat(const char *path, struct sfs_stat *buf);
int sfs_write(int fd, const void *buf, int n);
int sfs_close(int fd);

void init_sfs(void);
struct fs_node *init_initrd_sfs(void);

#define DIR_SPACE 54
#define FILE_SPACE 30

#define IE_SIZE 64	// Index entries size is 64 bytes each
#define IPB 8  // Items per block in index area. 512/64 = 8

// Number of Index entries needed for a name str whose length is 'len'
#define NIE_FILE(len) (((len)+34+1)/IE_SIZE) // first 1 byte for '\0'
#define NIE_DIR(len) (((len)+10+1)/IE_SIZE) // first 1 byte for '\0'

// Layout of areas
#define SUPBLK_AREA 0x1
#define RESV_AREA   0x2
#define DATA_AREA   0x3
#define FREE_AREA   0x4
#define INDEX_AREA  0x5

// Index area entries type
#define IAT_VOLUME_ID     0x01
#define IAT_START_MARK    0x02
#define IAT_UNUSED_ENT    0x10
#define IAT_DIR_ENT       0x11
#define IAT_FILE_ENT      0x12
#define IAT_UNUSABLE_ENT  0x18
#define IAT_DEL_DIR       0x19
#define IAT_DEL_FILE      0x1A
#define IAT_CONT_ENT      0x20	// 0x20 ~ 0xFF


struct sfs_superblock {
	uint8_t  resv_boot1[11];
	uint8_t  resv_bios[21];
	uint8_t  resv_boot2[372];
	uint64_t time_stamp;
	uint64_t da_blk;	// Size of data area in blocks
	uint64_t ia_size;	// Size of index area in bytes
	uint8_t  magic_num[3];	// Magic number (0x534653)
	uint8_t  fs_version;	// SFS version (0x10 for Version 1.0)
	uint64_t total_blk;	// Total number of blocks
	uint32_t resv_blk;	// Number of reserved blocks
	uint8_t  blk_size;
	uint8_t  checksum;
	uint8_t  resv_pt[64];	// partition table
	uint8_t  resv_boot_sig[2];  // boot signature
}__attribute__((packed));

struct sfs_vol_id {
	uint8_t etype;
	uint8_t resv[3];
	uint64_t time;
	uint8_t name[52];
}__attribute__((packed));

struct sfs_mark {
	uint8_t etype;
	uint8_t resv[63];
};

struct sfs_unused {
	uint8_t etype;
	uint8_t resv[63];
};

struct sfs_dir {
	uint8_t etype;
	uint8_t ne;  // number of entry
	uint64_t time;
	uint8_t name[54];
}__attribute__((packed));

struct sfs_file {
	uint8_t etype;
	uint8_t ne;  // number of entry
	uint64_t time;
	uint64_t blk_start;
	uint64_t blk_end;
	uint64_t len;
	uint8_t name[30];
}__attribute__((packed));

struct sfs_unusable {
	uint8_t etype;
	uint8_t resv1[9];
	uint64_t blk_start;
	uint64_t blk_end;
	uint8_t resv2[38];
}__attribute__((packed));

// general index struct, can be converted to other index types
struct sfs_index {
	uint8_t etype;	// entry type
	uint8_t data[63];
};

#define SFS_INODE_FREE  0x000
#define SFS_INODE_INUSE 0x001
#define SFS_INODE_READ  0x002
#define SFS_INODE_WRITE 0x004
#define SFS_INODE_DIRTY 0x008

// inode cache in memory for index
struct sfs_inode {
	uint8_t ni;	// index number in a single block
	int8_t nref;	// reference count
	int8_t flags;
	uint8_t *name;
	struct sfs_index sindex;  // index block (64bytes)
}__attribute__((packed));

#define DIRENT_LEN(x) (4 + 4 + (x) + 1)
struct sfs_dirent {
	uint32_t len;	// dirent len, equal (4+4+strlen(name)+1)
	uint32_t ino;	// inode number
	char *name;
};

#define SFS_TYPE_FILE	1   // file
#define SFS_TYPE_DIR	2   // directory
#define SFS_TYPE_DEV	3   // special device

struct sfs_file_desc {
	int f_type;
	int f_mode;
	int f_flags;
	uint32_t f_count;
	uint32_t f_pos;
	struct sfs_inode *f_inode;
};

#define FDT_SIZE 64	// size of fdtable[]
struct sfs_file_desc fdtable[FDT_SIZE];

#endif
