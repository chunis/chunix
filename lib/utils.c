#include <user.h>

void sleep(int sec)
{
	int start;

	start = get_ticks();

	while(1){
		if(get_ticks() - start > sec * 100)
			break;
	}
}
