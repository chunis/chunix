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
loop:
	jmp	loop
	iret

isr_comm:
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

isr_table:
	.long	isr0x00, isr0x01, isr0x02, isr0x03
	.long	isr0x04, isr0x05, isr0x06, isr0x07
	.long	isr0x08, isr0x09, isr0x0A, isr0x0B
	.long	isr0x0C, isr0x0D, isr0x0E, isr0x0F
	.long	isr0x10, isr0x11, isr0x12, isr0x13
	.long	isr0x14, isr0x15, isr0x16, isr0x17
	.long	isr0x18, isr0x19, isr0x1A, isr0x1B
	.long	isr0x1C, isr0x1D, isr0x1E, isr0x1F
