#include <stddef.h>
#include <stdlib.h>
#include "csp.h"
#include "dat.h"
#include "fn.h"

Proc*
proccreate(void (*fn)(void*), void *arg, size_t stksz)
{
	Proc *p;
	Thread *t;
	
	p = malloc(sizeof *p);
	if(p == nil){
		errorf("proccreate -- out of memory\n");
		return nil;
	}
	t = createthread(fn, arg, stksz);
	if(t == nil){
		errorf("proccreate -- could not create thread\n");
		return nil;
	}
	p->t = t;
	p->fn = fn;
	p->arg = arg;
	p->name = "<name>";
	return p;
}
