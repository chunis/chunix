#include <user.h>

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

	for(;;){
		sleep(1);
		printf("in todo: i = %d\n", i++);
	}
#if 0
	fd = open(file, O_CREAT);
	printf("fd = %d\n", fd);
	write(fd, str, strlen(str));
	close(fd);

	fd = open(file, O_RDWR);
	printf("fd = %d\n", fd);
	read(fd, buf, len);
	buf[len] = '\0';
	close(fd);
	printf("In todo, buf = %s\n", buf);

	while(1){
		printf("%xA.", i++);
		delay(120);
	}
#endif
}
