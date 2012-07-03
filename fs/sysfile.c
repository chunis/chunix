// syscalls in fs
// do some arguments check, then call into fs.c

int sf_open(const char *path, int flags)
{
	printf("in sf_open: path: %s, flags: %d\n", path, flags);
	return 0;
}

int sf_creat(const char *path, int flags)
{
	printf("in sf_creat: path: %s, flags: %d\n", path, flags);
	return 0;
}

int sf_read(int fd, void *buf, int n)
{
	printf("in sf_read: fd: %d, buf: %s, n: %d\n", fd, buf, n);
	return 0;
}

int sf_write(int fd, const void *buf, int n)
{
	printf("in sf_write: fd: %d, buf: %s, n: %d\n", fd, buf, n);
	return 0;
}

int sf_close(int fd)
{
	printf("in sf_close: fd: %d\n", fd);
	return 0;
}

