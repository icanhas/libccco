#include <stddef.h>
#include <stdlib.h>
#include "csp.h"
#include "dat.h"
#include "fn.h"

void
procfree(Proc *p)
{
	if(p == nil)
		return;
	killthread(p->t);
	free(p);
}