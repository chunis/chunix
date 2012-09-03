include ../make.inc

KERN = isr.o init.o string.o printf.o time.o syscall.o sched.o \
		task.o descriptor.o tools.o mm.o trap.o entry.o swtch.o

all: $(KERN)

%.o: %.c
	${CC} ${CFLAGS} -c -o $@ $<

%.o: %.s
	${AS} ${ASFLAGS} -o $@ $<

%.o: %.S
	${CC} ${CFLAGS} -c -o $@ $<

clean:
	@rm -f *.o
