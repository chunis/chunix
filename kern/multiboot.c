//
// This file is part of ChuniX and is released under the terms of
// GNU GPLv3 License, see License for details.
//
// Copyright (C) 2014, Chunis Deng (<chunchengfh@gmail.com>)
//
#include <multiboot.h>
#include <printf.h>

void dump_multiboot(struct multiboot_info *mb_ptr)
{
	printk("Multiboot Header at %x:\n", (uint32_t)mb_ptr);

	printk("Flags  : %x\t\t",  mb_ptr->flags);
	printk("Mem Low: %x\t",  mb_ptr->mem_lower);
	printk("Mem High: %x\n",  mb_ptr->mem_upper);
	printk("Boot dev: %x\t", mb_ptr->boot_device);
	printk("cmdline: %x\t",  mb_ptr->cmdline);
	printk("Mods cnt: %x\n",  mb_ptr->mods_count);
	printk("Mod Addr: %x\t",  mb_ptr->mods_addr);
	printk("Syms num: %x\t", mb_ptr->num);
	printk("Sym size: %x\n",  mb_ptr->size);
	printk("Sym addr: %x\t",  mb_ptr->addr);
	printk("Sym shdx: %x\t",  mb_ptr->shndx);
	printk("mmap len: %x\n", mb_ptr->mmap_length);
	printk("mmap Ad: %x\t",  mb_ptr->mmap_addr);
	printk("Drv leng: %x\t",  mb_ptr->drives_length);
	printk("Drv Addr: %x\n",  mb_ptr->drives_addr);
	printk("Config table: %x\t", mb_ptr->config_table);
	printk("Loader: %x\t",  mb_ptr->boot_loader_name);
	printk("APM tbl:  %x\n",  mb_ptr->apm_table);
	printk("VBE ctrl: %x\t",  mb_ptr->vbe_ctrl_info);
	printk("VBE info: %x\t", mb_ptr->vbe_mode_info);
	printk("VBE mode: %x\n",  mb_ptr->vbe_mode);
	printk("VBE seg: %x\t",  mb_ptr->vbe_iface_seg);
	printk("VBE off: %x\t",  mb_ptr->vbe_iface_off);
	printk("VBE len: %x\n", mb_ptr->vbe_iface_len);

	printk("\n");
	printk("Started with: %s\n", (char *)mb_ptr->cmdline);
	printk("Booted from: %s\n", (char *)mb_ptr->boot_loader_name);
	printk("%dkB lower memory\n", mb_ptr->mem_lower);
	printk("%dkB higher memory ", mb_ptr->mem_upper);

	int mem_mb = mb_ptr->mem_upper / 1024;
	printk("(%dMB)\n", mem_mb);
	printk("Found %d module(s).\n", mb_ptr->mods_count);
	if (mb_ptr->mods_count > 0) {
		int i;
		for (i = 0; i < mb_ptr->mods_count; ++i ) {
			uint32_t module_start = *((uint32_t*)mb_ptr->mods_addr + 8 * i);
			uint32_t module_end   = *(uint32_t*)(mb_ptr->mods_addr + 8 * i + 4);
			printk("Module %d is at %x:%x\n", i+1, module_start, module_end);
		}
	}
}

