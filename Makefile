
all: chunix.img hd.img

qemu: chunix.img hd.img
	qemu -m 32 -hda chunix.img -hdb hd.img

qemu-gdb: chunix.img hd.img .gdbinit
	qemu -m 32 -hda chunix.img -hdb hd.img -S -gdb tcp::1234

bochs: chunix.img hd.img
	bochs -f hd.bxrc

hd.img: tools/hd.img.bz2
	@bzcat tools/hd.img.bz2 > hd.img

chunix.img: bootsect kernel tools/blank_hd.img.bz2 user/hello user/todo
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

user/hello:
	@echo "build user apps..."
	(cd user && make)

user/todo:
	@echo "build user apps..."
	(cd user && make)

clean:
	(cd tools && make clean)
	(cd boot && make clean)
	(cd kern && make clean)
	(cd lib && make clean)
	(cd user && make clean)
	@rm -f chunix.img hd.img
