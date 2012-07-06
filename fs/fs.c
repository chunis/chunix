// Minix fs V1.0 implement

#include <types.h>
#include <hd.h>
#include <const.h>
#include <console.h>
#include <x86.h>
#include "../kern/task.h"
#include "../kern/printf.h"
#include "../kern/string.h"
#include "fs.h"


struct file file[NFILE];	// global file table
struct inode inode[NINODE];	// global inode table

uint8_t fbuf[BLOCK_SIZE];
struct superblock sb;


//
// Inode process
//
struct inode *iget(uint32_t dev, uint32_t inum)
{
	struct inode *ip = NULL;

	return ip;
}

struct inode* namei(char *path)
{
	struct inode *ip;

	ip = iget(ROOTDEV, ROOTINO);
}

//
// File process
//
int fileopen(const char *pathname, int flags)
{
	int fd = -1;
	int i;
	struct inode *inp = NULL;

	printf("In open: pathname: %s\n", pathname);

	// search free slot in fdp[]
	for(i = 0; i < NOFILE; i++){
		if(current->fdp[i] == 0){
			fd = i;
			break;
		}
	}
	if(fd < 0 || fd >= NOFILE)
		panic("open: fdp[] is full!");

	// search free slot in file[]
	for(i = 0; i < NFILE; i++){
		if(file[i].inode == 0)
			break;
	}
	if(i >= NFILE)
		panic("open: file[] is full!");

	current->fdp[fd] = &file[i];
	file[i].inode = inp;
	file[i].mode = flags;
	file[i].pos = 0;

	return fd;
}

int filecreat(const char *pathname, int flags)
{
	return 0;
}

int fileread(int fd, void *buf, int n)
{
	int count;

	printf("In read\n");
	return count;
}

int filewrite(int fd, const void *buf, int n)
{
	int count;

	printf("In write\n");
	return count;
}

int fileclose(int fd)
{
	int ret;

	printf("In close\n");
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
	printf("sb.s_ninodes:\t%d\n", (uint16_t)sb->s_ninodes);
	printf("sb.s_nzones:\t%d\n", (uint16_t)sb->s_nzones);
	printf("sb.s_imap_blks:\t%x\n", (uint16_t)sb->s_imap_blks);
	printf("sb.s_zmap_blks:\t%x\n", (uint16_t)sb->s_zmap_blks);
	printf("sb.s_firstdatazone:\t%d\n", (uint16_t)sb->s_firstdatazone);
	printf("sb.s_log_zone_size:\t%d\n", (uint16_t)sb->s_log_zone_size);
	printf("sb.s_max_size:\t%x\n", sb->s_max_size);
	printf("sb.s_magic:\t%x\n", (uint16_t)sb->s_magic);
	printf("sb.s_dev:\t%d\n", (uint16_t)sb->s_dev);
	printf("sb.s_flag:\t%d\n", (uint16_t)sb->s_flag);
	printf("sb.s_time:\t%d\n", sb->s_time);
}

void check_minixfs(void)
{
	char sum;
	int i;
	struct buf *sbuf;

	sbuf = bread(ROOTDEV, 2);
	dump_superblock(sbuf->data);
	memmove((char *)&sb, sbuf->data, sizeof(sb));

#if 0
	// check minix magic number
	printf("magic: %x\n", sb.s_magic);
	if(sb.s_magic == MINIX_MAGIC || sb.s_magic == MINIX_MAGIC2){
		printf("The fs is a minix fs\n");
		dump_superblock(&sb);
	} else {  // make a clean minix fs
		mk_minixfs();
	}
#endif
}

void init_minixfs(void)
{
	check_minixfs();
}
