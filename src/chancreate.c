#include <stdlib.h>
#include "csp.h"
#include "dat.h"
#include "fn.h"

Chan*
chancreate(long nel, long elsz)
{
	Chan *c;
	
	c = malloc(sizeof *c);
	if(c == nil){
		errorf("chancreate -- out of memory\n");
		return nil;
	}
	if(chanopen(c, nel, elsz) != 0){
		free(c);
		return nil;
	}
	return c;
}
