.text
.globl _start
_start:
	call libmain

loop: jmp loop
