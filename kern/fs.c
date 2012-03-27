/*
  fs.c: A trival implement for SFS (Simple File System).
  Refer to: http://dimensionalrift.homelinux.net/combuster/vdisk/sfs.html

limits:
	* files/dirs length should be less than 30+64*3 = 222 bytes
*/

#include <types.h>
#include "hd.h"
#include "fs.h"
#include "const.h"
#include "task.h"

char buf[SECT_SIZE];
char buf2[SECT_SIZE*2];

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

static struct inode1 *search_index(char *name, int type)
{
	int ic, j;
	int nb = (sb.ia_size - 1)/SECT_SIZE;
	struct sfs_index *ip;

	for(ic = 0; ic <= nb; ic++){
		hd_rw(HD_READ, 1, sb.total_blk-1-ic, buf);
		ip = (struct sfs_index *)buf;
		ip += 7;
		for(j = 0; j<8; j++){
			if(ip->etype == START_MARK)
				break;
			if(ip->etype == type)
				return ip;
			ip--;
		}
	}

	return NULL;
}

static struct inode1 *search_inode(char *name)
{
	int nie = NIE(strlen(name));
	struct inode1 *ret = NULL;

	if(nie == 1){
		ret = search_ibuf(ibuf1, name, 1);
	} else if(nie == 2){
		ret = search_ibuf(ibuf2, name, 2);
	} else if(nie == 3){
		ret = search_ibuf(ibuf3, name, 3);
	} else if(nie == 4){
		ret = search_ibuf(ibuf4, name, 4);
	}

	if(ret == NULL)
		ret = search_index(name, T_FILE);

	return ret;
}

static struct inode1 *create_file(char *f)
{
	int nie = NIE(strlen(f));
	int bn;		// number of block
	int bi;		// number in block
	struct sfs_file *sp, *sq;

	bn = sb.total_blk - 1 - (sb.ia_size - 1)/SECT_SIZE;
	bi = sb.ia_size % SECT_SIZE;
	bi /= IE_SIZE;

	return NULL;
};

int open(const char *pathname, int flags)
{
	int fd = -1;
	int i;
	struct inode1 *inp;

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
			if(!inp){
				printf("Create file fail!\n");
				return -1;
			}
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
	sb.ia_size = IE_SIZE * 2;	// by 64 bytes each, for vol_id and mark
	sb.magic_num[0] = 'S';
	sb.magic_num[1] = 'F';
	sb.magic_num[2] = 'S';
	sb.fs_version = 0x10;	// sfs vresion 1.0

	sb.total_blk = 80*2*1024; // we assume hd is 80M here with sb.blk_size=2
	sb.rev_blk = 1;     // one for superblock, no one for future usage
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
