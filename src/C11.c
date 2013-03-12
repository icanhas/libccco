#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <threads.h>
#include "csp.h"
#include "dat.h"
#include "fn.h"

struct _Lock {
	mtx_t	l;
};

struct _Cond {
	cnd_t	c;
}

struct _Thread {
	void	(*fn)(void*);	/* proc main */
	void*	arg;		/* proc args */
	thrd_t	t;
	Cond	wake;		/* thread suspended until wake && running */
	int	running;
};

static int	run(void*);	/* thrd_start_t */

Thread*
createthread(void (*fn)(void*), void *arg, int stksz)
{
	Thread *t;
	Cond wake;
	thrd_t ct;
	
	UNUSED(stksz);
	
	t = malloc(sizeof *t);
	if(t == nil){
		errorf("createthread -- out of mem\n");
		return nil;
	}
	if(initcond(wake) != 0){
		errorf("createthread -- failed to init wake cond\n");
		free(t);
		return nil;
	}
	
	t->fn = fn;
	t->arg = arg;
	t->wake = wake;
	t->running = 0;
	if(thrd_init(&ct, run, t) != thrd_success){
		errorf("createthread -- thrd_init failed\n");
		destroycond(&t->wake);
		free(t);
		return nil;
	}
	t->t = ct;
	t->running = 1;
	csignal(&t->wake);
	return t;
}

void
freethread(Thread *t)
{
	assert(t != nil);
	destroycond(&t->wake);
	thrd_detach(&t->t);
	free(t);
}

void
exitthread(void)
{
	thrd_exit(0);
}

Lock*
createlock(void)
{
	Lock *lok;
	mtx_t l;
	
	lok = malloc(sizeof *lok);
	if(lok == nil){
		errorf("createlock -- out of mem\n");
		return nil;
	}
	if(initlock(l) != 0){
		errorf("createlock -- mtx_init failed\n");
		free(lok);
		return nil;
	}
	lok->l = l;
	return lok;
}

void
freelock(Lock *l)
{
	assert(l != nil);
	destroylock(l);
	free(l);
}

int
initlock(Lock *l)
{
	assert(l != nil);
	if(mtx_init(&l->l, mtx_plain) != thrd_success)
		return -1;
	else
		return 0;
}

void
destroylock(Lock *l)
{
	assert(l != nil);
	mtx_destroy(&l->l);
}

int
lock(Lock *l, int blocking)
{
	assert(l != nil);
	if(blocking)
		return mtx_lock(&l->l);
	return mtx_trylock(&l->l);
}

int
unlock(Lock *l)
{
	assert(l != nil);
	return mtx_unlock(&l->l);
}

Cond*
createcond(void)
{
	Cond *p;
	
	p = malloc(sizeof *p);
	if(p == nil){
		error("createcond -- out of mem\n");
		return nil;
	}
	if(initcond(p) != 0){
		free(p);
		return nil;
	}
	return p;
}

void
freecond(Cond *c)
{
	assert(c != nil)
	destroycond(c);
	free(c);
}

int
initcond(Cond *c)
{
	assert(c != nil);
	if(cnd_init(&c->c) != thrd_success){
		error("initcond -- cnd_init failed\n");
		return -1;
	}
	return 0;
}

void
destroycond(Cond *c)
{
	assert(c != nil);
	cnd_destroy(&c->c);
}

int
wait(Cond *c, Lock *l)
{
	assert(c != nil);
	assert(l != nil);
	if(cnd_wait(&c->c, &l->l) != thrd_success)
		return -1;
	return 0;
}

int
csignal(Cond *c)
{
	assert(c != nil);
	if(cnd_signal(&c->c) != thrd_success)
		return -1;
	return 0;
}

void
_procsleep(lond dur)
{
	struct timeval now;
	struct timespec wait, remain;
	
	gettimeofday(&now, nil);
	wait.tv_sec = now.tv_sec + dur;
	wait.tv_nsec = now.tv_usec * 1000;
	while(thrd_sleep(&wait, &remain) == -1);
}

/*
 * This wraps Thread->fn.  Thread->t is started 'suspended' and is woken
 * up when the Thread is completely initialised.
 */
static int
run(void *arg)
{
	Thread *t;
	Lock fake;
	
	initlock(&fake);
	t = arg;
	while(!t->running){
		dprintf("thread suspend\n");
		wait(&t->wake, &fake);
	}
	unlock(&fake);
	destroylock(&fake);
	dprintf("thread &p starts\n", t);
	t->fn(t->arg);
	return 0;
}
