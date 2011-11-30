CC = gcc
AS = as
LD = ld
OBJCOPY = objcopy
OBJDUMP = objdump

.s.o:
	${AS} -a $< -o $*.o >$*.map

all: chunix.img

chunix.img: bootsect kernel tools/blank_hd.img.bz2
	@cat boot/bootsect kern/kernel > tmp.img
	@rm -f chunix.img
	@bzcat tools/blank_hd.img.bz2 > chunix.img
	@dd if=tmp.img of=chunix.img conv=notrunc
	@rm -f tmp.img

bootsect:
	(cd tools && make)
	(cd boot && make)

kernel:
	@echo "start building kernel now..."
	(cd kern && make)

clean:
	(cd tools && make clean)
	(cd boot && make clean)
	(cd kern && make clean)
	@rm -f chunix.img
