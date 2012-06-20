#include <inc/lib.h>

int main(int argc, char *argv[])
{
	mytodo();

	return 0;
}

static void delay(int time)
{
	int i, j, k;
	for(i=0; i<time; i++)
		for(j=0; j<100; j++)
			for(k=0; k<1000; k++)
				;
}

void mytodo(void)
{
	int i = 0;

	int fd;
	int len = 30;
	char buf[128];
	char *file = "/todo";
	char *str = "Todo: tool used to keep things need to do";

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
}
