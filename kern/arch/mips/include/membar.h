#ifndef _MIPS_MEMBAR_H_
#define _MIPS_MEMBAR_H_

/*
 * On the mips there's only one memory barrier instruction, so these
 * are all the same. This is not true on many other CPUs (x86, arm,
 * sparc, powerpc, etc.) We also mark the instruction as a compiler-
 * level barrier by telling gcc that it destroys memory; this prevents
 * gcc from reordering loads and stores around it.
 *
 * See include/membar.h for further information.
 */

MEMBAR_INLINE
void
membar_any_any(void)
{
	__asm volatile(
		".set push;"		/* save assembler mode */
		".set mips32;"		/* allow MIPS32 instructions */
		"sync;"			/* do it */
		".set pop"		/* restore assembler mode */
		:			/* no outputs */
		:			/* no inputs */
		: "memory");		/* "changes" memory */
}

MEMBAR_INLINE void membar_load_load(void) { membar_any_any(); }
MEMBAR_INLINE void membar_store_store(void) { membar_any_any(); }
MEMBAR_INLINE void membar_store_any(void) { membar_any_any(); }
MEMBAR_INLINE void membar_any_store(void) { membar_any_any(); }


#endif /* _MIPS_MEMBAR_H_ */
