ifeq ($(host),mingw32)
  E=.exe
  platofiles=Windows.$O
else
  platofiles=Pthread.$O
endif

-include Makeconfig

ifeq ($(FORCE_PTHREAD),1)
  platofiles=Pthread.$O
endif
ifeq ($(FORCE_C11),1)
  platofiles=C11.$O
endif

include Makefile.port
