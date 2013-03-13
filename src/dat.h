#define nil 0

typedef unsigned char	uchar;
typedef unsigned short	ushort;
typedef unsigned int	uint;
typedef unsigned long	ulong;
typedef struct Rand	Rand;

struct Rand {
	ulong *mt;
	ulong sz;
	ulong i;
};

enum {
	Closed,
	Open=	0xccc0
} Chanstatus;
