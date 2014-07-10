#include <user.h>

int main(int argc, char *argv[])
{
	int i;

	printf("Simple echo command for Chunix\n");

	for(i = 1; i < argc; i++){
		printf("%s", argv[i]);
		if(i+1 < argc)
			printf(" ");
		else
			printf("\n");
	}

	return 0;
}

