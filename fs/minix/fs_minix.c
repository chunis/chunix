//
// This file is part of ChuniX and is released under the terms of
// GNU GPLv3 License, see License for details.
//
// Copyright (C) 2014, Chunis Deng (<chunchengfh@gmail.com>)
//
// Minix fs V1.0 implement

#include <types.h>
#include <hd.h>
#include <const.h>
#include <console.h>
#include <x86.h>
#include <fs.h>
#include <printf.h>
#include <string.h>
#include <buf.h>
#include "../kern/task.h"
#include "fs_minix.h"


#define min(a, b)	((a) < (b) ? (a) : (b))

struct minix_file file[NFILE];	// global file table
struct minix_inode inode[NINODE];	// global inode table

uint8_t fbuf[BLOCK_SIZE];
struct superblock sb;

struct fs_node minix_fs = {
	NULL,
	"minix",
	1,
	NULL,
};

// read the super block
void readsb(uint32_t dev, struct d_superblock *sb)
{
	struct buf *bp;

	bp = bread(dev, 1*BPB);
	memmove(sb, bp->data, sizeof(struct d_superblock));
	brelse(bp);
}

/*
** Bitmap/blocks process
*/
uint32_t balloc(uint32_t dev)
{
	return 0;
}

void bfree(uint32_t dev, uint32_t bn)
{
	return;
}

/*
** Inode process
*/

struct minix_inode *ialloc(uint32_t dev, uint32_t type)
{
	int in;
	struct buf *bp;
	struct d_inode dp;
	struct superblock sb;

	return 0;
}

void ifree(uint32_t dev, uint32_t bn)
{
	return;
}

// find inode with number 'in' on 'dev', and return its memory copy.
struct minix_inode *iget(uint32_t dev, uint32_t in)
{
	struct minix_inode *ip, *ifree = 0;

	// check if the inode is already cached
	for(ip = inode; ip < inode + NINODE; ip++){
		if(ip->ref > 0 && ip->dev == dev && ip->inum == in){
			ip->ref++;
			return ip;
		}
		if(ifree == 0 && ip->ref == 0){
			ifree = ip;
		}
	}

	if(ifree == 0)  // no inode cache available
		panic("iget: no inodes caches");

	ip = ifree;
	ip->dev = dev;
	ip->inum = in;
	ip->ref = 1;
	ip->flag = 0;

	return ip;
}

struct minix_inode *idup(struct minix_inode *ip)
{
	ip->ref++;
	return ip;
}

// drop a reference to in-memory inode.
// recycled the inode cache entry if it's the last reference
// free inode and its content on disk if necessory
void iput(struct minix_inode *ip)
{
	// TODO: check if need to free inode and content by 'ifree()'
	ip->ref--;
}

// return the disk block address of the bn-th block in inode,
// allocates one if needed.
uint32_t bmap(struct minix_inode *ip, uint32_t bn)
{
	uint32_t addr;

	if(bn < NDIRECT){
		if((addr = ip->addr[bn]) == 0)
			ip->addr[bn] = addr = balloc(ip->dev);
		return addr;
	}

	// TODO: process indirect blocks
	panic("bmap: TODO");
}

// search path, skip leading '/', save path's first element to 'name',
// return the start of following element without any leading '/'.
static char* path_down(char *path, char *name)
{
	char *pp;
	int len;

	pp = path;
	while(*pp == '/')
		pp++;
	if(*pp == 0)
		return 0;

	path = pp;
	while(*pp != '/' && *pp != 0)
		pp++;
	len = pp - path;
	if(len >= DIR_LEN){
		memmove(name, path, DIR_LEN);
	} else {
		memmove(name, path, len);
		name[len] = 0;
	}

	while(*pp == '/')
		pp++;

	return pp;
}

// read data from inode
int readi(struct minix_inode *ip, char *dst, uint32_t off, uint32_t n)
{
	uint32_t s = 0, m;
	struct buf *bp;

	if(ip->type == T_DEV)
		panic("readi: TODO");

	if(n < 0 || off > ip->size){
		printk("readi: arguments wrong");
		return -1;
	}

	if(off + n > ip->size)
		n = ip->size - off;

	while(s < n){
		bp = bread(ip->dev, bmap(ip, off/SECT_SIZE));
		m = min(n-s, SECT_SIZE - off%SECT_SIZE);
		memmove(dst, bp->data + off%SECT_SIZE, m);
		brelse(bp);
		s += m;
		off += m;
		dst += m;
	}

	return n;
}

// write data to minix_inode
int writei(struct minix_inode *ip, char *src, uint32_t off, uint32_t n)
{
	uint32_t s = 0, m;
	struct buf *bp;

	if(ip->type == T_DEV)
		panic("writei: TODO");

	if(n < 0 || off > ip->size){
		printk("writei: arguments wrong");
		return -1;
	}

	while(s < n){
		bp = bread(ip->dev, bmap(ip, off/SECT_SIZE));
		m = min(n-s, SECT_SIZE - off%SECT_SIZE);
		memmove(bp->data + off%SECT_SIZE, src, m);
		bwrite(bp);
		brelse(bp);
		s += m;
		off += m;
		src += m;
	}

	return n;
}

// look for minix_dirent 'name' in dir whose minix_inode is 'dp'.
struct minix_inode *search_dir(struct minix_inode *dp, char *name)
{
	uint32_t len, in, desz;
	struct minix_dirent de;

	desz = sizeof(de);

	/*
	if(dp->type != T_DIR)
		panic("search_dir: not DIR");
	*/

	for(len = 0; len < dp->size; len += desz){
		if(readi(dp, (char *)&de, len, desz) != desz)
			panic("search_dir: readi fail");
		if(strncmp(name, de.name, DIR_LEN) == 0){  // found
			in = de.inum;
			return iget(dp->dev, in);
		}
	}

	return 0;
}

void dump_inode(struct minix_inode *ip)
{
	printk("ip->type(d): %d\n", ip->type);
	printk("ip->size(x): %x\n", ip->size);
	printk("ip->count(x): %x\n", ip->count);
}

// name to minix_inode translate
struct minix_inode* namei(char *path)
{
	char name[DIR_LEN];
	struct minix_inode *ip, *next;

	if(*path == '/')
		ip = iget(ROOTDEV, ROOTINO);
	else
		ip = idup(current->cwd);
	//dump_inode(ip);

	while((path = path_down(path, name)) != 0){
		printk("namei: path: %s, name: %s\n", path, name);
		/*
		if(ip->type != T_DIR){
			printk("ip isn't a dir\n");
			return 0;
		}
		*/

		if((next = search_dir(ip, name)) == 0){
			printk("file not found\n");
			return 0;
		}

		ip = next;
	}
	return ip;
}

//
// File process
//
int fileopen(const char *path, int flags)
{
	int fd = -1;
	int i;
	struct minix_inode *inp = NULL;

	void check_minixfs(void);
	check_minixfs();

	printk("In open: path: %s\n", path);

	// get minix_inode for path
	if((inp = namei(path)) == 0)
		return -1;

	// search free slot in ofile[]
	for(i = 0; i < NOFILE; i++){
		if(current->ofile[i] == 0){
			fd = i;
			break;
		}
	}
	if(fd < 0 || fd >= NOFILE)
		panic("open: ofile[] is full!");

	// search free slot in file[]
	for(i = 0; i < NFILE; i++){
		if(file[i].inode == 0)
			break;
	}
	if(i >= NFILE)
		panic("open: file[] is full!");

	current->ofile[fd] = &file[i];
	file[i].inode = inp;
	file[i].mode = flags;
	file[i].pos = 0;

	return fd;
}

int filecreat(const char *path, int flags)
{
	return 0;
}

int fileread(int fd, void *buf, int n)
{
	int count;

	printk("In read\n");
	return count;
}

int filewrite(int fd, const void *buf, int n)
{
	int count;

	printk("In write\n");
	return count;
}

int fileclose(int fd)
{
	int ret;

	printk("In close\n");
	return 0;
}

// we use a correct minixfs image for chunix currently,
// so we shouldn't need to make a minix fs now
void mk_minixfs(void)
{
	panic("In mk_minixfs");
}

void dump_superblock(struct superblock *sb)
{
	printk("sb.s_ninodes:\t%d\n", (uint16_t)sb->s_ninodes);
	printk("sb.s_nzones:\t%d\n", (uint16_t)sb->s_nzones);
	printk("sb.s_imap_blks:\t%x\n", (uint16_t)sb->s_imap_blks);
	printk("sb.s_zmap_blks:\t%x\n", (uint16_t)sb->s_zmap_blks);
	printk("sb.s_firstdatazone:\t%d\n", (uint16_t)sb->s_firstdatazone);
	printk("sb.s_log_zone_size:\t%d\n", (uint16_t)sb->s_log_zone_size);
	printk("sb.s_max_size:\t%x\n", sb->s_max_size);
	printk("sb.s_magic:\t%x\n", (uint16_t)sb->s_magic);
	printk("sb.s_dev:\t%d\n", (uint16_t)sb->s_dev);
	printk("sb.s_flag:\t%d\n", (uint16_t)sb->s_flag);
	printk("sb.s_time:\t%d\n", sb->s_time);
}

void check_minixfs(void)
{
	char sum;
	int i;
	struct buf *sbuf;

	printk("do some minixfs check...\n");
	sbuf = bread(ROOTDEV, 2);
	memmove((char *)&sb, sbuf->data, sizeof(sb));
	brelse(sbuf);

	// check minix magic number
	printk("magic: %x\n", sb.s_magic);
	if(sb.s_magic == MINIX_MAGIC || sb.s_magic == MINIX_MAGIC2){
		printk("The fs is a minix fs\n");
		dump_superblock(&sb);
	} else {  // make a clean minix fs
		mk_minixfs();
	}
}
