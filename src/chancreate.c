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
	Cond *avail;
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
	avail = createcond();
	if(avail == nil){
		errorf("chancreate -- createcond failed\n");
		goto Freec;
	}
	if(nel > 0)
		buf = calloc(nel, elsz);
	else
		buf = calloc(1, elsz);
	if(buf == nil){
		errorf("chancreate -- failed to alloc channel buffer\n");
		goto Freeavail;
	}
	c->l = l;
	c->avail = avail;
	c->b = buf;
	c->elsz = elsz;
	c->sz = nel;
	c->n = 0;
	c->s = 0;
	return c;

Freeavail:
	free(avail);
Freel:
	free(l);
Freec:
	free(c);
	return nil;
}
