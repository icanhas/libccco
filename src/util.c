#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include "csp.h"
#include "dat.h"
#include "fn.h"

void
errorf(const char *fmt, ...)
{
	va_list v;
	
	va_start(v, fmt);
	vfprintf(stderr, fmt, v);
	va_end(v);
}

void*
emalloc(size_t sz)
{
	void *p;
	
	if((p = malloc(sz)) == nil){
		errorf("emalloc -- out of memory\n");
		exit(1);
	}
	return p;
}
