#include <syscall.h>
#include <types.h>
#include "task.h"
#include "printf.h"

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

#define DBG 0
int sys_write(xx)
{
	int fd, len;
	char *p;
	int s;

	fd = argint(0);
	len = argstr(1, &p);

#if(DBG)
	printf("Enter sys_write now...\n");

	printf("fd: %d\n", fd);
	printf("p: %s\n", p);
	printf("len: %d\n", len);

	len = argint(2);
	printf("len: %d\n", len);
#endif

	if(fd == 1 || fd == 2)  // print to stdout
		printf("%s", p);

	return 0;
}

static int (*syscalls[])(void) = {
	[SYS_write] = sys_write,
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