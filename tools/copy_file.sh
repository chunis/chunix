#!/bin/sh

# copy files to minixfs.img
# Note: only root can do mount and umount

tmpdir=tmpdir$$
files="README"

[ `id -u` -ne 0 ] && echo "Only root can run this script!" && exit

mkdir -p $tmpdir
mount -t minix minixfs.img $tmpdir
cp $files $tmpdir
umount $tmpdir
rmdir $tmpdir
