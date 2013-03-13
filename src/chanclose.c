#include <stdlib.h>
#include <string.h>
#include "csp.h"
#include "dat.h"
#include "fn.h"

void
chanclose(Chan *c)
{
	if(c == nil)
		return;
	if(c->status != Open){
		errorf("chanclose -- called on closed channel\n");
		return;
	}
	lock(c->l, 1);
	c->status = Closed;
	freecond(c->da);
	freecond(c->sa);
	if(c->b != nil)
		free(c->b);
	unlock(c->l);
	freelock(c->l);
}
