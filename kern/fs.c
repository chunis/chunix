/*
  fs.c: A trival implement for SFS (Simple File System).
  Refer to: http://dimensionalrift.homelinux.net/combuster/vdisk/sfs.html
*/

#include <types.h>
#include "hd.h"
#include "fs.h"

char buf[SECT_SIZE];
struct superblock sb;

void hd_rw(int cmd, int nb, int offset, char *buf);

static char calc_checksum(char *sb)
{
	char sum = 0;
	int cnt;

	for(cnt=0x1ac; cnt<=0x1bd; cnt++)
		sum += sb[cnt];

	return sum;
}

void init_superblock(void)
{
	hd_rw(HD_READ, 1, 0, (char *)&sb);

	sb.time_stamp = 0;
	sb.da_blk = 0;
	sb.ia_size = 0;
	sb.magic_num[0] = 'S';
	sb.magic_num[1] = 'F';
	sb.magic_num[2] = 'S';
	sb.fs_version = 0x10;	// sfs vresion 1.0

	sb.total_blk = 60*2*1024; // we assume hd is 60M here with sb.blk_size=2
	sb.rev_blk = 2;     // one for superblock, one for future usage
	sb.blk_size = 2;    // 512 bytes per block

	// setup checksum
	sb.checksum = 0;
	sb.checksum = -calc_checksum((char *)&sb);
	printf("sb.checksum = %d\n", sb.checksum);

	hd_rw(HD_WRITE, 1, 0, (char *)&sb);
}

void mkfs(void)
{
	init_superblock();
}

void check_fs(void)
{
	char sum;

	hd_rw(HD_READ, 1, 0, buf);

	// check checksum
	sum = calc_checksum(buf);
	if(sum){
		printf("ERROR! checksum = %d, not zero!\n", sum);
	}

	// TODO: check if 'versioin + magic_num' == '0x10534653'
}

void init_fs(void)
{
	check_fs();
}
