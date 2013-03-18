#include <assert.h>
#include <csp.h>
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
	size_t i;
	int j;
	ulong r;
	Chan *done;
	
	procsetname("testrand");
	done = arg;
	for(i = 0; i < alen(tests); i++){
		procsrand(tests[i].seed);
		for(j = 0; j < Nrand; j++){
			r = procrand();
			if(r != tests[i].expect[j]){
				printf("fail: (seed %lu, j %d) -- expected %lu got %lu\n", tests[i].seed,
					j, tests[i].expect[j], r);
			}
		}
	}
	chansend(done, &(int){1});
}

int
main(int argc, char **argv)
{
	Proc procs[Nproc];
	Chan ch;
	int i;
	
	assert(chanopen(&ch, 0, sizeof(int)) == 0);
	for(i = 0; i < Nproc; ++i){
		assert(procinit(&procs[i], testrand, &ch, 0) == 0);
	}
	for(i = 0; i < Nproc; ++i){
		int n;
		
		chanrecv(&ch, &n);
	}
	for(i = 0; i < Nproc; ++i)
		prockill(&procs[i]);
	chanclose(&ch);
	fflush(stdout);
	return 0;
}
