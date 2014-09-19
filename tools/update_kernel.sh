#!/bin/sh

# copy kernel and initrd to floppy.img

tmpmnt=tmp_$$
mkdir -p $tmpmnt
fs=$1
shift

# prepare initrd
if [ "$fs" = "sfs" ]; then
	./mk_sfs_initrd initrd $*
else
	if [ "$fs" = "ext2" ]; then
		bzcat tools/initrd.bz2 > initrd
		sudo mount -o loop initrd $tmpmnt
	else
		bzcat tools/minixfs.img.bz2 > initrd
		mount -t minix initrd $tmpdir
	fi
	sudo cp $* $tmpmnt
	sudo umount $tmpmnt
fi

bzcat tools/floppy.img.bz2 > floppy.img
sudo mount -o loop floppy.img $tmpmnt
sudo cp tools/grub.conf $tmpmnt/BOOT/GRUB/MENU.LST
sudo cp kernel $tmpmnt/kernel
sudo mv initrd $tmpmnt/initrd
sudo umount $tmpmnt
rmdir $tmpmnt
