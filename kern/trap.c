#include "printf.h"
#include "task.h"

void trap(STACK_FRAME *tf)
{
	printf("tf->trapno: %d\n", tf->trapno);

	return;
}
