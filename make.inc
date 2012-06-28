AS = as
CC = gcc
LD = ld
NM = nm
AR = ar
OBJCOPY = objcopy
OBJDUMP = objdump

ASFLAGS = -32
LDFLAGS = -melf_i386 -nostdlib
CFLAGS += -Wall -Wno-format -Wno-unused -m32 -g -fno-strict-aliasing
CFLAGS += -fno-omit-frame-pointer -fno-stack-protector -fno-builtin
CFLAGS += -nostdinc -I../inc
#CFLAGS += -O1 -MD -gstabs -Werror
