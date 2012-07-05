#include <syscall.h>
#include <types.h>
#include "task.h"
#include "printf.h"
#include "sched.h"

extern int sf_open(const char *path, int flags);
extern int sf_creat(const char *path, int flags);
extern int sf_read(int fd, void *buf, int n);
extern int sf_write(int fd, const void *buf, int n);
extern int sf_close(int fd);

// return the int at addr from task p.
int getint(TASK_STRUCT *tp, uint32_t addr)
{
	// TODO: need to check if addr is in tp's address space
	return *(int *)addr;
}

// get the nul-terminated string at addr from task p.
// just sets *pp to point at it, and returns the length of string
int getstr(TASK_STRUCT *tp, uint32_t addr, char **pp)
{
	char *s;

	*pp = (char *)addr;
	for(s = *pp; *s; s++)
		; // do nothing

	return s - *pp;
}

// get the nth int syscall argument
int argint(int n)
{
	return getint(current, current->tf->esp+4+4*n);
}

// get the nth int syscall argument used as a pointer to a block of memory
char *argptr(int n)
{
	return (char *)argint(n);
}

// get the nth int syscall argument used as a string pointer
int argstr(int n, char **pp)
{
	int addr;

	addr = argint(n);
	return (char *)getstr(current, addr, pp);
}

uint32_t sys_gticks(void)
{
	return timer_ticks;
}

int sys_open(void)
{
	int flag;
	char *path;

	argstr(0, &path);
	flag = argint(1);

	sf_open(path, flag);
}

int sys_creat(void)
{
	int flag;
	char *path;

	argstr(0, &path);
	flag = argint(1);

	sf_creat(path, flag);
}

int sys_read(void)
{
	int fd, len;
	char *p;
	int s;

	fd = argint(0);
	len = argstr(1, &p);
	len = argint(2);

	sf_read(fd, p, len);
}

int sys_write(void)
{
	int fd, len;
	char *p;
	int s;

	fd = argint(0);
	len = argstr(1, &p);
	len = argint(2);

	if(fd == 1 || fd == 2){  // print to stdout
		if(len == 1)
			put_c(*p);
		else
			printf("%s", p);
	} else {
		sf_write(fd, p, len);
	}

	return 0;
}

int sys_close(void)
{
	int fd = argint(0);

	sf_close(fd);
}

static int (*syscalls[])(void) = {
	[SYS_open] = sys_open,
	[SYS_creat] = sys_creat,
	[SYS_read] = sys_read,
	[SYS_write] = sys_write,
	[SYS_close] = sys_close,
	[SYS_get_ticks] = sys_gticks,
};

void syscall(void)
{
	int num;

	// get syscall number
	num = current->tf->eax;

	if(syscalls[num]){
		current->tf->eax = syscalls[num]();
	} else {
		printf("Unknown syscall number: %d\n", num);
		current->tf->eax = -1;
	}
}
