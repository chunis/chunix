//
// This file is part of ChuniX and is released under the terms of
// GNU GPLv3 License, see License for details.
//
// Copyright (C) 2014, Chunis Deng (<chunchengfh@gmail.com>)
//
/*
  A trival implement for SFS (Simple File System).
  Refer to: http://www.d-rift.nl/combuster/vdisk/sfs.html
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

struct fs_node *sfs_root;	// root directory node

#define SFS_DEV MKDEV(MEM_MAJOR, 0)
//#define SFS_DEV MKDEV(HD_MAJOR, 0)
#define BLOCK_INDEX(da_num) ((da_num) + reserved_blk)

#define NIBUF	32
static struct sfs_inode ibuf[NIBUF];  // static variable contains all zeros

static char buf[SECT_SIZE];
static struct sfs_superblock sb;
static uint32_t nblk;  // total number of block
static uint32_t reserved_blk;  // superblock + reserved block
static uint32_t da_blocks;  // number of data area block
static uint32_t ia_num;  // number of index area
static uint32_t block_size;  // 2 means 512

static int sfs_find_index(const char *file, struct sfs_index *idxp);


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

static struct sfs_inode *search_index_buf(const char *name)
{
	int i;

	for(i = 0; i < NIBUF; i++){
		if(ibuf[i].flags & SFS_INODE_INUSE){
			if(strcmp(name, ibuf[i].name) == 0){
				return &ibuf[i];
			}
		}
	}
	return NULL;
}

// return the index of free position in ibuf or -1 if no slot available
static int ibuf_alloc(void)
{
	int i;

	for(i = 0; i < NIBUF; i++){
		if(ibuf[i].flags == 0)
			break;
	}
	return (i >= NIBUF ? -1 : i);
}

// find name in index area; and save the info into ibuf[]
static struct sfs_inode *search_index(const char *name, int type)
{
	struct sfs_index idx;
	struct sfs_inode *ip;
	int i;

	if(sfs_find_index(name, &idx)){ // find it in index area
		i = ibuf_alloc();
		if(i == -1){
			printk("ERROR!! search_index: no slot in ibuf\n");
			return NULL;
		}
		ip = &ibuf[i];

		if(ip->name)
			kfree(ip->name);
		ip->name = kmalloc(strlen(name)+1);
		if(ip->name == NULL){
			printk("kmalloc() fail\n");
			return NULL;
		}
		strcpy(ip->name, name);
		ip->flags = SFS_INODE_INUSE;
		ip->sindex = idx;
		return ip;
	}
	return NULL;
}

static void sfs_iupdate(void)
{
}

static void sfs_ialloc(void)
{
}

static struct sfs_inode *sfs_namei(const char *name)
{
	struct sfs_inode *ret = NULL;

	// Something wrong goes here
	//ret = search_index_buf(name);
	//if(ret == NULL)
		ret = search_index(name, SFS_TYPE_FILE);

	return ret;
}

static struct sfs_file_desc *fd_alloc(void)
{
	struct sfs_file_desc *fp;

	for(fp = fdtable; fp < fdtable + FDT_SIZE; fp++){
		if(fp->f_count == 0){  // found a free slot
			fp->f_count++;
			return fp;
		}
	}
	return NULL;
}

static struct sfs_inode *sfs_create(const char *path, int type)
{
	int len = strlen(path);
	struct sfs_file *sp;
	int bn;		// number of block
	int bi;		// number in block
	int nie;

#if 0
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


	// save superblock changes to hd
	sb.da_blk += NB_INIT;
	printk("sb.da_blk: %d\n", sb.da_blk);
	sfs_hd_rw(HD_WRITE, 0, 1, (char *)&sb);

	return (struct sfs_inode *)(++sp);
#endif
	return NULL;
}

int sfs_open(const char *pathname, int flags)
{
	int fd = -1;
	int i;
	struct sfs_inode *inp;
	struct sfs_file_desc *fdp;

	printk("in sfs_open: pathname=%s, flags = %d\n", pathname, flags);

	inp = sfs_namei(pathname);
	if(! inp){		// file doesn't exist
		if(flags & O_CREAT){
			inp = sfs_create(pathname, flags);
			if(!inp){
				printk("Create file fail!\n");
				return -1;
			}
		} else {
			return -1;
		}
	}

	// search free slot in ofile[]
	for(i = 0; i < NOFILE; i++){
		if(current->ofile[i] == 0){
			fd = i;
			break;
		}
	}
	if(fd < 0 || fd >= NOFILE){
		printk("open: ofile[] is full!");
		return -1;
	}

	// search free slot in fdtable[]
	fdp = fd_alloc();
	if(fdp == NULL){
		printk("open: fdtable[] is full!");
		return -1;
	}

	fdp->f_type = 0;
	fdp->f_mode = 0;
	fdp->f_flags = flags;
	fdp->f_pos = 0;
	fdp->f_inode = inp;
	current->ofile[fd] = fdp;

	printk("fd = %d\n", fd);
	return fd;
}

#define min(a, b)  ((a) < (b) ? (a) : (b))
int sfs_read(int fd, void *buf, int n)
{
	struct sfs_file_desc *fdp;
	struct sfs_file *fp;
	char xbuf[SECT_SIZE];
	int count, offset, nb, len;

	if(fd >= NOFILE){
		printk("ERROR! fd = %d doesn't existed\n");
		return -1;
	}

	fdp = current->ofile[fd];
	fp = (struct sfs_file *)&(fdp->f_inode->sindex);
	n = min(n, (fp->len - fdp->f_pos));
	nb = fp->blk_start + fdp->f_pos / SECT_SIZE;

	// copy from the first block start from f_pos
	offset = fdp->f_pos % SECT_SIZE;
	read_data_block(nb, xbuf);
	len = min(n, SECT_SIZE - offset);
	memmove(buf, xbuf + offset, len);
	count = len;
	fdp->f_pos += len;
	buf += len;
	n -= len;
	nb++;

	while(n > 0){
		read_data_block(nb, xbuf);
		len = min(SECT_SIZE, n);
		memmove(buf, xbuf, len);
		fdp->f_pos += len;
		count += len;
		buf += len;
		n -= len;
		nb++;
	}

	return count;
}

int sfs_write(int fd, const void *buf, int n)
{
	int count;

	printk("In write\n");
	return count;
}

int sfs_close(int fd)
{
	int ret;

	printk("In close\n");
	return 0;
}

// return 0 if succeed, -1 if fail
int sfs_stat(const char *path, struct sfs_stat *buf)
{
	struct sfs_inode *indp;
	struct sfs_index *idxp;
	//struct sfs_dir *dirp;
	struct sfs_file *filep;

	memset(buf, 0, sizeof(struct sfs_stat));

	indp = sfs_namei(path);
	if(indp == NULL){
		printk("sfs_stat: sfs_namei() fail\n");
		return -1;
	}

	idxp = &indp->sindex;
	if(idxp->etype == IAT_DIR_ENT){
		buf->st_mode = SFS_TYPE_DIR;
		//dirp = (struct sfs_dir *)idxp;
		buf->st_size = 0;
	} else if (idxp->etype == IAT_FILE_ENT){
		buf->st_mode = SFS_TYPE_FILE;
		filep = (struct sfs_file *)idxp;
		buf->st_size = filep->len;
	}

	buf->st_dev = SFS_DEV;
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
	reserved_blk = sb->resv_blk;
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

	while(nindex > 0){
		read_index(nindex, (char *)idxp);
		if(idxp->etype == IAT_DIR_ENT){
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
		} else if(idxp->etype == IAT_FILE_ENT){
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

// read at most n bytes content of file to buf.
// return the number of bytes sucessfully read.
// make sure buf has space no less than nb bytes.
// return -1 if something wrong.
int sfs_read_file(const char *file, char *buf, int nb)
{
	struct sfs_index idx;
	struct sfs_file *filep;
	char buffer[SECT_SIZE];
	uint64_t m, n;
	uint32_t sz;
	int len, ret = 0;

	if(! sfs_find_index(file, &idx))
		return -1;

	filep = (struct sfs_file *)&idx;
	if(filep->etype != IAT_FILE_ENT)
		return -1;

	memset(buf, 0, nb);
	m = filep->blk_start;
	n = filep->blk_end;
	len = filep->len;
	len = (len < nb ? len : nb);
	while(len > 0){
		sz = len > SECT_SIZE ? SECT_SIZE : len;
		read_data_block(m++, buffer);
		memmove(buf, buffer, sz);
		len -= sz;
		buf += sz;
		ret += sz;
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
	if(filep->etype != IAT_FILE_ENT)
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

void _sfs_open(struct fs_node *node, int flag)
{
	// for test
	sfs_cat_file("etc/motd");
}

struct fs_node *init_initrd_sfs(void)
{
	// TODO: Below will panic, since we are not running for any task
	// sfs_root = (struct fs_node *)kmalloc(sizeof(struct fs_node));
	// so use kalloc_page() instead, which is wasteful.
	// Maybe we need a kmalloc_kernel() besides kmalloc()?
	sfs_root = (struct fs_node *)kalloc_page(sizeof(struct fs_node));

	strcpy(sfs_root->name, "sfs");
	sfs_root->mask = 0;
	sfs_root->uid = 0;
	sfs_root->gid = 0;
	sfs_root->flags = FS_DIR;
	sfs_root->inode = 0;
	sfs_root->length = 0;
	sfs_root->impl = 0;

	sfs_root->open = _sfs_open;
	sfs_root->close = 0;
	sfs_root->read = 0;
	sfs_root->write = 0;
	sfs_root->readdir = 0;
	sfs_root->finddir = 0;
	sfs_root->ptr = 0;

	return sfs_root;
}
