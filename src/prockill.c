#include "ccco.h"
#include "dat.h"
#include "fn.h"

void
prockill(Proc *p)
{
	if(p == nil)
		return;
	freethread(p->t);
}
