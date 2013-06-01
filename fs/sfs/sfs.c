/*
  fs.c: A trival implement for SFS (Simple File System).
  Refer to: http://dimensionalrift.homelinux.net/combuster/vdisk/sfs.html

limits:
	* files/dirs length should be less than 30+64*3 = 222 bytes
*/

#include <types.h>
#include <hd.h>
#include <sfs.h>
#include <const.h>
#include <fs.h>
#include <console.h>
#include <x86.h>
#include <printf.h>
#include "../kern/task.h"

struct fs_node sfs_fs = {
	NULL,
	"sfs",
	1,
	NULL,
};

char buf[SECT_SIZE];
char buf2[SECT_SIZE*2];

struct sfs_superblock sb;
int ibufs[4] = { IBUF1, IBUF2, IBUF3, IBUF4 };

// TODO: work on this
void sfs_hd_rw(int cmd, int offset, int nb, char *buf);
void sfs_hd_rw(int cmd, int offset, int nb, char *buf) {}

static struct inode1 *search_ibuf(void *bufp, const char *name, int scale)
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

static struct inode1 *search_index(const char *name, int type)
{
	int ic, j;
	int nb = (sb.ia_size - 1)/SECT_SIZE;
	struct sfs_index *ip;

	for(ic = 0; ic <= nb; ic++){
		sfs_hd_rw(HD_READ, sb.total_blk-1-ic, 1, buf);
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

static struct inode1 *search_inode(const char *name)
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

static struct inode1 *create_file(const char *f)
{
	int nie = NIE(strlen(f));
	int bn;		// number of block
	int bi;		// number in block
	struct sfs_file *sp;

	bn = sb.total_blk - 1 - (sb.ia_size - 1)/SECT_SIZE;
	bi = sb.ia_size % SECT_SIZE;
	bi /= IE_SIZE;

	printk("nie: %d, sb.total_blk: %d\n", nie, sb.total_blk);
	sfs_hd_rw(HD_READ, bn-1, 2, buf2);	// read 2 blocks (end-of-index + free space)

	sp = (struct sfs_file *)buf2;
	sp += 16 - bi - nie + 1;  // begin from space used to be 'marker' entry
	sp->etype = FILE_ENTRY;
	sp->ne = nie - 1;
	sp->time = 0;	// we don't set it yet
	sp->blk_start = sb.da_blk;
	sp->blk_end = sp->blk_start + NB_INIT;	// each file is 2K when created
	sp->len = 0;
	strcpy(sp->name, f);
	sp--;	// to setup 'marker' entry
	sp->etype = START_MARK;

	sfs_hd_rw(HD_WRITE, bn-1, 2, buf2);

	// save superblock changes to hd
	printk("sb.da_blk: %d\n", sb.da_blk);
	sb.da_blk += NB_INIT;
	printk("sb.da_blk: %d\n", sb.da_blk);
	sfs_hd_rw(HD_WRITE, 0, 1, (char *)&sb);

	return (struct inode1 *)(++sp);
};

int open(const char *pathname, int flags)
{
	int fd = -1;
	int i;
	struct inode1 *inp;

	printk("pathname: %s\n", pathname);

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
				printk("Create file fail!\n");
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

	printk("In read\n");
	return count;
}

int write(int fd, const void *buf, int n)
{
	int count;

	printk("In write\n");
	return count;
}

int close(int fd)
{
	int ret;

	printk("In close\n");
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

static void init_superblock(void)
{
	struct sfs_vol_id *idp;
	struct sfs_mark *markp;

	sfs_hd_rw(HD_READ, 0, 1, (char *)&sb);

	sb.time_stamp = 0;
	sb.da_blk = 0;			// by blocks
	sb.ia_size = IE_SIZE * 2;	// by 64 bytes each, for vol_id and mark
	sb.magic_num[0] = 'S';
	sb.magic_num[1] = 'F';
	sb.magic_num[2] = 'S';
	sb.fs_version = 0x10;	// sfs vresion 1.0

	// for both qemu and bochs, hd size ~= 80M with sb.blk_size=2
	sb.total_blk = 170*16*63;
	sb.rev_blk = 1;     // one for superblock, no one for future usage
	sb.blk_size = 2;    // 512 bytes per block

	// setup checksum
	sb.checksum = 0;
	sb.checksum = -calc_checksum((char *)&sb);
	printk("sb.checksum = %d\n", sb.checksum);

	sfs_hd_rw(HD_WRITE, 0, 1, (char *)&sb);

	memset(buf, 0, SECT_SIZE);
	// vol id stay at the botton of hd area
	idp = (struct sfs_vol_id *)buf;
	idp += 7;
	idp->etype = VOLUME_ID;
	idp->resv[0] = idp->resv[1] = idp->resv[2] = 0;
	idp->time = 0;
	strcpy(idp->name, "HD of SFS-v1.0");  // any info here should be OK

	// starting marker just above vol id since no files yet in the hd
	// since only etype need to be set, idp is OK for it
	idp -= 1;
	idp->etype = START_MARK;

	// write it to the last block of hd
	sfs_hd_rw(HD_WRITE, sb.total_blk-1, 1, buf);
}

void mkfs(void)
{
	init_superblock();
}

void check_fs(void)
{
	char sum;

	sfs_hd_rw(HD_READ, 0, 1, buf);

	// check checksum
	sum = calc_checksum(buf);
	if(sum){
		printk("ERROR! checksum = %d, not zero!\n", sum);
	}

	// TODO: check if 'versioin + magic_num' == '0x10534653'
}

void init_fs(void)
{
	check_fs();
}
