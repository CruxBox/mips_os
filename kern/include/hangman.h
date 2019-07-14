
#ifndef HANGMAN_H
#define HANGMAN_H

/*
 * Simple deadlock detector. Enable with "options hangman" in the
 * kernel config.
 */

#include "opt-hangman.h"

#if OPT_HANGMAN

struct hangman_actor {
	const char *a_name;
	const struct hangman_lockable *a_waiting;
};

struct hangman_lockable {
	const char *l_name;
	const struct hangman_actor *l_holding;
};

void hangman_wait(struct hangman_actor *a, struct hangman_lockable *l);
void hangman_acquire(struct hangman_actor *a, struct hangman_lockable *l);
void hangman_release(struct hangman_actor *a, struct hangman_lockable *l);

#define HANGMAN_ACTOR(sym)	struct hangman_actor sym
#define HANGMAN_LOCKABLE(sym)	struct hangman_lockable sym

#define HANGMAN_ACTORINIT(a, n)	    ((a)->a_name = (n), (a)->a_waiting = NULL)
#define HANGMAN_LOCKABLEINIT(l, n)  ((l)->l_name = (n), (l)->l_holding = NULL)

#define HANGMAN_LOCKABLE_INITIALIZER	{ "spinlock", NULL }

#define HANGMAN_WAIT(a, l)	hangman_wait(a, l)
#define HANGMAN_ACQUIRE(a, l)	hangman_acquire(a, l)
#define HANGMAN_RELEASE(a, l)	hangman_release(a, l)

#else

#define HANGMAN_ACTOR(sym)
#define HANGMAN_LOCKABLE(sym)

#define HANGMAN_ACTORINIT(a, name)
#define HANGMAN_LOCKABLEINIT(a, name)

#define HANGMAN_LOCKABLE_INITIALIZER

#define HANGMAN_WAIT(a, l)
#define HANGMAN_ACQUIRE(a, l)
#define HANGMAN_RELEASE(a, l)

#endif

#endif /* HANGMAN_H */
