#include<types.h>
#include<syscall.h>
#include<thread.h>
#include<current.h>

int sys_getpid(pid_t* retval){

  *retval  = curthread->t_pid;
  return 0;

}
