#include<file_table.h>

int next_fd_calculate(int current){
  if(current == (MAXHANDLES-1)){
    return 3;
  }
  else return current+1;
}

int file_table_assign_std(struct file_table* table){

  KASSERT(table != NULL);

  int result;

  result = file_handle_assign_std(table, 0);
  
  if(result){
    return result;
  }

  result = file_handle_assign_std(table, 0);
  if(result){
    return result;
  }
  
  result = file_handle_assign_std(table, 0);
  if(result){
    return result;
  }
return 0;  

}

int get_fd(struct file_table* table){
  
  KASSERT(table != NULL);

  int result;
  
  spinlock_acquire(table->spin_table);

  int ret = -1;

  if(table->handles[table->nextfd]==NULL){

    ret = t->table->nextfd;
    
    table->nextfd = next_fd_calculate(table->nextfd);

    spinlock_release(t->spin_table);
    
    return ret;
  }

  for(int i=3;i<MAXHANDLES;i++){
    
    if(t->table[i].file_object != NULL) {
      continue;
    }

    t->table->nextfd = next_fd_calculate(i);
    ret = i;
    break;
	}

  spinlock_release(t->spin_table);

	return ret;
}


int file_table_init(struct file_table** table){

  *table = (struct file_table*)kmalloc(sizeof(struct file_table));
  
  if((*table)== NULL){
    return ENOMEM; 
  }

  spinlock_init((*table)->spin_table);

  int i;
  for(i = 0;i<MAXHANDLES;i++){
    (*table)->handles[i]=NULL;
  }

  (*table)->nextfd = 3;

  return 0;
}


int file_table_destroy(struct file_table* table){

  for(int i = 0; i < MAXHANDLES; i++){
    file_handle_destroy(table->handles[i]);
    table->handles[i]=NULL;
  }

  spinlock_cleanup(table->spin_table);

  kfree(table);
}

