#include <string.h>
#include "ccco.h"
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
	if(c->status != Open){
		errorf("send -- called on closed channel\n");
		return -1;
	}
	
	dprintf("send %s n=%ld recvbuf=%p\n", ((c->sz == 0) ? "unbuf" : "buf"), c->n, c->recvbuf);

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
	if(c->status != Open){
		errorf("recv -- called on closed channel\n");
		return -1;
	}
	
	dprintf("recv %s n=%ld recvbuf=%p\n", ((c->sz == 0) ? "unbuf" : "buf"), c->n, c->recvbuf);

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
	int nsent;
	
	while(c->recvbuf == nil){
		dprintf("send -- unbuf wait for receiver\n");
		wait(c->sa, c->l);
	}
	dprintf("send -- unbuf rendezvous (recvbuf=%p)\n", c->recvbuf);
	if(p == nil){
		memset(c->recvbuf, 0, c->elsz);
		nsent = 0;
	}else{
		memmove(c->recvbuf, p, c->elsz);
		nsent = 1;
	}
	c->n = 1;
	c->recvbuf = nil;
	notify(c->da);
	unlock(c->l);
	return nsent;
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
		dprintf("send -- buf full, wait n=%ld\n", c->n);
		wait(c->sa, c->l);
	}
	dprintf("send -- buf proceed n=%ld\n", c->n);
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
	notify(c->da);
	unlock(c->l);
	return nsent;
}

/* 
 * Non-blocking unbuffered send
 */
static int
nbusend(Chan *c, void *p)
{
	if(c->s != 0){
		dprintf("send -- nb unbuf channel already has sender\n");
		unlock(c->l);
		return 0;
	}
	c->s = 1;
	if(c->recvbuf == nil){
		dprintf("send -- nb unbuf no recver, return\n");
		unlock(c->l);
		return 0;
	}
	dprintf("send -- nb unbuf rendezvous\n");
	memmove(c->recvbuf, p, c->elsz);
	c->n = 1;
	c->s = 0;
	notify(c->da);
	while(c->n != 0){
		wait(c->sa, c->l);
	}
	c->recvbuf = nil;
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
	notify(c->da);
	unlock(c->l);
	return nsent;
}

/* 
 * Blocking unbuffered recv
 */
static int
urecv(Chan *c, void *p)
{	
	while(c->recvbuf != nil){
		/* 
		 * Another receiver got here before us; wait for a sender
		 * to meet it and do the op.
		 */
		wait(c->sc, c->l);
	}
	c->recvbuf = p;
	notify(c->sa);
	while(c->n != 1){
		dprintf("recv -- unbuf wait for sender\n");
		wait(c->da, c->l);
	}
	dprintf("recv -- unbuf rendezvous done\n");
	c->n = 0;
	notify(c->sc);
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
		dprintf("recv -- buf wait n=%ld\n", c->n);
		wait(c->da, c->l);
	}
	dprintf("recv -- buf proceed n=%ld\n", c->n);
	bp = (c->b + c->elsz * (c->s % c->sz));
	memmove(p, bp, c->elsz);
	c->n--;
	c->s++;
	notify(c->sa);
	unlock(c->l);
	return 1;
}

/* 
 * Non-blocking unbuffered recv
 */
static int
nburecv(Chan *c, void *p)
{
	if(c->recvbuf != nil){
		dprintf("recv -- nb unbuf channel already has recver\n");
		unlock(c->l);
		return 0;
	}
	c->recvbuf = p;
	if(c->s == 0){
		dprintf("recv -- nb unbuf no sender\n");
		c->recvbuf = nil;
		unlock(c->l);
		return 0;
	}
	dprintf("recv -- nb unbuf rendezvous\n");
	notify(c->sa);
	while(c->n != 1){
		wait(c->da, c->l);
	}
	c->n = 0;
	unlock(c->l);
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
	notify(c->sa);
	unlock(c->l);
	return 1;
}
