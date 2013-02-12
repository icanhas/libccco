#include <stdlib.h>
#include "csp.h"
#include "dat.h"
#include "fn.h"

void
chanfree(Chan *c)
{
	if(c == nil)
		return;
	lock(c->l, 1);
	destroycond(c->avail);
	if(c->b != nil)
		free(c->b);
	unlock(c->l);
	freelock(c->l);
	free(c);
}
