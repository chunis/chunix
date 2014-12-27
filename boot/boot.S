#
# This file is part of ChuniX and is released under the terms of
# GNU GPLv3 License, see License for details.
#
# Copyright (C) 2014, Chunis Deng (<chunchengfh@gmail.com>)
#
#include "asm.h"

# switch to 32-bit protected mode, and jump into C

.set PROT_MODE_CSEG,	0x08	# kernel code segment selector
.set PROT_MODE_DSEG,	0x10	# kernel data segment selector
.set CR0_PE_ON,		0x1	# flag for enable protect mode

.global	start
start:
	.code16
	cli
	cld

	# set up ds, es, ss
	xorw	%ax, %ax
	movw	%ax, %ds
	movw	%ax, %es
	movw	%ax, %ss

	# enable A20
ena20.1:
	inb	$0x64, %al
	testb	$0x2, %al
	jnz	ena20.1

	# 0xd1 -> port 0x64
	movb	$0xd1, %al
	outb	%al, $0x64

ena20.2:
	inb	$0x64, %al
	testb	$0x2, %al
	jnz	ena20.2

	# 0xdf -> port 0x60
	movb	$0xdf, %al
	outb	%al, $0x60

	# switch from real mode to protected mode using a bootstrap GDT
	lgdt	gdtdesc
	movl	%cr0, %eax
	orl	$CR0_PE_ON, %eax
	movl	%eax, %cr0

	# now jump to next instruction, but in 32-bit protected mode
	ljmp	$PROT_MODE_CSEG, $protcseg

	.code32
protcseg:
	# set up data segment registers
	movw	$PROT_MODE_DSEG, %ax
	movw	%ax, %ds
	movw	%ax, %es
	movw	%ax, %fs
	movw	%ax, %gs
	movw	%ax, %ss

	# now set up the stack pointer and call into C
	movl	$start, %esp
	call	bootmain

	# If bootmain returns, loop forever
loop:
	jmp	loop

# Bootstrap GDT
.p2align 2
gdt:
	SEG_NULL	# null seg
	SEG_CODE	# code seg
	SEG_DATA	# data seg

gdtdesc:
	.word	0x17	# sizeof(gdt)-1
	.long	gdt	# address of gdt
