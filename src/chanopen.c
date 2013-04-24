#include <stdlib.h>
#include "ccco.h"
#include "dat.h"
#include "fn.h"

int
chanopen(Chan *c, long nel, long elsz)
{
	Lock *l, *cl;
	Cond *da, *sa, *sc;
	uchar *buf;

	if(c == nil)
		return -1;
	if(c->status == Open){
		errorf("chanopen -- called on open channel\n");
		return -1;
	}
	l = createlock();
	if(l == nil){
		errorf("chanopen -- failed to alloc lock\n");
		return -1;
	}
	da = createcond();
	if(da == nil){
		errorf("chanopen -- createcond failed\n");
		freelock(l);
		return -1;
	}
	sa = createcond();
	if(sa == nil){
		errorf("chanopen -- createcond failed\n");
		freecond(da);
		freelock(l);
		return -1;
	}
	sc = createcond();
	if(sc == nil){
		errorf("chanopen -- createcond failed\n");
		freecond(sa);
		freecond(da);
		freelock(l);
		return -1;
	}
	if(nel > 0)
		buf = calloc(nel, elsz);
	else
		buf = calloc(1, elsz);
	if(buf == nil){
		errorf("chanopen -- failed to alloc channel buffer\n");
		freecond(sc);
		freecond(sa);
		freecond(da);
		freelock(l);
		return -1;
	}
	c->l = l;
	c->status = Open;
	c->da = da;
	c->sa = sa;
	c->sc = sc;
	c->b = buf;
	c->elsz = elsz;
	c->sz = nel;
	c->s = 0;
	c->n = 0;
	c->recvbuf = nil;
	return 0;
}
