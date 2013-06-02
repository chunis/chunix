#include <fs.h>
#include <printf.h>
#include <const.h>
#include <errno.h>

struct super_block super_blocks[NR_SUPER];
static struct fs_node *file_systems = NULL;

int register_filesystem(struct fs_node *fs)
{
	struct fs_node **tmp;

	printk("register filesystem: %s\n", fs->name);

	if(!fs)
		return -EINVAL;
	if(fs->next)
		return -EBUSY;

	tmp = &file_systems;
	while(*tmp){
		if(strcmp((*tmp)->name, fs->name) == 0)
			return -EBUSY;
		tmp = &(*tmp)->next;
	}
	*tmp = fs;

	return 0;
}

int unregister_filesystem(struct fs_node *fs)
{
	struct fs_node **tmp;

	printk("unregister filesystem: %s\n", fs->name);

	tmp = &file_systems;
	while(*tmp){
		if(fs == *tmp){
			*tmp = (*tmp)->next;
			fs->next = NULL;
			return 0;
		}
		tmp = &(*tmp)->next;
	}

	return -EINVAL;
}

void mount_root(void)
{
}

void list_fs(void)
{
	struct fs_node *tmp = file_systems;

	printk("Registered file systems: ");
	while(tmp){
		printk("%s ", tmp->name);
		tmp = tmp->next;
	}
	printk("\n");
}

void init_fs(void)
{
	add_ext2_fs();
	add_initrd_fs();

	list_fs();
	mount_root();
}
