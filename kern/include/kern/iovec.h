#ifndef _KERN_IOVEC_H_
#define _KERN_IOVEC_H_

/*
 * iovec structure, used in the readv/writev scatter/gather I/O calls,
 * and within the kernel for keeping track of blocks of data for I/O.
 */

struct iovec {
	/*
	 * For maximum type safety, when in the kernel, distinguish
	 * user pointers from kernel pointers.
	 *
	 * (A pointer is a user pointer if it *came* from userspace,
	 * not necessarily if it *points* to userspace. If a system
	 * call passes 0xdeadbeef, it points to the kernel, but it's
	 * still a user pointer.)
	 *
	 * In userspace, there are only user pointers; also, the name
	 * iov_base is defined by POSIX.
	 *
	 * Note that to work properly (without extra unwanted fiddling
	 * around) this scheme requires that void* and userptr_t have
	 * the same machine representation. Machines where this isn't
	 * true are theoretically possible under the C standard, but
	 * do not exist in practice.
	 */
#ifdef _KERNEL
        union {
                userptr_t  iov_ubase;	/* user-supplied pointer */
                void      *iov_kbase;	/* kernel-supplied pointer */
        };
#else
	void *iov_base;			/* user-supplied pointer */
#endif
        size_t iov_len;			/* Length of data */
};

#endif /* _KERN_IOVEC_H_ */
