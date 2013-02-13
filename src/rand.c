#include <assert.h>
#include <stdlib.h>
#include "csp.h"
#include "dat.h"
#include "fn.h"

#define Len		624
#define IA		397
#define twist(a, i, j)	(((a)[(i)] & 0x80000000) | ((a)[(j)] & 0x7fffffff))
#define magic(x)	(((x) & 1) * 0x9908b0df)

static void	gen(Rand*);

void
initrand(Rand *r, Lock *l)
{
	assert(r != nil);
	if(l != nil)
		lock(l, 1);
	r->sz = Len;
	r->mt = calloc(r->sz, sizeof(ulong));
	r->i = 0;
	if(l != nil)
		unlock(l);
}

void
destroyrand(Rand *r, Lock *l)
{
	assert(r != nil);
	assert(r->mt != nil);
	if(l != nil)
		lock(l, 1);
	r->sz = 0;
	free(r->mt);
	r->i = 0;
	if(l != nil)
		unlock(l);
}

void
_srand(Rand *r, ulong seed, Lock *l)
{
	ulong i;
	
	assert(r != nil);
	if(l != nil)
		lock(l, 1);
	r->mt[0] = seed;
	for(i = 1; i < r->sz; ++i){
		r->mt[i] = (0x41c64e6d * r->mt[i-1]) + 0x6073;
		r->mt[i] &= 0xffffffff;
	}
	r->i = 0;
	if(l != nil)
		unlock(l);
}

ulong
_rand(Rand *r, Lock *l)
{
	ulong y;
	
	assert(r != nil);
	if(l != nil)
		lock(l, 1);
	if(r->i >= Len-1)
		gen(r);
	y = r->mt[r->i];
	y ^= (y>>11);
	y ^= (y<<7) & 0x9d2c5680;
	y ^= (y<<15) & 0xefc60000;
	y ^= (y>>18);
	r->i++;
	r->i %= Len;
	if(l != nil)
		unlock(l);
	return y & 0xffffffff;
}

static void
gen(Rand *r)
{
	ulong i, y;
	
	r->i = 0;
	i = 0;
	for(; i < (Len - IA); ++i){
		y = twist(r->mt, i, i+1);
		r->mt[i] = r->mt[i + IA] ^ (y>>1) ^ magic(y);
	}
	for(; i < Len-1; ++i){
		y = twist(r->mt, i, i+1);
		r->mt[i] = r->mt[i - (Len - IA)] ^ (y>>1) ^ magic(y);
	}
	y = twist(r->mt, Len-1, 0);
	r->mt[Len-1] = r->mt[IA-1] ^ (y>>1) ^ magic(y);
}
