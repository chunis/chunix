CC = gcc
LD = ld

all: kernel

kernel: init.o
	${LD} -b binary -e main -Ttext 0x100000 -o kernel $^

%o: %c
	${CC} -nostdinc -I../inc -c -o $@ $<

clean:
	@rm -f *.o kernel
