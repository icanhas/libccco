#include <stddef.h>
#include <stdlib.h>
#include <pthread.h>
#include "csp.h"
#include "dat.h"
#include "fn.h"

struct _Lock {
	pthread_mutex_t	l;
	pthread_attr_t	attr;
};

struct _Thread {
	pthread_t		t;
	pthread_attr_t	attr;
};

Thread*
createthread(void (*fn)(void*), void *arg, size_t stksz)
{
	Thread *t;
	pthread_attr_t at;
	pthread_t p;
	/* FIXME: pthread wants ptr return type ;/ */
	void *(*fnn)(void*) = (void *(*)(void*))fn;
	
	t = malloc(sizeof *t);
	if(t == nil)
		return nil;

	pthread_attr_init(&at);
	if(stksz > 0){
		if(pthread_attr_setstacksize(&at, stksz) != 0)
			errorf("createthread -- specified stack size too small (%u)\n", stksz);
	}
	if(pthread_create(&p, &at, fnn, arg) != 0)
		errorf("createthread -- pthread_create failed\n");
	t->t = p;
	t->attr = at;
	return t;
}
	
void
killthread(Thread *t)
{
	if(t == nil)
		return;
	pthread_detach(t->t);
	pthread_attr_destroy(&t->attr);
	free(t);
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
lock(Lock *l)
{
	return pthread_mutex_lock(&l->l);
}

int
unlock(Lock *l)
{
	return pthread_mutex_unlock(&l->l);
}
