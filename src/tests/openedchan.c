#include <assert.h>
#include <csp.h>
#include <stdio.h>
#include "a.h"

int
main(int argc, char **argv)
{
	int fail;
	Chan c1, *c2;
	void *v;

	fail = 0;
	assert(chanopen(&c1, 1, 1) == 0);
	assert((c2 = chancreate(1, 1)) != nil);
	#define test(a)	do{ \
		if((a)){ \
			fprintf(stderr, "fail: (%s) -- a chan creation procedure"  \
				 " completed despite being called on an" \
				 " open channel\n", #a); \
			 fail = 1; \
		} \
	}while(0);
	test(chanopen(&c1, 2, 4) == 0);
	test(chanopen(c2, 2, 4) == 0);
	#undef test
	return fail;
}
