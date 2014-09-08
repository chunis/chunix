#include <user.h>

int main(int argc, char *argv[])
{
	struct stat state;
	int i = 0;

	stat("bin/hello", &state);
	printf("bin/hello size = |%d|\n", state.st_size);

	printf("Hello, world\n");
	while(i++ < 3){
		sleep(1);
		printf("%d..", i);
	}

	printf("\nGoodbye, world\n");
	exit();
}

