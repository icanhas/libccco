#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <windows.h>
#include "csp.h"
#include "dat.h"
#include "fn.h"

struct _Lock {
	CRITICAL_SECTION	cs;
};

struct _Cond {
	HANDLE	event;
	Lock	cl;	/* count lock */
	volatile long	nw;	/* num. waiters */
	volatile long	nr;	/* num. threads to release on signal */
	volatile long	wg;	/* wait generation */
};

struct _Thread {
	char*	name;
	void	(*fn)(void*);	/* proc 'main' */
	void*	arg;		/* proc args */
	HANDLE	h;		/* the Windows thread itself */
	Cond	wake;	/* thread starts when wake && running */
	volatile int	running;
	Rand	r;
};

/*
 * This is for thread-local storage.  A thread can call for a 
 * pointer to its own Thread structure.
 */
static DWORD	tlsindex = -1;

static DWORD WINAPI	run(void*);

Thread*
createthread(void (*fn)(void*), void *arg, int stksz)
{
	Thread *t;
	Cond wake;
	LPTHREAD_START_ROUTINE entr;
	HANDLE h;
	
	t = malloc(sizeof *t);
	if(t == nil)
		return nil;
	if(tlsindex == -1)
		tlsindex = TlsAlloc();
	if(initcond(&wake) != 0){
		TlsFree(tlsindex);
		return nil;
	}
	t->name = nil;
	t->fn = fn;
	t->arg = arg;
	t->wake = wake;
	t->running = 0;
	entr = (LPTHREAD_START_ROUTINE)run;
	h = CreateThread(nil, stksz, entr, t, 0, nil);
	if(h == nil){
		errorf("createthread -- win32 CreateThread failed\n");
		free(t);
		TlsFree(tlsindex);
		return nil;
	}
	t->h = h;
	t->running = 1;
	csignal(&t->wake);
	return t;
}

void
freethread(Thread *t)
{
	if(t == nil)
		return;
	CloseHandle(t->h);
	free(t);
}

void
exitthread(void)
{
	ExitThread(0);
}

void
setthreadname(const char *name)
{
	Thread *t;
	
	t = (Thread*)TlsGetValue(tlsindex);
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
	
	t = (Thread*)TlsGetValue(tlsindex);
	assert(t != nil);
	return t->name;
}

Lock*
createlock(void)
{
	Lock *l;
	
	l = malloc(sizeof *l);
	if(l == nil)
		return nil;
	initlock(l);
	return l;
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
	InitializeCriticalSection(&l->cs);
	return 0;
}

void
destroylock(Lock *l)
{
	assert(l != nil);
	DeleteCriticalSection(&l->cs);
}

int
lock(Lock *l, int blocking)
{
	assert(l != nil);
	if(blocking){
		EnterCriticalSection(&l->cs);
		return 1;
	}
	if(TryEnterCriticalSection(&l->cs))
		return 1;
	return 0;
}

int
unlock(Lock *l)
{
	assert(l != nil);
	LeaveCriticalSection(&l->cs);
	return 0;
}

Cond*
createcond(void)
{
	Cond *p;
	
	p = malloc(sizeof *p);
	if(p == nil){
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
	c->event = CreateEvent(NULL, TRUE, FALSE, TEXT("Cond"));
	if(c->event == nil){
		errorf("initcond -- CreateEvent failed\n");
		return -1;
	}
	if(initlock(&c->cl) != 0){
		errorf("initcond -- initlock failed\n");
		CloseHandle(c->event);
		return -1;
	}
	c->nw = 0;
	c->nr = 0;
	c->wg = 0;
	return 0;
}

void
destroycond(Cond *c)
{
	assert(c != nil);
	CloseHandle(c->event);
	destroylock(&c->cl);
}

int
wait(Cond *c, Lock *l)
{
	long mygen;
	int done, last;
	
	assert(c != nil);
	assert(l != nil);
	
	lock(&c->cl, 1);
	c->nw++;
	mygen = c->wg;
	unlock(&c->cl);
	unlock(l);
	for(done = 0; !done;){
		WaitForSingleObject(c->event, INFINITE);
		lock(&c->cl, 1);
		done = ((c->nr > 0) && (c->wg != mygen));
		unlock(&c->cl);
	}
	lock(l, 1);
	lock(&c->cl, 1);
	c->nw--;
	c->nr--;
	last = (c->nr == 0);
	unlock(&c->cl);
	if(last)
		ResetEvent(c->event);
}

int
csignal(Cond *c)
{
	assert(c != nil);
	
	lock(&c->cl, 1);
	if(c->nw > 0){
		SetEvent(c->event);
		/*
		 * Release all threads in this gen and create a new gen
		 */
		c->nr = c->nw;
		c->wg++;
	}
	unlock(&c->cl);
}

void
_procsleep(long dur)
{
	Sleep(dur * 1000);
}

void
_stlrand(ulong seed)
{
	Thread *t;
	
	t = (Thread*)TlsGetValue(tlsindex);
	assert(t != nil);
	_srand(&t->r, seed);
}

ulong
_tlrand(void)
{
	Thread *t;
	
	t = (Thread*)TlsGetValue(tlsindex);
	assert(t != nil);
	return _rand(&t->r);
}

/*
 * CreateThread takes its fn parameter as DWORD WINAPI (*)(void*),
 * so Thread->fn itself cannot be passed to it.  This has the proper
 * signature and wraps Thread->fn.  It also initialises thread-local
 * storage and waits for the Thread structure to be completely filled
 * out before finally calling the thread's main procedure.
 */
static DWORD WINAPI
run(void *arg)
{
	Thread *t;
	Lock fake;
	
	t = (Thread*)arg;
	TlsSetValue(tlsindex, t);
	
	initrand(&t->r);
	
	initlock(&fake);
	lock(&fake, 1);
	while(!t->running){
		dprintf("thread suspend\n");
		wait(&t->wake, &fake);
	}
	destroycond(&t->wake);
	unlock(&fake);
	destroylock(&fake);
	dprintf("thread %p starts\n", t);
	t->fn(t->arg);
	return nil;
}
