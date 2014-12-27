//
// This file is part of ChuniX and is released under the terms of
// GNU GPLv3 License, see License for details.
//
// Copyright (C) 2014, Chunis Deng (<chunchengfh@gmail.com>)
//
#ifndef __MULTIBOOT_H__
#define __MULTIBOOT_H__


#include <types.h>

#define MBT_HEADER_MAGIC          0x1BADB002

// This should be in %eax
#define MBT_BOOTLOADER_MAGIC      0x2BADB002

// Flags for multiboot header 'flags'
#define MBT_PAGE_ALIGN            0x00000001
#define MBT_MEMORY_INFO           0x00000002
#define MBT_VIDEO_MODE            0x00000004
#define MBT_AOUT_KLUDGE           0x00010000

// Flags for multiboot info structure 'flags'
#define MBT_INFO_MEMORY           0x00000001
#define MBT_INFO_BOOTDEV          0x00000002
#define MBT_INFO_CMDLINE          0x00000004
#define MBT_INFO_MODS             0x00000008
#define MBT_INFO_AOUT_SYMS        0x00000010
#define MBT_INFO_ELF_SHDR         0X00000020
#define MBT_INFO_MEM_MAP          0x00000040
#define MBT_INFO_DRIVE_INFO       0x00000080
#define MBT_INFO_CONFIG_TABLE     0x00000100
#define MBT_INFO_BOOT_LOADER      0x00000200
#define MBT_INFO_APM_TABLE        0x00000400
#define MBT_INFO_VIDEO_INFO       0x00000800


struct multiboot_header
{
	uint32_t magic;
	uint32_t flags;
	uint32_t checksum;

	// only for MBT_AOUT_KLUDGE
	uint32_t header_addr;
	uint32_t load_addr;
	uint32_t load_end_addr;
	uint32_t bss_end_addr;
	uint32_t entry_addr;

	// only for MBT_VIDEO_MODE
	uint32_t mode_type;
	uint32_t width;
	uint32_t height;
	uint32_t depth;
};

// The symbol table for a.out
struct mb_aout_symbol_table
{
	uint32_t tabsize;
	uint32_t strsize;
	uint32_t addr;
	uint32_t reserved;
};

// The section header table for ELF
struct mb_elf_sh_table
{
	uint32_t num;
	uint32_t size;
	uint32_t addr;
	uint32_t shndx;
};

struct multiboot_info
{
	uint32_t flags;
	uint32_t mem_lower;
	uint32_t mem_upper;
	uint32_t boot_device;
	uint32_t cmdline;
	uint32_t mods_count;
	uint32_t mods_addr;

	// for section header table
	uint32_t num;
	uint32_t size;
	uint32_t addr;
	uint32_t shndx;

	uint32_t mmap_length;
	uint32_t mmap_addr;
	uint32_t drives_length;
	uint32_t drives_addr;
	uint32_t config_table;
	uint32_t boot_loader_name;
	uint32_t apm_table;

	// Video
	uint32_t vbe_ctrl_info;
	uint32_t vbe_mode_info;
	uint16_t vbe_mode;
	uint16_t vbe_iface_seg;
	uint16_t vbe_iface_off;
	uint16_t vbe_iface_len;
};

struct multiboot_mod_list
{
	uint32_t mod_start;
	uint32_t mod_end;
	uint32_t cmdline;
	uint32_t pad;  // must be zero
};

void dump_multiboot(struct multiboot_info *mb_ptr);

#endif
