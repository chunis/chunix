//
// This file is part of ChuniX and is released under the terms of
// GNU GPLv3 License, see License for details.
//
// Copyright (C) 2014, Chunis Deng (<chunchengfh@gmail.com>)
//
#include <user.h>

int main(int argc, char *argv[])
{
	int i;

	/*
	printf("Simple echo command for Chunix\n");
	printf("argc @ %x = %d\n", &argc, argc);
	*/

	for(i = 1; i < argc; i++){
		printf("%s", argv[i]);
		if(i+1 < argc)
			printf(" ");
		else
			printf("\n");
	}

	exit();
}

