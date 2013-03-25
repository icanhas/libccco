#include "ccco.h"
#include "dat.h"
#include "fn.h"

int
chanrecv(Chan *c, void *p)
{
	return _recv(c, p, 1);
}
