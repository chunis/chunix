#include <stdio.h>
#include <sys/stat.h>


int get_file_size(char *filepath)
{
	int ret;
	struct stat buf;

	ret = stat(filepath, &buf);
	if(ret == -1){
		printf("stat error!\n");
		return ret;
	}

	printf("size: %d\n", buf.st_size);
	return buf.st_size;
}

int main(int argc, char *argv[])
{
	FILE *fp;
	int size;
	char num[3] = { 0x0, 0x55, 0xaa };

	if(argc < 2){
		printf("Error! \nUsage: %s input_file\n", argv[0]);
		return -1;
	}

	size = get_file_size(argv[1]);
	if(size > 510){
		printf("File %s is too large to stay in the first sector!\n", argv[1]);
		return -1;
	}

	fp = fopen(argv[1], "ab");
	if(fp == NULL){
		printf("Open file %s failed!\n", argv[1]);
		return -1;
	}

	size = 510 - size;
	while(size-- > 0){
		fwrite(num, 1, 1, fp);
	}

	fwrite(num+1, 1, 1, fp);
	fwrite(num+2, 1, 1, fp);
	fclose(fp);

	return 0;
}
