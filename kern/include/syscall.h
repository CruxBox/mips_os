#ifndef _SYSCALL_H_
#define _SYSCALL_H_

#include <types.h>
#include <cdefs.h> /* for __DEAD */
struct trapframe; /* from <machine/trapframe.h> */

/*
 * The system call dispatcher.
 */

void syscall(struct trapframe *tf);

/*
 * Support functions.
 */

/* Helper for fork(). You write this. */
void enter_forked_process(struct trapframe *tf);

/* Enter user mode. Does not return. */
__DEAD void enter_new_process(int argc, userptr_t argv, userptr_t env,
		       vaddr_t stackptr, vaddr_t entrypoint);


/*
 * Prototypes for IN-KERNEL entry points for system call implementations.
 */

int sys_reboot(int code);
int sys___time(userptr_t user_seconds, userptr_t user_nanoseconds);

/*
* process calls
*/
int sys_getpid(pid_t*);

/*
* file calls
*/
int sys_write(int fd, const void *buf, size_t buflen);
int sys_open(const char* filename, int flags, int* retval);
#endif /* _SYSCALL_H_ */
