#include <kern/unistd.h>
#include <syscall.h>
#include <lib.h>
#include <types.h>

int sys_write(int fd, const void* buf, size_t bufflen){
	(void)fd;

	kprintf(buf);

	return bufflen;
}
