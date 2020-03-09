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

	//get current directory for path (vfs_getcwd)
	//add file name to the path
	//open the file vfs_open and use flags
	//return available fd
	if(filename == NULL){
		return EFAULT;
	}

	char* path;

	path = kstrdup(filename);
	// struct iovec iov;
	// struct uio myuio;

	// uio_kinit(&iov, &myuio, path, sizeof(path), 0, UIO_READ);

	// int result = vfs_getcwd(&myuio);
	// if(result){
	// 	return result;
	// }

	// /* check for a free slot in file table */

	
	// int file_path_length = strlen(path) + strlen(filename) + 1 /* for / */ + 1 ;
	// char file_path[file_path_length];

	// //concat path and filename to give file_path	
	// int index=0;
	// {
	// 	for(int i = 0; i < strlen(path) ; i++, index++){
	// 		file_path[index] = path[i];
	// 	}

	// 	file_path[index++] = '/';

	// 	for(int i = 0 ; i < strlen(filename) ; i++, index++){
	// 		file_path[index] = filename[i];
	// 	}

	// 	file_path[index++] = '/0';

	// 	KASSERT(index == file_path_length); 
	// }

	struct vnode* file;

	int result = vfs_open(path, flags, 0, &file);
	if(result){
		return result;
	}

	//find fd;

	int fd = get_fd(curthread);

	if(fd == -1){
		// *retval = -1;
		return -1;
	}
	//open console files
	// struct rwlock* rw = (curthread->file_table[fd].lock);

	// switch(flags){
	// 	case O_RDONLY:
	// 		rwlock_acquire_read(rw);
	// 		break;
	// 	case O_WRONLY:
	// 		rwlock_acquire_write(rw);
	// 		break;
	// }

	*retval = fd;
	return 0;
}