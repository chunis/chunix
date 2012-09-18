#include <user.h>

int main(int argc, char *argv[])
{
	char *str = "Hello";

	for(;;){
		sleep(3);
		printf("in hello: I'm %s\n", str);
	}

	return 0;
}

