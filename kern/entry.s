
.text
.globl _start
.extern init_gdt, main
.extern gdt_ptr

.equ GDT_START,	0x000000
.equ STACKTOP,	0x0A0000
.equ KERN_START,0x100000
.equ KERN_CS,	0x08
.equ KERN_DS,	0x10

_start:
	movl	$STACKTOP, %esp

	sgdt	gdt_ptr
	call	init_gdt
	lgdt	gdt_ptr
	ljmp	$KERN_CS, $pm_mode

pm_mode:
	movw	$KERN_DS, %ax
	movw	%ax, %ds
	movw	%ax, %es
	movw	%ax, %fs
	movw	%ax, %gs
	movw	%ax, %ss

	call	main

loop:
	jmp		loop
