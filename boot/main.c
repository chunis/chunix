//
// This file is part of ChuniX and is released under the terms of
// GNU GPLv3 License, see License for details.
//
// Copyright (C) 2014, Chunis Deng (<chunchengfh@gmail.com>)
//
#include <elf.h>
#include <x86.h>

#define SECTSIZE	512
#define ELFHDR		((struct elf *)0x10000)	// arbitrary space

void readseg(uint8_t *va, uint32_t count, uint32_t offset);

void bootmain(void)
{
	struct proghdr *ph, *eph;
	uint8_t *pa;

	readseg((uint8_t *)ELFHDR, SECTSIZE*8, 0);
	if(ELFHDR->e_magic != ELF_MAGIC)
		goto bad;

	// load each program segment
	ph = (struct proghdr *)((uint8_t *)ELFHDR + ELFHDR->e_phoff);
	eph = ph + ELFHDR->e_phnum;
	while(ph < eph){
		pa = (uint8_t *)ph->ph_pa;
		readseg(pa, ph->ph_filesize, ph->ph_offset);
		if(ph->ph_memsize > ph->ph_filesize)
			stosb(pa + ph->ph_filesize, 0,
				ph->ph_memsize - ph->ph_filesize);

		ph++;
	}

	// call the entry point
	((void (*)(void))ELFHDR->e_entry)();

bad:
	while (1)
		;	// do nothing
}

void waitdisk(void)
{
	while((inb(0x1F7) & 0xC0) != 0x40)
		;	// do nothing
}

void readsect(void *dst, uint32_t offset)
{
	waitdisk();
	outb(0x1F2, 1);		// count = 1
	outb(0x1F3, offset);
	outb(0x1F4, offset >> 8);
	outb(0x1F5, offset >> 16);
	outb(0x1F6, (offset >> 24) | 0xE0);
	outb(0x1F7, 0x20);	// cmd 0x20 = read sectors

	waitdisk();
	insl(0x1F0, dst, SECTSIZE/4);
}

// read 'count' bytes from 'offset' into virtual address 'va'.
// might copy more than asked
void readseg(uint8_t *va, uint32_t count, uint32_t offset)
{
	uint8_t* end_va;
	end_va = va + count;

	// Round down to sector boundary
	va -= offset % SECTSIZE;

	// offset translates to sectors, and kernel starts at sector 1
	offset = (offset / SECTSIZE) + 1;

	while(va < end_va){
		readsect((uint8_t *)va, offset);
		va += SECTSIZE;
		offset++;
	}
}
