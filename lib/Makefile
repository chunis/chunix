include ../make.inc

LIB_OBJ = entry.o libmain.o syscall.o

%.o: %.c
	${CC} ${CFLAGS} -c -o $@ $<

%.o: %.S
	${CC} ${USER_CFLAGS} -c -o $@ $<

libcx.a: ${LIB_OBJ}
	${AR} r $@ ${LIB_OBJFILES}
