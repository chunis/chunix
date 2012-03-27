
all: chunix.img hd_qemu.img hd_bochs.img

qemu: chunix.img hd_qemu.img
	qemu -serial mon:stdio -hda chunix.img -hdb hd_qemu.img

bochs: chunix.img hd_bochs.img
	bochs -f hd.bxrc

hd_qemu.img: tools/hd_qemu.img.bz2
	@bzcat tools/hd_qemu.img.bz2 > hd_qemu.img

hd_bochs.img: tools/hd_bochs.img.bz2
	@bzcat tools/hd_bochs.img.bz2 > hd_bochs.img

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
	@rm -f chunix.img hd.img hd_bochs.img hd_qemu.img
