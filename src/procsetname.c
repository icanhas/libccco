#include <stdarg.h>
#include <stdio.h>
#include "csp.h"
#include "dat.h"
#include "fn.h"

void
procsetname(const char *fmt, ...)
{
	va_list v;
	char buf[1024] = {0};
	
	va_start(v, fmt);
	vsnprintf(buf, sizeof buf, fmt, v);
	va_end(v);
	buf[(sizeof buf)-1] = '\0';
	setthreadname(buf);
}
