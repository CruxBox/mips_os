#ifndef __FILE_HANDLE__
#define __FILE_HANDLE__


#include <thread.h>
#define MAXHANDLES 128

struct file_handle_node{
  
	struct rwlock *lock;
  	int offset;
    int flags;
    struct spinlock* fd_lock;
  	struct vnode* file_object;
	int ref_count;
};

int file_handle_assign_std(struct file_table* table,int fdnum);
int file_handle_create(struct file_handle_node** ptr, struct vnode* node, int offset, int flags);
int file_handle_destroy(struct file_handle_node* ptr);

#endif