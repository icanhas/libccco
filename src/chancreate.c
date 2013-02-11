#include <stddef.h>
#include <stdlib.h>
#include "csp.h"
#include "dat.h"
#include "fn.h"

Chan*
chancreate(int nel, int elsz)
{
	Chan *c;
	Lock *l;
	Cond *full, *flushed;
	
	c = malloc(sizeof *c);
	if(c == nil){
		errorf("chancreate -- out of memory\n");
		return nil;
	}
	l = createlock();
	if(l == nil){
		errorf("chancreate -- failed to alloc lock\n");
		free(c);
		return nil;
	}
	full = createcond();
	if(full == nil){
		errorf("chancreate -- createcond failed\n");
		free(c);
		free(l);
		return nil;
	}
	flushed = createcond();
	if(flushed == nil){
		errorf("chancreate -- createcond failed\n");
		free(c);
		free(l);
		free(full);
		return nil;
	}
	c->l = l;
	c->full = full;
	c->flushed = flushed;
	c->elsz = elsz;
	c->sz = nel;
	c->n = 0;
	c->s = 0;
	c->b = malloc(nel * elsz);
	return c;
}
