#include "../inc/mmu.h"

.text
.globl _start
.globl multiboot_header
.extern main

.equ KSTACKSIZE, 4096
.equ KERN_CS,	0x08
.equ KERN_DS,	0x10

.equ MBOOT_HEADER_MAGIC,  0x1BADB002 # Multiboot Magic value
.equ MBOOT_PAGE_ALIGN,    (1 << 0)    # Load kernel and modules on page boundary
.equ MBOOT_MEM_INFO,      (1 << 1)    # Provide memory info
.equ MBOOT_HEADER_FLAGS,  (MBOOT_PAGE_ALIGN | MBOOT_MEM_INFO)
.equ MBOOT_CHECKSUM,      -(MBOOT_HEADER_MAGIC + MBOOT_HEADER_FLAGS)

# Multiboot header.  Data to direct multiboot loader.
.p2align 2
multiboot_header:
	.long MBOOT_HEADER_MAGIC
	.long MBOOT_HEADER_FLAGS
	.long MBOOT_CHECKSUM

_start = V2P(va_start)
va_start:
	movl	%eax, %ecx	// save %eax for multiboot check

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

	movl	%ecx, %eax	// restore %eax
	pushl	%ebx
	call	main

loop:
	jmp	loop

.comm stack, KSTACKSIZE
