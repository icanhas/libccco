#include <string.h>
#include "csp.h"
#include "dat.h"
#include "fn.h"

int
chansend(Chan *c, void *p)
{
	return _send(c, p, 1);
}
