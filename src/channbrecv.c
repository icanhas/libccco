#include <string.h>
#include "csp.h"
#include "dat.h"
#include "fn.h"

int
channbrecv(Chan *c, void *p)
{
	uchar *bp;
	
	if(c == nil){
		errorf("chanrecv -- called on nil Chan\n");
		return -1;
	}
	if(p == nil){
		errorf("chanrecv -- given nil receiving buffer\n");
		return -1;
	}
	lock(c->l, 1);
	if(c->n < c->sz){
		printf("chanrecv -- return\n");
		unlock(c->l);
		return 0;
	}
	printf("chanrecv -- proceed\n");
	bp = (c->b + c->elsz * (c->s % c->sz));
	memmove(p, bp, c->elsz);
	c->n--;
	c->s++;
	signal(c->flushed);
	unlock(c->l);
	return 1;
}
