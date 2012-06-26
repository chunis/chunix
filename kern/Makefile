include ../make.inc

all: kernel

OBJS = isr.o init.o string.o console.o printf.o time.o syscall.o sched.o \
		keyboard.o task.o descriptor.o fs.o hd.o tools.o mm.o trap.o

kernel: entry.o $(OBJS) ../user/hello ../user/todo
	#${LD} ${LDFLAGS} -e _start -Ttext 0x200094 -o $@ $^
	$(LD) $(LDFLAGS) -T kernel.ld -o $@ entry.o $(OBJS) -b binary ../user/hello ../user/todo
	${OBJDUMP} -d $@ > $@.asm
	${NM} -n $@ > $@.sym

../user/hello:
	(cd ../user && make)

../user/todo:
	(cd ../user && make)

%.o: %.c
	${CC} ${CFLAGS} -c -o $@ $<

%.o: %.s
	${AS} ${ASFLAGS} -o $@ $<

%.o: %.S
	${CC} ${CFLAGS} -c -o $@ $<

clean:
	@rm -f *.o kernel kernel.asm
