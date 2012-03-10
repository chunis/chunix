/*
  fs.c: A trival implement for SFS (Simple File System).
  Refer to: http://dimensionalrift.homelinux.net/combuster/vdisk/sfs.html
*/

#ifndef __FS_H__
#define __FS_H__

#include <types.h>

#define OFILE  10	// open file for each process

// Layout of areas
#define SUPER_BLOCK   0x1
#define RESERVED_AREA   0x2
#define DATA_AREA    0x3
#define FREE_AREA    0x4
#define INDEX_AREA   0x5

// Index area entries type
#define VOLUME_ID     0x01
#define START_MARK    0x02
#define UNUSED_ENTRY  0x10
#define DIR_ENTRY     0x11
#define FILE_ENTRY    0x12
#define UNUSAB_ENTRY  0x18
#define DEL_DIR       0x19
#define DEL_FILE      0x1A
#define CONT_ENTRY    0x20	// 0x20 ~ 0xFF

struct superblock {
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
};

struct inode {
	uint8_t etype;	// entry type
	uint8_t data[63];
};

struct file_desp {
	int fd_mode;
	int fd_off;
	struct inode *fd_inode;
};

struct sfs_vol_id {
	uint8_t etype;
	uint8_t resv;
	uint64_t time;
	uint8_t name[52];
};

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
};

struct sfs_file {
	uint8_t etype;
	uint8_t ne;  // number of entry
	uint64_t time;
	uint64_t blk_start;
	uint64_t blk_end;
	uint64_t len;
	uint8_t name[30];
};

struct sfs_unusable {
	uint8_t etype;
	uint8_t resv1[9];
	uint64_t blk_start;
	uint64_t blk_end;
	uint8_t resv2[38];
};

#endif
