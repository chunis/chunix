include ../make.inc

all: fs.o sysfile.o buffer.o ext2.o

%.o: %.c
	${CC} ${CFLAGS} -c -o $@ $<

clean:
	@rm -f *.o
