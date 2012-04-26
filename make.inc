AS = as
CC = gcc
LD = ld

ASFLAGS = -32
CFLAGS = -nostdinc -I../inc -fno-builtin -m32
#CFLAGS += $(CFLAGS) -O1 -MD
#CFLAGS += -fno-omit-frame-pointer
#CFLAGS += -Wall -Wno-format -Wno-unused -Werror -gstabs

LDFLAGS = -melf_i386

OBJCOPY = objcopy
OBJDUMP = objdump
