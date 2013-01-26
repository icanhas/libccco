CC=gcc
O=o
A=a
host=${shell uname | tr '[:upper:]' '[:lower:]' | sed 's/_.*//; s/\//_/g'}

-include Makeconfig

ifeq ($(host),mingw32)
  E=.exe
  platofiles=src/Win.$O
else
  platofiles=src/Pthread.$O
endif

ifeq ($(FORCE_PTHREAD),1)
  platofiles=src/Pthread.$O
endif

include Makefile.port
