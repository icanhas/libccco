#include <stddef.h>
#include <stdlib.h>
#include "csp.h"
#include "dat.h"
#include "fn.h"

int
procinit(Proc *p, void (*fn)(void*), void *arg, int stksz)
{
	Thread *t;

	t = createthread(fn, arg, stksz);
	if(t == nil){
		errorf("procinit -- could not create thread\n");
		return -1;
	}
	p->t = t;
	return 0;
}
