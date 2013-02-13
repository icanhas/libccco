#include <pthread.h>
#include <assert.h>
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
	char*	name;
	void	(*fn)(void*);	/* proc 'main' */
	void*	arg;		/* proc args */
	pthread_t		t;
	pthread_attr_t	attr;
	Cond	wake;	/* thread starts when wake && running */
	int	running;
};

/*
 * These two are for thread-local storage.  A thread can call for a 
 * pointer to its own Thread structure.
 */
static pthread_key_t	tlskey;
static pthread_once_t	tlsonce = PTHREAD_ONCE_INIT;

static void*	run(void*);
static void	construct(void);
static void	destruct(void*);

Thread*
createthread(void (*fn)(void*), void *arg, int stksz)
{
	Thread *t;
	Cond wake;
	pthread_attr_t at;
	pthread_t p;
	
	t = malloc(sizeof *t);
	if(t == nil)
		return nil;

	if(initcond(&wake) != 0){
		free(t);
		return nil;
	}
	pthread_attr_init(&at);
	if(stksz > 0){
		if(pthread_attr_setstacksize(&at, stksz) != 0)
			errorf("createthread -- given stack size too small (%u)\n", stksz);
	}
	pthread_attr_setdetachstate(&at, PTHREAD_CREATE_JOINABLE);
	t->fn = fn;
	t->arg = arg;
	t->wake = wake;
	t->running = 0;
	if(pthread_create(&p, &at, run, t) != 0){
		errorf("createthread -- pthread_create failed\n");
		free(t);
		return nil;
	}
	t->t = p;
	t->attr = at;
	t->running = 1;
	signal(&t->wake);
	return t;
}
	
void
freethread(Thread *t)
{
	assert(t != nil);
	destroycond(&t->wake);
	pthread_detach(t->t);
	pthread_attr_destroy(&t->attr);
	free(t);
}

void
exitthread(void)
{
	pthread_exit(nil);
}

void
setthreadname(const char *name)
{
	Thread *t;
	
	t = (Thread*)pthread_getspecific(tlskey);
	assert(t != nil);
	if(t->name != nil)
		free(t->name);
	t->name = malloc(strlen(name)+1);
	strcpy(t->name, name);
}

char*
getthreadname(void)
{
	Thread *t;
	
	t = (Thread*)pthread_getspecific(tlskey);
	return t->name;
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
	if(pthread_mutexattr_init(&at) != 0){
		free(lok);
		return nil;
	}
	if(pthread_mutex_init(&l, &at) != 0){
		free(lok);
		return nil;
	}
	lok->l = l;
	lok->attr = at;
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
	return pthread_mutex_init(&l->l, nil);
}

void
destroylock(Lock *l)
{
	assert(l != nil);
	(void)pthread_mutex_destroy(&l->l);
	(void)pthread_mutexattr_destroy(&l->attr);
}

int
lock(Lock *l, int blocking)
{
	assert(l != nil);
	if(blocking)
		return pthread_mutex_lock(&l->l);
	else
		return pthread_mutex_trylock(&l->l);
}

int
unlock(Lock *l)
{
	assert(l != nil);
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

void
freecond(Cond *c)
{
	assert(c != nil);
	destroycond(c);
	free(c);
}

int
initcond(Cond *c)
{
	assert(c != nil);
	if(pthread_cond_init(&c->c, nil) != 0){
		errorf("initcond -- pthread_cond_init failed\n");
		return -1;
	}
	return 0;
}

void
destroycond(Cond *c)
{
	assert(c != nil);
	(void)pthread_cond_destroy(&c->c);
}

int
wait(Cond *c, Lock *l)
{
	assert(c != nil);
	assert(l != nil);
	return pthread_cond_wait(&c->c, &l->l);
}

int
signal(Cond *c)
{
	assert(c != nil);
	return pthread_cond_signal(&c->c);
}

void
_procsleep(long dur)
{
	pthread_mutex_t l = PTHREAD_MUTEX_INITIALIZER;
	pthread_cond_t c = PTHREAD_COND_INITIALIZER;
	struct timeval now;
	struct timespec wait;
	
	gettimeofday(&now, nil);
	wait.tv_sec = now.tv_sec + dur;
	wait.tv_nsec = now.tv_usec * 1000;
	
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
	Lock fake;
	
	initlock(&fake);
	
	t = (Thread*)arg;

	pthread_once(&tlsonce, construct);
	pthread_setspecific(tlskey, t);
	
	lock(&fake, 1);
	while(!t->running){
		dprintf("thread suspend\n");
		wait(&t->wake, &fake);
	}
	unlock(&fake);
	destroylock(&fake);
	dprintf("thread %p starts\n", t);
	t->fn(t->arg);
	return nil;
}

static void
construct(void)
{
	pthread_key_create(&tlskey, destruct);
}

static void
destruct(void *keydata)
{
	dprintf("destruct\n");
}
