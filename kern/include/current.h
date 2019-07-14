#ifndef _CURRENT_H_
#define _CURRENT_H_

/*
 * Definition of curcpu and curthread.
 *
 * The machine-dependent header should define either curcpu or curthread
 * as a macro (but not both); then we use one to get the other, and include
 * the header file needed to make that reference. (These includes are why
 * this file isn't rolled into either cpu.h or thread.h.)
 *
 * This material is machine-dependent because on some platforms it is
 * better/easier to keep track of curcpu and make curthread be
 * curcpu->c_curthread, and on others to keep track of curthread and
 * make curcpu be curthread->t_cpu.
 *
 * Either way we don't want retrieving curthread or curcpu to be
 * expensive; digging around in system board registers and whatnot is
 * not a very good idea. So we want to keep either curthread or curcpu
 * on-chip somewhere in some fashion.
 *
 * There are various possible approaches; for example, one might use
 * the MMU on each CPU to map that CPU's cpu structure to a fixed
 * virtual address that's the same on all CPUs. Then curcpu can be a
 * constant. (But one has to remember to use curcpu->c_self as the
 * canonical form of the pointer anywhere that's visible to other
 * CPUs.) On some CPUs the CPU number or cpu structure base address
 * can be stored in a supervisor-mode register, where it can be set up
 * during boot and then left alone. An alternative approach is to
 * reserve a register to hold curthread, and update it during context
 * switch.
 *
 * See each platform's machine/current.h for a discussion of what it
 * does and why.
 */

#include <machine/current.h>

#if defined(__NEED_CURTHREAD)

#include <cpu.h>
#define curthread curcpu->c_curthread
#define CURCPU_EXISTS() (curcpu != NULL)

#endif

#if defined(__NEED_CURCPU)

#include <thread.h>
#define curcpu curthread->t_cpu
#define CURCPU_EXISTS() (curthread != NULL)

#endif

/*
 * Definition of curproc.
 *
 * curproc is always the current thread's process.
 */

#define curproc (curthread->t_proc)


#endif /* _CURRENT_H_ */
