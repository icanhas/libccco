typedef struct _Lock		Lock;
typedef struct _Thread	Thread;
typedef struct Chan		Chan;
typedef struct Alt		Alt;
typedef struct Proc		Proc;

typedef enum Chanop {
	Chanend,
	Chansnd,
	Chanrcv,
	Channop,
	Channoblk
} Chanop;	/* mimics Plan 9 channel ops */

struct Chan {
	Lock	*l;
	size_t	elsz;	/* size of elems in channel */
	size_t	nel;	/* no. elems in channel */
	void*	b;	/* buffer */
};

struct Alt {
	Chan	*c;
};

struct Proc {
	Thread	*t;
	void	(*fn)(void*);	/* proc 'main' */
	void*	arg;		/* proc args */
	char*	name;
};

Proc*	proccreate(void (*fn)(void*), void*, size_t);
int	procinit(Proc*, void (*fn)(void*), void*, size_t);
void	prockill(Proc*);
Chan*	chancreate(size_t, size_t);
void	chanfree(Chan*);
void	chanalt(Alt*);
size_t	chanrecv(Chan*, void*);
void*	chanrecvp(Chan*);
size_t	channbrecv(Chan*, void*);
void*	channbrecvp(Chan*);
size_t	chansend(Chan*, void*);
size_t	chansendp(Chan*, void*);
size_t	channbsend(Chan*, void*);
size_t	channbsendp(Chan*, void*);
