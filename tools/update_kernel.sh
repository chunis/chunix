#!/bin/sh

sudo mount -o loop floppy.img /mnt
sudo cp kernel /mnt/kernel
sudo umount /mnt
