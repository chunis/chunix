#include "../inc/mmu.h"

.text
.globl _start, va_start
.extern init_gdt, main
.extern gdt_ptr

.equ KSTACKSIZE, 4096
.equ KERN_CS,	0x08
.equ KERN_DS,	0x10

_start = V2P(va_start)
va_start:
	// turn on paging
	movl	%cr4, %eax
	orl	$0x10, %eax	// page size extention
	movl	%eax, %cr4

	movl	$(V2P(entrypgdir)), %eax
	movl	%eax, %cr3

	movl	%cr0, %eax
	orl	$0x80000000, %eax  // enable paging
	movl	%eax, %cr0

	// setup stack pointer
	movl	$(stack + KSTACKSIZE), %esp
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
	jmp	loop

.comm stack, KSTACKSIZE
