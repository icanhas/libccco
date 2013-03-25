#include <ccco.h>
#include <stdio.h>
#include "a.h"

enum { Nproc=	10 };

static int fail;
static Chan *ch, *done;

static void
sender(void *arg)
{
	int n, d;
	
	n = *(int*)arg;
	while(chansendp(ch, &n) == -1);
	while(chanrecv(done, &d) == -1);
	procexit();
}

int
main(void)
{
	int i, nprocs, procn[Nproc];
	
	fail = 0;
	nprocs = 0;
	ch = chancreate(Nproc, sizeof(void*));
	done = chancreate(0, sizeof(int));
	if(ch == nil){
		fail = 1;
		printf("fail: chancreate failed\n");
	}
		
	for(i = 0; i < Nproc; i++){
		void *v;
		
		procn[i] = i;
		if(proccreate(sender, &procn[i], 0) == nil){
			fail = 1;
			printf("fail: proccreate failed\n");
		}else{
			nprocs++;
		}
		v = chanrecvp(ch);
		if(v == nil){
			fail = 1;
			printf("fail: recv error\n");
		}
		if(*(int*)v != i){
			fail = 1;
			printf("fail: (incoherency) -- expected %d, got %d\n", i, *(long*)v);
		}
	}
	fflush(stdout);
	for(i = 0; i < nprocs; i++){
		int d = 1;
		chansend(done, &d);
	}
	chanfree(ch);
	chanfree(done);
	return fail;
}
