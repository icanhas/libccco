#include <ccco.h>
#include <stdio.h>
#include "a.h"

Chan c1;

int
main(int argc, char **argv)
{
	int fail;
	Chan c2;
	void *v;

	fail = 0;
	#define test(a)	do{ \
		if((a)){ \
			fprintf(stderr, "fail: (%s) -- a chan procedure"  \
				 " completed despite being called on a" \
				 " closed channel\n", #a); \
			 fail = 1; \
		} \
	}while(0);
	chanclose(&c1);
	chanclose(&c2);
	test(chansend(&c1, v) >= 0);
	test(chansend(&c2, v) >= 0);
	chansendp(&c1, v);
	chansendp(&c2, v);
	test(channbsend(&c1, v) >= 0);
	test(channbsend(&c2, v) >= 0);
	test(chanrecv(&c1, v) >= 0);
	test(chanrecv(&c2, v) >= 0);
	test(channbrecv(&c1, v) >= 0);
	test(channbrecv(&c2, v) >= 0);
	test(chanrecvp(&c1) != nil);
	test(chanrecvp(&c2) != nil);
	#undef test
	return fail;
}
