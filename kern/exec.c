#include <types.h>
#include <mmu.h>
#include <elf.h>
#include <printf.h>
#include <fs_sfs.h>
//#include <console.h>
//#include <string.h>
//#include <fs.h>
#include "task.h"
#include <x86.h>


extern struct task *current;

int exec(char *path, char **argv)
{
	pde_t *pgdir, *oldpgdir;
	struct sfs_stat state;
	struct proghdr *ph, *eph;
	struct elf elfhdr;
	int elfsz = sizeof(struct elf);
	uint32_t offset, range = 0;
	uint32_t argc = 0, sp, ustack[2+MAXARGS+1];
	int ret;
	char *fp;

	// test passed arguments
	char **p = argv;

	printk(">>>\npath = %s\n", path);
	while(*p){
		argc++;
		printk("arg%d: %s\n", argc, *p++);
	}
	printk("argc = %d\n<<<\n", argc);

	// read the elf binary from disk, and check elf reader
	ret = sfs_read_file(path, (char *)&elfhdr, elfsz);
	printk("exec: ret = %d\n", ret);
	if(ret < elfsz){
		printk("exec: read file fail\n");
		goto fail;
	}
	if(elfhdr.e_magic != ELF_MAGIC){
		printk("exec: read file fail\n");
		goto fail;
	}
	if((pgdir = mapkvm()) == NULL){
		printk("exec: mapkvm() fail\n");
		goto fail;
	}

	// get binary file size
	sfs_stat(path, &state);
	printk("%s: size = %d\n", path, state.st_size);
	fp = kmalloc(state.st_size);
	if(!fp)
		panic("kmalloc");

	// read all binary from disk to buf 'fp'
	ret = sfs_read_file(path, fp, state.st_size);
	if(ret < state.st_size)
		panic("sfs_read_file");

	ph = (struct proghdr *)(fp + elfhdr.e_phoff);
	eph = ph + elfhdr.e_phnum;
	for(; ph < eph; ph++){
		if(ph->ph_type != ELF_PROG_LOAD)
			continue;

		region_alloc(current->pgdir, (void *)ph->ph_va, ph->ph_memsize, (PTE_W | PTE_U));
		memmove((void *)ph->ph_va, (void *)(fp+ph->ph_offset), ph->ph_filesize);
		memset((void *)(ph->ph_va + ph->ph_filesize), 0, ph->ph_memsize - ph->ph_filesize);
		copy_page(pgdir, (uint32_t)ph->ph_va);
		offset = ph->ph_va + ph->ph_memsize;
		range = (range >= offset ? range : offset);
	}
	kfree(fp);

	// map one page for the program's initial stack
	region_alloc(current->pgdir, (void *)USTACKTOP - PGSIZE, PGSIZE, (PTE_W | PTE_U));

	// setup arguments in stack
	sp = USTACKTOP;
	for(argc = 0; argv[argc]; argc++) {
		if(argc >= MAXARGS)
			goto fail;
		sp = (sp - (strlen(argv[argc]) + 1)) & ~3;
		if(copyout(current->pgdir, sp, argv[argc], strlen(argv[argc]) + 1) < 0)
			goto fail;
		ustack[2+argc] = sp;
	}
	ustack[2+argc] = 0;
	ustack[0] = argc;
	ustack[1] = sp - (argc+1)*4;  // argv pointer

	sp -= (2+argc+1) * 4;
	if(copyout(current->pgdir, sp, ustack, (2+argc+1)*4) < 0)
		goto fail;

	// copy stack and heap
	// TODO: check how many pages need to be copied
	copy_page(pgdir, (uint32_t)(USTACKTOP - PGSIZE));
	copy_page(pgdir, (uint32_t)HEAP_START);

	oldpgdir = current->pgdir;
	current->pgdir = pgdir;
	free_vm(oldpgdir);

	current->tf->eip = elfhdr.e_entry;
	current->tf->esp = sp;
	current->sz = range;

	lcr3((uint32_t *)P2V((uint32_t)current->pgdir));

	dprintk("---+--+--- exec() end\n");
	return 0;

fail:
	printk("Error! exec() failed\n");
	if(pgdir)
		free_vm(pgdir);
	return -1;
}
