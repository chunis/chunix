//
// This file is part of ChuniX and is released under the terms of
// GNU GPLv3 License, see License for details.
//
// Copyright (C) 2014, Chunis Deng (<chunchengfh@gmail.com>)
//
#include <user.h>

#define O_CREAT   1
#define O_RDWR    2
#define MOTD_SZ   1024


int main(int argc, char *argv[])
{
	int n, fd0, fd;
	int pid, wpid, ret;
	char buf[MOTD_SZ] = { 0 };
	char *motd = "etc/motd";

	// open stdin, stdout and stderr

	// discard fd=0 since stdin=0 isn't implemented yet
	fd0 = open(motd, O_RDWR);
	printf("fd0 = %d\n", fd0);

	// print motd
	fd = open(motd, O_RDWR);
	printf("fd = %d\n", fd);
	n = read(fd, buf, MOTD_SZ);
	printf("\n%s\n\n", buf);
	sleep(3);

	for(;;){
		printf("init: starting sh...\n");
		pid = fork();
		if(pid < 0){
			printf("Error! init: fork() failed!\n");
			exit();
		} else if(pid == 0){ // child
			ret = exec("bin/hsh", NULL);
			printf("!! init: exec() failed. ret = %d\n", ret);
			exit();
		}

		// parent
		while((wpid = wait()) >= 0 && wpid != pid){
			printf("dead task(pid = %d)!\n", wpid);
		}
	}
}
