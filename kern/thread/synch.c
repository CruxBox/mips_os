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
#include <spl.h>

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

	spinlock_init(&lock->lock_spinlock);
	// lock->available = 1;

	return lock;
}

void
 lock_destroy(struct lock *lock)
{
	KASSERT(lock != NULL);
	KASSERT(lock->holder == NULL);
	// KASSERT(lock->available == 1);

	// add stuff here as needed
	spinlock_cleanup(&lock->lock_spinlock);
	wchan_destroy(lock->lock_wchan);
	kfree(lock->lk_name);
	kfree(lock);
}

void
 lock_acquire(struct lock *lock)
{
	KASSERT(lock != NULL);

	/* Call this (atomically) before waiting for a lock */
	HANGMAN_WAIT(&curthread->t_hangman, &lock->lk_hangman);
	
	// KASSERT(curthread->t_in_interrupt == false); This caused an error when lock was being acquired while already in interrupt

	spinlock_acquire(&lock->lock_spinlock);

	struct wchan *waitlist = lock->lock_wchan;
	while (lock->holder != NULL)
	{
		wchan_sleep(waitlist, &lock->lock_spinlock);
	}

	KASSERT(lock->holder == NULL);

	lock->holder = curthread;
	/* Call this (atomically) once the lock is acquired */
	HANGMAN_ACQUIRE(&curthread->t_hangman, &lock->lk_hangman);

	KASSERT(lock->holder != NULL);

	spinlock_release(&lock->lock_spinlock);

	return;
}

void 
lock_release(struct lock *lock)
{

	// Write this
	KASSERT(lock != NULL);
	KASSERT(lock->holder != NULL);
	KASSERT(lock_do_i_hold(lock) == true);

	spinlock_acquire(&lock->lock_spinlock);
	
	KASSERT(lock->holder != NULL);
	lock->holder = NULL;

	//wake a thread from sleep
	wchan_wakeone(lock->lock_wchan, &lock->lock_spinlock);

	/* Call this (atomically) when the lock is released */
	HANGMAN_RELEASE(&curthread->t_hangman, &lock->lk_hangman);

	
	spinlock_release(&lock->lock_spinlock);

}

bool
 lock_do_i_hold(struct lock *lock)
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

	cv->wchan = wchan_create(cv->cv_name);

	spinlock_init(&cv->spinlock_wchan);

	return cv;
}

void
 cv_destroy(struct cv *cv)
{
	KASSERT(cv != NULL);

	wchan_destroy(cv->wchan);
	spinlock_cleanup(&cv->spinlock_wchan);
	kfree(cv->cv_name);
	kfree(cv);
}

void
 cv_wait(struct cv *cv, struct lock *lock)
{

	spinlock_acquire(&cv->spinlock_wchan);

	KASSERT(lock != NULL);
	KASSERT(lock_do_i_hold(lock) == true);

	struct wchan *waitlist = cv->wchan;

	lock_release(lock);

	wchan_sleep(waitlist, &cv->spinlock_wchan);

	spinlock_release(&cv->spinlock_wchan);

	lock_acquire(lock);
}

void 
cv_signal(struct cv *cv, struct lock *lock)
{

	spinlock_acquire(&cv->spinlock_wchan);

	KASSERT(lock_do_i_hold(lock) == true);

	wchan_wakeone(cv->wchan, &cv->spinlock_wchan);

	spinlock_release(&cv->spinlock_wchan);
}

void 
cv_broadcast(struct cv *cv, struct lock *lock)
{

	spinlock_acquire(&cv->spinlock_wchan);
	KASSERT(lock_do_i_hold(lock) == true);

	wchan_wakeall(cv->wchan, &cv->spinlock_wchan);

	spinlock_release(&cv->spinlock_wchan);
}


/*
	Utility functions for rqQueueItems
*/

void rwQueueItemDestroy(struct rwQueueItem** head) {
	if(head == NULL) return;
	if((*head) == NULL) return;
	struct rwQueueItem* next = NULL;
	if (*head != NULL) {
		next = (*head)->next;
		kfree(*head);
	}
	
	while(next != NULL){
		struct rwQueueItem* temp = next->next;
		kfree(next);
		next = temp;
	}
}


void rwQueueItemAdd(struct rwQueueItem** head, char entity) {
	if(head == NULL) return;
	struct rwQueueItem* newItem = kmalloc(sizeof(struct rwQueueItem));
	if(newItem == NULL) {
		panic("Memory not enough!\n");
		return;
	}
	newItem->entity = entity;
	newItem->next = NULL;
	if((*head) == NULL){
		*head = newItem;
	} else {
		struct rwQueueItem* temp = *head;
		while(temp->next != NULL){
			temp = temp->next;
		}
		temp->next = newItem;
	}
}


void rwQueueItemWake(struct rwlock* rw){
	struct rwQueueItem* temp = rw->lock_queue;
	struct rwQueueItem* head = temp;
	int count = 0;
	if (temp == NULL) return;
	char entity = temp->entity;
	if (entity == 'R') {	
		while((temp != NULL) && (temp->entity == entity)) {
			count++;
			temp = temp->next;
		}
		temp = head;
		while(count--){
			rw->lock_queue = temp->next;
			kfree(temp);
			wchan_wakeone(rw->lock_wchan, &rw->lock_spinlock);	
			temp = temp->next;
		}
	}
	else {
		count = 1;
		rw->lock_queue = temp->next;
		kfree(temp);
		wchan_wakeone(rw->lock_wchan, &rw->lock_spinlock);
	}
	kprintf("Found count: %d\n", count);
}


struct rwlock*
rwlock_create(const char* name)
{
	struct rwlock* rwlock_new;
	rwlock_new = kmalloc(sizeof(*rwlock_new));

	rwlock_new->rwlock_name = kstrdup(name);
	if(rwlock_new->rwlock_name == NULL){
		return NULL;
	}

	rwlock_new->numberOfReaders = 0;
	rwlock_new->isWriteMode = false;

	rwlock_new->lock_wchan = wchan_create(rwlock_new->rwlock_name);
	spinlock_init(&rwlock_new->lock_spinlock);
	rwlock_new->lock_queue = NULL;

	return rwlock_new;
}

void
rwlock_destroy (struct rwlock* rw)
{
	spinlock_cleanup(&rw->lock_spinlock);
	wchan_destroy(rw->lock_wchan);
	rwQueueItemDestroy(&rw->lock_queue);
	kfree(rw->rwlock_name);
	kfree(rw);
}

void
rwlock_acquire_read(struct rwlock* rw)
{
	KASSERT(rw != NULL);
	spinlock_acquire(&rw->lock_spinlock);
	bool isWchanEmpty = wchan_isempty(rw->lock_wchan, &rw->lock_spinlock);

	if((isWchanEmpty == true) && (rw->isWriteMode == false)) {
		rw->numberOfReaders+=1;
	}
	else{
		//add to lock_queue, R
		rwQueueItemAdd(&rw->lock_queue, 'R');
		wchan_sleep(rw->lock_wchan, &rw->lock_spinlock);
		KASSERT(rw->isWriteMode == false);
		rw->numberOfReaders++;
		// check for more readers that you can take from queue
		rwQueueItemWake(rw);
	}
	spinlock_release(&rw->lock_spinlock);
}

void
rwlock_release_read(struct rwlock* rw)
{	
	KASSERT(rw != NULL);
	spinlock_acquire(&rw->lock_spinlock);
	bool isWchanEmpty = wchan_isempty(rw->lock_wchan, &rw->lock_spinlock);
	rw->numberOfReaders -= 1;
	KASSERT(rw->numberOfReaders >= 0);
	
	if (rw->numberOfReaders == 0) {
		if(isWchanEmpty == false) {
			kprintf("Found!\n");
		rwQueueItemWake(rw);
			// check if queue has readers then wake them till you find a writer.
			// check if queue has writer, then simply wake it
		}
	}
	spinlock_release(&rw->lock_spinlock);

}

void
rwlock_acquire_write(struct rwlock* rw)
{
	KASSERT( rw != NULL);
	
	spinlock_acquire(&rw->lock_spinlock);
	bool isWchanEmpty = wchan_isempty(rw->lock_wchan, &rw->lock_spinlock);

	if((isWchanEmpty == true) && (rw->numberOfReaders == 0) && (rw->isWriteMode == false)) {
		rw->isWriteMode = true;
	}
	else{
		//add to lock_queue, W
		rwQueueItemAdd(&rw->lock_queue, 'W');
		wchan_sleep(rw->lock_wchan, &rw->lock_spinlock);
		KASSERT(rw->numberOfReaders == 0);
		rw->isWriteMode = true;
	}
	spinlock_release(&rw->lock_spinlock);

}

void
rwlock_release_write(struct rwlock* rw)
{
	KASSERT(rw != NULL);
	spinlock_acquire(&rw->lock_spinlock);
	bool isWchanEmpty = wchan_isempty(rw->lock_wchan, &rw->lock_spinlock);
	rw->isWriteMode = false;

	if(isWchanEmpty == false) {
		rwQueueItemWake(rw);
			// check if queue has readers then wake them till you find a writer.
			// check if queue has writer, then simply wake it
	}
	spinlock_release(&rw->lock_spinlock);
}
