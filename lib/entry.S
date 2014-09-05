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
