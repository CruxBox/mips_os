
/* Make sure to build out-of-line versions of spl inline functions */
#define SPL_INLINE	/* empty */

#include <types.h>
#include <lib.h>
#include <cpu.h>
#include <spl.h>
#include <thread.h>
#include <current.h>

/*
 * Machine-independent interrupt handling functions.
 *
 * Traditionally, all this code is machine-dependent.
 *
 * However.
 *
 * Since on OS/161 we don't support interrupt levels on any platform,
 * all we require under this logic is cpu_irqoff() and cpu_irqon()
 * that explicitly turn interrupts off and on.
 *
 * If we had multiple interrupt levels, the number of levels would in
 * general be different on different platforms (depending on hardware
 * requirements and hardware capabilities) so things would get more
 * complicated -- but nearly all of this code could remain MI.
 */


/*
 * Raise and lower the interrupt priority level.
 *
 * Each spinlock acquisition can raise and lower the priority level
 * independently. The spl calls also raise and lower the priority
 * level independently of the spinlocks. This is necessary because in
 * general spinlock acquisitions and releases don't nest perfectly,
 * and don't necessarily nest with respect to spl calls either.
 *
 * For example:
 *
 *    struct spinlock red, blue;
 *    int s;
 *
 *    spinlock_acquire(&red);
 *    s = splhigh();
 *    spinlock_acquire(&blue);
 *    splx(s);
 *    spinlock_release(&red);
 *    spinlock_release(&blue);
 *
 * In order to make this work we need to count the number of times
 * IPL_HIGH (or, if we had multiple interrupt priority levels, each
 * level independently) has been raised. Interrupts go off on the
 * first raise, and go on again only on the last lower.
 *
 * curthread->t_iplhigh_count is used to track this.
 */
void
splraise(int oldspl, int newspl)
{
	struct thread *cur = curthread;

	/* only one priority level, only one valid args configuration */
	KASSERT(oldspl == IPL_NONE);
	KASSERT(newspl == IPL_HIGH);

	if (!CURCPU_EXISTS()) {
		/* before curcpu initialization; interrupts are off anyway */
		return;
	}

	if (cur->t_iplhigh_count == 0) {
		cpu_irqoff();
	}
	cur->t_iplhigh_count++;
}

void
spllower(int oldspl, int newspl)
{
	struct thread *cur = curthread;

	/* only one priority level, only one valid args configuration */
	KASSERT(oldspl == IPL_HIGH);
	KASSERT(newspl == IPL_NONE);

	if (!CURCPU_EXISTS()) {
		/* before curcpu initialization; interrupts are off anyway */
		return;
	}

	cur->t_iplhigh_count--;
	if (cur->t_iplhigh_count == 0) {
		cpu_irqon();
	}
}


/*
 * Disable or enable interrupts and adjust curspl setting. Return old
 * spl level.
 */
int
splx(int spl)
{
	struct thread *cur = curthread;
	int ret;

	if (!CURCPU_EXISTS()) {
		/* before curcpu initialization; interrupts are off anyway */
		return spl;
	}

	if (cur->t_curspl < spl) {
		/* turning interrupts off */
		splraise(cur->t_curspl, spl);
		ret = cur->t_curspl;
		cur->t_curspl = spl;
	}
	else if (cur->t_curspl > spl) {
		/* turning interrupts on */
		ret = cur->t_curspl;
		cur->t_curspl = spl;
		spllower(ret, spl);
	}
	else {
		/* do nothing */
		ret = spl;
	}

	return ret;
}
