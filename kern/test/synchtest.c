/*
 * Synchronization test code.
 *
 * All the contents of this file are overwritten during automated
 * testing. Please consider this before changing anything in this file.
 */

#include <types.h>
#include <lib.h>
#include <clock.h>
#include <thread.h>
#include <synch.h>
#include <test.h>
#include <kern/test161.h>
#include <spinlock.h>

#define CREATELOOPS		8
#define NSEMLOOPS     63
#define NLOCKLOOPS    120
#define NCVLOOPS      5
#define NTHREADS      32
#define SYNCHTEST_YIELDER_MAX 16

static volatile unsigned long testval1;
static volatile unsigned long testval2;
static volatile unsigned long testval3;
static volatile int32_t testval4;

static struct semaphore *testsem = NULL;
static struct semaphore *testsem2 = NULL;
static struct lock *testlock = NULL;
static struct lock *testlock2 = NULL;
static struct cv *testcv = NULL;
static struct semaphore *donesem = NULL;

struct spinlock status_lock;
static bool test_status = TEST161_FAIL;

static unsigned long semtest_current;

static
bool
failif(bool condition) {
	if (condition) {
		spinlock_acquire(&status_lock);
		test_status = TEST161_FAIL;
		spinlock_release(&status_lock);
	}
	return condition;
}

static
void
semtestthread(void *junk, unsigned long num)
{
	(void)junk;

	int i;

	random_yielder(4);

	/*
	 * Only one of these should print at a time.
	 */
	P(testsem);
	semtest_current = num;

	kprintf_n("Thread %2lu: ", num);
	for (i=0; i<NSEMLOOPS; i++) {
		kprintf_t(".");
		kprintf_n("%2lu", num);
		random_yielder(4);
		failif((semtest_current != num));
	}
	kprintf_n("\n");

	V(donesem);
}

int
semtest(int nargs, char **args)
{
	(void)nargs;
	(void)args;

	int i, result;

	kprintf_n("Starting sem1...\n");
	for (i=0; i<CREATELOOPS; i++) {
		kprintf_t(".");
		testsem = sem_create("testsem", 2);
		if (testsem == NULL) {
			panic("sem1: sem_create failed\n");
		}
		donesem = sem_create("donesem", 0);
		if (donesem == NULL) {
			panic("sem1: sem_create failed\n");
		}
		if (i != CREATELOOPS - 1) {
			sem_destroy(testsem);
			sem_destroy(donesem);
		}
	}
	spinlock_init(&status_lock);
	test_status = TEST161_SUCCESS;

	kprintf_n("If this hangs, it's broken: ");
	P(testsem);
	P(testsem);
	kprintf_n("OK\n");
	kprintf_t(".");

	for (i=0; i<NTHREADS; i++) {
		kprintf_t(".");
		result = thread_fork("semtest", NULL, semtestthread, NULL, i);
		if (result) {
			panic("sem1: thread_fork failed: %s\n",
			      strerror(result));
		}
	}
	for (i=0; i<NTHREADS; i++) {
		kprintf_t(".");
		V(testsem);
		P(donesem);
	}

	sem_destroy(testsem);
	sem_destroy(donesem);
	testsem = donesem = NULL;

	kprintf_t("\n");
	success(test_status, SECRET, "sem1");

	return 0;
}

static
void
locktestthread(void *junk, unsigned long num)
{
	(void)junk;

	int i;

	for (i=0; i<NLOCKLOOPS; i++) {
		kprintf_t(".");
		KASSERT(!(lock_do_i_hold(testlock)));
		lock_acquire(testlock);
		KASSERT(lock_do_i_hold(testlock));
		random_yielder(4);

		testval1 = num;
		testval2 = num*num;
		testval3 = num%3;

		if (testval2 != testval1*testval1) {
			goto fail;
		}
		random_yielder(4);
		KASSERT(lock_do_i_hold(testlock));

		if (testval2%3 != (testval3*testval3)%3) {
			goto fail;
		}
		random_yielder(4);
		KASSERT(lock_do_i_hold(testlock));

		if (testval3 != testval1%3) {
			goto fail;
		}
		random_yielder(4);
		KASSERT(lock_do_i_hold(testlock));

		if (testval1 != num) {
			goto fail;
		}
		random_yielder(4);
		KASSERT(lock_do_i_hold(testlock));

		if (testval2 != num*num) {
			goto fail;
		}
		random_yielder(4);
		KASSERT(lock_do_i_hold(testlock));

		if (testval3 != num%3) {
			goto fail;
		}
		random_yielder(4);
		KASSERT(lock_do_i_hold(testlock));

		if (!(lock_do_i_hold(testlock))) {
			goto fail;
		}
		random_yielder(4);
		KASSERT(lock_do_i_hold(testlock));

		lock_release(testlock);
		KASSERT(!(lock_do_i_hold(testlock)));
	}

	/* Check for solutions that don't track ownership properly */

	for (i=0; i<NLOCKLOOPS; i++) {
		kprintf_t(".");
		if (lock_do_i_hold(testlock)) {
			goto fail2;
		}
	}

	V(donesem);
	return;

fail:
	lock_release(testlock);
fail2:
	failif(true);
	V(donesem);
	return;
}

int
locktest(int nargs, char **args)
{
	(void)nargs;
	(void)args;

	int i, result;

	kprintf_n("Starting lt1...\n");
	for (i=0; i<CREATELOOPS; i++) {
		kprintf_t(".");
		testlock = lock_create("testlock");
		if (testlock == NULL) {
			panic("lt1: lock_create failed\n");
		}
		donesem = sem_create("donesem", 0);
		if (donesem == NULL) {
			panic("lt1: sem_create failed\n");
		}
		if (i != CREATELOOPS - 1) {
			lock_destroy(testlock);
			sem_destroy(donesem);
		}
	}
	spinlock_init(&status_lock);
	test_status = TEST161_SUCCESS;

	for (i=0; i<NTHREADS; i++) {
		kprintf_t(".");
		result = thread_fork("synchtest", NULL, locktestthread, NULL, i);
		if (result) {
			panic("lt1: thread_fork failed: %s\n", strerror(result));
		}
	}
	for (i=0; i<NTHREADS; i++) {
		kprintf_t(".");
		P(donesem);
	}

	lock_destroy(testlock);
	sem_destroy(donesem);
	testlock = NULL;
	donesem = NULL;

	kprintf_t("\n");
	success(test_status, SECRET, "lt1");

	return 0;
}

/*
 * Note that the following tests that panic on success do minimal cleanup
 * afterward. This is to avoid causing a panic that could be unintentiontally
 * considered a success signal by test161. As a result, they leak memory,
 * don't destroy synchronization primitives, etc.
 */

int
locktest2(int nargs, char **args) {
	(void)nargs;
	(void)args;

	kprintf_n("Starting lt2...\n");
	kprintf_n("(This test panics on success!)\n");

	testlock = lock_create("testlock");
	if (testlock == NULL) {
		panic("lt2: lock_create failed\n");
	}

	secprintf(SECRET, "Should panic...", "lt2");
	lock_release(testlock);

	/* Should not get here on success. */

	success(TEST161_FAIL, SECRET, "lt2");

	/* Don't do anything that could panic. */

	testlock = NULL;
	return 0;
}

int
locktest3(int nargs, char **args) {
	(void)nargs;
	(void)args;

	kprintf_n("Starting lt3...\n");
	kprintf_n("(This test panics on success!)\n");

	testlock = lock_create("testlock");
	if (testlock == NULL) {
		panic("lt3: lock_create failed\n");
	}

	secprintf(SECRET, "Should panic...", "lt3");
	lock_acquire(testlock);
	lock_destroy(testlock);

	/* Should not get here on success. */

	success(TEST161_FAIL, SECRET, "lt3");

	/* Don't do anything that could panic. */

	testlock = NULL;
	return 0;
}

/*
 * Used by both lt4 and lt5 below. Simply acquires a lock in a separate
 * thread. Uses a semaphore as a barrier to make sure it gets the lock before
 * the driver completes.
 */

static
void
locktestacquirer(void * junk, unsigned long num)
{
  (void)junk;
	(void)num;

  lock_acquire(testlock);
  V(donesem);

	return;
}


int
locktest4(int nargs, char **args) {
  (void) nargs;
  (void) args;

	int result;

  kprintf_n("Starting lt4...\n");
  kprintf_n("(This test panics on success!)\n");

  testlock = lock_create("testlock");
  if (testlock == NULL) {
	 panic("lt4: lock_create failed\n");
  }

  donesem = sem_create("donesem", 0);
  if (donesem == NULL) {
	 lock_destroy(testlock);
	 panic("lt4: sem_create failed\n");
  }

	result = thread_fork("lt4", NULL, locktestacquirer, NULL, 0);
	if (result) {
		panic("lt4: thread_fork failed: %s\n", strerror(result));
	}

	P(donesem);
	secprintf(SECRET, "Should panic...", "lt4");
	lock_release(testlock);

  /* Should not get here on success. */

  success(TEST161_FAIL, SECRET, "lt4");

	/* Don't do anything that could panic. */

	testlock = NULL;
  donesem = NULL;
  return 0;
}

int
locktest5(int nargs, char **args) {
  (void) nargs;
  (void) args;

	int result;

  kprintf_n("Starting lt5...\n");
  kprintf_n("(This test panics on success!)\n");

  testlock = lock_create("testlock");
  if (testlock == NULL) {
	 panic("lt5: lock_create failed\n");
  }

  donesem = sem_create("donesem", 0);
  if (donesem == NULL) {
	 lock_destroy(testlock);
	 panic("lt5: sem_create failed\n");
  }

	result = thread_fork("lt5", NULL, locktestacquirer, NULL, 0);
	if (result) {
		panic("lt5: thread_fork failed: %s\n", strerror(result));
	}

	P(donesem);
	secprintf(SECRET, "Should panic...", "lt5");
	KASSERT(lock_do_i_hold(testlock));

  /* Should not get here on success. */

  success(TEST161_FAIL, SECRET, "lt5");

	/* Don't do anything that could panic. */

	testlock = NULL;
  donesem = NULL;
  return 0;
}

static
void
cvtestthread(void *junk, unsigned long num)
{
	(void)junk;

	int i;
	volatile int j;
	struct timespec ts1, ts2;

	for (i=0; i<NCVLOOPS; i++) {
		kprintf_t(".");
		lock_acquire(testlock);
		while (testval1 != num) {
			testval2 = 0;
			random_yielder(4);
			gettime(&ts1);
			cv_wait(testcv, testlock);
			gettime(&ts2);
			random_yielder(4);

			/* ts2 -= ts1 */
			timespec_sub(&ts2, &ts1, &ts2);

			/* Require at least 2000 cpu cycles (we're 25mhz) */
			if (ts2.tv_sec == 0 && ts2.tv_nsec < 40*2000) {
				kprintf_n("cv_wait took only %u ns\n", ts2.tv_nsec);
				kprintf_n("That's too fast... you must be busy-looping\n");
				failif(true);
				V(donesem);
				thread_exit();
			}

			testval2 = 0xFFFFFFFF;
		}
		testval2 = num;

		/*
		 * loop a little while to make sure we can measure the
		 * time waiting on the cv.
		 */
		for (j=0; j<3000; j++);

		random_yielder(4);
		cv_broadcast(testcv, testlock);
		random_yielder(4);
		failif((testval1 != testval2));

		kprintf_n("Thread %lu\n", testval2);
		testval1 = (testval1 + NTHREADS - 1) % NTHREADS;
		lock_release(testlock);
	}
	V(donesem);
}

int
cvtest(int nargs, char **args)
{
	(void)nargs;
	(void)args;

	int i, result;

	kprintf_n("Starting cvt1...\n");
	for (i=0; i<CREATELOOPS; i++) {
		kprintf_t(".");
		testlock = lock_create("testlock");
		if (testlock == NULL) {
			panic("cvt1: lock_create failed\n");
		}
		testcv = cv_create("testcv");
		if (testcv == NULL) {
			panic("cvt1: cv_create failed\n");
		}
		donesem = sem_create("donesem", 0);
		if (donesem == NULL) {
			panic("cvt1: sem_create failed\n");
		}
		if (i != CREATELOOPS - 1) {
			lock_destroy(testlock);
			cv_destroy(testcv);
			sem_destroy(donesem);
		}
	}
	spinlock_init(&status_lock);
	test_status = TEST161_SUCCESS;

	testval1 = NTHREADS-1;
	for (i=0; i<NTHREADS; i++) {
		kprintf_t(".");
		result = thread_fork("cvt1", NULL, cvtestthread, NULL, (long unsigned) i);
		if (result) {
			panic("cvt1: thread_fork failed: %s\n", strerror(result));
		}
	}
	for (i=0; i<NTHREADS; i++) {
		kprintf_t(".");
		P(donesem);
	}

	lock_destroy(testlock);
	cv_destroy(testcv);
	sem_destroy(donesem);
	testlock = NULL;
	testcv = NULL;
	donesem = NULL;

	kprintf_t("\n");
	success(test_status, SECRET, "cvt1");

	return 0;
}

////////////////////////////////////////////////////////////

/*
 * Try to find out if going to sleep is really atomic.
 *
 * What we'll do is rotate through NCVS lock/CV pairs, with one thread
 * sleeping and the other waking it up. If we miss a wakeup, the sleep
 * thread won't go around enough times.
 */

#define NCVS 250
#define NLOOPS 40
static struct cv *testcvs[NCVS];
static struct lock *testlocks[NCVS];
static struct semaphore *gatesem;
static struct semaphore *exitsem;

static
void
sleepthread(void *junk1, unsigned long junk2)
{
	(void)junk1;
	(void)junk2;

	unsigned i, j;

	random_yielder(4);

	for (j=0; j<NLOOPS; j++) {
		kprintf_t(".");
		for (i=0; i<NCVS; i++) {
			lock_acquire(testlocks[i]);
			random_yielder(4);
			V(gatesem);
			random_yielder(4);
			spinlock_acquire(&status_lock);
			testval4++;
			spinlock_release(&status_lock);
			cv_wait(testcvs[i], testlocks[i]);
			random_yielder(4);
			lock_release(testlocks[i]);
		}
		kprintf_n("sleepthread: %u\n", j);
	}
	V(exitsem);
}

static
void
wakethread(void *junk1, unsigned long junk2)
{
	(void)junk1;
	(void)junk2;

	unsigned i, j;

	random_yielder(4);

	for (j=0; j<NLOOPS; j++) {
		kprintf_t(".");
		for (i=0; i<NCVS; i++) {
			random_yielder(4);
			P(gatesem);
			random_yielder(4);
			lock_acquire(testlocks[i]);
			random_yielder(4);
			testval4--;
			failif((testval4 != 0));
			cv_signal(testcvs[i], testlocks[i]);
			random_yielder(4);
			lock_release(testlocks[i]);
		}
		kprintf_n("wakethread: %u\n", j);
	}
	V(exitsem);
}

int
cvtest2(int nargs, char **args)
{
	(void)nargs;
	(void)args;

	unsigned i;
	int result;

	kprintf_n("Starting cvt2...\n");
	for (i=0; i<CREATELOOPS; i++) {
		kprintf_t(".");
		gatesem = sem_create("gatesem", 0);
		if (gatesem == NULL) {
			panic("cvt2: sem_create failed\n");
		}
		exitsem = sem_create("exitsem", 0);
		if (exitsem == NULL) {
			panic("cvt2: sem_create failed\n");
		}
		if (i != CREATELOOPS - 1) {
			sem_destroy(gatesem);
			sem_destroy(exitsem);
		}
	}
	for (i=0; i<NCVS; i++) {
		kprintf_t(".");
		testlocks[i] = lock_create("cvtest2 lock");
		testcvs[i] = cv_create("cvtest2 cv");
	}
	spinlock_init(&status_lock);
	test_status = TEST161_SUCCESS;

	result = thread_fork("cvt2", NULL, sleepthread, NULL, 0);
	if (result) {
		panic("cvt2: thread_fork failed\n");
	}
	result = thread_fork("cvt2", NULL, wakethread, NULL, 0);
	if (result) {
		panic("cvt2: thread_fork failed\n");
	}
	P(exitsem);
	P(exitsem);

	sem_destroy(exitsem);
	sem_destroy(gatesem);
	exitsem = gatesem = NULL;
	for (i=0; i<NCVS; i++) {
		kprintf_t(".");
		lock_destroy(testlocks[i]);
		cv_destroy(testcvs[i]);
		testlocks[i] = NULL;
		testcvs[i] = NULL;
	}

	kprintf_t("\n");
	success(test_status, SECRET, "cvt2");

	return 0;
}

int
cvtest3(int nargs, char **args) {
	(void)nargs;
	(void)args;

	kprintf_n("Starting cvt3...\n");
	kprintf_n("(This test panics on success!)\n");

	testlock = lock_create("testlock");
	if (testlock == NULL) {
		panic("cvt3: lock_create failed\n");
	}
	testcv = cv_create("testcv");
	if (testcv == NULL) {
		panic("cvt3: cv_create failed\n");
	}

	secprintf(SECRET, "Should panic...", "cvt3");
	cv_wait(testcv, testlock);

	/* Should not get here on success. */

	success(TEST161_FAIL, SECRET, "cvt3");

	lock_destroy(testlock);
	cv_destroy(testcv);
	testcv = NULL;
	testlock = NULL;

	return 0;
}

int
cvtest4(int nargs, char **args) {
	(void)nargs;
	(void)args;

	kprintf_n("Starting cvt4...\n");
	kprintf_n("(This test panics on success!)\n");

	testlock = lock_create("testlock");
	if (testlock == NULL) {
		panic("cvt4: lock_create failed\n");
	}
	testcv = cv_create("testcv");
	if (testcv == NULL) {
		panic("cvt4: cv_create failed\n");
	}

	secprintf(SECRET, "Should panic...", "cvt4");
	cv_broadcast(testcv, testlock);

	/* Should not get here on success. */

	success(TEST161_FAIL, SECRET, "cvt4");

	lock_destroy(testlock);
	cv_destroy(testcv);
	testcv = NULL;
	testlock = NULL;

	return 0;
}

static
void
sleeperthread(void *junk1, unsigned long junk2) {
	(void)junk1;
	(void)junk2;

	random_yielder(4);
	lock_acquire(testlock);
	random_yielder(4);
	failif((testval1 != 0));
	testval1 = 1;
	cv_signal(testcv, testlock);

	random_yielder(4);
	cv_wait(testcv, testlock);
	failif((testval1 != 3));
	testval1 = 4;
	random_yielder(4);
	lock_release(testlock);
	random_yielder(4);

	V(exitsem);
}

static
void
wakerthread(void *junk1, unsigned long junk2) {
	(void)junk1;
	(void)junk2;

	random_yielder(4);
	lock_acquire(testlock2);
	failif((testval1 != 2));
	testval1 = 3;

	random_yielder(4);
	cv_signal(testcv, testlock2);
	random_yielder(4);
	lock_release(testlock2);
	random_yielder(4);

	V(exitsem);
}

int
cvtest5(int nargs, char **args) {
	(void)nargs;
	(void)args;

	int result;

	kprintf_n("Starting cvt5...\n");

	testlock = lock_create("testlock");
	if (testlock == NULL) {
		panic("cvt5: lock_create failed\n");
	}
	testlock2 = lock_create("testlock2");
	if (testlock == NULL) {
		panic("cvt5: lock_create failed\n");
	}
	testcv = cv_create("testcv");
	if (testcv == NULL) {
		panic("cvt5: cv_create failed\n");
	}
	exitsem = sem_create("exitsem", 0);
	if (exitsem == NULL) {
		panic("cvt5: sem_create failed\n");
	}
	spinlock_init(&status_lock);
	test_status = TEST161_SUCCESS;
	testval1 = 0;

	lock_acquire(testlock);
	lock_acquire(testlock2);

	result = thread_fork("cvt5", NULL, sleeperthread, NULL, 0);
	if (result) {
		panic("cvt5: thread_fork failed\n");
	}
	result = thread_fork("cvt5", NULL, wakerthread, NULL, 0);
	if (result) {
		panic("cvt5: thread_fork failed\n");
	}

	random_yielder(4);
	cv_wait(testcv, testlock);
	failif((testval1 != 1));
	testval1 = 2;
	random_yielder(4);
	lock_release(testlock);
	random_yielder(4);
	lock_release(testlock2);

	P(exitsem);
	P(exitsem);
	failif((testval1 != 4));

	sem_destroy(exitsem);
	cv_destroy(testcv);
	lock_destroy(testlock2);
	lock_destroy(testlock);

	success(test_status, SECRET, "cvt5");

	exitsem = NULL;
	testcv = NULL;
	testlock2 = NULL;
	testlock = NULL;
	testsem2 = NULL;
	testsem = NULL;

	return 0;
}

/*
Here starts the reader-writer locks testing.
RWT1 
Readers and writers can go randomly.
there are 26 writers writing 26 alphabets into a single buffer. the readers will go in and copy the buffer into their array[i] and we will use this later for checking with our "queue array".
in both of the threads there will be a spinlocked queue array that appends either W or R depending on the thread.
For checking each array[i] with queue array, we check if sizeof(array[i]) == no.of 'W' in the lengthof(q -> q[i]);
*/
struct rw* test;
char* buffer;
char* queue;
struct spinlock* spinner;
void
writer_thread(void *junk1, unsigned long num)
{
	(void)junk1;

	rwlock_acquire_write(test);
	kprintf_t("Writer Thread %d\n",num);
	queue[sizeof(queue)] = 'W';
	kprintf_t("queue: %s\n",queue);
	buffer[num] = 'a' + num;
	kprintf_t("buffer: %s\n",buffer);
	
	rwlock_release_write(test);
}

void
reader_thread(char* reader_buffer, unsigned long num)
{	
	rwlock_acquire_read(test);
	kprintf_t("Reader Thread %d\n",num);
	spinlock_acquire(&spinner);
	queue[sizeof(queue)] = 'R';
	kprintf_t("queue: %s\n",queue);
	spinlock_release(&spinner);
	
	reader_buffer[num] = kstrdup(buffer);
	kprintf_t("reader)buffer[%d]: %s\n",num,reader_buffer[num]);
	rwlock_release_read(test);

}

void
rwt1(int nargs, char** args)
{	
	spinlock_init(&spinner);
	test = rwlock_create("rwt1");

	buffer = kmalloc(26);
	if(buffer == NULL)
	{
		panic("Error creating buffer\n");
		return;
	}

	queue = kmalloc(26+27);
	if(queue == NULL)
	{
		panic("Error creating queue\n");
		return;
	}
	memset(queue,0,sizeof(queue));

	char* readers_buffer[27];

	test_status = TEST161_SUCCESS;

	for(int i=0;i<26;i++)
	{
		int result1 = thread_fork("writer_thread",NULL,writer_thread,NULL,i);
		if(result1)
		{
			panic("Error creating writer thread\n");
		}
		int result2 = thread_fork("reader_thread",NULL,reader_thread,readers_buffer,i);
		if(result2)
		{
			panic("Error creating reader thread\n");
		}
	}

	int result3 = thread_fork("last reader thread",NULL,reader_thread,reader_thread,26);
	if(result3)
	{
		panic("Error creating reader thread\n");
	}

	int w[26+27];
	memset(w,0,sizeof(w));
	for(int i = 0; i<53;i++)
	{
		if(queue[i]=='W') ++w[i];
		if(i>0) w[i]+=w[i-1];
	}

	for(int i = 0 ; i<53 ; i++)
	{
		failif(sizeof(readers_buffer[i]) != w[i]);
	}

	success(test_status, SECRET, "rwt1");

	rwlock_destroy(test);
	spinlock_cleanup(&spinner);
}
