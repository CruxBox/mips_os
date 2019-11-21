#ifndef _PROC_H_
#define _PROC_H_

/*
 * Definition of a process.
 *
 * Note: curproc is defined by <current.h>.
 */

#include <spinlock.h>

struct addrspace;
struct thread;
struct vnode;

/*
 * Process structure.
 *
 * Note that we only count the number of threads in each process.
 * (And, unless you implement multithreaded user processes, this
 * number will not exceed 1 except in kproc.) If you want to know
 * exactly which threads are in the process, e.g. for debugging, add
 * an array and a sleeplock to protect it. (You can't use a spinlock
 * to protect an array because arrays need to be able to call
 * kmalloc.)
 *
 * You will most likely be adding stuff to this structure, so you may
 * find you need a sleeplock in here for other reasons as well.
 * However, note that p_addrspace must be protected by a spinlock:
 * thread_switch needs to be able to fetch the current address space
 * without sleeping.
 */

#define MAXHANDLES 128


struct file_handle_node{
  
	struct rwlock *lock;
  char* seeker;
  struct vnode* file_object;
	
};

struct proc
{
	char *p_name;			/* Name of this process */
	struct spinlock p_lock; /* Lock for this structure */
	unsigned p_numthreads;  /* Number of threads in this process */

	/* VM */
	struct addrspace *p_addrspace; /* virtual address space */

	/* VFS */
	struct vnode *p_cwd; /* current working directory */

	volatile bool exited;

  // adding file handler array aka file table
  struct file_handle_node file_table[MAXHANDLES];
};

/* This is the process structure for the kernel and for kernel-only threads. */
extern struct proc *kproc;

/* Call once during system startup to allocate data structures. */
void proc_bootstrap(void);

/* get the free file descriptor in the process. Returns -1 if no fd found. */
int get_fd(struct proc *proc);

/* Create a fresh process for use by runprogram(). */
struct proc *proc_create_runprogram(const char *name);

/* Destroy a process. */
void proc_destroy(struct proc *proc);

/* Attach a thread to a process. Must not already have a process. */
int proc_addthread(struct proc *proc, struct thread *t);

/* Detach a thread from its process. */
void proc_remthread(struct thread *t);

/* Fetch the address space of the current process. */
struct addrspace *proc_getas(void);

/* Change the address space of the current process, and return the old one. */
struct addrspace *proc_setas(struct addrspace *);

#endif /* _PROC_H_ */
