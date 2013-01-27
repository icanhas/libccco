#define nil 0

typedef unsigned char	uchar;
typedef unsigned short	ushort;
typedef unsigned int	uint;
typedef unsigned long	ulong;

typedef enum Chanop {
	End,
	Send,
	Recv,
	Nop,
	Noblk
} Chanop;	/* mimics Plan 9 channel ops */
