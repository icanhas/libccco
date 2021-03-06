#include <pthread.h>
#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include "ccco.h"
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
	Lock	runwait;
	Rand	r;
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
	Lock runwait;
	pthread_attr_t at;
	pthread_t p;
	
	t = malloc(sizeof *t);
	if(t == nil)
		return nil;

	if(initlock(&runwait) != 0){
		free(t);
		return nil;
	}
	pthread_attr_init(&at);
	if(stksz > 0){
		if(pthread_attr_setstacksize(&at, stksz) != 0)
			errorf("createthread -- given stack size too small (%u)\n", stksz);
	}
	pthread_attr_setdetachstate(&at, PTHREAD_CREATE_JOINABLE);
	t->name = nil;
	t->fn = fn;
	t->arg = arg;
	t->runwait = runwait;
	lock(&t->runwait, 1);
	if(pthread_create(&p, &at, run, t) != 0){
		errorf("createthread -- pthread_create failed\n");
		free(t);
		return nil;
	}
	t->t = p;
	t->attr = at;
	unlock(&t->runwait);
	return t;
}

void
freethread(Thread *t)
{
	assert(t != nil);
	if(t->name != nil)
		free(t->name);
	pthread_detach(t->t);
	pthread_attr_destroy(&t->attr);
	destroyrand(&t->r);
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
	assert(t != nil);
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
	while(pthread_cond_destroy(&c->c) != 0);
}

int
wait(Cond *c, Lock *l)
{
	assert(c != nil);
	assert(l != nil);
	return pthread_cond_wait(&c->c, &l->l);
}

int
notify(Cond *c)
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

void
_stlrand(ulong seed)
{
	Thread *t;
	
	t = (Thread*)pthread_getspecific(tlskey);
	assert(t != nil);
	_srand(&t->r, seed);
}

ulong
_tlrand(void)
{
	Thread *t;
	
	t = (Thread*)pthread_getspecific(tlskey);
	assert(t != nil);
	return _rand(&t->r);
}

/*
 * pthread_create takes its fn parameter as void *(*)(void*), so
 * Thread->fn itself cannot be passed to it.  This has the proper signature
 * and wraps Thread->fn.  It also initialises thread-local storage and
 * waits for the Thread structure to be completely filled out before
 * finally starting the thread's main procedure.
 */
static void*
run(void *arg)
{
	Thread *t;
	
	t = (Thread*)arg;
	dprintf("thread %p spawn\n", t);
	pthread_once(&tlsonce, construct);
	pthread_setspecific(tlskey, t);
	initrand(&t->r);
	lock(&t->runwait, 1);
	unlock(&t->runwait);
	destroylock(&t->runwait);
	dprintf("thread %p run\n", t);
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
	Thread *t;
	
	dprintf("destruct\n");
	if(0){
	t = (Thread*)keydata;
	destroyrand(&t->r);
	}
}
