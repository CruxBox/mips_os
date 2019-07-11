#ifndef _THREADPRIVATE_H_
#define _THREADPRIVATE_H_

struct thread;		/* from <thread.h> */
struct thread_machdep;	/* from <machine/thread.h> */
struct switchframe;	/* from <machine/switchframe.h> */


/*
 * Subsystem-private thread defs.
 *
 * This file is to be used only by the thread subsystem. However, it
 * has to be placed in the public include directory (rather than the
 * threads directory) so the machine-dependent thread code can include
 * it. This is one of the drawbacks of putting all machine-dependent
 * material in a single directory: it exposes what ought to be private
 * interfaces.
 */


/*
 * Private thread functions.
 */

/* Entry point for new threads. */
void thread_startup(void (*entrypoint)(void *data1, unsigned long data2),
		    void *data1, unsigned long data2);

/* Initialize or clean up the machine-dependent portion of struct thread */
void thread_machdep_init(struct thread_machdep *tm);
void thread_machdep_cleanup(struct thread_machdep *tm);

/*
 * Machine-dependent functions for working on switchframes.
 *
 * Note that while the functions themselves are machine-dependent, their
 * declarations are not.
 */

/* Assembler-level context switch. */
void switchframe_switch(struct switchframe **prev, struct switchframe **next);

/* Thread initialization */
void switchframe_init(struct thread *,
		      void (*entrypoint)(void *data1, unsigned long data2),
		      void *data1, unsigned long data2);


#endif /* _THREADPRIVATE_H_ */
