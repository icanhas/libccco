#include <csp.h>
#include <stdio.h>
#include <unistd.h>
#include "a.h"

enum { Nproc = 1000 };

static int fail;
static Chan *ch;

static void
f(void *arg)
{
	int n;
	
	n = *(int*)arg;
	if(chansend(ch, &n) != 1)
		fail = 1;
	procexit();
	fail = 1;
	printf("proc %d still alive after procexit\n", n);
}

int
main(void)
{
	int i, nprocs, procn[Nproc];
	
	fail = 0;
	nprocs = 0;
	ch = chancreate(1000, sizeof(int));
	if(ch == nil){
		fail = 1;
		printf("chancreate failed\n");
	}
		
	for(i = 0; i < Nproc; ++i){
		procn[i] = i + 1;
		if(proccreate(f, &procn[i], 0) == nil){
			fail = 1;
			printf("proccreate failed\n");
		}else{
			nprocs++;
		}
	}
	
	for(i = 0; i < nprocs; ++i){
		int n;
		
		if(chanrecv(ch, &n) != 1){
			fail = 1;
			printf("recv error\n");
		}
	}
	chanfree(ch);
	return fail;
}
