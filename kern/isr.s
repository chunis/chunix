.set KER_CODE,	0x08	# kernel code segment selector
.set KER_DATA,	0x10	# kernel data segment selector

.text
.globl isr_table, isr_dummy

.macro isr_noerr en
isr\en:
	pushl	$0
	pushl	$\en
	jmp		isr_comm
.endm

.macro isr_err en
isr\en:
	pushl	$\en
	jmp		isr_comm
.endm

isr_dummy:
	pushal
	pushl	%ds
	pushl	%es
	pushl	%fs
	pushl	%gs
	pushl	%ss

	#movw	$KER_CODE, %ax
	#movw	%ax, %cs
	movw	$KER_DATA, %ax
	movw	%ax, %ds
	movw	%ax, %es
	movw	%ax, %fs
	movw	%ax, %gs
	movw	%ax, %ss
	#call	info

	popl	%ss
	popl	%gs
	popl	%fs
	popl	%es
	popl	%ds
	popal
	addl	$0x8, %esp		# for isr_nr and errno
	iret

isr_comm:
	popl	%eax
	pushl	%eax
	addl	$0x30, %eax
	movb	%al, 0xb8000+160*24+6
	jmp		isr_dummy

isr_noerr	0x00
isr_noerr	0x01
isr_noerr	0x02
isr_noerr	0x03
isr_noerr	0x04
isr_noerr	0x05
isr_noerr	0x06
isr_noerr	0x07
isr_err	0x08
isr_noerr	0x09
isr_err	0x0A
isr_err	0x0B
isr_err	0x0C
isr_err	0x0D
isr_err	0x0E
isr_noerr	0x0F
isr_noerr	0x10
isr_err	0x11
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

.globl current, tss
do_timer:
	subl	$0x4, %esp
	pushal
	pushl	%ds
	pushl	%es
	pushl	%fs
	pushl	%gs
	movw	%ss, %dx
	movw	%dx, %ds
	movw	%dx, %es

	incb	(0xb8000+160*24+2)	# just increase a char in screen for funny
	movl	$0x7c00, %esp		# TODO
	call	timer_isr
	movl	current, %esp

	popl	%gs
	popl	%fs
	popl	%es
	popl	%ds
	popal
	addl	$0x4, %esp
	iret

do_keyboard:
	subl	$0x4, %esp
	pushal
	pushl	%ds
	pushl	%es
	pushl	%fs
	pushl	%gs
	movw	%ss, %dx
	movw	%dx, %ds
	movw	%dx, %es

	movl	$0x7c00, %esp		# TODO
	call	keyboard_isr
	movl	current, %esp

	popl	%gs
	popl	%fs
	popl	%es
	popl	%ds
	popal
	addl	$0x4, %esp
	iret

do_hd:
	call	hd_isr
	iret

isr_table:
	.long	isr0x00, isr0x01, isr0x02, isr0x03
	.long	isr0x04, isr0x05, isr0x06, isr0x07
	.long	isr0x08, isr0x09, isr0x0A, isr0x0B
	.long	isr0x0C, isr0x0D, isr0x0E, isr0x0F
	.long	isr0x10, isr0x11, isr0x12, isr0x13
	.long	isr0x14, isr0x15, isr0x16, isr0x17
	.long	isr0x18, isr0x19, isr0x1A, isr0x1B
	.long	isr0x1C, isr0x1D, isr0x1E, isr0x1F
	.long	do_timer, do_keyboard, isr0x22, isr0x23
	.long	isr0x24, isr0x25, isr0x26, isr0x27
	.long	isr0x28, isr0x29, isr0x2A, isr0x2B
	.long	isr0x2C, isr0x2D, do_hd, isr0x2F
