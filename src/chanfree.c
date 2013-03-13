#include <stdlib.h>
#include "csp.h"
#include "dat.h"
#include "fn.h"

void
chanfree(Chan *c)
{
	if(c == nil)
		return;
	chanclose(c);
	free(c);
	c = nil;
}
