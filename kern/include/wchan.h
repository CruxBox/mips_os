#ifndef _WCHAN_H_
#define _WCHAN_H_

/*
 * Wait channel.
 */


struct spinlock; /* in spinlock.h */
struct wchan; /* Opaque */

/*
 * Create a wait channel. Use NAME as a symbolic name for the channel.
 * NAME should be a string constant; if not, the caller is responsible
 * for freeing it after the wchan is destroyed.
 */
struct wchan *wchan_create(const char *name);

/*
 * Destroy a wait channel. Must be empty and unlocked.
 */
void wchan_destroy(struct wchan *wc);

/*
 * Return nonzero if there are no threads sleeping on the channel.
 * This is meant to be used only for diagnostic purposes.
 */
bool wchan_isempty(struct wchan *wc, struct spinlock *lk);

/*
 * Go to sleep on a wait channel. The current thread is suspended
 * until awakened by someone else, at which point this function
 * returns.
 *
 * The associated lock must be locked. It will be unlocked while
 * sleeping, and relocked upon return.
 */
void wchan_sleep(struct wchan *wc, struct spinlock *lk);

/*
 * Wake up one thread, or all threads, sleeping on a wait channel.
 * The associated spinlock should be locked.
 *
 * The current implementation is FIFO but this is not promised by the
 * interface.
 */
void wchan_wakeone(struct wchan *wc, struct spinlock *lk);
void wchan_wakeall(struct wchan *wc, struct spinlock *lk);


#endif /* _WCHAN_H_ */
