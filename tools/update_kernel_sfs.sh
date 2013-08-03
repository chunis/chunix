#!/bin/sh

SFS_INIT=sfs-initrd

sudo mount -o loop floppy.img /mnt
sudo cp tools/grub.conf /mnt/BOOT/GRUB/MENU.LST
sudo cp kernel /mnt/kernel
./mk_sfs_initrd $SFS_INIT $*
sudo cp $SFS_INIT /mnt/initrd
sudo umount /mnt
rm -f $SFS_INIT
