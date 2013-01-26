#include <stddef.h>
#include <windows.h>
#include "csp.h"
#include "dat.h"
#include "fn.h"

struct _Lock {
	CRITICAL_SECTION	l;
};

struct _Thread {
	void	(*fn)(void*);	/* proc 'main' */
	void*	arg;		/* proc args */
	HANDLE	h;		/* thread handle */
};

/*
 * This wraps Thread->fn.
 */
DWORD WINAPI
run(void *arg)
{
	Thread *t;
	
	t = (Thread*)arg;
	t->fn(t->arg);
	return nil;
}

Thread*
createthread(void (*fn)(void*), void *arg, int stksz)
{
	Thread *t;
	LPTHREAD_START_ROUTINE entr;
	HANDLE h;
	
	t = malloc(sizeof *t);
	if(t == nil)
		return nil;
	t->fn = fn;
	t->arg = arg;
	entr = (LPTHREAD_START_ROUTINE)&run;
	h = CreateThread(nil, stksz, entr, t, 0, nil);
	if(h == nil){
		errorf("createthread - win32 CreateThread failed\n");
		free(t);
		return nil;
	}
	t->h = h;
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

Lock*
createlock(void)
{
	Lock *l;
	CRITICAL_SECTION cs;
	
	l = malloc(sizeof *l);
	if(l == nil)
		return nil;
	InitializeCriticalSection(&cs);
	l->l = cs;
	return l;
}

int
destroylock(Lock *l)
{
	DeleteCriticalSection(&l->l);
	return 0;
}

int
lock(Lock *l)
{
	EnterCriticalSection(&l->l);
	return 0;
}

int
unlock(Lock *l)
{
	LeaveCriticalSection(&l->l);
	return 0;
}
