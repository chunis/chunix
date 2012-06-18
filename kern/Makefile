include ../make.inc

all: kernel

OBJS = isr.o init.o string.o console.o printf.o time.o \
		keyboard.o task.o descriptor.o fs.o hd.o tools.o mm.o trap.o

kernel: entry.o $(OBJS)
	#${LD} ${LDFLAGS} -e _start -Ttext 0x200094 -o $@ $^
	$(LD) $(LDFLAGS) -T kernel.ld -o $@ entry.o $(OBJS)
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
