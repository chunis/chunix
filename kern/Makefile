include ../make.inc

all: kernel

kernel: entry.o isr.o init.o string.o console.o printf.o exception.o time.o keyboard.o task.o descriptor.o fs.o hd.o tools.o
	${LD} ${LDFLAGS} -e _start -Ttext 0x200094 -o $@ $^
	${OBJDUMP} -d $@ > $@.asm
	${NM} -n $@ > $@.sym

%.o: %.c
	${CC} ${CFLAGS} -c -o $@ $<

%.o: %.s
	${AS} ${ASFLAGS} -o $@ $<

%.o: %.S
	${CC} ${CFLAGS} -c -o $@ $<

clean:
	@rm -f *.o kernel kernel.asm
