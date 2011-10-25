#ifndef __ELF_H__
#define __ELF_H__

#include <types.h>

#define ELF_MAGIC 0x464C457FU	/* '\x7FELF' in little endian */

struct elf {
	uint32_t e_magic;
	uint8_t e_elf[12];
	uint16_t e_type;
	uint16_t e_machine;
	uint32_t e_version;
	uint32_t e_entry;
	uint32_t e_phoff;
	uint32_t e_shoff;
	uint32_t e_flags;
	uint16_t e_ehsize;
	uint16_t e_phentsize;
	uint16_t e_phnum;
	uint16_t e_shentsize;
	uint16_t e_shnum;
	uint16_t e_shstrndx;
};

struct proghdr {
	uint32_t ph_type;
	uint32_t ph_offset;
	uint32_t ph_va;
	uint32_t ph_pa;
	uint32_t ph_filesize;
	uint32_t ph_memsize;
	uint32_t ph_flags;
	uint32_t ph_align;
};

struct secthdr {
	uint32_t sh_name;
	uint32_t sh_type;
	uint32_t sh_flags;
	uint32_t sh_addr;
	uint32_t sh_offset;
	uint32_t sh_size;
	uint32_t sh_link;
	uint32_t sh_info;
	uint32_t sh_addralign;
	uint32_t sh_entsize;
};

// values for proghdr->ph_type
#define ELF_PROG_LOAD		1

// flag bits for proghdr->ph_flags
#define ELF_PROG_FLAG_EXEC	1
#define ELF_PROG_FLAG_WRITE	2
#define ELF_PROG_FLAG_READ	4

// values for secthdr->sh_type
#define ELF_SHT_NULL		0
#define ELF_SHT_PROGBITS	1
#define ELF_SHT_SYMTAB		2
#define ELF_SHT_STRTAB		3

// values for secthdr->sh_name
#define ELF_SHN_UNDEF		0

#endif
