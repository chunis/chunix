//
// This file is part of ChuniX and is released under the terms of
// GNU GPLv3 License, see License for details.
//
// Copyright (C) 2014, Chunis Deng (<chunchengfh@gmail.com>)
//
#ifndef __TRAP_H__
#define __TRAP_H__

#define T_GPFLT		13	// general protection fault
#define T_PGFLT		14	// page fault
#define T_IRQ0		32	// IRQ 0
#define T_SYSCALL	48	// system call

// for IRQs
#define IRQ_TIMER	 0
#define IRQ_KBD		 1
#define IRQ_COM1	 4
#define IRQ_SPURIOUS	 7
#define IRQ_IDE		14

#endif
