/*
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

#define NOOFREADERS 27

static bool test_status = TEST161_FAIL;
struct semaphore* donetest = NULL;
struct rwlock* test;
struct spinlock spinner;
char* buffer;
char* queue;
static int count = -1;
/*
 * Use these stubs to test your reader-writer locks.
 */
/*
Here starts the reader-writer locks testing.
RWT1 
Readers and writers can go randomly.
there are 26 writers writing 26 alphabets into a single buffer. the readers will go in and copy the buffer into their array[i] and we will use this later for checking with our "queue array".
in both of the threads there will be a spinlocked queue array that appends either W or R depending on the thread.
For checking each array[i] with queue array, we check if sizeof(array[i]) == no.of 'W' in the lengthof(q -> q[i]);
*/

char* readers_buffer[27];
void writer_thread(void*, unsigned long);
void reader_thread(void*, unsigned long);
int reader_positions[NOOFREADERS];

void
writer_thread(void *junk1, unsigned long num)
{
	(void)junk1;

	rwlock_acquire_write(test);
	//kprintf_n("Writer Thread %lu\n",num);

	random_yielder(4);
	count++;
	queue[count] = 'W';
	//kprintf_n("queue: %s\n",queue);

	random_yielder(4);

	buffer[num] = 'a' + num;
	//kprintf_n("buffer: %s\n",buffer);
	
	rwlock_release_write(test);
	V(donetest);
}

void
reader_thread(void* junk1, unsigned long num)
{	
	(void)junk1;

	rwlock_acquire_read(test);
	random_yielder(4);
	//kprintf_n("Reader Thread %lu\n",num);

	spinlock_acquire(&spinner);
	count++;
	queue[count] = 'R';
	reader_positions[num] = count;
	//kprintf_n("queue: %s\n",queue);
	spinlock_release(&spinner);
	
	random_yielder(4);
	readers_buffer[num] = kstrdup(buffer);
	//kprintf_n("reader_buffer[%lu]: %s\n",num,readers_buffer[num]);
	rwlock_release_read(test);
	V(donetest);
}

int rwtest(int nargs, char **args) 
{
	(void)nargs;
	(void)args;
	spinlock_init(&spinner);

	test = rwlock_create("rwt1");
	donetest = sem_create("donetest",0);

	memset(reader_positions,0,sizeof(reader_positions));

	buffer = kmalloc(26*sizeof(char));
	if(buffer == NULL)
	{
		panic("Error creating buffer\n");
		return 1;
	}
	memset(buffer,0,sizeof(buffer));

	queue = kmalloc(26+27);
	if(queue == NULL)
	{
		panic("Error creating queue\n");
		return 1;
	}
	memset(queue,0,sizeof(queue));

	test_status = TEST161_SUCCESS;

	for(unsigned long i=0;i<26;i++)
	{
		int result1 = thread_fork("writer_thread",NULL,writer_thread,NULL,i);
		if(result1)
		{
			panic("Error creating writer thread\n");
		}
		int result2 = thread_fork("reader_thread",NULL,reader_thread,NULL,i);
		if(result2)
		{
			panic("Error creating reader thread\n");
		}
	}

	int result3 = thread_fork("last reader thread",NULL,reader_thread,NULL,26);
	if(result3)
	{
		panic("Error creating reader thread\n");
	}

	for(int j = 0; j<53; j++)
	{
		P(donetest);
	}
	unsigned int w[26+27];
	memset(w,0,sizeof(w));
	for(int i = 0; i<53;i++)
	{
		if(queue[i]=='W') ++w[i];
		if(i>0) w[i]+=w[i-1];
	}

	
	for(int i = 0 ; i<NOOFREADERS ; i++)
	{
		if(strlen(readers_buffer[i]) != w[reader_positions[i]])
		{	
			test_status = TEST161_FAIL;
			break;
		}
	}

	success(test_status, SECRET, "rwt1");

	rwlock_destroy(test);
	spinlock_cleanup(&spinner);

	return 0;
}

int rwtest2(int nargs, char **args) {
	(void)nargs;
	(void)args;

	kprintf_n("rwt2 unimplemented\n");
	success(TEST161_FAIL, SECRET, "rwt2");

	return 0;
}

int rwtest3(int nargs, char **args) {
	(void)nargs;
	(void)args;

	kprintf_n("rwt3 unimplemented\n");
	success(TEST161_FAIL, SECRET, "rwt3");

	return 0;
}

int rwtest4(int nargs, char **args) {
	(void)nargs;
	(void)args;

	kprintf_n("rwt4 unimplemented\n");
	success(TEST161_FAIL, SECRET, "rwt4");

	return 0;
}

int rwtest5(int nargs, char **args) {
	(void)nargs;
	(void)args;

	kprintf_n("rwt5 unimplemented\n");
	success(TEST161_FAIL, SECRET, "rwt5");

	return 0;
}
