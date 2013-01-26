-include Makeconfig

ifeq ($(host),mingw32)
  E=.exe
else
  platlibs=-lpthread
endif

ifeq ($(FORCE_PTHREAD),1)
  platlibs=-lpthreads
endif

include Makefile.port
