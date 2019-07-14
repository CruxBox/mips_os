#ifndef _CLOCK_H_
#define _CLOCK_H_

/*
 * Time-related definitions.
 */

#include <kern/time.h>


/*
 * hardclock() is called on every CPU HZ times a second, possibly only
 * when the CPU is not idle, for scheduling.
 */

/* hardclocks per second */
#define HZ  100

void hardclock_bootstrap(void);
void hardclock(void);

/*
 * timerclock() is called on one CPU once a second to allow simple
 * timed operations. (This is a fairly simpleminded interface.)
 */
void timerclock(void);

/*
 * gettime() may be used to fetch the current time of day.
 */
void gettime(struct timespec *ret);

/*
 * arithmetic on times
 *
 * add: ret = t1 + t2
 * sub: ret = t1 - t2
 */

void timespec_add(const struct timespec *t1,
		  const struct timespec *t2,
		  struct timespec *ret);
void timespec_sub(const struct timespec *t1,
		  const struct timespec *t2,
		  struct timespec *ret);

/*
 * clocksleep() suspends execution for the requested number of seconds,
 * like userlevel sleep(3). (Don't confuse it with wchan_sleep.)
 */
void clocksleep(int seconds);


#endif /* _CLOCK_H_ */
