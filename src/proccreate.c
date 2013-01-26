#include <stddef.h>
#include <stdlib.h>
#include "csp.h"
#include "dat.h"
#include "fn.h"

Proc*
proccreate(void (*fn)(void*), void *arg, int stksz)
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
	p->name = "<name>";
	return p;
}
