#ifndef _TEST_H_
#define _TEST_H_

/* Get __PF() for declaring printf-like functions. */
#include <cdefs.h>
#include <kern/secret.h>

#include "opt-synchprobs.h"
#include "opt-automationtest.h"

/*
 * Declarations for test code and other miscellaneous high-level
 * functions.
 */

/*
 * Test code.
 */

/* data structure tests */
int arraytest(int, char **);
int arraytest2(int, char **);
int bitmaptest(int, char **);
int threadlisttest(int, char **);

/* thread tests */
int threadtest(int, char **);
int threadtest2(int, char **);
int threadtest3(int, char **);
int semtest(int, char **);
int locktest(int, char **);
int locktest2(int, char **);
int locktest3(int, char **);
int locktest4(int, char **);
int locktest5(int, char **);
int cvtest(int, char **);
int cvtest2(int, char **);
int cvtest3(int, char **);
int cvtest4(int, char **);
int cvtest5(int, char **);
int rwtest(int, char **);
int rwtest2(int, char **);
int rwtest3(int, char **);
int rwtest4(int, char **);
int rwtest5(int, char **);

/* semaphore unit tests */
int semu1(int, char **);
int semu2(int, char **);
int semu3(int, char **);
int semu4(int, char **);
int semu5(int, char **);
int semu6(int, char **);
int semu7(int, char **);
int semu8(int, char **);
int semu9(int, char **);
int semu10(int, char **);
int semu11(int, char **);
int semu12(int, char **);
int semu13(int, char **);
int semu14(int, char **);
int semu15(int, char **);
int semu16(int, char **);
int semu17(int, char **);
int semu18(int, char **);
int semu19(int, char **);
int semu20(int, char **);
int semu21(int, char **);
int semu22(int, char **);

/* filesystem tests */
int fstest(int, char **);
int readstress(int, char **);
int writestress(int, char **);
int writestress2(int, char **);
int longstress(int, char **);
int createstress(int, char **);
int printfile(int, char **);

/* HMAC/hash tests */
int hmacu1(int, char **);

/* other tests */
int kmalloctest(int, char **);
int kmallocstress(int, char **);
int kmalloctest3(int, char **);
int kmalloctest4(int, char **);
int kmalloctest5(int, char **);
int nettest(int, char **);

/* Routine for running a user-level program. */
int runprogram(char *progname);

/* Kernel menu system. */
void menu(char *argstr);

/* The main function, called from start.S. */
void kmain(char *bootstring);

/*
 * Synchronization driver primitives.
 */

#if OPT_SYNCHPROBS

void male_start(uint32_t);
void male_end(uint32_t);
void female_start(uint32_t);
void female_end(uint32_t);
void matchmaker_start(uint32_t);
void matchmaker_end(uint32_t);
int whalemating(int, char **);

void inQuadrant(int, uint32_t);
void leaveIntersection(uint32_t);
int stoplight(int, char **);

/*
 * Synchronization problem primitives.
 */

/*
 * whalemating.c.
 */

void whalemating_init(void);
void whalemating_cleanup(void);
void male(uint32_t);
void female(uint32_t);
void matchmaker(uint32_t);

/*
 * stoplight.c.
 */

void gostraight(uint32_t, uint32_t);
void turnleft(uint32_t, uint32_t);
void turnright(uint32_t, uint32_t);
void stoplight_init(void);
void stoplight_cleanup(void);
#endif

/*
 * Automation tests for detecting kernel deadlocks and livelocks.
 */

#if OPT_AUTOMATIONTEST
int dltest(int, char **);
int ll1test(int, char **);
int ll16test(int, char **);
#endif

void random_yielder(uint32_t);
void random_spinner(uint32_t);

/*
 * kprintf variants that do not (or only) print during automated testing.
 */

#ifdef SECRET_TESTING
#define kprintf_t(...) kprintf(__VA_ARGS__)
#define kprintf_n(...) silent(__VA_ARGS__)
#else
#define kprintf_t(...) silent(__VA_ARGS__)
#define kprintf_n(...) kprintf(__VA_ARGS__)
#endif

static inline void silent(const char *fmt, ...)
{
    (void)fmt;
};

#endif /* _TEST_H_ */
