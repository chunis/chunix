CC = gcc
LD = ld

all: kernel

kernel: entry.o init.o
	${LD} -s -e _start -Ttext 0x100000 -o kernel $^

%o: %c
	${CC} -nostdinc -I../inc -c -o $@ $<

%o: %s
	${CC} -nostdinc -I../inc -c -o $@ $<


clean:
	@rm -f *.o kernel
