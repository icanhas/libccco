#define dprintf	errorf

/* sendrecv.c */
int	_send(Chan*, void*, int);
int	_recv(Chan*, void*, int);

/* util.c */
void	errorf(const char*, ...);
void*	emalloc();

/*
 * Platform-dependent things
 * (Win|Pthread).c
 */
Thread*	createthread(void (*)(void*), void*, int);
void	freethread(Thread*);
void	exitthread();
Lock*	createlock(void);
int	initlock(Lock*);
int	destroylock(Lock*);
int	lock(Lock*, int);
int	unlock(Lock*);
Cond*	createcond(void);
int	initcond(Cond*);
int	destroycond(Cond*);
int	wait(Cond*, Lock*);
int	signal(Cond*);
void	_procsleep(long);