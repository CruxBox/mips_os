#ifndef _PID_H_
#define _PID_H_

#define INVALID_ID 0
#define BOOTPROC_ID 1

struct pid
{
    pid_t pid;   //holds the process id
    pid_t p_pid; //holds the parent process id, default is INVALID_ID
};

void pid_create(pid_t p_pid, int index);

void pid_bootstrap(void);

int pid_alloc(pid_t p_pid);

void pid_dealloc(pid_t pid);

#endif
