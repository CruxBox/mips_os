#define SPINLOCK_INLINE   /* empty */
#define MEMBAR_INLINE     /* empty */

#include <types.h>
#include <lib.h>
#include <cpu.h>
#include <spl.h>
#include <spinlock.h>
#include <membar.h>
#include <current.h>	/* for curcpu */

/*
 * Spinlocks.
 */


/*
 * Initialize spinlock.
 */
void
spinlock_init(struct spinlock *splk)
{
	spinlock_data_set(&splk->splk_lock, 0);
	splk->splk_holder = NULL;
	HANGMAN_LOCKABLEINIT(&splk->splk_hangman, "spinlock");
}

/*
 * Clean up spinlock.
 */
void
spinlock_cleanup(struct spinlock *splk)
{
	KASSERT(splk->splk_holder == NULL);
	KASSERT(spinlock_data_get(&splk->splk_lock) == 0);
}

/*
 * Get the lock.
 *
 * First disable interrupts (otherwise, if we get a timer interrupt we
 * might come back to this lock and deadlock), then use a machine-level
 * atomic operation to wait for the lock to be free.
 */
void
spinlock_acquire(struct spinlock *splk)
{
	struct cpu *mycpu;

	splraise(IPL_NONE, IPL_HIGH);

	/* this must work before curcpu initialization */
	if (CURCPU_EXISTS()) {
		mycpu = curcpu->c_self;
		if (splk->splk_holder == mycpu) {
			panic("Deadlock on spinlock %p\n", splk);
		}
		mycpu->c_spinlocks++;

		HANGMAN_WAIT(&curcpu->c_hangman, &splk->splk_hangman);
	}
	else {
		mycpu = NULL;
	}

	while (1) {
		/*
		 * Do test-test-and-set, that is, read first before
		 * doing test-and-set, to reduce bus contention.
		 *
		 * Test-and-set is a machine-level atomic operation
		 * that writes 1 into the lock word and returns the
		 * previous value. If that value was 0, the lock was
		 * previously unheld and we now own it. If it was 1,
		 * we don't.
		 */
		if (spinlock_data_get(&splk->splk_lock) != 0) {
			continue;
		}
		if (spinlock_data_testandset(&splk->splk_lock) != 0) {
			continue;
		}
		break;
	}

	membar_store_any();
	splk->splk_holder = mycpu;

	if (CURCPU_EXISTS()) {
		HANGMAN_ACQUIRE(&curcpu->c_hangman, &splk->splk_hangman);
	}
}

/*
 * Release the lock.
 */
void
spinlock_release(struct spinlock *splk)
{
	/* this must work before curcpu initialization */
	if (CURCPU_EXISTS()) {
		KASSERT(splk->splk_holder == curcpu->c_self);
		KASSERT(curcpu->c_spinlocks > 0);
		curcpu->c_spinlocks--;
		HANGMAN_RELEASE(&curcpu->c_hangman, &splk->splk_hangman);
	}

	splk->splk_holder = NULL;
	membar_any_store();
	spinlock_data_set(&splk->splk_lock, 0);
	spllower(IPL_HIGH, IPL_NONE);
}

/*
 * Check if the current cpu holds the lock.
 */
bool
spinlock_do_i_hold(struct spinlock *splk)
{
	if (!CURCPU_EXISTS()) {
		return true;
	}

	/* Assume we can read splk_holder atomically enough for this to work */
	return (splk->splk_holder == curcpu->c_self);
}
