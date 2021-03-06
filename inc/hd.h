//
// This file is part of ChuniX and is released under the terms of
// GNU GPLv3 License, see License for details.
//
// Copyright (C) 2014, Chunis Deng (<chunchengfh@gmail.com>)
//
#ifndef __HD_H__
#define __HD_H__

#include <types.h>
#include <fs_minix.h>

void init_hd(void);
void hd_rw(struct buf *bp);

#define SECT_SIZE  512

#define HD_READ   0x20
#define HD_WRITE  0x30

// Hard Disk Registers: command
#define HDR_DATA     0x1f0
#define HDR_ERR      0x1f1
#define HDR_NSECT    0x1f2
#define HDR_LBA_LOW  0x1f3
#define HDR_LBA_MID  0x1f4
#define HDR_LBA_HI   0x1f5
#define HDR_DEVICE   0x1f6
#define HDR_STATUS   0x1f7

// Hard Disk Registers: control
#define HDR_ALTER_STATUS 0x3f6

#endif
