//
// This file is part of ChuniX and is released under the terms of
// GNU GPLv3 License, see License for details.
//
// Copyright (C) 2014, Chunis Deng (<chunchengfh@gmail.com>)
//
#ifndef __PRINTF_H__
#define __PRINTF_H__

#include <types.h>

void printk(char *fmt, ...);
void panic(const char *str);
void assert_failed(const char *file, uint32_t line, const char *desc);

#define assert(statement) ((statement) ? (void)0 : assert_failed(__FILE__, __LINE__, #statement))

// for debug
#define KERN_DEBUG 1

#if KERN_DEBUG

#define dprintk printk
#define DBGx(x)  (printk("DBG ---> '%s:%d': " #x " = %x\n", \
			__FILE__, __LINE__, x))
#define DBGd(x) (printk("DBG ---> '%s:%d': " #x " = %d\n", \
			__FILE__, __LINE__, x))
#define DBGs(x) (printk("DBG ---> '%s:%d': " #x " = %s\n", \
			__FILE__, __LINE__, x))
#define DBG(x)	DBGx(x)

#else

#define dprintk
#define DBGx(x)
#define DBGd(x)
#define DBGs(x)
#define DBG(x)	DBGx(x)

#endif // end for debug

#endif
