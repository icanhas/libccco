#define dprintf	errorf

/*
 * rand.c
 */
void	initrand(Rand*, Lock*);
void	destroyrand(Rand*, Lock*);
void	_srand(Rand*, ulong, Lock*);
ulong	_rand(Rand*, Lock*);

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
 * Platform-dependent parts
 * Pthread.c
 * Win.c
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
int	signal(Cond*);
void	_procsleep(long);
void	_stlrand(ulong);
ulong	_tlrand(void);

