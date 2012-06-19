include ../make.inc

initcode: initcode.S
	$(CC) $(CFLAGS) -nostdinc -I. -c initcode.S
	$(LD) $(LDFLAGS) -N -e start -Ttext 0 -o initcode.out initcode.o
	$(OBJCOPY) -S -O binary initcode.out initcode
	$(OBJDUMP) -S initcode.o > initcode.asm

hello: hello.c
	$(CC) $(CFLAGS) -nostdinc -I. -c hello.c
	$(LD) $(LDFLAGS) -N -e start -Ttext 0 -o hello.out hello.o
	$(OBJCOPY) -S -O binary hello.out hello
	$(OBJDUMP) -S hello.o > hello.asm
