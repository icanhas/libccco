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
	char *buf;
	
	c = malloc(sizeof *c);
	if(c == nil){
		errorf("chancreate -- out of memory\n");
		return nil;
	}
	l = createlock();
	if(l == nil){
		errorf("chancreate -- failed to alloc lock\n");
		goto Freel;
	}
	full = createcond();
	if(full == nil){
		errorf("chancreate -- createcond failed\n");
		goto Freec;
	}
	flushed = createcond();
	if(flushed == nil){
		errorf("chancreate -- createcond failed\n");
		goto Freefull;
	}
	if(nel > 0)
		buf = calloc(nel, elsz);
	else
		buf = calloc(1, elsz);
	if(buf == nil){
		errorf("chancreate -- failed to alloc channel buffer\n");
		goto Freeflushed;
	}
	c->l = l;
	c->full = full;
	c->flushed = flushed;
	c->b = buf;
	c->elsz = elsz;
	c->sz = nel;
	c->n = 0;
	c->s = 0;
	return c;

Freeflushed:
	free(flushed);
Freefull:
	free(full);
Freel:
	free(l);
Freec:
	free(c);
	return nil;
}
