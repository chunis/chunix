#.section text

.text
.globl _start

_start:
#	movl	$STACKTOP, %esp
	movl	$0xA0000, %esp
	call	main

loop:
	jmp		loop
