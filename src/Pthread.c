#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "csp.h"
#include "dat.h"
#include "fn.h"

#define dprintf (void)

enum {
	Qinitsz	= 4,
	Qgrow	= 2
};

typedef struct Tqueue	Tqueue;

struct Tqueue {
	pthread_cond_t*	a;
	uint		max;
	uint		front;
	uint		rear;
};

struct _Lock {
	Tqueue		q;
	int		held;
	pthread_t		holder;
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
static int		initqueue(Tqueue*, uint);
static void	freequeue(Tqueue*);
static int		enqueue(Tqueue*, pthread_cond_t);
static int		dequeue(Tqueue*, pthread_cond_t*);
static void	shovequeue(Tqueue*);

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

Lock*
createlock(void)
{
	Lock *lok;
	pthread_mutex_t l;
	pthread_mutexattr_t at;
	
	lok = malloc(sizeof *lok);
	if(lok == nil)
		return nil;
	if(initqueue(&lok->q, Qinitsz) != 0)
		return nil;
	if(pthread_mutexattr_init(&at) != 0){
		freequeue(&lok->q);
		return nil;
	}
	if(pthread_mutex_init(&l, &at) != 0){
		freequeue(&lok->q);
		return nil;
	}
	lok->l = l;
	lok->attr = at;
	return lok;
}

int
initlock(Lock *l)
{
	initqueue(&l->q, Qinitsz);
	return pthread_mutex_init(&l->l, nil);
}

int
destroylock(Lock *l)
{
	freequeue(&l->q);
	return pthread_mutex_destroy(&l->l);
}

/* 
 * This is probably worthless in the face of contention.
 * Probably worse than blocking alone.
 */
int
lock(Lock *l, int blocking)
{
	pthread_cond_t wait;

	pthread_mutex_lock(&l->l);
	if(!l->held){
		dprintf("lock: !%p->held; lock\n", l);
		l->held = 1;
		l->holder = pthread_self();
		pthread_mutex_unlock(&l->l);
		return 1;
	}else{
		dprintf("lock: %p held by %p; wait\n", l, &l->holder);
		pthread_cond_init(&wait, nil);
		enqueue(&l->q, wait);
		if(!blocking){
			dprintf("lock: %p no block\n", l);
			return 0;
		}
		dprintf("lock: %p cond block\n", l);
		pthread_cond_wait(&wait, &l->l);

		dprintf("lock: %p cond wake\n", l);
		pthread_cond_destroy(&wait);
		l->holder = pthread_self();
		pthread_mutex_unlock(&l->l);
		return 1;
	}
}

int
unlock(Lock *l)
{
	pthread_cond_t waiter;

	pthread_mutex_lock(&l->l);
	if(!pthread_equal(l->holder, pthread_self())){
		errorf("unlock -- called by alien thread\n");
		return -1;
	}
	if(!dequeue(&l->q, &waiter)){
		dprintf("unlock: %p none queued; unlock\n", l);
		l->held = 0;
		pthread_mutex_unlock(&l->l);
		return 0;
	}else{
		dprintf("unlock: %p has queue; signal\n", l);
		pthread_mutex_unlock(&l->l);	/* waiter wants that lock */
		pthread_cond_signal(&waiter);
		dprintf("unlock: %p has queue; exchanged\n", l);
		return 1;
	}
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

static int
initqueue(Tqueue *q, uint nel)
{
	q->a = malloc(nel * sizeof(pthread_t));
	if(q->a == nil)
		return -1;
	q->max = nel;
	q->front = 0;
	q->rear = 0;
	return 0;
}

static void
freequeue(Tqueue *q)
{
	if(q == nil)
		return;
	free(q->a);
	memset(q, 0, sizeof(Tqueue));
}

static void
shovequeue(Tqueue *q)
{
	memmove(q->a, q->a+q->front, q->rear-q->front);
	q->rear -= q->front;
	q->front = 0;
}

static int
enqueue(Tqueue *q, pthread_cond_t t)
{
	pthread_cond_t *p;

	if(q == nil)
		return -1;
	if(q->rear >= q->max){
		q->max *= Qgrow;
		p = realloc(q->a, q->max * sizeof(pthread_cond_t));
		if(p == nil)
			return -1;
		q->a = p;
		shovequeue(q);
	}
	q->a[++q->rear] = t;
	return q->rear;
}

static int
dequeue(Tqueue *q, pthread_cond_t *out)
{
	if(q->front >= q->rear)
		return 0;	/* empty */
	*out = q->a[++q->front];
	return 1;
}
