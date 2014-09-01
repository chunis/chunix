#include <types.h>
#include <mmu.h>
#include <elf.h>
#include <printf.h>
#include <fs_sfs.h>
//#include <console.h>
//#include <string.h>
//#include <fs.h>
#include "task.h"


extern struct task *current;

int exec(char *path, char **argv)
{
	pde_t *pgdir, oldpgdir;
	struct sfs_stat state;
	struct proghdr *ph, *eph;
	struct elf elfhdr;
	int elfsz = sizeof(struct elf);
	uint32_t offset, range = 0;
	int ret;
	char *fp;

	printk("------> Enter exec() now, path = %s\n", path);

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
		offset = ph->ph_va + ph->ph_memsize;
		range = (range >= offset ? range : offset);
	}
	kfree(fp);

	// setup tp's eip to e_entry
	current->tf->eip = elfhdr.e_entry;

	// map one page for the program's initial stack
	region_alloc(current->pgdir, (void *)USTACKTOP - PGSIZE, PGSIZE, (PTE_W | PTE_U));

	// 4. setup arguments in stack

	return 0;

fail:
	return -1;
}

