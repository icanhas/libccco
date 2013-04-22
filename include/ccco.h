typedef struct Chan	Chan;
typedef struct Alt	Alt;
typedef struct Proc	Proc;
typedef struct _Lock	Lock;
typedef struct _Thread	Thread;
typedef struct _Cond	Cond;

struct Chan {
	Lock*		l;
	unsigned short	status;	/* open or not (dat.h) */
	Cond*		da;	/* data became available in buf */
	Cond*		sa;	/* recver wants to proceed, or just finished recving */
	Cond*		sc;	/* send complete */
	unsigned char*	b;	/* the buffer */
	volatile long	elsz;	/* size of an elem in buf */
	volatile long	sz;	/* sz*elsz == sizeof b */
	volatile long	s;	/* start */
	volatile long	n;	/* no. elems in buf */
	volatile int	u;	/* used in unbuffered ops only */
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
void	procsrand(unsigned long);
unsigned long	procrand(void);
Chan*	chancreate(long, long);
void	chanfree(Chan*);
int	chanopen(Chan*, long, long);
void	chanclose(Chan*);
void	chanalt(Alt*);
int	chanrecv(Chan*, void*);
void*	chanrecvp(Chan*);
int	channbrecv(Chan*, void*);
void*	channbrecvp(Chan*);
int	chansend(Chan*, void*);
int	chansendp(Chan*, void*);
int	channbsend(Chan*, void*);
int	channbsendp(Chan*, void*);
