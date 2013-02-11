#include <pthread.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include "csp.h"
#include "dat.h"
#include "fn.h"

struct _Lock {
	pthread_mutex_t	l;
	pthread_mutexattr_t	attr;
};

struct _Cond {
	pthread_cond_t	c;
};

struct _Thread {
	void	(*fn)(void*);	/* proc 'main' */
	void*	arg;		/* proc args */
	pthread_t		t;
	pthread_attr_t	attr;
};

static void*	run(void*);

Thread*
createthread(void (*fn)(void*), void *arg, int stksz)
{
	Thread *t;
	pthread_attr_t at;
	pthread_t p;
	
	t = malloc(sizeof *t);
	if(t == nil)
		return nil;

	pthread_attr_init(&at);
	if(stksz > 0){
		if(pthread_attr_setstacksize(&at, stksz) != 0)
			errorf("createthread -- given stack size too small (%u)\n", stksz);
	}
	pthread_attr_setdetachstate(&at, PTHREAD_CREATE_JOINABLE);
	t->fn = fn;
	t->arg = arg;
	if(pthread_create(&p, &at, run, t) != 0){
		errorf("createthread -- pthread_create failed\n");
		free(t);
		return nil;
	}
	t->t = p;
	t->attr = at;
	return t;
}
	
void
freethread(Thread *t)
{
	if(t == nil)
		return;
	pthread_detach(t->t);
	pthread_attr_destroy(&t->attr);
	free(t);
}

void
exitthread(void)
{
	pthread_exit(nil);
}

Lock*
createlock(void)
{
	Lock *lok;
	pthread_mutex_t l;
	pthread_mutexattr_t at;
	
	lok = malloc(sizeof *lok);
	if(lok == nil)
		return nil;
	if(pthread_mutexattr_init(&at) != 0)
		return nil;
	if(pthread_mutex_init(&l, &at) != 0)
		return nil;
	lok->l = l;
	lok->attr = at;
	return lok;
}

int
initlock(Lock *l)
{
	return pthread_mutex_init(&l->l, nil);
}

int
destroylock(Lock *l)
{
	return pthread_mutex_destroy(&l->l);
}

int
lock(Lock *l, int blocking)
{
	if(blocking)
		return pthread_mutex_lock(&l->l);
	else
		return pthread_mutex_trylock(&l->l);
}

int
unlock(Lock *l)
{
	return pthread_mutex_unlock(&l->l);
}

Cond*
createcond(void)
{
	Cond *p;
	
	if((p = malloc(sizeof *p)) == nil){
		errorf("createcond -- out of memory\n");
		return nil;
	}
	if(initcond(p) != 0){
		free(p);
		return nil;
	}
	return p;
}

int
initcond(Cond *c)
{
	if(pthread_cond_init(&c->c, nil) != 0){
		errorf("initcond -- pthread_cond_init failed\n");
		return -1;
	}
	return 0;
}

int
destroycond(Cond *c)
{
	return pthread_cond_destroy(&c->c);
}

int
wait(Cond *c, Lock *l)
{
	return pthread_cond_wait(&c->c, &l->l);
}

int
signal(Cond *c)
{
	return pthread_cond_signal(&c->c);
}

void
_procsleep(long dur)
{
	pthread_mutex_t l = PTHREAD_MUTEX_INITIALIZER;
	pthread_cond_t c = PTHREAD_COND_INITIALIZER;
	struct timespec wait, now;
	
	gettimeofday(&now, nil);
	wait.tv_sec = now.tv_sec + dur;
	wait.tv_nsec = now.tv_nsec;
	
	pthread_mutex_lock(&l);
	pthread_cond_timedwait(&c, &l, &wait);
	pthread_mutex_unlock(&l);
}

/*
 * pthread_create takes its fn parameter as void *(*)(void*), so
 * Thread->fn itself cannot be passed to it.  This has the proper
 * signature and wraps Thread->fn.
 */
static void*
run(void *arg)
{
	Thread *t;
	
	t = (Thread*)arg;
	t->fn(t->arg);
	return nil;
}
