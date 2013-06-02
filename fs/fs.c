#include <fs.h>
#include <printf.h>
#include <const.h>
#include <errno.h>
#include <types.h>
#include <string.h>

struct super_block super_blocks[NR_SUPER];
static struct fs_node *file_systems = NULL;
dev_t ROOT_DEV = 0x0305;  // scratch a dummy number temporarily
extern enum fs_type root_fs_type;

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

struct fs_node *get_fs_node(const char *name)
{
	struct fs_node *fs = file_systems;

	if (!name)
		return fs;

	while(fs){
		if(strncmp(name, fs->name, 4) == 0)
			break;
		fs = fs->next;
	}
	return fs;
}

static struct super_block *read_super(dev_t dev, char *name, int flags)
{
	struct super_block *s = super_blocks;
	struct fs_node *fs_nodp;

	if (!dev)
		return NULL;

	fs_nodp = get_fs_node(name);
	if(!fs_nodp){
		printk("VFS: get_fs_node(%s) failed\n", name);
		return NULL;
	}

	while(s < super_blocks + NR_SUPER){
		if (!s->s_dev)
			break;
		s++;
	}
	if (s >= super_blocks + NR_SUPER)
		return NULL;

	// find a free slot
	s->s_dev = dev;
	s->s_flags = flags;
	if (!fs_nodp->read_super(s)) {
		s->s_dev = 0;
		return NULL;
	}

	return s;
}

void mount_root(void)
{
	struct fs_node *fs_nodp;
	struct super_block *sb;
	struct inode *inode, d_inode;
	struct file filp;
	int flag = 0;

	memset(super_blocks, 0, sizeof(super_blocks));

	for(fs_nodp = file_systems; fs_nodp; fs_nodp = fs_nodp->next) {
		if (!fs_nodp->req_dev)
			continue;

		sb = read_super(ROOT_DEV, fs_nodp->name, flag);
		if(sb){
			inode = sb->s_mounted;
			sb->s_flags = flag;
			printk("VFS: mounted root(%s filesystem)\n", fs_nodp->name);
			return;
		}
	}

	printk("VFS: unable to mount root fs on %x:%x",
			MAJOR(ROOT_DEV), MINOR(ROOT_DEV));
	panic("VFS: unable to mount root\n");

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
	printk("init fs done.\n");
}
