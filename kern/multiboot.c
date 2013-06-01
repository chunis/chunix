#include <multiboot.h>
#include <printf.h>

void dump_multiboot(struct multiboot_info *mb_ptr)
{
	printf("Multiboot Header at %x:\n", (uint32_t)mb_ptr);

	printf("Flags  : %x\t\t",  mb_ptr->flags);
	printf("Mem Low: %x\t",  mb_ptr->mem_lower);
	printf("Mem High: %x\n",  mb_ptr->mem_upper);
	printf("Boot dev: %x\t", mb_ptr->boot_device);
	printf("cmdline: %x\t",  mb_ptr->cmdline);
	printf("Mods cnt: %x\n",  mb_ptr->mods_count);
	printf("Mod Addr: %x\t",  mb_ptr->mods_addr);
	printf("Syms num: %x\t", mb_ptr->num);
	printf("Sym size: %x\n",  mb_ptr->size);
	printf("Sym addr: %x\t",  mb_ptr->addr);
	printf("Sym shdx: %x\t",  mb_ptr->shndx);
	printf("mmap len: %x\n", mb_ptr->mmap_length);
	printf("mmap Ad: %x\t",  mb_ptr->mmap_addr);
	printf("Drv leng: %x\t",  mb_ptr->drives_length);
	printf("Drv Addr: %x\n",  mb_ptr->drives_addr);
	printf("Config table: %x\t", mb_ptr->config_table);
	printf("Loader: %x\t",  mb_ptr->boot_loader_name);
	printf("APM tbl:  %x\n",  mb_ptr->apm_table);
	printf("VBE ctrl: %x\t",  mb_ptr->vbe_ctrl_info);
	printf("VBE info: %x\t", mb_ptr->vbe_mode_info);
	printf("VBE mode: %x\n",  mb_ptr->vbe_mode);
	printf("VBE seg: %x\t",  mb_ptr->vbe_iface_seg);
	printf("VBE off: %x\t",  mb_ptr->vbe_iface_off);
	printf("VBE len: %x\n", mb_ptr->vbe_iface_len);

	printf("\n");
	printf("Started with: %s\n", (char *)mb_ptr->cmdline);
	printf("Booted from: %s\n", (char *)mb_ptr->boot_loader_name);
	printf("%dkB lower memory\n", mb_ptr->mem_lower);
	printf("%dkB higher memory ", mb_ptr->mem_upper);

	int mem_mb = mb_ptr->mem_upper / 1024;
	printf("(%dMB)\n", mem_mb);
	printf("Found %d module(s).\n", mb_ptr->mods_count);
	if (mb_ptr->mods_count > 0) {
		int i;
		for (i = 0; i < mb_ptr->mods_count; ++i ) {
			uint32_t module_start = *((uint32_t*)mb_ptr->mods_addr + 8 * i);
			uint32_t module_end   = *(uint32_t*)(mb_ptr->mods_addr + 8 * i + 4);
			printf("Module %d is at %x:%x\n", i+1, module_start, module_end);
		}
	}
}

