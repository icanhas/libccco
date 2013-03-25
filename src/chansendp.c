#include "ccco.h"
#include "dat.h"
#include "fn.h"

int
chansendp(Chan *c, void *p)
{
	return _send(c, (void*)&p, 1);
}
