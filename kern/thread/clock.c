#include <types.h>
#include <lib.h>
#include <cpu.h>
#include <wchan.h>
#include <clock.h>
#include <thread.h>
#include <current.h>

/*
 * Time handling.
 *
 * This is pretty primitive. A real kernel will typically have some
 * kind of support for scheduling callbacks to happen at specific
 * points in the future, usually with more resolution than one second.
 *
 * A real kernel also has to maintain the time of day; in OS/161 we
 * skimp on that because we have a known-good hardware clock.
 */

/*
 * Timing constants. These should be tuned along with any work done on
 * the scheduler.
 */
#define SCHEDULE_HARDCLOCKS	4	/* Reschedule every 4 hardclocks. */
#define MIGRATE_HARDCLOCKS	16	/* Migrate every 16 hardclocks. */

/*
 * Once a second, everything waiting on lbolt is awakened by CPU 0.
 */
static struct wchan *lbolt;
static struct spinlock lbolt_lock;

/*
 * Setup.
 */
void
hardclock_bootstrap(void)
{
	spinlock_init(&lbolt_lock);
	lbolt = wchan_create("lbolt");
	if (lbolt == NULL) {
		panic("Couldn't create lbolt\n");
	}
}

/*
 * This is called once per second, on one processor, by the timer
 * code.
 */
void
timerclock(void)
{
	/* Just broadcast on lbolt */
	spinlock_acquire(&lbolt_lock);
	wchan_wakeall(lbolt, &lbolt_lock);
	spinlock_release(&lbolt_lock);
}

/*
 * This is called HZ times a second (on each processor) by the timer
 * code.
 */
void
hardclock(void)
{
	/*
	 * Collect statistics here as desired.
	 */

	curcpu->c_hardclocks++;
	if ((curcpu->c_hardclocks % MIGRATE_HARDCLOCKS) == 0) {
		thread_consider_migration();
	}
	if ((curcpu->c_hardclocks % SCHEDULE_HARDCLOCKS) == 0) {
		schedule();
	}
	thread_yield();
}

/*
 * Suspend execution for n seconds.
 */
void
clocksleep(int num_secs)
{
	spinlock_acquire(&lbolt_lock);
	while (num_secs > 0) {
		wchan_sleep(lbolt, &lbolt_lock);
		num_secs--;
	}
	spinlock_release(&lbolt_lock);
}
