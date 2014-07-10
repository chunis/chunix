#include <user.h>

int main(int argc, char *argv[])
{
	struct stat state;

	stat("bin/todo", &state);
	printf("bin/todo size = |%d|\n", state.st_size);

	for(;;){
		sleep(400);
		printf("hello> \n");
	}

	return 0;
}

