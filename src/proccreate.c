#include <stddef.h>
#include <stdlib.h>
#include "csp.h"
#include "dat.h"
#include "fn.h"

Proc*
proccreate(void (*fn)(void*), void *arg, int stksz)
{
	Proc *p;
	
	p = malloc(sizeof *p);
	if(p == nil){
		errorf("proccreate -- out of memory\n");
		return nil;
	}
	if(procinit(p, fn, arg, stksz) != 0){
		free(p);
		return nil;
	}
	return p;
}
