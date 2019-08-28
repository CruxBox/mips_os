#ifndef _KERN_FCNTL_H_
#define _KERN_FCNTL_H_

/*
 * Constants for libc's <fcntl.h>.
 */


/*
 * Important
 */

/* Flags for open: choose one of these: */
#define O_RDONLY      0      /* Open for read */
#define O_WRONLY      1      /* Open for write */
#define O_RDWR        2      /* Open for read and write */
/* then or in any of these: */
#define O_CREAT       4      /* Create file if it doesn't exist */
#define O_EXCL        8      /* With O_CREAT, fail if file already exists */
#define O_TRUNC      16      /* Truncate file upon open */
#define O_APPEND     32      /* All writes happen at EOF (optional feature) */
#define O_NOCTTY     64      /* Required by POSIX, != 0, but does nothing */

/* Additional related definition */
#define O_ACCMODE     3      /* mask for O_RDONLY/O_WRONLY/O_RDWR */

/*
 * Not so important
 */

/* operation codes for flock() */
#define LOCK_SH         1       /* shared lock */
#define LOCK_EX         2       /* exclusive lock */
#define LOCK_UN         3       /* release the lock */
#define LOCK_NB         4       /* flag: don't block */

/*
 * Mostly pretty useless
 */

/* fcntl() operations */
#define F_DUPFD         0       /* like dup() but not quite */
#define F_GETFD         1       /* get per-handle flags */
#define F_SETFD         2       /* set per-handle flags */
#define F_GETFL         3       /* get per-file flags (O_* open flags) */
#define F_SETFL         4       /* set per-file flags (O_* open flags) */
#define F_GETOWN        5       /* get process/pgroup for SIGURG and SIGIO */
#define F_SETOWN        6       /* set process/pgroup for SIGURG and SIGIO */
#define F_GETLK         7       /* inspect record locks */
#define F_SETLK         8       /* acquire record locks nonblocking */
#define F_SETLKW        9       /* acquire record locks and wait */

/* flag for F_GETFD and F_SETFD */
#define FD_CLOEXEC      1       /* close-on-exec */

/* modes for fcntl (F_GETLK/SETLK) locking */
#define F_RDLCK         0       /* shared lock */
#define F_WRLCK         1       /* exclusive lock */
#define F_UNLCK         2       /* unlock */

/* struct for fcntl (F_GETLK/SETLK) locking */
struct flock {
	off_t l_start;          /* place in file */
	int l_whence;           /* SEEK_SET, SEEK_CUR, or SEEK_END */
	int l_type;             /* F_RDLCK or F_WRLCK */
	off_t l_len;            /* length of locked region */
	pid_t l_pid;            /* process that holds the lock */
};


#endif /* _KERN_FCNTL_H_ */
