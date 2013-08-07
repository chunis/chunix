#include <printf.h>

// syscalls in fs
// do some arguments check, then call into fs.c

int sf_open(const char *path, int flags)
{
	printk("in sf_open: path: %s, flags: %d\n", path, flags);
	//return fileopen(path, flags);
	return sfs_open(path, flags);
}

int sf_creat(const char *path, int flags)
{
	printk("in sf_creat: path: %s, flags: %d\n", path, flags);
	return filecreat(path, flags);
}

int sf_read(int fd, void *buf, int n)
{
	printk("in sf_read: fd: %d, buf: %s, n: %d\n", fd, buf, n);
	//return fileread(fd, buf, n);
	return sfs_read(fd, buf, n);
}

int sf_write(int fd, const void *buf, int n)
{
	printk("in sf_write: fd: %d, buf: %s, n: %d\n", fd, buf, n);
	return filewrite(fd, buf, n);
}

int sf_close(int fd)
{
	printk("in sf_close: fd: %d\n", fd);
	return fileclose(fd);
}

