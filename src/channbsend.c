#include <string.h>
#include "csp.h"
#include "dat.h"
#include "fn.h"

int
channbsend(Chan *c, void *p)
{
	int nsent;
	uchar *bp;

	if(c == nil)
		return -1;

	lock(c->l, 1);
	if(c->n >= c->sz){
		printf("chansend -- buf full, return\n");
		unlock(c->l);
		return 0;
	}
	printf("chansend -- buf not full, proceed\n");
	bp = (c->b + c->elsz * ((c->s + c->n) % c->sz));
	if(p == nil){
		/* send zeroes */
		memset(bp, 0, c->elsz);
		nsent = 0;
	}else{
		memmove(bp, p, c->elsz);
		nsent = 1;
	}
	c->n++;
	if(c->n >= c->sz){
		printf("chansend -- buf full, signal\n");
		signal(c->full);
	}
	unlock(c->l);
	return nsent;
}
