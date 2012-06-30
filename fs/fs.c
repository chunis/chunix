// Minix fs V1.0 implement

#include <types.h>
#include <hd.h>
#include <const.h>
#include <console.h>
#include <x86.h>
#include "../kern/task.h"
#include "fs.h"

extern void printf(char *fmt, ...);

uint8_t fbuf[BLOCK_SIZE];
struct superblock sb;


int open(const char *pathname, int flags)
{
	int fd = -1;
	int i;
	struct inode1 *inp;

	printf("In open\n");
	printf("pathname: %s\n", pathname);

#if 0
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

	current->fdp[fd] = &fdtable[i];
	fdtable[i].fd_inode = inp;
	fdtable[i].fd_mode = flags;
	fdtable[i].fd_off = 0;

#endif
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

	hd_rw(HD_READ, 2, 1, (char *)&sb);

	// check minix magic number
	printf("magic: %x\n", sb.s_magic);
	if(sb.s_magic == MINIX_MAGIC || sb.s_magic == MINIX_MAGIC2){
		printf("The fs is a minix fs\n");
		dump_superblock(&sb);
	} else {  // make a clean minix fs
		mk_minixfs();
	}
}

void init_minixfs(void)
{
	check_minixfs();
}
