#include <assert.h>
#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <csp.h>

void
uselesswork(void)
{
	int j;
	float f;
	
	for(j = 0; j < 100000; ++j)
		f = atan2(j, f);
}

void
f(void *p)
{
	int i;

	for(i = 0; i < 1000; ++i){
		uselesswork();
		printf("proc %d\n", *((int*)p));
	}
}

int
main(void)
{
	Proc *p1;
	Proc p2;
	Proc *p3;

	assert((p1 = proccreate(f, &(int){1}, 0)) != NULL);
	assert(procinit(&p2, f, &(int){2}, 0) == 0);
	assert((p3 = proccreate(f, &(int){3}, 256)) != NULL);
	f(&(int){0});	/* p1..3 should be killed immediately after this returns */
	procfree(p1);
	prockill(&p2);
	procfree(p3);
}
