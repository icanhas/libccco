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
	int	elsz;	/* size of elems in channel */
	int	nel;	/* no. elems in channel */
	void*	b;	/* buffer */
};

struct Alt {
	Chan	*c;
};

struct Proc {
	Thread	*t;
	char*	name;
};

Proc*	proccreate(void (*fn)(void*), void*, int);
void	procfree(Proc*);
int	procinit(Proc*, void (*fn)(void*), void*, int);
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
