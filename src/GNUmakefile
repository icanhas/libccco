ifeq (${OS},windows)
  platofiles=Windows.$O
else
  platofiles=Pthread.$O
endif

ifeq (${FORCE_PTHREAD},1)
  platofiles=Pthread.$O
endif
ifeq (${FORCE_C11},1)
  platofiles=C11.$O
endif

include Makefile.port
