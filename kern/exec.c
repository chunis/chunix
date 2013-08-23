#include <types.h>
#include <mmu.h>
#include <elf.h>
#include <printf.h>
//#include <x86.h>
//#include <mmu.h>
//#include <kbd.h>
//#include <console.h>
//#include <multiboot.h>
//#include <string.h>
//#include <fs.h>

int exec(char *path, char **argv)
{
	pde_t *pgdir, oldpgdir;
	struct elf elfhdr;
	int elfsz = sizeof(struct elf);

	printk("------> Enter exec() now <------\n");
	// 1. read the elf binary from disk, and check elf reader
	if(sfs_read_file(path, (char *)&elfhdr, elfsz) < elfsz)
		goto fail;
	if(elfhdr.e_magic != ELF_MAGIC)
		goto fail;
	if((pgdir = mapkvm()) == NULL)
		goto fail;

	// 2. load program into memory
	//

	// 3. setup user stack
	//

	// 4. setup arguments in stack
	//

	// 5. other things
	//

	return 0;

fail:
	//if(pgdir)
	//	free_vm(pgdir);
	return -1;
}

