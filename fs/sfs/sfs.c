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
#define BLOCK_INDEX(da_num) ((da_num) + reserved_blk)

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

static void read_block(uint32_t sec, void *buf)
{
	struct buf *bp;

	bp = bread(SFS_DEV, sec);
	memmove(buf, bp->data, 512);
	brelse(bp);
}

static void read_data_block(uint32_t n, void *buf)
{
	read_block(BLOCK_INDEX(n), buf);
}

static void write_block(uint32_t sec, void *buf)
{
	return;
}

// read the content of sec-th item from index area into buf.
// buf size should be no less than IE_SIZE = 64 bytes.
static void read_index(uint32_t sec, void *buf)
{
	struct buf *bp;
	uint32_t n = nblk - sec/IPB - 1;
	uint32_t offset = SECT_SIZE - (sec % IPB + 1) * IE_SIZE;

	//printk("sec = %d, n = %d, offset = %d\n", sec, n, offset);
	bp = bread(SFS_DEV, n);
	memmove(buf, bp->data + offset, IE_SIZE);
	brelse(bp);
}

static void write_index(uint32_t sec, void *buf)
{
	return;
}

void sfs_namei(const char *name) {}

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

	settextcolor(11, 0);
	printk("basic info in SFS file system:\n");
	printk("total blocks: %d\n", nblk);
	printk("sb + rsvd blocks: %d\n", reserved_blk);
	printk("data area blocks: %d\n", da_blocks);
	printk("index area items: %d\n", ia_num);
	printk("block_size: %d (2 = 512, 3 = 1024 bytes)\n", block_size);
	resettextcolor();
}

static void check_sfs(void)
{
	struct sfs_superblock *sb;
	char sum;

	read_block(0, buf);

	// check checksum
	sum = calc_checksum(buf);
	if(sum){
		printk("ERROR! checksum = %d, not zero!\n", sum);
	}

	sb = (struct sfs_superblock *)buf;
	if(strncmp(sb->magic_num, "SFS", 3) || sb->fs_version != 0x10){
		printk("!!!!! Error !!!!! Not a sfs file system\n");
		return;
	}

	obtain_sb_info(sb);
}

// if find, return 1; else return 0
static int sfs_find_index(const char *file, struct sfs_index *idxp)
{
	int nindex = ia_num - 1;  // skip the marker entry
	struct sfs_dir *dirp;
	struct sfs_file *filep;
	uint32_t len = strlen(file);
	char *str, *p;
	int ne, ret = 0;

	printk("cat file: %s\n", file);
	while(nindex > 0){
		read_index(nindex, (char *)idxp);
		if(idxp->etype == DIR_ENT){
			dirp = (struct sfs_dir *)idxp;
			nindex -= dirp->ne + 1;
			if(dirp->ne == 0 && len <= DIR_SPACE){
				//printk("dir: %s\n", dirp->name);
				if(strncmp(file, dirp->name, len) == 0){
					printk("found dir: %s\n", dirp->name);
					ret = 1;
				}
			} else {
				str = kmalloc((dirp->ne + 1) * IE_SIZE);
				if(!str)
					panic("kmalloc");
				strncpy(str, dirp->name, DIR_SPACE);
				p = str + DIR_SPACE;
				ne = dirp->ne;
				while(ne > 0){
					read_index(nindex + ne, p);
					p += IE_SIZE;
					ne--;
				}
				printk("long dir: %s\n", str);
				if(strncmp(file, dirp->name, len) == 0){
					printk("found dir: %s\n", dirp->name);
					ret = 1;
				}
				kfree(str);
			}
		} else if(idxp->etype == FILE_ENT){
			filep = (struct sfs_file *)idxp;
			nindex -= filep->ne + 1;
			if(filep->ne == 0 && len <= FILE_SPACE){
				//printk("file: %s\n", filep->name);
				if(strncmp(file, filep->name, len) == 0){
					printk("found file: %s\n", filep->name);
					ret = 1;
				}
			} else {
				str = kmalloc((filep->ne + 1) * IE_SIZE);
				if(!str)
					panic("kmalloc");
				strncpy(str, filep->name, FILE_SPACE);
				p = str + FILE_SPACE;
				ne = filep->ne;
				while(ne > 0){
					read_index(nindex + ne, p);
					p += IE_SIZE;
					ne--;
				}
				printk("long file: %s\n", str);
				if(strncmp(file, filep->name, len) == 0){
					printk("found file: %s\n", filep->name);
					ret = 1;
				}
				kfree(str);
			}
		} else {
			printk("etype: %x\n", idxp->etype);
			break;
		}
		if(ret)
			return ret;
	}
	return ret;
}

void sfs_cat_file(const char *file)
{
	struct sfs_index idx;
	struct sfs_file *filep;
	char buf[SECT_SIZE];
	uint64_t m, n;
	uint32_t sz, i;
	int len;

	if(! sfs_find_index(file, &idx))
		return;

	filep = (struct sfs_file *)&idx;
	if(filep->etype != FILE_ENT)
		return;

	m = filep->blk_start;
	n = filep->blk_end;
	len = filep->len;
	while(len > 0){
		sz = len > SECT_SIZE ? SECT_SIZE : len;
		//printk("m = %d, n = %d, len = %d, sz = %d\n", m, n, len, sz);
		read_data_block(m++, buf);
		for(i = 0; i < sz; i++)
			printk("%c", buf[i]);
		len -= SECT_SIZE;
	}
}

void init_sfs(void)
{
	check_sfs();
	sfs_cat_file("README");
}
