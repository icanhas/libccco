/* util.c */
void	errorf(const char*, ...);
void*	emalloc(size_t);

/*
 * Platform-dependent things
 * (Win|Pthread).c
 */
Thread*	createthread(void (*)(void*), void*, int);
void	freethread(Thread*);
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
