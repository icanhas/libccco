#include <assert.h>
#include <ccco.h>
#include <stdio.h>
#include "a.h"

enum {
	Nrand=	1000,
	Nproc=	4
};

struct {
	ulong	seed;
	ulong	expect[Nrand];
} tests[] = {
	{
		8191,
		{
		#include "nums8191.h"
		}
	}, {
		7747,
		{
		#include "nums7747.h"
		}
	}, {
		2147483647,
		{
		#include "nums2147483647.h"
		}
	}
};

void
testrand(void *arg)
{
	Chan *reply;
	size_t i;
	int j;
	ulong r;
	
	procsetname("testrand");
	reply = arg;
	for(i = 0; i < alen(tests); i++){
		procsrand(tests[i].seed);
		for(j = 0; j < Nrand; j++){
			r = procrand();
			if(r != tests[i].expect[j]){
				printf("fail: (seed %lu, j %d) -- expected %lu got %lu\n", tests[i].seed,
					j, tests[i].expect[j], r);
				chansend(reply, &(int){1});
			}
		}
	}
	chansend(reply, &(int){0});
}

int
main(int argc, char **argv)
{
	Proc procs[Nproc];
	Chan reply;
	int i, n, fail;
	
	assert(chanopen(&reply, 0, sizeof(int)) == 0);
	for(i = 0; i < Nproc; ++i){
		assert(procinit(&procs[i], testrand, &reply, 0) == 0);
	}
	fail = n = 0;
	for(i = 0; i < Nproc; ++i){
		chanrecv(&reply, &n);
		fail |= n;
	}
	for(i = 0; i < Nproc; ++i)
		prockill(&procs[i]);
	chanclose(&reply);
	fflush(stdout);
	return fail;
}
