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
		if(c->sz == 0){
			/* Unbuffered case */
			while(c->n != -1){
				dprintf("send -- unbuf wait for recv\n");
				wait(c->avail, c->l);
			}
			dprintf("send -- unbuf proceed\n");
			memmove(c->b, p, c->elsz);
			c->n = 1;
			signal(c->avail);
			unlock(c->l);
			return 1;
		}
		while(c->n >= c->sz){
			dprintf("send -- buf full, wait\n");
			wait(c->avail, c->l);
		}
		dprintf("send -- buf proceed\n");
	}else{
		if(c->sz == 0){
			/* Unbuffered case */
			if(c->n != -1){
				dprintf("send -- nb unbuf no recv, return\n");
				unlock(c->l);
				return 0;
			}
			dprintf("send -- nb unbuf proceed\n");
			memmove(c->b, p, c->elsz);
			c->n = 1;
			signal(c->avail);
			unlock(c->l);
			return 1;
		}
		/* Buffered case */
		if(c->n >= c->sz){
			dprintf("chansend -- nb buf full, return\n");
			unlock(c->l);
			return 0;
		}
		dprintf("send -- nb buf proceed\n");
	}
	bp = (c->b + c->elsz * ((c->s + c->n) % c->sz));
	if(p == nil){
		memset(bp, 0, c->elsz);
		nsent = 0;
	}else{
		memmove(bp, p, c->elsz);
		nsent = 1;
	}
	c->n++;
	dprintf("send -- signal\n");
	signal(c->avail);
	unlock(c->l);
	return nsent;
}

int
_recv(Chan *c, void *p, int blocking)
{
	uchar *bp;
	
	if(c == nil){
		errorf("recv -- nil Chan\n");
		return -1;
	}
	if(p == nil){
		errorf("recv -- nil receiving buffer\n");
		return -1;
	}

	lock(c->l, 1);
	if(blocking){
		if(c->sz == 0){
			/* Unbuffered case */
			c->n = -1;
			signal(c->avail);
			while(c->n != 1){
				dprintf("recv -- unbuf wait for send\n");
				wait(c->avail, c->l);
			}
			dprintf("recv -- unbuf proceed\n");
			memmove(p, c->b, c->elsz);
			signal(c->avail);
			unlock(c->l);
			return 1;
		}
		/* Buffered case */
		while(c->n < 1){
			dprintf("recv -- buf wait\n");
			wait(c->avail, c->l);
		}
		dprintf("chanrecv -- buf proceed\n");
	}else{
		if(c->sz == 0){
			/* Unbuffered case */
			if(c->n < 1){
				dprintf("recv -- nb unbuf return\n");
				return 0;
			}
			dprintf("chanrecv -- nb unbuf proceed\n");
			memmove(p, c->b, c->elsz);
			signal(c->avail);
			return 1;
		}
		/* Buffered case */
		if(c->n < 1){
			dprintf("recv -- nb buf return\n");
			unlock(c->l);
			return 0;
		}
		dprintf("chanrecv -- nb buf proceed\n");
	}
	bp = (c->b + c->elsz * (c->s % c->sz));
	memmove(p, bp, c->elsz);
	c->n--;
	c->s++;
	signal(c->avail);
	unlock(c->l);
	return 1;
}
