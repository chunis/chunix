#define VIDEO_RAM	0xb8000

int main(void)
{
	int i = 0;
	char wheel[] = { '\\', '|', '/', '-' };
	// char *p = (char *)VIDEO_RAM + 160*14;

	for(;;){
		__asm__ ("movb	%%al, 0xb8000+160*24"::"a"(wheel[i]));
		if(i == sizeof wheel)
			i = 0;
		else
			i++;
	}

	return 0;
}
