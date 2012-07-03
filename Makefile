# Makefile for Chunix, a simple x86 os

include make.inc


DRV  = console.o hd.o keyboard.o
FS   = fs.o sysfile.o
KERN = isr.o init.o string.o printf.o time.o syscall.o sched.o \
		task.o descriptor.o tools.o mm.o trap.o
USER = hello todo

DRVOBJ  = $(patsubst %,drv/%,$(DRV))
FSOBJ   = $(patsubst %,fs/%,$(FS))
KERNOBJ = $(patsubst %,kern/%,$(KERN))
USEROBJ = $(patsubst %,user/%,$(USER))

OBJS = $(DRVOBJ) $(FSOBJ) $(KERNOBJ) $(USEROBJ)

#HD = hd.img
HD = minixfs.img

all: chunix.img $(HD)

qemu: chunix.img $(HD)
	qemu -m 32 -hda chunix.img -hdb $(HD)

qemu-gdb: chunix.img $(HD) .gdbinit
	qemu -m 32 -hda chunix.img -hdb $(HD) -S -gdb tcp::1234

bochs: chunix.img $(HD)
	bochs -f hd.bxrc

$(HD): tools/$(HD).bz2
	@bzcat tools/$(HD).bz2 > $(HD)

chunix.img: bootsect kern/kernel tools/blank_hd.img.bz2
	@cat boot/bootsect kern/kernel > tmp.img
	@rm -f chunix.img
	@bzcat tools/blank_hd.img.bz2 > chunix.img
	@dd if=tmp.img of=chunix.img conv=notrunc
	@rm -f tmp.img

kern/kernel: buildall kern/kernel.ld
	$(LD) $(LDFLAGS) -T kern/kernel.ld -o $@ kern/entry.o $(KERNOBJ) $(FSOBJ) $(DRVOBJ) -b binary $(USEROBJ)
	${OBJDUMP} -d $@ > $@.asm
	${NM} -n $@ > $@.sym

buildall:
	(cd drv && make)
	(cd fs && make)
	(cd kern && make)
	(cd user && make)

bootsect:
	(cd tools && make)
	(cd boot && make)

$(DRVOBJ):
	(cd drv && make)

$(FSOBJ):
	(cd fs && make)

$(KERNOBJ) kern/entry.o:
	(cd kern && make)

$(USEROBJ):
	(cd user && make)

clean:
	(cd tools && make clean)
	(cd boot && make clean)
	(cd kern && make clean)
	(cd drv && make clean)
	(cd fs && make clean)
	(cd lib && make clean)
	(cd user && make clean)
	@rm -f chunix.img #$(HD)

cleanall: clean
	@rm -f $(HD) *.img bochs.log
