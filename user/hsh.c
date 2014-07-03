// humber shell

#include <user.h>

int main(int argc, char *argv[])
{
	char buf[1024];

	for(;;){
		printf("hsh > ");

		//memset(buf, 0, 1024);
		gets(buf, 1024);
		printf("You Enter: %s\n", buf);
	}

	return 0;
}
