/* util.c */
void	errorf(const char*, ...);
void*	emalloc(size_t);

/*
 * Platform-dependent things
 */
Thread*	createthread(void (*)(void*), void*, size_t);
void	killthread(Thread*);
int	initlock(Lock*);
int	destroylock(Lock*);
int	lock(Lock*);
int	unlock(Lock*);
