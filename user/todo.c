#include <user.h>

#define O_CREAT   1
#define O_RDWR    2

int main(int argc, char *argv[])
{
	int n, i = 0;
	int fd;
	char buf[40] = { 0 };

	fd = open("README", O_RDWR);
	printf("fd = %d\n", fd);
	n = read(fd, buf, 30);
	printf("n = %d\n%s", n, buf);

	for(;;){
		sleep(3);
		printf("in todo: i = %d\n", i++);
	}
}
