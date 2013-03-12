CFLAGS?=-std=c99

hostos=${shell uname | tr '[:upper:]' '[:lower:]' | sed 's/_.*//; s;/;_;g'}
sub=s/i.86/i386/; s/x86_64/amd64/; s/x86-64/amd64/; s/x64/amd64/
hostarch=${shell uname -m | sed '${sub}'}

-include Makeconfig

OS?=${hostos}
ARCH?=${hostarch}

ifeq (${OS},mingw32)
  OS=windows
endif
ifeq (${OS},Windows_NT)
  OS=windows
endif

ifeq (${OS},windows)
  E=.exe
  ifeq (${hostos},mingw32)
    ifeq (${CC},cc)
      CC=gcc
    endif
  endif
endif

include Makefile.port
