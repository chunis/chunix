#include <fs.h>
#include <printf.h>

int register_filesystem(struct fs_node *fs)
{
	printf("register_filesystem: %s\n", fs->name);

	return 0;
}
