#include "../inc/syscall.h"
#include "../inc/trap.h"

.macro SYSCALL name sn
.globl \name
\name:
	movl	$\sn, %eax
	int	$T_SYSCALL
	ret
.endm

SYSCALL open	SYS_open
SYSCALL creat	SYS_creat
SYSCALL read	SYS_read
SYSCALL write	SYS_write
SYSCALL close	SYS_close
SYSCALL get_ticks SYS_get_ticks
SYSCALL fork	SYS_fork
SYSCALL wait	SYS_wait
SYSCALL exit	SYS_exit
SYSCALL exec	SYS_exec
