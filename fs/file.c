//
// This file is part of ChuniX and is released under the terms of
// GNU GPLv3 License, see License for details.
//
// Copyright (C) 2014, Chunis Deng (<chunchengfh@gmail.com>)
//
#include <fs.h>
#include <printf.h>

//
// File process
//
int fileopen(const char *path, int flags)
{
	printk("In fileopen\n");
	return 0;
}

int filecreat(const char *path, int flags)
{
	printk("In filecreat\n");
	return 0;
}

int fileread(int fd, void *buf, int n)
{
	int count;

	printk("In fileread\n");
	return count;
}

int filewrite(int fd, const void *buf, int n)
{
	int count;

	printk("In filewrite\n");
	return count;
}

int fileclose(int fd)
{
	int ret;

	printk("In fileclose\n");
	return 0;
}
