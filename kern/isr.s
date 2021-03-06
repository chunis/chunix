#
# This file is part of ChuniX and is released under the terms of
# GNU GPLv3 License, see License for details.
#
# Copyright (C) 2014, Chunis Deng (<chunchengfh@gmail.com>)
#
.set KER_CODE,	0x08	# kernel code segment selector
.set KER_DATA,	0x10	# kernel data segment selector

.text

.macro isr_noerr en
isr\en:
	pushl	$0
	pushl	$\en
	jmp	isr_comm
.endm

.macro isr_err en
isr\en:
	pushl	$\en
	jmp	isr_comm
.endm

isr_comm:
	subl	$0x4, %esp
	pushal
	pushl	%ds
	pushl	%es
	pushl	%fs
	pushl	%gs

	movw	$KER_DATA, %ax
	movw	%ax, %ds
	movw	%ax, %es
	movw	%ax, %fs
	movw	%ax, %gs
	pushl	%esp	# now esp points to the start of stack_frame
	call	trap
	addl	$0x4, %esp

	# after return from trap(), just drop to trap_ret
	jmp	trap_ret

.globl trap_ret
trap_ret:
	popl	%gs
	popl	%fs
	popl	%es
	popl	%ds
	popal
	addl	$0xC, %esp		# for isr_nr, errno and ret_addr
	iret


isr_noerr	0x00
isr_noerr	0x01
isr_noerr	0x02
isr_noerr	0x03
isr_noerr	0x04
isr_noerr	0x05
isr_noerr	0x06
isr_noerr	0x07
isr_err		0x08
isr_noerr	0x09
isr_err		0x0A
isr_err		0x0B
isr_err		0x0C
isr_err		0x0D
isr_err		0x0E
isr_noerr	0x0F
isr_noerr	0x10
isr_err		0x11
isr_noerr	0x12
isr_noerr	0x13
isr_noerr	0x14
isr_noerr	0x15
isr_noerr	0x16
isr_noerr	0x17
isr_noerr	0x18
isr_noerr	0x19
isr_noerr	0x1A
isr_noerr	0x1B
isr_noerr	0x1C
isr_noerr	0x1D
isr_noerr	0x1E
isr_noerr	0x1F

# for IRQ0 ~ IRQ15
isr_noerr	0x20
isr_noerr	0x21
isr_noerr	0x22
isr_noerr	0x23
isr_noerr	0x24
isr_noerr	0x25
isr_noerr	0x26
isr_noerr	0x27
isr_noerr	0x28
isr_noerr	0x29
isr_noerr	0x2A
isr_noerr	0x2B
isr_noerr	0x2C
isr_noerr	0x2D
isr_noerr	0x2E
isr_noerr	0x2F

# for syscall
isr_noerr	0x30

.data
.globl isr_table
isr_table:
	.long	isr0x00, isr0x01, isr0x02, isr0x03
	.long	isr0x04, isr0x05, isr0x06, isr0x07
	.long	isr0x08, isr0x09, isr0x0A, isr0x0B
	.long	isr0x0C, isr0x0D, isr0x0E, isr0x0F
	.long	isr0x10, isr0x11, isr0x12, isr0x13
	.long	isr0x14, isr0x15, isr0x16, isr0x17
	.long	isr0x18, isr0x19, isr0x1A, isr0x1B
	.long	isr0x1C, isr0x1D, isr0x1E, isr0x1F
	.long	isr0x20, isr0x21, isr0x22, isr0x23
	.long	isr0x24, isr0x25, isr0x26, isr0x27
	.long	isr0x28, isr0x29, isr0x2A, isr0x2B
	.long	isr0x2C, isr0x2D, isr0x2E, isr0x2F
	.long	isr0x30
