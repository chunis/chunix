include ../make.inc

all: fs.o

%.o: %.c
	${CC} ${CFLAGS} -c -o $@ $<

clean:
	@rm -f *.o
