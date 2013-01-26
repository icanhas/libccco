CC=gcc
inc=-Iinclude
ver=0.0.1

-include Makeconfig

ofiles= \
  src/Pthread.o \
  src/proccreate.o \
  src/prockill.o

all: libcsp.a
	
.c.o:
	${CC} ${CFLAGS} ${inc} -c -o $@ $<

libcsp.a: ${ofiles}
	${AR} rsc $@ $^
	
clean:
	rm ${ofiles}
	rm libcsp.a

dist: clean
	tar cf ./ tmp.tar
	<tmp.tar gzip >libcsp.tar-${ver}.tgz
	<tmp.tar bzip2 >libcsp.tar-${ver}.tbz2
	rm tmp.tar

