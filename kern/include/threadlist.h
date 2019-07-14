#ifndef _THREADLIST_H_
#define _THREADLIST_H_


struct thread;	/* from <thread.h> */

/*
 * AmigaOS-style linked list of threads.
 *
 * The two threadlistnodes in the threadlist structure are always on
 * the list, as bookends; this removes all the special cases in the
 * list handling code. However, this means that iterating starts with
 * the "second" element in the list (tl_head.tln_next, or
 * tl_tail.tln_prev) and it ends at the last element that's actually a
 * thread.
 *
 * Note that this means that assigning or memcpying threadlist
 * structures will break them. Don't do that...
 *
 * ->tln_self always points to the thread that contains the
 * threadlistnode. We could avoid this if we wanted to instead use
 *
 *    (struct thread *)((char *)node - offsetof(struct thread, t_listnode))
 *
 * to get the thread pointer. But that's gross.
 */

struct threadlistnode {
	struct threadlistnode *tln_prev;
	struct threadlistnode *tln_next;
	struct thread *tln_self;
};

struct threadlist {
	struct threadlistnode tl_head;
	struct threadlistnode tl_tail;
	unsigned tl_count;
};

/* Initialize and clean up a thread list node. */
void threadlistnode_init(struct threadlistnode *tln, struct thread *self);
void threadlistnode_cleanup(struct threadlistnode *tln);

/* Initialize and clean up a thread list. Must be empty at cleanup. */
void threadlist_init(struct threadlist *tl);
void threadlist_cleanup(struct threadlist *tl);

/* Check if it's empty */
bool threadlist_isempty(struct threadlist *tl);

/* Add and remove: at ends */
void threadlist_addhead(struct threadlist *tl, struct thread *t);
void threadlist_addtail(struct threadlist *tl, struct thread *t);
struct thread *threadlist_remhead(struct threadlist *tl);
struct thread *threadlist_remtail(struct threadlist *tl);

/* Add and remove: in middle. (TL is needed to maintain ->tl_count.) */
void threadlist_insertafter(struct threadlist *tl,
			    struct thread *onlist, struct thread *addee);
void threadlist_insertbefore(struct threadlist *tl,
			     struct thread *addee, struct thread *onlist);
void threadlist_remove(struct threadlist *tl, struct thread *t);

/* Iteration; itervar should previously be declared as (struct thread *) */
#define THREADLIST_FORALL(itervar, tl) \
	for ((itervar) = (tl).tl_head.tln_next->tln_self; \
	     (itervar) != NULL; \
	     (itervar) = (itervar)->t_listnode.tln_next->tln_self)

#define THREADLIST_FORALL_REV(itervar, tl) \
	for ((itervar) = (tl).tl_tail.tln_prev->tln_self; \
	     (itervar) != NULL; \
	     (itervar) = (itervar)->t_listnode.tln_prev->tln_self)


#endif /* _THREADLIST_H_ */
