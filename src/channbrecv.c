#include "csp.h"
#include "dat.h"
#include "fn.h"

int
channbrecv(Chan *c, void *p)
{
	return _recv(c, p, 0);
}
