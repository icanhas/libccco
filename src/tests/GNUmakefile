ifeq (${OS},windows)
  platlibs=-lkernel32
else
  platlibs=-lpthread
endif

ifeq (${FORCE_PTHREAD},1)
  platlibs=-lpthread
endif

include Makefile.port
