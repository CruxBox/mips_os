#ifndef __FILE_TABLE__
#define __FILE_TABLE__

#include <kern/fcntl.h>
#include <kern/unistd.h>
#include <spinlock.h>
#include <synch.h>
#include <file_handle.h>

#define MAXHANDLES 128

struct file_table{
	struct spinlock* spin_table;
	int nextfd;
	struct file_handle_node* handles[MAXHANDLES];
};

int file_table_init(struct file_table** table);

int file_table_destroy(struct file_table* table);

int get_fd(struct file_table* table, int* fd);

int next_fd_calculate(int current);

int file_table_assign_std(struct file_table* table);

#endif