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
sys_open(const char* file_path, int flags, int* retval)
{

	struct vnode* vn;

	if(file_path == NULL){
		return EFAULT;
	}

	char *filename = NULL;
	filename = kmalloc(strlen(file_path)+1);
	if(filename == NULL){
		return ENOMEM;
	}

	int result = copyinstr((const_userptr_t)file_path, filename, strlen(file_path)+1, NULL);
	if(result){
		*retval = -1;
		return result;
	}
	
	result = vfs_open(filename, flags, 0, &vn);
	if(result){
		*retval = -1;
		return result;
	}

	int fd = get_fd(curproc->table);
	if(fd == EMFILE){
		kfree(filename);
		return 
	}

	result = file_handle_create(&(curproc->table[fd]), vn, 0, flags);

	if(result){
		*retval = -1;
		return result;
	}

	return 0;
}