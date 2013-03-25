ofiles=\
  man3/libccco.ps \
  man3/libccco.pdf \
  man3/libccco.html

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
	cp man3/libccco.3 ${PREFIX}/man/man3/libccco.3
	chmod 0644 ${PREFIX}/man/man3/libccco.3

nuke:
	rm -f ${PREFIX}/man/man3/libcsp.3
