.text
.globl _start
_start:
	pushl $0
	pushl $0
	call libmain

loop: jmp loop
