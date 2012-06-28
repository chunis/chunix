#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#include <types.h>
#include "task.h"
#include "descriptor.h"

#define ISR_NUM		0x30

#define KER_NULL	0x00
#define KER_CODE	0x08
#define KER_DATA	0x10
#define USR_CODE	0x18
#define USR_DATA	0x20
#define KER_TSS 	0x28

TSS_STRUCT tss;

#endif
