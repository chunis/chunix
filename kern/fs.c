/*
  fs.c: A trival implement for SFS (Simple File System).
  Refer to: http://dimensionalrift.homelinux.net/combuster/vdisk/sfs.html
*/

#include "fs.h"

struct superblock sb;

void init_superblock(void)
{
	sb.rev_boot1[11] = 0;
	sb.rev_bios[21] = 0;
	sb.rev_boot2[372] = 0;
	sb.time_stamp[8] = 0;
	sb.da_blk[8] = 0;
	sb.ia_size[8] = 0;
	sb.magic_num[3] = 0x534653;
	sb.fs_version = 0x10;	// sfs vresion 1.0
	sb.total_blk[8] = 0;
	sb.rev_blk[4] = 2;	// one for superblock, one for future usage
	sb.blk_size = 2;	// 512 bytes per block
	sb.checksum = 0;
	sb.rev_pt[64] = 0;  // partition table
	sb.rev_boot_sig[2] = 0;  // boot signature
}

void init_fs(void)
{
	init_superblock();
}
