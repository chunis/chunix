include ../make.inc

all: console.o hd.o keyboard.o

%.o: %.c
	${CC} ${CFLAGS} -c -o $@ $<

clean:
	@rm -f *.o
