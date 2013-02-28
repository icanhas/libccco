#include "csp.h"
#include "dat.h"
#include "fn.h"

void*
chanrecvp(Chan *c)
{
	void *p;
	
	if(_recv(c, &p, 1) != 1)
		return nil;
	return p;
}