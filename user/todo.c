#include <user.h>

#define O_CREAT   1
#define O_RDWR    2

char *xargv[] = { "bin/echo", "my", "chunix", 0 };
void mytest(void)
{
	printf("Start mytest()...\n");
	exec("bin/echo", xargv);
	printf("End mytest()...\n");
}

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
		exit();
	} else if(pid == 0){ // child
		printf("I'm child\n");
		//mytest();
		ret = exec("bin/hsh", NULL);
		printf("!! exec() failed. ret = %d\n", ret);
		exit();
	}

	// parent
	ret = wait();
	printf("I'm parent, ret = %d\n", ret);

	for(;;){
		sleep(15);
		printf("in todo: i = %d\n", i++);
	}
}
