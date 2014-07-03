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

void msleep(int msec)
{
	int start;

	start = get_ticks();

	while(1){
		if(get_ticks() - start > msec / 10)
			break;
	}
}

// get at most n chars to buf, ends with '\n' if less than n chars
void gets(char *buf, int n)
{
	int i = 0;
	char c;

	while(i+1 < n){
		if(read(0, &c, 1) < 1)
			break;
		buf[i++] = c;
		if(c == '\n' || c == '\r')
			break;
	}
	buf[i] = '\0';
}
