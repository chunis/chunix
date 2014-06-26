#include <user.h>

#define O_CREAT   1
#define O_RDWR    2

int main(int argc, char *argv[])
{
	int n, i = 0;
	int fd;
	char buf[40] = { 0 };
	int pid, ret;

#if 0
	fd = open("README", O_RDWR);
	printf("fd = %d\n", fd);
	n = read(fd, buf, 39);
	printf("n = %d\n%s\n\n", n, buf);
#endif

	pid = fork();
	if(pid < 0){
		printf("Error! fork() failed!\n");
	} else if(pid == 0){
		printf("I'm child\n");
		ret = exec("bin/hello", NULL);
		printf("After exec(), ret = %d\n", ret);

		for(;;){
			sleep(5);
			printf("in todo for child: i = %d\n", i++);
		}
		exit();
	} else {
		ret = wait();
		printf("I'm parent, ret = %d\n", ret);
	}

	for(;;){
		sleep(8);
		printf("in todo: i = %d\n", i++);
	}
}
