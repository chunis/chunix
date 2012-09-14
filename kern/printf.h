#ifndef __PRINTF_H__
#define __PRINTF_H__

void printf(char *fmt, ...);
void panic(char *str);

#define DBGx(x)  (printf("DBG ---> '%s:%d': " #x " = %x\n", \
			__FILE__, __LINE__, x))
#define DBGd(x) (printf("DBG ---> '%s:%d': " #x " = %d\n", \
			__FILE__, __LINE__, x))
#define DBGs(x) (printf("DBG ---> '%s:%d': " #x " = %s\n", \
			__FILE__, __LINE__, x))
#define DBG(x)	DBGx(x)

#endif
