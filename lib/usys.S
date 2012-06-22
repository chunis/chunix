#include "syscall.h"
#include "../inc/trap.h"

.macro SYSCALL name sn
.globl \name
\name:
	movl	$\sn, %eax
	int	$T_SYSCALL
	ret
.endm

SYSCALL write SYS_write
