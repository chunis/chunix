#include <user.h>

int main(int argc, char *argv[])
{
	//printf("Hello, world\n");
	//printf("I'm task %x\n", uproc->pid);
	char *str = "Hello";

	write(1, str, 6);

	for(;;) ;

	return 0;
}

