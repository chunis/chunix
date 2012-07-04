#ifndef __CONST_H__
#define __CONST_H__

#define NULL  ((void *)0)

#define KER_NULL	0x00
#define KER_CODE	0x08
#define KER_DATA	0x10
#define USR_CODE	0x18
#define USR_DATA	0x20
#define KER_TSS 	0x28

// some params for kernel
#define NPROC		64	// maximum number of processes (tasks)
#define NBUF		16	// number of disk block buffer
#define NOFILE		16	// maximum opened files each tasks
#define NFILE		256	// opened files for all tasks
#define NINODE		128	// maximum number of active inodes
#define KSTACKSIZE	4096	// per-task kernel stack size

#define FL_IF	0x00000200

// Descriptor attributes
// Byte-6
#define DA_LIM_4K	0x80
#define DA_32		0x40
// Byte-5
#define DA_P		0x80
#define DA_DPL0		0x00
#define DA_DPL3		0x60
// Byte-5: code/data segment types
#define DA_DR		0x90	// data, read-only
#define DA_DRW		0x92	// data, read, write
#define DA_DRWA		0x93	// data, read, write, accessed
#define DA_C		0x98	// code, exec-only
#define DA_CR		0x9A	// code, exec, read
#define DA_CC		0x9C	// code, confirm
#define DA_CCR		0x9E	// code, confirm, read
// Byte-5: system/gate segment types
#define DA_LDT		0x82	// ldt
#define DA_TASKGATE		0x85	// task gate
#define DA_386TSS		0x89	// available 386 TSS
#define DA_386TSS_B		0x8B	// busy 386 TSS
#define DA_CALLGATE		0x8C	// 386 call gate
#define DA_INTRGATE		0x8E	// 386 interrupt gate
#define DA_TRAPGATE		0x8F	// 386 trap gate

// Selector attribute
#define SA_RPL0		0
#define SA_RPL3		3
#define SA_TIG		0
#define SA_TIL		4
#define SA_RPL_MASK	~0x3	// 0...0011
#define SA_TI_MASK	~0x7	// 0...0111

#endif
