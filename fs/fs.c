// Minix fs V1.0 implement

#include <types.h>
#include <hd.h>
#include <const.h>
#include <console.h>
#include <x86.h>
#include "../kern/task.h"

char buf[SECT_SIZE];
char buf2[SECT_SIZE*2];

struct superblock sb;

void hd_rw(int cmd, int nb, int offset, char *buf);



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

void mkfs(void) { }

void check_minixfs(void)
{
	char sum;
	int i;

	printf("Enter check_fs now\n");
	hd_rw(HD_READ, 1, 0, buf);

	// check minix magic number

	for(i=0; i<sizeof(buf); i++)
		printf("%x ", buf[i]);
}

void init_minixfs(void)
{
	check_minixfs();
}
