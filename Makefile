CC = gcc
AS = as
LD = ld
OBJCOPY = objcopy
OBJDUMP = objdump

.s.o:
	${AS} -a $< -o $*.o >$*.map

all: chunix.img

chunix.img: bootsect kernel
	@cat boot/bootsect kern/kernel > chunix.img

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
