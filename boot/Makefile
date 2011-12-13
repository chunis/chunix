include ../make.inc

bootsect: boot.o main.o
	${LD} ${LDFLAGS} -N -e start -Ttext 0x7c00 -o $@.out $^
	$(OBJDUMP) -S $@.out > $@.asm
	$(OBJCOPY) -S -O binary -j .text $@.out $@
	../tools/mkboot bootsect

boot.o: main.o boot.S
	${CC} ${CFLAGS} -c boot.S

# use '-Os' to make the output file 'bootsect' smaller than 512 bytes.
main.o: main.c
	${CC} ${CFLAGS} -Os -c main.c

clean:
	@rm -f bootsect* *.o *.map

