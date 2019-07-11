#ifndef _SPL_H_
#define _SPL_H_

#include <cdefs.h>

/* Inlining support - for making sure an out-of-line copy gets built */
#ifndef SPL_INLINE
#define SPL_INLINE INLINE
#endif

/*
 * Machine-independent interface to interrupt enable/disable.
 *
 * "spl" stands for "set priority level", and was originally the name of
 * a VAX assembler instruction.
 *
 * The idea is that one can block less important interrupts while
 * processing them, but still allow more urgent interrupts to interrupt
 * that processing.
 *
 * Ordinarily there would be a whole bunch of defined interrupt
 * priority levels and functions for setting them - spltty(),
 * splbio(), etc., etc. But we don't support interrupt priorities in
 * OS/161, so there are only three:
 *
 *      spl0()       sets IPL to 0, enabling all interrupts.
 *      splhigh()    sets IPL to the highest value, disabling all interrupts.
 *      splx(s)      sets IPL to S, enabling whatever state S represents.
 *
 * All three return the old interrupt state. Thus, these are commonly used
 * as follows:
 *
 *      int s = splhigh();
 *      [ code ]
 *      splx(s);
 *
 * Note that these functions only affect interrupts on the current
 * processor.
 */

SPL_INLINE int spl0(void);
SPL_INLINE int splhigh(void);
int splx(int);

/*
 * Integer interrupt priority levels.
 */
#define IPL_NONE   0
#define IPL_HIGH   1

/*
 * Lower-level functions for explicitly raising and lowering
 * particular interrupt levels. These are used by splx() and by the
 * spinlock code.
 *
 * A previous setting of OLDIPL is cancelled and replaced with NEWIPL.
 *
 * For splraise, NEWIPL > OLDIPL, and for spllower, NEWIPL < OLDIPL.
 */
void splraise(int oldipl, int newipl);
void spllower(int oldipl, int newipl);

////////////////////////////////////////////////////////////

SPL_INLINE
int
spl0(void)
{
	return splx(IPL_NONE);
}

SPL_INLINE
int
splhigh(void)
{
	return splx(IPL_HIGH);
}


#endif /* _SPL_H_ */
