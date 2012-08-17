#include <user.h>

#define O_CREAT   1
#define O_RDWR    2

int main(int argc, char *argv[])
{
	int i = 0;
	int fd;

	fd = open("/README", O_RDWR);
	printf("fd = %d\n", fd);

	for(;;){
		sleep(30);
		printf("in todo: i = %d\n", i++);
	}
}
