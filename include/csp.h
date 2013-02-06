typedef struct Chan	Chan;
typedef struct Alt	Alt;
typedef struct Proc	Proc;
typedef struct _Lock	Lock;
typedef struct _Thread	Thread;
typedef struct _Cond	Cond;

struct Chan {
	Lock*	l;
	Cond*	full;	/* when nel == max-1, this is signaled */
	Cond*	flushed;
	int	isfull;
	int	elsz;	/* size of elems in buffer */
	int	max;	/* sizeof b = elsz * max */
	int	nel;	/* no. elems in buffer */
	char*	b;	/* the buffer */
};

struct Alt {
	Chan*	c;
};

struct Proc {
	Thread*	t;
	char*	name;
};

Proc*	proccreate(void (*)(void*), void*, int);
void	procfree(Proc*);
int	procinit(Proc*, void (*)(void*), void*, int);
void	prockill(Proc*);
Chan*	chancreate(int, int);
void	chanfree(Chan*);
void	chanalt(Alt*);
int	chanrecv(Chan*, void*);
void*	chanrecvp(Chan*);
int	channbrecv(Chan*, void*);
void*	channbrecvp(Chan*);
int	chansend(Chan*, void*);
int	chansendp(Chan*, void*);
int	channbsend(Chan*, void*);
int	channbsendp(Chan*, void*);
