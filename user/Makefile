include ../make.inc

all: ${USER}
	echo "Build user apps is done"

%: %.c ../lib/entry.o ../lib/libcx.a
	$(CC) $(CFLAGS) -c -o $@.o $<
	$(LD) $(LDFLAGS) -N -e _start -Ttext 0 -o $@ ../lib/entry.o $@.o -L../lib -lcx
	$(OBJDUMP) -S $@ > $@.asm
	$(NM) -n $@ > $@.sym

../lib/entry.o:
	(cd ../lib && make)

../lib/libcx.a:
	(cd ../lib && make)

clean:
	@rm -f *.o *.asm *.sym ${USER}
