/*
  fs.c: A trival implement for SFS (Simple File System).
  Refer to: http://dimensionalrift.homelinux.net/combuster/vdisk/sfs.html
*/

#include <types.h>
#include "hd.h"
#include "fs.h"
#include "const.h"
#include "task.h"

#define IE_SIZE 64	// Index entries size is 64 bytes each

char buf[SECT_SIZE];
struct superblock sb;

void hd_rw(int cmd, int nb, int offset, char *buf);

static struct inode *search_inode()
{
	return NULL;
}

static struct inode *create_file()
{
	return NULL;
}

int open(const char *pathname, int flags)
{
	int fd = -1;
	int i;
	struct inode *inp;

	printf("In open\n");
	printf("pathname: %s\n", pathname);

	// search free slot in fdp[]
	for(i = 0; i < OFILE; i++){
		if(current->fdp[i] == 0){
			fd = i;
			break;
		}
	}
	if(fd < 0 || fd >= OFILE)
		panic("open: fdp[] is full!");

	// search free slot in fdtable[]
	for(i = 0; i < NR_FDT; i++){
		if(fdtable[i].fd_inode == 0)
			break;
	}
	if(i >= NR_FDT)
		panic("open: fdtable[] is full!");

	inp = search_inode(pathname);
	if(! inp){		// file doesn't exist
		if(flags & O_CREAT){
			inp = create_file(pathname);
			if(!inp)
				printf("Create file fail!\n");
		} else {
			return -1;
		}
	}
	current->fdp[fd] = &fdtable[i];
	fdtable[i].fd_inode = inp;
	fdtable[i].fd_mode = flags;
	fdtable[i].fd_off = 0;

	return fd;
}

int read(int fd, void *buf, int n)
{
	int count;

	printf("In read\n");
	return count;
}

int write(int fd, const void *buf, int n)
{
	int count;

	printf("In write\n");
	return count;
}

int close(int fd)
{
	int ret;

	printf("In close\n");
	return 0;
}

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
	sb.da_blk = 0;			// by blocks
	sb.ia_size = 0;			// by 64 bytes each
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
