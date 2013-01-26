ifeq ($(host),mingw32)
  E=.exe
  platofiles=Win.$O
else
  platofiles=Pthread.$O
endif

ifeq ($(FORCE_PTHREAD),1)
  platofiles=Pthread.$O
endif

-include Makeconfig
include Makefile.port
