include ../make.inc

all: ${DRV}

%.o: %.c
	${CC} ${CFLAGS} -c -o $@ $<

clean:
	@rm -f ${DRV}
