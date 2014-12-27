#
# This file is part of ChuniX and is released under the terms of
# GNU GPLv3 License, see License for details.
#
# Copyright (C) 2014, Chunis Deng (<chunchengfh@gmail.com>)
#
#include "../inc/mmu.h"

.text
.globl _start
_start:
	cmpl $USTACKTOP, %esp
	jne args_exist

	pushl $0
	pushl $0

args_exist:
	call libmain

loop: jmp loop
