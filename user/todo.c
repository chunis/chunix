#include <user.h>

#define O_CREAT   1
#define O_RDWR    2

void mytodo(void);

int main(int argc, char *argv[])
{
	mytodo();

	return 0;
}

void mytodo(void)
{
	int i = 0;

	int fd;
	int len = 30;
	char buf[128];
	char *file = "/todo";
	char *str = "Todo: tool used to keep things need to do";

#if 1
	int fdo[4];

	for(i=0; i<4; i++)
		fdo[i] = open("/README", O_RDWR);
	for(i=0; i<4; i++)
		printf("fdo[%d] = %d\n", i, fdo[i]);
#else
	// check if read /README works or not
	fd = open("/README", O_RDWR);
	printf("fd = %d\n", fd);
	read(fd, buf, len);
	buf[len] = '\0';
	close(fd);
	printf("In todo, buf = %s\n", buf);

	// check if creat() works or not
	fd = open(file, O_CREAT);
	printf("fd = %d\n", fd);
	write(fd, str, len);
	close(fd);

	// read out to check if write works or not
	fd = open(file, O_RDWR);
	printf("fd = %d\n", fd);
	read(fd, buf, len);
	buf[len] = '\0';
	close(fd);
	printf("In todo, buf = %s\n", buf);
#endif

	for(;;){
		sleep(30);
		printf("in todo: i = %d\n", i++);
	}
}
