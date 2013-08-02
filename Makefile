# Makefile for Chunix, a simple x86 os

include make.inc


DRVOBJ  = $(patsubst %,drv/%,$(DRV))
FSOBJ   = $(patsubst %,fs/%,$(FS))
KERNOBJ = $(patsubst %,kern/%,$(KERN))
USEROBJ = $(patsubst %,user/%,$(USER))
SFS_FILE = $(USEROBJ) README

OBJS = $(DRVOBJ) $(FSOBJ) $(KERNOBJ) $(USEROBJ)
GDB_ARG = -S -gdb tcp::1234

HD = hd.img
#HD = minixfs.img
FD = floppy.img

all: chunix.img $(HD)

qemu: chunix.img $(HD)
	qemu -m 32 -hda chunix.img -hdb $(HD)

qemu-gdb: chunix.img $(HD) .gdbinit
	qemu -m 32 -hda chunix.img -hdb $(HD) $(GDB_ARG)

bochs: chunix.img $(HD)
	sed -i 's/hd.img/$(HD)/' hd.bxrc
	bochs -f hd.bxrc

$(HD): tools/$(HD).bz2 mk_sfs_fs
	if [ $(HD) != "hd.img" ]; then bzcat tools/$(HD).bz2 > $(HD); fi

mk_sfs_fs: mk_sfs $(SFS_FILE)
	./mk_sfs $(HD) $(SFS_FILE)

mk_sfs: tools/mk_sfs.c
	gcc -o mk_sfs tools/mk_sfs.c

grub: kernel initrd tools/$(FD).bz2 $(HD)
	@bzcat tools/$(FD).bz2 > $(FD)
	tools/update_kernel.sh
	qemu -m 32 -fda $(FD) -hdb $(HD)

grub-gdb: kernel initrd tools/$(FD).bz2 $(HD) .gdbinit
	@bzcat tools/$(FD).bz2 > $(FD)
	tools/update_kernel.sh
	qemu -m 32 -fda $(FD) -hdb $(HD) $(GDB_ARG)

chunix.img: bootsect kernel tools/blank_hd.img.bz2
	@cat boot/bootsect kernel > tmp.img
	@rm -f chunix.img
	@bzcat tools/blank_hd.img.bz2 > chunix.img
	@dd if=tmp.img of=chunix.img conv=notrunc
	@rm -f tmp.img

kernel: buildall kern/kernel.ld
	$(LD) $(LDFLAGS) -T kern/kernel.ld -o $@ kern/entry.o $(KERNOBJ) $(FSOBJ) $(DRVOBJ) -b binary $(USEROBJ)
	${OBJDUMP} -d $@ > $@.asm
	${NM} -n $@ > $@.sym

initrd: tools/initrd.bz2
	@bzcat $^ > $@
	tools/mk_initrd.sh

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
	@rm -f mk_sfs initrd chunix.img #$(HD)

cleanall: clean
	@rm -f $(HD) *.img bochs.log *.asm *.sym
