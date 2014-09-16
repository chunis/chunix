#include <user.h>

int main(int argc, char *argv[])
{
	struct stat state;
	int i = 0;

	sleep(4);
	stat("bin/hello", &state);
	dprintf("bin/hello size = |%d|\n", state.st_size);

	dprintf("start hello: Hello, world\n");
	for(;;){  // run forever, until we remove it totally from kernel
		sleep(60);
		dprintf("hello %d..\n", i);
	}

	dprintf("\nend hello: Goodbye, world\n");
	exit();
}

