// humber shell

#include <user.h>


hsh_version = "V0.01-dev @ 2014-07-10";

void help(void)
{
	printf("    help\tThis help\n");
	printf("    hello\tRun Hello\n");
	printf("    version\tGet Hsh's version info\n");
}

void get_hsh_version(void)
{
	printf("    hsh %s\n", hsh_version);
}

void clean_screen(void)
{
	// TODO
	printf("(Run command 'clr')\n");
	return 0;
}

int run_cmd(char *cmd)
{
	printf("(Run command '%s')\n", cmd);
	return 0;
}

int main(int argc, char *argv[])
{
	int ret;
	char buf[1024];

	for(;;){
		printf("hsh > ");

		//memset(buf, 0, 1024);
		gets(buf, 1024);
		buf[strlen(buf)-1] = '\0';
		//printf("You Entered: |%s|\n", buf);
		if(strcmp(buf, "help") == 0){
			help();
		} else if(strcmp(buf, "version") == 0){
			get_hsh_version();
		} else if(strcmp(buf, "clr") == 0){
			clean_screen();
		} else if(strcmp(buf, "hello") == 0){
			ret = run_cmd(buf);
			if(ret){
				printf("cmd '%s' failed (ret = %d)\n", ret);
			}
		} else if(strlen(buf)){
			printf("command '%s' not found\n", buf);
		}
	}

	return 0;
}
