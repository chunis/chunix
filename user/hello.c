#include <user.h>

int main(int argc, char *argv[])
{
	char *str = "Hello";

	for(;;){
		sleep(2);
		printf("in hello: I'm %s\n", str);
	}

	return 0;
}

