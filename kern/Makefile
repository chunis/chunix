CC = gcc
LD = ld
CFLAG = -nostdinc -I../inc -fno-builtin
OBJDUMP = objdump

all: kernel

kernel: entry.o init.o string.o
	${LD} -s -e _start -Ttext 0x200094 -o kernel $^
	${OBJDUMP} -d kernel > kernel.asm

%o: %c
	${CC} ${CFLAG} -c -o $@ $<

%o: %s
	${CC} ${CFLAG} -c -o $@ $<


clean:
	@rm -f *.o kernel
