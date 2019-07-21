/*
 * Driver code is in kern/tests/synchprobs.c We will
 * replace that file. This file is yours to modify as you see fit.
 *
 * You should implement your solution to the whalemating problem below.
 */

#include <types.h>
#include <lib.h>
#include <thread.h>
#include <test.h>
#include <synch.h>

/*
 * Called by the driver during initialization.
 */

struct semaphore *sem_male;
struct semaphore *sem_female;
struct cv *cv_male;
struct cv *cv_female;
struct lock *lock;

void whalemating_init()
{

	lock = lock_create('lock');

	sem_male = sem_create('sem_male', 0);
	sem_female = sem_create('sem_female', 0);

	cv_male = cv_create('cv_male');
	cv_female = cv_create('cv_female');

	return;
}

/*
 * Called by the driver during teardown.
 */

void whalemating_cleanup()
{

	lock_destroy(lock);

	sem_destroy(sem_male);
	sem_destroy(sem_female);

	cv_destroy(cv_male);
	cv_destroy(cv_female);

	return;
}

void male(uint32_t index)
{
	male_start(index);

	lock_acquire(lock);

	V(sem_male);
	cv_wait(cv_male, lock);

	lock_release(lock);

	male_end(index);

	return;
}

void female(uint32_t index)
{
	female_start(index);

	lock_acquire(lock);

	V(sem_female);

	cv_wait(cv_female, lock);

	lock_release(lock);

	female_end(index);

	return;
}

void matchmaker(uint32_t index)
{
	(void)index;
	matchmaker_start(index);

	P(sem_male);
	P(sem_female);

	lock_acquire(lock);

	cv_signal(cv_male, lock);
	cv_signal(cv_female, lock);

	lock_release(lock);

	matchmaker_end(index);

	return;
}
