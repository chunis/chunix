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
#define IRQ_IDE		14

#endif
