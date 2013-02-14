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
	Cond *da, *sa;
	uchar *buf;
	
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
	da = createcond();
	if(da == nil){
		errorf("chancreate -- createcond failed\n");
		goto Freec;
	}
	sa = createcond();
	if(sa == nil){
		errorf("chancreate -- createcond failed\n");
		goto Freeda;
	}
	if(nel > 0)
		buf = calloc(nel, elsz);
	else
		buf = calloc(1, elsz);
	if(buf == nil){
		errorf("chancreate -- failed to alloc channel buffer\n");
		goto Freesa;
	}
	c->l = l;
	c->da = da;
	c->sa = sa;
	c->b = buf;
	c->elsz = elsz;
	c->sz = nel;
	c->n = 0;
	c->s = 0;
	return c;
Freesa:
	free(sa);
Freeda:
	free(da);
Freel:
	free(l);
Freec:
	free(c);
	return nil;
}
