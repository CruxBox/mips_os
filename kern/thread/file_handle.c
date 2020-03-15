
#include <types.h>
#include <lib.h>
#include <kern/errno.h>
#include <vfs.h>
#include <file_handle.h>


int file_handle_assign_std(struct file_table* table, int fdnum){
  char console_path[5] = {'c','o','n',':','\n'};

  struct vnode* file;
  int flags,mode;

  switch(fdnum){
    case 0:
      flags = O_RDONLY;
      mode = 0444;
      break;
    case 1:
    case 2:
      flags = O_WRONLY;
      mode = 0222;
      break;
    default:
        return EINVAL;
  }

  int result = vfs_open(console_path, flags, mode, &file);
  if(result){
    return result;
  }

  result = file_handle_create(&(table->handles[fdnum]), file, 0, flags);
  if(result){
    return result;
  }

  return 0;
}

int file_handle_create(struct file_handle_node** ptr, struct vnode* node, int offset, int flags){

	*ptr = (struct file_handle_node*)kmalloc(sizeof(struct file_handle_node));

	if(*ptr == NULL){
		return ENOMEM;
	}
	spinlock_init((*ptr)->fd_lock);
	
	(*ptr)->lock = rwlock_create("file_handle_lock");
	
	if((*ptr)->lock == NULL){
		return ENOMEM;
	}

	(*ptr)->offset = offset;	
	
	(*ptr)->flags = flags;

	(*ptr)->file_object = node;
	VOP_INCREF(node);
	(*ptr)->ref_count++;

	return 0;
}

int file_handle_destroy(struct file_handle_node* ptr){
	
	KASSERT(ptr->ref_count == 0);

	spinlock_cleanup(ptr->fd_lock);
	rwlock_destroy(ptr->lock);

	ptr->fd_lock = NULL;
	ptr->lock = NULL;
	ptr->file_object = NULL;

	kfree(ptr);

	return 0;
}
