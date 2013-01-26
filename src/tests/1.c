#include <assert.h>
#include <stddef.h>
#include "csp.h"

void
f(void *p)
{
	(void)p;
}

int
main(void)
{
	Proc *p1;
	Proc p2;
	
	assert((p1 = proccreate(f, NULL, 0)) != NULL);
	procfree(p1);
	assert(procinit(&p2, f, NULL, 0) == 0);
	prockill(&p2);
}
