include ../make.inc

all: ${FS}

%.o: %.c
	${CC} ${CFLAGS} -c -o $@ $<

clean:
	@rm -f ${FS}
