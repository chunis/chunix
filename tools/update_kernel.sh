#!/bin/sh

sudo mount -o loop floppy.img /mnt
sudo cp tools/grub.conf /mnt/BOOT/GRUB/MENU.LST
sudo cp kernel /mnt/kernel
sudo cp initrd /mnt/initrd
sudo umount /mnt
