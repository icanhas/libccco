#include <stddef.h>
#include "csp.h"
#include "dat.h"
#include "fn.h"

Chan*
chancreate(size_t elsz, size_t nel)
{
	Chan *c;
	Lock *l;
	
	l = createlock();
	if(l == nil){
		errorf("chancreate -- failed to alloc lock\n");
		return nil;
	}
	c->l = l;
	c->elsz = elsz;
	c->nel = nel;
	c->b = nil;
	return c;
}
