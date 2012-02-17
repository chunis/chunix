/*
  fs.c: A trival implement for SFS (Simple File System).
  Refer to: http://dimensionalrift.homelinux.net/combuster/vdisk/sfs.html
*/

#ifndef __FS_H__
#define __FS_H__

#include <types.h>

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
	uint8_t rev_boot1[11];
	uint8_t rev_bios[21];
	uint8_t rev_boot2[372];
	uint8_t time_stamp[8];
	uint8_t da_blk[8];
	uint8_t ia_size[8];
	uint8_t magic_num[3];
	uint8_t fs_version;
	uint8_t total_blk[8];
	uint8_t rev_blk[4];
	uint8_t blk_size;
	uint8_t checksum;
	uint8_t rev_pt[64];  // partition table
	uint8_t rev_boot_sig[2];  // boot signature
};

#endif
