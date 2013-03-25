#include <stdlib.h>
#include "ccco.h"
#include "dat.h"
#include "fn.h"

void
procfree(Proc *p)
{
	if(p == nil)
		return;
	prockill(p);
	free(p);
	p = nil;
}
