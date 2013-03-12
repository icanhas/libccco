CFLAGS?=-std=c99

hostos!=uname | tr '[:upper:]' '[:lower:]' | sed 's/_.*//; s;/;_;g'
sub=s/i.86/i386/; s/x86_64/amd64/; s/x86-64/amd64/; s/x64/amd64/
hostarch!=uname -m | sed '${sub}'

OS?=${hostos}
ARCH?=${hostarch}

.if ${OS} == mingw32
  OS=windows
.elif ${OS} == Windows_NT
  OS=windows
.endif

.if ${OS} == windows
  E=.exe
  .if ${hostos} == mingw32
    .if ${CC} == cc
      CC=gcc
    .endif
  .endif
.endif

.include "Makefile.port"

