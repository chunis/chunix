include ../make.inc

all: kernel

kernel: entry.o isr.o init.o string.o console.o printf.o exception.o time.o keyboard.o task.o descriptor.o fs.o hd.o tools.o
	${LD} ${LDFLAGS} -s -e _start -Ttext 0x200094 -o kernel $^
	${OBJDUMP} -d kernel > kernel.asm

%.o: %.c
	${CC} ${CFLAGS} -c -o $@ $<

%.o: %.s
	${AS} ${ASFLAGS} -o $@ $<

clean:
	@rm -f *.o kernel kernel.asm
