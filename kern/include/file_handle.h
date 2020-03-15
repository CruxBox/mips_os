#ifndef __FILE_HANDLE__
#define __FILE_HANDLE__

#include <kern/fcntl.h>
#include <kern/unistd.h>
#include <spinlock.h>
#include <synch.h>
#include <vnode.h>
#include <file_table.h>

struct file_table;

struct file_handle_node{
  
	struct rwlock *lock;
  	int offset;
    int flags;
    struct spinlock* fd_lock;
  	struct vnode* file_object;
	int ref_count;
};

int file_handle_assign_std(struct file_table*,int fdnum);
int file_handle_create(struct file_handle_node**, struct vnode* node, int offset, int flags);
int file_handle_destroy(struct file_handle_node*);

#endif