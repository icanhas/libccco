CC=gcc
O=o
A=a
inc=-Iinclude
name=libhoares
ver=0.0.1

-include Makeconfig

ofiles= \
  src/Pthread.$O \
  src/proccreate.$O \
  src/procinit.$O \
  src/prockill.$O \
  src/util.$O

all: ${name}.$A
	
.c.$O:
	${CC} ${CFLAGS} ${inc} -c -o $@ $<

${name}.$A: ${ofiles}
	${AR} rsc $@ $^
	
clean:
	rm -f ${ofiles}
	rm -f ${name}.$A

dist: clean
	tar cf tmp.tar Makefile include src
	gzip <tmp.tar >${name}-${ver}.tar.gz
	bzip2 <tmp.tar >${name}-${ver}.tar.bz2
	rm tmp.tar
