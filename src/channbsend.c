#include "ccco.h"
#include "dat.h"
#include "fn.h"

int
channbsend(Chan *c, void *p)
{
	_send(c, p, 0);
}
