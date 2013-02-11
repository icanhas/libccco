#include <string.h>
#include "csp.h"
#include "dat.h"
#include "fn.h"

int
_send(Chan *c, void *p, int blocking)
{
	int nsent;
	uchar *bp;

	if(c == nil)
		return -1;

	lock(c->l, 1);
	if(blocking){
		while(c->n >= c->sz){
			printf("chansend -- buf full, wait\n");
			wait(c->flushed, c->l);
		}
	}else{
		if(c->n >= c->sz){
			printf("chansend -- buf full, return\n");
			unlock(c->l);
			return 0;
		}
	}
	printf("chansend -- proceed\n");
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

int
_recv(Chan *c, void *p, int blocking)
{
	uchar *bp;
	
	if(c == nil){
		errorf("recv -- called on nil Chan\n");
		return -1;
	}
	if(p == nil){
		errorf("recv -- given nil receiving buffer\n");
		return -1;
	}
	lock(c->l, 1);
	if(blocking){
		while(c->n < c->sz){
			printf("recv -- wait\n");
			wait(c->full, c->l);
		}
	}else{
		if(c->n < c->sz){
			printf("recv -- return\n");
			unlock(c->l);
			return 0;
		}
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
