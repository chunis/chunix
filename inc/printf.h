#ifndef __PRINTF_H__
#define __PRINTF_H__

#include <types.h>

//void printf(char *fmt, ...);
void panic(const char *str);
void assert_failed(const char *file, uint32_t line, const char *desc);

#define assert(statement) ((statement) ? (void)0 : assert_failed(__FILE__, __LINE__, #statement))

#define DBGx(x)  (printf("DBG ---> '%s:%d': " #x " = %x\n", \
			__FILE__, __LINE__, x))
#define DBGd(x) (printf("DBG ---> '%s:%d': " #x " = %d\n", \
			__FILE__, __LINE__, x))
#define DBGs(x) (printf("DBG ---> '%s:%d': " #x " = %s\n", \
			__FILE__, __LINE__, x))
#define DBG(x)	DBGx(x)

#endif
