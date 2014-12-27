//
// This file is part of ChuniX and is released under the terms of
// GNU GPLv3 License, see License for details.
//
// Copyright (C) 2014, Chunis Deng (<chunchengfh@gmail.com>)
//
#include <user.h>

// a very simple cat. Just work with files, no stdin support

char buf[512];

void cat(int fd)
{
	int n;

	while( (n=read(fd, buf, sizeof(buf))) > 0)
		write(1, buf, n);
}

int main(int argc, char *argv[])
{
	int i = 0, fd;

	while(++i < argc){
		printf("debug cat: open %s\n", argv[i]);
		fd = open(argv[i], 0);
		if(fd < 0){
			printf("Error, open failed\n");
			return -1;
		}

		cat(fd);
		close(fd);
	}

	exit();
}
