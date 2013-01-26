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
	rm -f ${ofiles}
	rm -f libcsp.a

dist: clean
	tar cf tmp.tar Makefile include src
	gzip <tmp.tar >libcsp-${ver}.tar.gz
	bzip2 <tmp.tar >libcsp-${ver}.tar.bz2
	rm tmp.tar

