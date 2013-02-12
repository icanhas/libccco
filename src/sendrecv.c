#include <string.h>
#include "csp.h"
#include "dat.h"
#include "fn.h"

static int	usend(Chan*, void*);
static int	bsend(Chan*, void*);
static int	nbusend(Chan*, void*);
static int	nbbsend(Chan*, void*);
static int	urecv(Chan*, void*);
static int	brecv(Chan*, void*);
static int	nburecv(Chan*, void*);
static int	nbbrecv(Chan*, void*);

int
_send(Chan *c, void *p, int blocking)
{
	int nsent;
	uchar *bp;

	if(c == nil){
		errorf("send -- nil Chan\n");
		return -1;
	}

	lock(c->l, 1);
	if(blocking){
		if(c->sz == 0)
			return usend(c, p);
		else
			return bsend(c, p);
	}else{
		if(c->sz == 0)
			return nbusend(c, p);
		else
			return nbbsend(c, p);
	}
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
		errorf("recv -- nil receive buffer\n");
		return -1;
	}

	lock(c->l, 1);
	if(blocking){
		if(c->sz == 0)
			return urecv(c, p);
		else
			return brecv(c, p);
	}else{
		if(c->sz == 0)
			return nburecv(c, p);
		else
			return nbbrecv(c, p);
	}
}

/* 
 * Blocking unbuffered send
 */
static int
usend(Chan *c, void *p)
{
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

/* 
 * Blocking buffered send
 */
static int
bsend(Chan *c, void *p)
{
	uchar *bp;
	int nsent;
	
	while(c->n >= c->sz){
		dprintf("send -- buf full, wait\n");
		wait(c->avail, c->l);
	}
	dprintf("send -- buf proceed\n");
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

/* 
 * Non-blocking unbuffered send
 */
static int
nbusend(Chan *c, void *p)
{
	if(c->n != -1){
		dprintf("send -- nb unbuf no recver, return\n");
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

/* 
 * Non-blocking buffered send
 */
static int
nbbsend(Chan *c, void *p)
{
	uchar *bp;
	int nsent;

	if(c->n >= c->sz){
		dprintf("send -- nb buf full, return\n");
		unlock(c->l);
		return 0;
	}
	dprintf("send -- nb buf proceed\n");
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

/* 
 * Blocking unbuffered recv
 */
static int
urecv(Chan *c, void *p)
{
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

/* 
 * Blocking buffered recv
 */
static int
brecv(Chan *c, void *p)
{
	uchar *bp;
	
	while(c->n < 1){
		dprintf("recv -- buf wait\n");
		wait(c->avail, c->l);
	}
	dprintf("recv -- buf proceed\n");
	bp = (c->b + c->elsz * (c->s % c->sz));
	memmove(p, bp, c->elsz);
	c->n--;
	c->s++;
	signal(c->avail);
	unlock(c->l);
	return 1;
}

/* 
 * Non-blocking unbuffered recv
 */
static int
nburecv(Chan *c, void *p)
{
	if(c->n < 1){
		dprintf("recv -- nb unbuf return\n");
		return 0;
	}
	dprintf("recv -- nb unbuf proceed\n");
	memmove(p, c->b, c->elsz);
	signal(c->avail);
	return 1;
}

/* 
 * Non-blocking buffered recv
 */
static int
nbbrecv(Chan *c, void *p)
{
	uchar *bp;
	
	if(c->n < 1){
		dprintf("recv -- nb buf return\n");
		unlock(c->l);
		return 0;
	}
	dprintf("recv -- nb buf proceed\n");
	bp = (c->b + c->elsz * (c->s % c->sz));
	memmove(p, bp, c->elsz);
	c->n--;
	c->s++;
	signal(c->avail);
	unlock(c->l);
	return 1;
}
