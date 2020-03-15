#include <kern/unistd.h>
#include <types.h>
#include <syscall.h>
#include <kern/fcntl.h>
#include <uio.h>
#include <vfs.h>
#include <thread.h>
#include <current.h>
#include <synch.h>
#include <kern/errno.h>

int
sys_open(const char* filename, int flags, int* retval)
{

	struct vnode* vn;

	int result = vfs_open(filename, flags, 0, &vn);
	if(result){
		*retval = -1;
		return result;
	}

	int fd = get_fd(curthread);
	lock_acquire(curthread->file_table[fd].lock);
	curthread->file_table[fd].file_object = vn;
	curthread->file_table[fd].refcount = 1;
	curthread->file_table[fd].seeker = 
	return 0;
}