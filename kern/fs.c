/*
  fs.c: A trival implement for SFS (Simple File System).
  Refer to: http://dimensionalrift.homelinux.net/combuster/vdisk/sfs.html
*/

#include <types.h>
#include "hd.h"
#include "fs.h"
#include "const.h"
#include "task.h"

char buf[SECT_SIZE];
struct superblock sb;
int ibufs[4] = { IBUF1, IBUF2, IBUF3, IBUF4 };

void hd_rw(int cmd, int nb, int offset, char *buf);

static struct inode1 *search_ibuf(void *bufp, char *name, int scale)
{
	int i;
	struct inode1 *p = (struct inode1 *)bufp;

	for(i = 0; i<ibufs[scale-1]; i++){
		if(p->nb == 0)	// slot not in use
			continue;

		if(! strcmp(p->sfile.name, name) )	// found it
			return p;

		p += scale;
	}
	return NULL;
}

static struct inode1 *search_inode(char *name)
{
	int len = strlen(name);
	struct inode1 *ret = NULL;

	if(len <= 30){
		ret = search_ibuf(ibuf1, name, 1);
	} else if(len <= 30 + IE_SIZE){
		ret = search_ibuf(ibuf2, name, 2);
	} else if(len <= 30 + IE_SIZE*2){
		ret = search_ibuf(ibuf3, name, 3);
	} else if(len <= 30 + IE_SIZE*3){
		ret = search_ibuf(ibuf4, name, 4);
	}

	return ret;
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
	for(i = 0; i < FDT_SIZE; i++){
		if(fdtable[i].fd_inode == 0)
			break;
	}
	if(i >= FDT_SIZE)
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
	struct sfs_vol_id *idp;
	struct sfs_mark *markp;

	hd_rw(HD_READ, 1, 0, (char *)&sb);

	sb.time_stamp = 0;
	sb.da_blk = 0;			// by blocks
	sb.ia_size = IE_SIZE * 2;			// by 64 bytes each
	sb.magic_num[0] = 'S';
	sb.magic_num[1] = 'F';
	sb.magic_num[2] = 'S';
	sb.fs_version = 0x10;	// sfs vresion 1.0

	sb.total_blk = 80*2*1024; // we assume hd is 80M here with sb.blk_size=2
	sb.rev_blk = 2;     // one for superblock, one for future usage
	sb.blk_size = 2;    // 512 bytes per block

	// setup checksum
	sb.checksum = 0;
	sb.checksum = -calc_checksum((char *)&sb);
	printf("sb.checksum = %d\n", sb.checksum);

	hd_rw(HD_WRITE, 1, 0, (char *)&sb);

	// TODO
	// memset(buf, 0, SECT_SIZE);

	// vol id stay at the botton of hd area
	idp = (struct sfs_vol_id *)buf;
	idp += 7;
	idp->etype = VOLUME_ID;
	idp->resv = 0;
	idp->time = 0;
	strcpy(idp->name, "HD of SFS-v1.0");  // any info here should be OK

	// starting marker just above vol id since no files yet in the hd
	// since only etype need to be set, idp is OK for it
	idp -= 1;
	idp->etype = START_MARK;

	// write it to the last block of hd
	hd_rw(HD_WRITE, 1, sb.total_blk-1, buf);
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
