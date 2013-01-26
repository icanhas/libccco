CC=gcc
O=o
A=a
inc=-Iinclude
name=libhoares
libname=lib/${name}.$A
ver=0.0.1

-include Makeconfig

ofiles= \
  src/Pthread.$O \
  src/proccreate.$O \
  src/procinit.$O \
  src/prockill.$O \
  src/chancreate.$O \
  src/util.$O

all: ${libname}
	
.c.$O:
	${CC} ${CFLAGS} ${inc} -c -o $@ $<

${libname}: ${ofiles}
	mkdir -p lib
	${AR} rsc $@ $^
	
clean:
	rm -f ${ofiles}
	rm -f ${libname}

dist: clean
	tar cf tmp.tar Makefile include src
	gzip <tmp.tar >${name}-${ver}.tar.gz
	bzip2 <tmp.tar >${name}-${ver}.tar.bz2
	rm tmp.tar
