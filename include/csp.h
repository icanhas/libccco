typedef struct Chan	Chan;
typedef struct Alt	Alt;
typedef struct Proc	Proc;
typedef struct _Lock	Lock;
typedef struct _Thread	Thread;
typedef struct _Cond	Cond;

struct Chan {
	Lock*	l;
	Cond*	avail;	/* data became available OR  space became available */
	unsigned char*	b;	/* the buffer */
	int	elsz;	/* size of elems in buf */
	int	sz;	/* sz*elsz == sizeof b */
	int	s;	/* start */
	int	n;	/* no. elems in buf */
};

struct Alt {
	Chan*	c;
};

struct Proc {
	Thread*	t;
};

Proc*	proccreate(void (*)(void*), void*, int);
void	procfree(Proc*);
int	procinit(Proc*, void (*)(void*), void*, int);
void	prockill(Proc*);
void	procsetname(const char*, ...);
char*	procgetname(void);
void	procsleep(long);
void	procexit(void);
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
