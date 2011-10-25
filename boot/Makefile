CC = gcc
AS = as
LD = ld

all: boot.o main.o
	${LD} --oformat binary -N -e start -Ttext 0x7c00 -o bootsect $^
	../tools/mkboot bootsect

boot.o: main.o boot.S
	${CC} -nostdinc -I../inc/ -c boot.S

# use '-Os' to make the output file 'bootsect' smaller than 512 bytes.
main.o: main.c
	${CC} -Os -nostdinc -I../inc/ -c main.c

clean:
	@rm -f bootsect *.o *.map

