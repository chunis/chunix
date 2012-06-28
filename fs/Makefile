include ../make.inc

all: sfs.o

%.o: %.c
	${CC} ${CFLAGS} -c -o $@ $<

clean:
	@rm -f *.o
