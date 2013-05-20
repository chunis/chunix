#!/bin/sh

sudo mount -o loop initrd /mnt
sudo cp -r initrd_dir/* /mnt/
sudo cp README /mnt
sudo umount /mnt
