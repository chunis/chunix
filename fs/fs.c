// Minix fs V1.0 implement

#include <types.h>
#include <hd.h>
#include <const.h>
#include <console.h>
#include <x86.h>
#include "../kern/task.h"
#include "fs.h"

extern void printf(char *fmt, ...);
void hd_rw(int cmd, int nb, int offset, char *buf);

uint8_t fbuf[SECT_SIZE];
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

void check_minixfs(void)
{
	char sum;
	int i;
	struct d_superblock *sb;

	printf("Enter check_fs now\n");
	hd_rw(HD_READ, 1, 2, fbuf);

	/*
	// TODO: why below loop will output all 0x0s?
	// maybe something wrong in printf() with %x
	for(i=0; i<sizeof(fbuf); i++)
		printf("%x ", fbuf[i]);
	printf("\n%x\n", fbuf[0]);  // but this is right: it isn't 0x0
	*/

	// check minix magic number
	sb = (struct d_superblock *)fbuf;
	printf("magic: %x\n", sb->s_magic);
	if(sb->s_magic == MINIX_MAGIC || sb->s_magic == MINIX_MAGIC2){
		printf("The fs is a minix fs\n");
	} else {  // make a clean minix fs
		mk_minixfs();
	}
}

void init_minixfs(void)
{
	check_minixfs();
}
