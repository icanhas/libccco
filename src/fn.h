#define dprintf	errorf

/*
 * rand.c
 */
void	initrand(Rand*);
void	destroyrand(Rand*);
void	_srand(Rand*, ulong);
ulong	_rand(Rand*);

/* 
 * sendrecv.c 
 */
int	_send(Chan*, void*, int);
int	_recv(Chan*, void*, int);

/* 
 * util.c 
 */
void	errorf(const char*, ...);
void*	emalloc();

/*
 * Pthread.c
 * Windows.c
 * C11.c
 */
Thread*	createthread(void (*)(void*), void*, int);
void	freethread(Thread*);
void	exitthread();
void	setthreadname(const char*);
char*	getthreadname(void);
Lock*	createlock(void);
void	freelock(Lock*);
int	initlock(Lock*);
void	destroylock(Lock*);
int	lock(Lock*, int);
int	unlock(Lock*);
Cond*	createcond(void);
void	freecond(Cond*);
int	initcond(Cond*);
void	destroycond(Cond*);
int	wait(Cond*, Lock*);
int	csignal(Cond*);
void	_procsleep(long);
void	_stlrand(ulong);
ulong	_tlrand(void);

