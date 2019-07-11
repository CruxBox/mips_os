#ifndef _KERN_SIGNAL_H_
#define _KERN_SIGNAL_H_

/*
 * Machine-independent definitions for signals.
 */


/*
 * The signals.
 *
 * The values of many of these are "well known", particularly 1, 9,
 * 10, and 11.
 *
 * Note that Unix signals are a semantic cesspool; many have special
 * properties or are supposed to interact with the system in special
 * ways. It is gross.
 */

#define SIGHUP		1	/* Hangup */
#define SIGINT		2	/* Interrupt (^C) */
#define SIGQUIT		3	/* Quit (typically ^\) */
#define SIGILL		4	/* Illegal instruction */
#define SIGTRAP		5	/* Breakpoint trap */
#define SIGABRT		6	/* abort() call */
#define SIGEMT		7	/* Emulator trap */
#define SIGFPE		8	/* Floating point exception */
#define SIGKILL		9	/* Hard kill (unblockable) */
#define SIGBUS		10	/* Bus error, typically bad pointer alignment*/
#define SIGSEGV		11	/* Segmentation fault */
#define SIGSYS		12	/* Bad system call */
#define SIGPIPE		13	/* Broken pipe */
#define SIGALRM		14	/* alarm() expired */
#define SIGTERM		15	/* Termination requested (default kill) */
#define SIGURG		16	/* Urgent data on socket */
#define SIGSTOP		17	/* Hard process stop (unblockable) */
#define SIGTSTP		18	/* Terminal stop (^Z) */
#define SIGCONT		19	/* Time to continue after stop */
#define SIGCHLD		20	/* Child process exited */
#define SIGTTIN		21	/* Stop on tty read while in background */
#define SIGTTOU		22	/* Stop on tty write while in background */
#define SIGIO		23	/* Nonblocking or async I/O is now ready */
#define SIGXCPU		24	/* CPU time resource limit exceeded */
#define SIGXFSZ		25	/* File size resource limit exceeded */
#define SIGVTALRM	26	/* Like SIGALRM but in virtual time */
#define SIGPROF		27	/* Profiling timer */
#define SIGWINCH	28	/* Window size change on tty */
#define SIGINFO		29	/* Information request (typically ^T) */
#define SIGUSR1		20	/* Application-defined */
#define SIGUSR2		31	/* Application-defined */
#define SIGPWR		32	/* Power failure */
#define _NSIG		32


/* Type for a set of signals; used by e.g. sigprocmask(). */
typedef __u32 sigset_t;

/* flags for sigaction.sa_flags */
#define SA_ONSTACK	1	/* Use sigaltstack() stack. */
#define SA_RESTART	2	/* Restart syscall instead of interrupting. */
#define SA_RESETHAND	4	/* Clear handler after one usage. */

/* codes for sigprocmask() */
#define SIG_BLOCK	1	/* Block selected signals. */
#define SIG_UNBLOCK	2	/* Unblock selected signals. */
#define SIG_SETMASK	3	/* Set mask to the selected signals. */

/* Type for a signal handler function. */
typedef void (*__sigfunc)(int);

/* Magic values for signal handlers. */
#define SIG_DFL		((__sigfunc) 0)		/* Default behavior. */
#define SIG_IGN		((__sigfunc) 1)		/* Ignore the signal. */

/*
 * Struct for sigaction().
 */
struct sigaction {
	__sigfunc sa_handler;
	sigset_t sa_mask;
	unsigned sa_flags;
};

/*
 * Struct for sigaltstack().
 * (not very important)
 */
struct sigaltstack {
	void *ss_sp;
	size_t ss_size;
	unsigned ss_flags;
};


#endif /* _KERN_SIGNAL_H_ */
