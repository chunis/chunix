#include <const.h>
#include "tools.h"
#include <console.h>
#include "string.h"
#include "sys.h"

char monbuf[1024];  // monitor buffer

char *readline(char *str)
{
	int i = 0;

	if(str != NULL)
		printf("%s", str);

	while( (monbuf[i++] = get_c()) != '\n')
		;  // do nothing
	monbuf[--i] = '\0'; // delete the last '\n'

	return monbuf;
}

char *strip_lspace(char const *str)
{
	while(str && *str == ' ')
		str++;
	return str;
}

// a very simple calculator just works on +-*/%
void bc(void)
{
	printf("Sorry, not done yet\n");
}

void clr(void)
{
	clean_screen();
}

void help(void)
{
	printf("Available commands:\n");
	printf("    bc: a very simple calculator just works on +,-,*,/,%%\n");
	printf("    clr: clean screen\n");
	printf("    info: some info about chunix\n");
	printf("    help: show available commands\n");
}

void sysinfo(void)
{
	printf("ChuniX, versioin: %s\n\n", sys_version);
	printf("%s\n", sys_info);
	printf("Build: %s\n\n", __DATE__);
}

void monitor(void)
{
	char *input;
	char *cmd;

	// run as a simple shell forever
	while(1){
		input = readline(">>> ");

		cmd = strip_lspace(input);
		if(*cmd == '\0')
			continue;

		if( !strncmp(cmd, "bc", 2) ){
			bc();
		} else if( !strncmp(cmd, "clr", 3) ){
			clr();
		} else if( !strncmp(cmd, "help", 4) ){
			help();
		} else if( !strncmp(cmd, "info", 4) ){
			sysinfo();
		} else{
			printf("Error! no such command: '%s'. type 'help' for help\n", cmd);
		}
	}
}

static void delay(int time)
{
	int i, j, k;
	for(i=0; i<time; i++)
		for(j=0; j<100; j++)
			for(k=0; k<1000; k++)
				;
}

// do some test for file functions
void test_fs(void)
{
	int i = 0;

	int fd;
	int len = 20;
	char buf[64];
	char *file = "/first_file";
	char *str = "file used to check file functions";

#if 0
	fd = fileopen(file, O_CREAT);
	printf("fd = %d\n", fd);
	filewrite(fd, str, strlen(str));
	fileclose(fd);

	fd = fileopen(file, O_RDWR);
	printf("fd = %d\n", fd);
	fileread(fd, buf, len);
	buf[len] = '\0';
	fileclose(fd);
	printf("In test_fs, buf = %s\n", buf);
#endif

#if 1
	while(1){
		printf("%xA.", i++);
		delay(120);
	}
#endif
	monitor();
}


void wheel(void)
{
	int i = 0;
	char wheel[] = { '\\', '|', '/', '-' };
	for(;;){
		__asm__ ("movb	%%al, 0x800b8000+160*24"::"a"(wheel[i]));
		if(i == sizeof wheel)
			i = 0;
		else
			i++;
	}
}
