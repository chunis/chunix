//
// This file is part of ChuniX and is released under the terms of
// GNU GPLv3 License, see License for details.
//
// Copyright (C) 2014, Chunis Deng (<chunchengfh@gmail.com>)
//
# assembler macros to create x86 segments

#define STA_R	0x2		// Readable (executable segments)
#define STA_W	0x2		// Writable (non-executable segments)
#define STA_X	0x8		// Executable segments

#define SEG_NULL		\
	.word 0, 0;			\
	.byte 0, 0, 0, 0

#define SEG_CODE			\
	.word 0xffff, 0;		\
	.byte 0, (0x90 | (STA_X | STA_R)), (0xC0 | 0xF), 0

#define SEG_DATA			\
	.word 0xffff, 0;		\
	.byte 0, (0x90 | STA_W), (0xC0 | 0xF), 0
