#include <stdlib.h>
#include "csp.h"
#include "dat.h"
#include "fn.h"

void
chanclose(Chan *c)
{
	if(c == nil)
		return;
	lock(c->l, 1);
	freecond(c->da);
	freecond(c->sa);
	if(c->b != nil)
		free(c->b);
	unlock(c->l);
	freelock(c->l);
	memset(c, 0, sizeof(Chan));
}
