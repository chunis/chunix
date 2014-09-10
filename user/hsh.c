// humber shell

#include <user.h>

#define MAXARGS 16
#define BUFZ	1024

char *hsh_version = "V0.01-dev @ 2014-07-10";
char buf[BUFZ];

static void help(void)
{
	printf("    help\tThis help\n");
	printf("    hello\tRun Hello\n");
	printf("    version\tGet Hsh's version info\n");
}

static void get_hsh_version(void)
{
	printf("    hsh %s\n", hsh_version);
}

static void clean_screen(void)
{
	// TODO
	printf("(Run command 'clr')\n");
	return 0;
}


char *str_forward(char **start, char *end)
{
	char *p, *s;

	p = *start;
	while((p < end) && (*p == ' ' || *p == '\t'))
		p++;
	s = p;

	while(*p != ' ' && *p != '\t' && p < end)
		p++;

	*start = p;
	return s;
}

void split_args(char *str, char **buf, int maxn)
{
	char *p, *q;
	int i = 0;

	q = str + strlen(str);
	p = str_forward(&str, str+strlen(str));
	while(p < q){
		if(++i >= maxn){
			printf("Too many arguments!\n");
			//exit(-1);
		}
		*buf = p;
		*buf++;
		*str++ = '\0';
		p = str_forward(&str, str+strlen(str));
	}
	*buf = '\0'; // add 'NULL' as the last element of **buf
}

int check_xargv(char *xargv[])
{
	int argc = 0;
	char **p = xargv;

	while(*p){
		argc++;
		printf("%d: %s\n", argc, *p++);
	}
	printf("argc = %d\n", argc);

	return 0;
}

static void run_cmd(char *cmd)
{
	char *p, *xargv[MAXARGS];
	char cmdbuf[32] = "bin/";
	char *cmdp = cmdbuf + strlen(cmdbuf);
	int pid;

	printf("(Run command '%s')\n", cmd);
	split_args(cmd, xargv, MAXARGS);
	check_xargv(xargv);

	pid = fork();
	if(pid == -1){
		printf("fork() failed\n");
		exit();
	}
	else if(pid == 0){
		strcpy(cmdp, xargv[0]);
		exec(cmdbuf, xargv);
		printf("cmd '%s' failed\n", xargv[0]);
	}
	wait();
}

int main(int argc, char *argv[])
{
	int ret = 0;

	for(;;){
		printf("hsh > ");

		//memset(buf, 0, BUFZ);
		gets(buf, BUFZ);
		buf[strlen(buf)-1] = '\0';
		//printf("Command: |%s|\n", buf);
		if(strcmp(buf, "help") == 0){
			help();
		} else if(strcmp(buf, "version") == 0){
			get_hsh_version();
		} else if(strcmp(buf, "clr") == 0){
			clean_screen();
		} else if(strncmp(buf, "hello", 5) == 0
			|| strncmp(buf, "echo", 4) == 0){
			run_cmd(buf);
		} else if(strlen(buf)){
			printf("command '%s' not found\n", buf);
			ret = -1;
		}
	}

	exit();
}
