#include <const.h>
#include <fs.h>

struct fs_node initrd_ext2_fs = {
	NULL,
	"initrd",
	0,
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
