AS = as
CC = gcc
LD = ld

ASFLAGS = -32
CFLAGS = -nostdinc -I../inc -fno-builtin -m32
LDFLAGS = -melf_i386

OBJCOPY = objcopy
OBJDUMP = objdump
