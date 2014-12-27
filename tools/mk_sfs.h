//
// This file is part of ChuniX and is released under the terms of
// GNU GPLv3 License, see License for details.
//
// Copyright (C) 2014, Chunis Deng (<chunchengfh@gmail.com>)
//
/*
  A trival implement for SFS (Simple File System).
  Refer to: http://dimensionalrift.homelinux.net/combuster/vdisk/sfs.html
        or: http://www.d-rift.nl/combuster/vdisk/sfs.html
*/

#ifndef __MK_SFS_H__
#define __MK_SFS_H__


#define DIR_SPACE 54
#define FILE_SPACE 30

// Number of Index entries needed for a name str whose length is 'len'
#define NIE_FILE(len) (((len)+34+1)/IE_SIZE) // first 1 byte for '\0'
#define NIE_DIR(len) (((len)+10+1)/IE_SIZE) // first 1 byte for '\0'

#define IE_SIZE 64	// Index entries size is 64 bytes each

//typedef signed char int8_t;
typedef unsigned char uint8_t;
//typedef short int16_t;
//typedef unsigned short uint16_t;
//typedef int int32_t;
typedef unsigned int uint32_t;
//typedef long long int64_t;
typedef unsigned long long uint64_t;

// Layout of areas
#define SUPER_BLK   0x1
#define RESV_AREA   0x2
#define DATA_AREA   0x3
#define FREE_AREA   0x4
#define INDEX_AREA  0x5

// Index area entries type
#define VOLUME_ID     0x01
#define START_MARK    0x02
#define UNUSED_ENT    0x10
#define DIR_ENT       0x11
#define FILE_ENT      0x12
#define UNUSABLE_ENT  0x18
#define DEL_DIR       0x19
#define DEL_FILE      0x1A
#define CONT_ENT      0x20	// 0x20 ~ 0xFF


struct sfs_superblock {
	uint8_t  rev_boot1[11];
	uint8_t  rev_bios[21];
	uint8_t  rev_boot2[372];
	uint64_t time_stamp;
	uint64_t da_blk;
	uint64_t ia_size;
	uint8_t  magic_num[3];
	uint8_t  fs_version;
	uint64_t total_blk;
	uint32_t rev_blk;
	uint8_t  blk_size;
	uint8_t  checksum;
	uint8_t  rev_pt[64];  // partition table
	uint8_t  rev_boot_sig[2];  // boot signature
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

struct sfs_index {
	uint8_t etype;	// entry type
	uint8_t data[63];
};

// inode cache in memory for index
struct sfs_inode {
	int nb;		// block number in hd
	uint8_t ni;	// index number in a single block
	int8_t nref;	// reference count
	int8_t rw;	// read/write flag
	int8_t dirty;	// is this inode changed?

	struct sfs_index *sindex;  // index block (64bytes)
}__attribute__((packed));

#endif
