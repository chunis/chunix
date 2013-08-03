/*
  A trival implement for SFS (Simple File System).
  Refer to: http://dimensionalrift.homelinux.net/combuster/vdisk/sfs.html
        or: http://www.d-rift.nl/combuster/vdisk/sfs.html
*/

#include <types.h>
#include <hd.h>
#include <const.h>
#include <fs.h>
#include <fs_sfs.h>
#include <console.h>
#include <x86.h>
#include <printf.h>
#include "../kern/task.h"


#define SFS_DEV MKDEV(MEM_MAJOR, 0)
//#define SFS_DEV MKDEV(HD_MAJOR, 0)

char buf[SECT_SIZE];
struct sfs_superblock sb;
uint32_t nblk;  // total number of block
uint32_t reserved_blk;  // superblock + reserved block
uint32_t da_blocks;  // number of data area block
uint32_t ia_num;  // number of index area
uint32_t block_size;  // 2 means 512


struct fs_node sfs_fs = {
	NULL,
	"sfs",
	1,
	NULL,
};

void add_sfs_fs(void)
{
	register_filesystem(&sfs_fs);
}

void del_sfs_fs(void)
{
	unregister_filesystem(&sfs_fs);
}

void sfs_init(void) {}

void sfs_read_sb(void){}
void sfs_write_sb(void){}


void sfs_namei(const char *name) {}

static sfs_bread(char *blk, int blk_no)
{
	struct buf *bp;

	bp = bread(SFS_DEV, blk_no);
	memmove(blk, bp->data, 512);
	brelse(bp);
}

/*
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
*/

/*
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
*/

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

	for(cnt = 0x1ac; cnt <= 0x1bd; cnt++)
		sum += sb[cnt];

	return sum;
}

// obtain superblock info
static void obtain_sb_info(struct sfs_superblock *sb)
{
	uint64_t time = sb->time_stamp;

	nblk = sb->total_blk;
	reserved_blk = sb->rev_blk;
	da_blocks = sb->da_blk;
	ia_num = sb->ia_size / IE_SIZE;
	block_size = sb->blk_size;

	printk("in obtain_sb_info: da_blocks = %d\n", da_blocks);
}

static void check_sfs(void)
{
	struct sfs_superblock *sb;
	char sum;

	sfs_bread(buf, 0);

	// check checksum
	sum = calc_checksum(buf);
	if(sum){
		printk("ERROR! checksum = %d, not zero!\n", sum);
	}

	// TODO: check if 'versioin + magic_num' == '0x10534653'
	sb = (struct sfs_superblock *)buf;
	printk("magic_num: %x%x%x\n", sb->magic_num);
	if(strncpy(sb->magic_num, "SFS", 3) || sb->fs_version != 0x10){
		printk("!!!!! Error !!!!! Not a sfs file system\n");
		return;
	}

	obtain_sb_info(sb);
}

void init_sfs(void)
{
	check_sfs();
}
