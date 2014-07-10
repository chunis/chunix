include ../make.inc

LIB_OBJS = libmain.o usys.o printf.o utils.o string.o

all: libcx.a entry.o

libcx.a: ${LIB_OBJS}
	${AR} r $@ ${LIB_OBJS}

%.o: %.c
	${CC} ${CFLAGS} -c -o $@ $<

%.o: %.S
	${CC} ${CFLAGS} -c -o $@ $<

clean:
	@rm -f *.o libcx.a
