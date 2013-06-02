#include <const.h>
#include <fs.h>


extern enum fs_type root_fs_type;

static struct super_block *initrd_read_super(struct super_block *sb)
{
	printk("In initrd_read_super\n");

	if(root_fs_type != INITRD)
		return NULL;

	printk("Enter initrd_read_super now\n");
	return sb;
}

struct fs_node initrd_ext2_fs = {
	initrd_read_super,
	"initrd",
	1,	// set to 1 temporarily
	NULL,
};

void add_initrd_fs(void)
{
	register_filesystem(&initrd_ext2_fs);
}

void del_initrd_fs(void)
{
	unregister_filesystem(&initrd_ext2_fs);
}
