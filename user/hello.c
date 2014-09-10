#include <user.h>

int main(int argc, char *argv[])
{
	struct stat state;
	int i = 0;

	sleep(4);
	stat("bin/hello", &state);
	printf("bin/hello size = |%d|\n", state.st_size);

	printf("Hello, world\n");
	for(;;){  // run forever, until we remove it totally from kernel
		sleep(60);
		printf("hello %d..\n", i);
	}

	printf("\nGoodbye, world\n");
	exit();
}

