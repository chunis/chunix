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

DRV  = console.o hd.o keyboard.o memhd.o
FS   = buffer.o sysfile.o fs.o file.o ext2/ext2.o initrd_ext2/initrd_ext2.o sfs/sfs.o
KERN = isr.o init.o string.o printf.o time.o syscall.o sched.o \
		task.o descriptor.o tools.o mm.o trap.o swtch.o \
		multiboot.o heap.o get_time.o

USER = hello todo
