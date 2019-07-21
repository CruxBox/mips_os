/*
 * Driver code is in kern/tests/synchprobs.c We will replace that file. This
 * file is yours to modify as you see fit.
 *
 * You should implement your solution to the stoplight problem below. The
 * quadrant and direction mappings for reference: (although the problem is, of
 * course, stable under rotation)
 *
 *   |0 |
 * -     --
 *    01  1
 * 3  32
 * --    --
 *   | 2|
 *
 * As way to think about it, assuming cars drive on the right: a car entering
 * the intersection from direction X will enter intersection quadrant X first.
 * The semantics of the problem are that once a car enters any quadrant it has
 * to be somewhere in the intersection until it call leaveIntersection(),
 * which it should call while in the final quadrant.
 *
 * As an example, let's say a car approaches the intersection and needs to
 * pass through quadrants 0, 3 and 2. Once you call inQuadrant(0), the car is
 * considered in quadrant 0 until you call inQuadrant(3). After you call
 * inQuadrant(2), the car is considered in quadrant 2 until you call
 * leaveIntersection().
 *
 * You will probably want to write some helper functions to assist with the
 * mappings. Modular arithmetic can help, e.g. a car passing straight through
 * the intersection entering from direction X will leave to direction (X + 2)
 * % 4 and pass through quadrants X and (X + 3) % 4.  Boo-yah.
 *
 * Your solutions below should call the inQuadrant() and leaveIntersection()
 * functions in synchprobs.c to record their progress.
 */

#include <types.h>
#include <lib.h>
#include <thread.h>
#include <test.h>
#include <synch.h>

/*
 * Called by the driver during initialization.
 */

struct lock *global;
struct lock *quad[4];

void stoplight_init()
{
	//1 global lock
	//4 locks
	global = lock_create("global");

	for (int i = 0; i < 4; i++)
	{
		quad[i] = lock_create("quad" + (char)i);
	}
	return;
}

/*
 * Called by the driver during teardown.
 */

void stoplight_cleanup()
{
	lock_destroy(global);

	for (int i = 0; i < 4; i++)
		lock_destroy(quad[i]);

	return;
}

void turnright(uint32_t direction, uint32_t index)
{
	lock_acquire(global);

	lock_acquire(quad[direction]);

	lock_release(global);

	inQuadrant(direction, index);
	leaveIntersection(index);

	lock_release(quad[direction]);

	return;
}

void gostraight(uint32_t direction, uint32_t index)
{
	lock_acquire(global);

	for (uint32_t i = direction; i >= (direction - 2); i--)
	{
		lock_acquire(quad[i % 4]);
	}
	lock_release(global);

	for (uint32_t i = 0; i < 3; i++)
	{
		inQuadrant((direction - i) % 4, index);
	}
	leaveIntersection(index);

	for (uint32_t i = direction; i >= (direction - 2); i--)
	{
		lock_release(quad[i % 4]);
	}

	return;
}
void turnleft(uint32_t direction, uint32_t index)
{
	lock_acquire(global);

	for (uint32_t i = direction; i >= (direction - 3); i--)
	{
		lock_acquire(quad[i % 4]);
	}
	lock_release(global);

	for (uint32_t i = 0; i < 4; i++)
	{
		inQuadrant((direction - i) % 4, index);
	}
	leaveIntersection(index);

	for (uint32_t i = direction; i >= (direction - 3); i--)
	{
		lock_release(quad[i % 4]);
	}

	return;
}
