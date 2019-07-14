/*
 * Synchronization primitives.
 * The specifications of the functions are in synch.h.
 */

#include <types.h>
#include <lib.h>
#include <spinlock.h>
#include <wchan.h>
#include <thread.h>
#include <current.h>
#include <synch.h>

////////////////////////////////////////////////////////////
//
// Semaphore.

struct semaphore *
sem_create(const char *name, unsigned initial_count)
{
	struct semaphore *sem;

	sem = kmalloc(sizeof(*sem));
	if (sem == NULL)
	{
		return NULL;
	}

	sem->sem_name = kstrdup(name);
	if (sem->sem_name == NULL)
	{
		kfree(sem);
		return NULL;
	}

	sem->sem_wchan = wchan_create(sem->sem_name);
	if (sem->sem_wchan == NULL)
	{
		kfree(sem->sem_name);
		kfree(sem);
		return NULL;
	}

	spinlock_init(&sem->sem_lock);
	sem->sem_count = initial_count;

	return sem;
}

void sem_destroy(struct semaphore *sem)
{
	KASSERT(sem != NULL);

	/* wchan_cleanup will assert if anyone's waiting on it */
	spinlock_cleanup(&sem->sem_lock);
	wchan_destroy(sem->sem_wchan);
	kfree(sem->sem_name);
	kfree(sem);
}

void P(struct semaphore *sem)
{
	KASSERT(sem != NULL);

	/*
	 * May not block in an interrupt handler.
	 *
	 * For robustness, always check, even if we can actually
	 * complete the P without blocking.
	 */
	KASSERT(curthread->t_in_interrupt == false);

	/* Use the semaphore spinlock to protect the wchan as well. */
	spinlock_acquire(&sem->sem_lock);
	while (sem->sem_count == 0)
	{
		/*
		 *
		 * Note that we don't maintain strict FIFO ordering of
		 * threads going through the semaphore; that is, we
		 * might "get" it on the first try even if other
		 * threads are waiting. Apparently according to some
		 * textbooks semaphores must for some reason have
		 * strict ordering. Too bad. :-)
		 *
		 * Exercise: how would you implement strict FIFO
		 * ordering?
		 */
		wchan_sleep(sem->sem_wchan, &sem->sem_lock);
	}
	KASSERT(sem->sem_count > 0);
	sem->sem_count--;
	spinlock_release(&sem->sem_lock);
}

void V(struct semaphore *sem)
{
	KASSERT(sem != NULL);

	spinlock_acquire(&sem->sem_lock);

	sem->sem_count++;
	KASSERT(sem->sem_count > 0);
	wchan_wakeone(sem->sem_wchan, &sem->sem_lock);

	spinlock_release(&sem->sem_lock);
}

////////////////////////////////////////////////////////////
//
// Lock.

struct lock *
lock_create(const char *name)
{
	struct lock *lock;

	lock = kmalloc(sizeof(*lock));
	if (lock == NULL)
	{
		return NULL;
	}

	lock->lk_name = kstrdup(name);
	if (lock->lk_name == NULL)
	{
		kfree(lock);
		return NULL;
	}

	lock->holder = NULL;

	HANGMAN_LOCKABLEINIT(&lock->lk_hangman, lock->lk_name);

	// add stuff here as needed

	lock->lock_wchan = wchan_create(lock->lk_name);
	if (lock->lock_wchan == NULL)
	{
		kfree(lock->lk_name);
		kfree(lock);
	}

	spinlock_init(&lock->spinlock_wchan);
	lock->available = 1;

	return lock;
}

void lock_destroy(struct lock *lock)
{
	KASSERT(lock != NULL);

	// add stuff here as needed
	spinlock_cleanup(&lock->spinlock_wchan);
	wchan_destroy(lock->lock_wchan);
	kfree(lock->lk_name);
	kfree(lock);
}

void lock_acquire(struct lock *lock)
{
	/* Call this (atomically) before waiting for a lock */
	HANGMAN_WAIT(&curthread->t_hangman, &lock->lk_hangman);

	// Write this
	KASSERT(lock != NULL);
	KASSERT(curthread->t_in_interrupt == false);

	if (lock->holder == curthread)
		return;

	spinlock_acquire(&lock->spinlock_wchan);
	while (lock->available != 1)
	{
		wchan_sleep(lock->lock_wchan, &lock->spinlock_wchan);
	}

	KASSERT(lock->holder == NULL);
	KASSERT(lock->available == 1);

	lock->holder = curthread;
	lock->available = 0;
	KASSERT(lock->holder != NULL);

	spinlock_release(&lock->spinlock_wchan);

	HANGMAN_ACQUIRE(&curthread->t_hangman, &lock->lk_hangman);
	return;
	/* Call this (atomically) once the lock is acquired */
}

void lock_release(struct lock *lock)
{
	/* Call this (atomically) when the lock is released */

	// Write this
	KASSERT(lock != NULL);

	if (lock_do_i_hold(lock))
	{
		KASSERT(lock->holder != NULL);

		lock->holder = NULL;
		lock->available = 1;

		//wake a thread from sleep
		wchan_wakeone(lock->lock_wchan, &lock->spinlock_wchan);
	}

	HANGMAN_RELEASE(&curthread->t_hangman, &lock->lk_hangman);
}

int lock_do_i_hold(struct lock *lock)
{
	return (lock->holder == curthread);
}

////////////////////////////////////////////////////////////
//
// CV

struct cv *
cv_create(const char *name)
{
	struct cv *cv;

	cv = kmalloc(sizeof(*cv));
	if (cv == NULL)
	{
		return NULL;
	}

	cv->cv_name = kstrdup(name);
	if (cv->cv_name == NULL)
	{
		kfree(cv);
		return NULL;
	}

	// add stuff here as needed

	return cv;
}

void cv_destroy(struct cv *cv)
{
	KASSERT(cv != NULL);

	// add stuff here as needed

	kfree(cv->cv_name);
	kfree(cv);
}

void cv_wait(struct cv *cv, struct lock *lock)
{
	// Write this
	(void)cv;   // suppress warning until code gets written
	(void)lock; // suppress warning until code gets written
}

void cv_signal(struct cv *cv, struct lock *lock)
{
	// Write this
	(void)cv;   // suppress warning until code gets written
	(void)lock; // suppress warning until code gets written
}

void cv_broadcast(struct cv *cv, struct lock *lock)
{
	// Write this
	(void)cv;   // suppress warning until code gets written
	(void)lock; // suppress warning until code gets written
}
