ofiles=\
  man3/libcsp.ps \
  man3/libcsp.pdf \
  man3/libcsp.html

all: ${ofiles}

.3.ps:
	<$< preconv | troff -mdoc | grops >$@

.3.pdf:
	<$< preconv | troff -mdoc | grops | ps2pdf >$@

.3.html:
	<$< preconv | troff -mdoc | grops | ps2html >$@

clean:
	rm -f ${ofiles}

install:
	@mkdir -p ${PREFIX}/man/man3/
	cp man3/libcsp.3 ${PREFIX}/man/man3/libcsp.3
	chmod 0644 ${PREFIX}/man/man3/libcsp.3

nuke:
	rm -f ${PREFIX}/man/man3/libcsp.3
